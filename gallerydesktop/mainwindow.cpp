#include "mainwindow.h"
#include "albumwidget.h"
#include "gallerywidget.h"
#include "picturewidget.h"
#include "albummodel.h"
#include "picturemodel.h"
#include "thumbnailproxymodel.h"

#include <QStackedWidget>
#include <QItemSelectionModel>
#include <QDebug>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle(tr("Gallery"));
    setWindowIcon(QIcon(":/icons/logo.png"));


    // ------------------------------
    // Crear stacked widget
    // ------------------------------
    mStackedWidget = new QStackedWidget(this);

    // ------------------------------
    // Crear widgets
    // ------------------------------
    mGalleryWidget = new GalleryWidget(this);
    mPictureWidget = new PictureWidget(this);

    // ------------------------------
    // Crear modelos
    // ------------------------------
    AlbumModel* albumModel = new AlbumModel(this);
    QItemSelectionModel* albumSelectionModel = new QItemSelectionModel(albumModel, this);

    PictureModel* pictureModel = new PictureModel(*albumModel, this);
    ThumbnailProxyModel* thumbnailModel = new ThumbnailProxyModel(this);
    thumbnailModel->setSourceModel(pictureModel);
    QItemSelectionModel* pictureSelectionModel = new QItemSelectionModel(thumbnailModel, this);

    // ------------------------------
    // Asignar modelos a widgets
    // ------------------------------
    qDebug() << "=== MainWindow: Asignando modelos ===";
    mGalleryWidget->setAlbumModel(albumModel);
    qDebug() << "AlbumModel asignado";
    mGalleryWidget->setAlbumSelectionModel(albumSelectionModel);
    qDebug() << "AlbumSelectionModel asignado";
    mGalleryWidget->setPictureModel(thumbnailModel);
    qDebug() << "PictureModel asignado";
    mGalleryWidget->setPictureSelectionModel(pictureSelectionModel);
    qDebug() << "PictureSelectionModel asignado";


    mPictureWidget->setModel(thumbnailModel);
    mPictureWidget->setSelectionModel(pictureSelectionModel);

    // ------------------------------
    // Conectar señales
    // ------------------------------
    connect(mGalleryWidget, &GalleryWidget::pictureActivated,
            this, &MainWindow::displayPicture);

    connect(mPictureWidget, &PictureWidget::backToGallery,
            this, &MainWindow::displayGallery);

    // ------------------------------
    // Añadir widgets al stacked widget
    // ------------------------------
    mStackedWidget->addWidget(mGalleryWidget);
    mStackedWidget->addWidget(mPictureWidget);

    // ------------------------------
    // Mostrar la vista de galerías al inicio
    // ------------------------------
    displayGallery();

    // ------------------------------
    // Asignar el stacked widget como central
    // ------------------------------
    setCentralWidget(mStackedWidget);
}

MainWindow::~MainWindow()
{

}

// ------------------------------
// Slots para cambiar entre vistas
// ------------------------------
void MainWindow::displayGallery()
{
    mStackedWidget->setCurrentWidget(mGalleryWidget);
}

void MainWindow::displayPicture(const QModelIndex& index)
{
    qDebug() << "=== MainWindow::displayPicture ===";
    qDebug() << "Index row:" << index.row();
    qDebug() << "Index válido:" << index.isValid();

    // Establecer la foto actual antes de cambiar de widget
    mPictureWidget->setCurrentIndex(index);

    // Cambiar al widget de foto
    mStackedWidget->setCurrentWidget(mPictureWidget);
}
