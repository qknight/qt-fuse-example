#include <QtCore/QCoreApplication>
#include <QDebug>
#include <QObject>
#include <QThread>
#include <signal.h>


#include "QFuse.hh"
#include "MyDaemon.hh"

static int setup_unix_signal_handlers() {
    struct sigaction sigHup, sigTerm, sigInt;

    sigHup.sa_handler = MyDaemon::hupSignalHandler;
    sigemptyset(&sigHup.sa_mask);
    sigHup.sa_flags = 0;
    sigHup.sa_flags |= SA_RESTART;

    if (sigaction(SIGHUP, &sigHup, 0) > 0)
        return 1;

    sigTerm.sa_handler = MyDaemon::termSignalHandler;
    sigemptyset(&sigTerm.sa_mask);
    sigTerm.sa_flags |= SA_RESTART;

    if (sigaction(SIGTERM, &sigTerm, 0) > 0)
        return 2;

    sigInt.sa_handler = MyDaemon::intSignalHandler;
    sigemptyset(&sigInt.sa_mask);
    sigInt.sa_flags |= SA_RESTART;

    if (sigaction(SIGINT, &sigInt, 0) > 0)
        return 3;

    return 0;
}

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);
    MyDaemon d;
    QFuse qFuse;

//     QThread* workerThread1 = new QThread;
//     QObject::connect(workerThread1, SIGNAL(started()), &qFuse, SLOT(doWork()));
//     QObject::connect(workerThread1, SIGNAL(finished()), &qFuse, SLOT(deleteLater()));

    QObject::connect(&d, SIGNAL(sigINT()), &qFuse, SLOT(shutDown()));
    QObject::connect(&d, SIGNAL(sigTERM()), &qFuse, SLOT(shutDown()));
    QObject::connect(&qFuse, SIGNAL(sigShutDownComplete()), &a, SLOT(quit()));
    
    // fusermount -u dst will trigger this slot at qFuse
    QObject::connect(&a, SIGNAL(aboutToQuit()), &qFuse, SLOT(shutDown()));

    if (setup_unix_signal_handlers() != 0) {
        qFatal("setup_unix_signal_handlers couldn't install the signal handles properly");
        exit(1);
    }

    // startup of FUSE backend
//     qFuse.moveToThread(workerThread1);
//     workerThread1->start();
    qFuse.doWork();
    
    return a.exec();
}



