#include "PictureDao.h"
#include "qsqldatabase.h"
#include "qsqlquery.h"
#include <QSqlError>
#include "Picture.h"

/**
 * Constructor de PictureDao
 * @param database Referencia a la base de datos SQL que se utilizará para las operaciones
 *
 * Inicializa el objeto PictureDao con una referencia a la base de datos.
 * Este DAO (Data Access Object) encapsula todas las operaciones de acceso
 * a datos relacionadas con las imágenes (pictures) en la base de datos.
 */
PictureDao::PictureDao(QSqlDatabase& database) :
    mDatabase(database)  // Almacena la referencia a la base de datos
{
}

/**
 * Inicializa la tabla de imágenes en la base de datos
 *
 * Verifica si la tabla "pictures" existe en la base de datos.
 * Si no existe, la crea con la siguiente estructura:
 * - id: Clave primaria autoincremental que identifica únicamente cada imagen
 * - album_id: Clave foránea que referencia al álbum al que pertenece la imagen
 * - url: Ruta o URL del archivo de imagen (tipo TEXT)
 *
 * Esta función debe ser llamada durante la inicialización de la aplicación
 * para garantizar que la tabla existe antes de realizar operaciones.
 */
void PictureDao::init() const
{
    // Verifica si la tabla "pictures" ya existe en la base de datos
    if (!mDatabase.tables().contains("pictures")) {
        // Crea un objeto query para ejecutar comandos SQL
        QSqlQuery query(mDatabase);

        // Ejecuta la consulta SQL para crear la tabla pictures
        // Concatena strings para mejor legibilidad del código
        query.exec(QString("CREATE TABLE pictures")
        + " (id INTEGER PRIMARY KEY AUTOINCREMENT, "  // ID único de la imagen
        + "album_id INTEGER, "                         // ID del álbum asociado
        + "url TEXT)");                                // Ruta del archivo
    }
}

/**
 * Obtiene todas las imágenes asociadas a un álbum específico
 * @param albumId ID del álbum del cual se quieren obtener las imágenes
 * @return QVector con punteros a objetos Picture que pertenecen al álbum especificado
 *
 * Realiza una consulta filtrada por album_id y crea objetos Picture
 * para cada registro encontrado en la base de datos.
 *
 * Nota: El llamador es responsable de liberar la memoria de los punteros devueltos.
 */
QVector<Picture*> PictureDao::picturesForAlbum(int albumId) const
{
    // Vector que almacenará los punteros a las imágenes recuperadas
    QVector<Picture*> list;

    // Crea un objeto query para la base de datos
    QSqlQuery query(mDatabase);

    // Prepara una consulta parametrizada para evitar inyección SQL
    // Selecciona solo las imágenes que pertenecen al álbum especificado
    query.prepare("SELECT * FROM pictures WHERE album_id = :albumId");

    // Vincula el ID del álbum al parámetro :albumId de la consulta
    query.bindValue(":albumId", albumId);

    // Ejecuta la consulta SELECT
    query.exec();

    // Itera sobre cada fila del resultado de la consulta
    while (query.next()) {
        // Crea un nuevo objeto Picture en el heap
        Picture* pic = new Picture();

        // Establece el ID de la imagen desde la columna "id"
        pic->setId(query.value("id").toInt());

        // Establece la URL/ruta del archivo desde la columna "url"
        pic->setFileUrl(query.value("url").toString());

        // Establece el ID del álbum al que pertenece
        pic->setAlbumId(albumId);

        // Añade el puntero a la imagen en el vector
        list.push_back(pic);
    }

    return list;  // Retorna el vector con todas las imágenes del álbum
}

/**
 * Elimina una imagen de la base de datos
 * @param pictureId ID de la imagen que se desea eliminar
 *
 * Elimina permanentemente la imagen con el ID especificado de la base de datos.
 * Si la operación falla, registra el error en la consola de debug.
 *
 * Nota: Esta función  elimina el registro de la base de datos,
 * si elimina el archivo físico de imagen del sistema de archivos.
 */
void PictureDao::removePicture(int pictureId) const
{
    // Crea un objeto query (nota: usa el constructor por defecto,
    // podría ser mejor usar QSqlQuery query(mDatabase) para consistencia)
    QSqlQuery query;

    // Prepara la consulta SQL DELETE con parámetro nombrado
    query.prepare("DELETE FROM pictures WHERE id = :id");

    // Vincula el ID de la imagen a eliminar al parámetro :id
    query.bindValue(":id", pictureId);

    // Ejecuta la consulta DELETE y verifica si tuvo éxito
    if (!query.exec()) {
        // Si la ejecución falla, muestra el error en la consola de debug
        qDebug() << "Error al eliminar picture:" << query.lastError();
    }
}
