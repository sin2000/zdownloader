#ifndef ZIPPY_LINK_EXTRACTOR_H
#define ZIPPY_LINK_EXTRACTOR_H

#include <QObject>
#include <QString>
#include <memory>

class QScriptEngine;
class QScriptValue;
class QTimer;

class zippy_link_extractor : public QObject
{
  Q_OBJECT
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
  void check_file_existence(const QString & html);
  bool eval_javascript(const QString & html, int after_dl_button_pos);
  void schedule_abort_javascript_eval();
  void abort_javascript_eval();
  QString check_for_eval_errors(const QScriptValue & val) const;

  static std::unique_ptr<QScriptEngine> script_engine;
  QString source_link;
  QString dest_link;
  parse_error last_error;
  QTimer * abort_js_timer;
};

#endif // ZIPPY_LINK_EXTRACTOR_H
