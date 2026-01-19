#ifndef GALLERYWIDGET_H
#define GALLERYWIDGET_H

#include <QWidget>
#include <QModelIndex>

class AlbumListWidget;
class AlbumWidget;
class AlbumModel;
class ThumbnailProxyModel;
class QItemSelectionModel;

namespace Ui {
class GalleryWidget;
}

class GalleryWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GalleryWidget(QWidget* parent = nullptr);
    ~GalleryWidget();

    void setAlbumModel(AlbumModel* model);
    void setAlbumSelectionModel(QItemSelectionModel* selectionModel);

    void setPictureModel(ThumbnailProxyModel* model);
    void setPictureSelectionModel(QItemSelectionModel* selectionModel);

signals:
    void pictureActivated(const QModelIndex& index);

private:
    Ui::GalleryWidget* ui;


    AlbumListWidget* mAlbumListWidget;
    AlbumWidget* mAlbumWidget;


    QItemSelectionModel* mAlbumSelectionModel = nullptr;
    QItemSelectionModel* mPictureSelectionModel = nullptr;

private slots:
    void onPictureActivated(const QModelIndex& index);

};

#endif // GALLERYWIDGET_H
