#include "increasing_timer.h"
#include <QTimer>
#include <QElapsedTimer>

increasing_timer::increasing_timer(QObject * parent)
  :QObject(parent),
   timer(new QTimer(this)),
   reset_timer(new QElapsedTimer),
   increase_from_msec(0),
   increase_to_msec(0),
   increase_step_msec(1000),
   reset_after_msec(0),
   interval_msec(0)
{
  timer->setSingleShot(true);

  connect(timer, &QTimer::timeout, this, &increasing_timer::timeout);
}

increasing_timer::~increasing_timer()
{
  delete reset_timer;
}

void increasing_timer::set_increase_range(int from_msec, int to_msec)
{
  increase_from_msec = from_msec;
  increase_to_msec = to_msec;
}

void increasing_timer::set_reset_after(int msec)
{
  reset_after_msec = msec;
}

void increasing_timer::set_increase_step(int msec)
{
  increase_step_msec = msec;
}

int increasing_timer::start()
{
  if(reset_timer->isValid() == false)
  {
    interval_msec = increase_from_msec;
  }
  else
  {
    if(reset_timer->hasExpired(reset_after_msec))
    {
      interval_msec = increase_from_msec;
    }
    else
    {
      if(interval_msec < increase_to_msec)
        interval_msec += increase_step_msec;
    }
  }

  reset_timer->start();
  timer->start(interval_msec);

  return interval_msec;
}

int increasing_timer::start(int msec)
{
  timer->start(msec);

  return msec;
}

void increasing_timer::stop()
{
  reset_timer->invalidate();
  timer->stop();
}
