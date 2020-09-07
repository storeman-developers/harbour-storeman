#pragma once

#include <QString>

class QJsonObject;

struct OrnAppListItem
{
    OrnAppListItem(const QJsonObject &data);

    bool valid;
    quint32 appId;
    quint32 created;
    quint32 updated;
    quint32 ratingCount;
    float rating;
    QString title;
    QString userName;
    QString iconSource;
    QString sinceUpdate;
    QString category;
    quint32 categoryId;
    QString package;

private:
    static QString sinceLabel(quint32 value);
};
