#ifndef ORNREPOMODEL_H
#define ORNREPOMODEL_H

#include <QAbstractListModel>

#include "ornrepo.h"

class OrnRepoModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(bool hasEnabledRepos READ hasEnabledRepos NOTIFY enabledReposChanged)
    Q_PROPERTY(bool hasDisabledRepos READ hasDisabledRepos NOTIFY enabledReposChanged)

public:

    enum Roles
    {
        RepoAuthorRole = Qt::UserRole + 1,
        RepoAliasRole,
        RepoEnabledRole,
        SortRole
    };
    Q_ENUM(Roles)

    explicit OrnRepoModel(QObject *parent = nullptr);

    bool hasEnabledRepos() const;
    bool hasDisabledRepos() const;

public slots:
    void reset();

signals:
    void enabledReposChanged();

private slots:
    void onRepoModified(const QString &alias, int action);

private:
    int mEnabledRepos;
    OrnRepoList mData;

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;
};

#endif // ORNREPOMODEL_H
