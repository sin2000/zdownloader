#include "links_queue.h"
#include <qdebugex.h>

void links_queue::add(const QString & item)
{
  if(link_map.contains(item))
  {
    qDebug() << "Item not added. Link" << item << "already exists in link_map.";
    return;
  }

  pending_items.enqueue(item);
  link_map.insert(item);
}

void links_queue::add(const QList<QString> & items)
{
  for(const auto & item : items)
  {
    add(item);
  }
}

void links_queue::clear()
{
  pending_items.clear();
  working_items.clear();
  link_map.clear();
}

bool links_queue::has_pending_items() const
{
  return pending_items.isEmpty() == false;
}

bool links_queue::has_working_items() const
{
  return working_items.isEmpty() == false;
}

const QString & links_queue::get_next()
{
  const QString item = pending_items.dequeue();
  working_items.append(item);

  return working_items.last();
}

void links_queue::finish(const QString & item)
{
  remove_working_item(item);
}

void links_queue::stop(const QString & item)
{
  if(remove_working_item(item))
  {
    pending_items.prepend(item);
  }
}

bool links_queue::contains_link(const QString & item) const
{
  return link_map.contains(item);
}

bool links_queue::remove_working_item(const QString & item)
{
  const bool removed = working_items.removeOne(item);
  if(removed == false)
  {
    qDebug() << "Could not remove item:" << item;
    return false;
  }

  return true;
}
