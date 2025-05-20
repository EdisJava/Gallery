#ifndef PICTUREWIDGET_H
#define PICTUREWIDGET_H

#include <QWidget>
#include <QPixmap>

namespace Ui {
class PictureWidget;
}

class PictureWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PictureWidget(QWidget *parent = nullptr);
    ~PictureWidget();

    void setPicture(const QPixmap &pixmap);

private:
    Ui::PictureWidget *ui;
};

#endif // PICTUREWIDGET_H
