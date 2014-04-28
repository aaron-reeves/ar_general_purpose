/*
csv.h/cpp
----------
Begin: 2014/04/11
Author (modified version): Aaron Reeves <development@reevesdigital.com>
------------------------------------------------------------------------

Original code by Naohiro Hasegawa
From https://github.com/hnaohiro/qt-csv/blob/master/csv.cpp
*/

#include "csv.h"

#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QRegExp>
#include <QDebug>

QStringList CSV::parseLine( const QString& string ) {
  enum State {Normal, Quote} state = Normal;
  QStringList line;
  QString temp, value;

  temp = string.trimmed();

  for (int i = 0; i < temp.size(); i++) {
    QChar current = temp.at(i);

    // Normal state
    if (state == Normal) {
      // comma encountered
      if (current == ',') {
        // add line
        line.append(value.trimmed());
        value.clear();
      }
      // One quote mark encountered.  Ignore commas until the matching quote mark is encountered.
      else if (current == '"') {
        state = Quote;
      }
      // other character encountered
      else {
        value += current;
      }
    }
    // Quote
    else if (state == Quote) {
      // double quote
      if (current == '"') {
        int index = (i+1 < temp.size()) ? i+1 : temp.size();
        QChar next = temp.at(index);
        if (next == '"') {
          value += '"';
          i++;
        } else {
          state = Normal;
        }
      }
      // other
      else {
        value += current;
      }
    }
  }

  line.append(value.trimmed());

  return line;
}

QList<QStringList> parse(const QString &string){
  enum State {Normal, Quote} state = Normal;
  QList<QStringList> data;
  QStringList line;
  QString value;

  for (int i = 0; i < string.size(); i++) {
    QChar current = string.at(i);

    // Normal
    if (state == Normal) {
      // newline
      if (current == '\n') {
        // add value
        line.append(value.trimmed());
        value.clear();
        // add line
        data.append(line);
        line.clear();
      }
      // comma
      else if (current == ',') {
        // add line
        line.append(value);
        value.clear();
      }
      // double quote
      else if (current == '"') {
        state = Quote;
      }
      // character
      else {
        value += current;
      }
    }
    // Quote
    else if (state == Quote) {
      // double quote
      if (current == '"') {
        int index = (i+1 < string.size()) ? i+1 : string.size();
        QChar next = string.at(index);
        if (next == '"') {
          value += '"';
          i++;
        } else {
          state = Normal;
        }
      }
      // other
      else {
        value += current;
      }
    }
  }

  return data;
}


QString initString(const QString &string){
  QString result = string;
  result.replace("\r\n", "\n");
  if (result.at(result.size()-1) != '\n') {
    result += '\n';
  }
  return result;
}


QList<QStringList> CSV::parseFromString(const QString &string){
  return parse(initString(string));
}


QList<QStringList> CSV::parseFromFile(const QString &filename, const QString &codec){
  QString string;
  QFile file(filename);
  if (file.open(QIODevice::ReadOnly)) {
    QTextStream in(&file);
    if( !codec.isEmpty() )
      in.setCodec(QTextCodec::codecForName(codec.toLatin1()));
    string = in.readAll();
    file.close();
  }
  return parse(initString(string));
}


bool CSV::write(const QList<QStringList> data, const QString &filename, const QString &codec){
  QFile file(filename);
  if (!file.open(QIODevice::WriteOnly)) {
    return false;
  }

  QTextStream out(&file);
  if( !codec.isEmpty() )
    out.setCodec(codec.toLatin1());

  foreach (const QStringList &line, data) {
    QStringList output;
    foreach (QString value, line) {
      if (value.contains(QRegExp(",|\r\n"))) {
        output << ("\"" + value + "\"");
      } else if (value.contains("\"")) {
        output << value.replace("\"", "\"\"");
      } else {
        output << value;
      }
    }
    out << output.join(",") << "\r\n";
  }

  file.close();

  return true;
}
