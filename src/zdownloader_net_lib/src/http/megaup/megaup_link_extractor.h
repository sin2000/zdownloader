#ifndef MEGAUP_LINK_EXTRACTOR_H
#define MEGAUP_LINK_EXTRACTOR_H

#include <QString>

class megaup_link_extractor
{
public:
  enum parse_error {
    file_does_not_exists,
    extracted_link_invalid,
    unknown_error
  };

  megaup_link_extractor();

  const QString & get_extracted_link() const;
  parse_error get_last_error() const;
  bool parse_html(const QString & html);

private:
  bool validate_link();

  QString dest_link;
  parse_error last_error;
};

#endif // MEGAUP_LINK_EXTRACTOR_H
