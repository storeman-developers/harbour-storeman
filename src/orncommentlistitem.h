#pragma once

#include <QString>

class QJsonObject;

struct OrnCommentListItem
{
    OrnCommentListItem(const QJsonObject &data);

    quint32 commentId;
    quint32 parentId;
    quint32 created;
    quint32 userId;
    QString userName;
    QString userIconSource;
    QString text;
};
