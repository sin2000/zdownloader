#ifndef QT_COMPAT_H
#define QT_COMPAT_H

#include <QtGlobal>
#include <QTextStream>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))

  #include <Qt>

  namespace qt_compat {
    constexpr auto split_skip_empty_parts = Qt::SkipEmptyParts;
    constexpr auto split_keep_empty_parts = Qt::KeepEmptyParts;
    constexpr auto endl = Qt::endl;
  }

#else

  #include <QString>

  namespace qt_compat {
    constexpr QString::SplitBehavior split_skip_empty_parts = QString::SkipEmptyParts;
    constexpr QString::SplitBehavior split_keep_empty_parts = QString::KeepEmptyParts;
    constexpr auto endl = ::endl;
  }

#endif

#endif // QT_COMPAT_H
