#ifndef ORNPACKAGEVERSION_H
#define ORNPACKAGEVERSION_H


#include <QVariantList>

struct OrnPackageVersion
{
    quint64 downloadSize{0};
    quint64 installSize{0};
    QString version;
    QString arch;
    QString repoAlias;

    OrnPackageVersion() = default;
    OrnPackageVersion(const QString &version);
    OrnPackageVersion(quint64 dsize, quint64 isize,
                      const QString &version, QString arch, QString alias);

    void clear();

    QString packageId(const QString &name) const;

    bool operator ==(const OrnPackageVersion &other) const;
    inline bool operator !=(const OrnPackageVersion &other) const
    { return !this->operator ==(other); }

    bool operator <(const OrnPackageVersion &other) const;

private:
    QVariantList versionParts;
};

using OrnPackageVersionList = QList<OrnPackageVersion>;

Q_DECLARE_METATYPE(QList<OrnPackageVersion>)

#endif // ORNPACKAGEVERSION_H
