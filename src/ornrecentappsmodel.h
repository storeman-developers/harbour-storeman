#pragma once

#include "ornabstractappsmodel.h"

class OrnRecentAppsModel : public OrnAbstractAppsModel
{
    Q_OBJECT

public:
    explicit OrnRecentAppsModel(QObject *parent = nullptr);

    // QAbstractItemModel interface
public:
    void fetchMore(const QModelIndex &parent) override;
};
