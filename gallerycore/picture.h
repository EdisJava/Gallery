#include <QUrl>
#include <QString>
#include "gallerycore_global.h"

class GALLERYCORE_EXPORT Picture
{
public:
    Picture(const QString& filePath = "");
    Picture(const QUrl& fileUrl);

    int id() const;
    int albumId() const;

    void setId(int id);
    void setAlbumId(int albumId);
    void setFileUrl(const QUrl& fileUrl);

    QUrl fileUrl() const;

private:
    int mId;
    int mAlbumId;
    QUrl mFileUrl;
};
