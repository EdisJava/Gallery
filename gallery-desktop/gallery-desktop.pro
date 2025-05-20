QT       += core gui widgets

CONFIG   += c++17
INCLUDEPATH += $$PWD/../QuaZip

TEMPLATE = app
TARGET = gallery-desktop

SOURCES += \
    albumimageswidget.cpp \
    albumlistwidget.cpp \
    albumwidget.cpp \
    gallerywidget.cpp \
    main.cpp \
    MainWindow.cpp \
    picturewidget.cpp
HEADERS += \
    MainWindow.h \
    albumimageswidget.h \
    albumlistwidget.h \
    albumwidget.h \
    gallerywidget.h \
    picturewidget.h

FORMS += \
    MainWindow.ui \
    albumimageswidget.ui \
    albumlistwidget.ui \
    albumwidget.ui \
    gallerywidget.ui \
    picturewidget.ui \


RESOURCES += \
    resource.qrc

# Link con gallery-core
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../gallery-core/release/ -lgallery-core
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../gallery-core/debug/ -lgallery-core
else:unix: LIBS += -L$$OUT_PWD/../gallery-core/ -lgallery-core

INCLUDEPATH += ../QuaZip
DEPENDPATH += ../QuaZip


INCLUDEPATH += $$PWD/../QuaZip/headers
DEPENDPATH += $$PWD/../QuaZip/headers
