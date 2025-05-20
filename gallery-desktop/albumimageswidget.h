#ifndef ALBUMIMAGESWIDGET_H
#define ALBUMIMAGESWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QString>
#include <QPushButton>

class AlbumImagesWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AlbumImagesWidget(QWidget *parent = nullptr);

    void loadImagesForAlbum(const QString &albumPath);

    void exportAlbumAsZip(const QString &albumPath, const QString &zipPath);

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;

private:
    QWidget *imagesContainer;
    QGridLayout *imagesLayout;
    QLabel *label;
    QPushButton *btnExportZip;

    QString currentAlbumPath;
    QString selectedImagePath;

    bool convertImageFormat(const QString &originalFilePath, const QString &newExtension);
    bool changeImageResolution(const QString &filePath, int newWidth, int newHeight);
};

#endif // ALBUMIMAGESWIDGET_H
