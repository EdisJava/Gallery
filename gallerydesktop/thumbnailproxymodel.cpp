#include "thumbnailproxymodel.h"
#include "Picturemodel.h"
#include "qfileinfo.h"
#include <QImageReader>
const unsigned int THUMBNAIL_SIZE = 350;

ThumbnailProxyModel::ThumbnailProxyModel(QObject* parent)
    : QIdentityProxyModel(parent)  // inicializamos la clase base
{

}

void ThumbnailProxyModel::generateThumbnails(
    const QModelIndex& startIndex,
    int count)
{
    qDebug() << "=== generateThumbnails ===";
    qDebug() << "startIndex válido:" << startIndex.isValid();
    qDebug() << "count:" << count;

    if (!startIndex.isValid()) {
        return;
    }
    const QAbstractItemModel* model = startIndex.model();
    int lastIndex = startIndex.row() + count;
    for(int row = startIndex.row(); row < lastIndex; row++) {
        QString filepath = model->data(model->index(row, 0),
            PictureModel::PictureRole::FilePathRole).toString();

        qDebug() << "  Generando thumbnail para:" << filepath;

        //Convertir a ruta local
        QUrl url(filepath);
        QString localPath = url.toLocalFile();

        qDebug() << "  URL original:" << filepath;
        qDebug() << "  Ruta local:" << localPath;

        QFileInfo fileInfo(localPath);
        qDebug() << "  Archivo existe:" << fileInfo.exists();
        qDebug() << "  Es archivo:" << fileInfo.isFile();
        qDebug() << "  Ruta absoluta:" << fileInfo.absoluteFilePath();
        qDebug() << "  Tamaño:" << fileInfo.size() << "bytes";
        qDebug() << "  Extensión:" << fileInfo.suffix();
        qDebug() << "  Es legible:" << fileInfo.isReadable();


        QImageReader reader(localPath);
        qDebug() << "  Formato detectado:" << reader.format();
        qDebug() << "  Puede leer:" << reader.canRead();
        if (!reader.canRead()) {
            qDebug() << "  Error QImageReader:" << reader.errorString();
        }


        QPixmap pixmap(localPath);

        if (pixmap.isNull()) {
            qDebug() << "  ERROR: QPixmap es null para:" << localPath;
        } else {
            qDebug() << "  OK: Pixmap cargado, tamaño:" << pixmap.size();
        }

        auto thumbnail = new QPixmap(pixmap.scaled(THUMBNAIL_SIZE, THUMBNAIL_SIZE,
            Qt::KeepAspectRatio, Qt::SmoothTransformation));
        mThumbnails.insert(filepath, thumbnail);
    }
}

void ThumbnailProxyModel::reloadThumbnails()
{
    qDebug() << "=== ThumbnailProxyModel::reloadThumbnails ===";
    qDebug() << "rowCount():" << rowCount();

    qDeleteAll(mThumbnails);
    mThumbnails.clear();
    generateThumbnails(index(0, 0), rowCount());

    qDebug() << "Thumbnails generados:" << mThumbnails.count();

}

void ThumbnailProxyModel::setSourceModel(QAbstractItemModel* sourceModel)
{
    QIdentityProxyModel::setSourceModel(sourceModel);
    if (!sourceModel) {
        return;
    }

    connect(sourceModel, &QAbstractItemModel::modelReset,
            [this] {
                qDebug() << "ThumbnailProxyModel: modelReset recibido";
                reloadThumbnails();
            });

    connect(sourceModel, &QAbstractItemModel::rowsInserted,
            [this] (const QModelIndex& parent, int first, int last) {
                qDebug() << "ThumbnailProxyModel: rowsInserted" << first << last;
                generateThumbnails(index(first, 0), last - first + 1);
            });

    connect(sourceModel, &QAbstractItemModel::dataChanged,
            [this] (const QModelIndex& topLeft, const QModelIndex& bottomRight) {
                qDebug() << "ThumbnailProxyModel: dataChanged recibido";
                reloadThumbnails();
            });
}

QVariant ThumbnailProxyModel::data(const QModelIndex& index, int role)
    const
{
    if (role != Qt::DecorationRole) {
        return QIdentityProxyModel::data(index, role);
    }
    QString filepath = sourceModel()->data(index,
        PictureModel::PictureRole::FilePathRole).toString();
    return *mThumbnails[filepath];
}


PictureModel* ThumbnailProxyModel::pictureModel() const
{
    return static_cast<PictureModel*>(sourceModel());
}
