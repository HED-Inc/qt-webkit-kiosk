#ifndef UNIXSIGNALS_H
#define UNIXSIGNALS_H

/**
 * UnixSignals
 * Inspired by https://doc.qt.io/qt-5/unix-signals.html
 */

#include <QObject>
#include <QSocketNotifier>

class UnixSignals : public QObject
{
    Q_OBJECT

public:
    UnixSignals( QObject *parent = 0 );

    void setupUnixSignalHandlers();

    // Unix signal handlers
    static void hupSignalHandler(int unused);
    static void intSignalHandler(int unused);
    static void termsignalHandler(int unused);
    static void usr1SignalHandler(int unused);
    static void usr2SignalHandler(int unused);

Q_SIGNALS:
    void sigHUP();
    void sigINT();
    void sigTERM();
    void sigUSR1();
    void sigUSR2();

private slots:
    // Qt signal handlers
    void handleSigHup();
    void handleSigInt();
    void handleSigTerm();
    void handleSigUsr1();
    void handleSigUsr2();

private:
    void setupUnixSignalHandler(int, void (*)(int));
    static int sighupFd[2];
    static int sigintFd[2];
    static int sigtermFd[2];
    static int sigusr1Fd[2];
    static int sigusr2Fd[2];

    QSocketNotifier *snHup;
    QSocketNotifier *snInt;
    QSocketNotifier *snTerm;
    QSocketNotifier *snUsr1;
    QSocketNotifier *snUsr2;
};

#endif // UNIXSIGNALS_H
