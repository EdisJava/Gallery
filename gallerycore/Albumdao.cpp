#include <QSqlDatabase>
#include <QSqlQuery>
#include "DatabaseManager.h"
#include <QVariant>
#include "Album.h"
#include <memory>


AlbumDao::AlbumDao(QSqlDatabase& database) :
    mDatabase(database)
{
}
void AlbumDao::init() const
{
    if (!mDatabase.tables().contains("albums")) {
        QSqlQuery query(mDatabase);
        query.exec("CREATE TABLE albums (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT)");
    }
}

void AlbumDao::addAlbum(Album& album) const
{
    QSqlQuery query(mDatabase);
    query.prepare("INSERT INTO albums (name) VALUES (:name)");
    query.bindValue(":name", album.name());
    query.exec();
    album.setID(query.lastInsertId().toInt());
}


QVector<Album*> AlbumDao::albums() const
{
    QSqlQuery query("SELECT * FROM albums", mDatabase);
    query.exec();

    QVector<Album*> list;

    while (query.next()) {
        Album* album = new Album();
        album->setID(query.value("id").toInt());
        album->setName(query.value("name").toString());

        list.push_back(album);
    }

    return list;
}


// Actualizar un álbum en la base de datos
void AlbumDao::updateAlbum(const Album& album) const
{
    QSqlQuery query(mDatabase);
    query.prepare("UPDATE albums SET name = :name WHERE id = :id");
    query.bindValue(":name", album.name());
    query.bindValue(":id", album.id());
    query.exec();
}

// Eliminar un álbum por ID
void AlbumDao::removeAlbum(int albumId) const
{
    QSqlQuery query(mDatabase);
    query.prepare("DELETE FROM albums WHERE id = :id");
    query.bindValue(":id", albumId);
    query.exec();
}

/*
 * Antes: QVector<std::unique_ptr<Album>>
 * Ahora: std::vector<std::unique_ptr<Album>>
 *
 * list.push_back(std::move(album));
 * Esto evita el error de use of deleted function porque unique_ptr no se puede copiar, solo mover.
 *
 * Encapsulamiento de RAII
 * Cada Album es ahora propiedad de std::unique_ptr y el vector se encarga de destruirlos al fina
*/
