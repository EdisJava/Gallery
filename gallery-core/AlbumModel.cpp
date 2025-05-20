#include "AlbumModel.h"
#include "DatabaseManager.h"
#include "AlbumDao.h"  // ✅ AÑADE ESTA LÍNEA

AlbumModel::AlbumModel(QObject* parent)
    : QAbstractListModel(parent),
    mDb(DatabaseManager::instance()),
    mAlbums(std::make_unique<std::vector<std::unique_ptr<Album>>>(mDb.albumDao->albums()))
{}

QModelIndex AlbumModel::addAlbum(const Album& album) {
    int rowIndex = rowCount();
    beginInsertRows(QModelIndex(), rowIndex, rowIndex);
    std::unique_ptr<Album> newAlbum(new Album(album));
    mDb.albumDao->addAlbum(*newAlbum);
    mAlbums->push_back(std::move(newAlbum));
    endInsertRows();
    return index(rowIndex, 0);
}

int AlbumModel::rowCount(const QModelIndex& parent) const {
    Q_UNUSED(parent);
    return static_cast<int>(mAlbums->size());
}

QVariant AlbumModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const Album* album = mAlbums->at(index.row()).get();

    if (role == Qt::DisplayRole)
        return album->name();

    else if (role == Qt::UserRole)
        return QVariant::fromValue(const_cast<Album*>(album));

    return QVariant();
}


bool AlbumModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (!isIndexValid(index) || role != NameRole)
        return false;

    Album& album = *mAlbums->at(index.row());
    album.setName(value.toString());
    mDb.albumDao->updateAlbum(album);
    emit dataChanged(index, index);
    return true;
}

bool AlbumModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (row < 0 || row + count > rowCount(parent))
        return false;

    beginRemoveRows(parent, row, row + count - 1);

    for (int i = 0; i < count; ++i) {
        Album& album = *(*mAlbums)[row];
        mDb.albumDao->removeAlbum(album.id());
        mAlbums->erase(mAlbums->begin() + row);
    }

    endRemoveRows();
    return true;
}


QHash<int, QByteArray> AlbumModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[IdRole] = "id";
    roles[NameRole] = "name";
    return roles;
}

bool AlbumModel::isIndexValid(const QModelIndex& index) const {
    return index.isValid() && index.row() >= 0 && index.row() < rowCount();
}

bool AlbumModel::updateAlbum(const QModelIndex& index, const Album& album)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= rowCount())
        return false;

    // Actualiza la base de datos
    mDb.albumDao->updateAlbum(album);

    // Actualiza el objeto en la lista local
    (*mAlbums)[index.row()]->setName(album.name());
    // Notifica a la vista que los datos cambiaron
    emit dataChanged(index, index, {Qt::DisplayRole});

    return true;
}

