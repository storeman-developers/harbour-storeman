#include "orncategorylistitem.h"
#include "orn.h"
#include "ornapirequest.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

#include <QDebug>

const QMap<quint32, const char*> OrnCategoryListItem::categories{
    //% "Coding Competition"
    { 3092, QT_TRID_NOOP("orn-cat-coding-competition") },
    //% "Applications"
    {    1, QT_TRID_NOOP("orn-cat-applications") },
    //% "Application"
    {  257, QT_TRID_NOOP("orn-cat-application") },
    //% "Ambience & Themes"
    { 1845, QT_TRID_NOOP("orn-cat-ambience-themes") },
    //% "Business"
    {    2, QT_TRID_NOOP("orn-cat-business") },
    //% "City guides & maps"
    {    3, QT_TRID_NOOP("orn-cat-city-guides-maps") },
    //% "Education & Science"
    { 1324, QT_TRID_NOOP("orn-cat-education-science") },
    //% "Entertainment"
    {    4, QT_TRID_NOOP("orn-cat-entertainment") },
    //% "Music"
    {    5, QT_TRID_NOOP("orn-cat-music") },
    //% "Network"
    {    8, QT_TRID_NOOP("orn-cat-network") },
    //% "News & info"
    {    6, QT_TRID_NOOP("orn-cat-news-info") },
    //% "Patches"
    { 2983, QT_TRID_NOOP("orn-cat-patches") },
    //% "Photo & video"
    {    7, QT_TRID_NOOP("orn-cat-photo-video") },
    //% "Public Transport"
    { 3755, QT_TRID_NOOP("orn-cat-public-transport") },
    //% "Social Networks"
    {    9, QT_TRID_NOOP("orn-cat-social-networks") },
    //% "Sports"
    {   10, QT_TRID_NOOP("orn-cat-sports") },
    //% "System"
    {  147, QT_TRID_NOOP("orn-cat-system") },
    //% "Unknown"
    {  250, QT_TRID_NOOP("orn-cat-unknown") },
    //% "Utilities"
    {   11, QT_TRID_NOOP("orn-cat-utilities") },
    //% "Games"
    {   12, QT_TRID_NOOP("orn-cat-games") },
    //% "Game"
    {  256, QT_TRID_NOOP("orn-cat-game") },
    //% "Action"
    {   13, QT_TRID_NOOP("orn-cat-action") },
    //% "Adventure"
    {   14, QT_TRID_NOOP("orn-cat-adventure") },
    //% "Arcade"
    {   15, QT_TRID_NOOP("orn-cat-arcade") },
    //% "Card & casino"
    {   16, QT_TRID_NOOP("orn-cat-card-casino") },
    //% "Education"
    {   17, QT_TRID_NOOP("orn-cat-education") },
    //% "Puzzle"
    {   18, QT_TRID_NOOP("orn-cat-puzzle") },
    //% "Sports"
    {   19, QT_TRID_NOOP("orn-cat-sports") },
    //% "Strategy"
    {   20, QT_TRID_NOOP("orn-cat-strategy") },
    //% "Trivia"
    {   21, QT_TRID_NOOP("orn-cat-trivia") },
    //% "Translations"
    { 3413, QT_TRID_NOOP("orn-cat-translations") },
    //% "Fonts"
    { 3155, QT_TRID_NOOP("orn-cat-fonts") },
    //% "Libraries"
    {  247, QT_TRID_NOOP("orn-cat-libraries") }
};

OrnCategoryListItem::OrnCategoryListItem(const QJsonObject &jsonObject)
    : categoryId(Orn::toUint(jsonObject[QStringLiteral("tid")]))
    , appsCount(Orn::toUint(jsonObject[QStringLiteral("apps_count")]))
    , depth(jsonObject[QStringLiteral("depth")].toVariant().toUInt())
    , name(categoryName(categoryId))
{}

QString OrnCategoryListItem::categoryName(const quint32 &tid)
{
    if (categories.contains(tid))
    {
        return qtTrId(categories[tid]);
    }
    else
    {
        qWarning() << "Categories dictionary does not contain tid"
                   << tid << "- dictionary update can be required";
        //% "Unknown category"
        return qtTrId("orn-cat-unknown2");
    }
}

OrnItemList OrnCategoryListItem::parse(const QJsonObject &jsonObject)
{
    OrnItemList list;
    QString childrenKey(QStringLiteral("childrens"));
    if (jsonObject.contains(childrenKey))
    {
        auto childrenArray = jsonObject[childrenKey].toArray();
        for (const QJsonValueRef child : childrenArray)
        {
            list << OrnCategoryListItem::parse(child.toObject());
        }
        std::sort(list.begin(), list.end(),
                  [](OrnAbstractListItem *a, OrnAbstractListItem *b)
        {
            return static_cast<OrnCategoryListItem *>(a)->name <
                   static_cast<OrnCategoryListItem *>(b)->name;
        });

    }
    list.prepend(new OrnCategoryListItem(jsonObject));
    return list;
}
