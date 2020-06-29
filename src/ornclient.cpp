#include "ornclient.h"
#include "ornclient_p.h"
#include "ornutils.h"

#include <functional>
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

#define USER_UID            QStringLiteral("user/uid")
#define USER_NAME           QStringLiteral("user/name")
#define USER_REALNAME       QStringLiteral("user/realname")
#define USER_MAIL           QStringLiteral("user/mail")
#define USER_CREATED        QStringLiteral("user/created")
#define USER_PICTURE        QStringLiteral("user/picture/url")
#define USER_PUBLISHER      QStringLiteral("user/publisher")

#define SECRET_COOKIE       QStringLiteral("cookie")
#define SECRET_TOKEN        QStringLiteral("token")
#define SECRET_PASSWORD     QStringLiteral("password")

#define APPLICATION_JSON    QByteArrayLiteral("application/json")

#define BOOKMARKS_FILE      QStringLiteral("bookmarks")
#define CATEGORIES_FILE     QStringLiteral("hidden_categories")


void readIds(const QString &filename, QSet<quint32> &store)
{
    auto path = QStandardPaths::locate(QStandardPaths::AppLocalDataLocation, filename);
    if (path.isEmpty())
    {
        return;
    }
    QFile file(path);
    if (file.open(QFile::ReadOnly))
    {
        qDebug() << "Reading" << filename << "file" << path;
        QDataStream stream(&file);
        stream >> store;
    }
    else
    {
        qWarning() << "Could not read" << filename << "file" << path;
    }
}

void writeIds(const QString &filename, QSet<quint32> &store)
{
    auto path = OrnUtils::locate(filename);
    QFile file(path);
    if (file.open(QFile::WriteOnly))
    {
        qDebug() << "Writing" << path;
        QDataStream stream(&file);
        stream << store;
    }
    else
    {
        qWarning() << "Could not write" << path;
    }
}

OrnClientPrivate::~OrnClientPrivate()
{
    // Write ids of bookmarked apps
    writeIds(BOOKMARKS_FILE, bookmarks);

    // Write ids of hidden categories
    writeIds(CATEGORIES_FILE, hiddenCategories);
}

void OrnClientPrivate::removeUser()
{
    settings->remove(QStringLiteral("user"));
    if (secrets.isValid())
    {
        secrets.reset();
    }
    userCookie = QNetworkCookie();
    userToken.clear();
}

bool OrnClientPrivate::relogin()
{
    if (!secrets.isValid())
    {
        return false;
    }
    auto password = secrets.data(SECRET_PASSWORD);
    if (password.isEmpty())
    {
        return false;
    }
    auto username = settings->value(USER_NAME).toString();
    q_func()->login(username, QString::fromUtf8(password), true);
    return true;
}

void OrnClientPrivate::setCookieTimer()
{
    Q_Q(OrnClient);

    qDebug() << "Setting cookie timer";

    if (cookieTimer->isActive())
    {
        QObject::disconnect(cookieTimer, &QTimer::timeout, 0, 0);
        cookieTimer->stop();
    }

    auto expirationDate = userCookie.expirationDate();
    if (!expirationDate.isValid())
    {
        return;
    }

    constexpr qint64 msec_day = 24 * 60 * 60 * 1000; // one day
    auto msec_to_expiry = QDateTime::currentDateTime().msecsTo(expirationDate);

    // Try to re-login if password is stored
    if (msec_to_expiry <= msec_day && relogin())
    {
        return;
    }

    // Otherwise send authorization notifications to user
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

    if (d->secrets.isValid())
    {
        auto cookie = d->secrets.data(SECRET_COOKIE);
        if (!cookie.isEmpty())
        {
            d->userCookie = QNetworkCookie::parseCookies(cookie).first();
        }
        d->userToken = d->secrets.data(SECRET_TOKEN);
    }

    // Check if authorisation has expired
    if (this->authorised())
    {
        qDebug() << "Checking authorisation status";
        auto request = this->apiRequest(QStringLiteral("session"));
        auto reply = d->nam->get(request);
        connect(reply, &QNetworkReply::finished, this, [this, reply]()
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

    // Read ids of bookmarked apps
    readIds(BOOKMARKS_FILE, d->bookmarks);

    // Read ids of hidden categories
    readIds(CATEGORIES_FILE, d->hiddenCategories);

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
        QJsonParseError error{0, QJsonParseError::NoError};
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
            // Try to re-login if password is stored
            if (!d->relogin())
            {
                // Otherwise remove user data
                QString usernameKey(USER_NAME);
                auto username = d->settings->value(usernameKey);
                d->removeUser();
                // Save the last username to simplify manual re-login
                d->settings->setValue(usernameKey, username);
                d->setCookieTimer();
                emit this->cookieIsValidChanged();
            }
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

bool OrnClient::categoryVisible(quint32 categoryId) const
{
    return !this->d_func()->hiddenCategories.contains(categoryId);
}

void OrnClient::setCategoryVisibility(quint32 categoryId, bool visible)
{
    Q_D(OrnClient);

    auto hidden = d->hiddenCategories.contains(categoryId);

    if (hidden != visible)
    {
        return;
    }

    if (hidden)
    {
        qDebug() << "Unhiding app category id" << categoryId;
        d->hiddenCategories.remove(categoryId);
    }
    else
    {
        qDebug() << "Hiding app category id" << categoryId;
        d->hiddenCategories.insert(categoryId);
    }

    emit this->categoryVisibilityChanged(categoryId, visible);

}

void OrnClient::toggleCategoryVisibility(quint32 categoryId)
{
    Q_D(OrnClient);

    auto hidden = d->hiddenCategories.contains(categoryId);

    if (hidden)
    {
        qDebug() << "Unhiding app category id" << categoryId;
        d->hiddenCategories.remove(categoryId);
    }
    else
    {
        qDebug() << "Hiding app category id" << categoryId;
        d->hiddenCategories.insert(categoryId);
    }

    emit this->categoryVisibilityChanged(categoryId, !hidden);
}

void OrnClient::login(const QString &username, QString password, bool savePassword)
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
    connect(reply, &QNetworkReply::finished, this, [this, reply, savePassword, password]()
    {
        auto cookieVariant = reply->header(QNetworkRequest::SetCookieHeader);
        auto jsonDoc = this->processReply(reply, AuthorisationError);
        if (!cookieVariant.isValid() || !jsonDoc.isObject())
        {
            return;
        }

        Q_D(OrnClient);

        auto jsonObject = jsonDoc.object();

        d->userCookie = cookieVariant.value<QList<QNetworkCookie>>().first();
        d->userToken = OrnUtils::toString(jsonObject[QStringLiteral("token")]).toUtf8();

        if (d->secrets.isValid())
        {
            if (savePassword)
            {
                d->secrets.storeData(SECRET_PASSWORD, password.toUtf8());
            }
            d->secrets.storeData(SECRET_COOKIE, d->userCookie.toRawForm());
            d->secrets.storeData(SECRET_TOKEN, d->userToken);
        }

        auto settings = d->settings;

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
        auto namePart = [&jsonObject, &undKey, &valueKey](const QString &partKey)
        { return jsonObject[partKey].toObject()[undKey].toArray().first().toObject()[valueKey].toString(); };
        auto name = namePart(QStringLiteral("field_name"));
        auto surname = namePart(QStringLiteral("field_surname"));
        if (!surname.isEmpty())
        {
            if (!name.isEmpty())
            {
                name.append(" ").append(surname);
            }
            else
            {
                name = surname;
            }
        }
        settings->setValue(USER_REALNAME, name);

        qDebug() << "Successful authorisation";
        emit this->authorisedChanged();
        d->setCookieTimer();
    });
}

void OrnClient::logout()
{
    if (!this->authorised())
    {
        return;
    }

    Q_D(OrnClient);

    d->removeUser();
    d->setCookieTimer();
    emit this->authorisedChanged();
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
    connect(reply, &QNetworkReply::finished, this, [this, reply, appId]()
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
    connect(reply, &QNetworkReply::finished, this, [this, reply, appId]()
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
    connect(reply, &QNetworkReply::finished, this, [this, reply, appId, commentId]()
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
    connect(reply, &QNetworkReply::finished, this, [this, reply, appId, value]()
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
