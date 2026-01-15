#include "picturewidget.h"
#include "qfileinfo.h"
#include "ui_picturewidget.h"
#include "thumbnailproxymodel.h"
#include <QItemSelection>
#include <QMessageBox>
#include <QFile>
#include "PictureModel.h"

PictureWidget::PictureWidget(QWidget *parent)
    : QWidget(parent),
    ui(new Ui::PictureWidget),
    mModel(nullptr),
    mSelectionModel(nullptr)
{
    ui->setupUi(this);
    ui->namelabel->clear(); // elimina cualquier texto base

    // Conectar botones
    connect(ui->deleteButton, &QPushButton::clicked, this, &PictureWidget::deletePicture);
    connect(ui->backButton, &QPushButton::clicked, this, &PictureWidget::backToGallery);
    connect(ui->previousButton, &QPushButton::clicked, this, [this] {
        if (!mSelectionModel) return;
        int row = mSelectionModel->currentIndex().row();
        if (row > 0) {
            mSelectionModel->setCurrentIndex(mModel->index(row - 1, 0),
                                             QItemSelectionModel::ClearAndSelect);
        }
    });
    connect(ui->nextButton, &QPushButton::clicked, this, [this] {
        if (!mSelectionModel) return;
        int row = mSelectionModel->currentIndex().row();
        if (mModel && row < mModel->rowCount() - 1) {
            mSelectionModel->setCurrentIndex(mModel->index(row + 1, 0),
                                             QItemSelectionModel::ClearAndSelect);
        }
    });
}

void PictureWidget::setCurrentIndex(const QModelIndex& index)
{
    qDebug() << "=== PictureWidget::setCurrentIndex ===";
    qDebug() << "Index válido:" << index.isValid();
    qDebug() << "Index row:" << index.row();

    if (!index.isValid() || !mSelectionModel) {
        qDebug() << "Index inválido o no hay selectionModel";
        return;
    }

    // Establecer la selección actual
    mSelectionModel->setCurrentIndex(index, QItemSelectionModel::ClearAndSelect);

    // Cargar la imagen
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

void PictureWidget::updatePicturePixmap()
{
    if (mPixmap.isNull()) {
        ui->picturelabel->clear();  // limpia la etiqueta si no hay imagen
        ui->namelabel->clear();
        return;
    }

    // Escala la imagen para que encaje en el label manteniendo proporción
    ui->picturelabel->setPixmap(
        mPixmap.scaled(ui->picturelabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation)
        );

    // Actualizar nombre del archivo según la selección actual
    if (!mSelectionModel || !mSelectionModel->currentIndex().isValid()) {
        ui->namelabel->clear(); // si no hay selección
        return;
    }

    QModelIndex currentIndex = mSelectionModel->currentIndex();
    QString filePath = mModel->data(currentIndex, PictureModel::FilePathRole).toString();
    QFileInfo fileInfo(filePath);

    // Poner solo el nombre del archivo en el label
    ui->namelabel->setText(fileInfo.fileName());
}

PictureWidget::~PictureWidget()
{
    delete ui;
}

void PictureWidget::setModel(ThumbnailProxyModel* model)
{
    mModel = model;

    if (mModel) {
        connect(mModel, &QAbstractItemModel::dataChanged,
                this, [this](const QModelIndex& ) {
                    updatePicturePixmap();
                });
    }
}

void PictureWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);  // Llama al método base
    updatePicturePixmap();        // Actualiza la imagen escalada
}

void PictureWidget::setSelectionModel(QItemSelectionModel* selectionModel)
{
    if (mSelectionModel) {
        disconnect(mSelectionModel, &QItemSelectionModel::selectionChanged,
                   this, &PictureWidget::loadPicture);
    }

    mSelectionModel = selectionModel;

    if (mSelectionModel) {
        connect(mSelectionModel, &QItemSelectionModel::selectionChanged,
                this, &PictureWidget::loadPicture);
    }
}

void PictureWidget::deletePicture()
{
    if (!mModel || !mSelectionModel) return;

    QModelIndex currentIndex = mSelectionModel->currentIndex();
    if (!currentIndex.isValid()) return;

    // Confirmar eliminación
    auto reply = QMessageBox::question(this, "Eliminar imagen",
         "¿Estás seguro de que quieres eliminar esta imagen?");
    if (reply != QMessageBox::Yes) return;

    // Eliminamos del modelo
    PictureModel* pictureModel = mModel->pictureModel();
    if (pictureModel) {
        pictureModel->removePicture(currentIndex.row());
    }

    // Limpiamos la vista
    mPixmap = QPixmap();
    updatePicturePixmap();

    // Selecciona la siguiente imagen si existe
    int row = currentIndex.row();
    int count = mModel->rowCount();
    if (row < count) {
        mSelectionModel->setCurrentIndex(mModel->index(row, 0),
                                         QItemSelectionModel::ClearAndSelect);
    } else if (count > 0) {
        mSelectionModel->setCurrentIndex(mModel->index(count - 1, 0),
                                         QItemSelectionModel::ClearAndSelect);
    }
}

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
    updatePicturePixmap();
}


