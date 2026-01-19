#include "albumwidget.h"
#include "picturedelegate.h"
#include "thumbnailproxymodel.h"
#include "ui_albumwidget.h"
#include <QInputDialog>
#include <QFileDialog>
#include "AlbumModel.h"
#include "PictureModel.h"

/**
 * Constructor de AlbumWidget
 * @param parent Widget padre para la jerarquía de Qt (gestión automática de memoria)
 *
 * Inicializa el widget principal de gestión de álbumes que proporciona:
 * - Visualización de miniaturas de imágenes en un ListView
 * - Botones para editar, eliminar álbumes y añadir imágenes
 * - Interfaz de usuario completa para la gestión de álbumes e imágenes
 *
 * Configura la apariencia del ListView de miniaturas y conecta las señales
 * de los botones con sus respectivos slots.
 */
AlbumWidget::AlbumWidget(QWidget *parent) :
    QWidget(parent),                    // Llama al constructor de la clase base
    ui(new Ui::AlbumWidget),           // Crea la interfaz generada por Qt Designer
    mAlbumModel(nullptr),              // Inicializa puntero al modelo de álbumes
    mAlbumSelectionModel(nullptr),     // Inicializa puntero al modelo de selección de álbumes
    mPictureModel(nullptr),            // Inicializa puntero al modelo proxy de imágenes
    mPictureSelectionModel(nullptr)    // Inicializa puntero al modelo de selección de imágenes
{
    // Configura todos los widgets definidos en el archivo .ui
    ui->setupUi(this);

    // CONFIGURACIÓN DEL LISTVIEW DE MINIATURAS

    // Establece el espacio entre elementos en píxeles
    ui->thumbnailListView->setSpacing(5);

    // Ajusta automáticamente el tamaño de los elementos cuando cambia el tamaño de la ventana
    ui->thumbnailListView->setResizeMode(QListView::Adjust);

    // Organiza los elementos en flujo horizontal (de izquierda a derecha)
    ui->thumbnailListView->setFlow(QListView::LeftToRight);

    // Permite que los elementos se envuelvan a la siguiente línea automáticamente
    ui->thumbnailListView->setWrapping(true);

    // Establece un delegado personalizado para renderizar las miniaturas
    // PictureDelegate se encarga de cómo se dibuja cada imagen en la lista
    ui->thumbnailListView->setItemDelegate(
        new PictureDelegate(this));

    // CONEXIONES DE SEÑALES Y SLOTS

    // Conecta el doble clic en una imagen con la función pictureActivated
    // Permite abrir/activar una imagen al hacer doble clic
    connect(ui->thumbnailListView, &QListView::doubleClicked,
            this, &AlbumWidget::pictureActivated);

    // Conecta el botón de eliminar con la función deleteAlbum
    connect(ui->deleteButton, &QPushButton::clicked,
            this, &AlbumWidget::deleteAlbum);

    // Conecta el botón de editar con la función editAlbum
    connect(ui->editButton, &QPushButton::clicked,
            this, &AlbumWidget::editAlbum);

    // Conecta el botón de añadir imágenes con la función addPictures
    connect(ui->addPictureButton, &QPushButton::clicked,
            this, &AlbumWidget::addPictures);
}

/**
 * Destructor de AlbumWidget
 *
 * Libera la memoria de la interfaz de usuario creada dinámicamente.
 */
AlbumWidget::~AlbumWidget()
{
    delete ui;  // Libera la memoria de la UI
}

/**
 * Establece el modelo de álbumes y configura las conexiones necesarias
 * @param albumModel Puntero al AlbumModel que contiene los datos de los álbumes
 *
 * Conecta el modelo de álbumes con el widget y establece una conexión
 * para detectar cuando los datos de un álbum cambian (por ejemplo, cuando
 * se renombra). Si el álbum modificado es el que está actualmente
 * seleccionado, recarga la interfaz para mostrar los cambios.
 */
void AlbumWidget::setAlbumModel(AlbumModel* albumModel)
{
    // Almacena el puntero al modelo de álbumes
    mAlbumModel = albumModel;

    // Conecta la señal dataChanged del modelo
    // Esta señal se emite cuando los datos de un álbum cambian
    connect(mAlbumModel, &QAbstractItemModel::dataChanged,
            [this] (const QModelIndex &topLeft) {
                // Verifica si el álbum modificado es el actualmente seleccionado
                if (topLeft == mAlbumSelectionModel->currentIndex()) {
                    // Recarga la interfaz para mostrar los datos actualizados
                    loadAlbum(topLeft);
                }
            });
}

/**
 * Establece el modelo de selección de álbumes
 * @param SelectionModel Puntero al modelo de selección compartido
 *
 * Configura el modelo de selección que permite sincronizar la selección
 * de álbumes entre diferentes vistas (por ejemplo, AlbumListWidget y AlbumWidget).
 *
 * Conecta la señal de cambio de selección para:
 * - Limpiar la UI si no hay nada seleccionado
 * - Cargar el álbum seleccionado y mostrar sus imágenes
 *
 * Si ya hay un álbum seleccionado al momento de llamar esta función,
 * lo carga automáticamente.
 */
void AlbumWidget::setAlbumSelectionModel(QItemSelectionModel* SelectionModel)
{
    // Almacena el puntero al modelo de selección
    mAlbumSelectionModel = SelectionModel;

    qDebug() << "AlbumWidget::setAlbumSelectionModel llamado";
    qDebug() << "SelectionModel tiene selección:"
             << (SelectionModel ? SelectionModel->hasSelection() : false);

    // Conecta la señal de cambio de selección
    // Se ejecuta cada vez que el usuario selecciona un álbum diferente
    connect(SelectionModel,
            &QItemSelectionModel::selectionChanged,
            [this] (const QItemSelection &selected) {
                // Si no hay nada seleccionado, limpia la interfaz
                if (selected.isEmpty()) {
                    clearUi();
                    return;
                }
                // Carga el primer álbum seleccionado
                // (debería ser solo uno debido a SingleSelection)
                loadAlbum(selected.indexes().first());
            });

    // Si ya hay un álbum seleccionado al inicializar, cárgalo automáticamente
    if (SelectionModel->hasSelection()) {
        QModelIndex currentIndex = SelectionModel->currentIndex();
        if (currentIndex.isValid()) {
            qDebug() << "Cargando álbum inicial automáticamente";
            loadAlbum(currentIndex);
        }
    }
}

/**
 * Carga y muestra un álbum específico en la interfaz
 * @param albumIndex Índice del modelo que representa el álbum a cargar
 *
 * Esta función es el corazón del widget. Realiza las siguientes tareas:
 * 1. Valida que el índice y los modelos sean válidos
 * 2. Extrae el ID y nombre del álbum desde el modelo
 * 3. Configura el modelo de imágenes para mostrar las fotos del álbum
 * 4. Actualiza la interfaz de usuario (nombre del álbum, visibilidad de botones)
 *
 * Esta función es llamada cuando:
 * - El usuario selecciona un álbum diferente
 * - Se renombra el álbum actual
 * - Se inicializa el widget con un álbum ya seleccionado
 */
void AlbumWidget::loadAlbum(const QModelIndex& albumIndex)
{
    qDebug() << "=== loadAlbum llamado ===";
    qDebug() << "albumIndex válido:" << albumIndex.isValid();

    // VALIDACIONES: Verifica que todos los elementos necesarios estén disponibles

    if (!albumIndex.isValid()) {
        qDebug() << "Index inválido, retornando";
        return;
    }

    if (!mAlbumModel) {
        qDebug() << "mAlbumModel es NULL!";
        return;
    }

    if (!mPictureModel) {
        qDebug() << "mPictureModel es NULL!";
        return;
    }

    // EXTRACCIÓN DE DATOS: Obtiene la información del álbum desde el modelo

    // Obtiene el ID del álbum usando el rol personalizado IdRole
    int albumId = mAlbumModel->data(albumIndex, AlbumModel::Roles::IdRole).toInt();

    // Obtiene el nombre del álbum usando el rol estándar DisplayRole
    QString albumName = mAlbumModel->data(albumIndex, Qt::DisplayRole).toString();

    qDebug() << "Album ID:" << albumId;
    qDebug() << "Album Name:" << albumName;

    // ACTUALIZACIÓN DEL MODELO DE IMÁGENES
    // Configura el modelo de imágenes para mostrar solo las fotos de este álbum
    // pictureModel() accede al modelo subyacente dentro del proxy model
    mPictureModel->pictureModel()->setAlbumId(albumId);

    // ACTUALIZACIÓN DE LA INTERFAZ DE USUARIO

    // Muestra el nombre del álbum en el label correspondiente
    ui->albumName->setText(albumName);

    // Hace visibles los botones de acción (solo visibles cuando hay álbum cargado)
    ui->deleteButton->setVisible(true);
    ui->editButton->setVisible(true);
    ui->addPictureButton->setVisible(true);

    qDebug() << "UI actualizada correctamente";
}

/**
 * Edita el nombre del álbum actualmente seleccionado
 *
 * Muestra un cuadro de diálogo que permite al usuario cambiar el nombre
 * del álbum. Si el usuario confirma y proporciona un nombre válido:
 * 1. Actualiza el nombre en el modelo (que automáticamente actualiza la BD)
 * 2. La señal dataChanged se emite automáticamente
 * 3. La interfaz se actualiza mediante la conexión establecida en setAlbumModel()
 *
 * Si no hay álbum seleccionado o el usuario cancela, no se hace nada.
 */
void AlbumWidget::editAlbum()
{
    // Verifica que haya un álbum seleccionado
    if (mAlbumSelectionModel->selectedIndexes().isEmpty()) {
        return;
    }

    // Obtiene el índice del álbum actualmente seleccionado
    QModelIndex currentAlbumIndex =
        mAlbumSelectionModel->selectedIndexes().first();

    // Obtiene el nombre actual del álbum para mostrarlo como valor predeterminado
    QString oldAlbumName = mAlbumModel->data(currentAlbumIndex,
                                             AlbumModel::Roles::NameRole).toString();

    bool ok;  // Variable para saber si el usuario confirmó

    // Muestra el cuadro de diálogo para solicitar el nuevo nombre
    QString newName = QInputDialog::getText(
        this,                    // Widget padre
        "Album's name",          // Título de la ventana
        "Change Album name",     // Etiqueta del campo
        QLineEdit::Normal,       // Modo de entrada normal
        oldAlbumName,            // Valor predeterminado (nombre actual)
        &ok                      // Referencia para saber si confirmó
        );

    // Si el usuario confirmó y el nuevo nombre no está vacío
    if (ok && !newName.isEmpty()) {
        // Actualiza el nombre en el modelo usando setData
        // Esto automáticamente actualiza la base de datos y emite dataChanged
        mAlbumModel->setData(currentAlbumIndex, newName, AlbumModel::Roles::NameRole);
    }
}

/**
 * Elimina el álbum actualmente seleccionado
 *
 * Elimina el álbum de la base de datos y del modelo. Después de eliminar,
 * intenta seleccionar automáticamente otro álbum para mejorar la experiencia
 * del usuario:
 * 1. Primero intenta seleccionar el álbum anterior (fila - 1)
 * 2. Si no existe, intenta seleccionar el siguiente álbum (misma fila)
 * 3. Si no hay más álbumes, la UI se limpia automáticamente (por selectionChanged)
 *
 * Esta función también elimina todas las imágenes asociadas al álbum
 * (manejado por el modelo mediante la señal rowsRemoved).
 */
void AlbumWidget::deleteAlbum()
{
    // Verifica que haya un álbum seleccionado
    if (mAlbumSelectionModel->selectedIndexes().isEmpty()) {
        return;
    }

    // Obtiene el número de fila del álbum a eliminar
    int row = mAlbumSelectionModel->currentIndex().row();

    // Elimina el álbum del modelo (y automáticamente de la base de datos)
    mAlbumModel->removeRow(row);

    // SELECCIÓN AUTOMÁTICA POST-ELIMINACIÓN

    // Intenta seleccionar el álbum previo (fila anterior)
    QModelIndex previousModelIndex = mAlbumModel->index(row - 1, 0);
    if(previousModelIndex.isValid()) {
        mAlbumSelectionModel->setCurrentIndex(previousModelIndex,
                                              QItemSelectionModel::SelectCurrent);
        return;  // Selección exitosa, termina aquí
    }

    // Si no hay álbum previo, intenta seleccionar el siguiente
    // (que ahora ocupa la misma posición debido a la eliminación)
    QModelIndex nextModelIndex = mAlbumModel->index(row, 0);
    if(nextModelIndex.isValid()) {
        mAlbumSelectionModel->setCurrentIndex(nextModelIndex,
                                              QItemSelectionModel::SelectCurrent);
        return;  // Selección exitosa, termina aquí
    }

    // Si no hay álbumes disponibles, la señal selectionChanged limpiará la UI
}

/**
 * Establece el modelo proxy de imágenes (con miniaturas)
 * @param pictureModel Puntero al ThumbnailProxyModel que proporciona las miniaturas
 *
 * El ThumbnailProxyModel es un modelo proxy que envuelve al PictureModel
 * y proporciona funcionalidad adicional como la generación de miniaturas
 * de las imágenes para mostrarlas eficientemente en el ListView.
 *
 * Asocia el modelo proxy con el ListView de miniaturas para que pueda
 * mostrar las imágenes del álbum actual.
 */
void AlbumWidget::setPictureModel(ThumbnailProxyModel* pictureModel)
{
    // Almacena el puntero al modelo proxy de imágenes
    mPictureModel = pictureModel;

    // Asocia el modelo con el ListView para mostrar las miniaturas
    ui->thumbnailListView->setModel(pictureModel);
}

/**
 * Establece el modelo de selección para las imágenes
 * @param selectionModel Puntero al modelo de selección compartido para imágenes
 *
 * Permite sincronizar la selección de imágenes entre diferentes vistas.
 * Por ejemplo, si hay un PictureWidget separado, ambas vistas pueden
 * compartir la misma selección.
 */
void AlbumWidget::setSelectionModel(QItemSelectionModel* selectionModel)
{
    // Asocia el modelo de selección con el ListView de miniaturas
    ui->thumbnailListView->setSelectionModel(selectionModel);
}

/**
 * Establece el modelo de selección de imágenes (método alternativo)
 * @param selectionModel Puntero al modelo de selección para imágenes
 *
 * NOTA: Esta función parece ser redundante con setSelectionModel().
 * Ambas hacen exactamente lo mismo. Posiblemente una de ellas debería
 * eliminarse o renombrarse para mayor claridad.
 */
void AlbumWidget::setPictureSelectionModel(QItemSelectionModel* selectionModel)
{
    // Asocia el modelo de selección con el ListView de miniaturas
    ui->thumbnailListView->setSelectionModel(selectionModel);
}

/**
 * Añade una o más imágenes al álbum actualmente seleccionado
 *
 * Muestra un diálogo de selección de archivos que permite al usuario
 * elegir múltiples imágenes (archivos .jpg o .png) para añadir al álbum.
 *
 * Para cada imagen seleccionada:
 * 1. Crea un objeto Picture con la ruta del archivo
 * 2. Lo añade al modelo (que automáticamente lo guarda en la BD y copia el archivo)
 * 3. Después de añadir todas, selecciona la última imagen añadida
 *
 * Si el usuario cancela o no selecciona ningún archivo, no se hace nada.
 */
void AlbumWidget::addPictures()
{
    // Muestra el diálogo de selección de archivos
    QStringList filenames =
        QFileDialog::getOpenFileNames(
            this,                                    // Widget padre
            "Add pictures",                          // Título del diálogo
            QDir::homePath(),                        // Directorio inicial (carpeta home)
            "Picture files (*.jpg *.png)"           // Filtro de archivos
            );

    // Verifica que el usuario haya seleccionado al menos un archivo
    if (!filenames.isEmpty()) {
        QModelIndex lastModelIndex;  // Para almacenar el índice de la última imagen añadida

        // Itera sobre cada archivo seleccionado
        for (auto filename : filenames) {
            // Crea un objeto Picture con la ruta del archivo
            Picture picture(filename);

            // Añade la imagen al modelo
            // pictureModel() accede al PictureModel subyacente dentro del proxy
            // addPicture guarda la imagen en la BD y actualiza el modelo
            lastModelIndex = mPictureModel->pictureModel()->addPicture(picture);
        }

        // Selecciona la última imagen añadida en el ListView
        // Esto proporciona feedback visual al usuario de que las imágenes se añadieron
        ui->thumbnailListView->setCurrentIndex(lastModelIndex);
    }
}

/**
 * Limpia la interfaz de usuario cuando no hay álbum seleccionado
 *
 * Restablece la UI a su estado inicial:
 * - Limpia el nombre del álbum
 * - Oculta todos los botones de acción
 *
 * Esta función es llamada automáticamente cuando:
 * - Se deselecciona un álbum
 * - Se elimina el último álbum
 * - La selección se vuelve inválida por cualquier motivo
 */
void AlbumWidget::clearUi()
{
    // Limpia el texto del nombre del álbum
    ui->albumName->setText("");

    // Oculta los botones de acción (no tienen sentido sin álbum seleccionado)
    ui->deleteButton->setVisible(false);
    ui->editButton->setVisible(false);
    ui->addPictureButton->setVisible(false);
}
