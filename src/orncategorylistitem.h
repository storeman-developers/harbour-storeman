#pragma once

#include <QString>
#include <QList>

class QJsonObject;

struct OrnCategoryListItem
{
    friend class OrnCategoriesModel;

    OrnCategoryListItem(const QJsonObject &data);

    static QString categoryName(quint32 tid);

    quint32 categoryId;
    quint32 appsCount;
    quint32 depth;
    QString name;
    QList<quint32> parents;
};
