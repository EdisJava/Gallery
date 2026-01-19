#include "Databasemanager.h"
#include <QSqlDatabase>

/**
 * Retorna la instancia única del DatabaseManager (patrón Singleton)
 * @return Referencia a la única instancia de DatabaseManager
 *
 * Implementa el patrón Singleton usando una variable estática local.
 * La primera vez que se llama, crea la instancia; las siguientes veces retorna
 * la misma instancia ya creada. Esto garantiza que solo exista una conexión
 * a la base de datos en toda la aplicación.
 *
 * Thread-safe en C++11 y versiones posteriores: la inicialización de variables
 * estáticas locales está garantizada como thread-safe por el estándar.
 */
DatabaseManager& DatabaseManager::instance()
{
    static DatabaseManager singleton;  // Se crea solo una vez, persiste durante toda la ejecución
    return singleton;  // Retorna siempre la misma instancia
}

/**
 * Constructor privado de DatabaseManager
 * @param path Ruta del archivo de base de datos SQLite (valor por defecto definido en el .h)
 *
 * Inicializa la conexión a la base de datos y los objetos DAO (Data Access Object).
 * Este constructor es privado para asegurar que solo se pueda crear una instancia
 * mediante el método instance() (patrón Singleton).
 *
 * Pasos que realiza:
 * 1. Crea la conexión a la base de datos SQLite
 * 2. Inicializa los DAOs (albumDao y pictureDao) con la conexión
 * 3. Establece la ruta del archivo de base de datos
 * 4. Abre la conexión a la base de datos
 * 5. Inicializa las tablas necesarias en la base de datos
 */
DatabaseManager::DatabaseManager(const QString& path) :
    // Crea un nuevo objeto QSqlDatabase con el driver QSQLITE para bases de datos SQLite
    mDatabase(new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE"))),
    // Inicializa el DAO de álbumes pasándole la referencia a la base de datos
    albumDao(*mDatabase),
    // Inicializa el DAO de imágenes pasándole la referencia a la base de datos
    pictureDao(*mDatabase)
{
    // Establece la ruta del archivo de base de datos SQLite
    mDatabase->setDatabaseName(path);

    // Abre la conexión a la base de datos
    // Si el archivo no existe, SQLite lo creará automáticamente
    mDatabase->open();

    // Inicializa la tabla de álbumes en la base de datos
    // Crea la tabla si no existe
    albumDao.init();

    // Inicializa la tabla de imágenes en la base de datos
    // Crea la tabla si no existe
    pictureDao.init();
}

/**
 * Destructor de DatabaseManager
 *
 * Se encarga de cerrar correctamente la conexión a la base de datos
 * y liberar la memoria del objeto QSqlDatabase.
 *
 * Es importante cerrar la conexión antes de eliminar el objeto para
 * asegurar que todos los datos pendientes se escriban correctamente
 * y los recursos se liberen de forma apropiada.
 */
DatabaseManager::~DatabaseManager()
{
    // Cierra la conexión a la base de datos
    // Esto asegura que todos los cambios pendientes se escriban en disco
    mDatabase->close();

    // Libera la memoria del objeto QSqlDatabase creado dinámicamente
    delete mDatabase;
}
