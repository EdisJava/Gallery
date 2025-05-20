#ifndef ALBUMDAO_H
#define ALBUMDAO_H

#include <vector>
#include <memory>

class QSqlDatabase;
class Album;

class AlbumDao
{
public:
    AlbumDao(QSqlDatabase& database);
    void init() const;

    void addAlbum(Album& album) const;
    void updateAlbum(const Album& album) const;
    void removeAlbum(int id) const;
    std::vector<std::unique_ptr<Album>> albums() const;  // ✅ aquí está

private:
    QSqlDatabase& mDatabase;
};

#endif // ALBUMDAO_H
