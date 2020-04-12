#ifndef TEXT_LOGGER_MANAGER_H
#define TEXT_LOGGER_MANAGER_H

#include "text_file_logger.h"
#include <name_to_object_binder.h>

class ZD_LOGGERSHARED_EXPORT text_logger_manager : public name_to_object_binder<text_file_logger>
{
public:
  text_logger_manager(QThread * thread);
};

#endif // TEXT_LOGGER_MANAGER_H
