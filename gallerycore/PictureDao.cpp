#include "PictureDao.h"
#include "qsqldatabase.h"
#include "qsqlquery.h"
#include <QSqlError>
#include "Picture.h"


PictureDao::PictureDao(QSqlDatabase& database) :
    mDatabase(database)
{

}

QVector<Picture*> PictureDao::picturesForAlbum(int albumId) const
{
    QVector<Picture*> list;
    QSqlQuery query(mDatabase);
    query.prepare("SELECT * FROM pictures WHERE album_id = :albumId");
    query.bindValue(":albumId", albumId);
    query.exec();

    while (query.next()) {
        Picture* pic = new Picture();
        pic->setId(query.value("id").toInt());
        pic->setFileUrl(query.value("url").toString());
        pic->setAlbumId(albumId);
        list.push_back(pic);
    }

    return list;
}

void PictureDao::init() const
{
    if (!mDatabase.tables().contains("pictures")) {
        QSqlQuery query(mDatabase);
        query.exec(QString("CREATE TABLE pictures")
                   + " (id INTEGER PRIMARY KEY AUTOINCREMENT, "
                   + "album_id INTEGER, "
                   + "url TEXT)");
    }
}

void PictureDao::removePicture(int pictureId) const
{
    QSqlQuery query;
    query.prepare("DELETE FROM pictures WHERE id = :id");
    query.bindValue(":id", pictureId);
    if (!query.exec()) {
        qDebug() << "Error al eliminar picture:" << query.lastError();
    }
}
