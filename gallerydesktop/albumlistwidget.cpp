#include "AlbumListWidget.h"
#include <QInputDialog>
#include "AlbumModel.h"
#include "ui_albumlistwidget.h"

/**
 * Constructor de AlbumListWidget
 * @param parent Widget padre para la jerarquía de Qt (gestión automática de memoria)
 *
 * Inicializa el widget de lista de álbumes que proporciona la interfaz gráfica
 * para mostrar, crear y seleccionar álbumes.
 *
 * Configura la UI generada por Qt Designer y conecta las señales básicas.
 */
AlbumListWidget::AlbumListWidget(QWidget *parent) :
    QWidget(parent),           // Llama al constructor de la clase base QWidget
    ui(new Ui::AlbumListWidget),  // Crea la interfaz de usuario generada por Qt Designer
    mAlbumModel(nullptr)       // Inicializa el puntero al modelo como nullptr
{
    // Configura todos los widgets definidos en el archivo .ui
    ui->setupUi(this);

    // Conecta el botón de crear álbum con el slot correspondiente
    // Cuando se hace clic en el botón, se llama a la función createAlbum()
    connect(ui->createAlbumButton, &QPushButton::clicked,
            this, &AlbumListWidget::createAlbum);
}

/**
 * Destructor de AlbumListWidget
 *
 * Libera la memoria de la interfaz de usuario creada dinámicamente.
 * El objeto ui fue creado con 'new' en el constructor, por lo que
 * debe ser eliminado manualmente aquí.
 */
AlbumListWidget::~AlbumListWidget()
{
    delete ui;  // Libera la memoria de la UI
}

/**
 * Establece el modelo de datos para la lista de álbumes
 * @param model Puntero al AlbumModel que contiene los datos de los álbumes
 *
 * Conecta el QListView de la interfaz con el modelo de datos.
 * Configura el comportamiento de selección para permitir seleccionar
 * un álbum a la vez (selección simple de filas completas).
 *
 * Esta función debe ser llamada después de crear el AlbumModel para
 * que la vista pueda mostrar los álbumes.
 */
void AlbumListWidget::setModel(AlbumModel* model)
{
    qDebug() << "=== AlbumListWidget::setModel ===";

    // Almacena el puntero al modelo para uso posterior
    mAlbumModel = model;

    // Asocia el modelo con el QListView para que muestre los datos
    ui->albumList->setModel(mAlbumModel);

    // CONFIGURAR EL COMPORTAMIENTO DE SELECCIÓN

    // Permite seleccionar solo un elemento a la vez
    ui->albumList->setSelectionMode(QAbstractItemView::SingleSelection);

    // La selección afecta a filas completas, no a celdas individuales
    ui->albumList->setSelectionBehavior(QAbstractItemView::SelectRows);

    // Debug: muestra cuántas filas tiene el modelo
    qDebug() << "Modelo asignado, filas:" << (model ? model->rowCount() : 0);
}

/**
 * Establece el modelo de selección para sincronizar la selección entre vistas
 * @param selectionModel Puntero al QItemSelectionModel compartido entre vistas
 *
 * Permite compartir la selección de álbumes entre múltiples vistas (widgets).
 * Por ejemplo, cuando se selecciona un álbum en esta lista, otras partes
 * de la aplicación (como el widget de imágenes) pueden reaccionar al cambio.
 *
 * Configura dos conexiones para debug y manejo de selección:
 * 1. Escucha cambios en la selección para debugging
 * 2. Captura clics directos en la lista para forzar la selección
 */
void AlbumListWidget::setSelectionModel(QItemSelectionModel* selectionModel)
{
    qDebug() << "=== AlbumListWidget::setSelectionModel ===";
    qDebug() << "selectionModel:" << selectionModel;

    // Asocia el modelo de selección con el QListView
    // Esto permite que múltiples vistas compartan la misma selección
    ui->albumList->setSelectionModel(selectionModel);

    // CONEXIÓN 1: Conectar la señal de cambio de selección para debugging
    // Esta lambda se ejecuta cada vez que cambia la selección
    connect(selectionModel, &QItemSelectionModel::selectionChanged,
            [](const QItemSelection& selected, const QItemSelection& deselected) {
                qDebug() << "*** AlbumListWidget capturó selectionChanged ***";
                // Muestra cuántos elementos fueron seleccionados
                qDebug() << "Selected count:" << selected.indexes().count();
                // Muestra cuántos elementos fueron deseleccionados
                qDebug() << "Deselected count:" << deselected.indexes().count();
            });

    // CONEXIÓN 2: Conectar el clic directo en la lista
    // Captura el evento de clic para forzar explícitamente la selección
    // Esto puede ser necesario si el comportamiento automático no funciona correctamente
    connect(ui->albumList, &QAbstractItemView::clicked,
            [selectionModel](const QModelIndex& index) {
                qDebug() << "*** albumList CLICKED ***";
                // Debug: verifica si el índice es válido
                qDebug() << "Index válido:" << index.isValid();
                // Muestra la fila del elemento clicado
                qDebug() << "Row:" << index.row();
                // Muestra los datos (nombre del álbum) del elemento
                qDebug() << "Data:" << index.data().toString();

                // Forzar la selección explícitamente
                // ClearAndSelect: limpia la selección anterior y selecciona el nuevo elemento
                selectionModel->setCurrentIndex(index,
                                                QItemSelectionModel::ClearAndSelect);
            });
}

/**
 * Crea un nuevo álbum mediante un diálogo de entrada
 *
 * Muestra un cuadro de diálogo que solicita al usuario el nombre del nuevo álbum.
 * Si el usuario confirma y proporciona un nombre válido (no vacío):
 * 1. Crea un nuevo objeto Album con el nombre proporcionado
 * 2. Lo añade al modelo (que automáticamente lo guarda en la BD)
 * 3. Selecciona el nuevo álbum en la lista para que sea visible
 *
 * Esta función es llamada cuando el usuario hace clic en el botón "Crear Álbum"
 * (conectado en el constructor).
 */
void AlbumListWidget::createAlbum()
{
    // Verifica que el modelo esté configurado antes de intentar crear un álbum
    if(!mAlbumModel) {
        return;  // Sale si no hay modelo configurado
    }

    bool ok;  // Variable para almacenar si el usuario confirmó el diálogo

    // Muestra un cuadro de diálogo para solicitar el nombre del álbum
    QString albumName = QInputDialog::getText(
        this,                          // Widget padre del diálogo
        tr("Create a new Album"),      // Título de la ventana del diálogo
        tr("Choose a name"),           // Etiqueta del campo de texto
        QLineEdit::Normal,             // Modo de entrada de texto (normal, sin máscara)
        tr("New album"),               // Texto predeterminado en el campo
        &ok                            // Referencia para saber si el usuario confirmó
        );

    // Verifica que el usuario confirmó (OK) y que el nombre no esté vacío
    if (ok && !albumName.isEmpty()) {
        // Crea un nuevo objeto Album con el nombre proporcionado
        Album album(albumName);

        // Añade el álbum al modelo
        // El modelo se encarga de guardarlo en la base de datos y actualizar la vista
        QModelIndex createdIndex = mAlbumModel->addAlbum(album);

        // Selecciona automáticamente el álbum recién creado en la lista
        // Esto mejora la experiencia del usuario al mostrar el nuevo álbum seleccionado
        ui->albumList->setCurrentIndex(createdIndex);

        // Debug: confirma que el álbum fue creado y muestra su posición
        qDebug() << "Álbum creado en index:" << createdIndex.row();
    }
    // Si el usuario cancela o el nombre está vacío, no se hace nada
}
