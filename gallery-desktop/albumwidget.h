#ifndef ALBUMWIDGET_H
#define ALBUMWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

class AlbumWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AlbumWidget(QWidget *parent = nullptr);

    void setAlbumName(const QString &name);
    void setAlbumPath(const QString &path);

    QString albumName() const;
    QString albumPath() const;

signals:
    void renameRequested(const QString &newName);
    void addImageRequested(const QString &albumPath);
    void deleteImageRequested(const QString &albumPath);

private slots:
    void onRenameClicked();
    void onAddImageClicked();
    void onDeleteImageClicked();

private:
    QLabel *label;
    QPushButton *renameButton;
    QPushButton *addImageButton;
    QPushButton *deleteImageButton;

    QString currentAlbumName;
    QString currentAlbumPath;
};

#endif // ALBUMWIDGET_H
