#include "AlbumModel.h"
#include "DatabaseManager.h"
#include "Album.h"
#include <QDebug>

AlbumModel::AlbumModel(QObject* parent) :
    QAbstractListModel(parent),
    mDb(DatabaseManager::instance())
{
    // Cargar los álbumes desde DAO
    auto albumsFromDao = mDb.albumDao.albums(); // QVector<Album*>
    mAlbums = albumsFromDao; // copiar punteros directamente
}

AlbumModel::~AlbumModel()
{
    // Destructor vacío para evitar el error del compilador
}
int AlbumModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return mAlbums.size();
}

bool AlbumModel::isIndexValid(const QModelIndex& index) const
{
    return index.isValid() && index.row() >= 0 && index.row() < mAlbums.size();
}


QVariant AlbumModel::data(const QModelIndex& index, int role) const
{
    if (!isIndexValid(index)) {
        return QVariant();
    }

    const Album& album = *mAlbums.at(index.row());

    switch (role) {
    case Roles::IdRole:
        return album.id();
    case Roles::NameRole:
    case Qt::DisplayRole:
        return album.name();
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> AlbumModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Roles::IdRole] = "id";
    roles[Roles::NameRole] = "name";
    return roles;
}

QModelIndex AlbumModel::addAlbum(const Album& album)
{
    int rowIndex = rowCount();
    beginInsertRows(QModelIndex(), rowIndex, rowIndex);

    // Crear un nuevo Album dinámicamente
    Album* newAlbum = new Album(album);
    mDb.albumDao.addAlbum(*newAlbum);

    // Añadir al QVector
    mAlbums.push_back(newAlbum);

    endInsertRows();
    return index(rowIndex, 0);
}

bool AlbumModel::setData(const QModelIndex& index, const QVariant& value,
                         int role)
{
    if (!isIndexValid(index) || role != Roles::NameRole) {
        return false;
    }

    Album& album = *mAlbums.at(index.row());
    album.setName(value.toString());
    mDb.albumDao.updateAlbum(album);

    emit dataChanged(index, index);
    return true;
}

bool AlbumModel::removeRows(int row, int count, const QModelIndex& parent)
{
    if (row < 0 || row + count > rowCount() || count <= 0) {
        return false;
    }

    beginRemoveRows(parent, row, row + count - 1);

    // Eliminar álbumes de la base de datos y liberar memoria
    for (int i = 0; i < count; ++i) {
        Album* album = mAlbums.at(row + i);
        mDb.albumDao.removeAlbum(album->id());
        delete album;
    }

    // Quitar los punteros del QVector
    mAlbums.erase(mAlbums.begin() + row, mAlbums.begin() + row + count);

    endRemoveRows();
    return true;
}
