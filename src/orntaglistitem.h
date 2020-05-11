#pragma once

#include <QString>

class QJsonObject;

struct OrnTagListItem
{
    OrnTagListItem(const QJsonObject &jsonObject);

    quint32 tagId;
    quint32 appsCount;
    QString name;
};
