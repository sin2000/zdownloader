#ifndef NAME_TO_OBJECT_BINDER_H
#define NAME_TO_OBJECT_BINDER_H

#include <QHash>
#include <QThread>

template<class T>
class name_to_object_binder
{
public:
  typedef QHash<QString, T *> obj_by_name_type;

  name_to_object_binder(QThread * destination_thread);
  virtual ~name_to_object_binder();

  // if name exists returns existing object
  T * add_object(const QString & name);
  void remove_object(const QString & name);
  T * get_by_name(const QString & name) const;
  const obj_by_name_type & get_object_map() const;

private:
  // name -> object
  obj_by_name_type obj_by_name_map;
  QThread * dest_thread;
};

template<class T>
name_to_object_binder<T>::name_to_object_binder(QThread * destination_thread)
  :dest_thread(destination_thread)
{
}

template<class T>
name_to_object_binder<T>::~name_to_object_binder()
{
  qDeleteAll(obj_by_name_map);
}

template<class T>
T * name_to_object_binder<T>::add_object(const QString & name)
{
  T * obj = get_by_name(name);
  if(obj == nullptr)
  {
    obj = new T(name);
    obj_by_name_map.insert(name, obj);
    obj->moveToThread(dest_thread);
  }

  return obj;
}

template<class T>
void name_to_object_binder<T>::remove_object(const QString & name)
{
  const auto it = obj_by_name_map.find(name);
  if(it == obj_by_name_map.end())
    return;

  auto * obj = dynamic_cast<QObject *>(it.value());
  if(obj)
    obj->deleteLater();

  obj_by_name_map.remove(name);
}

template<class T>
T * name_to_object_binder<T>::get_by_name(const QString & name) const
{
  const auto it = obj_by_name_map.find(name);
  if(it == obj_by_name_map.end())
    return nullptr;

  return it.value();
}

template<class T>
const typename name_to_object_binder<T>::obj_by_name_type & name_to_object_binder<T>::get_object_map() const
{
  return obj_by_name_map;
}

#endif // NAME_TO_OBJECT_BINDER_H
