#include "gallerywidget.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QIcon>

GalleryWidget::GalleryWidget(QWidget *parent)
    : QWidget(parent)
{
    setWindowIcon(QIcon(":/icons/icono.png"));
    mainLayout = new QVBoxLayout(this);

    btnSelectBaseFolder = new QPushButton("", this);
    btnCreateAlbum = new QPushButton("", this);
    btnDeleteAlbum = new QPushButton("", this);
    btnAddImage = new QPushButton("", this);
    btnDeleteImage = new QPushButton("", this);

    albumListWidget = new AlbumListWidget(this);
    albumImagesWidget = new AlbumImagesWidget(this);

    // Layout botones arriba
    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(btnSelectBaseFolder);
    buttonsLayout->addWidget(btnCreateAlbum);
    buttonsLayout->addWidget(btnDeleteAlbum);
    buttonsLayout->addWidget(btnAddImage);
    buttonsLayout->addWidget(btnDeleteImage);

    mainLayout->addLayout(buttonsLayout);
    mainLayout->addWidget(albumListWidget);
    mainLayout->addWidget(albumImagesWidget);

    //Imagenes de botones
    btnSelectBaseFolder->setIcon(QIcon(":/icons/select_folder.png"));
    btnAddImage->setIcon(QIcon(":/icons/add_photo.png"));
    btnDeleteImage->setIcon(QIcon(":/icons/delete_photo.png"));
    btnCreateAlbum->setIcon(QIcon(":/icons/add_album.png"));
    btnDeleteAlbum->setIcon(QIcon(":/icons/delete_album.png"));




    // Conexiones
    connect(btnSelectBaseFolder, &QPushButton::clicked, this, &GalleryWidget::selectBaseFolder);
    connect(albumListWidget, &AlbumListWidget::albumSelected, this, &GalleryWidget::onAlbumSelected);
    connect(btnCreateAlbum, &QPushButton::clicked, this, &GalleryWidget::createAlbum);
    connect(btnDeleteAlbum, &QPushButton::clicked, this, &GalleryWidget::deleteAlbum);
    connect(btnAddImage, &QPushButton::clicked, this, &GalleryWidget::addImageToAlbum);
    connect(btnDeleteImage, &QPushButton::clicked, this, &GalleryWidget::deleteImageFromAlbum);
    connect(albumListWidget, &AlbumListWidget::albumSelected,
            this, &GalleryWidget::onAlbumSelected);

    connect(btnSelectBaseFolder, &QPushButton::clicked,
            this, &GalleryWidget::selectBaseFolder);
}

void GalleryWidget::selectBaseFolder()
{
    QString folder = QFileDialog::getExistingDirectory(this, "Selecciona la carpeta base de álbumes");
    if (folder.isEmpty())
        return;

    currentBaseFolder = folder;
    albumListWidget->loadAlbumsFromFolder(currentBaseFolder);
    albumImagesWidget->loadImagesForAlbum(""); // limpia imágenes
    currentAlbumName.clear();
    currentAlbumPath.clear();
}

void GalleryWidget::onAlbumSelected(const QString &albumName)
{
    updateCurrentAlbum(albumName);
    albumImagesWidget->loadImagesForAlbum(currentAlbumPath);
}

void GalleryWidget::updateCurrentAlbum(const QString &albumName)
{
    currentAlbumName = albumName;
    currentAlbumPath = currentBaseFolder + "/" + currentAlbumName;
}

void GalleryWidget::createAlbum()
{
    if (currentBaseFolder.isEmpty()) {
        QMessageBox::warning(this, "Error", "Selecciona primero la carpeta base.");
        return;
    }

    bool ok;
    QString newAlbumName = QInputDialog::getText(this, "Crear álbum",
                                                 "Nombre del nuevo álbum:",
                                                 QLineEdit::Normal,
                                                 QString(), &ok);
    if (!ok || newAlbumName.isEmpty())
        return;

    albumListWidget->createAlbum(newAlbumName);
}

void GalleryWidget::deleteAlbum()
{
    albumListWidget->deleteSelectedAlbum();
    // Refrescar la lista y limpiar imágenes si borró el álbum seleccionado
    albumListWidget->loadAlbumsFromFolder(currentBaseFolder);
    albumImagesWidget->loadImagesForAlbum("");
    currentAlbumName.clear();
    currentAlbumPath.clear();
}

void GalleryWidget::addImageToAlbum()
{
    if (currentAlbumPath.isEmpty()) {
        QMessageBox::warning(this, "Error", "Selecciona primero un álbum.");
        return;
    }

    QString imagePath = QFileDialog::getOpenFileName(this, "Selecciona imagen para añadir",
                                                     QString(), "Imágenes (*.png *.jpg *.jpeg *.bmp)");
    if (imagePath.isEmpty())
        return;

    QFileInfo fileInfo(imagePath);
    QString destPath = currentAlbumPath + "/" + fileInfo.fileName();

    if (!QFile::copy(imagePath, destPath)) {
        QMessageBox::warning(this, "Error", "No se pudo copiar la imagen al álbum.");
        return;
    }

    albumImagesWidget->loadImagesForAlbum(currentAlbumPath);
}

void GalleryWidget::deleteImageFromAlbum()
{
    if (currentAlbumPath.isEmpty()) {
        QMessageBox::warning(this, "Error", "Selecciona primero un álbum.");
        return;
    }

    // Para simplificar, abrimos un diálogo para seleccionar la imagen a borrar
    QString imageToDelete = QFileDialog::getOpenFileName(this, "Selecciona imagen para borrar",
                                                         currentAlbumPath,
                                                         "Imágenes (*.png *.jpg *.jpeg *.bmp)");
    if (imageToDelete.isEmpty())
        return;

    if (!QFile::remove(imageToDelete)) {
        QMessageBox::warning(this, "Error", "No se pudo borrar la imagen.");
        return;
    }

    albumImagesWidget->loadImagesForAlbum(currentAlbumPath);
}
