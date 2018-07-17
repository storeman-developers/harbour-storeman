#include "ornapplistitem.h"
#include "orn.h"
#include "orncategorylistitem.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <QVariant>


OrnAppListItem::OrnAppListItem(const QJsonObject &jsonObject)
    : appId(jsonObject[QStringLiteral("appid")].toVariant().toUInt())
    , created(Orn::toUint(jsonObject[QStringLiteral("created")]))
    , updated(Orn::toUint(jsonObject[QStringLiteral("updated")]))
    , title(Orn::toString(jsonObject[QStringLiteral("title")]))
    , iconSource(Orn::toString(jsonObject[QStringLiteral("icon")].toObject()[QStringLiteral("url")]))
    , sinceUpdate(sinceLabel(created))
{
    QString nameKey(QStringLiteral("name"));

    QString ratingKey(QStringLiteral("rating"));
    auto ratingObject = jsonObject[ratingKey].toObject();
    ratingCount = Orn::toUint(ratingObject[QStringLiteral("count")]);
    rating = ratingObject[ratingKey].toString().toFloat();

    userName = Orn::toString(jsonObject[QStringLiteral("user")].toObject()[nameKey]);

    auto categories = jsonObject[QStringLiteral("category")].toArray();
    auto tid = Orn::toUint(categories.last().toObject()[QStringLiteral("tid")]);
    category = OrnCategoryListItem::categoryName(tid);

    package = Orn::toString(jsonObject[QStringLiteral("package")].toObject()[nameKey]);
}

QString OrnAppListItem::sinceLabel(const quint32 &value)
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
