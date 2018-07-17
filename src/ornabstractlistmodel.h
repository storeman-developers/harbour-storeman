#ifndef ORNABSTRACTLISTMODEL_H
#define ORNABSTRACTLISTMODEL_H


#include "ornabstractlistitem.h"

#include <QAbstractListModel>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrlQuery>
#include <QCryptographicHash>

#include <QDebug>

class OrnApiRequest;

class OrnAbstractListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(OrnApiRequest* apiRequest READ apiRequest CONSTANT)

public:
    OrnAbstractListModel(bool fetchable, QObject *parent = nullptr);
    ~OrnAbstractListModel();

    OrnApiRequest *apiRequest() const;

public slots:
    void reset();

signals:
    void replyProcessed();

protected:
    void apiCall(const QString &resource, QUrlQuery query = QUrlQuery());
    template<typename T>
    void processReply(const QJsonDocument &jsonDoc)
    {
        auto jsonArray = jsonDoc.array();
        if (jsonArray.size() > 0)
        {
            if (!mFetchable)
            {
                mCanFetchMore = false;
            }
            else
            {
                // An ugly patch for some models with repeating data (search model)
                auto replyHash = QCryptographicHash::hash(
                            jsonDoc.toJson(), QCryptographicHash::Md5);
                if (mPrevReplyHash == replyHash)
                {
                    qDebug() << "Current reply is equal to the previous one. "
                                "Considering the model has fetched all data";
                    mCanFetchMore = false;
                    return;
                }
                mPrevReplyHash = replyHash;
            }
            OrnItemList list;
            for (const QJsonValueRef jsonValue: jsonArray)
            {
                // Each class of list item should implement a constructor
                // SomeListItem(const QJsonObject &)
                list << new T(jsonValue.toObject());
            }
            auto row = mData.size();
            this->beginInsertRows(QModelIndex(), row, row + list.size() - 1);
            mData.append(list);
            ++mPage;
            qDebug() << list.size() << "item(s) have been added to the model";
            this->endInsertRows();
        }
        else
        {
            qDebug() << "Reply is empty, the model has fetched all data";
            mCanFetchMore = false;
        }
        emit this->replyProcessed();
    }

protected slots:
    virtual void onJsonReady(const QJsonDocument &jsonDoc) = 0;

protected:
    bool    mFetchable;
    bool    mCanFetchMore;
    quint32 mPage;
    OrnItemList mData;
    OrnApiRequest *mApiRequest;

private:
    QByteArray mPrevReplyHash;

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const;
    virtual void fetchMore(const QModelIndex &parent) = 0;
    bool canFetchMore(const QModelIndex &parent) const;
};

#endif // ORNABSTRACTLISTMODEL_H
