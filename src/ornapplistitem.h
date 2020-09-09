#pragma once

#include <QString>
#include <QDateTime>

class QJsonObject;

struct OrnAppListItem
{
    OrnAppListItem(const QJsonObject &data);

    bool valid;
    quint32 appId;
    quint32 ratingCount;
    float rating;
    QString title;
    QString userName;
    QString iconSource;
    QString sinceUpdate;
    QString category;
    quint32 categoryId;
    QString package;
    QDateTime createDate;
};
