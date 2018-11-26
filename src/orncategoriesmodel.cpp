#include "orncategoriesmodel.h"

#include <QUrl>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

OrnCategoriesModel::OrnCategoriesModel(QObject *parent)
    : OrnAbstractListModel(false, parent)
{}

QVariant OrnCategoriesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    const auto &category = mData[index.row()];
    switch (role) {
    case CategoryIdRole:
        return category.categoryId;
    case AppsCountRole:
        return category.appsCount;
    case DepthRole:
        return category.depth;
    case NameRole:
        return category.name;
    default:
        return QVariant();
    }
}

void OrnCategoriesModel::fetchMore(const QModelIndex &parent)
{
    if (parent.isValid())
    {
        return;
    }
    OrnAbstractListModel::fetch(QStringLiteral("categories"));
}

QHash<int, QByteArray> OrnCategoriesModel::roleNames() const
{
    return {
        { CategoryIdRole, "categoryId" },
        { AppsCountRole,  "appsCount" },
        { DepthRole,      "depth" },
        { NameRole,       "name" }
    };
}

void OrnCategoriesModel::processReply(const QJsonDocument &jsonDoc)
{
    auto categoriesArray = jsonDoc.array();
    if (categoriesArray.isEmpty())
    {
        qWarning() << "Api reply is empty";
        return;
    }

    QList<OrnCategoryListItem> list;
    for (const QJsonValueRef category : categoriesArray)
    {
        list.append(OrnCategoryListItem::parse(category.toObject()));
    }
    this->beginInsertRows(QModelIndex(), 0, list.size() - 1);
    mData = list;
    this->endInsertRows();
    qDebug() << list.size() << "items have been added to the model";
    mFetching = false;
    mCanFetchMore = false;
    emit this->fetchingChanged();
}
