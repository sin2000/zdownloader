#include "zippy_link_extractor.h"
#include <string_utils.h>
#include <QUrl>
#include <QFileInfo>

zippy_link_extractor::zippy_link_extractor()
  :last_error(unknown_error)
{
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

  QString js_link;
  if(find_js_value(html, "document.getElementById('dlbutton').href", &js_link, after_dlbutton_pos) == false)
    return false;

  const QString filename = find_filename_in_js(js_link);
  if(filename.isEmpty())
    return false;

  QString vd;
  int end_pos;

  if(algo_parse3(html, after_dlbutton_pos, filename))
    return true;

  if(algo_parse2(html, after_dlbutton_pos, filename) == false)
  {
    if(find_js_value(html, "d", &vd, after_dlbutton_pos, &end_pos) == false)
      return old_algo_parse(html, after_dlbutton_pos, js_link, filename);

    if(find_js_value(html, "d", &vd, end_pos) == false)
      return false;

    const QStringList vd_split = vd.split('%');
    if(vd_split.isEmpty())
      return false;

    bool conv_ok = false;
    int a = vd_split.at(0).toInt(&conv_ok);
    if(conv_ok == false)
      return false;

    const int b = a % 53;
    const int c = 8;
    const int d = a % 13;
    a %= 900;
    const int dyn = a * b + c + d;
    const QString dyn_part = QString::number(dyn);

    compose_dest_link(dyn_part, filename);
  }

  return true;
}

bool zippy_link_extractor::algo_parse2(const QString & html, int after_dlbutton_pos, const QString & filename)
{
  QString vb;
  int end_pos;
  if(find_js_value(html, "n", &vb, after_dlbutton_pos, &end_pos) == false)
    return false;

  if(find_js_value(html, "b", &vb, end_pos) == false)
    return false;

  bool conv_ok = false;
  int b = vb.toInt(&conv_ok);
  if(conv_ok == false)
    return false;

  b += 6;
  const QString dyn_part = QString::number(b) + "3";
  compose_dest_link(dyn_part, filename);

  return true;
}

bool zippy_link_extractor::algo_parse3(const QString & html, int after_dlbutton_pos, const QString & filename)
{
  QString va;
  int end_pos;
  if(find_js_value(html, "a", &va, after_dlbutton_pos, &end_pos) == false)
    return false;

  if(html.indexOf("\"asdasd\".", end_pos) == -1)
    return false;

  bool conv_ok = false;
  int a = va.toInt(&conv_ok);
  if(conv_ok == false)
    return false;

  a = a * a * a;
  a += 3;
  const QString dyn_part = QString::number(a);
  compose_dest_link(dyn_part, filename);

  return true;
}

bool zippy_link_extractor::old_algo_parse(const QString & html, int after_dlbutton_pos, const QString & js_link,
                                          const QString & filename)
{
  QString dyn_part;
  QString va;
  if(find_js_value(html, "a", &va, after_dlbutton_pos))
  {
    QString vb;
    if(find_js_value(html, "b", &vb, after_dlbutton_pos + va.size()) == false)
      return false;

    bool conv_ok = false;
    int a = va.toInt(&conv_ok);
    if(conv_ok == false)
      return false;

    int b = vb.toInt(&conv_ok);
    if(conv_ok == false)
      return false;

    const int dyn = (a / 3) + (a % b);
    dyn_part = QString::number(dyn);
  }
  else
  {
    const QString dyn_string = string_utils::pull_string(js_link, "(", ")");
    if(dyn_string.isEmpty())
       return false;

    dyn_part = parse_and_compute_dyn_string(dyn_string);
    if(dyn_part.isEmpty())
       return false;
  }

  compose_dest_link(dyn_part, filename);

  return true;
}

void zippy_link_extractor::check_file_existence(const QString & html)
{
  // "File does not exist on this server"; "File has expired and does not exist anymore on this server"
  const QString msg = "does not exist";
  if(html.contains(msg, Qt::CaseInsensitive))
    last_error = file_does_not_exists;
}

bool zippy_link_extractor::find_js_value(const QString & js, const QString & js_var_name, QString * value, int start_from,
                                         int * end_pos) const
{
  const QString var_prefix = js_var_name + " = ";

  int pos = js.indexOf(var_prefix, start_from);
  if(pos == -1)
    return false;

  int end = js.indexOf(';', pos + var_prefix.size());
  if(end == -1)
    return false;

  pos += var_prefix.size();

  *value = js.mid(pos, end - pos);
  if(end_pos)
    *end_pos = pos;

  return true;
}

QString zippy_link_extractor::find_filename_in_js(const QString & js) const
{
  QString filename;

  int end = js.lastIndexOf('"');
  if(end == -1)
    return "";

  --end;

  int start = js.lastIndexOf('/', end);
  if(start == -1)
    return "";

  filename = js.mid(start + 1, end - start);

  return filename;
}

QString zippy_link_extractor::parse_and_compute_dyn_string(const QString & dyn_string) const
{
  const QStringList list = dyn_string.split(' ', QString::SkipEmptyParts);
  if(list.size() != 7)
    return "";

  bool conv_ok = false;

  const int a = list.at(0).toInt(&conv_ok);
  if(conv_ok == false)
    return "";

  const int b = list.at(2).toInt(&conv_ok);
  if(conv_ok == false)
    return "";

  const int c = list.at(4).toInt(&conv_ok);
  if(conv_ok == false)
    return "";

  const int d = list.at(6).toInt(&conv_ok);
  if(conv_ok == false)
    return "";

  const int res = (a % b) + (c % d);

  return QString::number(res);
}

void zippy_link_extractor::compose_dest_link(const QString & dynamic_part, const QString & filename)
{
  QUrl url(source_link);
  QString src_path_prefix = QFileInfo(url.path()).path();
  if(src_path_prefix.size() >= 2)
    src_path_prefix[1] = 'd';

  QUrl relative(src_path_prefix + "/" + dynamic_part + "/" + filename);

  dest_link = url.resolved(relative).toString(QUrl::None);
}
