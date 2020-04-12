#include "megaup_link_extractor.h"
#include <string_utils.h>
#include <QFileInfo>
#include <QUrl>

megaup_link_extractor::megaup_link_extractor()
  :last_error(unknown_error)
{
}

const QString & megaup_link_extractor::get_extracted_link() const
{
  return dest_link;
}

megaup_link_extractor::parse_error megaup_link_extractor::get_last_error() const
{
  return last_error;
}

bool megaup_link_extractor::parse_html(const QString & html)
{
  last_error = unknown_error;

  QString middle_link = string_utils::pull_string(html, "('.download-timer').html", "'>");
  if(middle_link.isEmpty())
    return false;

  middle_link = string_utils::pull_string(middle_link, "href='");

  if(middle_link.isEmpty())
    return false;

  dest_link = middle_link;

  return validate_link();
}

bool megaup_link_extractor::validate_link()
{
  const QUrl url(dest_link, QUrl::StrictMode);

  if(url.isValid() && url.scheme() == "https" && url.host().endsWith("megaup.net"))
    return true;

  last_error = extracted_link_invalid;
  return false;
}
