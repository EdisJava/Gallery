#include "picturewidget.h"
#include "qfileinfo.h"
#include "ui_picturewidget.h"
#include "thumbnailproxymodel.h"
#include <QItemSelection>
#include <QMessageBox>
#include <QFile>
#include "PictureModel.h"

/**
 * Constructor de PictureWidget
 * @param parent Widget padre
 *
 * PictureWidget se encarga de:
 * - Mostrar una imagen individual a gran tamaño
 * - Permitir navegación (anterior / siguiente)
 * - Mostrar el nombre del archivo
 * - Eliminar la imagen seleccionada
 */
PictureWidget::PictureWidget(QWidget *parent)
    : QWidget(parent),
    ui(new Ui::PictureWidget),
    mModel(nullptr),
    mSelectionModel(nullptr)
{
    // Inicializa la interfaz gráfica
    ui->setupUi(this);

    // Limpia el label del nombre al iniciar
    ui->namelabel->clear();

    /**
     * =========================
     * CONEXIONES DE BOTONES
     * =========================
     */

    // Botón eliminar imagen
    connect(ui->deleteButton, &QPushButton::clicked,
            this, &PictureWidget::deletePicture);

    // Botón volver a la galería
    connect(ui->backButton, &QPushButton::clicked,
            this, &PictureWidget::backToGallery);

    // Botón imagen anterior
    connect(ui->previousButton, &QPushButton::clicked, this, [this] {
        if (!mSelectionModel) return;

        int row = mSelectionModel->currentIndex().row();
        if (row > 0) {
            mSelectionModel->setCurrentIndex(
                mModel->index(row - 1, 0),
                QItemSelectionModel::ClearAndSelect
                );
        }
    });

    // Botón imagen siguiente
    connect(ui->nextButton, &QPushButton::clicked, this, [this] {
        if (!mSelectionModel) return;

        int row = mSelectionModel->currentIndex().row();
        if (mModel && row < mModel->rowCount() - 1) {
            mSelectionModel->setCurrentIndex(
                mModel->index(row + 1, 0),
                QItemSelectionModel::ClearAndSelect
                );
        }
    });
}

/**
 * Establece el índice actual de imagen
 * @param index Índice del modelo correspondiente a la imagen
 *
 * Este método:
 * - Fuerza la selección en el selectionModel
 * - Obtiene la imagen desde el modelo
 * - Actualiza la vista
 */
void PictureWidget::setCurrentIndex(const QModelIndex& index)
{
    qDebug() << "=== PictureWidget::setCurrentIndex ===";
    qDebug() << "Index válido:" << index.isValid();
    qDebug() << "Index row:" << index.row();

    // Validaciones básicas
    if (!index.isValid() || !mSelectionModel) {
        qDebug() << "Index inválido o no hay selectionModel";
        return;
    }

    // Fuerza la selección actual
    mSelectionModel->setCurrentIndex(
        index,
        QItemSelectionModel::ClearAndSelect
        );

    // Carga la imagen desde el modelo
    if (mModel) {
        QVariant decoration = mModel->data(index, Qt::DecorationRole);
        if (decoration.canConvert<QPixmap>()) {
            mPixmap = qvariant_cast<QPixmap>(decoration);
            updatePicturePixmap();
            qDebug() << "Imagen cargada correctamente";
        } else {
            qDebug() << "ERROR: No se pudo convertir a QPixmap";
        }
    }
}

/**
 * Actualiza la imagen mostrada en pantalla
 *
 * Se encarga de:
 * - Escalar la imagen al tamaño del widget
 * - Mantener la proporción
 * - Actualizar el nombre del archivo
 */
void PictureWidget::updatePicturePixmap()
{
    // Si no hay imagen, limpiar la vista
    if (mPixmap.isNull()) {
        ui->picturelabel->clear();
        ui->namelabel->clear();
        return;
    }

    // Escala la imagen manteniendo proporción
    ui->picturelabel->setPixmap(
        mPixmap.scaled(
            ui->picturelabel->size(),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
            )
        );

    // Verifica que exista una selección válida
    if (!mSelectionModel || !mSelectionModel->currentIndex().isValid()) {
        ui->namelabel->clear();
        return;
    }

    // Obtiene el nombre del archivo desde el modelo
    QModelIndex currentIndex = mSelectionModel->currentIndex();
    QString filePath = mModel->data(
                                 currentIndex,
                                 PictureModel::FilePathRole
                                 ).toString();

    QFileInfo fileInfo(filePath);

    // Muestra solo el nombre del archivo
    ui->namelabel->setText(fileInfo.fileName());
}

/**
 * Destructor de PictureWidget
 *
 * Libera la interfaz gráfica.
 */
PictureWidget::~PictureWidget()
{
    delete ui;
}

/**
 * Asigna el modelo de imágenes (proxy)
 * @param model ThumbnailProxyModel
 *
 * Escucha cambios en el modelo para actualizar la imagen si es necesario.
 */
void PictureWidget::setModel(ThumbnailProxyModel* model)
{
    mModel = model;

    if (mModel) {
        connect(mModel, &QAbstractItemModel::dataChanged,
                this, [this](const QModelIndex&) {
                    updatePicturePixmap();
                });
    }
}

/**
 * Evento de redimensionado del widget
 *
 * Se usa para reescalar la imagen cuando cambia el tamaño de la ventana.
 */
void PictureWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    updatePicturePixmap();
}

/**
 * Asigna el modelo de selección de imágenes
 * @param selectionModel QItemSelectionModel compartido
 *
 * Gestiona correctamente las conexiones para evitar duplicados.
 */
void PictureWidget::setSelectionModel(QItemSelectionModel* selectionModel)
{
    // Desconecta el selectionModel anterior si existía
    if (mSelectionModel) {
        disconnect(mSelectionModel,
                   &QItemSelectionModel::selectionChanged,
                   this,
                   &PictureWidget::loadPicture);
    }

    mSelectionModel = selectionModel;

    // Conecta el nuevo selectionModel
    if (mSelectionModel) {
        connect(mSelectionModel,
                &QItemSelectionModel::selectionChanged,
                this,
                &PictureWidget::loadPicture);
    }
}

/**
 * Carga una imagen cuando cambia la selección
 * @param selected Selección actual
 *
 * Este slot se activa automáticamente al cambiar la selección.
 */
void PictureWidget::loadPicture(const QItemSelection& selected)
{
    if (!mModel) return;
    if (selected.indexes().isEmpty()) return;

    QModelIndex index = selected.indexes().first();
    if (!index.isValid()) return;

    QVariant decoration = mModel->data(index, Qt::DecorationRole);
    if (decoration.canConvert<QPixmap>()) {
        mPixmap = qvariant_cast<QPixmap>(decoration);
        updatePicturePixmap();
    }

    // Asegura refresco visual
    updatePicturePixmap();
}

/**
 * Elimina la imagen actualmente seleccionada
 *
 * - Solicita confirmación al usuario
 * - Elimina la imagen del modelo real
 * - Actualiza la selección y la vista
 */
void PictureWidget::deletePicture()
{
    if (!mModel || !mSelectionModel) return;

    QModelIndex currentIndex = mSelectionModel->currentIndex();
    if (!currentIndex.isValid()) return;

    // Confirmación de eliminación
    auto reply = QMessageBox::question(
        this,
        "Eliminar imagen",
        "¿Estás seguro de que quieres eliminar esta imagen?"
        );
    if (reply != QMessageBox::Yes) return;

    // Elimina la imagen del modelo base
    PictureModel* pictureModel = mModel->pictureModel();
    if (pictureModel) {
        pictureModel->removePicture(currentIndex.row());
    }

    // Limpia la imagen actual
    mPixmap = QPixmap();
    updatePicturePixmap();

    // Ajusta la selección tras la eliminación
    int row = currentIndex.row();
    int count = mModel->rowCount();

    if (row < count) {
        mSelectionModel->setCurrentIndex(
            mModel->index(row, 0),
            QItemSelectionModel::ClearAndSelect
            );
    } else if (count > 0) {
        mSelectionModel->setCurrentIndex(
            mModel->index(count - 1, 0),
            QItemSelectionModel::ClearAndSelect
            );
    }
}

