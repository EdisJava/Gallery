#include "AlbumListWidget.h"
#include <QInputDialog>
#include "AlbumModel.h"
#include "ui_albumlistwidget.h"
AlbumListWidget::AlbumListWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AlbumListWidget),
    mAlbumModel(nullptr)
{
    ui->setupUi(this);
    connect(ui->createAlbumButton, &QPushButton::clicked,
            this, &AlbumListWidget::createAlbum);
}
AlbumListWidget::~AlbumListWidget()
{
    delete ui;
}

void AlbumListWidget::setModel(AlbumModel* model)
{
    qDebug() << "=== AlbumListWidget::setModel ===";
    mAlbumModel = model;
    ui->albumList->setModel(mAlbumModel);

    // Configurar el comportamiento de selección
    ui->albumList->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->albumList->setSelectionBehavior(QAbstractItemView::SelectRows);

    qDebug() << "Modelo asignado, filas:" << (model ? model->rowCount() : 0);
}

void AlbumListWidget::setSelectionModel(QItemSelectionModel* selectionModel)
{
    qDebug() << "=== AlbumListWidget::setSelectionModel ===";
    qDebug() << "selectionModel:" << selectionModel;

    ui->albumList->setSelectionModel(selectionModel);

    // Conectar para debuggear
    connect(selectionModel, &QItemSelectionModel::selectionChanged,
            [](const QItemSelection& selected, const QItemSelection& deselected) {
                qDebug() << "*** AlbumListWidget capturó selectionChanged ***";
                qDebug() << "Selected count:" << selected.indexes().count();
                qDebug() << "Deselected count:" << deselected.indexes().count();
            });

    // También conectar clicked directamente
    connect(ui->albumList, &QAbstractItemView::clicked,
            [selectionModel](const QModelIndex& index) {
                qDebug() << "*** albumList CLICKED ***";
                qDebug() << "Index válido:" << index.isValid();
                qDebug() << "Row:" << index.row();
                qDebug() << "Data:" << index.data().toString();

                // Forzar la selección explícitamente
                selectionModel->setCurrentIndex(index,
                                                QItemSelectionModel::ClearAndSelect);
            });
}

void AlbumListWidget::createAlbum()
{
    if(!mAlbumModel) {
        return;
    }
    bool ok;
    QString albumName = QInputDialog::getText(this,
                                              "Create a new Album",
                                              "Choose an name",
                                              QLineEdit::Normal,
                                              "New album",
                                              &ok);
    if (ok && !albumName.isEmpty()) {
        Album album(albumName);
        QModelIndex createdIndex = mAlbumModel->addAlbum(album);
        ui->albumList->setCurrentIndex(createdIndex);

        qDebug() << "Álbum creado en index:" << createdIndex.row();
    }
}
