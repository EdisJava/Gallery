#ifndef ALBUM_H
#define ALBUM_H

#include <QString>

#include "gallerycore_global.h"

class GALLERYCORE_EXPORT Album
{
public:
    explicit Album(const QString& name = "");

    int id() const;
    void setID (int id);
    QString name () const;
    void setName(const QString& name);



private:
    int mID;
    QString mName;

};

#endif // ALBUM_H
