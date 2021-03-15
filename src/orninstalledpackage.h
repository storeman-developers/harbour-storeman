#pragma once

#include <QMetaType>
#include <QString>

struct OrnInstalledPackage
{
    bool updateAvailable{false};
    QString id;
    QString name;
    QString title;
    QString titleUnlocalized;
    QString icon;
};

using OrnInstalledPackageList = QList<OrnInstalledPackage>;

Q_DECLARE_METATYPE(QList<OrnInstalledPackage>)
