#ifndef ORNAPPLISTITEM_H
#define ORNAPPLISTITEM_H

#include <QString>

class QJsonObject;

struct OrnAppListItem
{
    OrnAppListItem(const QJsonObject &jsonObject);

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
    QString package;

private:
    static QString sinceLabel(quint32 value);
};

#endif // ORNAPPLISTITEM_H
