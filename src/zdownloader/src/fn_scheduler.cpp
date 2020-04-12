#include "fn_scheduler.h"
#include <QTimer>
#include <qdebugex.h>

fn_scheduler::fn_scheduler(QObject * parent)
  :QObject(parent),
   check_timer(new QTimer(this))
{
  check_timer->setSingleShot(true);
  check_timer->setTimerType(Qt::PreciseTimer);
  connect(check_timer, &QTimer::timeout, this, &fn_scheduler::timeout);
}

void fn_scheduler::set_activation_time(const QTime & time)
{
  act_date.setTime(time);
}

void fn_scheduler::start()
{
  const QDateTime now = QDateTime::currentDateTime();
  act_date.setDate(now.date());
  if(act_date.time() < now.time())
    act_date = act_date.addDays(1);

  const int msecs = static_cast<int>(now.msecsTo(act_date));
  check_timer->start(msecs);
  qDebug() << "fn_scheduler activation at:" << act_date;
}

bool fn_scheduler::is_active() const
{
  return check_timer->isActive();
}

void fn_scheduler::timeout()
{
  qDebug() << "fn_scheduler activated";
  emit activated();
}
