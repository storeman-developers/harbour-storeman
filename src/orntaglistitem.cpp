#include "orntaglistitem.h"
#include "ornutils.h"

#include <QJsonObject>


OrnTagListItem::OrnTagListItem(const QJsonObject &jsonObject)
    : tagId(OrnUtils::toUint(jsonObject[QStringLiteral("tid")]))
    , appsCount(OrnUtils::toUint(jsonObject[QStringLiteral("apps_count")]))
    , name(OrnUtils::toString(jsonObject[QStringLiteral("name")]))
{}
