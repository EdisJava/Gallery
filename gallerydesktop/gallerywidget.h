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

    // ---------------------
    // Setters para modelos y selection models
    // ---------------------
    void setAlbumModel(AlbumModel* model);
    void setAlbumSelectionModel(QItemSelectionModel* selectionModel);

    void setPictureModel(ThumbnailProxyModel* model);
    void setPictureSelectionModel(QItemSelectionModel* selectionModel);

signals:
    void pictureActivated(const QModelIndex& index); // para MainWindow

private slots:
    void onPictureActivated(const QModelIndex& index);

private:
    Ui::GalleryWidget* ui;

    // Widgets internos
    AlbumListWidget* mAlbumListWidget;
    AlbumWidget* mAlbumWidget;

    // Selection models almacenados
    QItemSelectionModel* mAlbumSelectionModel = nullptr;
    QItemSelectionModel* mPictureSelectionModel = nullptr;
};

#endif // GALLERYWIDGET_H
