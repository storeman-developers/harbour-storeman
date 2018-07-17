#include "ornapirequest.h"
#include "orn.h"
#include "ornclient.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonParseError>

const QString OrnApiRequest::apiUrlPrefix(QStringLiteral("https://openrepos.net/api/v1/"));
const QByteArray OrnApiRequest::langName(QByteArrayLiteral("Accept-Language"));
const QByteArray OrnApiRequest::langValue(QLocale::system().name().left(2).toUtf8());
const QByteArray OrnApiRequest::platformName(QByteArrayLiteral("Warehouse-Platform"));
const QByteArray OrnApiRequest::platformValue(QByteArrayLiteral("SailfishOS"));

OrnApiRequest::OrnApiRequest(QObject *parent)
    : QObject(parent)
    , mNetworkReply(nullptr)
{}

OrnApiRequest::~OrnApiRequest()
{
    if (mNetworkReply)
    {
        mNetworkReply->deleteLater();
    }
}

void OrnApiRequest::run(const QNetworkRequest &request)
{
    if (mNetworkReply)
    {
        qDebug() << "Request is already running";
        return;
    }
    qDebug() << "Fetching data from" << request.url().toString();
    emit this->runningChanged(true);
    mNetworkReply = Orn::networkAccessManager()->get(request);
    connect(mNetworkReply, &QNetworkReply::finished, this, &OrnApiRequest::onReplyFinished);
}

bool OrnApiRequest::running() const
{
    return mNetworkReply != nullptr;
}

QNetworkRequest OrnApiRequest::networkRequest(const QUrl &url)
{
    QNetworkRequest request;
    auto client = OrnClient::instance();
    if (client->authorised())
    {
        request = client->authorisedRequest();
    }
    request.setRawHeader(langName, langValue);
    request.setRawHeader(platformName, platformValue);
    request.setUrl(url);
    return request;
}

void OrnApiRequest::reset()
{
    if (mNetworkReply)
    {
        mNetworkReply->deleteLater();
        mNetworkReply = nullptr;
        emit this->runningChanged(false);
    }
}

void OrnApiRequest::onReplyFinished()
{
    if (mNetworkReply->error() != QNetworkReply::NoError)
    {
        qDebug() << "Network request error" << mNetworkReply->error()
                 << "-" << mNetworkReply->errorString();
        this->reset();
        return;
    }

    QJsonParseError error;
    auto jsonDoc = QJsonDocument::fromJson(mNetworkReply->readAll(), &error);
    if (error.error != QJsonParseError::NoError)
    {
        qCritical() << "Could not parse reply:" << error.errorString();
        this->reset();
        return;
    }

    emit this->jsonReady(jsonDoc);

    this->reset();
}
