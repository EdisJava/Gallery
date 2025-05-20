#include "albumwidget.h"
#include <QInputDialog>

AlbumWidget::AlbumWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    label = new QLabel("Nombre del álbum", this);
    layout->addWidget(label);

    renameButton = new QPushButton("Renombrar álbum", this);
    layout->addWidget(renameButton);

    addImageButton = new QPushButton("Añadir imagen", this);
    layout->addWidget(addImageButton);

    deleteImageButton = new QPushButton("Borrar imagen", this);
    layout->addWidget(deleteImageButton);

    connect(renameButton, &QPushButton::clicked, this, &AlbumWidget::onRenameClicked);
    connect(addImageButton, &QPushButton::clicked, this, &AlbumWidget::onAddImageClicked);
    connect(deleteImageButton, &QPushButton::clicked, this, &AlbumWidget::onDeleteImageClicked);
}

void AlbumWidget::setAlbumName(const QString &name)
{
    currentAlbumName = name;
    label->setText(name);
}

void AlbumWidget::setAlbumPath(const QString &path)
{
    currentAlbumPath = path;
}

QString AlbumWidget::albumName() const
{
    return currentAlbumName;
}

QString AlbumWidget::albumPath() const
{
    return currentAlbumPath;
}

void AlbumWidget::onRenameClicked()
{
    bool ok;
    QString newName = QInputDialog::getText(this, "Renombrar álbum",
                                            "Nuevo nombre:",
                                            QLineEdit::Normal,
                                            currentAlbumName, &ok);
    if (ok && !newName.isEmpty() && newName != currentAlbumName) {
        emit renameRequested(newName);
    }
}

void AlbumWidget::onAddImageClicked()
{
    emit addImageRequested(currentAlbumPath);
}

void AlbumWidget::onDeleteImageClicked()
{
    emit deleteImageRequested(currentAlbumPath);
}

