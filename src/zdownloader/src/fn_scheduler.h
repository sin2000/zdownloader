#ifndef FN_SCHEDULER_H
#define FN_SCHEDULER_H

#include <QObject>
#include <QDateTime>

class QTimer;

class fn_scheduler : public QObject
{
  Q_OBJECT
public:
  fn_scheduler(QObject * parent = nullptr);
  void set_activation_time(const QTime & time);
  void start();
  bool is_active() const;

signals:
  void activated();

private:
  void timeout();

  QDateTime act_date;
  QTimer * check_timer;
};

#endif // FN_SCHEDULER_H
