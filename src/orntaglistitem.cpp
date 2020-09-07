#include "orntaglistitem.h"
#include "ornutils.h"
#include "ornconst.h"

#include <QJsonObject>


OrnTagListItem::OrnTagListItem(const QJsonObject &data)
    : tagId(OrnUtils::toUint(data[OrnConst::tid]))
    , appsCount(OrnUtils::toUint(data[OrnConst::appsCount]))
    , name(OrnUtils::toString(data[OrnConst::name]))
{}
