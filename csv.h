/*
csv.h/cpp
----------
Begin: 2014/04/11
Author (modified version): Aaron Reeves <development@reevesdigital.com>
------------------------------------------------------------------------

Original code (name space CSV) by Naohiro Hasegawa, https://github.com/hnaohiro/qt-csv/blob/master/csv.h
Original code (class qCSV) believed to be by Shaun Case, Animal Population Health Institute, Colorado State University.
*/

#ifndef CSV_H
#define CSV_H

#include <QFile>
#include <QMap>
#include <QString>
#include <QStringList>

namespace CSV {
  QStringList parseLine( const QString& string );
  QList<QStringList> parseFromString(const QString &string);
  QList<QStringList> parseFromFile(const QString &filename, const QString &codec = "");
  bool write(const QList<QStringList> data, const QString &filename, const QString &codec = "");
};

class qCSV {
  public:
    qCSV();
    qCSV( const QString& filename, const bool containsFieldList, const QChar& stringToken = '\0', const bool stringsContainCommas = true );
    ~qCSV();

    // Accessor Members
    QString currentLine(){clearError(); return _currentLine;}
    QString field ( int index );
    QString field ( QString fName );
    QString fieldName( int index );
    int error(){return _error;}
    QString errorMsg(){return _errorMsg;}
    int setColumnCount(){ return _columnCount; }
    QMap< int, QString > fieldData(){ return _fieldData; }
    int fieldCount(){ return _fields.size();}
    QList<QString> fieldNames(){return _fields.keys();}

    // Mutator Members
    void setContainsFieldList ( bool setVal ); //  if True line one of the file contains a list of field names
    void setFilename ( QString filename );
    bool open();
    bool close();
    int moveNext();
    void setStringToken ( QChar token );
    void setColumnCount( int set_val ){ _columnCount = set_val;}
    void setStringsContainCommas( bool set_val ){ _stringsContainCommas = set_val;}
    void setConcatenateDanglingEnds( bool set_val ){ _concatenateDanglingEnds = set_val; }

    enum CSVErrorMessages{
      qCSV_ERROR_NONE,
      qCSV_ERROR_OPEN,
      qCSV_ERROR_CLOSE,
      qCSV_ERROR_LINE_EMPTY,
      qCSV_ERROR_NO_FIELDLIST,
      qCSV_ERROR_INVALID_FIELD_NAME,
      qCSV_ERROR_INDEX_OUT_OF_RANGE,
      qCSV_ERROR_BAD_READ,
      qCSV_ERROR_INVALID_FIELD_COUNT
    };

  protected:
    void initialize();

    QString   _srcFilename;
    QFile     _srcFile;
    QString   _currentLine;
    int       _currentLineNumber;
    int       _error;
    QString   _errorMsg;
    QChar     _stringToken;
    bool      _usesStringToken;
    bool      _containsFieldList;
    int       _columnCount;
    bool      _stringsContainCommas;
    bool      _concatenateDanglingEnds;

    QMap< QString, int > _fields;
    QMap< int, QString > _fieldData;

    void clearError();
};

#endif // CSV_H

