#ifndef ORNCATEGORYLISTITEM_H
#define ORNCATEGORYLISTITEM_H


#include "ornabstractlistitem.h"

#include <QMap>

class QJsonObject;

struct OrnCategoryListItem : public OrnAbstractListItem
{
    friend class OrnCategoriesModel;

    OrnCategoryListItem(const QJsonObject &jsonObject);

    static QString categoryName(const quint32 &tid);

    quint32 categoryId;
    quint32 appsCount;
    quint32 depth;
    QString name;

private:
    static OrnItemList parse(const QJsonObject &jsonObject);

    static const QMap<quint32, const char*> categories;
};

#endif // ORNCATEGORYLISTITEM_H
