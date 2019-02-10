#ifndef ORNCLIENT_P_H
#define ORNCLIENT_P_H

#include <QSet>
#include <QVariant>
#include <QNetworkCookie>

class QSettings;
class QTimer;
class QNetworkAccessManager;
class OrnClient;

struct OrnClientPrivate
{
    explicit OrnClientPrivate(OrnClient *q_ptr);
    virtual ~OrnClientPrivate();

    void removeUser();

    QSettings *settings;
    QTimer *cookieTimer;
    QNetworkAccessManager *nam;
    QSet<quint32> bookmarks;
    QByteArray lang;
    QByteArray userToken;
    QNetworkCookie userCookie;

    static void prepareComment(QJsonObject &object, const QString &body);
};

#endif // ORNCLIENT_P_H
