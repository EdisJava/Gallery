#ifndef ALBUM_H
#define ALBUM_H

#include <QString>
#include <QUrl>
#include <QMetaType>  // Asegúrate de incluir este

class Album
{
public:
    Album();
    Album(const QString& name);  // <--- Agrega esta línea

    int id() const;
    void setId(int id);

    QString name() const;
    void setName(const QString &name);

private:
    int mId;
    QString mName;
};


// ¡Q_DECLARE_METATYPE debe estar FUERA de la clase, justo aquí!
Q_DECLARE_METATYPE(Album*)

#endif // ALBUM_H
