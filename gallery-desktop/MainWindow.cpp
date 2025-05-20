#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "gallerywidget.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Asumiendo que en tu mainwindow.ui el centralWidget tiene un layout, p.ej. QVBoxLayout
    GalleryWidget *gallery = new GalleryWidget(this);
    ui->centralwidget->layout()->addWidget(gallery);
}

MainWindow::~MainWindow()
{
    delete ui;
}
