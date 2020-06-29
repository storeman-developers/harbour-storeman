#pragma once

#include "ornsecrets.h"

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
    ~OrnClientPrivate() override;

    void removeUser();
    bool relogin();
    void setCookieTimer();

    QSettings *settings{nullptr};
    QTimer *cookieTimer{nullptr};
    QNetworkAccessManager *nam{nullptr};
    QSet<quint32> bookmarks;
    QSet<quint32> hiddenCategories;
    QByteArray lang;
    QByteArray userToken;
    QNetworkCookie userCookie;
    OrnSecrets secrets;

    static void prepareComment(QJsonObject &object, const QString &body);
};
