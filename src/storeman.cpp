/*!
    \fn void Storeman::updatesNotification(bool show, quint32 replaceId)

    Emitted when the updates notification state should be changed.

    \a show holds if the notification should be showed or closed.

    \a replaceId holds the notification ID to replace or
    0 if a new notification shuld be published.
 */

#include "storeman_p.h"
#include "ornutils.h"
#include "ornpm.h"
#include "ornapplication.h"
#include "ornrepo.h"
#include "ornclient.h"
#include "ornconst.h"

#include <notification.h>
#include <networkmanager.h>

#include <QDateTime>
#include <QFileInfo>
#include <QtConcurrentRun>
#include <QNetworkReply>
#include <QVersionNumber>

static const QString MAINPAGE_SHOW_RECENT     {QStringLiteral("mainpage/show_recent")};
static const QString MAINPAGE_ORDER           {QStringLiteral("mainpage/order")};
static const QString UPDATES_INTERVAL         {QStringLiteral("updates/interval")};
static const QString UPDATES_ENABLED          {QStringLiteral("updates/enabled")};
static const QString UPDATES_SMART            {QStringLiteral("updates/smart")};
static const QString UPDATES_SHOW_NOTIFICATION{QStringLiteral("updates/show_notification")};
static const QString UPDATES_LAST_CHECK       {QStringLiteral("updates/last_check")};
static const QString REFRESH_CACHE_ENABLE     {QStringLiteral("refresh_cache/enable")};
static const QString GROUP_HINTS              {QStringLiteral("hints")};


Storeman::Storeman(QObject *parent)
    : QObject(*new StoremanPrivate(), parent)
{
    Q_D(Storeman);

    d->appsCache.setMaxCost(3);

    auto ornpm = OrnPm::instance();
    connect(ornpm, &OrnPm::updatablePackagesChanged,
            this, &Storeman::onUpdatablePackagesChanged);

    d->updatesTimer.setInterval(this->updateInterval());
    d->updatesTimer.setSingleShot(false);
    connect(&d->updatesTimer, &QTimer::timeout, this, &Storeman::refreshRepos);

    connect(this, &Storeman::checkForUpdatesChanged,
            this, &Storeman::startUpdatesTimer);
    connect(ornpm, &OrnPm::initialisedChanged,
            this, &Storeman::startUpdatesTimer);
    connect(NetworkManager::instance(), &NetworkManager::stateChanged,
            this, &Storeman::startUpdatesTimer);
    this->startUpdatesTimer();

    if (this->refreshOnSystemUpgrade())
    {
        QTimer::singleShot(1000, this, &Storeman::checkSystemVersion);
    }
}

QObject *Storeman::qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    static Storeman instance;
    return &instance;
}

bool Storeman::showRecentOnStart() const
{
    return d_func()->settings.value(MAINPAGE_SHOW_RECENT).toBool();
}

void Storeman::setShowRecentOnStart(bool value)
{
    if (value != this->showRecentOnStart())
    {
        d_func()->settings.setValue(MAINPAGE_SHOW_RECENT, value);
        emit this->showRecentOnStartChanged();
    }
}

QVariantList Storeman::mainPageOrder() const
{
    auto res = d_func()->settings.value(MAINPAGE_ORDER, QVariantList({
        RecentlyUpdated,
        Categories,
        Bookmarks,
        Repositories,
        MyRepository,
        InstalledApps,
        LocalRpms,
    })).toList();

    // TODO: Remove later
    if (!res.contains(MyRepository))
    {
        res.insert(res.indexOf(Repositories) + 1, MyRepository);
    }

    return res;
}

void Storeman::setMainPageOrder(const QVariantList &value)
{
    if (this->mainPageOrder() != value)
    {
        d_func()->settings.setValue(MAINPAGE_ORDER, value);
        emit this->mainPageOrderChanged();
    }
}

void Storeman::resetMainPageOrder()
{
    d_func()->settings.remove(MAINPAGE_ORDER);
    emit this->mainPageOrderChanged();
}

int Storeman::updateInterval() const
{
    // Default update interval is 10 minutes
    return d_func()->settings.value(UPDATES_INTERVAL, 600000).toInt();
}

void Storeman::setUpdateInterval(int value)
{
    Q_ASSERT(value > 0);
    if (this->updateInterval() != value)
    {
        Q_D(Storeman);
        d->settings.setValue(UPDATES_INTERVAL, value);
        d->updatesTimer.setInterval(value);
        emit this->updateIntervalChanged();
    }
}

bool Storeman::checkForUpdates() const
{
    return d_func()->settings.value(UPDATES_ENABLED, true).toBool();
}

void Storeman::setCheckForUpdates(bool value)
{
    if (value != this->checkForUpdates())
    {
        d_func()->settings.setValue(UPDATES_ENABLED, value);
        emit this->checkForUpdatesChanged();
    }
}

bool Storeman::smartUpdate() const
{
    return d_func()->settings.value(UPDATES_SMART, true).toBool();
}

void Storeman::setSmartUpdate(bool value)
{
    if (value != this->smartUpdate())
    {
        d_func()->settings.setValue(UPDATES_SMART, value);
        emit this->smartUpdateChanged();
    }
}

bool Storeman::showUpdatesNotification() const
{
    return d_func()->settings.value(UPDATES_SHOW_NOTIFICATION, true).toBool();
}

void Storeman::setShowUpdatesNotification(bool value)
{
    if (value != this->showUpdatesNotification())
    {
        d_func()->settings.setValue(UPDATES_SHOW_NOTIFICATION, value);
        emit this->showUpdatesNotificationChanged();
        this->onUpdatablePackagesChanged();
    }
}

bool Storeman::refreshOnSystemUpgrade() const
{
    return d_func()->settings.value(REFRESH_CACHE_ENABLE, true).toBool();
}

void Storeman::setRefreshOnSystemUpgrade(bool value)
{
    if (this->refreshOnSystemUpgrade() == value)
    {
        return;
    }

    d_func()->settings.setValue(REFRESH_CACHE_ENABLE, value);
    emit this->refreshOnSystemUpgradeChanged();
}

bool Storeman::fileExists(const QString &filePath)
{
    Q_ASSERT_X(!filePath.isEmpty(), Q_FUNC_INFO, "An empty string is passed");
    return QFile::exists(filePath);
}

bool Storeman::removeFile(const QString &filePath)
{
    Q_ASSERT_X(!QFileInfo(filePath).isDir(), Q_FUNC_INFO, "Path must be a file");
    return QFile::remove(filePath);
}

QString hintKey(Storeman::Hint hint)
{
    auto me   = QMetaEnum::fromType<Storeman::Hint>();
    auto name = me.valueToKey(hint);
    Q_ASSERT(name);
    return QString::fromLatin1(name);
}

bool Storeman::showHint(Storeman::Hint hint)
{
    Q_D(Storeman);

    d->settings.beginGroup(GROUP_HINTS);
    auto value = d->settings.value(hintKey(hint), false).toBool();
    d->settings.endGroup();
    return !value;
}

void Storeman::setHintShowed(Storeman::Hint hint)
{
    Q_D(Storeman);

    d->settings.beginGroup(GROUP_HINTS);
    d->settings.setValue(hintKey(hint), true);
    d->settings.endGroup();
}

OrnApplication *Storeman::cachedApp(quint32 appId)
{
    if (appId == 0)
    {
        return nullptr;
    }

    Q_D(Storeman);

    if (d->appsCache.contains(appId))
    {
        return d->appsCache[appId];
    }

    auto app = new OrnApplication(this);
    app->setAppId(appId);
    d->appsCache.insert(appId, app);
    return app;
}

void Storeman::resetUpdatesTimer()
{
    Q_D(Storeman);

    OrnPm::instance()->refreshRepos();
    d->settings.setValue(UPDATES_LAST_CHECK, QDateTime::currentMSecsSinceEpoch());
    d->updatesTimer.start();
}

void Storeman::refreshRepos()
{
    if (this->smartUpdate())
    {
        auto client = OrnClient::instance();
        auto request = client->apiRequest(QStringLiteral("apps"), QUrlQuery(QStringLiteral("pagesize=1")));
        auto reply = client->networkAccessManager()->get(request);
        connect(reply, &QNetworkReply::finished, [this, reply]()
        {
            if (reply->error() == QNetworkReply::NoError)
            {
                auto json = QJsonDocument::fromJson(reply->readAll()).array();
                if (!json.isEmpty())
                {
                    Q_D(Storeman);
                    auto lastUpdate = OrnUtils::toUint(json[0].toObject()[OrnConst::updated]);
                    auto lastCheck = d->settings.value(UPDATES_LAST_CHECK).toLongLong();
                    if (qlonglong(lastUpdate) * 1000 > lastCheck)
                    {
                        d->refreshRepos();
                    }
                }
            }
            reply->deleteLater();
        });
    }
    else
    {
        d_func()->refreshRepos();
    }
}

inline Notification *previousNotification()
{
    Notification *prev = nullptr;
    QLatin1String category("x-storeman.updates");
    for (auto o : Notification::notifications())
    {
        auto n = dynamic_cast<Notification *>(o);
        if (n->category() == category)
        {
#ifdef QT_DEBUG
            if (!prev)
            {
                prev = n;
            }
            else
            {
                qCritical("There should always be no more than one notification!");
                n->deleteLater();
            }
#else
            prev = n;
#endif
        }
        else
        {
            n->deleteLater();
        }
    }
    return prev;
}

void Storeman::onUpdatablePackagesChanged()
{
    Q_D(Storeman);

    QString key{QStringLiteral("updates/last_packages")};
    auto ornpm = OrnPm::instance();    
    auto prev = previousNotification();

    if (this->showUpdatesNotification() && ornpm->updatesAvailable())
    {
        if (!prev)
        {
            // There is no existing notification: show a new one
            emit this->updatesNotification(true, 0);
            return;
        }

        auto cur_packages = OrnPm::instance()->updatablePackages();
        auto cur_packages_set = cur_packages.toSet();
        auto prev_packages_set = d->settings.value(key).toStringList().toSet();
        auto newpackages = cur_packages_set == prev_packages_set ?
                    false : !prev_packages_set.contains(cur_packages_set);

        // Do not show notification on application start
        if (newpackages)
        {
            emit this->updatesNotification(true, prev->replacesId());
            prev->deleteLater();
            d->settings.setValue(key, cur_packages);
        }
    }
    else
    {
        emit this->updatesNotification(false, prev ? prev->replacesId() : 0);
        d->settings.remove(key);
    }
}

void Storeman::startUpdatesTimer()
{
    Q_D(Storeman);

    if (this->checkForUpdates() &&
        OrnPm::instance()->initialised() &&
        NetworkManager::instance()->connected())
    {
        qDebug("Starting updates timer");
        auto delta = QDateTime::currentMSecsSinceEpoch() -
                d->settings.value(UPDATES_LAST_CHECK).toLongLong();
        if (delta >= this->updateInterval())
        {
            this->refreshRepos();
        }
        d->updatesTimer.start();
    }
    else
    {
        qDebug("Stopping updates timer");
        d->updatesTimer.stop();
    }
}

void Storeman::checkSystemVersion()
{
    Q_D(Storeman);

    QString key{QStringLiteral("refresh_cache/os_version")};

    auto lastVersion = QVersionNumber::fromString(d->settings.value(key).toString());
    auto version     = OrnUtils::systemVersion();

    d->settings.setValue(key, version.toString());

    if (lastVersion.isNull() || version == lastVersion)
    {
        return;
    }

    qDebug() << "Repo refresh is required due to OS upgrade";
    OrnPm::instance()->refreshCache(true);
}

void Storeman::checkRepos()
{
    Q_D(Storeman);

    QString key{QStringLiteral("repo_suggested")};
    if (!d->settings.contains(key))
    {
        QtConcurrent::run(d, &StoremanPrivate::checkRepos);
        d->settings.setValue(key, true);
    }
}

void StoremanPrivate::checkRepos()
{
    auto repos = OrnPm::instance()->repoList();
    QString author{QStringLiteral("osetr")};
    auto it = std::find_if(repos.begin(), repos.end(), [author](const OrnRepo &repo)
    {
        return (repo.author == author);
    });

    Q_Q(Storeman);
    if (it == repos.end())
    {
        // Repo is not added
        emit q->repoSuggestion(author, false);
    }
    else if (!it->enabled)
    {
        // Repo is added but disabled
        emit q->repoSuggestion(author, true);
    }
}

void StoremanPrivate::refreshRepos()
{
    OrnPm::instance()->refreshRepos();
    settings.setValue(UPDATES_LAST_CHECK, QDateTime::currentMSecsSinceEpoch());
    emit q_func()->recentAppsChanged();
}
