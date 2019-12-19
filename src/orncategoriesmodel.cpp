#include "orncategoriesmodel.h"
#include "ornclient.h"

#include <QUrl>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <functional>


OrnCategoriesModel::OrnCategoriesModel(QObject *parent)
    : OrnAbstractListModel(false, parent)
{
    connect(OrnClient::instance(), &OrnClient::categoryVisibilityChanged,
            this, [this](quint32 categoryId, bool visible)
    {
        Q_UNUSED(visible)

        for (size_t i = 0, size = mData.size(); i < size; ++i)
        {
            if (mData[i].categoryId == categoryId)
            {
                auto ind = this->createIndex(i, 0);
                emit this->dataChanged(ind, ind, {VisibilityRole});
                return;
            }
        }
    });
}

QVariantList childCategories(const std::deque<OrnCategoryListItem> &data, quint32 parentID)
{
    QVariantList res;
    for (const auto &cat : data)
    {
        if (cat.parents.contains(parentID)) {
            res.append(cat.categoryId);
        }
    }
    return res;
}

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
    case VisibilityRole:
        return OrnClient::instance()->categoryVisible(category.categoryId);
    case ChildrenRole:
        return childCategories(mData, category.categoryId);
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
        { NameRole,       "name" },
        { VisibilityRole, "visible" },
        { ChildrenRole,   "children" }
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

    QString childrenKey(QStringLiteral("childrens"));
    std::function<bool(const OrnCategoryListItem &a, const OrnCategoryListItem &b)> compare =
            [](const OrnCategoryListItem &a, const OrnCategoryListItem &b) -> bool
    {
        return a.name < b.name;
    };
    std::function<void(std::deque<OrnCategoryListItem>&, const QJsonObject&)> parse;
    parse = [&childrenKey, &compare, &parse](std::deque<OrnCategoryListItem> &data, const QJsonObject &jsonObject)
    {
        data.emplace_back(jsonObject);
        if (jsonObject.contains(childrenKey))
        {
            auto childrenArray = jsonObject[childrenKey].toArray();
            for (const QJsonValueRef child : childrenArray)
            {
                parse(data, child.toObject());
            }
            auto end = data.end();
            std::sort(end - childrenArray.size(), end, compare);
        }
    };

    for (const QJsonValueRef category : categoriesArray)
    {
        parse(mData, category.toObject());
    }

    auto size = mData.size();
    this->beginInsertRows(QModelIndex(), 0, size - 1);
    this->endInsertRows();
    qDebug() << size << "items have been added to the model";
    mFetching = false;
    mCanFetchMore = false;
    emit this->fetchingChanged();
}
