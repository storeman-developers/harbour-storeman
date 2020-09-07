#include "orncommentlistitem.h"
#include "ornutils.h"
#include "ornconst.h"

#include <QJsonObject>


OrnCommentListItem::OrnCommentListItem(const QJsonObject &data)
    : commentId(OrnUtils::toUint(data[OrnConst::cid]))
    , parentId(OrnUtils::toUint(data[OrnConst::pid]))
    , created(OrnUtils::toUint(data[OrnConst::created]))
    , text(OrnUtils::toString(data[OrnConst::text]))
{
    auto user      = data[OrnConst::user].toObject();
    userId         = OrnUtils::toUint(user[OrnConst::uid]);
    userName       = OrnUtils::toString(user[OrnConst::name]);
    userIconSource = OrnUtils::toString(user[OrnConst::picture].toObject()[OrnConst::url]);
}
