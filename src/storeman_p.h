#pragma once

#include "storeman.h"

#include <QtCore/private/qobject_p.h>
#include <QCache>
#include <QSettings>
#include <QTimer>


class StoremanPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(Storeman)

    void refreshRepos();

    QSettings settings;
    QTimer    updatesTimer;
    QCache<quint32, OrnApplication> appsCache;
};
