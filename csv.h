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
#include <ar_general_purpose/strutils.h>

/*
 * Basic use:
 * ==========
 *  QCsv csv(
 *    csvFileName,      // const QString& filename,
 *    true,             // const bool containsFieldList,
 *    true,             //const bool stringsContainCommas = true,
 *    QCsv::LineByLine, // const int mode = qCSV::LineByLine,
 *    false             // const bool checkForComment = false
 *  );
 *
 *  if( !csv.open() ) {
 *    error();
 *    return false;
 *  }
 *
 *  while( -1 != csv.moveNext() ) {
 *    doStuff();
 *  }
 *
 *  csv.close();
 *
 *  return true;
 */

namespace CSV {
  enum StringCase {
    OriginalCase,
    TitleCase,
    UpperCase,
    LowerCase
  };

  // Parses a single CSV line into its component parts
  QStringList parseLine( const QString& string, const QChar delimiter = ',' );

  // Parses a multi-line CSV string into its component parts
  QList<QStringList> parseFromString(const QString &string, const QChar delimiter = ',' );

  // Parses a CSV file
  QList<QStringList> parseFromFile(const QString &filename, const QChar delimiter = ',', const QString &codec = QString() );

  // Generates a string list containing properly formatted CSV elements
  QStringList csvStringList( const QStringList& elements, const QChar delimiter = ',', const StringCase stringCase = OriginalCase );

  // Writes a properly formatted CSV line from its component parts
  QString writeLine( const QStringList& elements, const QChar delimiter = ',', const StringCase stringCase = OriginalCase );

  // Writes a properly formatted multi-line CSV string from its component parts
  bool write(const QList<QStringList>& data, const QString &filename, const QChar delimiter = ',', const QString& codec = QString() );
}


/* A class for reading, processing, and manipulating CSV-formatted data,
 * or other data that is similarly delimited.
 * This class will work with files or with multi-line strings, properly parsing
 * CSV data into its component parts.
 * It can also be used to construct a CSV-formatted data set from scratch.
 */
class QCsv {
  public:
    /* CSV-formatted files may be read in one of two ways:
     *  - Reading one line at a time from a file, which may be more efficient
     *    for processing very large data files for some applications.  This is because
     *    a line of data is read and can then be processed, but is not stored as part of the
     *    CSV object itself.  Access to data, then, is restricted to a single line at a time.
     *  - Reading (or generating) the contents of an entire file all at once, which
     *    may be more flexible.  All data is stored as part of the CSV object, which consumes more
     *    memory, but all data is available at any time.
     *
     * The values in this enum indicate which of these two modes is being used.  The
     * The third mode, qCSV_UnspecifiedMode, is a sometimes used as a default setting
     * which must be changed before any real work can be carried out.
     */
    // FIXME: Do more with modes, to ensure that the object is in the right mode before and
    // after the various function calls.
    enum QCsvMode {
      UnspecifiedMode,
      LineByLine,
      EntireFile
    };

    // Values in this enum are used to indicate error conditions.
    enum CSVErrorCode {
      ERROR_NONE,
      ERROR_OPEN,
      ERROR_LINE_EMPTY,
      ERROR_NO_FIELDLIST,
      ERROR_INVALID_FIELD_NAME,
      ERROR_INDEX_OUT_OF_RANGE,
      ERROR_BAD_READ,
      ERROR_INVALID_FIELD_COUNT,
      ERROR_WRONG_MODE,
      ERROR_OTHER
    };

    enum ColumnFormat {
      DateFormat,
      TimeFormat,
      DateTimeFormat
    };

    QCsv(); // Constructs an empty CSV object with an unspecified mode.  Use properties below to specify settings.

    // Constructs a CSV object from a file, with the indicated properties.
    // For all other property values, reasonable defaults are used.
    // Most property values (see below) need to be specified before open() is called.
    QCsv(
      const QString& filename,
      const bool containsFieldList,
      const bool stringsContainDelimiters = true,
      const QCsvMode mode = QCsv::LineByLine,
      const bool checkForComments = false
    );

    // These constructors are used to build a CSV data set from scratch.
    // All will construct a CSV object in qCSV_EntireFile mode.
    QCsv( const QStringList& fieldNames ); // Column names are specified, but no data is provided.
    QCsv( const QStringList& fieldNames, const QList<QStringList>& data ); // Column names and data are provided.
    QCsv( const QStringList& fieldNames, const QStringList& data ); // Column names and a single row of data are provided.
    QCsv( const QList<QStringList>& data ); // Data only without field names.

    QCsv( const QCsv& other ); // Creates a copy of an existing CSV object.
    QCsv& operator=( const QCsv& other ); // Assignment operator

    virtual ~QCsv(); // Destroys the object.

    // Parses a CSV-formatted multi-line string, breaking it into its component parts
    // which can then be accessed via the various qCSV functions.
    // Don't forget that for all other property values, reasonable defaults are used.
    // Also, most property values (see below) need to be specified before open() is called.
    void processString(
        const QString& text,
        const bool containsFieldList,
        const bool stringsContainDelimiters
    );

    // Opens the file/object for reading/manipulation.  Returns false and sets an error flag if open failed.
    // This isn't strictly necessary for mode EntireFile, as it happens implicitly (at least when the main version of the constructor is used).
    // It's a good habit to get into, however, as explicitly opening a LineByLine file is required.
    virtual bool open();
    void close(); // Closes an open file.
    bool toFront(); // Resets to the top of the file, so that moveNext() will return the first row of data.
    int moveNext();  // Moves to the next row of data.  Returns the number of fields encountered, or -1 if the row is empty or does not exist.

    int fieldCount() const; // The number of fields/columns in the CSV object
    int rowCount(); // The number of rows in the CSV object.  Not available in line-by-line mode.
    int rowCount() const;

    // The index of the current row.  Line 0 is the first row of data.  A current row number of
    // -1 is used internally to indicate that no data has yet been read from a file.
    int currentRowNumber() const;

    // Field/column names are case-insensitive.
    QString fieldName( const int index ) const; // Returns the field/column name of field/column index.
    QStringList fieldNames() const {return _fieldNames; } // Returns a list of all field/column names.
    int fieldIndexOf( const QString& fieldName ); // Returns the field/column number of the specified field name.
    bool renameFields( const QStringList& newFieldNames ); // Rename all fields/columns with the names in the list.  The number of new names provided must match the number of existing names.
    bool renameField( QString oldName, QString newName ); // Change the name of field 'oldName' to 'newName'.
    bool containsFieldName( const QString& fieldName ); // Is there a field called 'fieldName'?

    // The field at position index (starting from 0) or with the name 'fieldName' of the current line.
    // May be used with either line-by-line or entire-file mode.
    QString field( const int index );
    QString field( const QString& fieldName );

    // The field at position index (starting from 0) or with the name 'fieldName' of the row at 'rowNumber'.
    // Side-effect: move the current row number to 'rowNumber'.
    // Available only in entire-file mode.
    QString field( const int index, const int rowNumber );
    QString field( const QString& fieldName, const int rowNumber );

    QString currentRow(); // Returns the current line of the object as a CSV-formatted string
    QStringList rowData(); // Returns all of the data from the current line of the object as a QStringList
    QStringList rowData( const int idx ); // Returns all of the data from line idx of the object as a QStringList (only for entire-file mode)
    QStringList rowData( const int idx ) const;

    // Returns all of the values in the column/field specified by 'index' or 'fieldName'.
    // Optionally: return only the unique values in this field (if 'unique' is true).
    // While these work with line-by-line mode, only a single value will be returned for this mode.
    QStringList fieldValues( const int index, const bool unique = false );
    QStringList fieldValues( const QString& fieldName, const bool unique = false );

    // Set the value of the field at 'index' or 'fieldName' in the current row to 'val'.
    bool setField( const int index, const QString& val );
    bool setField( const QString& fieldName, const QString& val );

    // Set the value of the field at 'index' or 'fieldName' in the row 'rowNumber' to 'val'.
    // Side effect: move the current row number to 'rowNumber'.
    // These work only in entire-file mode.
    bool setField( const int index, const int rowNumber, const QString& val );
    bool setField( const QString& fieldName, const int rowNumber, const QString& val );

    // It's not always possible to automatically determine the intended format of a column,
    // particularly for dates and times.  These functions, which can be called once a file is open,
    // will properly format a column with the indicated date or time format.
    // For Excel dates, see CXlCsv::setFieldFormatXl(), which deals with the conversion of integers
    // to dates using either the 1900 and 1904 date systems.
    bool setFieldFormat( const QString& fieldName, const ColumnFormat columnFmt, const StrUtilsDateFormat dateFmt, const int defaultCentury = 2000 );
    bool setFieldFormat( const int fieldIdx, const ColumnFormat columnFmt, const StrUtilsDateFormat dateFmt, const int defaultCentury = 2000 );

    bool writeFile( const QString &filename, const QString &codec = QString() ); // Write contents of the CSV object to a file.

    // Generates a subset of this object, with only rows in which the indicated field contains the indicated value (which may or may not be case-sensitive).
    // These functions work only in entire-file mode.
    QCsv filter( const int index, const QString& value, const Qt::CaseSensitivity cs = Qt::CaseSensitive );
    QCsv filter( const QString& fieldName, const QString& value, const Qt::CaseSensitivity cs = Qt::CaseSensitive );

    // Functions for modifying a CSV object in memory. These work only for read mode qCSV_EntireFile.
    bool appendField( const QString& fieldName ); // Add a new field/column with the name 'fieldName'.  The field will be empty, but can be added to with setField.
    bool removeField( const QString& fieldName ); // Remove the field/column 'fieldName' (as well as all data in the column!)
    bool removeField( const int index ); // Remove the field/column at 'index' (as well as all data in the column)
    bool append( const QStringList& values ); // Add a new row to the end of the CSV structure.
    bool append(const QCsv& other ); // Add the contents of other to this.
    bool merge( const QCsv& other ); // Add items from other that do not already appear in structure to this structure.

    QString asTable(); // Renders the CSV object as a formatted table with fixed-width columns.
    void debug( int nLines = 0 ); // Prints contents of the object to the debugging console.

    // Return any current error flag or a human-readable error message.
    CSVErrorCode error(){return _error;}
    QString errorMsg(){ return _errorMsg; }
    void setError( const CSVErrorCode error, const QString& msg ) { _error = error; _errorMsg = msg; } // Almost exclusively used internally, but there are cases when this might be set.

    // How many comment rows (rows that start with '#') were encountered at the beginning of the file (and not processed)?
    // Used with checkForComment
    int nCommentRows(){ return _comments.count(); }
    QStringList comments() { return _comments; }

    QString sourceFileName() { return _srcFilename; }

    // Properties
    //-----------
    bool isOpen() const { return _isOpen; }

    // These properties can be used
    void setContainsFieldList( const bool setVal ); // If true then the first line of the file will be treated as a list of field/column names.  Default value is true.
    bool containsFieldList() const { return _containsFieldList; }

    void setFilename( const QString& filename ); // The file name of the CSV file to process
    QString filename() const { return _srcFilename; }

    void setDelimiter( const QChar val ) { _delimiter = val; } // The delimiter character.  Default value is a comma.
    QChar delimiter() const { return _delimiter; }

    void setMode( const QCsvMode val ) { _mode = val; } // Either line-by-line or entire-file.  See enum above.
    QCsvMode mode() const { return _mode; }

    // If true, then lines at the start of the file that begin with '#' will be treated as comments and will not be processed.
    // Default value is false.
    // Note that this currently is not used to find and skip comments that occur anywhere else in a file: only the lines at the top are checked.
    void setCheckForComments( const bool val ) { _checkForComments = val; }
    bool checkForComments() const { return _checkForComments; }

    // Used to specify the number of lines at the start of a file to skip (i.e., not to process).
    // Similar to checkForComments, but skips a fixed number of lines rather than checking for lines that start with #.
    bool setLinesToSkip( const int val );
    int linesToSkip() const { return _linesToSkip; }

    // If true, then check that values contain delimiters, e.g.:
    // This is one value: "Reeves, Aaron"
    // This is two values: Reeves, Aaron
    // Unless absolutely certain that a delimiter will never appear inside a quoted value, set this to true (the default value).
    // If absolutely certain that this can never be the case, performance might be slightly enhanced by setting this to false.
    void setStringsContainDelimiters( bool val ){ _stringsContainDelimiters = val;}
    bool stringsContainDelimiters() const { return _stringsContainDelimiters; }

    // If line breaks are encountered within quotation marks, what should be used to indicate the end-of-line in the value?  Default is a single space.
    // For example, if the following three values were encountered in a CSV file (note the line break):
    //
    //   1, "Aaron
    //   Reeves", SRUC
    //
    // The default settings would render these as: 1, Aaron Reeves, SRUC
    // To preserve line breaks, setEolDelimiter to either '\n' or '\r\n'
    void setEolDelimiter( const QString& val ) { _eolDelimiter = val; }
    QString eolDelimiter() const { return _eolDelimiter; }

    // Replaces single quotes " with double quotes "", and if the string contains a delimiter or a space, wraps s in double quotes.
    static QString csvQuote( QString s, const QChar delimiter = ',' );
    static QString csvQuote( QStringList s, const QChar delimiter = ',' );

  protected:
    void initialize();
    int readNext();
    void assign( const QCsv& other );

    bool openFileAndReadHeader();
    int readHeader();
    QString readLine();

    bool identicalFieldNames( const QStringList& otherNames ) const;

    void clearError();
    QStringList writeLine( const QStringList& line );

    bool isCommentLine( const QString& line );

    void setFieldNames( const QStringList& fieldNames );

    void finishWithFile();

    QString tablePadded( const QString& val, const int len );
    QString tableDiv( const int len );

    QString      _srcFilename;
    QFile*       _srcFile;
    bool         _isOpen;
    QString      _currentLine;
    int          _currentRowNumber;
    CSVErrorCode _error;
    QString      _errorMsg;
    bool         _containsFieldList;
    bool         _stringsContainDelimiters;
    QString      _eolDelimiter;
    QChar        _delimiter;
    bool         _checkForComments;
    int          _linesToSkip;
    int          _linesSkipped;
    QCsvMode     _mode;

    QStringList _comments;

    // Key is the field name, converted to lower case.
    // Value is the position of the field in the file (i.e., the column number), starting from 0.
    QHash<QString, int> _fieldsLookup;

    // List of field names as they were in the original file.
    QStringList _fieldNames;

    // Data for the current row.
    QStringList _fieldData;

    // All rows of data, if an entire file has been read into memory.
    QList<QStringList> _data;
};


/* Just like QCsv, but emits a signal whenever data is read.
 * This signal, nBytesRead, indicates the number of bytes of data read from a file,
 * and can be handled in the same way any other Qt signal might be treated.
 */
class QCsvObject : public QObject, QCsv {
  Q_OBJECT

  public:
    QCsvObject();

    // This version is used to generate a CSV object from a file.
    QCsvObject(
      const QString& filename,
      const bool containsFieldList,
      const bool stringsContainDelimiters = true,
      const QCsvMode mode = QCsv::LineByLine,
      const bool checkForComment = false
    );

    // Used for building a CSV data set from scratch.
    QCsvObject( const QStringList& fieldNames );
    QCsvObject( const QStringList& fieldNames, const QList<QStringList>& data );

    //qCSV( const qCSV& other );

    virtual ~QCsvObject();

    int readNext();

  signals:
    void nBytesRead( const int val );
};


#endif // CSV_H

