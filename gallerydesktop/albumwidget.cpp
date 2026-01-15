#include "albumwidget.h"
#include "picturedelegate.h"
#include "thumbnailproxymodel.h"
#include "ui_albumwidget.h"

#include <QInputDialog>
#include <QFileDialog>

#include "AlbumModel.h"
#include "PictureModel.h"

AlbumWidget::AlbumWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AlbumWidget),
    mAlbumModel(nullptr),
    mAlbumSelectionModel(nullptr),
    mPictureModel(nullptr),
    mPictureSelectionModel(nullptr)
{
    ui->setupUi(this);

   // clearUi();



    ui->thumbnailListView->setSpacing(5);
    ui->thumbnailListView->setResizeMode(QListView::Adjust);
    ui->thumbnailListView->setFlow(QListView::LeftToRight);
    ui->thumbnailListView->setWrapping(true);
    ui->thumbnailListView->setItemDelegate(
        new PictureDelegate(this));

    connect(ui->thumbnailListView, &QListView::doubleClicked,
            this, &AlbumWidget::pictureActivated);

    connect(ui->deleteButton, &QPushButton::clicked,
            this, &AlbumWidget::deleteAlbum);

    connect(ui->editButton, &QPushButton::clicked,
            this, &AlbumWidget::editAlbum);

    connect(ui->addPictureButton, &QPushButton::clicked,
            this, &AlbumWidget::addPictures);
}

AlbumWidget::~AlbumWidget()
{
    delete ui;
}

void AlbumWidget::setAlbumModel(AlbumModel* albumModel)
{
    mAlbumModel = albumModel;
    connect(mAlbumModel, &QAbstractItemModel::dataChanged,
            [this] (const QModelIndex &topLeft) {
                if (topLeft == mAlbumSelectionModel->currentIndex()) {
                    loadAlbum(topLeft);
                }
            });
}


void AlbumWidget::setAlbumSelectionModel(QItemSelectionModel* SelectionModel)
{
    mAlbumSelectionModel = SelectionModel;

    qDebug() << "AlbumWidget::setAlbumSelectionModel llamado";
    qDebug() << "SelectionModel tiene selección:" << (SelectionModel ? SelectionModel->hasSelection() : false);

     connect(SelectionModel,
        &QItemSelectionModel::selectionChanged,
        [this] (const QItemSelection &selected) {
            if (selected.isEmpty()) {
                    clearUi();
                    return;
                }
                loadAlbum(selected.indexes().first());
            });

    if (SelectionModel->hasSelection()) {
        QModelIndex currentIndex = SelectionModel->currentIndex();
        if (currentIndex.isValid()) {
            qDebug() << "Cargando álbum inicial automáticamente";
            loadAlbum(currentIndex);
        }
    }


}

void AlbumWidget::setPictureModel(ThumbnailProxyModel* pictureModel)
{
    mPictureModel = pictureModel;
    ui->thumbnailListView->setModel(pictureModel);


}

void AlbumWidget::setSelectionModel(QItemSelectionModel* selectionModel)
{
    ui->thumbnailListView->setSelectionModel(selectionModel);
}

void AlbumWidget::setPictureSelectionModel(QItemSelectionModel* selectionModel)
{
    ui->thumbnailListView->setSelectionModel(selectionModel);
}

void AlbumWidget::deleteAlbum()
{
    if (mAlbumSelectionModel->selectedIndexes().isEmpty()) {
        return;
    }
    int row = mAlbumSelectionModel->currentIndex().row();
    mAlbumModel->removeRow(row);
    // Intenta seleccionar el albm previo
    QModelIndex previousModelIndex = mAlbumModel->index(row - 1, 0);
    if(previousModelIndex.isValid()) {
        mAlbumSelectionModel->setCurrentIndex(previousModelIndex, QItemSelectionModel::SelectCurrent);
        return;
    }
    // Intenta seleccionar el album siguiente
    QModelIndex nextModelIndex = mAlbumModel->index(row, 0);
    if(nextModelIndex.isValid()) {
        mAlbumSelectionModel->setCurrentIndex(nextModelIndex, QItemSelectionModel::SelectCurrent);
        return;
    }
}

void AlbumWidget::editAlbum()
{
    if (mAlbumSelectionModel->selectedIndexes().isEmpty()) {
        return;
    }
    QModelIndex currentAlbumIndex =
        mAlbumSelectionModel->selectedIndexes().first();
    QString oldAlbumName = mAlbumModel->data(currentAlbumIndex, AlbumModel::Roles::NameRole).toString();
    bool ok;
    QString newName = QInputDialog::getText(this, "Album's name", "Change Album name",
         QLineEdit::Normal, oldAlbumName, &ok);
    if (ok && !newName.isEmpty()) {
        mAlbumModel->setData(currentAlbumIndex, newName, AlbumModel::Roles::NameRole);
    }
}

void AlbumWidget::addPictures()
{
    QStringList filenames =
        QFileDialog::getOpenFileNames(this, "Add pictures", QDir::homePath(), "Picture files (*.jpg *.png)");
    if (!filenames.isEmpty()) {
        QModelIndex lastModelIndex;
        for (auto filename : filenames) {
            Picture picture(filename);
            lastModelIndex = mPictureModel->pictureModel()->addPicture(picture);
        }
        ui->thumbnailListView->setCurrentIndex(lastModelIndex);
    }
}

void AlbumWidget::clearUi()
{
    ui->albumName->setText("");
    ui->deleteButton->setVisible(false);
    ui->editButton->setVisible(false);
    ui->addPictureButton->setVisible(false);
}


void AlbumWidget::loadAlbum(const QModelIndex& albumIndex)
{
    qDebug() << "=== loadAlbum llamado ===";
    qDebug() << "albumIndex válido:" << albumIndex.isValid();

    if (!albumIndex.isValid()) {
        qDebug() << "Index inválido, retornando";
        return;
    }

    if (!mAlbumModel) {
        qDebug() << "mAlbumModel es NULL!";
        return;
    }

    if (!mPictureModel) {
        qDebug() << "mPictureModel es NULL!";
        return;
    }

    int albumId = mAlbumModel->data(albumIndex, AlbumModel::Roles::IdRole).toInt();
    QString albumName = mAlbumModel->data(albumIndex, Qt::DisplayRole).toString();

    qDebug() << "Album ID:" << albumId;
    qDebug() << "Album Name:" << albumName;

    mPictureModel->pictureModel()->setAlbumId(albumId);
    ui->albumName->setText(albumName);
    ui->deleteButton->setVisible(true);
    ui->editButton->setVisible(true);
    ui->addPictureButton->setVisible(true);

    qDebug() << "UI actualizada correctamente";
}

