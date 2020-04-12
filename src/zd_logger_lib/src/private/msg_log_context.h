#ifndef MSG_LOG_CONTEXT_H
#define MSG_LOG_CONTEXT_H

#include <QMetaType>
#include <QString>

class msg_log_context
{
public:
  qint64 timestamp_msecs; // since epoch
  QtMsgType msg_type;
  QString category;
  QString file;
  QString function;
  int line;
  int version;
  QString msg; // text of message
};

//Q_DECLARE_METATYPE(msg_log_context)

#endif // MSG_LOG_CONTEXT_H
