/*
csv.h/cpp
----------
Begin: 2014/04/11
Author (modified version): Aaron Reeves <aaron.reeves@naadsm.org>
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
#include <QVariant>
#include <QVariantList>

namespace CSV {
  QStringList parseLine( const QString& string );
  QList<QStringList> parseFromString(const QString &string);
  QList<QStringList> parseFromFile(const QString &filename, const QString &codec = "");
  QString writeLine( const QStringList& line );
  bool write(const QList<QStringList> data, const QString &filename, const QString &codec = "");
}


class qCSV {
  public:
    qCSV();
    qCSV(
      const QString& filename,
      const bool containsFieldList,
      const QChar& stringToken = '\0',
      const bool stringsContainCommas = true,
      const int readMode = qCSV::qCSV_ReadLineByLine
    );
    ~qCSV();

    void debug();

    // Accessor Members
    QString currentLine(){ clearError(); return _currentLine; }
    int currentLineNumber(){ return _currentLineNumber; }
    QString field ( int index );
    QString field ( QString fName );
    QString fieldName( int index );
    QVariantList fields( QString fName );
    QVariantList fields( int index );
    int error(){return _error;}
    QString errorMsg(){ return _errorMsg; }
    int columnCount(){ return _columnCount; }
    QStringList fieldData(){ return _fieldData; }
    int fieldCount(){ return _fieldNames.count(); }
    QStringList fieldNames(){return _fieldNames; }
    int rowCount();
    bool writeFile( const QString &filename, const QString &codec = "" );

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

    enum ReadModes {
      qCSV_ReadLineByLine,
      qCSV_ReadEntireFile
    };

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

    int _readMode;

    // Key is the field name.
    // Value is the position of the field in the file (i.e., the column number), starting from 0.
    QHash<QString, int> _fieldsLookup;

    // List of field names as they were in the original file.
    QStringList _fieldNames;

    // Data for the current row.
    QStringList _fieldData;

    // All rows of data, if an entire file has been read into memory.
    QList<QStringList> _data;

    void clearError();
    QStringList writeLine( const QStringList& line );
};

#endif // CSV_H

