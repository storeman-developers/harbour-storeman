#pragma once

#include "ornappsmodel.h"

class OrnBookmarksModel : public OrnAppsModel
{
    Q_OBJECT

public:
    explicit OrnBookmarksModel(QObject *parent = nullptr);

private:
    QJsonArray mFetchedApps;

    // QAbstractItemModel interface
public:
    void fetchMore(const QModelIndex &parent) override;
};
