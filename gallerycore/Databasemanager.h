#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QString>
#include "AlbumDao.h"
#include "PictureDao.h"

const QString DATABASE_FILENAME = "gallery.db";

class QSqlDatabase;
class DatabaseManager
{
public:
    static DatabaseManager& instance();
    ~DatabaseManager();

protected:
    DatabaseManager(const QString& path = DATABASE_FILENAME);
    DatabaseManager& operator=(const DatabaseManager& rhs);

private:
    QSqlDatabase* mDatabase;

    //No mover, si no, crashea
public:
    const AlbumDao albumDao;
    const PictureDao pictureDao;


};

#endif
