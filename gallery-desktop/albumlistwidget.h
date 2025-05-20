#ifndef ALBUMLISTWIDGET_H
#define ALBUMLISTWIDGET_H

#include <QListWidget>

class AlbumListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit AlbumListWidget(QWidget *parent = nullptr);

    void loadAlbumsFromFolder(const QString &folderPath);
    void createAlbum(const QString &albumName);
    void deleteSelectedAlbum();

    QString getCurrentFolderPath() const { return currentFolderPath; }

signals:
    void albumSelected(const QString &albumName);
    void albumRenamed(const QString &oldName, const QString &newName);

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;

private slots:
    void renameAlbum();

private:
    QString currentFolderPath;
};

#endif // ALBUMLISTWIDGET_H
