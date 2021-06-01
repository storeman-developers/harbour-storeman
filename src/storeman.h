#pragma once

#include <QObject>
#include <QVariant>

class QQmlEngine;
class QJSEngine;

class OrnApplication;
class StoremanPrivate;

class Storeman : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(Storeman)

    Q_PROPERTY(bool showRecentOnStart READ showRecentOnStart WRITE setShowRecentOnStart NOTIFY showRecentOnStartChanged)
    Q_PROPERTY(QVariantList mainPageOrder READ mainPageOrder WRITE setMainPageOrder RESET resetMainPageOrder NOTIFY mainPageOrderChanged)
    Q_PROPERTY(int updateInterval READ updateInterval WRITE setUpdateInterval NOTIFY updateIntervalChanged)
    Q_PROPERTY(bool checkForUpdates READ checkForUpdates WRITE setCheckForUpdates NOTIFY checkForUpdatesChanged)
    Q_PROPERTY(bool smartUpdate READ smartUpdate WRITE setSmartUpdate NOTIFY smartUpdateChanged)
    Q_PROPERTY(bool showUpdatesNotification READ showUpdatesNotification WRITE setShowUpdatesNotification NOTIFY showUpdatesNotificationChanged)
    Q_PROPERTY(bool refreshOnSystemUpgrade READ refreshOnSystemUpgrade WRITE setRefreshOnSystemUpgrade NOTIFY refreshOnSystemUpgradeChanged)
    Q_PROPERTY(bool searchUnusedRepos READ searchUnusedRepos WRITE setSearchUnusedRepos NOTIFY searchUnusedReposChanged)

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
        MyRepository,
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

    bool refreshOnSystemUpgrade() const;
    void setRefreshOnSystemUpgrade(bool value);

    bool searchUnusedRepos() const;
    void setSearchUnusedRepos(bool value);

    Q_INVOKABLE static bool fileExists(const QString &filePath);
    Q_INVOKABLE static bool removeFile(const QString &filePath);

    Q_INVOKABLE bool showHint(Storeman::Hint hint);
    Q_INVOKABLE void setHintShowed(Storeman::Hint hint);

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
    void refreshOnSystemUpgradeChanged();
    void searchUnusedReposChanged();
    void updatesNotification(bool show, quint32 replaceId);
    void repoSuggestion(const QString &author, bool enableOnly);
    void recentAppsChanged();

private slots:
    void refreshRepos();
    void onUpdatablePackagesChanged();
    void startUpdatesTimer();
    void checkSystemVersion();

private:
    explicit Storeman(QObject *parent = nullptr);
};
