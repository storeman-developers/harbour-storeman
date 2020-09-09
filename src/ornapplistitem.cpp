#include "ornapplistitem.h"
#include "orncategorylistitem.h"
#include "ornutils.h"
#include "ornconst.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <QVariant>


QString sinceLabel(quint32 value)
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

OrnAppListItem::OrnAppListItem(const QJsonObject &data)
    : valid(data.size() > 1)
    , appId(data[OrnConst::appid].toVariant().toUInt())
    , title(OrnUtils::toString(data[OrnConst::title]))
    , iconSource(OrnUtils::toString(data[OrnConst::icon].toObject()[OrnConst::url]))
{
    auto created = OrnUtils::toUint(data[OrnConst::created]);
    if (created > 0)
    {
        createDate.setMSecsSinceEpoch(qint64(created) * 1000);
        sinceUpdate = sinceLabel(created);
    }

    auto ratingobj = data[OrnConst::rating].toObject();
    ratingCount    = OrnUtils::toUint(ratingobj[OrnConst::count]);
    rating         = ratingobj[OrnConst::rating].toString().toFloat();

    userName = OrnUtils::toString(data[OrnConst::user].toObject()[OrnConst::name]);

    auto categories = data[OrnConst::category].toArray();
    auto tid = OrnUtils::toUint(categories.last().toObject()[OrnConst::tid]);
    category = OrnCategoryListItem::categoryName(tid);
    categoryId = tid;

    package = OrnUtils::toString(data[OrnConst::package].toObject()[OrnConst::name]);

    if (iconSource.isEmpty() ||
        iconSource.endsWith(QStringLiteral("icon-defaultpackage.png")))
    {
        iconSource = QStringLiteral("image://theme/icon-launcher-default");
    }
}
