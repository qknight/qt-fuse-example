#ifndef QFUSE_HH
#define QFUSE_HH

#include <QtCore/QCoreApplication>
#include <QDebug>
#include <QObject>

#include <fuse.h>


class CppSignalWrapper: public QObject {
    Q_OBJECT
public:
    void fuseExited() {
        emit fuseExitedSig();
    }
signals:
    void fuseExitedSig();
};

class QFuse : public QObject {
    Q_OBJECT
public:
    QFuse(QObject* parent = 0);
    ~QFuse();
signals:
    void sigShutDownComplete();
public slots:
    void doWork();
    int shutDown();
private:
    struct fuse_operations fusefs_oper;
    int fuse_stat;
    int argc;
    char** argv;
};

#endif
