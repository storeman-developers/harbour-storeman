#ifndef ORNCLIENT_H
#define ORNCLIENT_H

#include <QObject>
#include <QUrlQuery>

class QQmlEngine;
class QJSEngine;
class QNetworkAccessManager;
class QNetworkRequest;
class QNetworkReply;

struct OrnClientPrivate;

class OrnClient : public QObject
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
        NetworkError,
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

    QNetworkRequest apiRequest(const QString &resource, const QUrlQuery &query = QUrlQuery()) const;
    QNetworkAccessManager *networkAccessManager() const;
    QJsonDocument processReply(QNetworkReply *reply, Error code = NetworkError);

    bool authorised() const;
    bool cookieIsValid() const;
    quint32 userId() const;
    QString userName() const;
    QString userIconSource() const;

    Q_INVOKABLE QList<quint32> bookmarks() const;
    Q_INVOKABLE bool hasBookmark(quint32 appId) const;
    Q_INVOKABLE bool addBookmark(quint32 appId);
    Q_INVOKABLE bool removeBookmark(quint32 appId);

public slots:
    void login(const QString &username, const QString &password);
    void logout();

    void comment(quint32 appId, const QString &body, quint32 parentId = 0);
    void editComment(quint32 appId, quint32 commentId, const QString &body);
    void deleteComment(quint32 appId, quint32 commentId);

    void vote(quint32 appId, quint32 value);

signals:
    void error(Error code);
    void authorisedChanged();
    void dayToExpiry();
    void cookieIsValidChanged();
    void commentActionFinished(CommentAction action, quint32 appId, quint32 cid);
    void bookmarkChanged(quint32 appid, bool bookmarked);
    void userVoteFinished(quint32 appId, quint32 userVote, quint32 count, float rating);

private slots:
    void setCookieTimer();

private:
    explicit OrnClient(QObject *parent = nullptr);
    ~OrnClient();

    OrnClientPrivate *d_ptr;
};

#endif // ORNCLIENT_H
