/*
csv.h/cpp
---------
Begin: 2014/04/11
Author (modified version): Aaron Reeves <aaron.reeves@naadsm.org>
------------------------------------------------------------------

Original code (name space CSV) by Naohiro Hasegawa, https://github.com/hnaohiro/qt-csv/blob/master/csv.h
Original code (class qCSV) by Shaun Case, Animal Population Health Institute, Colorado State University.
*/

#ifndef CSV_H
#define CSV_H

#include <QtCore>

namespace CSV {
  QStringList parseLine( const QString& string, const QChar delimiter = ',' );
  QList<QStringList> parseFromString(const QString &string, const QChar delimiter = ',' );
  QList<QStringList> parseFromFile(const QString &filename, const QChar delimiter = ',', const QString &codec = "" );
  QString writeLine( const QStringList& line, const QChar delimiter = ',' );
  bool write(const QList<QStringList> data, const QString &filename, const QChar delimiter = ',', const QString &codec = "" );
}


class qCSV : public QObject {
  Q_OBJECT

  public:
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

    qCSV();
    qCSV(
      const QString& filename,
      const bool containsFieldList,
      const QChar& stringToken = '\0',
      const bool stringsContainCommas = true,
      const int readMode = qCSV::qCSV_ReadLineByLine,
      const bool checkForComment = false
    );
    virtual ~qCSV();

    void debug();

    // Accessor Members
    QString currentLine(){ clearError(); return _currentLine; }
    int currentLineNumber(){ return _currentLineNumber; }
    QString field( int index );
    QString field( QString fName );
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
    int nCommentRows(){ return _nCommentRows; }

    void setField( const int index, const QString& val );
    void setField( QString fName, const QString& val );

    // Mutator Members
    void setContainsFieldList ( bool setVal ); //  if True line one of the file contains a list of field names
    void setFilename ( QString filename );
    bool open();
    bool close();
    int moveNext();
    void setStringToken ( QChar token );
    void setColumnCount( int set_val ){ _columnCount = set_val;}
    void setStringsContainCommas( bool set_val ){ _stringsContainDelimiters = set_val;}
    void setConcatenateDanglingEnds( bool set_val ){ _concatenateDanglingEnds = set_val; }
    void setEolDelimiter( const QString& val ) { _eolDelimiter = val; }
    void setDelimiter( const QChar val ) { _delimiter = val; }

  signals:
    void nBytesRead( const int val );

  protected:
    void initialize();

    QString   _srcFilename;
    QFile     _srcFile;
    QString   _currentLine;
    int       _currentLineNumber;
    bool      _firstDataRowEncountered;
    int       _error;
    QString   _errorMsg;
    QChar     _stringToken;
    bool      _usesStringToken;
    bool      _containsFieldList;
    int       _columnCount;
    bool      _stringsContainDelimiters;
    bool      _concatenateDanglingEnds;
    QString   _eolDelimiter;
    QChar     _delimiter;
    bool      _checkForComment;
    int       _nCommentRows;

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

    bool isCommentLine( const QString& line );
};

#endif // CSV_H

