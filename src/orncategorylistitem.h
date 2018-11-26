#ifndef ORNCATEGORYLISTITEM_H
#define ORNCATEGORYLISTITEM_H

#include <QString>

class QJsonObject;

struct OrnCategoryListItem
{
    friend class OrnCategoriesModel;

    OrnCategoryListItem(const QJsonObject &jsonObject);

    static QString categoryName(const quint32 &tid);

    quint32 categoryId;
    quint32 appsCount;
    quint32 depth;
    QString name;

private:
    static QList<OrnCategoryListItem> parse(const QJsonObject &jsonObject);
};

#endif // ORNCATEGORYLISTITEM_H
