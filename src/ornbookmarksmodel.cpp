#include "ornbookmarksmodel.h"
#include "ornapplistitem.h"
#include "ornclient.h"
#include "orn.h"

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

void OrnBookmarksModel::addApp(const quint32 &appId)
{
    qDebug() << "Adding app" << appId << "to bookmarks model";
    auto url = OrnApiRequest::apiUrl(QStringLiteral("apps/%1/compact").arg(appId));
    auto request = OrnApiRequest::networkRequest(url);
    auto reply = Orn::networkAccessManager()->get(request);
    connect(reply, &QNetworkReply::finished, [this, reply]()
    {
        if (reply->error() == QNetworkReply::NoError)
        {
            QJsonParseError error;
            auto jsonDoc = QJsonDocument::fromJson(reply->readAll(), &error);
            if (error.error == QJsonParseError::NoError)
            {
                QJsonArray arr;
                arr.append(jsonDoc.object());
                emit mApiRequest->jsonReady(QJsonDocument(arr));
            }
            else
            {
                qCritical() << "Could not parse reply:" << error.errorString();
            }
        }
        else
        {
            qDebug() << "Network request error" << reply->error()
                     << "-" << reply->errorString();
        }
        reply->deleteLater();
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
