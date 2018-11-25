#ifndef ORNBOOKMARKSMODEL_H
#define ORNBOOKMARKSMODEL_H

#include "ornabstractappsmodel.h"

class OrnBookmarksModel : public OrnAbstractAppsModel
{
    Q_OBJECT

public:
    explicit OrnBookmarksModel(QObject *parent = nullptr);

private:
    QJsonArray mFetchedApps;

    // QAbstractItemModel interface
public:
    void fetchMore(const QModelIndex &parent);
};

#endif // ORNBOOKMARKSMODEL_H
