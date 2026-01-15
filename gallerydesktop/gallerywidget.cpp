#include "gallerywidget.h"
#include "ui_gallerywidget.h"
#include "albumlistwidget.h"
#include "albumwidget.h"
#include "albummodel.h"
#include "picturemodel.h"
#include "thumbnailproxymodel.h"
#include <QItemSelectionModel>

GalleryWidget::GalleryWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::GalleryWidget)
{
    ui->setupUi(this);


    // Obtener widgets promovidos desde UI
    mAlbumListWidget = ui->albumListWidget;
    mAlbumWidget = ui->albumWidget;

    // Hacer visible el AlbumWidget
    if (mAlbumWidget) {
        mAlbumWidget->setVisible(true);
        mAlbumWidget->show();
    }


    // Conectar señal de AlbumWidget cuando se activa una imagen
    if (mAlbumWidget) {
        connect(mAlbumWidget, &AlbumWidget::pictureActivated,
                this, &GalleryWidget::onPictureActivated);
    }
    qDebug() << "AlbumWidget geometry:" << mAlbumWidget->geometry();
    qDebug() << "AlbumWidget visible:" << mAlbumWidget->isVisible();
    qDebug() << "AlbumWidget size:" << mAlbumWidget->size();
}

GalleryWidget::~GalleryWidget()
{
    delete ui;
}

// ---------------------
// Álbum
// ---------------------
void GalleryWidget::setAlbumModel(AlbumModel* model)
{
    qDebug() << "=== GalleryWidget::setAlbumModel ===";
    qDebug() << "model:" << model;
    qDebug() << "mAlbumListWidget:" << mAlbumListWidget;
    qDebug() << "mAlbumWidget:" << mAlbumWidget;

    if (!mAlbumListWidget) return;

    // Asignar modelo
    mAlbumListWidget->setModel(model);

    // Aplicar selection model si ya existe
    if (mAlbumSelectionModel)
        mAlbumListWidget->setSelectionModel(mAlbumSelectionModel);

    // AGREGAR ESTO - MUY IMPORTANTE:
    if (mAlbumWidget) {
        qDebug() << "Llamando a mAlbumWidget->setAlbumModel";
        mAlbumWidget->setAlbumModel(model);

        // Si ya existe el selectionModel, asignarlo también
        if (mAlbumSelectionModel) {
            mAlbumWidget->setAlbumSelectionModel(mAlbumSelectionModel);
        }
    }
}

void GalleryWidget::setAlbumSelectionModel(QItemSelectionModel* selectionModel)
{
    qDebug() << "=== GalleryWidget::setAlbumSelectionModel ===";
    qDebug() << "selectionModel:" << selectionModel;
    qDebug() << "mAlbumWidget:" << mAlbumWidget;

    mAlbumSelectionModel = selectionModel;

    // AGREGAR: Asignar al AlbumListWidget también
    if (mAlbumListWidget) {
        qDebug() << "Asignando selectionModel a AlbumListWidget";
        mAlbumListWidget->setSelectionModel(selectionModel);
    }

    // Asignar al AlbumWidget
    if (mAlbumWidget) {
        qDebug() << "Llamando a mAlbumWidget->setAlbumSelectionModel";
        mAlbumWidget->setAlbumSelectionModel(selectionModel);
    } else {
        qDebug() << "ERROR: mAlbumWidget es NULL!";
    }
}

// ---------------------
// Imágenes
// ---------------------
void GalleryWidget::setPictureModel(ThumbnailProxyModel* model)
{
    if (!mAlbumWidget) return;

    // Asignar modelo al AlbumWidget
    mAlbumWidget->setPictureModel(model);

    // Aplicar selection model si ya existía
    if (mPictureSelectionModel)
        mAlbumWidget->setSelectionModel(mPictureSelectionModel);
}

void GalleryWidget::setPictureSelectionModel(QItemSelectionModel* selectionModel)
{
    mPictureSelectionModel = selectionModel;

}

// ---------------------
// Slot interno para reenviar la señal
// ---------------------
void GalleryWidget::onPictureActivated(const QModelIndex& index)
{
    emit pictureActivated(index); // esto es lo que espera MainWindow
}



/*
 * El Problema:

 * En GalleryWidget::setAlbumModel(),
 *  se intentaba asignar el selectionModel al AlbumListWidget,
 *  pero en ese momento mAlbumSelectionModel todavía era nullptr
 *  porque se asignaba después desde MainWindow
 *
 * La Solución:
 *
 * Mover la asignación del selectionModel a AlbumListWidget
 * dentro de GalleryWidget::setAlbumSelectionModel():
 *
 * Funcionamiento
 * El selectionModel captura el cambio
 * Se emite la señal selectionChanged
 * Se llama a loadAlbum() en AlbumWidget
 * Se actualiza el título y se cargan las fotos
 */




