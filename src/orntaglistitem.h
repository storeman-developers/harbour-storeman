#ifndef ORNTAGLISTITEM_H
#define ORNTAGLISTITEM_H


#include "ornabstractlistitem.h"

class QJsonObject;

struct OrnTagListItem : public OrnAbstractListItem
{
    OrnTagListItem(const QJsonObject &jsonObject);

    quint32 tagId;
    quint32 appsCount;
    QString name;
};

#endif // ORNTAGLISTITEM_H
