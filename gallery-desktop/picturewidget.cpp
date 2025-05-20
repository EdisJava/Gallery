#include "picturewidget.h"
#include "ui_picturewidget.h"

PictureWidget::PictureWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PictureWidget)
{
    ui->setupUi(this);
}

PictureWidget::~PictureWidget()
{
    delete ui;
}

void PictureWidget::setPicture(const QPixmap &pixmap)
{
    ui->label->setPixmap(pixmap.scaled(ui->label->size(),
                                       Qt::KeepAspectRatio,
                                       Qt::SmoothTransformation));
}
