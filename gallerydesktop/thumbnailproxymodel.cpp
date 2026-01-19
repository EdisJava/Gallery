#include "thumbnailproxymodel.h"
#include "Picturemodel.h"
#include "qfileinfo.h"
#include <QImageReader>

// Tamaño máximo (ancho y alto) de los thumbnails generados
const unsigned int THUMBNAIL_SIZE = 350;

// Constructor del proxy model
// Usa QIdentityProxyModel porque no altera estructura ni índices,
// solo modifica los datos que expone (en este caso, DecorationRole)
ThumbnailProxyModel::ThumbnailProxyModel(QObject* parent)
    : QIdentityProxyModel(parent)
{
}

// Genera thumbnails a partir de un índice inicial y una cantidad de filas
void ThumbnailProxyModel::generateThumbnails(
    const QModelIndex& startIndex,
    int count)
{
    // Mensajes de depuración
    qDebug() << "=== generateThumbnails ===";
    qDebug() << "startIndex válido:" << startIndex.isValid();
    qDebug() << "count:" << count;

    // Si el índice inicial no es válido, no se genera nada
    if (!startIndex.isValid()) {
        return;
    }

    // Modelo original (source model)
    const QAbstractItemModel* model = startIndex.model();

    // Calcula la última fila a procesar
    int lastIndex = startIndex.row() + count;

    // Recorre las filas indicadas
    for(int row = startIndex.row(); row < lastIndex; row++) {

        // Obtiene la ruta del archivo desde el modelo original
        QString filepath = model->data(model->index(row, 0),
                                       PictureModel::PictureRole::FilePathRole).toString();

        qDebug() << "  Generando thumbnail para:" << filepath;

        // Convierte la ruta (posiblemente URL) a ruta local
        QUrl url(filepath);
        QString localPath = url.toLocalFile();

        // Información de depuración sobre la ruta
        qDebug() << "  URL original:" << filepath;
        qDebug() << "  Ruta local:" << localPath;

        // Información del archivo en disco
        QFileInfo fileInfo(localPath);
        qDebug() << "  Archivo existe:" << fileInfo.exists();
        qDebug() << "  Es archivo:" << fileInfo.isFile();
        qDebug() << "  Ruta absoluta:" << fileInfo.absoluteFilePath();
        qDebug() << "  Tamaño:" << fileInfo.size() << "bytes";
        qDebug() << "  Extensión:" << fileInfo.suffix();
        qDebug() << "  Es legible:" << fileInfo.isReadable();

        // Lector de imágenes para comprobar formato y lectura
        QImageReader reader(localPath);
        qDebug() << "  Formato detectado:" << reader.format();
        qDebug() << "  Puede leer:" << reader.canRead();

        // Si no puede leer, se muestra el error
        if (!reader.canRead()) {
            qDebug() << "  Error QImageReader:" << reader.errorString();
        }

        // Carga la imagen como QPixmap
        QPixmap pixmap(localPath);

        // Comprobación de carga correcta
        if (pixmap.isNull()) {
            qDebug() << "  ERROR: QPixmap es null para:" << localPath;
        } else {
            qDebug() << "  OK: Pixmap cargado, tamaño:" << pixmap.size();
        }

        // Se crea el thumbnail escalando el pixmap original
        auto thumbnail = new QPixmap(
            pixmap.scaled(
                THUMBNAIL_SIZE,
                THUMBNAIL_SIZE,
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
                )
            );

        // Se guarda el thumbnail en el mapa usando la ruta como clave
        mThumbnails.insert(filepath, thumbnail);
    }
}

// Regenera todos los thumbnails desde cero
void ThumbnailProxyModel::reloadThumbnails()
{
    qDebug() << "=== ThumbnailProxyModel::reloadThumbnails ===";
    qDebug() << "rowCount():" << rowCount();

    // Elimina todos los QPixmap almacenados
    qDeleteAll(mThumbnails);
    mThumbnails.clear();

    // Genera thumbnails para todas las filas del modelo
    generateThumbnails(index(0, 0), rowCount());

    qDebug() << "Thumbnails generados:" << mThumbnails.count();
}

// Asigna el modelo fuente al proxy
void ThumbnailProxyModel::setSourceModel(QAbstractItemModel* sourceModel)
{
    // Llama al método base
    QIdentityProxyModel::setSourceModel(sourceModel);

    // Si no hay modelo fuente, se sale
    if (!sourceModel) {
        return;
    }

    // Cuando el modelo se resetea completamente
    connect(sourceModel, &QAbstractItemModel::modelReset,
            [this] {
                qDebug() << "ThumbnailProxyModel: modelReset recibido";
                reloadThumbnails();
            });

    // Cuando se insertan nuevas filas
    connect(sourceModel, &QAbstractItemModel::rowsInserted,
            [this] (const QModelIndex& parent, int first, int last) {
                qDebug() << "ThumbnailProxyModel: rowsInserted" << first << last;
                generateThumbnails(index(first, 0), last - first + 1);
            });

    // Cuando cambian datos del modelo
    connect(sourceModel, &QAbstractItemModel::dataChanged,
            [this] (const QModelIndex& topLeft, const QModelIndex& bottomRight) {
                qDebug() << "ThumbnailProxyModel: dataChanged recibido";
                reloadThumbnails();
            });
}

// Devuelve los datos del modelo
QVariant ThumbnailProxyModel::data(const QModelIndex& index, int role) const
{
    // Para cualquier rol distinto de DecorationRole,
    // se delega directamente al modelo base
    if (role != Qt::DecorationRole) {
        return QIdentityProxyModel::data(index, role);
    }

    // Obtiene la ruta del archivo desde el modelo fuente
    QString filepath = sourceModel()->data(index,
                                           PictureModel::PictureRole::FilePathRole).toString();

    // Devuelve el thumbnail asociado a esa ruta
    return *mThumbnails[filepath];
}

// Devuelve el modelo fuente tipado como PictureModel
PictureModel* ThumbnailProxyModel::pictureModel() const
{
    return static_cast<PictureModel*>(sourceModel());
}
