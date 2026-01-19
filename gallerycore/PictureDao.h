#include <QVector>
class QSqlDatabase;
class Picture;
class PictureDao
{
public:
    explicit PictureDao(QSqlDatabase& database);

    void init() const;
    void addPictureInAlbum(int albumId, Picture& picture) const;
    void removePicture(int id) const;
    void removePicturesForAlbum(int albumId) const;

    QVector<Picture*> picturesForAlbum(int albumId) const;

private:
    QSqlDatabase& mDatabase;
};
