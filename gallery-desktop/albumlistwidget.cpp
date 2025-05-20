#include "albumlistwidget.h"

#include <QDir>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QContextMenuEvent>
#include <QDebug>

AlbumListWidget::AlbumListWidget(QWidget *parent)
    : QListWidget(parent)
{
    connect(this, &QListWidget::itemClicked, [this](QListWidgetItem *item){
        if(item)
            emit albumSelected(item->text());
    });
}

void AlbumListWidget::loadAlbumsFromFolder(const QString &folderPath)
{
    currentFolderPath = folderPath;
    clear();

    QDir dir(folderPath);
    QStringList folders = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for(const QString &folder : folders){
        addItem(folder);
    }
}

void AlbumListWidget::createAlbum(const QString &albumName)
{
    if(currentFolderPath.isEmpty())
        return;

    QDir dir(currentFolderPath);
    if(dir.exists(albumName)){
        QMessageBox::warning(this, "Error", "El álbum ya existe.");
        return;
    }
    if(!dir.mkdir(albumName)){
        QMessageBox::warning(this, "Error", "No se pudo crear el álbum.");
        return;
    }
    addItem(albumName);
}

void AlbumListWidget::deleteSelectedAlbum()
{
    QListWidgetItem *item = currentItem();
    if(!item)
        return;

    QString albumName = item->text();
    QDir dir(currentFolderPath + "/" + albumName);

    if(dir.exists()){
        if(!dir.removeRecursively()){
            QMessageBox::warning(this, "Error", "No se pudo borrar el álbum.");
            return;
        }
    }

    delete item;
}

void AlbumListWidget::contextMenuEvent(QContextMenuEvent *event)
{
    QListWidgetItem *item = itemAt(event->pos());
    if(!item)
        return;

    QMenu menu(this);
    QAction *renameAction = menu.addAction("Renombrar álbum");

    QAction *selectedAction = menu.exec(event->globalPos());
    if(selectedAction == renameAction){
        renameAlbum();
    }
}

void AlbumListWidget::renameAlbum()
{
    QListWidgetItem *item = currentItem();
    if(!item)
        return;

    QString oldName = item->text();

    bool ok;
    QString newName = QInputDialog::getText(this, "Renombrar álbum",
                                            "Nuevo nombre:",
                                            QLineEdit::Normal,
                                            oldName, &ok);
    if(!ok || newName.isEmpty() || newName == oldName)
        return;

    QDir dir(currentFolderPath);
    if(dir.exists(newName)){
        QMessageBox::warning(this, "Error", "Ya existe un álbum con ese nombre.");
        return;
    }

    // Renombrar carpeta en disco
    if(!dir.rename(oldName, newName)){
        QMessageBox::warning(this, "Error", "No se pudo renombrar el álbum.");
        return;
    }

    // Actualizar item y emitir señal
    item->setText(newName);
    emit albumRenamed(oldName, newName);
}
