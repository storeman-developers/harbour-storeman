/*!
    \fn void Storeman::updatesNotification(bool show, quint32 replaceId)

    Emitted when the updates notification state should be changed.

    \a show holds if the notification should be showed or closed.

    \a replaceId holds the notification ID to replace or
    0 if a new notification shuld be published.
 */

#include "storeman.h"

#include <nemonotifications-qt5/notification.h>
#include <connman-qt5/networkmanager.h>

#include <QSettings>
#include <QCoreApplication>
#include <QDateTime>
#include <QTimer>
#include <QFileInfo>

#include <ornpm.h>


Storeman *Storeman::gInstance = nullptr;

Storeman::Storeman(QObject *parent)
    : QObject(parent)
    , mSettings(new QSettings(this))
    , mUpdatesTimer(new QTimer(this))
{
    auto ornpm = OrnPm::instance();
    connect(ornpm, &OrnPm::updatablePackagesChanged,
            this, &Storeman::onUpdatablePackagesChanged);

    mUpdatesTimer->setInterval(this->updateInterval());
    mUpdatesTimer->setSingleShot(false);
    connect(mUpdatesTimer, &QTimer::timeout, this, &Storeman::refreshRepos);

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

    if (!gInstance)
    {
        gInstance = new Storeman(qApp);
    }

    return gInstance;
}

int Storeman::updateInterval() const
{
    // Default update interval is 10 minutes
    return mSettings->value(QStringLiteral("updates/interval"), 600000).toInt();
}

void Storeman::setUpdateInterval(const int &value)
{
    Q_ASSERT(value > 0);
    if (this->updateInterval() != value)
    {
        mSettings->setValue(QStringLiteral("updates/interval"), value);
        mUpdatesTimer->setInterval(value);
        emit this->updateIntervalChanged();
    }
}

bool Storeman::removeFile(const QString &filePath)
{
    Q_ASSERT_X(!QFileInfo(filePath).isDir(), Q_FUNC_INFO, "Path must be a file");
    return QFile(filePath).remove();
}

void Storeman::resetUpdatesTimer()
{
    this->refreshRepos();
    mUpdatesTimer->start();
}

void Storeman::refreshRepos()
{
    OrnPm::instance()->refreshRepos();
    mSettings->setValue(QStringLiteral("updates/last_check"),
                        QDateTime::currentMSecsSinceEpoch());
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

    if (ornpm->updatesAvailable())
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
    if (OrnPm::instance()->initialised() &&
        NetworkManager::instance()->state() == QLatin1String("online"))
    {
        qDebug("Starting updates timer");
        auto delta = QDateTime::currentMSecsSinceEpoch() -
                mSettings->value(QStringLiteral("updates/last_check")).toLongLong();
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
