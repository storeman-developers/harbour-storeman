#include "ornclient.h"
#include "ornclient_p.h"
#include "ornutils.h"

#include <QSettings>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
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
#define USER_TOKEN          QStringLiteral("user/token")
#define USER_UID            QStringLiteral("user/uid")
#define USER_NAME           QStringLiteral("user/name")
#define USER_REALNAME       QStringLiteral("user/realname")
#define USER_MAIL           QStringLiteral("user/mail")
#define USER_CREATED        QStringLiteral("user/created")
#define USER_PICTURE        QStringLiteral("user/picture/url")
#define USER_PUBLISHER      QStringLiteral("user/publisher")

#define APPLICATION_JSON    QByteArrayLiteral("application/json")


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

void OrnClientPrivate::removeUser()
{
    settings->remove(QStringLiteral("user"));
}

void OrnClientPrivate::setCookieTimer()
{
    Q_Q(OrnClient);

    qDebug() << "Setting cookie timer";

    QObject::disconnect(cookieTimer, &QTimer::timeout, 0, 0);
    cookieTimer->stop();
    auto expirationDate = userCookie.expirationDate();
    if (expirationDate.isValid())
    {
        constexpr qint64 msec_day = 24 * 60 * 60 * 1000; // one day
        auto msec_to_expiry = QDateTime::currentDateTime().msecsTo(expirationDate);
        if (msec_to_expiry > msec_day)
        {
            QObject::connect(cookieTimer, &QTimer::timeout, q, &OrnClient::dayToExpiry);
            cookieTimer->start(msec_to_expiry - msec_day);
        }
        else if (msec_to_expiry > 0)
        {
            emit q->dayToExpiry();
            QObject::connect(cookieTimer, &QTimer::timeout, q, &OrnClient::cookieIsValidChanged);
            cookieTimer->start(msec_to_expiry);
        }
        else
        {
            emit q->cookieIsValidChanged();
        }
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
    : QObject(*new OrnClientPrivate(), parent)
{
    Q_D(OrnClient);

    d->settings = new QSettings(this);
    d->cookieTimer = new QTimer(this);
    d->nam = new QNetworkAccessManager(this);
    d->lang = QLocale::system().name().left(2).toUtf8();

    if (d->settings->contains(USER_TOKEN) && d->settings->contains(USER_COOKIE))
    {
        auto cookie = d->settings->value(USER_COOKIE).toByteArray();
        d->userCookie = QNetworkCookie::parseCookies(cookie).first();
        d->userToken = d->settings->value(USER_TOKEN).toByteArray();
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
            stream >> d->bookmarks;
        }
        else
        {
            qWarning() << "Could not read bookmarks file" << path;
        }
    }

    // Check if authorisation has expired
    if (this->authorised())
    {
        qDebug() << "Checking authorisation status";
        auto request = this->apiRequest(QStringLiteral("session"));
        auto reply = d->nam->get(request);
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
            // NOTE: Remove this code in future
            Q_D(OrnClient);
            QString publisherKey(USER_PUBLISHER);
            if (this->authorised() && !d->settings->contains(publisherKey))
            {
                auto uid = QString::number(this->userId());
                auto request2 = this->apiRequest(QStringLiteral("users/").append(uid));
                auto reply2 = d->nam->get(request2);
                connect(reply2, &QNetworkReply::finished, this, [this, reply2, publisherKey]
                {
                    auto jsonObject = this->processReply(reply2).object();
                    jsonObject = jsonObject[QStringLiteral("user")].toObject();
                    jsonObject = jsonObject[QStringLiteral("roles")].toObject();
                    this->d_func()->settings->setValue(publisherKey, jsonObject.contains(QChar('4')));
                });
            }
        });
    }
    // NOTE: Remove this code in future
    else if (d->settings->contains(USER_UID))
    {
        QString usernameKey(USER_NAME);
        auto username = d->settings->value(usernameKey);
        d->removeUser();
        // Save the last username to simplify re-login
        d->settings->setValue(usernameKey, username);
        d->userCookie = QNetworkCookie();
        d->userToken.clear();
    }

    // A workaround as qml does not call a destructor
    connect(qApp, &QGuiApplication::aboutToQuit, this, &OrnClient::deleteLater);

    // Configure cookie timer
    d->cookieTimer->setSingleShot(true);
    QTimer::singleShot(1000, std::bind(&OrnClientPrivate::setCookieTimer, d));
}

OrnClient *OrnClient::instance()
{
    static auto instance = new OrnClient(qApp);
    return instance;
}

QNetworkRequest OrnClient::apiRequest(const QString &resource, const QUrlQuery &query) const
{
    Q_D(const OrnClient);

    QNetworkRequest request;
    if (this->cookieIsValid())
    {
        request.setHeader(QNetworkRequest::CookieHeader, QVariant::fromValue(d->userCookie));
        request.setRawHeader(QByteArrayLiteral("X-CSRF-Token"), d->userToken);
    }
    request.setRawHeader(QByteArrayLiteral("Accept-Language"), d->lang);
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
    return this->d_func()->nam;
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
        Q_D(OrnClient);

        qWarning() << "Network request error" << reply->error() << "-" << reply->errorString();
        auto status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (status == 403 && this->cookieIsValid())
        {
            QString usernameKey(USER_NAME);
            auto username = d->settings->value(usernameKey);
            d->removeUser();
            // Save the last username to simplify re-login
            d->settings->setValue(usernameKey, username);
            d->userCookie = QNetworkCookie();
            d->userToken.clear();
            d->setCookieTimer();
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
    return !this->d_func()->userToken.isNull() && this->cookieIsValid();
}

bool OrnClient::cookieIsValid() const
{
    auto expirationDate = this->d_func()->userCookie.expirationDate();
    return expirationDate.isValid() && expirationDate > QDateTime::currentDateTime();
}

bool OrnClient::isPublisher() const
{
    return this->d_func()->settings->value(USER_PUBLISHER).toBool();
}

quint32 OrnClient::userId() const
{
    return this->d_func()->settings->value(USER_UID).toUInt();
}

QString OrnClient::userName() const
{
    return this->d_func()->settings->value(USER_NAME).toString();
}

QString OrnClient::userIconSource() const
{
    return this->d_func()->settings->value(USER_PICTURE).toString();
}

QList<quint32> OrnClient::bookmarks() const
{
    return this->d_func()->bookmarks.toList();
}

bool OrnClient::hasBookmark(quint32 appId) const
{
    return this->d_func()->bookmarks.contains(appId);
}

bool OrnClient::addBookmark(quint32 appId)
{
    Q_D(OrnClient);

    auto ok = !d->bookmarks.contains(appId);
    if (ok)
    {
        qDebug() << "Adding to bookmarks app id" << appId;
        d->bookmarks.insert(appId);
        emit this->bookmarkChanged(appId, true);
    }
    return ok;
}

bool OrnClient::removeBookmark(quint32 appId)
{
    auto ok = this->d_func()->bookmarks.remove(appId);
    if (ok)
    {
        qDebug() << "Removing from bookmarks app id" << appId;
        emit this->bookmarkChanged(appId, false);
    }
    return ok;
}

void OrnClient::login(const QString &username, const QString &password)
{
    Q_D(OrnClient);

    // Remove old credentials and stop timer
    d->removeUser();
    d->setCookieTimer();

    QNetworkRequest request;
    request.setUrl(API_URL_PREFIX.append(QStringLiteral("user/login")));
    request.setHeader(QNetworkRequest::ContentTypeHeader, APPLICATION_JSON);

    QJsonObject jsonObject;
    jsonObject.insert(QStringLiteral("username"), username);
    jsonObject.insert(QStringLiteral("password"), password);
    QJsonDocument jsonDoc(jsonObject);

    auto reply = d->nam->post(request, jsonDoc.toJson());
    connect(reply, &QNetworkReply::finished, [this, reply]()
    {
        auto cookieVariant = reply->header(QNetworkRequest::SetCookieHeader);
        auto jsonDoc = this->processReply(reply, AuthorisationError);
        if (cookieVariant.isValid() && jsonDoc.isObject())
        {
            Q_D(OrnClient);

            auto jsonObject = jsonDoc.object();

            d->removeUser();
            d->userCookie = cookieVariant.value<QList<QNetworkCookie>>().first();
            auto settings = d->settings;
            settings->setValue(USER_COOKIE, d->userCookie.toRawForm());

            d->userToken = OrnUtils::toString(jsonObject[QStringLiteral("token")]).toUtf8();
            settings->setValue(USER_TOKEN, d->userToken);

            jsonObject = jsonObject[QStringLiteral("user")].toObject();
            settings->setValue(USER_UID, OrnUtils::toUint(jsonObject[QStringLiteral("uid")]));
            settings->setValue(USER_NAME, OrnUtils::toString(jsonObject[QStringLiteral("name")]));
            settings->setValue(USER_MAIL, OrnUtils::toString(jsonObject[QStringLiteral("mail")]));
            settings->setValue(USER_CREATED, OrnUtils::toDateTime(jsonObject[QStringLiteral("created")]));
            settings->setValue(USER_PICTURE, jsonObject[QStringLiteral("picture")]
                    .toObject()[QStringLiteral("url")].toString());
            auto roles = jsonObject[QStringLiteral("roles")].toObject();
            // Role `4` stands for `publisher`
            settings->setValue(USER_PUBLISHER, roles.contains(QChar('4')));

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
            d->setCookieTimer();
        }
    });
}

void OrnClient::logout()
{
    if (this->authorised())
    {
        Q_D(OrnClient);

        d->settings->remove(QStringLiteral("user"));
        d->userCookie = QNetworkCookie();
        d->userToken.clear();
        d->setCookieTimer();
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

    auto reply = this->d_func()->nam->post(request, QJsonDocument(commentObject).toJson());
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

    auto reply = this->d_func()->nam->put(request, QJsonDocument(commentObject).toJson());
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

    auto reply = this->d_func()->nam->deleteResource(request);
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
    auto reply = this->d_func()->nam->post(request, QJsonDocument(voteObject).toJson());
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
