#include "ornapplistitem.h"
#include "orncategorylistitem.h"
#include "ornutils.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <QVariant>


OrnAppListItem::OrnAppListItem(const QJsonObject &jsonObject)
    : valid(jsonObject.size() > 1)
    , appId(jsonObject[QStringLiteral("appid")].toVariant().toUInt())
    , created(OrnUtils::toUint(jsonObject[QStringLiteral("created")]))
    , updated(OrnUtils::toUint(jsonObject[QStringLiteral("updated")]))
    , title(OrnUtils::toString(jsonObject[QStringLiteral("title")]))
    , iconSource(OrnUtils::toString(jsonObject[QStringLiteral("icon")].toObject()[QStringLiteral("url")]))
    , sinceUpdate(sinceLabel(created))
{
    QString nameKey(QStringLiteral("name"));

    QString ratingKey(QStringLiteral("rating"));
    auto ratingObject = jsonObject[ratingKey].toObject();
    ratingCount = OrnUtils::toUint(ratingObject[QStringLiteral("count")]);
    rating = ratingObject[ratingKey].toString().toFloat();

    userName = OrnUtils::toString(jsonObject[QStringLiteral("user")].toObject()[nameKey]);

    auto categories = jsonObject[QStringLiteral("category")].toArray();
    auto tid = OrnUtils::toUint(categories.last().toObject()[QStringLiteral("tid")]);
    category = OrnCategoryListItem::categoryName(tid);

    package = OrnUtils::toString(jsonObject[QStringLiteral("package")].toObject()[nameKey]);

    if (iconSource.isEmpty() ||
        iconSource.endsWith(QStringLiteral("icon-defaultpackage.png")))
    {
        iconSource = QStringLiteral("image://theme/icon-launcher-default");
    }
}

QString OrnAppListItem::sinceLabel(quint32 value)
{
    auto curDate = QDate::currentDate();
    auto date = QDateTime::fromMSecsSinceEpoch(qint64(value) * 1000).date();
    auto days = date.daysTo(curDate);
    if (days == 0)
    {
        //% "Today"
        return qtTrId("orn-today");
    }
    if (days == 1)
    {
        //% "Yesterday"
        return qtTrId("orn-yesterday");
    }
    if (days < 7 && date.dayOfWeek() < curDate.dayOfWeek())
    {
        //% "This week"
        return qtTrId("orn-this-week");
    }
    if (days < curDate.daysInMonth() && date.day() < curDate.day())
    {
        //% "This month"
        return qtTrId("orn-this-month");
    }
    //: Output format for the month and year - %0 is a long month name and %1 is a year (for example "May 2017")
    //% "%0 %1"
    return qtTrId("orn-month-format").arg(
                QDate::longMonthName(date.month(), QDate::StandaloneFormat)).arg(date.year());
}
