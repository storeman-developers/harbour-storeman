#ifndef ORNCOMMENTLISTITEM_H
#define ORNCOMMENTLISTITEM_H


#include "ornabstractlistitem.h"

class QJsonObject;

struct OrnCommentListItem : public OrnAbstractListItem
{
    OrnCommentListItem(const QJsonObject &jsonObject);

    quint32 commentId;
    quint32 parentId;
    quint32 created;
    quint32 userId;
    QString userName;
    QString userIconSource;
    QString text;
};

#endif // ORNCOMMENTLISTITEM_H
