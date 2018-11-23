#ifndef ORNCLIENT_H
#define ORNCLIENT_H

#include "ornapirequest.h"

#include <QSet>
#include <QVariant>

class QSettings;
class QTimer;
class QQmlEngine;
class QJSEngine;

class OrnClient : public OrnApiRequest
{
    friend class OrnBackup;
    friend class OrnApiRequest;

    Q_OBJECT
    Q_PROPERTY(bool authorised READ authorised NOTIFY authorisedChanged)
    Q_PROPERTY(bool cookieIsValid READ cookieIsValid NOTIFY cookieIsValidChanged)
    Q_PROPERTY(quint32 userId READ userId NOTIFY authorisedChanged)
    Q_PROPERTY(QString userName READ userName NOTIFY authorisedChanged)
    Q_PROPERTY(QString userIconSource READ userIconSource NOTIFY authorisedChanged)

public:

    enum Error
    {
        AuthorisationError,
        CommentSendError,
        CommentDeleteError,
    };
    Q_ENUM(Error)

    enum CommentAction
    {
        CommentAdded,
        CommentEdited,
        CommentDeleted,
    };
    Q_ENUM(CommentAction)

    static OrnClient *instance();
    static inline QObject *qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine)
    {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)

        return OrnClient::instance();
    }

    bool authorised() const;
    bool cookieIsValid() const;
    quint32 userId() const;
    QString userName() const;
    QString userIconSource() const;

    Q_INVOKABLE QList<quint32> bookmarks() const;
    Q_INVOKABLE bool hasBookmark(const quint32 &appId) const;
    Q_INVOKABLE bool addBookmark(const quint32 &appId);
    Q_INVOKABLE bool removeBookmark(const quint32 &appId);

public slots:
    void login(const QString &username, const QString &password);
    void logout();

    void comment(quint32 appId, const QString &body, quint32 parentId = 0);
    void editComment(quint32 appId, quint32 commentId, const QString &body);
    void deleteComment(const quint32 &appId, const quint32 &commentId);

    void vote(const quint32 &appId, const quint32 &value);

signals:
    void error(Error code);
    void authorisedChanged();
    void dayToExpiry();
    void cookieIsValidChanged();
    void commentActionFinished(CommentAction action, quint32 appId, quint32 cid);
    void bookmarkChanged(quint32 appid, bool bookmarked);
    void userVoteFinished(const quint32 &appId, const quint32 &userVote,
                          const quint32 &count, const float &rating);

private slots:
    void setCookieTimer();
    void onLoggedIn();

private:
    explicit OrnClient(QObject *parent = nullptr);
    ~OrnClient();
    QNetworkRequest authorisedRequest();
    QJsonDocument processReply();
    static void prepareComment(QJsonObject &object, const QString &body);

private:
    QSettings *mSettings;
    QTimer *mCookieTimer;
    QSet<quint32> mBookmarks;

    static OrnClient *gInstance;
};

#endif // ORNCLIENT_H
