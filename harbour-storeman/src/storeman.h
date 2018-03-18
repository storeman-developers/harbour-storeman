#ifndef STOREMAN_H
#define STOREMAN_H

#include <QObject>

class QQmlEngine;
class QJSEngine;
class QSettings;
class QTimer;

class Storeman : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int updateInterval READ updateInterval WRITE setUpdateInterval NOTIFY updateIntervalChanged)
    Q_PROPERTY(bool showUpdatesNotification READ showUpdatesNotification WRITE setShowUpdatesNotification NOTIFY showUpdatesNotificationChanged)

public:
    enum Hint
    {
        CommentDelegateHint,
        CommentFieldHint,
        ApplicationRatingHint
    };
    Q_ENUM(Hint)

    static QObject *qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine);

    int updateInterval() const;
    void setUpdateInterval(const int &value);

    bool showUpdatesNotification() const;
    void setShowUpdatesNotification(bool value);

    Q_INVOKABLE static bool removeFile(const QString &filePath);

    Q_INVOKABLE bool showHint(const Hint &hint) const;
    Q_INVOKABLE void setHintShowed(const Hint &hint);

public slots:
    void resetUpdatesTimer();

signals:
    void updateIntervalChanged();
    void showUpdatesNotificationChanged();
    void updatesNotification(bool show, quint32 replaceId);

private slots:
    void refreshRepos();
    void onUpdatablePackagesChanged();
    void startUpdatesTimer();

private:
    explicit Storeman(QObject *parent = nullptr);

    QSettings *mSettings;
    QTimer *mUpdatesTimer;

    static Storeman *gInstance;
};

#endif // STOREMAN_H
