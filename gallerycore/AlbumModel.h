#include <QAbstractListModel>
#include <QHash>
#include "gallerycore_global.h"
#include "Album.h"
#include "DatabaseManager.h"


class GALLERYCORE_EXPORT AlbumModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles {
    IdRole = Qt::UserRole + 1,NameRole,
    };

    AlbumModel(QObject* parent = 0);
    ~AlbumModel();

    QModelIndex addAlbum(const Album& album);
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
    bool removeRows(int row, int count, const QModelIndex& parent)  override;

private:
    bool isIndexValid(const QModelIndex& index) const;

private:
    DatabaseManager& mDb;
    QVector<Album*> mAlbums;
};
