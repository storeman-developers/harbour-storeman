#include "orncommentlistitem.h"
#include "orn.h"

#include <QJsonObject>


OrnCommentListItem::OrnCommentListItem(const QJsonObject &jsonObject)
    : commentId(Orn::toUint(jsonObject[QStringLiteral("cid")]))
    , parentId(Orn::toUint(jsonObject[QStringLiteral("pid")]))
    , created(Orn::toUint(jsonObject[QStringLiteral("created")]))
    , text(Orn::toString(jsonObject[QStringLiteral("text")]))
{
    auto user = jsonObject[QStringLiteral("user")].toObject();
    userId = Orn::toUint(user[QStringLiteral("uid")]);
    userName = Orn::toString(user[QStringLiteral("name")]);
    userIconSource = Orn::toString(user[QStringLiteral("picture")].toObject()[QStringLiteral("url")]);
}
