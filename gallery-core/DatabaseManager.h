#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H
#include <memory>

#include <QString>

class QSqlDatabase;
class AlbumDao;
class PictureDao;

const QString DATABASE_FILENAME = "gallery.db";

class DatabaseManager
{
public:
    static DatabaseManager& instance();
    ~DatabaseManager();
    std::unique_ptr<AlbumDao> albumDao;
    std::unique_ptr<PictureDao> pictureDao;

protected:
    DatabaseManager(const QString& path = DATABASE_FILENAME);
    DatabaseManager& operator=(const DatabaseManager& rhs);
    DatabaseManager(const DatabaseManager& rhs);

private:
    QSqlDatabase* mDatabase;
};

#endif // DATABASEMANAGER_H
