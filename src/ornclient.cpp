#include "ornclient.h"
#include "ornclient_p.h"
#include "ornutils.h"

#include <QSettings>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkCookie>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QStandardPaths>
#include <QDataStream>
#include <QDir>
#include <QGuiApplication>

#include <QDebug>

#define API_URL_PREFIX      QStringLiteral("https://openrepos.net/api/v1/")

#define USER_COOKIE         QStringLiteral("user/cookie")
#define USER_COOKIE_EXPIRE  QStringLiteral("user/cookie/expire")
#define USER_TOKEN          QStringLiteral("user/token")
#define USER_UID            QStringLiteral("user/uid")
#define USER_NAME           QStringLiteral("user/name")
#define USER_REALNAME       QStringLiteral("user/realname")
#define USER_MAIL           QStringLiteral("user/mail")
#define USER_CREATED        QStringLiteral("user/created")
#define USER_PICTURE        QStringLiteral("user/picture/url")

#define APPLICATION_JSON    QByteArrayLiteral("application/json")


OrnClient *OrnClientPrivate::instance = nullptr;

OrnClientPrivate::OrnClientPrivate(OrnClient *q_ptr)
    : settings(new QSettings(q_ptr))
    , cookieTimer(new QTimer(q_ptr))
    , nam(new QNetworkAccessManager(q_ptr))
    , lang(QLocale::system().name().left(2).toUtf8())
{
    if (settings->contains(USER_TOKEN) && settings->contains(USER_COOKIE))
    {
        auto cookie = settings->value(USER_COOKIE).toByteArray();
        userCookie = QVariant::fromValue(QNetworkCookie::parseCookies(cookie).first());
        userToken = settings->value(USER_TOKEN).toByteArray();
    }

    // Read ids of bookmarked apps
    auto path = QStandardPaths::locate(
                QStandardPaths::AppLocalDataLocation, QStringLiteral("bookmarks"));
    if (!path.isEmpty())
    {
        QFile file(path);
        if (file.open(QFile::ReadOnly))
        {
            qDebug() << "Reading bookmarks file" << path;
            QDataStream stream(&file);
            stream >> bookmarks;
        }
        else
        {
            qWarning() << "Could not read bookmarks file" << path;
        }
    }
}

OrnClientPrivate::~OrnClientPrivate()
{
    // Write ids of bookmarked apps
    auto path = OrnUtils::locate(QStringLiteral("bookmarks"));
    QFile file(path);
    if (file.open(QFile::WriteOnly))
    {
        qDebug() << "Writing bookmarks file" << path;
        QDataStream stream(&file);
        stream << bookmarks;
    }
    else
    {
        qWarning() << "Could not write bookmarks file" << path;
    }
}

void OrnClientPrivate::prepareComment(QJsonObject &object, const QString &body)
{
    QJsonObject bodyObject;
    bodyObject.insert(QStringLiteral("value"), body);
    bodyObject.insert(QStringLiteral("format"), QStringLiteral("filtered_html"));
    QJsonArray undArray;
    undArray.append(bodyObject);
    QJsonObject undObject;
    undObject.insert(QStringLiteral("und"), undArray);

    object.insert(QStringLiteral("comment_body"), undObject);
}

OrnClient::OrnClient(QObject *parent)
    : QObject(parent)
    , d_ptr(new OrnClientPrivate(this))
{
    // Check if authorisation has expired
    if (this->authorised())
    {
        qDebug() << "Checking authorisation status";
        auto request = this->apiRequest(QStringLiteral("session"));
        auto reply = d_ptr->nam->get(request);
        connect(reply, &QNetworkReply::finished, [this, reply]()
        {
#ifdef QT_DEBUG
            if (this->processReply(reply).object().contains(QStringLiteral("token")))
            {
                qDebug() << "Client is authorised";
            }
#else
            this->processReply(reply);
#endif
        });
    }

    // A workaround as qml does not call a destructor
    connect(qApp, &QGuiApplication::aboutToQuit, this, &OrnClient::deleteLater);

    // Configure cookie timer
    d_ptr->cookieTimer->setSingleShot(true);
    QTimer::singleShot(1000, this, &OrnClient::setCookieTimer);
}

OrnClient::~OrnClient()
{
    delete d_ptr;
}

OrnClient *OrnClient::instance()
{
    if(!OrnClientPrivate::instance)
    {
        OrnClientPrivate::instance = new OrnClient();
    }
    return OrnClientPrivate::instance;
}

QNetworkRequest OrnClient::apiRequest(const QString &resource, const QUrlQuery &query) const
{
    QNetworkRequest request;
    if (this->cookieIsValid())
    {
        request.setHeader(QNetworkRequest::CookieHeader, d_ptr->userCookie);
        request.setRawHeader(QByteArrayLiteral("X-CSRF-Token"), d_ptr->userToken);
    }
    request.setRawHeader(QByteArrayLiteral("Accept-Language"), d_ptr->lang);
    request.setRawHeader(QByteArrayLiteral("Warehouse-Platform"),
                         QByteArrayLiteral("SailfishOS"));
    QUrl url(API_URL_PREFIX.append(resource));
    if (!query.isEmpty())
    {
        url.setQuery(query);
    }
    request.setUrl(url);
    return request;
}

QNetworkAccessManager *OrnClient::networkAccessManager() const
{
    return d_ptr->nam;
}

QJsonDocument OrnClient::processReply(QNetworkReply *reply, Error code)
{
    QJsonDocument doc;

    if (reply->error() == QNetworkReply::NoError)
    {
#ifdef QT_DEBUG
        QJsonParseError error;
        doc = QJsonDocument::fromJson(reply->readAll(), &error);
        if (error.error != QJsonParseError::NoError)
        {
            qCritical() << "Could not parse reply:" << error.errorString();
        }
#else
        doc = QJsonDocument::fromJson(reply->readAll());
#endif
    }
    else
    {
        qWarning() << "Network request error" << reply->error() << "-" << reply->errorString();
        auto status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (status == 403 && this->cookieIsValid())
        {
            d_ptr->settings->remove(USER_COOKIE_EXPIRE);
            this->setCookieTimer();
            emit this->cookieIsValidChanged();
        }
        else
        {
            emit this->error(code);
        }
    }

    reply->deleteLater();
    return doc;
}

bool OrnClient::authorised() const
{
    return !d_ptr->userToken.isNull() && d_ptr->userCookie.isValid();
}

bool OrnClient::cookieIsValid() const
{
    QString cookieExpireKey(USER_COOKIE_EXPIRE);
    return d_ptr->settings->contains(cookieExpireKey) &&
           d_ptr->settings->value(cookieExpireKey).toDateTime() > QDateTime::currentDateTime();
}

quint32 OrnClient::userId() const
{
    return d_ptr->settings->value(USER_UID).toUInt();
}

QString OrnClient::userName() const
{
    return d_ptr->settings->value(USER_NAME).toString();
}

QString OrnClient::userIconSource() const
{
    return d_ptr->settings->value(USER_PICTURE).toString();
}

QList<quint32> OrnClient::bookmarks() const
{
    return d_ptr->bookmarks.toList();
}

bool OrnClient::hasBookmark(quint32 appId) const
{
    return d_ptr->bookmarks.contains(appId);
}

bool OrnClient::addBookmark(quint32 appId)
{
    auto ok = !d_ptr->bookmarks.contains(appId);
    if (ok)
    {
        qDebug() << "Adding to bookmarks app id" << appId;
        d_ptr->bookmarks.insert(appId);
        emit this->bookmarkChanged(appId, true);
    }
    return ok;
}

bool OrnClient::removeBookmark(quint32 appId)
{
    auto ok = d_ptr->bookmarks.remove(appId);
    if (ok)
    {
        qDebug() << "Removing from bookmarks app id" << appId;
        emit this->bookmarkChanged(appId, false);
    }
    return ok;
}

void OrnClient::login(const QString &username, const QString &password)
{
    // Remove old credentials and stop timer
    d_ptr->settings->remove(QStringLiteral("user"));
    this->setCookieTimer();

    QNetworkRequest request;
    request.setUrl(API_URL_PREFIX.append(QStringLiteral("user/login")));
    request.setHeader(QNetworkRequest::ContentTypeHeader, APPLICATION_JSON);

    QJsonObject jsonObject;
    jsonObject.insert(QStringLiteral("username"), username);
    jsonObject.insert(QStringLiteral("password"), password);
    QJsonDocument jsonDoc(jsonObject);

    auto reply = d_ptr->nam->post(request, jsonDoc.toJson());
    connect(reply, &QNetworkReply::finished, [this, reply]()
    {
        auto cookieVariant = reply->header(QNetworkRequest::SetCookieHeader);
        auto jsonDoc = this->processReply(reply, AuthorisationError);
        if (cookieVariant.isValid() && jsonDoc.isObject())
        {
            auto jsonObject = jsonDoc.object();

            auto cookie = cookieVariant.value<QList<QNetworkCookie>>().first();
            auto settings = d_ptr->settings;
            d_ptr->userCookie = cookie.toRawForm(QNetworkCookie::NameAndValueOnly);
            settings->setValue(USER_COOKIE, d_ptr->userCookie);
            settings->setValue(USER_COOKIE_EXPIRE, cookie.expirationDate());

            d_ptr->userToken = OrnUtils::toString(jsonObject[QStringLiteral("token")]).toUtf8();
            settings->setValue(USER_TOKEN, d_ptr->userToken);

            jsonObject = jsonObject[QStringLiteral("user")].toObject();
            settings->setValue(USER_UID, OrnUtils::toUint(jsonObject[QStringLiteral("uid")]));
            settings->setValue(USER_NAME, OrnUtils::toString(jsonObject[QStringLiteral("name")]));
            settings->setValue(USER_MAIL, OrnUtils::toString(jsonObject[QStringLiteral("mail")]));
            settings->setValue(USER_CREATED, OrnUtils::toDateTime(jsonObject[QStringLiteral("created")]));
            settings->setValue(USER_PICTURE, jsonObject[QStringLiteral("picture")]
                    .toObject()[QStringLiteral("url")].toString());

            QString undKey(QStringLiteral("und"));
            QString valueKey(QStringLiteral("value"));
            auto name = jsonObject[QStringLiteral("field_name")].toObject()[undKey]
                    .toArray().first().toObject()[valueKey].toString();
            auto surname = jsonObject[QStringLiteral("field_surname")].toObject()[undKey]
                    .toArray().first().toObject()[valueKey].toString();
            auto hasName = !name.isEmpty();
            auto hasSurname = !surname.isEmpty();
            auto fullname = hasName && hasSurname ? name.append(" ").append(surname) :
                                                    hasName ? name : hasSurname ? surname : QString();
            settings->setValue(USER_REALNAME, fullname);

            qDebug() << "Successful authorisation";
            emit this->authorisedChanged();
            this->setCookieTimer();
        }
    });
}

void OrnClient::logout()
{
    if (this->authorised())
    {
        d_ptr->settings->remove(QStringLiteral("user"));
        d_ptr->userCookie.clear();
        d_ptr->userToken.clear();
        this->setCookieTimer();
        emit this->authorisedChanged();
    }
}

void OrnClient::comment(quint32 appId, const QString &body, quint32 parentId)
{
    auto request = this->apiRequest(QStringLiteral("comments"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, APPLICATION_JSON);

    QJsonObject commentObject;
    OrnClientPrivate::prepareComment(commentObject, body);
    commentObject.insert(QStringLiteral("appid"), QString::number(appId));
    if (parentId != 0)
    {
        commentObject.insert(QStringLiteral("pid"), QString::number(parentId));
    }

    auto reply = d_ptr->nam->post(request, QJsonDocument(commentObject).toJson());
    connect(reply, &QNetworkReply::finished, [this, reply, appId]()
    {
        auto jsonDoc = this->processReply(reply, CommentSendError);
        if (jsonDoc.isObject())
        {
            auto cid = OrnUtils::toUint(jsonDoc.object()[QStringLiteral("cid")]);
            emit this->commentActionFinished(CommentAdded, appId, cid);
            qDebug() << "Comment" << cid << "added for app" << appId;
        }
    });
}

void OrnClient::editComment(quint32 appId, quint32 commentId, const QString &body)
{
    auto request = this->apiRequest(QStringLiteral("comments/%0").arg(commentId));
    request.setHeader(QNetworkRequest::ContentTypeHeader, APPLICATION_JSON);

    QJsonObject commentObject;
    OrnClientPrivate::prepareComment(commentObject, body);

    auto reply = d_ptr->nam->put(request, QJsonDocument(commentObject).toJson());
    connect(reply, &QNetworkReply::finished, [this, reply, appId]()
    {
        auto jsonDoc = this->processReply(reply, CommentSendError);
        if (jsonDoc.isArray())
        {
            auto cid = OrnUtils::toUint(jsonDoc.array().first());
            emit this->commentActionFinished(CommentEdited, appId, cid);
            qDebug() << "Comment edited:" << cid;
        }
    });
}

void OrnClient::deleteComment(quint32 appId, quint32 commentId)
{
    auto request = this->apiRequest(QStringLiteral("comments/%0").arg(commentId));
    request.setHeader(QNetworkRequest::ContentTypeHeader, APPLICATION_JSON);

    auto reply = d_ptr->nam->deleteResource(request);
    connect(reply, &QNetworkReply::finished, [this, reply, appId, commentId]()
    {
        auto jsonDoc = this->processReply(reply, CommentDeleteError);
        if (jsonDoc.isArray())
        {
            auto value = jsonDoc.array().first();
            if (value.isBool() && value.toBool())
            {
                emit this->commentActionFinished(CommentDeleted, appId, commentId);
                qDebug() << "Comment" << commentId << "was deleted";
            }
        }
    });
}

void OrnClient::vote(quint32 appId, quint32 value)
{
    if (value < 1 || value > 100)
    {
        qCritical() << "Vote must be in range [0, 100], provided value is" << value;
        return;
    }

    auto request = this->apiRequest(QStringLiteral("votes"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, APPLICATION_JSON);

    QJsonObject voteObject = {
        {QStringLiteral("appid"), QString::number(appId)},
        {QStringLiteral("value"), QString::number(value)}
    };

    qDebug() << "Posting user vote" << value << "for app" << appId;
    auto reply = d_ptr->nam->post(request, QJsonDocument(voteObject).toJson());
    connect(reply, &QNetworkReply::finished, [this, reply, appId, value]()
    {
        if (reply->error() == QNetworkReply::NoError)
        {
            auto replyObject = QJsonDocument::fromJson(reply->readAll()).object();
            QString ratingKey(QStringLiteral("rating"));
            auto ratingObject = replyObject[ratingKey].toObject();
            qDebug() << "Received vote reply for app" << appId << ratingObject;
            emit this->userVoteFinished(appId, value,
                                        OrnUtils::toUint(ratingObject[QStringLiteral("count")]),
                                        ratingObject[ratingKey].toString().toFloat());
        }
        else
        {
            qWarning() << "Posting user vote" << value << "for app" << appId << "failed!";
        }
        reply->deleteLater();
    });
}

void OrnClient::setCookieTimer()
{
    auto cookieTimer = d_ptr->cookieTimer;
    disconnect(cookieTimer, &QTimer::timeout, 0, 0);
    cookieTimer->stop();
    QString cookieExpireKey(USER_COOKIE_EXPIRE);
    if (d_ptr->settings->contains(cookieExpireKey))
    {
        auto msec_to_expiry = QDateTime::currentDateTime().msecsTo(
                    d_ptr->settings->value(cookieExpireKey).toDateTime());
        if (msec_to_expiry > 86400000)
        {
            connect(cookieTimer, &QTimer::timeout, this, &OrnClient::dayToExpiry);
            cookieTimer->start(msec_to_expiry - 86400000);
        }
        else if (msec_to_expiry > 0)
        {
            emit this->dayToExpiry();
            connect(cookieTimer, &QTimer::timeout, this, &OrnClient::cookieIsValidChanged);
            cookieTimer->start(msec_to_expiry);
        }
        else
        {
            emit this->cookieIsValidChanged();
        }
    }
}
