#include "orncommentlistitem.h"
#include "ornutils.h"

#include <QJsonObject>


OrnCommentListItem::OrnCommentListItem(const QJsonObject &jsonObject)
    : commentId(OrnUtils::toUint(jsonObject[QStringLiteral("cid")]))
    , parentId(OrnUtils::toUint(jsonObject[QStringLiteral("pid")]))
    , created(OrnUtils::toUint(jsonObject[QStringLiteral("created")]))
    , text(OrnUtils::toString(jsonObject[QStringLiteral("text")]))
{
    auto user = jsonObject[QStringLiteral("user")].toObject();
    userId = OrnUtils::toUint(user[QStringLiteral("uid")]);
    userName = OrnUtils::toString(user[QStringLiteral("name")]);
    userIconSource = OrnUtils::toString(user[QStringLiteral("picture")].toObject()[QStringLiteral("url")]);
}
