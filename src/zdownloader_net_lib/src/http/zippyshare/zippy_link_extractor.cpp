#include "zippy_link_extractor.h"
#include <string_utils.h>
#include <QUrl>
#include <QFileInfo>
#include <QScriptEngine>
#include <QTimer>
#include <meta_object_ext.h>
#include <qdebugex.h>

std::unique_ptr<QScriptEngine> zippy_link_extractor::script_engine = nullptr;

zippy_link_extractor::zippy_link_extractor()
  :last_error(unknown_error),
   abort_js_timer(new QTimer(this))
{
  if(script_engine == nullptr)
  {
    script_engine = std::unique_ptr<QScriptEngine>(new QScriptEngine);
    script_engine->setProcessEventsInterval(5 * 1000);
  }

  abort_js_timer->setSingleShot(true);
  abort_js_timer->setInterval(15 * 1000);
  connect(abort_js_timer, &QTimer::timeout, this, &zippy_link_extractor::abort_javascript_eval);
}

const QString & zippy_link_extractor::get_extracted_link() const
{
  return dest_link;
}

QString zippy_link_extractor::get_extracted_filename() const
{
  return QFileInfo(QUrl(dest_link).path()).fileName();
}

zippy_link_extractor::parse_error zippy_link_extractor::get_last_error() const
{
  return last_error;
}

bool zippy_link_extractor::parse_html(const QString & html, const QString & src_link)
{
  last_error = unknown_error;

  source_link = src_link;

  const QString id = "id=\"dlbutton\"";

  int pos = html.indexOf(id);
  if(pos == -1)
  {
    check_file_existence(html);
    return false;
  }

  const int after_dlbutton_pos = pos + id.size();

  return eval_javascript(html, after_dlbutton_pos);
}

void zippy_link_extractor::check_file_existence(const QString & html)
{
  // "File does not exist on this server"; "File has expired and does not exist anymore on this server"
  const QString msg = "does not exist";
  if(html.contains(msg, Qt::CaseInsensitive))
    last_error = file_does_not_exists;
}

bool zippy_link_extractor::eval_javascript(const QString & html, int after_dl_button_pos)
{
  const QString tmp = html.mid(after_dl_button_pos);
  QString script = string_utils::pull_string(tmp, "<script type=\"text/javascript\">", "</script>");
  if(script.size() > 4096)
    return false;
  script.replace("document.getElementById('dlbutton')", "zdownloader_sin2000");
  script.replace("document.getElementById('fimage')", "ffk6f3l6ns9g834");
  script.prepend("var zdownloader_sin2000 = {}; var ffk6f3l6ns9g834 = {};");
  script += "zdownloader_sin2000.href;";

  meta_object_ext::invoke_async(this, &zippy_link_extractor::schedule_abort_javascript_eval);
  script_engine->pushContext();
  const QScriptValue val = script_engine->evaluate(script);
  script_engine->popContext();
  abort_js_timer->stop();
  if(check_for_eval_errors(val).isEmpty() == false)
    return false;

  const QString href = val.toString();
  if(href.isEmpty())
    return false;

  const QUrl url(source_link);
  const QUrl h(href);
  dest_link = url.resolved(h).toString(QUrl::None);

  return true;
}

void zippy_link_extractor::schedule_abort_javascript_eval()
{
  if(script_engine->isEvaluating())
    abort_js_timer->start();
}

void zippy_link_extractor::abort_javascript_eval()
{
  if(script_engine->isEvaluating())
  {
    script_engine->abortEvaluation();
    qDebug() << "WARNING: zippy_link_extractor - javascript eval timeout";
  }
}

QString zippy_link_extractor::check_for_eval_errors(const QScriptValue & val) const
{
  if(script_engine->hasUncaughtException())
  {
    const QString script_err = script_engine->uncaughtException().toString();
    const QString err_msg = QObject::tr("javascript_obj is invalid: %1").arg(script_err);
    return err_msg;
  }

  if(val.isValid() == false)
  {
    const QString err_msg = QObject::tr("javascript_obj is not valid");
    return err_msg;
  }

  return "";
}
