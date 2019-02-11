#ifndef ORNCLIENT_P_H
#define ORNCLIENT_P_H

#include <private/qobject_p.h>
#include <QSet>
#include <QVariant>
#include <QNetworkCookie>

class QSettings;
class QTimer;
class QNetworkAccessManager;
class OrnClient;

class OrnClientPrivate : public QObjectPrivate
{
    Q_DISABLE_COPY(OrnClientPrivate)
    Q_DECLARE_PUBLIC(OrnClient)

public:
    OrnClientPrivate() = default;
    virtual ~OrnClientPrivate();

    void removeUser();
    void setCookieTimer();

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
