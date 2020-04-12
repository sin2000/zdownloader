#include "meta_object_ext.h"
#include <QMetaProperty>

QUrlQuery meta_object_ext::custobj_to_url_query(const QMetaObject & meta_obj, const void * obj)
{
  QUrlQuery url_query;

  const int prop_count = meta_obj.propertyCount();
  for(int i = 0; i < prop_count; ++i)
  {
    const QMetaProperty prop = meta_obj.property(i);
    const char * name = prop.name();
    QVariant val = prop.readOnGadget(obj);
    if(val.isValid())
    {
      switch(val.type())
      {
        case QVariant::Int:
          if(val.toInt() == -1)
            continue;
          break;

        case QVariant::String:
          if(val.toString().isNull())
            continue;
          break;

        default:
          break;
      }

      url_query.addQueryItem(QString::fromLatin1(name), val.toString());
    }
  }

  return url_query;
}
