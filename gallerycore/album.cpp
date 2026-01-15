#include "album.h"


Album::Album(const QString& name)
    : mID(-1),
    mName(name)
{
}

int Album::id() const
{
    return mID;
}

void Album::setID(int id)
{
    mID = id;
}

QString Album::name() const
{
    return mName;
}

void Album::setName(const QString& name)
{
    mName = name;
}

