#ifndef DOWNLOADABLE_ITEMS_H
#define DOWNLOADABLE_ITEMS_H

#include <QQueue>
#include <QMap>
#include <QHash>

class download_item;
class QFile;
class text_file_logger;
class finished_downloads_settings;

class downloadable_items
{
public:
  downloadable_items();
  ~downloadable_items();

  void load_state(const finished_downloads_settings & fin_set);

  void sort_and_add(const QList<download_item> & items);
  void clear();
  bool has_pending_items() const;
  bool has_working_items() const;
  download_item * get_next();
  void pause(download_item * item);
  void resume_first_paused();
  QString finish(download_item * item); // returns filename if item is last in group
  void delete_item(download_item * item);
  void stop(const download_item * item);
  bool contains_link(const QString & link) const;
  QList<const download_item *> get_segmented_items() const;
  void set_item_segment_ends_by_filename(const QString & filename, const QVector<qint64> & seg_ends, qint64 remaining_bytes);
  void set_download_lists_directory(const QString & dir_path);
  qint64 get_sum_of_remaining_bytes() const;
  int get_all_items_count() const;

private:
  void load_from_file();
  void save_to_file();

  void add(const download_item & item);
  void parse_file_content(const QString & content, QList<download_item> * parsed_content) const;
  void load_parsed_content(const QList<download_item> & parsed_content);
  bool remove_working_item(const download_item * item);

  QString download_lists_directory;
  const QString pending_download_list_filename = "download_list.txt";
  QString download_list_file_path;
  QFile * save_file;
  const QString finished_downloads_base_fn = "finished_downloads";
  text_file_logger * fin_dls_logger;

  QQueue<const download_item *> pending_items;
  QList<download_item *> working_items;
  QList<const download_item *> paused_items;
  QList<const download_item *> all_items;

  QMap<QString, download_item *> filename_map;
  QHash<QString, const download_item *> link_map;

  // key: group_id; value: how many items left in group before unpack
  QMap<int, int> items_left_in_group_map;

  int max_group_id;
};

#endif // DOWNLOADABLE_ITEMS_H
