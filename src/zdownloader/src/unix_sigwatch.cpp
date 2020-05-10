#include "unix_sigwatch.h"
#include <sys/socket.h>
#include <unistd.h>
#include <cerrno>
#include <QList>
#include <QSocketNotifier>
#include <qdebugex.h>

int unix_sigwatch::sockpair[];

unix_sigwatch::unix_sigwatch(QObject *parent)
  :QObject(parent)
{
  if(::socketpair(AF_UNIX, SOCK_STREAM, 0, sockpair))
  {
    qDebug() << "unix_sigwatch: socketpair: " << ::strerror(errno);
    return;
  }

  notifier = new QSocketNotifier(sockpair[1], QSocketNotifier::Read, this);
  QObject::connect(notifier, &QSocketNotifier::activated, this, &unix_sigwatch::socket_notifier_activated);
  notifier->setEnabled(true);
}

void unix_sigwatch::watch_for_signal(int signal)
{
  if(watched_signals.contains(signal))
  {
    qDebug() << "Already watching for signal" << signal;
    return;
  }

  struct sigaction sigact;
  sigact.sa_handler = unix_sigwatch::signal_handler;
  sigact.sa_flags = 0;
  sigemptyset(&sigact.sa_mask);
  sigact.sa_flags |= SA_RESTART;
  if(::sigaction(signal, &sigact, nullptr))
  {
    qDebug() << "unix_sigwatch - sigaction: " << ::strerror(errno);
    return;
  }

  watched_signals.append(signal);
}

void unix_sigwatch::signal_handler(int signal)
{
  ::write(sockpair[0], &signal, sizeof(signal));
}

void unix_sigwatch::socket_notifier_activated(int socket)
{
  int signal;
  ::read(socket, &signal, sizeof(signal));
  qDebug() << "Caught signal:" << ::strsignal(signal);
  emit unix_signal(signal);
}
