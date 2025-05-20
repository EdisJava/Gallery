#include "AlbumDao.h"
#include "Album.h"

#include <QSqlQuery>
#include <QVariant>

AlbumDao::AlbumDao(QSqlDatabase& database) : mDatabase(database) {}

void AlbumDao::init() const {
    if (!mDatabase.tables().contains("albums")) {
        QSqlQuery query(mDatabase);
        query.exec("CREATE TABLE albums (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT)");
    }
}

void AlbumDao::addAlbum(Album& album) const {
    QSqlQuery query(mDatabase);
    query.prepare("INSERT INTO albums (name) VALUES (?)");
    query.addBindValue(album.name());
    query.exec();
    album.setId(query.lastInsertId().toInt());
}

void AlbumDao::updateAlbum(const Album& album) const {
    QSqlQuery query(mDatabase);
    query.prepare("UPDATE albums SET name = ? WHERE id = ?");
    query.addBindValue(album.name());
    query.addBindValue(album.id());
    query.exec();
}

void AlbumDao::removeAlbum(int id) const {
    QSqlQuery query(mDatabase);
    query.prepare("DELETE FROM albums WHERE id = ?");
    query.addBindValue(id);
    query.exec();
}

std::vector<std::unique_ptr<Album>> AlbumDao::albums() const {
    QSqlQuery query("SELECT * FROM albums", mDatabase);
    query.exec();

    std::vector<std::unique_ptr<Album>> list;
    while (query.next()) {
        std::unique_ptr<Album> album(new Album());
        album->setId(query.value("id").toInt());
        album->setName(query.value("name").toString());
        list.push_back(std::move(album));
    }
    return list;
}

