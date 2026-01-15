#include "mainwindow.h"
#include "qfileinfo.h"
#include <QTranslator>

#include <QApplication>
#include<qimagereader.h>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    QLocale locale;
    QString language = locale.name().section('_', 0, 0); // "es", "en", etc.
    qDebug() << "Idioma del sistema:" << language;

    QTranslator translator;
    if(language == "es") {
        // Cargar traducción solo si el idioma es español
        if(translator.load(":/translations/translations/ch03_gallery_core_es_ES.qm")) {
            a.installTranslator(&translator);
            qDebug() << "Traducción española cargada.";
        } else {
            qDebug() << "No se pudo cargar app_es.qm";
        }
    }





   /* QTranslator translator;
    if(translator.load(":/translations/translations/ch03_gallery_core_es_ES.qm")) {
        a.installTranslator(&translator);
    }  else {
        qDebug() << "No se pudo cargar el .qm";
    }*/

    // Ver qué formatos de imagen están soportados
    qDebug() << "=== Formatos de imagen soportados ===";
    QList<QByteArray> formats = QImageReader::supportedImageFormats();
    for (const QByteArray& format : formats) {
        qDebug() << "  -" << format;
    }



    QImage testImage(200, 200, QImage::Format_RGB32);
    testImage.fill(Qt::red);
    QString testNewPath = "C:/Users/Edgar/Pictures/Gallery/test.png";
    bool saved = testImage.save(testNewPath, "PNG");
    qDebug() << "Imagen de prueba guardada:" << saved << "en" << testNewPath;

    // Intentar cargarla
    QPixmap testPixmap(testNewPath);
    qDebug() << "Test pixmap null:" << testPixmap.isNull();



    MainWindow w;
    w.show();
    return a.exec();
}
