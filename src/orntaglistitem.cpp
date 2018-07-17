#include "orntaglistitem.h"
#include "orn.h"

#include <QJsonObject>


OrnTagListItem::OrnTagListItem(const QJsonObject &jsonObject)
    : tagId(Orn::toUint(jsonObject[QStringLiteral("tid")]))
    , appsCount(Orn::toUint(jsonObject[QStringLiteral("apps_count")]))
    , name(Orn::toString(jsonObject[QStringLiteral("name")]))
{}
