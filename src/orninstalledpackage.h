#ifndef ORNINSTALLEDPACKAGE_H
#define ORNINSTALLEDPACKAGE_H


#include <QMetaType>
#include <QString>

struct OrnInstalledPackage
{
    bool updateAvailable{false};
    QString id;
    QString name;
    QString title;
    QString icon;
};

using OrnInstalledPackageList = QList<OrnInstalledPackage>;

Q_DECLARE_METATYPE(QList<OrnInstalledPackage>)

#endif // ORNINSTALLEDPACKAGE_H
