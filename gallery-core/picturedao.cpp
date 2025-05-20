#include "PictureDao.h"
#include "Picture.h"
#include <QSqlQuery>
#include <QVariant>

PictureDao::PictureDao(QSqlDatabase& database) : mDatabase(database) {}

void PictureDao::init() const {
    if (!mDatabase.tables().contains("pictures")) {
        QSqlQuery query(mDatabase);
        query.exec("CREATE TABLE pictures (id INTEGER PRIMARY KEY AUTOINCREMENT, album_id INTEGER, url TEXT)");
    }
}

std::vector<std::unique_ptr<Picture>> PictureDao::picturesForAlbum(int albumId) const {
    QSqlQuery query(mDatabase);
    query.prepare("SELECT * FROM pictures WHERE album_id = ?");
    query.addBindValue(albumId);
    query.exec();

    std::vector<std::unique_ptr<Picture>> list;
    while (query.next()) {
        std::unique_ptr<Picture> picture(new Picture());
        picture->setId(query.value("id").toInt());
        picture->setAlbumId(query.value("album_id").toInt());
        picture->setFileUrl(QUrl(query.value("url").toString()));
        list.push_back(std::move(picture));
    }
    return list;
}

void PictureDao::addPictureInAlbum(int albumId, Picture& picture) const {
    QSqlQuery query(mDatabase);
    query.prepare("INSERT INTO pictures (album_id, url) VALUES (?, ?)");
    query.addBindValue(albumId);
    query.addBindValue(picture.fileUrl().toString());
    query.exec();
    picture.setId(query.lastInsertId().toInt());
    picture.setAlbumId(albumId);
}


void PictureDao::removePicture(int id) const {
    QSqlQuery query(mDatabase);
    query.prepare("DELETE FROM pictures WHERE id = ?");
    query.addBindValue(id);
    query.exec();
}

void PictureDao::removePicturesForAlbum(int albumId) const {
    QSqlQuery query(mDatabase);
    query.prepare("DELETE FROM pictures WHERE album_id = ?");
    query.addBindValue(albumId);
    query.exec();
}

