#ifndef ALBUMLISTWIDGET_H
#define ALBUMLISTWIDGET_H

#include <QWidget>
#include <QItemSelectionModel>

namespace Ui {
class AlbumListWidget;
}

class AlbumModel;
class AlbumListWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AlbumListWidget(QWidget* parent = nullptr);
    ~AlbumListWidget();

    void setModel(AlbumModel* model);
    void setSelectionModel(QItemSelectionModel* selectionModel);

private:
    Ui::AlbumListWidget* ui;
    AlbumModel* mAlbumModel = nullptr;

private slots:
    void createAlbum();


};

#endif // ALBUMLISTWIDGET_H
