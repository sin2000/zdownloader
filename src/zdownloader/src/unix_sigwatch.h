#ifndef SIGWATCH_H
#define SIGWATCH_H

#include <signal.h>
#include <QObject>

class QSocketNotifier;

class unix_sigwatch : public QObject
{
  Q_OBJECT
public:
  unix_sigwatch(QObject * parent = nullptr);
  void watch_for_signal(int signal);

signals:
  void unix_signal(int signal);

private:
  static void signal_handler(int signal);
  void socket_notifier_activated(int socket);

  static int sockpair[2];
  QSocketNotifier * notifier;
  QList<int> watched_signals;
};

#endif // SIGWATCH_H
