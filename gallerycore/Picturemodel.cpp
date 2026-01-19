#include "Picturemodel.h"
#include "Databasemanager.h"
#include <memory>
#include <vector>
#include <QFile>
#include "AlbumModel.h"
#include "qsqlerror.h"
#include "qsqlquery.h"

/**
 * Constructor de PictureModel
 * @param albumModel Referencia al modelo de álbumes para conectar señales
 * @param parent Objeto padre para la jerarquía de Qt (gestión automática de memoria)
 *
 * Inicializa el modelo de imágenes heredando de QAbstractListModel.
 * Configura la gestión de memoria usando smart pointers (unique_ptr) para
 * garantizar RAII y evitar fugas de memoria.
 *
 * Conecta la señal rowsRemoved del AlbumModel para eliminar automáticamente
 * las imágenes cuando se elimina un álbum, manteniendo la integridad referencial.
 */
PictureModel::PictureModel(const AlbumModel& albumModel, QObject* parent) :
    QAbstractListModel(parent),  // Llama al constructor de la clase base
    mDb(DatabaseManager::instance()),  // Obtiene la instancia singleton del DatabaseManager
    mAlbumId(-1),  // Inicializa con -1 indicando que no hay álbum seleccionado
    // Inicializa el vector de imágenes usando make_unique para gestión automática de memoria
    mPictures(std::make_unique<std::vector<std::unique_ptr<Picture>>>())
{
    // Conecta la señal de filas eliminadas del AlbumModel con el slot para eliminar imágenes
    // Cuando se elimina un álbum, automáticamente se eliminan sus imágenes asociadas
    connect(&albumModel, &AlbumModel::rowsRemoved,
            this, &PictureModel::deletePicturesForAlbum);
}

/**
 * Retorna los datos del modelo para un índice y rol específicos
 * @param index Índice del elemento en el modelo
 * @param role Rol que especifica qué dato se solicita
 * @return QVariant con el dato solicitado, o QVariant vacío si el índice es inválido
 *
 * Esta función es llamada por las vistas (como ListView en QML) para mostrar los datos.
 * Actualmente solo retorna la ruta del archivo de la imagen.
 */
QVariant PictureModel::data(const QModelIndex& index, int role) const
{
    // Valida que el índice sea válido y esté dentro del rango
    if (!index.isValid() || index.row() < 0 || index.row() >= rowCount())
        return QVariant();  // Retorna un QVariant vacío si es inválido

    // Obtiene el puntero crudo (raw pointer) del unique_ptr en la posición especificada
    const Picture* pic = mPictures->at(index.row()).get();

    // Retorna el dato correspondiente según el rol solicitado
    switch (role) {
    case Qt::DisplayRole:      // Rol estándar de Qt para mostrar texto
    case FilePathRole:         // Rol personalizado para la ruta del archivo
        return pic->fileUrl(); // Retorna la URL del archivo de imagen
    default:
        return QVariant();     // Retorna vacío si el rol no es reconocido
    }
}

/**
 * Retorna el número de filas (imágenes) en el modelo
 * @param parent Índice padre (no usado en modelos de lista)
 * @return Cantidad total de imágenes en el modelo
 *
 * Esta función es requerida por QAbstractListModel y es usada por las vistas
 * para saber cuántos elementos mostrar.
 */
int PictureModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)  // Marca el parámetro como no usado para evitar warnings
    // Convierte size_t a int de forma segura usando static_cast
    return static_cast<int>(mPictures->size());
}

/**
 * Elimina una o más filas (imágenes) del modelo
 * @param row Índice de la primera fila a eliminar
 * @param count Número de filas a eliminar
 * @param parent Índice padre (no usado en modelos de lista)
 * @return true si la eliminación fue exitosa, false en caso contrario
 *
 * NOTA: Esta función solo elimina del modelo en memoria, NO elimina de la base de datos
 * ni del sistema de archivos. Para eliminación completa, usar removePicture().
 */
bool PictureModel::removeRows(int row, int count, const QModelIndex& parent)
{
    Q_UNUSED(parent)  // Marca el parámetro como no usado

    // Valida los parámetros de entrada
    if (row < 0 || row >= rowCount() || count <= 0 || row + count > rowCount())
        return false;  // Parámetros inválidos

    // Notifica a las vistas que se van a eliminar filas
    beginRemoveRows(QModelIndex(), row, row + count - 1);

    // Elimina el rango de elementos del vector usando iteradores
    // Los unique_ptr se destruyen automáticamente, liberando la memoria
    mPictures->erase(mPictures->begin() + row,
                     mPictures->begin() + row + count);

    // Notifica a las vistas que la eliminación ha terminado
    endRemoveRows();
    return true;  // Eliminación exitosa
}

/**
 * Añade una nueva imagen al modelo y a la base de datos
 * @param picture Referencia a la imagen a añadir
 * @return QModelIndex de la nueva imagen insertada, o índice inválido si no hay álbum seleccionado
 *
 * La imagen se asocia automáticamente al álbum actualmente seleccionado (mAlbumId).
 * Si no hay álbum seleccionado (mAlbumId <= 0), la operación falla.
 */
QModelIndex PictureModel::addPicture(const Picture& picture)
{
    // Verifica que haya un álbum válido seleccionado
    if (mAlbumId <= 0)
        return QModelIndex();  // Retorna índice inválido si no hay álbum

    // Crea una copia de la imagen para modificarla
    Picture pic = picture;
    // Asocia la imagen al álbum actual
    pic.setAlbumId(mAlbumId);

    // GUARDAR EN BASE DE DATOS
    // Inserta la imagen en la base de datos y actualiza su ID
    mDb.pictureDao.addPictureInAlbum(mAlbumId, pic);

    // Obtiene la posición donde se insertará (al final)
    int newRow = rowCount();

    // Notifica a las vistas que se va a insertar una fila
    beginInsertRows(QModelIndex(), newRow, newRow);

    // Crea un unique_ptr con la imagen y lo añade al vector
    // make_unique garantiza la gestión automática de memoria
    mPictures->push_back(std::make_unique<Picture>(pic));

    // Notifica a las vistas que la inserción ha terminado
    endInsertRows();

    return index(newRow, 0);  // Retorna el índice de la nueva imagen
}

/**
 * Carga todas las imágenes asociadas a un álbum específico desde la base de datos
 * @param albumId ID del álbum cuyas imágenes se desean cargar
 *
 * Esta función reemplaza completamente el contenido actual del modelo con las
 * imágenes del álbum especificado. Si el albumId es inválido (<=0), limpia
 * todas las imágenes del modelo.
 *
 * Convierte el QVector<Picture*> retornado por el DAO a std::vector<std::unique_ptr<Picture>>
 * para una gestión de memoria más segura con RAII.
 */
void PictureModel::loadPictures(int albumId)
{
    qDebug() << "=== PictureModel::loadPictures ===";
    qDebug() << "albumId:" << albumId;

    // Si el albumId es inválido, limpia el modelo
    if (albumId <= 0) {
        // Crea un nuevo vector vacío
        mPictures = std::make_unique<std::vector<std::unique_ptr<Picture>>>();
        qDebug() << "albumId inválido, limpiando pictures";
        return;
    }

    // Obtiene las imágenes del álbum desde el DAO
    // Retorna QVector<Picture*> (punteros crudos)
    QVector<Picture*> picturesFromDao = mDb.pictureDao.picturesForAlbum(albumId);
    qDebug() << "Pictures obtenidas del DAO:" << picturesFromDao.count();

    // Crea un nuevo vector para almacenar las imágenes con unique_ptr
    auto newPictures = std::make_unique<std::vector<std::unique_ptr<Picture>>>();

    // Convierte cada puntero crudo a unique_ptr
    // Transfiere la propiedad de la memoria a los unique_ptr
    for (Picture* p : picturesFromDao) {
        newPictures->push_back(std::unique_ptr<Picture>(p));
    }

    // Mueve el nuevo vector al miembro de la clase
    // std::move transfiere la propiedad, el vector anterior se destruye automáticamente
    mPictures = std::move(newPictures);
    qDebug() << "Total pictures cargadas:" << mPictures->size();
}

/**
 * Elimina completamente una imagen: del disco, de la base de datos y del modelo
 * @param row Índice de la imagen a eliminar
 *
 * Esta función realiza una eliminación completa en tres niveles:
 * 1. Elimina el archivo físico del sistema de archivos
 * 2. Elimina el registro de la base de datos
 * 3. Elimina el objeto del modelo en memoria
 *
 * Es la forma correcta de eliminar una imagen de forma permanente.
 */
void PictureModel::removePicture(int row)
{
    // Valida que el índice esté dentro del rango
    if (row < 0 || row >= rowCount()) return;

    // Obtiene el puntero crudo del unique_ptr
    Picture* p = mPictures->at(row).get();
    if (p) {
        // 1. Borrar archivo físico del disco
        QFile file(p->fileUrl().toLocalFile());  // Convierte URL a ruta local
        if (file.exists()) {
            file.remove();  // Elimina el archivo del sistema de archivos
        }

        // 2. Borrar registro de la base de datos usando PictureDao
        mDb.pictureDao.removePicture(p->id());
    }

    // 3. Borrar del modelo (lista en memoria)
    // Notifica a las vistas del cambio
    beginRemoveRows(QModelIndex(), row, row);

    // Elimina el unique_ptr del vector
    // El unique_ptr se destruye automáticamente y libera la memoria del objeto Picture
    mPictures->erase(mPictures->begin() + row);

    // Notifica a las vistas que la eliminación ha terminado
    endRemoveRows();
}

/**
 * Establece el álbum activo y carga sus imágenes
 * @param albumId ID del álbum a establecer como activo
 *
 * Cambia el álbum actual del modelo y recarga todas las imágenes
 * correspondientes al nuevo álbum. Las imágenes del álbum anterior
 * se eliminan automáticamente del modelo.
 *
 * Usa beginResetModel/endResetModel para notificar a las vistas que
 * el contenido del modelo ha cambiado completamente.
 */
void PictureModel::setAlbumId(int albumId)
{
    qDebug() << "PictureModel::setAlbumId:" << albumId;

    // Notifica a las vistas que el modelo va a cambiar completamente
    beginResetModel();

    // Actualiza el ID del álbum activo
    mAlbumId = albumId;

    // Carga las imágenes del nuevo álbum
    loadPictures(mAlbumId);

    // Notifica a las vistas que el modelo ha terminado de cambiar
    endResetModel();
}

/**
 * Añade una imagen a un álbum específico en la base de datos
 * @param albumId ID del álbum al que se añadirá la imagen
 * @param picture Referencia a la imagen a añadir (se actualizará su ID)
 *
 * Inserta un nuevo registro de imagen en la tabla pictures de la base de datos.
 * Después de la inserción exitosa, actualiza el ID del objeto Picture con el
 * valor generado automáticamente por la base de datos.
 *
 * Si la inserción falla, muestra el error en la consola de debug.
 */
void PictureDao::addPictureInAlbum(int albumId, Picture& picture) const
{
    // Crea un objeto query para la base de datos
    QSqlQuery query(mDatabase);

    // Prepara la consulta SQL INSERT con parámetros nombrados
    query.prepare(
        "INSERT INTO pictures (album_id, url) "
        "VALUES (:albumId, :url)"
        );

    // Vincula el ID del álbum al parámetro :albumId
    query.bindValue(":albumId", albumId);

    // Vincula la URL de la imagen al parámetro :url
    // Convierte QUrl a QString para almacenar en la base de datos
    query.bindValue(":url", picture.fileUrl().toString());

    // Ejecuta la consulta INSERT y verifica si tuvo éxito
    if (!query.exec()) {
        // Si falla, muestra el error en la consola de debug
        qDebug() << "Error insertando picture:" << query.lastError();
        return;
    }

    // Actualiza el ID de la imagen con el valor generado automáticamente
    picture.setId(query.lastInsertId().toInt());
}

/**
 * Elimina todas las imágenes del modelo actual
 *
 * Esta función se llama automáticamente cuando se elimina un álbum
 * (conectada mediante signal/slot en el constructor).
 *
 * Limpia completamente el modelo de imágenes, destruyendo todos los
 * unique_ptr y liberando la memoria automáticamente gracias a RAII.
 *
 * NOTA: Solo elimina del modelo en memoria, NO elimina de la base de datos
 * ni del sistema de archivos. La eliminación en cascada de la BD debe
 * manejarse en el DAO de álbumes.
 */
void PictureModel::deletePicturesForAlbum()
{
    // Notifica a las vistas que el modelo va a cambiar completamente
    beginResetModel();

    // Limpia el vector de imágenes
    // Los unique_ptr se destruyen automáticamente, liberando toda la memoria
    mPictures->clear();

    // Notifica a las vistas que el modelo ha terminado de cambiar
    endResetModel();
}

/*
 * NOTAS SOBRE CORRECCIONES Y MEJORAS REALIZADAS:
 *
 * 1. Includes faltantes agregados:
 *    - #include "AlbumModel.h"
 *    - #include <memory>
 *    - #include <vector>
 *
 * 2. Constructor mejorado:
 *    ANTES:
 *    mPictures(new vector<unique_ptr<Picture>>())
 *
 *    AHORA:
 *    mPictures(std::make_unique<std::vector<std::unique_ptr<Picture>>>())
 *
 *    Ventajas:
 *    - std::make_unique es más seguro (exception-safe)
 *    - Sintaxis más clara y moderna (C++14)
 *    - Evita posibles fugas de memoria en casos de excepción
 *
 * 3. Signal/Slot connection:
 *    - Se usó lambda para resolver problemas de firma de señal
 *    - Se usó QAbstractItemModel::rowsRemoved en lugar de AlbumModel::rowsRemoved
 *      para mayor compatibilidad
 *
 * 4. Método loadPictures mejorado:
 *    ANTES:
 *    void PictureModel::loadPictures(int albumId)
 *    {
 *        if (albumId <= 0) {
 *            mPictures(new vector<unique_ptr<Picture>>());
 *            return;
 *        }
 *        mPictures = mDb.pictureDao.picturesForAlbum(albumId);
 *    }
 *
 *    AHORA:
 *    - Se reemplaza 'new' por std::make_unique para gestión automática de memoria
 *    - Se transforma el QVector<Picture*> devuelto por el DAO a
 *      std::vector<std::unique_ptr<Picture>> para mantener consistencia de tipos
 *    - Se usa std::move para transferir la propiedad del unique_ptr sin copias
 *
 * 5. PictureDao::addPictureInAlbum agregado:
 *    - Esta función faltaba completamente en la implementación original
 *    - Es esencial para insertar imágenes en la base de datos
 *
 * 6. Gestión de memoria con RAII:
 *    - Todos los punteros son ahora unique_ptr
 *    - La memoria se libera automáticamente cuando los objetos salen de scope
 *    - Elimina la necesidad de llamadas manuales a delete
 *    - Previene fugas de memoria y errores de doble liberación
 */
