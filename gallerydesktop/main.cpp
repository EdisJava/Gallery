#include "mainwindow.h"
#include "qfileinfo.h"
#include <QTranslator>
#include <QApplication>
#include <QImageReader>

/**
 * Punto de entrada principal de la aplicación Qt
 *
 * Este archivo se encarga de:
 * - Inicializar QApplication
 * - Detectar el idioma del sistema
 * - Cargar traducciones si están disponibles
 * - Verificar formatos de imagen soportados
 * - Realizar pruebas de guardado y carga de imágenes
 * - Crear y mostrar la ventana principal
 */
int main(int argc, char *argv[])
{
    // Inicializa la aplicación Qt
    // Gestiona el loop de eventos, estilos, traducciones, etc.
    QApplication a(argc, argv);

    /**
     * =========================
     * DETECCIÓN DE IDIOMA
     * =========================
     */

    // Obtiene la configuración regional del sistema
    QLocale locale;

    // Extrae el idioma base ("es", "en", etc.)
    // Ejemplo: "es_ES" -> "es"
    QString language = locale.name().section('_', 0, 0);
    qDebug() << "Idioma del sistema:" << language;

    /**
     * =========================
     * CARGA DE TRADUCCIONES
     * =========================
     */

    // Objeto encargado de traducir cadenas usando archivos .qm
    QTranslator translator;

    // Si el idioma del sistema es español, intenta cargar la traducción
    if(language == "es") {
        // Carga el archivo de traducción desde los recursos Qt
        if(translator.load(":/translations/translations/ch03_gallery_core_es_ES.qm")) {
            // Instala el traductor en la aplicación
            a.installTranslator(&translator);
            qDebug() << "Traducción española cargada.";
        } else {
            // Debug en caso de error de carga
            qDebug() << "No se pudo cargar app_es.qm";
        }
    }

    /**
     * =========================
     * DEBUG: FORMATOS DE IMAGEN
     * =========================
     */

    // Muestra todos los formatos de imagen soportados por Qt
    qDebug() << "=== Formatos de imagen soportados ===";
    QList<QByteArray> formats = QImageReader::supportedImageFormats();
    for (const QByteArray& format : formats) {
        qDebug() << "  -" << format;
    }

    /**
     * =========================
     * DEBUG: PRUEBA DE IMÁGENES
     * =========================
     */

    // Crea una imagen de prueba en memoria
    QImage testImage(200, 200, QImage::Format_RGB32);

    // Rellena la imagen con color rojo
    testImage.fill(Qt::red);

    // Ruta donde se guardará la imagen de prueba
    QString testNewPath = "C:/Users/Edgar/Pictures/Gallery/test.png";

    // Guarda la imagen en formato PNG
    bool saved = testImage.save(testNewPath, "PNG");
    qDebug() << "Imagen de prueba guardada:" << saved << "en" << testNewPath;

    // Intenta cargar la imagen guardada como QPixmap
    QPixmap testPixmap(testNewPath);

    // Verifica si el pixmap se cargó correctamente
    qDebug() << "Test pixmap null:" << testPixmap.isNull();

    /**
     * =========================
     * CREACIÓN DE LA UI
     * =========================
     */

    // Crea la ventana principal de la aplicación
    MainWindow w;

    // Muestra la ventana principal
    w.show();

    // Inicia el loop de eventos de la aplicación
    return a.exec();
}
