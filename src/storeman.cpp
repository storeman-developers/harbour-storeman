/*!
    \fn void Storeman::updatesNotification(bool show, quint32 replaceId)

    Emitted when the updates notification state should be changed.

    \a show holds if the notification should be showed or closed.

    \a replaceId holds the notification ID to replace or
    0 if a new notification shuld be published.
 */

#include "storeman.h"

#include <notification.h>
#include <networkmanager.h>

#include <QSettings>
#include <QDateTime>
#include <QTimer>
#include <QFileInfo>
#include <QtConcurrentRun>
#include <QNetworkReply>

#include "ornutils.h"
#include "ornpm.h"
#include "ornapplication.h"
#include "ornrepo.h"
#include "ornclient.h"

#define STOREMAN_AUTHOR QStringLiteral("osetr")

#define MAINPAGE_SHOW_RECENT        QStringLiteral("mainpage/show_recent")
#define UPDATES_INTERVAL            QStringLiteral("updates/interval")
#define UPDATES_ENABLED             QStringLiteral("updates/enabled")
#define UPDATES_SMART               QStringLiteral("updates/smart")
#define UPDATES_SHOW_NOTIFICATION   QStringLiteral("updates/show_notification")
#define UPDATES_LAST_CHECK          QStringLiteral("updates/last_check")
#define HINTS                       QStringLiteral("hints/")


Storeman::Storeman(QObject *parent)
    : QObject(parent)
    , mSettings(new QSettings(this))
    , mUpdatesTimer(new QTimer(this))
{
    mAppsCache.setMaxCost(3);

    auto ornpm = OrnPm::instance();
    connect(ornpm, &OrnPm::updatablePackagesChanged,
            this, &Storeman::onUpdatablePackagesChanged);

    mUpdatesTimer->setInterval(this->updateInterval());
    mUpdatesTimer->setSingleShot(false);
    connect(mUpdatesTimer, &QTimer::timeout, this, &Storeman::refreshRepos);

    connect(this, &Storeman::checkForUpdatesChanged,
            this, &Storeman::startUpdatesTimer);
    connect(ornpm, &OrnPm::initialisedChanged,
            this, &Storeman::startUpdatesTimer);
    connect(NetworkManager::instance(), &NetworkManager::stateChanged,
            this, &Storeman::startUpdatesTimer);
    this->startUpdatesTimer();
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
    return mSettings->value(MAINPAGE_SHOW_RECENT).toBool();
}

void Storeman::setShowRecentOnStart(bool value)
{
    if (value != this->showRecentOnStart())
    {
        mSettings->setValue(MAINPAGE_SHOW_RECENT, value);
        emit this->showRecentOnStartChanged();
    }
}

int Storeman::updateInterval() const
{
    // Default update interval is 10 minutes
    return mSettings->value(UPDATES_INTERVAL, 600000).toInt();
}

void Storeman::setUpdateInterval(int value)
{
    Q_ASSERT(value > 0);
    if (this->updateInterval() != value)
    {
        mSettings->setValue(UPDATES_INTERVAL, value);
        mUpdatesTimer->setInterval(value);
        emit this->updateIntervalChanged();
    }
}

bool Storeman::checkForUpdates() const
{
    return mSettings->value(UPDATES_ENABLED, true).toBool();
}

void Storeman::setCheckForUpdates(bool value)
{
    if (value != this->checkForUpdates())
    {
        mSettings->setValue(UPDATES_ENABLED, value);
        emit this->checkForUpdatesChanged();
    }
}

bool Storeman::smartUpdate() const
{
    return mSettings->value(UPDATES_SMART, true).toBool();
}

void Storeman::setSmartUpdate(bool value)
{
    if (value != this->smartUpdate())
    {
        mSettings->setValue(UPDATES_SMART, value);
        emit this->smartUpdateChanged();
    }
}

bool Storeman::showUpdatesNotification() const
{
    return mSettings->value(UPDATES_SHOW_NOTIFICATION, true).toBool();
}

void Storeman::setShowUpdatesNotification(bool value)
{
    if (value != this->showUpdatesNotification())
    {
        mSettings->setValue(UPDATES_SHOW_NOTIFICATION, value);
        emit this->showUpdatesNotificationChanged();
        this->onUpdatablePackagesChanged();
    }
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

bool Storeman::showHint(Storeman::Hint hint) const
{
    auto me = QMetaEnum::fromType<Hint>();
    auto name = me.valueToKey(hint);
    Q_ASSERT(name);
    return !mSettings->value(HINTS.append(name), false).toBool();
}

void Storeman::setHintShowed(Storeman::Hint hint)
{
    auto me = QMetaEnum::fromType<Hint>();
    auto name = me.valueToKey(hint);
    Q_ASSERT(name);
    mSettings->setValue(HINTS.append(name), true);
}

OrnApplication *Storeman::cachedApp(quint32 appId)
{
    if (appId == 0)
    {
        return nullptr;
    }

    if (mAppsCache.contains(appId))
    {
        return mAppsCache[appId];
    }

    auto app = new OrnApplication(this);
    app->setAppId(appId);
    mAppsCache.insert(appId, app);
    return app;
}

void Storeman::resetUpdatesTimer()
{
    OrnPm::instance()->refreshRepos();
    mSettings->setValue(UPDATES_LAST_CHECK, QDateTime::currentMSecsSinceEpoch());
    mUpdatesTimer->start();
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
                    auto lastUpdate = OrnUtils::toUint(json[0].toObject()[QStringLiteral("updated")]);
                    auto lastCheck = mSettings->value(UPDATES_LAST_CHECK).toLongLong();
                    if (qlonglong(lastUpdate) * 1000 > lastCheck)
                    {
                        OrnPm::instance()->refreshRepos();
                        mSettings->setValue(UPDATES_LAST_CHECK, QDateTime::currentMSecsSinceEpoch());
                    }
                }
            }
            reply->deleteLater();
        });
    }
    else
    {
        OrnPm::instance()->refreshRepos();
        mSettings->setValue(UPDATES_LAST_CHECK, QDateTime::currentMSecsSinceEpoch());
    }
}

inline Notification *previousNotification()
{
    Notification *prev = nullptr;
    QLatin1String category("x-storeman.updates");
    for (auto o : Notification::notifications())
    {
        auto n = static_cast<Notification *>(o);
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
    QString key(QStringLiteral("updates/last_packages"));
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
        auto prev_packages_set = mSettings->value(key).toStringList().toSet();
        auto newpackages = cur_packages_set == prev_packages_set ?
                    false : !prev_packages_set.contains(cur_packages_set);

        // Do not show notification on application start
        if (newpackages)
        {
            emit this->updatesNotification(true, prev->replacesId());
            prev->deleteLater();
            mSettings->setValue(key, cur_packages);
        }
    }
    else
    {
        emit this->updatesNotification(false, prev ? prev->replacesId() : 0);
        mSettings->remove(key);
    }
}

void Storeman::startUpdatesTimer()
{
    if (this->checkForUpdates() &&
        OrnPm::instance()->initialised() &&
        NetworkManager::instance()->state() == QLatin1String("online"))
    {
        qDebug("Starting updates timer");
        auto delta = QDateTime::currentMSecsSinceEpoch() -
                mSettings->value(UPDATES_LAST_CHECK).toLongLong();
        if (delta >= this->updateInterval())
        {
            this->refreshRepos();
        }
        mUpdatesTimer->start();
    }
    else
    {
        qDebug("Stopping updates timer");
        mUpdatesTimer->stop();
    }
}

void Storeman::checkRepos()
{
    QString key(QStringLiteral("repo_suggested"));
    if (!mSettings->contains(key))
    {
        QtConcurrent::run(this, &Storeman::checkReposImpl);
        mSettings->setValue(key, true);
    }
}

void Storeman::checkReposImpl()
{
    auto repos = OrnPm::instance()->repoList();
    QString author(STOREMAN_AUTHOR);
    auto it = std::find_if(repos.begin(), repos.end(), [author](const OrnRepo &repo)
    {
        return (repo.author == author);
    });
    if (it == repos.end())
    {
        // Repo is not added
        emit this->repoSuggestion(author, false);
    }
    else if (!it->enabled)
    {
        // Repo is added but disabled
        emit this->repoSuggestion(author, true);
    }
}
