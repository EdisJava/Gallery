#ifndef GALLERYWIDGET_H
#define GALLERYWIDGET_H

#include <QWidget>
#include "albumlistwidget.h"
#include "albumimageswidget.h"


class QPushButton;
class QVBoxLayout;

class GalleryWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GalleryWidget(QWidget *parent = nullptr);

private slots:
    void selectBaseFolder();
    void onAlbumSelected(const QString &albumName);
    void createAlbum();
    void deleteAlbum();
    void addImageToAlbum();
    void deleteImageFromAlbum();

private:
    AlbumListWidget *albumListWidget;
    AlbumImagesWidget *albumImagesWidget;

    QPushButton *btnSelectBaseFolder;
    QPushButton *btnCreateAlbum;
    QPushButton *btnDeleteAlbum;
    QPushButton *btnAddImage;
    QPushButton *btnDeleteImage;

    QString currentBaseFolder;
    QString currentAlbumName;
    QString currentAlbumPath;

    QVBoxLayout *mainLayout;

    void updateCurrentAlbum(const QString &albumName);
};

#endif // GALLERYWIDGET_H
