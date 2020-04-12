#ifndef ZIPPY_LINK_EXTRACTOR_H
#define ZIPPY_LINK_EXTRACTOR_H

#include <QString>

class zippy_link_extractor
{
public:
  enum parse_error {
    file_does_not_exists,
    unknown_error
  };

  zippy_link_extractor();

  const QString & get_extracted_link() const;
  QString get_extracted_filename() const;
  parse_error get_last_error() const;
  bool parse_html(const QString & html, const QString & src_link);

private:
  bool algo_parse2(const QString & html, int after_dlbutton_pos, const QString & filename);
  bool algo_parse3(const QString & html, int after_dlbutton_pos, const QString & filename);
  bool old_algo_parse(const QString & html, int after_dlbutton_pos, const QString & js_link,
                      const QString & filename);
  void check_file_existence(const QString & html);
  bool find_js_value(const QString & js, const QString & js_var_name, QString * value, int start_from = 0,
                     int * end_pos = nullptr) const;
  QString find_filename_in_js(const QString & js) const;
  QString parse_and_compute_dyn_string(const QString & dyn_string) const;
  void compose_dest_link(const QString & dynamic_part, const QString & filename);

  QString source_link;
  QString dest_link;
  parse_error last_error;
};

#endif // ZIPPY_LINK_EXTRACTOR_H
