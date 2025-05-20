#include "albumimageswidget.h"
#include <QDir>
#include <QPixmap>
#include <QFile>
#include <QFileInfo>
#include <QMenu>
#include <QContextMenuEvent>
#include <QMessageBox>
#include <QImage>
#include <QVBoxLayout>
#include <QProcess>
#include <QOperatingSystemVersion>
#include <QFileDialog>


AlbumImagesWidget::AlbumImagesWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    label = new QLabel("Seleccione un álbum", this);
    mainLayout->addWidget(label);

    imagesContainer = new QWidget(this);
    imagesLayout = new QGridLayout(imagesContainer);
    imagesContainer->setLayout(imagesLayout);

    mainLayout->addWidget(imagesContainer);

    btnExportZip = new QPushButton("Exportar álbum a ZIP", this);
    mainLayout->addWidget(btnExportZip);
    connect(btnExportZip, &QPushButton::clicked, this, [this]() {
        if (currentAlbumPath.isEmpty()) {
            QMessageBox::warning(this, "Error", "No hay ningún álbum cargado.");
            return;
        }

        QString zipFilePath = QFileDialog::getSaveFileName(this, "Guardar archivo ZIP", currentAlbumPath + ".zip", "Archivos ZIP (*.zip)");
        if (!zipFilePath.isEmpty()) {
            exportAlbumAsZip(currentAlbumPath, zipFilePath);
        }
    });
}

void AlbumImagesWidget::loadImagesForAlbum(const QString &albumPath)
{
    currentAlbumPath = albumPath;
    label->setText("Imágenes del álbum: " + albumPath);

    // Limpia el layout actual
    QLayoutItem *child;
    while ((child = imagesLayout->takeAt(0)) != nullptr) {
        if (child->widget()) {
            child->widget()->deleteLater();
        }
        delete child;
    }

    QDir dir(albumPath);
    if (!dir.exists())
        return;

    QStringList imageFiles = dir.entryList(QStringList() << "*.png" << "*.jpg" << "*.jpeg" << "*.bmp", QDir::Files);

    int row = 0, col = 0;
    const int maxCols = 4;

    for (const QString &imageFile : imageFiles) {
        QString fullPath = dir.absoluteFilePath(imageFile);

        QLabel *imageLabel = new QLabel();
        imageLabel->setFixedSize(150, 150);
        imageLabel->setScaledContents(true);

        QPixmap pix(fullPath);
        imageLabel->setPixmap(pix.scaled(imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

        imagesLayout->addWidget(imageLabel, row, col);

        col++;
        if (col >= maxCols) {
            col = 0;
            row++;
        }

        // Para poder saber qué imagen está seleccionada en el menú contextual:
        // Instalamos filtro de eventos para clic derecho en cada etiqueta de imagen
        imageLabel->setProperty("imagePath", fullPath);
        imageLabel->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(imageLabel, &QLabel::customContextMenuRequested, this, [this, imageLabel](const QPoint &pos){
            selectedImagePath = imageLabel->property("imagePath").toString();
            QContextMenuEvent event(QContextMenuEvent::Mouse, pos, imageLabel->mapToGlobal(pos));
            this->contextMenuEvent(&event);
        });
    }
}

bool AlbumImagesWidget::convertImageFormat(const QString &originalFilePath, const QString &newExtension) {
    QImage image(originalFilePath);
    if (image.isNull()) return false;

    QString newFilePath = originalFilePath;
    int lastDotIndex = newFilePath.lastIndexOf('.');
    if (lastDotIndex != -1) {
        newFilePath = newFilePath.left(lastDotIndex);
    }
    newFilePath += newExtension;

    return image.save(newFilePath);
}

bool AlbumImagesWidget::changeImageResolution(const QString &filePath, int newWidth, int newHeight) {
    QImage image(filePath);
    if (image.isNull()) return false;

    QImage resized = image.scaled(newWidth, newHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    return resized.save(filePath);
}

void AlbumImagesWidget::contextMenuEvent(QContextMenuEvent *event) {
    if (selectedImagePath.isEmpty()) {
        QWidget::contextMenuEvent(event);
        return;
    }

    QMenu menu(this);

    QAction *toPng = menu.addAction("Convertir a PNG");
    QAction *toJpg = menu.addAction("Convertir a JPG");
    QAction *toJpeg = menu.addAction("Convertir a JPEG");
    menu.addSeparator();
    QAction *resize1024x768 = menu.addAction("Cambiar resolución a 1024x768");
    QAction *resize800x600 = menu.addAction("Cambiar resolución a 800x600");

    QAction *selectedAction = menu.exec(event->globalPos());
    if (!selectedAction) return;

    if (selectedAction == toPng) {
        if (convertImageFormat(selectedImagePath, ".png")) {
            QMessageBox::information(this, "Éxito", "Imagen convertida a PNG.");
            loadImagesForAlbum(currentAlbumPath);
        } else {
            QMessageBox::warning(this, "Error", "No se pudo convertir la imagen.");
        }
    } else if (selectedAction == toJpg) {
        if (convertImageFormat(selectedImagePath, ".jpg")) {
            QMessageBox::information(this, "Éxito", "Imagen convertida a JPG.");
            loadImagesForAlbum(currentAlbumPath);
        } else {
            QMessageBox::warning(this, "Error", "No se pudo convertir la imagen.");
        }
    } else if (selectedAction == toJpeg) {
        if (convertImageFormat(selectedImagePath, ".jpeg")) {
            QMessageBox::information(this, "Éxito", "Imagen convertida a JPEG.");
            loadImagesForAlbum(currentAlbumPath);
        } else {
            QMessageBox::warning(this, "Error", "No se pudo convertir la imagen.");
        }
    } else if (selectedAction == resize1024x768) {
        if (changeImageResolution(selectedImagePath, 1024, 768)) {
            QMessageBox::information(this, "Éxito", "Resolución cambiada a 1024x768.");
            loadImagesForAlbum(currentAlbumPath);
        } else {
            QMessageBox::warning(this, "Error", "No se pudo cambiar la resolución.");
        }
    } else if (selectedAction == resize800x600) {
        if (changeImageResolution(selectedImagePath, 800, 600)) {
            QMessageBox::information(this, "Éxito", "Resolución cambiada a 800x600.");
            loadImagesForAlbum(currentAlbumPath);
        } else {
            QMessageBox::warning(this, "Error", "No se pudo cambiar la resolución.");
        }
    }
}
void AlbumImagesWidget::exportAlbumAsZip(const QString &albumPath, const QString &zipPath)
{
    QProcess process;

    if (QOperatingSystemVersion::currentType() == QOperatingSystemVersion::Windows) {
        // Usamos PowerShell Compress-Archive (Windows 10+)
        QString command = QString("powershell Compress-Archive -Path \"%1\\*\" -DestinationPath \"%2\"")
                              .arg(albumPath)
                              .arg(zipPath);

        process.start(command);
    } else {
        // Linux/macOS: comando zip -r
        // Nota: zip debe estar instalado en el sistema
        QString command = QString("zip -r \"%1\" \"%2\"").arg(zipPath).arg(albumPath);
        process.start(command);
    }

    process.waitForFinished(-1);

    if (process.exitCode() == 0) {
        QMessageBox::information(this, "Éxito", "Álbum exportado como ZIP.");
    } else {
        QString errorOutput = process.readAllStandardError();
        QMessageBox::warning(this, "Error al exportar ZIP",
                             "No se pudo exportar el álbum.\nDetalles:\n" + errorOutput);
    }
}
