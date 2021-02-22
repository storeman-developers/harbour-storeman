#include "ornclient.h"
#include "ornclient_p.h"
#include "ornutils.h"
#include "ornconst.h"

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

static const QString API_URL_PREFIX      {QStringLiteral("https://openrepos.net/api/v1/")};

static const QString SECRET_COOKIE       {QStringLiteral("cookie")};
static const QString SECRET_TOKEN        {QStringLiteral("token")};
static const QString SECRET_PASSWORD     {QStringLiteral("password")};

static const QByteArray APPLICATION_JSON {QByteArrayLiteral("application/json")};

static const QString CATEGORIES_FILE     {QStringLiteral("hidden_categories")};


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
    writeIds(OrnConst::bookmarks, bookmarks);

    // Write ids of hidden categories
    writeIds(CATEGORIES_FILE, hiddenCategories);
}

void OrnClientPrivate::removeUser()
{
    settings->remove(OrnConst::user);
    secrets.removeCollection();
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
    settings->beginGroup(OrnConst::user);
    auto username = settings->value(OrnConst::name).toString();
    settings->endGroup();
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

QVariant OrnClientPrivate::userProperty(const QString &key) const
{
    settings->beginGroup(OrnConst::user);
    auto value = settings->value(key);
    settings->endGroup();
    return value;
}

void OrnClientPrivate::prepareComment(QJsonObject &object, const QString &body)
{
    object.insert(QStringLiteral("comment_body"), QJsonObject{
        {OrnConst::und, QJsonArray{
                QJsonObject{
                    {OrnConst::value,                body},
                    {QStringLiteral("format"), QStringLiteral("filtered_html")},
                }
            }
        },
    });
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
            d->userCookie = QNetworkCookie::parseCookies(cookie).constFirst();
        }
        d->userToken = d->secrets.data(SECRET_TOKEN);
    }

    // Check if authorisation has expired
    if (this->authorised())
    {
        qDebug() << "Checking authorisation status";
        auto request = this->apiRequest("session");
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
    readIds(OrnConst::bookmarks, d->bookmarks);

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
    QUrl url(API_URL_PREFIX + resource);
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
                d->settings->beginGroup(OrnConst::user);
                // Otherwise remove user data
                auto username = d->settings->value(OrnConst::name);
                d->removeUser();
                // Save the last username to simplify manual re-login
                d->settings->setValue(OrnConst::name, username);
                d->settings->endGroup();
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
    return this->d_func()->userProperty(OrnConst::publisher).toBool();
}

quint32 OrnClient::userId() const
{
    return this->d_func()->userProperty(OrnConst::uid).toUInt();
}

QString OrnClient::userName() const
{
    return this->d_func()->userProperty(OrnConst::name).toString();
}

QString OrnClient::userIconSource()
{
    Q_D(OrnClient);
    // TODO: Remove later
    QString oldkey{QStringLiteral("user/picture/url")};
    if (d->settings->contains(oldkey))
    {
        auto value = d->settings->value(oldkey);
        d->settings->remove(oldkey);
        d->settings->beginGroup(OrnConst::user);
        d->settings->setValue(OrnConst::picture, value);
        d->settings->sync();
        d->settings->endGroup();
        return value.toString();
    }
    return d->userProperty(OrnConst::picture).toString();
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

bool checkPublisher(const QJsonObject &data)
{
    // Role `4` stands for `publisher`
    return data[OrnConst::roles].toObject().contains(QChar('4'));
}

QString namePart(const QJsonValue &data)
{
    return data.toObject()[OrnConst::und].toArray().first().toObject()[OrnConst::value].toString();;
}

QString realname(const QJsonObject &data)
{
    auto name    = namePart(data[QStringLiteral("field_name")]);
    auto surname = namePart(data[QStringLiteral("field_surname")]);
    if (surname.isEmpty())
    {
        return name;
    }
    if (name.isEmpty())
    {
        return surname;
    }
    return name.append(" ").append(surname);
}

void OrnClient::login(const QString &username, QString password, bool savePassword)
{
    Q_D(OrnClient);

    // Remove old credentials and stop timer
    d->removeUser();
    d->setCookieTimer();

    QNetworkRequest request;
    request.setUrl(QStringLiteral("user/login").prepend(API_URL_PREFIX));
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

        auto data = jsonDoc.object();

        d->userCookie = cookieVariant.value<QList<QNetworkCookie>>().first();
        d->userToken = OrnUtils::toString(data[QStringLiteral("token")]).toUtf8();

        if (savePassword)
        {
            d->secrets.storeData(SECRET_PASSWORD, password.toUtf8());
        }
        d->secrets.storeData(SECRET_COOKIE, d->userCookie.toRawForm());
        d->secrets.storeData(SECRET_TOKEN, d->userToken);

        auto settings = d->settings;

        settings->beginGroup(OrnConst::user);
        data = data[OrnConst::user].toObject();
        settings->setValue(OrnConst::uid,       OrnUtils::toUint(data[OrnConst::uid]));
        settings->setValue(OrnConst::name,      OrnUtils::toString(data[OrnConst::name]));
        settings->setValue(OrnConst::mail,      OrnUtils::toString(data[OrnConst::mail]));
        settings->setValue(OrnConst::created,   OrnUtils::toDateTime(data[OrnConst::created]));
        settings->setValue(OrnConst::picture,   data[OrnConst::picture].toObject()[OrnConst::url].toString());
        settings->setValue(OrnConst::publisher, checkPublisher(data));
        settings->setValue(OrnConst::realname,  realname(data));
        settings->endGroup();

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
    auto request = this->apiRequest(OrnConst::comments);
    request.setHeader(QNetworkRequest::ContentTypeHeader, APPLICATION_JSON);

    QJsonObject commentObject;
    OrnClientPrivate::prepareComment(commentObject, body);
    commentObject.insert(OrnConst::appid, QString::number(appId));
    if (parentId != 0)
    {
        commentObject.insert(OrnConst::pid, QString::number(parentId));
    }

    auto reply = this->d_func()->nam->post(request, QJsonDocument(commentObject).toJson());
    connect(reply, &QNetworkReply::finished, this, [this, reply, appId]()
    {
        auto jsonDoc = this->processReply(reply, CommentSendError);
        if (jsonDoc.isObject())
        {
            auto cid = OrnUtils::toUint(jsonDoc.object()[OrnConst::cid]);
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
        {OrnConst::appid, QString::number(appId)},
        {OrnConst::value, QString::number(value)}
    };

    qDebug() << "Posting user vote" << value << "for app" << appId;
    auto reply = this->d_func()->nam->post(request, QJsonDocument(voteObject).toJson());
    connect(reply, &QNetworkReply::finished, this, [this, reply, appId, value]()
    {
        if (reply->error() == QNetworkReply::NoError)
        {
            auto replyObject = QJsonDocument::fromJson(reply->readAll()).object();
            QString ratingKey(OrnConst::rating);
            auto ratingObject = replyObject[ratingKey].toObject();
            qDebug() << "Received vote reply for app" << appId << ratingObject;
            emit this->userVoteFinished(appId, value,
                                        OrnUtils::toUint(ratingObject[OrnConst::count]),
                                        ratingObject[ratingKey].toString().toFloat());
        }
        else
        {
            qWarning() << "Posting user vote" << value << "for app" << appId << "failed!";
        }
        reply->deleteLater();
    });
}
