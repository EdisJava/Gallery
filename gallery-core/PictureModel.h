#ifndef PICTUREMODEL_H
#define PICTUREMODEL_H

#include <QAbstractListModel>
#include <memory>
#include <vector>

#include "Picture.h"
#include "DatabaseManager.h"

class AlbumModel;

class PictureModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum PictureRoles {
        FilePathRole = Qt::UserRole + 1
    };

    PictureModel(const AlbumModel& albumModel, QObject* parent = nullptr);


    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool removeRows(int row, int count, const QModelIndex& parent) override;

    void setAlbumId(int albumId);
    void clearAlbum();
    void addPicture(const Picture &picture);

    void loadPictures(int albumId);
public slots:
    void deletePicturesForAlbum();

private:
    bool isIndexValid(const QModelIndex& index) const;

private:
    DatabaseManager& mDb;
    int mAlbumId;
    std::unique_ptr<std::vector<std::unique_ptr<Picture>>> mPictures;
};

#endif // PICTUREMODEL_H
