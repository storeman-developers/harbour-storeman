#ifndef ORNCOMMENTSMODEL_H
#define ORNCOMMENTSMODEL_H

#include "ornabstractlistmodel.h"
#include "orncommentlistitem.h"

class OrnCommentListItem;
class QNetworkReply;

class OrnCommentsModel : public OrnAbstractListModel<OrnCommentListItem>
{
    Q_OBJECT
    Q_PROPERTY(quint32 appId READ appId WRITE setAppId NOTIFY appIdChanged)

public:
    enum Role
    {
        CommentIdRole = Qt::UserRole,
        ParentIdRole,
        CreatedRole,
        UserIdRole,
        UserNameRole,
        ParentUserNameRole,
        UserIconSourceRole,
        TextRole
    };
    Q_ENUM(Role)

    explicit OrnCommentsModel(QObject *parent = nullptr);

    quint32 appId() const;
    void setAppId(quint32 appId);

    Q_INVOKABLE int findItemRow(quint32 cid) const;

signals:
    void appIdChanged();

private:
    QNetworkReply *fetchComment(quint32 cid);
    QJsonObject processReply(QNetworkReply *reply);

private:
    quint32 mAppId;

    // QAbstractItemModel interface
public:
    QVariant data(const QModelIndex &index, int role) const;
    void fetchMore(const QModelIndex &parent);
    QHash<int, QByteArray> roleNames() const;
};

#endif // ORNCOMMENTSMODEL_H
