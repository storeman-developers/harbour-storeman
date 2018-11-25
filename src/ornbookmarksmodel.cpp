#include "ornbookmarksmodel.h"
#include "ornapplistitem.h"
#include "ornclient.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>

OrnBookmarksModel::OrnBookmarksModel(QObject *parent) :
    OrnAbstractAppsModel(false, parent)
{
    connect(OrnClient::instance(), &OrnClient::bookmarkChanged,
            this, &OrnBookmarksModel::onBookmarkChanged);
}

void OrnBookmarksModel::onBookmarkChanged(quint32 appId, bool bookmarked)
{
    if (bookmarked)
    {
        this->addApp(appId);
    }
    else
    {
        auto s = mData.size();
        for (int i = 0; i < s; ++i)
        {
            auto app = static_cast<OrnAppListItem *>(mData[i]);
            if (app->appId == appId)
            {
                qDebug() << "Removing app" << appId << "from bookmarks model";
                this->beginRemoveRows(QModelIndex(), i, i);
                mData.removeAt(i);
                this->endRemoveRows();
                delete app;
                return;
            }
        }
    }
}

void OrnBookmarksModel::addApp(quint32 appId)
{
    qDebug() << "Fetching app" << appId << "to add to bookmarks model";
    auto client = OrnClient::instance();
    auto request = client->apiRequest(QStringLiteral("apps/%1/compact").arg(appId));
    auto reply = client->networkAccessManager()->get(request);
    connect(reply, &QNetworkReply::finished, [this, client, reply]()
    {
        auto doc = client->processReply(reply);
        if (doc.isObject())
        {
            QJsonArray arr({doc.object()});
            this->onJsonReady(QJsonDocument(arr));
        }
    });
}

void OrnBookmarksModel::fetchMore(const QModelIndex &parent)
{
    if (parent.isValid())
    {
        return;
    }

    for (const auto &appid : OrnClient::instance()->bookmarks())
    {
        this->addApp(appid);
    }
}
