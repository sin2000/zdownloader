#ifndef WIN_SIGWATCH_H
#define WIN_SIGWATCH_H

#include <QObject>
#include <Windows.h>

class win_sigwatch : public QObject
{
  Q_OBJECT
public:
  win_sigwatch(QObject * parent = nullptr);
  void watch_for_signals();

signals:
  void signal_received();

private:
  static BOOL WINAPI CtrlHandler(DWORD fdwCtrlType);
  static win_sigwatch * myobj;
};

#endif // WIN_SIGWATCH_H
