#include "ornbookmarksmodel.h"
#include "ornapplistitem.h"
#include "ornclient.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>

OrnBookmarksModel::OrnBookmarksModel(QObject *parent)
    : OrnAbstractAppsModel(false, parent)
{
    auto client = OrnClient::instance();
    connect(OrnClient::instance(), &OrnClient::bookmarkChanged,
            [this, client](quint32 appId, bool bookmarked)
    {
        if (bookmarked)
        {
            auto request = client->apiRequest(QStringLiteral("apps/%1/compact").arg(appId));
            qDebug() << "Fetching data from" << request.url().toString();
            auto reply = client->networkAccessManager()->get(request);
            connect(reply, &QNetworkReply::finished, [this, client, reply, appId]()
            {
                auto doc = client->processReply(reply);
                if (doc.isObject())
                {
                    qDebug() << "Adding app" << appId << "to bookmarks model";
                    QJsonArray arr({doc.object()});
                    this->processReply(QJsonDocument(arr));
                }
            });
        }
        else
        {
            auto s = mData.size();
            for (size_t i = 0; i < s; ++i)
            {
                if (mData[i].appId == appId)
                {
                    qDebug() << "Removing app" << appId << "from bookmarks model";
                    this->beginRemoveRows(QModelIndex(), i, i);
                    mData.erase(mData.begin() + i);
                    mData.shrink_to_fit();
                    this->endRemoveRows();
                    return;
                }
            }
        }
    });
}

void OrnBookmarksModel::fetchMore(const QModelIndex &parent)
{
    if (parent.isValid())
    {
        return;
    }

    mFetching = true;
    emit this->fetchingChanged();

    auto client = OrnClient::instance();
    auto bookmarks = client->bookmarks();
    auto size = bookmarks.size();
    QString resourceTmpl(QStringLiteral("apps/%1/compact"));

    for (const auto &appid : bookmarks)
    {
        auto request = client->apiRequest(resourceTmpl.arg(appid));
        qDebug() << "Fetching data from" << request.url().toString();
        auto reply = client->networkAccessManager()->get(request);
        connect(reply, &QNetworkReply::finished, [this, client, size, reply]()
        {
            auto doc = client->processReply(reply);
            if (doc.isObject())
            {
                mFetchedApps.append(doc.object());
                if (mFetchedApps.size() == size)
                {
                    this->processReply(QJsonDocument(mFetchedApps));
                    mFetchedApps = QJsonArray();
                    mFetching = false;
                    emit this->fetchingChanged();
                }
            }
        });
    }
}
