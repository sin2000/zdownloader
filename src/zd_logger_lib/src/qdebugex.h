#ifndef QDEBUGEX_H
#define QDEBUGEX_H

#include <QDebug>
#undef qDebug
#define qDebug QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC).debug().noquote

#endif // QDEBUGEX_H
