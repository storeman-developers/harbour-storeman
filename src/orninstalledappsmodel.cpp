#include "orninstalledappsmodel.h"
#include "ornpm.h"
#include "ornutils.h"

#include <QDebug>

OrnInstalledAppsModel::OrnInstalledAppsModel(QObject *parent)
    : QAbstractListModel(parent)
    , mResetting(false)
{
    auto ornPm = OrnPm::instance();
    connect(ornPm, &OrnPm::installedPackages,
            this, &OrnInstalledAppsModel::onInstalledPackages);
    connect(ornPm, &OrnPm::packageInstalled,
            this, &OrnInstalledAppsModel::onPackageInstalled);
    connect(ornPm, &OrnPm::packageRemoved,
            this, &OrnInstalledAppsModel::onPackageRemoved);
    connect(ornPm, &OrnPm::updatablePackagesChanged,
            this , &OrnInstalledAppsModel::onUpdatablePackagesChanged);
    this->reset();
}

void OrnInstalledAppsModel::reset()
{
    qDebug() << "Resetting model";
    this->beginResetModel();
    mResetting = true;
    mData.clear();
    OrnPm::instance()->getInstalledPackages();
}

void OrnInstalledAppsModel::onInstalledPackages(const OrnInstalledPackageList &packages)
{
    if (mResetting)
    {
        mData.append(packages);
        mResetting = false;
        this->endResetModel();
        return;
    }

    auto size = packages.size();
    if (size == 0)
    {
        return;
    }

    auto count = mData.size();
    QModelIndex parentIndex;

    // Just append new apps if there is no items
    if (count == 0)
    {
        this->beginInsertRows(parentIndex, 0, size - 1);
        mData.append(packages);
        this->endInsertRows();
        return;
    }

    // Create a hash for the further work
    QHash<QString, OrnInstalledPackage> packageHash;
    for (const auto &package : packages)
    {
        packageHash[package.name] = package;
    }

    for (OrnInstalledPackageList::size_type i = count - 1; i >= 0; --i)
    {
        // Update updated packages
        auto name = mData[i].name;
        if (packageHash.contains(name))
        {
            qDebug() << "Updating model item" << name;
            mData[i] = packageHash[name];
            auto ind = this->createIndex(i, 0);
            emit this->dataChanged(ind, ind);
            // Remove processed packages from the hash
            packageHash.remove(name);
        }
        // Remove removed packages
        else
        {
            qDebug() << "Removing model item" << name;
            this->beginRemoveRows(parentIndex, i, i);
            mData.removeAt(i);
            this->endRemoveRows();
        }
    }

    // Append other packages to the model
    auto installed = packageHash.values();
    count = mData.size();
    this->beginInsertRows(parentIndex, count, count + installed.size() - 1);
    mData.append(installed);
    this->endInsertRows();
}

void OrnInstalledAppsModel::onPackageInstalled(const QString &packageName)
{
    OrnPm::instance()->getInstalledPackages(packageName);
}

void OrnInstalledAppsModel::onPackageRemoved(const QString &packageName)
{
    auto size = mData.size();
    for (OrnInstalledPackageList::size_type i = 0; i < size; ++i)
    {
        if (mData[i].name == packageName)
        {
            qDebug() << "Removing model item" << packageName;
            this->beginRemoveRows(QModelIndex(), i, i);
            mData.removeAt(i);
            this->endRemoveRows();
            return;
        }
    }
}

void OrnInstalledAppsModel::onUpdatablePackagesChanged()
{
    auto ornPm = OrnPm::instance();
    QVector<int> roles = { SortRole, UpdateAvailableRole };
    auto count = mData.size();
    for (OrnInstalledPackageList::size_type i = 0; i < count; ++i)
    {
        auto &package = mData[i];
        bool ua = ornPm->packageStatus(package.name) ==
                OrnPm::PackageUpdateAvailable;
        if (package.updateAvailable != ua)
        {
            package.updateAvailable = ua;
            auto ind = this->createIndex(i, 0);
            emit this->dataChanged(ind, ind, roles);
        }
    }
}

int OrnInstalledAppsModel::rowCount(const QModelIndex &parent) const
{
    return !parent.isValid() ? mData.size() : 0;
}

QVariant OrnInstalledAppsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    auto package = mData[index.row()];
    switch (role)
    {
    case NameRole:
        return package.name;
    case TitleRole:
        return package.title;
    case VersionRole:
        return OrnUtils::packageVersion(package.id);
    case IconRole:
        return package.icon;
    case SortRole:
        // At first show packages with available updates then sort by title
        return QString::number(!package.updateAvailable).append(package.title);
    case SectionRole:
        return package.title.at(0).toUpper();
    case UpdateAvailableRole:
        // Return int to make it easier to parse
        return int(package.updateAvailable);
    case UpdateVersionRole:
        return OrnPm::instance()->updateVersion(package.name);
    case IdRole:
        return package.id;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> OrnInstalledAppsModel::roleNames() const
{
    return {
        { NameRole,    "packageName"    },
        { TitleRole,   "packageTitle"   },
        { VersionRole, "packageVersion" },
        { IconRole,    "packageIcon"    },
        { SectionRole, "section"        },
        { UpdateAvailableRole, "updateAvailable" },
        { UpdateVersionRole,   "updateVersion" },
        { IdRole,      "packageId"}
    };
}
