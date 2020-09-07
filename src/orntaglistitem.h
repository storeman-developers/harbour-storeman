#pragma once

#include <QString>

class QJsonObject;

struct OrnTagListItem
{
    OrnTagListItem(const QJsonObject &data);

    quint32 tagId;
    quint32 appsCount;
    QString name;
};
