#ifndef META_OBJECT_EXT_H
#define META_OBJECT_EXT_H

#include "zd_shared_lib_global.h"
#include <functional>
#include <QEvent>
#include <QCoreApplication>
#include <QUrlQuery>

namespace detail {
template <typename F>
struct FEvent : QEvent
{
  using Fun = typename std::decay<F>::type;
  const QObject * const obj;
  const QMetaObject * const type = obj->metaObject();
  Fun fun;

  template <typename Fun>
  FEvent(const QObject * object, Fun && func)
    :QEvent(QEvent::None),
     obj(object),
     fun(std::forward<Fun>(func))
  {
  }

  ~FEvent()
  {
    if(obj->metaObject()->inherits(type)) // ensure that the object is not being destructed
      fun();
  }
};
}

class ZD_SHARED_LIB_EXPORT meta_object_ext
{
public:
  // obj must have Q_GADGET inside
  static QUrlQuery custobj_to_url_query(const QMetaObject & meta_obj, const void * obj);

  template <typename F>
  static void post_to_object(F && fun, QObject * obj)
  {
     QCoreApplication::postEvent(obj, new detail::FEvent<F>(obj, std::forward<F>(fun)));
  }

  // use example: meta_object_ext::invoke_async(this, &scp_user_guest::set_post_body, input);
  template<typename T, typename R, typename... Params, typename... Args>
  static void invoke_async(T * object, R (T::*function)(Params...), Args &&... args)
  {
    post_to_object(std::bind(function, object, std::forward<Args>(args)...), object);
  }
};

#endif // META_OBJECT_EXT_H
