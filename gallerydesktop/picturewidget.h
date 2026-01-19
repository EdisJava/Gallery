#ifndef PICTUREWIDGET_H
#define PICTUREWIDGET_H

#include <QWidget>
#include <QItemSelection>

namespace Ui {
class PictureWidget;
}

    class PictureModel;
    class QItemSelectionModel;
    class ThumbnailProxyModel;
    class PictureWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PictureWidget(QWidget *parent = 0);
    ~PictureWidget();
    void setModel(ThumbnailProxyModel* model);
    void setSelectionModel(QItemSelectionModel* selectionModel);

signals:
    void backToGallery();

public slots:
    void setCurrentIndex(const QModelIndex& index);

protected:
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void deletePicture();
    void loadPicture(const QItemSelection& selected);

private:
    void updatePicturePixmap();
    Ui::PictureWidget* ui;
    ThumbnailProxyModel* mModel;
    QItemSelectionModel* mSelectionModel;
    QPixmap mPixmap;

};
#endif // PICTUREWIDGET_H
