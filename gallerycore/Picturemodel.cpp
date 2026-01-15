#include "Picturemodel.h"
#include"Databasemanager.h"
#include <memory>
#include <vector>
#include <QFile>
#include "AlbumModel.h"
#include "qsqlerror.h"
#include "qsqlquery.h"
PictureModel::PictureModel (const AlbumModel& albumModel, QObject* parent) :
    QAbstractListModel(parent),
    mDb(DatabaseManager::instance()),
    mAlbumId(-1),
    mPictures(std::make_unique<std::vector<std::unique_ptr<Picture>>>())
{
    connect(&albumModel, &AlbumModel::rowsRemoved,
            this, &PictureModel::deletePicturesForAlbum);

}

void PictureModel::setAlbumId(int albumId)
{
    qDebug() << "PictureModel::setAlbumId:" << albumId;


    beginResetModel();
    mAlbumId = albumId;
    loadPictures(mAlbumId);
    endResetModel();
}

void PictureModel::loadPictures(int albumId)
{
    qDebug() << "=== PictureModel::loadPictures ===";
    qDebug() << "albumId:" << albumId;

    if (albumId <= 0) {
        mPictures = std::make_unique<std::vector<std::unique_ptr<Picture>>>();
        qDebug() << "albumId inválido, limpiando pictures";
        return;
    }

    QVector<Picture*> picturesFromDao = mDb.pictureDao.picturesForAlbum(albumId);
    qDebug() << "Pictures obtenidas del DAO:" << picturesFromDao.count();

    auto newPictures = std::make_unique<std::vector<std::unique_ptr<Picture>>>();

    for (Picture* p : picturesFromDao) {
        newPictures->push_back(std::unique_ptr<Picture>(p));
    }

    mPictures = std::move(newPictures);
    qDebug() << "Total pictures cargadas:" << mPictures->size();

}

QModelIndex PictureModel::addPicture(const Picture& picture)
{
    if (mAlbumId <= 0)
        return QModelIndex();

    Picture pic = picture;
    pic.setAlbumId(mAlbumId);

    //  GUARDAR EN BASE DE DATOS
    mDb.pictureDao.addPictureInAlbum(mAlbumId, pic);

    int newRow = rowCount();
    beginInsertRows(QModelIndex(), newRow, newRow);
    mPictures->push_back(std::make_unique<Picture>(pic));
    endInsertRows();

    return index(newRow, 0);
}

/*QModelIndex PictureModel::addPicture(const Picture& picture)
{
    int newRow = rowCount(); // índice de la nueva fila
    beginInsertRows(QModelIndex(), newRow, newRow);

    mPictures->push_back(std::make_unique<Picture>(picture)); // añade la copia
    endInsertRows();

    // Devuelve el QModelIndex de la nueva fila
    return index(newRow, 0);
}*/

void PictureModel::deletePicturesForAlbum()
{

    beginResetModel();
    mPictures->clear();  // si mPictures es QVector<Picture*>
    endResetModel();
}

int PictureModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return static_cast<int>(mPictures->size());
}

QVariant PictureModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= rowCount())
        return QVariant();

    const Picture* pic = mPictures->at(index.row()).get();

    switch (role) {
    case Qt::DisplayRole:
    case FilePathRole:
        return pic->fileUrl();
    default:
        return QVariant();
    }
}

bool PictureModel::removeRows(int row, int count, const QModelIndex& parent)
{
    Q_UNUSED(parent)
    if (row < 0 || row >= rowCount() || count <= 0 || row + count > rowCount())
        return false;

    beginRemoveRows(QModelIndex(), row, row + count - 1);

    mPictures->erase(mPictures->begin() + row,
                     mPictures->begin() + row + count);

    endRemoveRows();
    return true;
}

void PictureModel::removePicture(int row)
{
    if (row < 0 || row >= rowCount()) return;

    Picture* p = mPictures->at(row).get();
    if (p) {
        //  Borrar archivo del disco
        QFile file(p->fileUrl().toLocalFile());
        if (file.exists()) {
            file.remove();
        }

        // Borrar de la base de datos usando PictureDao
        mDb.pictureDao.removePicture(p->id());
    }

    //  Borrar del modelo (lista en memoria)
    beginRemoveRows(QModelIndex(), row, row);
    mPictures->erase(mPictures->begin() + row);
    endRemoveRows();
}

void PictureDao::addPictureInAlbum(int albumId, Picture& picture) const
{
    QSqlQuery query(mDatabase);
    query.prepare(
        "INSERT INTO pictures (album_id, url) "
        "VALUES (:albumId, :url)"
        );
    query.bindValue(":albumId", albumId);
    query.bindValue(":url", picture.fileUrl().toString());

    if (!query.exec()) {
        qDebug() << "Error insertando picture:" << query.lastError();
        return;
    }

    picture.setId(query.lastInsertId().toInt());
}



/*
 *Faltaban incluir albummodel.h memory y vector
*/


/*
 * Constructor de PictureModel antes
 *
 * PictureModel::PictureModel(const AlbumModel& albumModel, QObject* parent) :
 *   QAbstractListModel(parent),
 *   mDb(DatabaseManager::instance()),
 *  mAlbumId(-1),
 *   mPictures(new vector<unique_ptr<Picture>>())
 *   {
 *   connect(&albumModel, &AlbumModel::rowsRemoved,
 *           this, &PictureModel::deletePicturesForAlbum);
 *   }
 *
 *
 * mPicture se inicializa con std::make_unique
 * Se uso lambda en rowsRemoved para problemas de señal
 * Se uso QAbstractitemModel::rowsRemoved en lugar de AlbumModel::rowsRemoved
 */



/*
 *  Metodo loadPictures antes
 *
 *  void PictureModel::loadPictures(int albumId)
 *   {
 *   if (albumId <= 0) {
 *       mPictures(new vector<unique_ptr<Picture>>());
 *       return;
 *   }
 *   mPictures = mDb.pictureDao.picturesForAlbum(albumId);
 *   }
 *
 *
 *   Se reemplaza mPictures(new vector<unique_ptr<Picture>>()) por std::make_unique
 *   Se transforma el QVector<Picture*> devuelto por el DAO a std::vector<std::unique_ptr<Picture>>
 *   para respetar el tipo de mPictures
 *   Se usa std::move para asignar el nuevo unique_ptr a mPictures
 */


/*
 * Los errores eran que habia unique_ptr que no estabam declarados
 * señales que daban error de AlbumModel
 * PictureDao no estaba
 */
