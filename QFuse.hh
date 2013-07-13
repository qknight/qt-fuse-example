#ifndef QFUSE_HH
#define QFUSE_HH

#include <QtCore/QCoreApplication>
#include <QDebug>
#include <QObject>

#include <fuse.h>


class QFuse : public QObject {
    Q_OBJECT
public:
    QFuse(QObject* parent = 0);
signals:
    void sigShutDownComplete();
public slots:
    int doWork();
    int shutDown();
private:
    struct fuse_operations fusefs_oper;
    int fuse_stat;
};

#endif
