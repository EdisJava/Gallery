#include "picturedelegate.h"
#include <QPainter>

/**
 * Constantes de configuración visual del delegate
 *
 * Se definen como constantes globales para:
 * - Centralizar la configuración
 * - Facilitar ajustes de estilo
 * - Evitar valores mágicos dentro del código
 */
const unsigned int BANNER_HEIGHT      = 20;       // Altura del banner inferior/superior
const unsigned int BANNER_COLOR       = 0x303030; // Color de fondo del banner (gris oscuro)
const unsigned int BANNER_ALPHA       = 200;      // Nivel de transparencia del banner
const unsigned int BANNER_TEXT_COLOR  = 0xffffff; // Color del texto (blanco)
const unsigned int HIGHLIGHT_ALPHA    = 100;      // Transparencia del resaltado de selección

/**
 * Constructor de PictureDelegate
 * @param parent Objeto padre dentro de la jerarquía de Qt
 *
 * PictureDelegate se encarga de personalizar la forma en la que
 * se dibujan las imágenes (thumbnails) dentro de una vista.
 */
PictureDelegate::PictureDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
}

/**
 * Método principal de dibujo del delegate
 * @param painter Objeto encargado de dibujar en pantalla
 * @param option Opciones de estilo (estado, selección, rectángulo, etc.)
 * @param index Índice del modelo correspondiente al elemento a pintar
 *
 * Este método se llama automáticamente por Qt para cada elemento visible.
 * Aquí se dibuja:
 * 1. La imagen (thumbnail)
 * 2. Un banner semitransparente con el nombre del archivo
 * 3. Un overlay de selección si el elemento está seleccionado
 */
void PictureDelegate::paint(QPainter* painter,
                            const QStyleOptionViewItem& option,
                            const QModelIndex& index) const
{
    // Guarda el estado actual del painter para no afectar otros elementos
    painter->save();

    /**
     * =========================
     * DIBUJO DE LA IMAGEN
     * =========================
     */

    // Obtiene el QPixmap desde el modelo usando DecorationRole
    QPixmap pixmap = index.model()->data(index,
                                         Qt::DecorationRole).value<QPixmap>();

    // Dibuja la imagen en el rectángulo asignado al item
    painter->drawPixmap(option.rect.x(),
                        option.rect.y(),
                        pixmap);

    /**
     * =========================
     * DIBUJO DEL BANNER
     * =========================
     */

    // Define el rectángulo del banner (sobre la imagen)
    QRect bannerRect(option.rect.x(),
                     option.rect.y(),
                     pixmap.width(),
                     BANNER_HEIGHT);

    // Configura el color del banner con transparencia
    QColor bannerColor = QColor(BANNER_COLOR);
    bannerColor.setAlpha(BANNER_ALPHA);

    // Rellena el banner con el color configurado
    painter->fillRect(bannerRect, bannerColor);

    /**
     * =========================
     * TEXTO DEL BANNER
     * =========================
     */

    // Obtiene el nombre del archivo desde el modelo
    QString filename = index.model()->data(index,
                                           Qt::DisplayRole).toString();

    // Establece el color del texto
    painter->setPen(BANNER_TEXT_COLOR);

    // Dibuja el nombre del archivo centrado en el banner
    painter->drawText(bannerRect, Qt::AlignCenter, filename);

    /**
     * =========================
     * RESALTADO DE SELECCIÓN
     * =========================
     */

    // Si el item está seleccionado, dibuja un overlay semitransparente
    if (option.state.testFlag(QStyle::State_Selected)) {
        QColor selectedColor = option.palette.highlight().color();
        selectedColor.setAlpha(HIGHLIGHT_ALPHA);
        painter->fillRect(option.rect, selectedColor);
    }

    // Restaura el estado original del painter
    painter->restore();
}

/**
 * Devuelve el tamaño recomendado para el item
 * @param index Índice del modelo correspondiente al elemento
 * @return Tamaño basado en el tamaño real de la imagen
 *
 * Qt utiliza este método para calcular:
 * - El layout de la vista
 * - El espaciado entre elementos
 */
QSize PictureDelegate::sizeHint(const QStyleOptionViewItem&,
                                const QModelIndex& index) const
{
    // Obtiene el pixmap asociado al item
    const QPixmap& pixmap = index.model()->data(index,
                                                Qt::DecorationRole).value<QPixmap>();

    // El tamaño del item coincide con el tamaño de la imagen
    return pixmap.size();
}
