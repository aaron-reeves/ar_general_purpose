/*
Original code by Naohiro Hasegawa
From https://github.com/hnaohiro/qt-csv/blob/master/csv.h
*/

#ifndef CSV_H
#define CSV_H

#include <QStringList>

namespace CSV {
  QStringList parseLine( const QString& string );
  QList<QStringList> parseFromString(const QString &string);
  QList<QStringList> parseFromFile(const QString &filename, const QString &codec = "");
  bool write(const QList<QStringList> data, const QString &filename, const QString &codec = "");
};

#endif // CSV_H

