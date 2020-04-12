#include "debug_logger.h"
#include <QString>
#include <QTextStream>

void debug_logger::log_msg(const QString & msg) const
{
  static QTextStream qts(stdout);

  qts << msg << endl;
}
