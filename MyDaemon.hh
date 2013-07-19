// http://harmattan-dev.nokia.com/docs/library/html/qt4/unix-signals.html
// http://qt-project.org/forums/viewthread/1227

#ifndef MYDAEMON__HH
#define MYDAEMON__HH

#include <QSocketNotifier>


class MyDaemon : public QObject
{
    Q_OBJECT

public:
    MyDaemon(QObject *parent = 0);
    ~MyDaemon();

    // Unix signal handlers.
    static void intSignalHandler(int unused);
    static void hupSignalHandler(int unused);
    static void termSignalHandler(int unused);

public slots:
    // Qt signal handlers.
    void handleSigInt();
    void handleSigHup();
    void handleSigTerm();
signals:
    void sigTERM();
    void sigHUP();
    void sigINT();

private:
    static int sigintFd[2];
    static int sighupFd[2];
    static int sigtermFd[2];

    QSocketNotifier *snInt;
    QSocketNotifier *snHup;
    QSocketNotifier *snTerm;

};

#endif
