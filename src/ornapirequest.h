#ifndef ORNAPIREQUEST_H
#define ORNAPIREQUEST_H

#include <QObject>
#include <QUrl>

class QNetworkReply;
class QNetworkRequest;

class OrnApiRequest : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool running READ running NOTIFY runningChanged)

public:
    explicit OrnApiRequest(QObject *parent = nullptr);
    ~OrnApiRequest();

    void run(const QNetworkRequest &request);
    bool running() const;

    inline static QUrl apiUrl(const QString &resource) { return QUrl(apiUrlPrefix + resource); }

    static QNetworkRequest networkRequest(const QUrl &url);

public slots:
    void reset();

signals:
    void runningChanged(bool);
    void jsonReady(const QJsonDocument &jsonDoc);

protected slots:
    void onReplyFinished();

protected:
    QNetworkReply *mNetworkReply;

private:
    static const QString apiUrlPrefix;
    static const QByteArray langName;
    static const QByteArray langValue;
    static const QByteArray platformName;
    static const QByteArray platformValue;
};

#endif // ORNAPIREQUEST_H
