#ifndef DEBUG_LOGGER_H
#define DEBUG_LOGGER_H

class QString;

class debug_logger
{
public:
  void log_msg(const QString & msg) const;
};

#endif // DEBUG_LOGGER_H
