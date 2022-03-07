#include <signal.h>
#include <QApplication>
#include <QDebug>
#include <sys/socket.h>
#include "unixsignals.h"

/**
 * @brief UnixSignals::UnixSignals
 * @param parent
 *
 * Inspired by https://doc.qt.io/qt-5/unix-signals.html
 * This class should be created only once
 *
 */

int UnixSignals::sighupFd[2];
int UnixSignals::sigintFd[2];
int UnixSignals::sigtermFd[2];
int UnixSignals::sigusr1Fd[2];
int UnixSignals::sigusr2Fd[2];

UnixSignals::UnixSignals( QObject *parent )
    : QObject(parent)
{
    if(::socketpair(AF_UNIX, SOCK_STREAM, 0, sighupFd)) {
        qFatal("%s:%d:%s, Couldn't create SIGHUP socketpair. errno is: %d",
                __FILE__, __LINE__, __func__, errno);
    }

    if(::socketpair(AF_UNIX, SOCK_STREAM, 0, sigintFd)) {
        qFatal("%s:%d:%s, Couldn't create SIGINT socketpair. errno is: %d",
                __FILE__, __LINE__, __func__, errno);
    }

    if(::socketpair(AF_UNIX, SOCK_STREAM, 0, sigtermFd)) {
        qFatal("%s:%d:%s, Couldn't create SIGTERM socketpair. errno is: %d",
                __FILE__, __LINE__, __func__, errno);
    }

    if(::socketpair(AF_UNIX, SOCK_STREAM, 0, sigusr1Fd)) {
        qFatal("%s:%d:%s, Couldn't create SIGUSR1 socketpair. errno is: %d",
                __FILE__, __LINE__, __func__, errno);
    }

    if(::socketpair(AF_UNIX, SOCK_STREAM, 0, sigusr2Fd)) {
        qFatal("%s:%d:%s, Couldn't create SIGUSR2 socketpair. errno is: %d",
                __FILE__, __LINE__, __func__, errno);
    }

    snHup = new QSocketNotifier(sighupFd[1], QSocketNotifier::Read, this);
    connect(snHup, SIGNAL(activated(QSocketDescriptor)), this, SLOT(handleSigHup()));

    snInt = new QSocketNotifier(sigintFd[1], QSocketNotifier::Read, this);
    connect(snInt, SIGNAL(activated(QSocketDescriptor)), this, SLOT(handleSigInt()));

    snTerm = new QSocketNotifier(sigtermFd[1], QSocketNotifier::Read, this);
    connect(snTerm, SIGNAL(activated(QSocketDescriptor)), this, SLOT(handleSigTerm()));

    snUsr1 = new QSocketNotifier(sigusr1Fd[1], QSocketNotifier::Read, this);
    connect(snUsr1, SIGNAL(activated(QSocketDescriptor)), this, SLOT(handleSigUsr1()));

    snUsr2 = new QSocketNotifier(sigusr2Fd[1], QSocketNotifier::Read, this);
    connect(snUsr2, SIGNAL(activated(QSocketDescriptor)), this, SLOT(handleSigUsr2()));
}


void UnixSignals::setupUnixSignalHandlers()
{
    setupUnixSignalHandler(SIGHUP, UnixSignals::hupSignalHandler);
    setupUnixSignalHandler(SIGINT, UnixSignals::intSignalHandler);
    setupUnixSignalHandler(SIGTERM, UnixSignals::termsignalHandler);
    setupUnixSignalHandler(SIGUSR1, UnixSignals::usr1SignalHandler);
    setupUnixSignalHandler(SIGUSR2, UnixSignals::usr2SignalHandler);
}


// Unix signal handlers
void UnixSignals::hupSignalHandler(int signum) {
    ::write(sighupFd[0], &signum, sizeof(signum));
}

void UnixSignals::intSignalHandler(int signum) {
    ::write(sigintFd[0], &signum, sizeof(signum));
}

void UnixSignals::termsignalHandler(int signum) {
    ::write(sigtermFd[0], &signum, sizeof(signum));
}

void UnixSignals::usr1SignalHandler(int signum) {
    ::write(sigusr1Fd[0], &signum, sizeof(signum));
}

void UnixSignals::usr2SignalHandler(int signum) {
    ::write(sigusr2Fd[0], &signum, sizeof(signum));
}

// Qt signal handlers
// we emit a Qt signal unique to each corresponding Unix signal, thus another
// class can then connect to that Qt signal, and take whatever action it wants
void UnixSignals::handleSigHup() {
    snHup->setEnabled(false);
    int tmp;
    ::read(sighupFd[1], &tmp, sizeof(tmp));
    emit sigHUP();
    snHup->setEnabled(true);
}

void UnixSignals::handleSigInt() {
    snInt->setEnabled(false);
    int tmp;
    ::read(sigintFd[1], &tmp, sizeof(tmp));
    emit sigINT();
    snInt->setEnabled(true);
}

void UnixSignals::handleSigTerm() {
    snTerm->setEnabled(false);
    int tmp;
    ::read(sigtermFd[1], &tmp, sizeof(tmp));
    emit sigTERM();
    snTerm->setEnabled(true);
}

void UnixSignals::handleSigUsr1() {
    snUsr1->setEnabled(false);
    int tmp;
    ::read(sigusr1Fd[1], &tmp, sizeof(tmp));
    emit sigUSR1();
    snUsr1->setEnabled(true);
}

void UnixSignals::handleSigUsr2() {
    snUsr2->setEnabled(false);
    int tmp;
    ::read(sigusr2Fd[1], &tmp, sizeof(tmp));
    emit sigUSR2();
    snUsr2->setEnabled(true);
}

void UnixSignals::setupUnixSignalHandler(int signum, void (*sig_handler)(int))
{
    struct sigaction sigact;
    sigact.sa_handler = sig_handler;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = 0;
    sigact.sa_flags |= SA_RESTART;

    if (sigaction(signum, &sigact, NULL)) {
        qFatal("%s:%d:%s, Couldn't install signal handler for %d. errno is: %d",
                __FILE__, __LINE__, __func__, signum, errno);
    }
}
