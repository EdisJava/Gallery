#include "DatabaseManager.h"
#include "AlbumDao.h"
#include "PictureDao.h"
#include <memory>

#include <QSqlDatabase>

DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager singleton;
    return singleton;
}

DatabaseManager::DatabaseManager(const QString& path)
    : mDatabase(new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE")))
{
    mDatabase->setDatabaseName(path);
    mDatabase->open();

    albumDao = std::make_unique<AlbumDao>(*mDatabase);
    pictureDao = std::make_unique<PictureDao>(*mDatabase);

    albumDao->init();
    pictureDao->init();
}


DatabaseManager::~DatabaseManager() {
    mDatabase->close();
    delete mDatabase;
}
