#ifndef QFUSE_HH
#define QFUSE_HH

#include <QtCore/QCoreApplication>
#include <QDebug>
#include <QObject>
#include <QtNetwork>

#include <fuse.h>
#include "fuse/fuse_lowlevel.h"

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
    static int fuse_ll_receive_buf2(struct fuse_session *se, struct fuse_buf *buf, struct fuse_chan **chp);
    static void fuse_ll_process_buf2(void *data, const struct fuse_buf *buf, struct fuse_chan *ch);
signals:
    void sigShutDownComplete();
public slots:
    void doWork();
    int shutDown();
private slots:
    void readFile();
//     void displayError(QLocalSocket::LocalSocketError s);
private:
    struct fuse_operations fusefs_oper;
    int fuse_stat;
    int argc;
    char** argv;
//     QLocalSocket* socket;
    QSocketNotifier* sn;
    QFile* file;
    QDataStream* in;
};

#endif
