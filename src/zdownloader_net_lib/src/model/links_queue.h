#ifndef LINKS_QUEUE_H
#define LINKS_QUEUE_H

#include <QQueue>
#include <QSet>

class links_queue
{
public:
  void add(const QString & item);
  void add(const QList<QString> & items);
  void clear();
  bool has_pending_items() const;
  bool has_working_items() const;
  const QString & get_next();
  void finish(const QString & item);
  void stop(const QString & item);
  bool contains_link(const QString & item) const;

private:
  bool remove_working_item(const QString & item);

  QQueue<QString> pending_items;
  QList<QString> working_items;

  QSet<QString> link_map;
};

#endif // LINKS_QUEUE_H
