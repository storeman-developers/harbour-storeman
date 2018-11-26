#ifndef ORNTAGLISTITEM_H
#define ORNTAGLISTITEM_H

#include <QString>

class QJsonObject;

struct OrnTagListItem
{
    OrnTagListItem(const QJsonObject &jsonObject);

    quint32 tagId;
    quint32 appsCount;
    QString name;
};

#endif // ORNTAGLISTITEM_H
