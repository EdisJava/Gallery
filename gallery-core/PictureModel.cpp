#include "PictureModel.h"
#include "AlbumModel.h"
#include "DatabaseManager.h"
#include "PictureDao.h"

PictureModel::PictureModel(const AlbumModel& albumModel, QObject* parent)
    : QAbstractListModel(parent),
    mDb(DatabaseManager::instance()),
    mAlbumId(-1),
    mPictures(new std::vector<std::unique_ptr<Picture>>())
{
    connect(&albumModel, &AlbumModel::rowsRemoved,
            this, &PictureModel::deletePicturesForAlbum);
}

#include "PictureModel.h"
#include "DatabaseManager.h"

void PictureModel::addPicture(const Picture &picture)
{
    // Agrega la imagen en la base de datos
    mDb.pictureDao->addPictureInAlbum(picture.albumId(), const_cast<Picture&>(picture));

    // Recarga las imágenes del álbum para actualizar el modelo
    loadPictures(picture.albumId());
}


int PictureModel::rowCount(const QModelIndex& parent) const {
    Q_UNUSED(parent);
    return static_cast<int>(mPictures->size());
}

QVariant PictureModel::data(const QModelIndex& index, int role) const {
    if (!isIndexValid(index)) return QVariant();
    const Picture& picture = *mPictures->at(index.row());
    if (role == FilePathRole || role == Qt::DisplayRole)
        return picture.fileUrl().toString();
    return QVariant();
    if (role == Qt::UserRole)
        return QVariant::fromValue(const_cast<Picture*>(&picture));

}

bool PictureModel::removeRows(int row, int count, const QModelIndex& parent) {
    Q_UNUSED(parent);
    if (row < 0 || (row + count) > rowCount()) return false;

    beginRemoveRows(QModelIndex(), row, row + count - 1);
    while (count--) {
        const Picture& picture = *mPictures->at(row);
        mDb.pictureDao->removePicture(picture.id());
        mPictures->erase(mPictures->begin() + row);
    }
    endRemoveRows();
    return true;
}

void PictureModel::setAlbumId(int albumId) {
    beginResetModel();
    mAlbumId = albumId;
    loadPictures(albumId);
    endResetModel();
}

void PictureModel::loadPictures(int albumId) {
    beginResetModel();
    if (albumId <= 0) {
        mPictures.reset(new std::vector<std::unique_ptr<Picture>>());
    } else {
        mPictures = std::make_unique<std::vector<std::unique_ptr<Picture>>>(mDb.pictureDao->picturesForAlbum(albumId));
    }
    endResetModel();
}


void PictureModel::clearAlbum() {
    beginResetModel();
    mPictures.reset(new std::vector<std::unique_ptr<Picture>>());
    mAlbumId = -1;
    endResetModel();
}

void PictureModel::deletePicturesForAlbum() {
    mDb.pictureDao->removePicturesForAlbum(mAlbumId);
    clearAlbum();
}

bool PictureModel::isIndexValid(const QModelIndex& index) const {
    return index.row() >= 0 && index.row() < rowCount();
}
