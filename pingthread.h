#ifndef PINGTHREAD_H
#define PINGTHREAD_H

#include "QThread"

class PingThread: public QThread
{
    Q_OBJECT
    QString url;
    void run();
public:
    PingThread(QString url, QObject *parent):
        QThread(parent),url(url){}
signals:
    void Result(PingThread *thread, QString url, QString time);
};

#endif // PINGTHREAD_H
