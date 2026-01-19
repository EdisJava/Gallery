#include <QSqlDatabase>
#include <QSqlQuery>
#include "DatabaseManager.h"
#include <QVariant>
#include "Album.h"
#include <memory>

/**
 * Constructor de AlbumDao
 * @param database Referencia a la base de datos SQL que se utilizará para las operaciones
 * Inicializa el objeto AlbumDao con una referencia a la base de datos
 */
AlbumDao::AlbumDao(QSqlDatabase& database) :
    mDatabase(database)
{
}

/**
 * Inicializa la tabla de álbumes en la base de datos
 * Verifica si la tabla "albums" existe, y si no existe, la crea
 * La tabla contiene dos campos:
 * - id: clave primaria autoincremental
 * - name: nombre del álbum (tipo TEXT)
 */
void AlbumDao::init() const
{
    // Verifica si la tabla "albums" ya existe en la base de datos
    if (!mDatabase.tables().contains("albums")) {
        // Crea un objeto query para ejecutar comandos SQL
        QSqlQuery query(mDatabase);
        // Ejecuta la consulta SQL para crear la tabla albums
        query.exec("CREATE TABLE albums (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT)");
    }
}

/**
 * Obtiene todos los álbumes de la base de datos
 * @return QVector con punteros a objetos Album que contienen todos los álbumes almacenados
 * Nota: El llamador es responsable de liberar la memoria de los punteros devueltos
 */
QVector<Album*> AlbumDao::albums() const
{
    // Prepara y ejecuta una consulta para obtener todos los álbumes
    QSqlQuery query("SELECT * FROM albums", mDatabase);
    query.exec();

    // Vector que almacenará los punteros a los álbumes recuperados
    QVector<Album*> list;

    // Itera sobre cada fila del resultado de la consulta
    while (query.next()) {
        // Crea un nuevo objeto Album en el heap
        Album* album = new Album();
        // Establece el ID del álbum desde la columna "id" de la consulta
        album->setID(query.value("id").toInt());
        // Establece el nombre del álbum desde la columna "name" de la consulta
        album->setName(query.value("name").toString());
        // Añade el puntero al álbum en el vector
        list.push_back(album);
    }

    return list;
}

/**
 * Añade un nuevo álbum a la base de datos
 * @param album Referencia al objeto Album que se desea añadir
 * Inserta el álbum en la base de datos y actualiza su ID con el valor generado automáticamente
 */
void AlbumDao::addAlbum(Album& album) const
{
    // Crea un objeto query para la base de datos
    QSqlQuery query(mDatabase);
    // Prepara la consulta SQL con un parámetro nombrado para evitar inyección SQL
    query.prepare("INSERT INTO albums (name) VALUES (:name)");
    // Vincula el valor del nombre del álbum al parámetro :name
    query.bindValue(":name", album.name());
    // Ejecuta la consulta INSERT
    query.exec();
    // Actualiza el ID del álbum con el ID generado automáticamente por la base de datos
    album.setID(query.lastInsertId().toInt());
}

/**
 * Actualiza un álbum existente en la base de datos
 * @param album Referencia constante al objeto Album con los datos actualizados
 * Modifica el nombre del álbum correspondiente al ID especificado
 */
void AlbumDao::updateAlbum(const Album& album) const
{
    // Crea un objeto query para la base de datos
    QSqlQuery query(mDatabase);
    // Prepara la consulta SQL UPDATE con parámetros nombrados
    query.prepare("UPDATE albums SET name = :name WHERE id = :id");
    // Vincula el nuevo nombre del álbum al parámetro :name
    query.bindValue(":name", album.name());
    // Vincula el ID del álbum al parámetro :id para identificar qué álbum actualizar
    query.bindValue(":id", album.id());
    // Ejecuta la consulta UPDATE
    query.exec();
}

/**
 * Elimina un álbum de la base de datos
 * @param albumId ID del álbum que se desea eliminar
 * Elimina permanentemente el álbum con el ID especificado de la base de datos
 */
void AlbumDao::removeAlbum(int albumId) const
{
    // Crea un objeto query para la base de datos
    QSqlQuery query(mDatabase);
    // Prepara la consulta SQL DELETE con parámetro nombrado
    query.prepare("DELETE FROM albums WHERE id = :id");
    // Vincula el ID del álbum a eliminar al parámetro :id
    query.bindValue(":id", albumId);
    // Ejecuta la consulta DELETE
    query.exec();
}

/*
 * NOTAS SOBRE EVOLUCIÓN DEL CÓDIGO Y BUENAS PRÁCTICAS:
 *
 * Cambio de implementación:
 * - Antes: QVector<std::unique_ptr<Album>>
 * - Ahora: std::vector<std::unique_ptr<Album>>
 *
 * Razón del cambio:
 * list.push_back(std::move(album));
 * Esto evita el error "use of deleted function" porque std::unique_ptr no se puede copiar,
 * solo mover. Al usar std::move(), transferimos la propiedad del objeto de forma explícita.
 *
 * Ventajas del patrón RAII (Resource Acquisition Is Initialization):
 * - Cada Album es ahora propiedad de std::unique_ptr
 * - El vector se encarga automáticamente de destruir los objetos Album al finalizar su ciclo de vida
 * - Evita fugas de memoria (memory leaks)
 * - Hace el código más seguro y robusto
 * - No es necesario llamar manualmente a delete para liberar memoria
 */
