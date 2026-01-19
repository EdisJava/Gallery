#include <memory>
#include <vector>
#include <QAbstractListModel>
#include "gallerycore_global.h"
#include "Picture.h"

class Album;
class DatabaseManager;
class AlbumModel;

class GALLERYCORE_EXPORT PictureModel : public QAbstractListModel
{
    Q_OBJECT
public:

    enum PictureRole{
        FilePathRole = Qt::UserRole + 1
    };

    PictureModel (const AlbumModel& albumModel, QObject* parent = 0);

    QModelIndex addPicture(const Picture& picture);
    QVariant data(const QModelIndex& index, int role) const override;

    void setPictureModel(PictureModel* pictureModel);
    void removePicture(int row);
    void setAlbumId(int albumId);
    void clearAlbum();
    bool removeRows(int row, int count, const QModelIndex& parent) override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

public slots:
    void deletePicturesForAlbum();

private:
    void loadPictures(int albumId);
    bool isIndexValid(const QModelIndex& index) const;

    DatabaseManager& mDb;
    int mAlbumId;
    std::unique_ptr<std::vector<std::unique_ptr<Picture>>> mPictures;

};
