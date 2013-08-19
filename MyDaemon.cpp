// http://harmattan-dev.nokia.com/docs/library/html/qt4/unix-signals.html
// http://qt-project.org/forums/viewthread/1227

#include "MyDaemon.hh"

#include <QDebug>

#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <unistd.h>
#include "ColorCodes.hh"


//needed to not get an undefined reference to static members
int MyDaemon::sighupFd[2];
int MyDaemon::sigtermFd[2];
int MyDaemon::sigintFd[2];

MyDaemon::MyDaemon(QObject *parent)
    : QObject(parent)
{
    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, sighupFd))
        qFatal("Couldn't create HUP socketpair");

    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, sigtermFd))
        qFatal("Couldn't create TERM socketpair");

    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, sigintFd))
        qFatal("Couldn't create INT socketpair");

    snInt = new QSocketNotifier(sigintFd[1], QSocketNotifier::Read, this);
    connect(snInt, SIGNAL(activated(int)), this, SLOT(handleSigInt()));

    snHup = new QSocketNotifier(sighupFd[1], QSocketNotifier::Read, this);
    connect(snHup, SIGNAL(activated(int)), this, SLOT(handleSigHup()));

    snTerm = new QSocketNotifier(sigtermFd[1], QSocketNotifier::Read, this);
    connect(snTerm, SIGNAL(activated(int)), this, SLOT(handleSigTerm()));
}


MyDaemon::~MyDaemon() {
    delete snHup;
    delete snTerm;
    delete snInt;
}


void MyDaemon::intSignalHandler(int)
{
    char a = 1;
    ::write(sigintFd[0], &a, sizeof(a));
}


void MyDaemon::hupSignalHandler(int)
{
    char a = 1;
    ::write(sighupFd[0], &a, sizeof(a));
}


void MyDaemon::termSignalHandler(int)
{
    char a = 1;
    ::write(sigtermFd[0], &a, sizeof(a));
}


void MyDaemon::handleSigInt()
{
    snInt->setEnabled(false);
    char tmp;
    ::read(sigintFd[1], &tmp, sizeof(tmp));

    qDebug().nospace() << MAGENTA << "emit sigINT();" << RESET;

    emit sigINT();
    snInt->setEnabled(true);
}


void MyDaemon::handleSigTerm()
{
    snTerm->setEnabled(false);
    char tmp;
    ::read(sigtermFd[1], &tmp, sizeof(tmp));

    qDebug().nospace() << MAGENTA <<  "emit sigTerm()" << RESET;

    emit sigTERM();
    snTerm->setEnabled(true);
}


void MyDaemon::handleSigHup()
{
    snHup->setEnabled(false);
    char tmp;
    ::read(sighupFd[1], &tmp, sizeof(tmp));

    emit sigHUP();
    qDebug().nospace() << MAGENTA <<  "emit sigHUP()" << RESET;

    snHup->setEnabled(true);
}


