#include "orn.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QStandardPaths>
#include <QDir>
#include <QNetworkAccessManager>
#include <QCoreApplication>

#include <QDebug>

namespace Orn
{

QList<quint32> toIntList(const QJsonValue &value)
{
    auto array = value.toArray();
    QString tidKey(QStringLiteral("tid"));
    QList<quint32> list;
    for (const QJsonValueRef v : array)
    {
        list << toUint(v.toObject()[tidKey]);
    }
    return list;
}

QString locate(const QString &filename)
{
    auto dir = QDir(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
    if (!dir.exists() && !dir.mkpath(QChar('.')))
    {
        qWarning() << "Could not create local data dir" << dir.path();
        return QString();
    }
    return dir.absoluteFilePath(filename);
}

static QNetworkAccessManager *nam = nullptr;

QNetworkAccessManager *networkAccessManager()
{
    if (!nam)
    {
        nam = new QNetworkAccessManager(qApp);
    }
    return nam;
}

} // namespace Orn
