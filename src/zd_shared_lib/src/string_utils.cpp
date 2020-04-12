#include "string_utils.h"

string_utils::string_utils()
{
}

int string_utils::find_first_of(const QSet<QChar> & chars, const QString & text, int from)
{
  const int len = text.length();
  while(from < len)
  {
    if(chars.contains(text.at(from)))
      return from;
    ++from;
  }

  return -1;
}

QString string_utils::pull_string(const QString & src, const QString & start, const QString & end)
{
  const int start_size = start.size();

  int startp = src.indexOf(start);
  if(startp == -1)
    return "";

  if(end.isEmpty() == false)
  {
    int endp = src.indexOf(end, startp + start_size);
    if(endp == -1)
      return "";

    return src.mid(startp + start_size, endp - startp - start_size);
  }

  return src.mid(startp + start_size);
}

QString string_utils::seconds_to_string(qint64 seconds)
{
  qint64 minutes = seconds / 60;
  qint64 hours = 0;
  qint64 days = 0;
  if(minutes)
  {
    seconds %= 60;
    hours = minutes / 60;
    if(hours)
    {
      minutes %= 60;
      days = hours / 24;
      if(days)
        hours %= 24;
    }
  }

  QString ret;
  if(days)
  {
    ret = QString("%1d%2h%3m").arg(QString::number(days), QString::number(hours),
                                   QString::number(minutes));
  }
  else
  {
    if(hours)
    {
      ret = QString("%1h%2m").arg(QString::number(hours), QString::number(minutes));
    }
    else
    {
      if(minutes)
        ret = QString("%1m").arg(QString::number(minutes));
    }
  }

  ret += QString("%1s").arg(QString::number(seconds));

  return ret;
}
