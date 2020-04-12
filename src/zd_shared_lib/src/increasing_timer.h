#ifndef INCREASING_TIMER_H
#define INCREASING_TIMER_H

#include "zd_shared_lib_global.h"
#include <QObject>

class QTimer;
class QElapsedTimer;

class ZD_SHARED_LIB_EXPORT increasing_timer : public QObject
{
  Q_OBJECT
public:
  increasing_timer(QObject * parent = nullptr);
  ~increasing_timer();
  void set_increase_range(int from_msec, int to_msec);
  void set_reset_after(int msec);
  void set_increase_step(int msec);
  int start();
  int start(int msec);
  void stop();

signals:
  void timeout();

private:
  QTimer * timer;
  QElapsedTimer * reset_timer;
  int increase_from_msec;
  int increase_to_msec;
  int increase_step_msec;
  int reset_after_msec;
  int interval_msec;
};

#endif // INCREASING_TIMER_H
