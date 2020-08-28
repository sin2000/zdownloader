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

  const QString script = prepare_script_from_html(html);
  if(script.isEmpty())
    return false;

  return eval_javascript(script);
}

void zippy_link_extractor::check_file_existence(const QString & html)
{
  // "File does not exist on this server"; "File has expired and does not exist anymore on this server"
  const QString msg = "does not exist";
  if(html.contains(msg, Qt::CaseInsensitive))
    last_error = file_does_not_exists;
}

QString zippy_link_extractor::prepare_script_from_html(const QString & html)
{
  QString script = string_utils::pull_string(html, "<script type=\"text/javascript\">", "</script>");
  if(script.size() > 4096)
    return "";

  QString src_prepend =
      R"(
        var document = {};
        document.getElementById = function(p) {
          if (!this.hasOwnProperty(p)) { this[p] = { getAttribute : function(p) { return this[p] } } }
          return this[p];
        };
      )";

  const QStringList js_method_args = js_get_chained_methods_args(script, { "getElementById", "getAttribute" });
  for(int i = 0; i < js_method_args.size(); i += 2)
  {
    const QString & id_name = js_method_args.at(0);
    const QString & attr_name = js_method_args.at(1);
    const QString attr_val = html_get_attribute_val(html, id_name, attr_name);
    src_prepend += "document.getElementById('" + id_name + "')['" + attr_name + "'] = '" + attr_val + "';";
  }

  script.prepend(src_prepend);
  script += "document.dlbutton.href;";

  return script;
}

bool zippy_link_extractor::eval_javascript(const QString & script)
{
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

QStringList zippy_link_extractor::js_get_chained_methods_args(const QString & js_src, const QStringList & methods)
{
  const QString script = string_utils::remove_all_whitespace(js_src);
  QStringList args;
  QStringRef js_fragment = &script;
  while(js_fragment.isEmpty() == false)
  {
    args.append(js_find_next_chained_methods_args(js_fragment, methods, &js_fragment));
  }

  if(args.size() % methods.size())
    return QStringList();

  return args;
}

QStringList zippy_link_extractor::js_find_next_chained_methods_args(const QStringRef & src, const QStringList & methods, QStringRef * js_fragment)
{
  QStringList ret;
  QStringRef ref = src;
  const int methods_size = methods.size();
  bool not_found = false;
  for(int i = 0; i < methods_size; ++i)
  {
    const QString curr_method = '.' + methods[i] + '(';
    int pos = -1;
    if(i == 0)
      pos = ref.indexOf(curr_method);
    else
      pos = ref.startsWith(curr_method) ? 0 : -1;

    if(pos == -1)
    {
      if(i == 0)
        ref.clear();
      not_found = true;
      break;
    }

    ref = ref.mid(pos + curr_method.size());
    // 'a') - minimal
    if((ref.size() < 4) || (ref.at(0) != '"' && ref.at(0) != '\''))
    {
      not_found = true;
      break;
    }

    const auto delim_char = ref.at(0);
    ref = ref.mid(1);
    pos = ref.indexOf(delim_char);
    if(pos == -1)
    {
      not_found = true;
      break;
    }

    const QString val = ref.left(pos).toString();
    ref = ref.mid(pos + 1);
    if(ref.isEmpty() || ref.at(0) != ')' || val.isEmpty())
    {
      not_found = true;
      break;
    }

    ret.append(val);

    ref = ref.mid(1);
  }

  *js_fragment = ref;
  if(not_found)
    return QStringList();

  return ret;
}

QString zippy_link_extractor::html_get_attribute_val(const QString & html, const QString & id_name, const QString & attr_name)
{
  const QString id = "id=\"" + id_name + "\"";

  int pos = html.indexOf(id);
  if(pos == -1)
    return "";

  pos = html.lastIndexOf('<', pos);
  const int end = html.indexOf('>', pos + id.size());
  if(pos == -1 || end == -1)
    return "";

  QStringRef ref = html.midRef(pos, end - pos);

  const QString attr = attr_name + "=\"";
  pos = ref.indexOf(attr);
  if(pos == -1)
    return "";

  ref = ref.mid(pos + attr.size());
  pos = ref.indexOf('"');
  if(pos == -1)
    return "";

  const QString val = ref.left(pos).toString();

  return val;
}
