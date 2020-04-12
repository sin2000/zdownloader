#include "text_logger_manager.h"

text_logger_manager::text_logger_manager(QThread * thread)
  :name_to_object_binder(thread)
{
}
