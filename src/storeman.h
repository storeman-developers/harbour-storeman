#ifndef STOREMAN_H
#define STOREMAN_H

#include <QObject>
#include <QCache>
#include <QVariant>

class QQmlEngine;
class QJSEngine;
class QSettings;
class QTimer;

class OrnApplication;

class Storeman : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool showRecentOnStart READ showRecentOnStart WRITE setShowRecentOnStart NOTIFY showRecentOnStartChanged)
    Q_PROPERTY(QVariantList mainPageOrder READ mainPageOrder WRITE setMainPageOrder RESET resetMainPageOrder NOTIFY mainPageOrderChanged)
    Q_PROPERTY(int updateInterval READ updateInterval WRITE setUpdateInterval NOTIFY updateIntervalChanged)
    Q_PROPERTY(bool checkForUpdates READ checkForUpdates WRITE setCheckForUpdates NOTIFY checkForUpdatesChanged)
    Q_PROPERTY(bool smartUpdate READ smartUpdate WRITE setSmartUpdate NOTIFY smartUpdateChanged)
    Q_PROPERTY(bool showUpdatesNotification READ showUpdatesNotification WRITE setShowUpdatesNotification NOTIFY showUpdatesNotificationChanged)

public:
    enum Hint
    {
        CommentDelegateHint,
        CommentFieldHint,
        ApplicationRateAndBookmarkHint
    };
    Q_ENUM(Hint)

    enum MainPageItem
    {
        RecentlyUpdated,
        Categories,
        Bookmarks,
        Repositories,
        InstalledApps,
        LocalRpms,
    };
    Q_ENUM(MainPageItem)

    static QObject *qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine);

    bool showRecentOnStart() const;
    void setShowRecentOnStart(bool value);

    QVariantList mainPageOrder() const;
    void setMainPageOrder(const QVariantList &value);
    void resetMainPageOrder();

    int updateInterval() const;
    void setUpdateInterval(int value);

    bool checkForUpdates() const;
    void setCheckForUpdates(bool value);

    bool smartUpdate() const;
    void setSmartUpdate(bool value);

    bool showUpdatesNotification() const;
    void setShowUpdatesNotification(bool value);

    Q_INVOKABLE static bool fileExists(const QString &filePath);
    Q_INVOKABLE static bool removeFile(const QString &filePath);

    Q_INVOKABLE bool showHint(Hint hint) const;
    Q_INVOKABLE void setHintShowed(Hint hint);

    Q_INVOKABLE OrnApplication *cachedApp(quint32 appId);

    Q_INVOKABLE void checkRepos();

public slots:
    void resetUpdatesTimer();

signals:
    void showRecentOnStartChanged();
    void mainPageOrderChanged();
    void updateIntervalChanged();
    void checkForUpdatesChanged();
    void smartUpdateChanged();
    void showUpdatesNotificationChanged();
    void updatesNotification(bool show, quint32 replaceId);
    void repoSuggestion(const QString &author, bool enableOnly);

private slots:
    void refreshRepos();
    void onUpdatablePackagesChanged();
    void startUpdatesTimer();

private:
    explicit Storeman(QObject *parent = nullptr);
    void checkReposImpl();

    QSettings *mSettings;
    QTimer *mUpdatesTimer;
    QCache<quint32, OrnApplication> mAppsCache;
};

#endif // STOREMAN_H
