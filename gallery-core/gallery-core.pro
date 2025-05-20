QT += core sql
CONFIG += c++17
TEMPLATE = lib
TARGET = gallery-core

INCLUDEPATH += .

HEADERS += \
    DatabaseManager.h \
    gallery-core_global.h \
    Album.h \
    Picture.h \
    AlbumModel.h \
    PictureModel.h \
    AlbumDao.h \
    PictureDao.h

SOURCES += \
    Album.cpp \
    DatabaseManager.cpp \
    Picture.cpp \
    AlbumModel.cpp \
    PictureModel.cpp \
    AlbumDao.cpp \
    PictureDao.cpp
