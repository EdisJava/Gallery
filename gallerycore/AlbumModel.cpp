#include "AlbumModel.h"
#include "DatabaseManager.h"
#include "Album.h"
#include <QDebug>

/**
 * Constructor de AlbumModel
 * @param parent Objeto padre para la jerarquía de Qt (gestión automática de memoria)
 * Inicializa el modelo de álbumes heredando de QAbstractListModel y carga
 * todos los álbumes existentes desde la base de datos
 */
AlbumModel::AlbumModel(QObject* parent) :
    QAbstractListModel(parent),  // Llama al constructor de la clase base
    mDb(DatabaseManager::instance())  // Obtiene la instancia singleton del DatabaseManager
{
    // Cargar los álbumes desde el DAO (Data Access Object)
    auto albumsFromDao = mDb.albumDao.albums(); // Retorna QVector<Album*>
    mAlbums = albumsFromDao; // Copia directa de los punteros al vector del modelo
}

/**
 * Destructor de AlbumModel
 * Nota: Actualmente vacío, pero necesario para evitar errores del compilador
 * La liberación de memoria de los álbumes se realiza en removeRows()
 */
AlbumModel::~AlbumModel()
{
    // Destructor vacío para evitar el error del compilador
}

/**
 * Define los nombres de roles personalizados para acceder a los datos del modelo
 * @return QHash que mapea los roles (int) a sus nombres (QByteArray)
 * Permite acceder a las propiedades del álbum desde QML usando los nombres definidos
 * Por ejemplo: album.name o album.id en QML
 */
QHash<int, QByteArray> AlbumModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Roles::IdRole] = "id";      // Rol para acceder al ID del álbum
    roles[Roles::NameRole] = "name";  // Rol para acceder al nombre del álbum
    return roles;
}

/**
 * Retorna los datos del modelo para un índice y rol específicos
 * @param index Índice del elemento en el modelo
 * @param role Rol que especifica qué dato se solicita (id, name, etc.)
 * @return QVariant con el dato solicitado, o QVariant vacío si el índice es inválido
 * Esta función es llamada por las vistas (como ListView en QML) para mostrar los datos
 */
QVariant AlbumModel::data(const QModelIndex& index, int role) const
{
    // Verifica si el índice es válido antes de acceder a los datos
    if (!isIndexValid(index)) {
        return QVariant();  // Retorna un QVariant vacío si el índice es inválido
    }

    // Obtiene una referencia constante al álbum en la posición especificada
    const Album& album = *mAlbums.at(index.row());

    // Retorna el dato correspondiente según el rol solicitado
    switch (role) {
    case Roles::IdRole:
        return album.id();  // Retorna el ID del álbum
    case Roles::NameRole:
    case Qt::DisplayRole:   // Rol estándar de Qt para mostrar texto
        return album.name();  // Retorna el nombre del álbum
    default:
        return QVariant();  // Retorna vacío si el rol no es reconocido
    }
}

/**
 * Retorna el número de filas (elementos) en el modelo
 * @param parent Índice padre (no usado en modelos de lista)
 * @return Cantidad total de álbumes en el modelo
 * Esta función es requerida por QAbstractListModel y es usada por las vistas
 * para saber cuántos elementos mostrar
 */
int AlbumModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)  // Marca el parámetro como no usado para evitar warnings
    return mAlbums.size();  // Retorna el tamaño del vector de álbumes
}

/**
 * Valida si un índice del modelo es válido
 * @param index Índice a validar
 * @return true si el índice es válido, false en caso contrario
 * Un índice es válido si existe, su fila es >= 0 y está dentro del rango del vector
 */
bool AlbumModel::isIndexValid(const QModelIndex& index) const
{
    return index.isValid() &&          // El índice debe ser válido
           index.row() >= 0 &&         // La fila debe ser no negativa
           index.row() < mAlbums.size();  // La fila debe estar dentro del rango
}

/**
 * Añade un nuevo álbum al modelo y a la base de datos
 * @param album Referencia constante al objeto Album a añadir
 * @return QModelIndex del nuevo álbum insertado
 * Notifica a las vistas sobre la inserción mediante beginInsertRows/endInsertRows
 */
QModelIndex AlbumModel::addAlbum(const Album& album)
{
    int rowIndex = rowCount();  // Obtiene el índice donde se insertará (al final)

    // Notifica a las vistas que se van a insertar filas
    beginInsertRows(QModelIndex(), rowIndex, rowIndex);

    // Crear un nuevo Album dinámicamente en el heap (copia del álbum recibido)
    Album* newAlbum = new Album(album);

    // Añade el álbum a la base de datos y actualiza su ID
    mDb.albumDao.addAlbum(*newAlbum);

    // Añadir el puntero al QVector del modelo
    mAlbums.push_back(newAlbum);

    // Notifica a las vistas que la inserción ha terminado
    endInsertRows();

    return index(rowIndex, 0);  // Retorna el índice del nuevo álbum
}

/**
 * Modifica los datos de un álbum existente en el modelo
 * @param index Índice del álbum a modificar
 * @param value Nuevo valor a establecer
 * @param role Rol que especifica qué dato modificar
 * @return true si la modificación fue exitosa, false en caso contrario
 * Actualiza tanto el modelo como la base de datos y notifica a las vistas del cambio
 */
bool AlbumModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    // Valida que el índice sea válido y que el rol sea NameRole
    if (!isIndexValid(index) || role != Roles::NameRole) {
        return false;  // No se puede modificar
    }

    // Obtiene una referencia al álbum a modificar
    Album& album = *mAlbums.at(index.row());

    // Establece el nuevo nombre del álbum
    album.setName(value.toString());

    // Actualiza el álbum en la base de datos
    mDb.albumDao.updateAlbum(album);

    // Notifica a las vistas que los datos han cambiado
    emit dataChanged(index, index);

    return true;  // Modificación exitosa
}

/**
 * Elimina una o más filas (álbumes) del modelo y de la base de datos
 * @param row Índice de la primera fila a eliminar
 * @param count Número de filas a eliminar
 * @param parent Índice padre (no usado en modelos de lista)
 * @return true si la eliminación fue exitosa, false en caso contrario
 * Libera la memoria de los álbumes eliminados y actualiza la base de datos
 */
bool AlbumModel::removeRows(int row, int count, const QModelIndex& parent)
{
    // Valida los parámetros de entrada
    if (row < 0 || row + count > rowCount() || count <= 0) {
        return false;  // Parámetros inválidos
    }

    // Notifica a las vistas que se van a eliminar filas
    beginRemoveRows(parent, row, row + count - 1);

    // Eliminar álbumes de la base de datos y liberar memoria
    for (int i = 0; i < count; ++i) {
        // Obtiene el puntero al álbum a eliminar
        Album* album = mAlbums.at(row + i);

        // Elimina el álbum de la base de datos usando su ID
        mDb.albumDao.removeAlbum(album->id());

        // Libera la memoria del objeto Album
        delete album;
    }

    // Quitar los punteros del QVector
    // Usa erase() con iteradores para eliminar un rango de elementos
    mAlbums.erase(mAlbums.begin() + row, mAlbums.begin() + row + count);

    // Notifica a las vistas que la eliminación ha terminado
    endRemoveRows();

    return true;  // Eliminación exitosa
}
