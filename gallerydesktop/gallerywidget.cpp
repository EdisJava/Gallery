#include "gallerywidget.h"
#include "ui_gallerywidget.h"
#include "albumlistwidget.h"
#include "albumwidget.h"
#include "albummodel.h"
#include "picturemodel.h"
#include "thumbnailproxymodel.h"
#include <QItemSelectionModel>

/**
 * Constructor de GalleryWidget
 * @param parent Widget padre dentro de la jerarquía de Qt
 *
 * GalleryWidget actúa como un widget contenedor y coordinador:
 * - Contiene la lista de álbumes (AlbumListWidget)
 * - Contiene el visor de álbumes e imágenes (AlbumWidget)
 * - Centraliza los modelos y selection models para mantener
 *   sincronizadas todas las vistas.
 */
GalleryWidget::GalleryWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::GalleryWidget)
{
    // Inicializa la interfaz creada con Qt Designer
    ui->setupUi(this);

    // Obtiene las referencias a los widgets hijos definidos en el .ui
    mAlbumListWidget = ui->albumListWidget;
    mAlbumWidget = ui->albumWidget;

    // Asegura que el AlbumWidget esté visible
    // (útil para debug cuando se trabaja con layouts)
    if (mAlbumWidget) {
        mAlbumWidget->setVisible(true);
        mAlbumWidget->show();
    }

    // Conecta la activación de una imagen desde AlbumWidget
    // con el slot local que reenviará la señal hacia MainWindow
    if (mAlbumWidget) {
        connect(mAlbumWidget, &AlbumWidget::pictureActivated,
                this, &GalleryWidget::onPictureActivated);
    }

    // Información de debug para verificar geometría y visibilidad
    qDebug() << "AlbumWidget geometry:" << mAlbumWidget->geometry();
    qDebug() << "AlbumWidget visible:" << mAlbumWidget->isVisible();
    qDebug() << "AlbumWidget size:" << mAlbumWidget->size();
}

/**
 * Destructor de GalleryWidget
 *
 * Libera la memoria asociada a la interfaz gráfica.
 * Qt se encarga automáticamente de destruir los widgets hijos.
 */
GalleryWidget::~GalleryWidget()
{
    delete ui;
}

/**
 * Asigna el modelo de álbumes a los widgets correspondientes
 * @param model Puntero al AlbumModel que contiene los álbumes
 *
 * Este método:
 * - Asigna el modelo a AlbumListWidget
 * - Asigna el mismo modelo a AlbumWidget
 * - Sincroniza el modelo de selección si ya existe
 */
void GalleryWidget::setAlbumModel(AlbumModel* model)
{
    qDebug() << "=== GalleryWidget::setAlbumModel ===";
    qDebug() << "model:" << model;
    qDebug() << "mAlbumListWidget:" << mAlbumListWidget;
    qDebug() << "mAlbumWidget:" << mAlbumWidget;

    // Seguridad: si no existe la lista de álbumes, no continuar
    if (!mAlbumListWidget) return;

    // Asigna el modelo de datos a la lista de álbumes
    mAlbumListWidget->setModel(model);

    // Si el selection model ya existe, se asigna también
    if (mAlbumSelectionModel)
        mAlbumListWidget->setSelectionModel(mAlbumSelectionModel);

    // Asignación del modelo también al visor de álbumes
    if (mAlbumWidget) {
        qDebug() << "Llamando a mAlbumWidget->setAlbumModel";
        mAlbumWidget->setAlbumModel(model);

        // Comparte el mismo modelo de selección entre vistas
        if (mAlbumSelectionModel) {
            mAlbumWidget->setAlbumSelectionModel(mAlbumSelectionModel);
        }
    }
}

/**
 * Asigna el modelo de selección de álbumes
 * @param selectionModel QItemSelectionModel compartido
 *
 * Este método es CLAVE para sincronizar la selección:
 * - Lista de álbumes
 * - Widget de visualización de álbum
 *
 * Debe llamarse una vez que el selectionModel ha sido creado
 * (normalmente desde MainWindow).
 */
void GalleryWidget::setAlbumSelectionModel(QItemSelectionModel* selectionModel)
{
    qDebug() << "=== GalleryWidget::setAlbumSelectionModel ===";
    qDebug() << "selectionModel:" << selectionModel;
    qDebug() << "mAlbumWidget:" << mAlbumWidget;

    // Almacena el selection model para uso posterior
    mAlbumSelectionModel = selectionModel;

    // Asigna el selection model a la lista de álbumes
    if (mAlbumListWidget) {
        qDebug() << "Asignando selectionModel a AlbumListWidget";
        mAlbumListWidget->setSelectionModel(selectionModel);
    }

    // Asigna el mismo selection model al AlbumWidget
    if (mAlbumWidget) {
        qDebug() << "Llamando a mAlbumWidget->setAlbumSelectionModel";
        mAlbumWidget->setAlbumSelectionModel(selectionModel);
    } else {
        qDebug() << "ERROR: mAlbumWidget es NULL!";
    }
}

/**
 * Asigna el modelo de imágenes (thumbnails) al AlbumWidget
 * @param model Puntero al ThumbnailProxyModel
 *
 * El proxy model se encarga de:
 * - Adaptar el modelo base de imágenes
 * - Gestionar tamaños y visualización de miniaturas
 */
void GalleryWidget::setPictureModel(ThumbnailProxyModel* model)
{
    if (!mAlbumWidget) return;

    // Asigna el modelo de imágenes al AlbumWidget
    mAlbumWidget->setPictureModel(model);

    // Aplica el modelo de selección si ya existía
    if (mPictureSelectionModel)
        mAlbumWidget->setSelectionModel(mPictureSelectionModel);
}

/**
 * Asigna el modelo de selección de imágenes
 * @param selectionModel QItemSelectionModel para las imágenes
 *
 * Se almacena para ser aplicado posteriormente cuando el
 * modelo de imágenes esté disponible.
 */
void GalleryWidget::setPictureSelectionModel(QItemSelectionModel* selectionModel)
{
    mPictureSelectionModel = selectionModel;
}

/**
 * Slot que captura la activación de una imagen
 * @param index Índice del modelo correspondiente a la imagen activada
 *
 * Reemite la señal hacia el exterior (MainWindow),
 * actuando como intermediario entre widgets.
 */
void GalleryWidget::onPictureActivated(const QModelIndex& index)
{
    emit pictureActivated(index); // Señal esperada por MainWindow
}

/*
 * =========================
 * PROBLEMA Y SOLUCIÓN
 * =========================
 *
 * PROBLEMA:
 *
 * En GalleryWidget::setAlbumModel() se intentaba asignar
 * el selectionModel al AlbumListWidget, pero en ese momento
 * mAlbumSelectionModel todavía era nullptr, ya que:
 *
 * - El selectionModel se crea en MainWindow
 * - Se asigna DESPUÉS de llamar a setAlbumModel()
 *
 * SOLUCIÓN:
 *
 * Mover la asignación del selectionModel a:
 * GalleryWidget::setAlbumSelectionModel()
 *
 * FUNCIONAMIENTO FINAL:
 *
 * 1. MainWindow crea el QItemSelectionModel
 * 2. GalleryWidget recibe el selectionModel
 * 3. Se asigna a AlbumListWidget y AlbumWidget
 * 4. selectionChanged() se emite correctamente
 * 5. AlbumWidget carga el álbum seleccionado
 * 6. Se actualizan título e imágenes
 *
 * Resultado: sincronización perfecta entre vistas
 */


