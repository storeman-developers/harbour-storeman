#include "ornbookmarksmodel.h"
#include "ornapplistitem.h"
#include "ornclient.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>

OrnBookmarksModel::OrnBookmarksModel(QObject *parent)
    : OrnAbstractAppsModel(false, parent)
{
    auto client = OrnClient::instance();
    connect(client, &OrnClient::bookmarkChanged, this,
            [this, client](quint32 appId, bool bookmarked)
    {
        if (this->canFetchMore(QModelIndex()))
        {
            // Model was not initialized yet so just ignore the signal
            return;
        }

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
            for (size_t i = 0, s = mData.size(); i < s; ++i)
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

    auto client = OrnClient::instance();
    auto bookmarks = client->bookmarks();
    auto size = bookmarks.size();

    if (size == 0)
    {
        return;
    }

    mFetching = true;
    emit this->fetchingChanged();

    QString resourceTmpl(QStringLiteral("apps/%1/compact"));

    for (const auto &appid : bookmarks)
    {
        auto request = client->apiRequest(resourceTmpl.arg(appid));
        qDebug() << "Fetching data from" << request.url().toString();
        auto reply = client->networkAccessManager()->get(request);
        connect(reply, &QNetworkReply::finished, this,
                [this, client, size, reply, appid]()
        {
            auto doc = client->processReply(reply);
            if (doc.isObject())
            {
                mFetchedApps.append(doc.object());
            }
            else
            {
                QJsonObject badApp;
                badApp.insert(QStringLiteral("appid"), QString::number(appid));
                mFetchedApps.append(badApp);
            }
            if (mFetchedApps.size() == size)
            {
                this->processReply(QJsonDocument(mFetchedApps));
                mFetchedApps = QJsonArray();
                mFetching = false;
                emit this->fetchingChanged();
            }
        });
    }
}
