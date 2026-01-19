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

/**
 * Constructor de MainWindow
 * @param parent Widget padre
 *
 * MainWindow actúa como el controlador principal de la aplicación:
 * - Crea y gestiona los modelos de datos
 * - Inicializa los widgets principales
 * - Centraliza los QItemSelectionModel
 * - Controla la navegación entre vistas (galería ↔ imagen)
 */
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    /**
     * =========================
     * CONFIGURACIÓN DE VENTANA
     * =========================
     */

    // Establece el título de la ventana principal (traducible)
    setWindowTitle(tr("Gallery"));

    // Establece el icono de la aplicación desde los recursos Qt
    setWindowIcon(QIcon(":/icons/logo.png"));

    /**
     * =========================
     * STACKED WIDGET (NAVEGACIÓN)
     * =========================
     */

    // QStackedWidget permite cambiar entre vistas sin destruirlas
    mStackedWidget = new QStackedWidget(this);

    // Widget principal de galería (lista de álbumes + thumbnails)
    mGalleryWidget = new GalleryWidget(this);

    // Widget de visualización de una imagen individual
    mPictureWidget = new PictureWidget(this);

    /**
     * =========================
     * CREACIÓN DE MODELOS
     * =========================
     */

    // Modelo que contiene la lista de álbumes
    AlbumModel* albumModel = new AlbumModel(this);

    // Modelo de selección para los álbumes (compartido entre vistas)
    QItemSelectionModel* albumSelectionModel =
        new QItemSelectionModel(albumModel, this);

    // Modelo de imágenes dependiente del álbum seleccionado
    PictureModel* pictureModel =
        new PictureModel(*albumModel, this);

    // Proxy model para mostrar miniaturas (thumbnails)
    ThumbnailProxyModel* thumbnailModel = new ThumbnailProxyModel(this);
    thumbnailModel->setSourceModel(pictureModel);

    // Modelo de selección para las imágenes (basado en el proxy model)
    QItemSelectionModel* pictureSelectionModel =
        new QItemSelectionModel(thumbnailModel, this);

    /**
     * =========================
     * ASIGNACIÓN DE MODELOS
     * =========================
     */

    qDebug() << "=== MainWindow: Asignando modelos ===";

    // Asigna el modelo de álbumes a GalleryWidget
    mGalleryWidget->setAlbumModel(albumModel);
    qDebug() << "AlbumModel asignado";

    // Asigna el modelo de selección de álbumes
    mGalleryWidget->setAlbumSelectionModel(albumSelectionModel);
    qDebug() << "AlbumSelectionModel asignado";

    // Asigna el modelo de imágenes (thumbnails)
    mGalleryWidget->setPictureModel(thumbnailModel);
    qDebug() << "PictureModel asignado";

    // Asigna el modelo de selección de imágenes
    mGalleryWidget->setPictureSelectionModel(pictureSelectionModel);
    qDebug() << "PictureSelectionModel asignado";

    // Asigna los modelos directamente al PictureWidget
    mPictureWidget->setModel(thumbnailModel);
    mPictureWidget->setSelectionModel(pictureSelectionModel);

    /**
     * =========================
     * CONEXIONES ENTRE VISTAS
     * =========================
     */

    // Cuando se activa una imagen en la galería → mostrar PictureWidget
    connect(mGalleryWidget, &GalleryWidget::pictureActivated,
            this, &MainWindow::displayPicture);

    // Botón "volver" desde PictureWidget → volver a la galería
    connect(mPictureWidget, &PictureWidget::backToGallery,
            this, &MainWindow::displayGallery);

    /**
     * =========================
     * CONFIGURACIÓN DEL STACK
     * =========================
     */

    // Añade los widgets al stacked widget
    mStackedWidget->addWidget(mGalleryWidget);
    mStackedWidget->addWidget(mPictureWidget);

    // Vista inicial: galería
    displayGallery();

    // Establece el stacked widget como contenido central
    setCentralWidget(mStackedWidget);

    /**
     * =========================
     * ESTILO VISUAL
     * =========================
     */

    // Permite que el fondo personalizado sea visible
    this->setAutoFillBackground(true);

    // Aplica un gradiente vertical como fondo de la ventana
    this->setStyleSheet(
        "background: qlineargradient("
        "x1:0, y1:0, x2:0, y2:1,"
        "stop:0 #ffffff,"
        "stop:1 #ffa500);"
        );
}

/**
 * Destructor de MainWindow
 *
 * No es necesario liberar manualmente los widgets,
 * ya que Qt se encarga mediante la jerarquía de padres.
 */
MainWindow::~MainWindow()
{
}

/**
 * Muestra la vista de galería
 *
 * Cambia el QStackedWidget para mostrar GalleryWidget.
 */
void MainWindow::displayGallery()
{
    mStackedWidget->setCurrentWidget(mGalleryWidget);
}

/**
 * Muestra una imagen individual
 * @param index Índice del modelo correspondiente a la imagen seleccionada
 *
 * Este método:
 * 1. Establece la imagen activa en PictureWidget
 * 2. Cambia la vista actual del QStackedWidget
 */
void MainWindow::displayPicture(const QModelIndex& index)
{
    qDebug() << "=== MainWindow::displayPicture ===";
    qDebug() << "Index row:" << index.row();
    qDebug() << "Index válido:" << index.isValid();

    // Establece la imagen actual antes de cambiar de vista
    mPictureWidget->setCurrentIndex(index);

    // Cambia a la vista de imagen individual
    mStackedWidget->setCurrentWidget(mPictureWidget);
}
