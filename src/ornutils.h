#ifndef ORNUTILS_H
#define ORNUTILS_H

#include <QDateTime>
#include <QJsonValue>

namespace OrnUtils
{

inline quint32 toUint(const QJsonValue &value)
{
    return value.toString().remove(QChar(',')).toUInt();
}

inline QString toString(const QJsonValue &value)
{
    return value.toString().trimmed();
}

inline QDateTime toDateTime(const QJsonValue &value)
{
    return QDateTime::fromMSecsSinceEpoch(qint64(toUint(value)) * 1000);
}

QList<quint32> toIntList(const QJsonValue &value);

QString locate(const QString &filename);

inline QString packageName(const QString &id)
{
    return id.section(QChar(';'), 0, 0);
}

inline QString packageVersion(const QString &id)
{
    return id.section(QChar(';'), 1, 1);
}

inline QString packageArch(const QString &id)
{
    return id.section(QChar(';'), 2, 2);
}

inline QString packageRepo(const QString &id)
{
    return id.section(QChar(';'), 3, 3);
}

} // namespace OrnUtils

#endif // ORNUTILS_H
