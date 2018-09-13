/*
csv.h/cpp
----------
Begin: 2014/04/11
Author (modified version): Aaron Reeves <aaron.reeves@naadsm.org>
------------------------------------------------------------------------

Original code (name space CSV) by Naohiro Hasegawa, https://github.com/hnaohiro/qt-csv/blob/master/csv.h
Original code (class qCSV) believed to be by Shaun Case, Animal Population Health Institute, Colorado State University.
*/

#include "csv.h"

#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QRegExp>
#include <QDebug>

#include <ar_general_purpose/strutils.h>

QStringList CSV::parseLine( const QString& string, const QChar delimiter /* = ',' */ ) {
  enum State {Normal, Quote} state = Normal;
  QStringList line;
  QString temp, value;

  temp = string.trimmed();

  //qDebug() << "String size:" << temp.size();

  for (int i = 0; i < temp.size(); i++) {
    QChar current = temp.at(i);

    // Normal state
    if (state == Normal) {
      // Delimiter encountered
      if (current == delimiter ) {
        // add line
        line.append(value.trimmed());
        value.clear();
      }
      // One quote mark encountered.  Ignore delimiters until the matching quote mark is encountered.
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
      if( current == '"' ) {
        // If at the end of the line
        if( i == temp.size() - 1 ) {
          state = Normal;
        }
        else {
          int index = i+1; // (i+1 < temp.size() - 1) ? i+1 : temp.size() - 1;
          QChar next = temp.at(index);
          if (next == '"') {
            value += '"';
            i++;
          } else {
            state = Normal;
          }
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

QList<QStringList> parse(const QString &string, const QChar delimiter /* = ',' */){
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
      else if (current == delimiter) {
        // add line
        line.append(value.trimmed());
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

  if( !result.isEmpty() && ( result.at(result.size()-1) != '\n' ) ) {
    result += '\n';
  }
  return result;
}


QList<QStringList> CSV::parseFromString(const QString &string , const QChar delimiter /* = ',' */){
  return parse( initString(string), delimiter );
}


QList<QStringList> CSV::parseFromFile(const QString &filename, const QChar delimiter /* = ',' */, const QString &codec){
  QString string;
  QFile file(filename);
  if (file.open(QIODevice::ReadOnly)) {
    QTextStream in(&file);
    if( !codec.isEmpty() )
      in.setCodec(QTextCodec::codecForName(codec.toLatin1()));
    string = in.readAll();
    file.close();
  }
  return parse(initString(string), delimiter );
}


QString CSV::writeLine( const QStringList& line, const QChar delimiter /* = ',' */, const int stringCase /* = OriginalCase */ ) {
  QStringList output;

  foreach (QString value, line) {
    switch( stringCase ) {
      case TitleCase:
        value = titleCase( value );
        break;
      case UpperCase:
        value = value.toUpper();
        break;
      case LowerCase:
        value = value.toLower();
        break;
      default:
        // Do nothing.  Use OriginalCase.
        break;
    }

    value.replace( "\"", "\"\"" );

    if( value.contains( QRegExp( "\"\r\n" ) ) || value.contains( delimiter ) || value.contains( QRegExp( "\\s+" ) ) ) {
      output << ("\"" + value + "\"");
    } else {
      output << value;
    }
  }

  return( output.join( delimiter ) );
}


bool CSV::write(const QList<QStringList> data, const QString &filename,  const QChar delimiter /* = ',' */, const QString &codec){
  QFile file(filename);
  if (!file.open( QFile::WriteOnly | QFile::Text )) {
    return false;
  }

  QTextStream out(&file);
  if( !codec.isEmpty() )
    out.setCodec(codec.toLatin1());

  foreach (const QStringList &line, data) {
    QString output = writeLine( line, delimiter );
    out << output << "\r\n";
  }

  file.close();

  return true;
}


QCsv::QCsv() {
  initialize();
}


QCsv::QCsv (
  const QString& filename,
  const bool containsFieldList,
  const bool stringsContainDelimiters /* = true */,
  const QCsvMode mode /* = qCsv::ReadLineByLine */,
  const bool checkForComments /* = false */
) {
  initialize();

  _srcFilename = filename;
  _containsFieldList = containsFieldList;
  _checkForComments = checkForComments;

  _stringsContainDelimiters = stringsContainDelimiters;
  _mode = mode;

  if( EntireFile == _mode ) {
    this->open();
  }
}


void QCsv::processString(
    QString text,
    const bool containsFieldList,
    const bool stringsContainDelimiters
) {
  initialize();

  _srcFilename = QString(); // There is no source file.
  _containsFieldList = containsFieldList;
  _checkForComments = false; // There is no allowance for comments here.
  _comments.clear();

  _stringsContainDelimiters = stringsContainDelimiters;
  _mode = EntireFile;

  QList<QStringList> items = CSV::parseFromString( text.trimmed() );

  QString str;
  if( containsFieldList ) {
    for( int i = 0; i < items.at(0).count(); ++i ) {
      str = items.at(0).at(i).trimmed();
      _fieldNames.append( str );
      _fieldsLookup.insert( str.toLower(), i );
    }
  }

  for( int i = 1; i < items.count(); ++i ) {
    _data.append( items.at( i ) );
  }

  this->toFront();
}


QCsv::QCsv( const QStringList& fieldNames ) {
  initialize();
  setFieldNames( fieldNames );
}


QCsv::QCsv( const QStringList& fieldNames, const QList<QStringList>& data ) {
  initialize();
  setFieldNames( fieldNames );

  for( int i = 0; i < data.count(); ++i ) {
    appendRow( data.at(i) );
  }
}


QCsv::QCsv( const QStringList& fieldNames, const QStringList& data ) {
  initialize();
  setFieldNames( fieldNames );

  appendRow( data );
}


bool QCsv::renameFields( const QStringList& newFieldNames ) {
  if( newFieldNames.count() != _fieldNames.count() )
    return false;
  else {
    _fieldNames.clear();
    _fieldsLookup.clear();

    for( int i = 0; i < newFieldNames.count(); ++i ) {
      _fieldNames.append( newFieldNames.at(i).trimmed() );
      _fieldsLookup.insert( newFieldNames.at(i).trimmed().toLower(), i );
    }
    return true;
  }
}


bool QCsv::renameField( QString oldName, QString newName ) {
  newName = newName.trimmed();
  oldName = oldName.trimmed();

  if( oldName.toLower() == newName.toLower() ) {
    // This could be a change of case.  Go through the motions, just in case.
    int idx = fieldIndexOf( oldName.toLower() );

    _fieldNames.replace( idx, newName );
    int hashVal = _fieldsLookup.value( oldName.toLower() );
    _fieldsLookup.remove( oldName.toLower() );
    _fieldsLookup.insert( newName.toLower(), hashVal );

    return true;
  }
  else if( !_fieldNames.contains( oldName, Qt::CaseInsensitive ) )
    return false;
  else if( _fieldNames.contains( newName, Qt::CaseInsensitive ) )
    return false;
  else {
    int idx = fieldIndexOf( oldName.toLower() );

    _fieldNames.replace( idx, newName );
    int hashVal = _fieldsLookup.value( oldName.toLower() );
    _fieldsLookup.remove( oldName.toLower() );
    _fieldsLookup.insert( newName.toLower(), hashVal );

    return true;
  }
}


void QCsv::setFieldNames( const QStringList& fieldNames ) {
  _mode = EntireFile;
  _containsFieldList = true;
  QString str;

  for( int i = 0; i < fieldNames.count(); ++i ) {
     str = fieldNames.at(i).trimmed();
    _fieldNames.append( str );
    _fieldsLookup.insert( str.toLower(), i );
  }
}


void QCsv::initialize() {
  _srcFilename = "";
  _srcFile = NULL;
  _isOpen = false;

  clearError();

  _currentLine = "";
  _currentRowNumber = -1;
  _containsFieldList = true;
  _stringsContainDelimiters = true;
  _mode = UnspecifiedMode;
  _eolDelimiter = " ";
  _delimiter = ',';
  _checkForComments = false;
  _comments.clear();

  _linesToSkip = 0;
  _linesSkipped = 0;
}


QCsv::QCsv( const QCsv& other ) {
  assign( other );
}


QCsv& QCsv::operator=( const QCsv& other ) {
  assign( other );

  return *this;
}


void QCsv::assign( const QCsv& other ) {
  _srcFilename = other._srcFilename;
  _srcFile = NULL;
  _isOpen = other._isOpen;

  _currentLine = other._currentLine;
  _currentRowNumber = other._currentRowNumber;
  _error = other._error;
  _errorMsg = other._errorMsg;
  _containsFieldList = other._containsFieldList;
  _stringsContainDelimiters = other._stringsContainDelimiters;
  _mode = other._mode;
  _eolDelimiter = other._eolDelimiter;
  _delimiter = other._delimiter;
  _checkForComments = other._checkForComments;
  _comments = other._comments;

  _linesToSkip = other._linesToSkip;
  _linesSkipped = other._linesToSkip;

  _fieldsLookup = other._fieldsLookup;
  _fieldNames = other._fieldNames;
  _fieldData = other._fieldData;
  _data = other._data;

  if( other._isOpen && ( LineByLine == other._mode ) ) {
    this->open();
  }
}


QCsv::~QCsv() {
  this->close();

  if( NULL != _srcFile ) {
    _srcFile->close();
    delete _srcFile;
    _srcFile = NULL;
  }
}


void QCsv::debug( int nLines /* = 0 */ ) {
  qDebug() << "qCSV contents:";

  qDebug() << "numFields:" << this->fieldCount();
  qDebug() << "numFieldNames:" << this->fieldNames().count();
  qDebug() << "numRows:" << this->rowCount();

  qDebug() << this->fieldNames().join( _delimiter ).prepend( "  " );

  if( LineByLine == _mode )
    qDebug() << "(There is nothing to display)";
  else {
    if( (1 > nLines) || ( _data.count() < nLines ) )
      nLines = _data.count();

    for( int i = 0; i < nLines; ++i ) {
      qDebug() << _data.at(i).join( _delimiter ).prepend( "  " );
    }
  }
}


// Accessors
QString QCsv::currentRow() {
  clearError();
  switch( _mode ) {
    case LineByLine:
      return _currentLine;
      break;
    case EntireFile:
      return CSV::writeLine( _data.at( _currentRowNumber ) );
      break;
    default:
      return "";
      break;
  }
}


QStringList QCsv::rowData() {
  if( LineByLine == _mode )
    return _fieldData;
  else
    return _data.at( currentRowNumber() )
  ;
}


QStringList QCsv::rowData( const int idx ) {
  Q_ASSERT( LineByLine != _mode );
  clearError();

  if( ( 0 > idx ) || ( _data.count() < idx ) ) {
    _error = ERROR_INDEX_OUT_OF_RANGE;
    return QStringList();
  }
  else {
    return _data.at( idx );
  }
}


QString QCsv::field( const int index ){
  QStringList* dataList;
  QString ret_val = "";
  clearError();

  if( LineByLine == _mode )
    dataList = &_fieldData;
  else
    dataList = &( _data[ _currentRowNumber ] );

  if ( dataList->size() > 0 ){
    if ( dataList->size() > index ){
      ret_val = dataList->at( index ).trimmed();
    }
    else{
      _error = ERROR_INDEX_OUT_OF_RANGE;
      _errorMsg = "For File Linenumber: " + QString::number ( _currentRowNumber ) + ", Field index, " + QString::number ( index ) + ", out of range";
    }
  }
  else{
    _error = ERROR_LINE_EMPTY;
    _errorMsg = "The current line, " + QString::number ( _currentRowNumber ) + " is empty.  Did you read a line first?";
  }

  return ret_val;
}


QString QCsv::field( const QString& fieldName ){
  QStringList* dataList;
  QString ret_val = "";
  clearError();

  if( LineByLine == _mode )
    dataList = &_fieldData;
  else
    dataList = &( _data[ _currentRowNumber ] );

  if ( _containsFieldList ){
    if ( dataList->size() > 0 ){
      if ( _fieldsLookup.contains( fieldName.trimmed().toLower() ) ){
        int index = _fieldsLookup.value( fieldName.trimmed().toLower() );

        ret_val = field( index );
      }
      else{
        _error = ERROR_INVALID_FIELD_NAME;
        _errorMsg = "Invalid Field Name: " + fieldName;
      }
    }
    else{
      _error = ERROR_LINE_EMPTY;
      _errorMsg = "The current line, " + QString::number ( _currentRowNumber ) + " is empty.  Did you read a line first?";
    }
  }
  else {
    _error = ERROR_NO_FIELDLIST;
    _errorMsg = "The current settings do not include a field list.";
  }

  return ret_val;
}


bool QCsv::setField( const int index, const QString& val ) {
  QStringList* dataList;
  clearError();
  bool result = true; // until shown otherwise.

  if( LineByLine == _mode )
    dataList = &_fieldData;
  else
    dataList = &( _data[ _currentRowNumber ] );

  if( 0 < dataList->size() ) {
    if ( index < dataList->size() ) {
      if( LineByLine == _mode ) {
        _fieldData[index] = val.trimmed();
        _currentLine = CSV::writeLine( _fieldData );
      }
      else {
        _data[_currentRowNumber][index] = val.trimmed();
      }
    }
    else {
      _error = ERROR_INDEX_OUT_OF_RANGE;
      _errorMsg = "For File Linenumber: " + QString::number ( _currentRowNumber ) + ", Field index, " + QString::number ( index ) + ", out of range";
      result = false;
    }
  }
  else{
    _error = ERROR_LINE_EMPTY;
    _errorMsg = "The current line, " + QString::number ( _currentRowNumber ) + " is empty.  Did you read a line first?";
    result = false;
  }

  return result;
}


bool QCsv::setField( const QString& fieldName, const QString& val ) {
  QStringList* dataList;
  clearError();
  bool result = true; // until shown otherwise.

  if( LineByLine == _mode )
    dataList = &_fieldData;
  else
    dataList = & (_data[ _currentRowNumber ] );

  if ( _containsFieldList ){
    if ( dataList->size() > 0 ){
      if ( _fieldsLookup.contains( fieldName.trimmed().toLower() ) ){
        int index = _fieldsLookup.value( fieldName.trimmed().toLower() );

        setField( index, val );
      }
      else{
        _error = ERROR_INVALID_FIELD_NAME;
        _errorMsg = "Invalid Field Name: " + fieldName;
        result = false;
      }
    }
    else{
      _error = ERROR_LINE_EMPTY;
      _errorMsg = "The current line, " + QString::number ( _currentRowNumber ) + " is empty.  Did you read a line first?";
      result = false;
    }
  }
  else {
    _error = ERROR_NO_FIELDLIST;
    _errorMsg = "The current settings do not include a field list.";
    result = false;
  }

  return result;
}


bool QCsv::setField( const int index, const int rowNumber, const QString& val ) {
  // This function will not work with qCSV_LineByLine mode.
  Q_ASSERT( LineByLine != _mode );
  clearError();
  bool result = true; // until shown otherwise.

  if( LineByLine == _mode ) {
    setError( ERROR_WRONG_MODE, "Only EntireFile mode may be used with this function." );
    result = false;
  }
  else if( rowNumber > (_data.count() - 1) ) {
    _error = ERROR_INDEX_OUT_OF_RANGE;
    _errorMsg = QString( "There is no row %1" ).arg( rowNumber );
    result = false;
  }
  else {
    _currentRowNumber = rowNumber;
    setField( index, val );
  }

  return result;
}


bool QCsv::setField( const QString& fieldName, const int rowNumber, const QString& val ) {
  // This function will not work with qCSV_LineByLine mode.
  Q_ASSERT( LineByLine != _mode );
  clearError();
  bool result = true; // until shown otherwise.

  if( LineByLine == _mode ) {
    setError( ERROR_WRONG_MODE, "Only EntireFile mode may be used with this function." );
    result = false;
  }
  else if ( rowNumber > (_data.count() - 1) ) {
    _error = ERROR_INDEX_OUT_OF_RANGE;
    _errorMsg = QString( "There is no row %1" ).arg( rowNumber );
    result = false;
  }
  else {
    _currentRowNumber = rowNumber;
    setField( fieldName.trimmed(), val );
  }

  return result;
}

QString QCsv::field( const int index, const int rowNumber ) {
  // This function will not work with qCSV_LineByLine mode.
  Q_ASSERT( LineByLine != _mode );
  clearError();

  if( LineByLine == _mode ) {
    setError( ERROR_WRONG_MODE, "Only EntireFile mode may be used with this function." );
    return QString();
  }
  else if( rowNumber > (_data.count() - 1) ) {
    _error = ERROR_INDEX_OUT_OF_RANGE;
    _errorMsg = QString( "There is no row %1" ).arg( rowNumber );
    return QString();
  }
  else {
    _currentRowNumber = rowNumber;
    return field( index );
  }
}

QString QCsv::field( const QString& fieldName, const int rowNumber ) {
  // This function will not work with qCSV_LineByLine mode.
  Q_ASSERT( LineByLine != _mode );
  clearError();

  if( LineByLine == _mode ) {
    setError( ERROR_WRONG_MODE, "Only EntireFile mode may be used with this function." );
    return QString();
  }
  else if( rowNumber > (_data.count() - 1) ) {
    _error = ERROR_INDEX_OUT_OF_RANGE;
    _errorMsg = QString( "There is no row %1" ).arg( rowNumber );
    return QString();
  }
  else {
    _currentRowNumber = rowNumber;
    return field( fieldName );
  }
}


int QCsv::currentRowNumber() const {
  if( -1 == _currentRowNumber )
    return 0;
  else
    return _currentRowNumber;
}


QString QCsv::fieldName( const int index ){
  QString ret_val = "";
  if ( _containsFieldList ){
    ret_val = _fieldNames.at( index );
  }

  return ret_val;
}


int QCsv::fieldIndexOf( const QString& fieldName ) {
  int result = -1;

  for( int i = 0; i < _fieldNames.count(); ++i ) {
    if( 0 == fieldName.trimmed().compare( _fieldNames.at(i), Qt::CaseInsensitive ) ) {
      result = i;
      break;
    }
  }

  return result;
}


bool QCsv::containsFieldName( const QString& fieldName ) {
  return _fieldsLookup.contains( fieldName.trimmed().toLower() );
}


bool QCsv::appendField( const QString& fieldName ) {
  // This function will not work with qCSV_LineByLine mode.
  Q_ASSERT( LineByLine != _mode );
  clearError();

  if( LineByLine == _mode ) {
    setError( ERROR_WRONG_MODE, "Only EntireFile mode may be used with this function." );
    return false;
  }
  else {
    _fieldNames.append( fieldName.trimmed() );
    _fieldsLookup.insert( fieldName.trimmed().toLower(), _fieldNames.count() - 1 );

    for( int i = 0; i < _data.count(); ++i ) {
      _data[i].append( "" );
    }

    return true;
  }
}


bool QCsv::removeField( const QString& fieldName ) {
  // This function will not work with qCSV_LineByLine mode.
  Q_ASSERT( LineByLine != _mode );
  clearError();

  if( LineByLine == _mode ) {
    setError( ERROR_WRONG_MODE, "Only EntireFile mode may be used with this function." );
    return false;
  }
  else {
    if( _fieldsLookup.contains( fieldName.trimmed().toLower() ) )
      return removeField( _fieldsLookup.value( fieldName.trimmed().toLower() ) );
    else {
      _error = ERROR_INVALID_FIELD_NAME;
      _errorMsg = "Invalid Field Name: " + fieldName;
      return false;
    }
  }
}


bool QCsv::removeField( const int index ) {
  // This function will not work with qCSV_LineByLine mode.
  Q_ASSERT( LineByLine != _mode );
  clearError();

  if( LineByLine == _mode ) {
    setError( ERROR_WRONG_MODE, "Only EntireFile mode may be used with this function." );
    return false;
  }
  else if( index < fieldCount() ) {
    if( _containsFieldList ) {
      _fieldNames.removeAt( index );

      QList<QString> keys = _fieldsLookup.keys( index );
      for( int i = 0; i < keys.count(); ++i ) {
        _fieldsLookup.remove( keys.at(i) );
      }
    }

    for( int i = 0; i < _data.count(); ++i ) {
      _data[i].removeAt( index );
    }
    return true;
  }
  else {
    _error = ERROR_INDEX_OUT_OF_RANGE;
    _errorMsg = QString( "Invalid Field Number: %1" ).arg( index );
    return false;
  }
}


bool QCsv::appendRow( const QStringList& values ) {
  // This function only works for qCSV_EntireFile.
  Q_ASSERT( LineByLine != _mode );
  clearError();

  if( LineByLine == _mode ) {
    setError( ERROR_WRONG_MODE, "Only EntireFile mode may be used with this function." );
    return false;
  }
  else if( _data.isEmpty() || ( values.count() == fieldCount() ) ) {
    QStringList trimmedVals;
    for( int i = 0; i < values.count(); ++i ) {
      trimmedVals.append( values.at(i).trimmed() );
    }

    _data.append( trimmedVals );
    return true;
  }
  else {
    _error = ERROR_INDEX_OUT_OF_RANGE;
    _errorMsg = QString( "Field count mismatch: %1 <> %2" ).arg( values.count() ).arg( fieldCount() );
    return false;
  }
}


QStringList QCsv::fieldValues( const QString& fieldName, const bool unique /* = false */ ) {
  QStringList result;
  clearError();

  if( !_containsFieldList ) {
    _error = ERROR_NO_FIELDLIST;
    _errorMsg = "The current settings do not include a field list.";
  }
  else {
    if( _fieldsLookup.contains( fieldName.trimmed().toLower() ) ){
      int index = _fieldsLookup.value( fieldName.trimmed().toLower() );

      result  = fieldValues( index, unique );
    }
    else {
      _error = ERROR_INVALID_FIELD_NAME;
      _errorMsg = "Invalid Field Name: " + fieldName;
    }
  }

  return result;
}


QStringList QCsv::fieldValues( const int index, const bool unique /* = false */ ) {
  QStringList result;
  clearError();

  if( LineByLine == _mode ) {
    result.append( this->field( index ) );
  }
  else {
    if( index > ( fieldCount() - 1) ) {
      _error = ERROR_INDEX_OUT_OF_RANGE;
      _errorMsg = QString( "There is no column %1" ).arg( index );
    }
    else {
      for( int i = 0; i < _data.count(); ++i ) {
        if( unique ) {
          if( !result.contains( _data.at( i ).at( index ) ) )
            result.append( _data.at( i ).at( index ) );
        }
        else {
          result.append( _data.at( i ).at( index ) );
        }
      }
    }
  }

  return result;
}


QCsv QCsv::filter( const int index, const QString& value, const Qt::CaseSensitivity cs /* = Qt::CaseSensitive */ ) {
  // This function will not work with qCSV_LineByLine mode.
  Q_ASSERT( LineByLine != _mode );
  clearError();

  QCsv result;
  if( LineByLine == _mode ) {
    setError( ERROR_WRONG_MODE, "Only EntireFile mode may be used with this function." );
    result.setError( ERROR_WRONG_MODE, "Filtered CSVs can only be created from CSVs in EntireFile mode." );
  }
  else {
    result = QCsv( this->fieldNames() );

    for( int i = 0; i < _data.count(); ++i ) {
      if( 0 == value.compare( this->field( index, i ), cs ) ) {
        result.appendRow( _data.at(i) );
      }
    }
  }

  result.toFront();
  return result;
}


QCsv QCsv::filter( const QString& fieldName, const QString& value, const Qt::CaseSensitivity cs /* = Qt::CaseSensitive */ ) {
  QCsv result;

  // This function will not work with qCSV_LineByLine mode.
  Q_ASSERT( LineByLine != _mode );
  clearError();

  if( LineByLine == _mode ) {
    setError( ERROR_WRONG_MODE, "Only EntireFile mode may be used with this function." );
    result.setError( ERROR_WRONG_MODE, "Filtered CSVs can only be created from CSVs in EntireFile mode." );
  }
  else if( !_containsFieldList ) {
    _error = ERROR_NO_FIELDLIST;
    _errorMsg = "The current settings do not include a field list.";
    result.setError( _error, _errorMsg );
  }
  else if( !_fieldsLookup.contains( fieldName.trimmed().toLower() ) ) {
    _error = ERROR_INVALID_FIELD_NAME;
    _errorMsg = "Invalid Field Name: " + fieldName;
     result.setError( _error, _errorMsg );
  }
  else {
    result = filter( _fieldsLookup.value( fieldName.trimmed().toLower() ), value, cs );
  }

  return result;
}


int QCsv::fieldCount() {
  if( _containsFieldList )
    return _fieldNames.count();
  else if( LineByLine == _mode )
    return _fieldData.count();
  else if( 0 < rowCount() )
    return _data.at(0).count();
  else
    return 0;
}

int QCsv::rowCount() {
  int result;

  if( LineByLine == _mode ) {
    setError( ERROR_WRONG_MODE, "Only EntireFile mode may be used with this function." );
    result = -1;
  }
  else
    result = _data.count();

  return result;
}


QStringList QCsv::writeLine( const QStringList& line ) {
  clearError();

  QStringList output;

  output.clear();
  foreach (QString value, line) {
    value.replace("\"", "\"\"");

    if (value.contains(QRegExp(",|\r\n")))
      output << ("\"" + value + "\"");
    else
      output << value;
  }

  return output;
}


bool QCsv::writeFile( const QString &filename, const QString &codec ) {
  clearError();

  QStringList output;
  QStringList header;

  QFile file(filename);
  if (!file.open(QIODevice::WriteOnly)) {
    return false;
  }

  QTextStream out(&file);
  if( !codec.isEmpty() )
    out.setCodec(codec.toLatin1());

  // Write the header row first...
  if( this->_containsFieldList ) {
    for( int i = 0; i < _fieldNames.count(); ++i ) {
      header.append( _fieldNames.at(i) );
    }
    output = writeLine( header );
    out << output.join(",") << "\r\n";
  }

  // Then write the data.
  foreach (const QStringList &line, _data) {
    output = writeLine( line );
    out << output.join(",") << "\r\n";
  }


  file.close();

  return true;
}


//  Mutators
void QCsv::setContainsFieldList ( bool setVal ){
  clearError();
  _containsFieldList = setVal;
}


bool QCsv::setLinesToSkip( const int val ) {
  if( 0 > val ) {
    setError( ERROR_INDEX_OUT_OF_RANGE, "Number of lines to skip must be 0 or more." );
    return false;
  }
  else {
    _linesToSkip = val;
    return true;
  }
}


void QCsv::setFilename( QString filename ){
  clearError();
  if( NULL != _srcFile ) {
    _srcFile->close();
    _isOpen = false;
    delete _srcFile;
    _srcFile = NULL;
  }

  _srcFilename = filename;
}


bool QCsv::open() {
  if( EntireFile != mode() ) {
    _isOpen = openFileAndReadHeader();
  }
  else {
    if( !isOpen() ) {
      if( openFileAndReadHeader() ) {
        int fieldsRead = 0;
        while( -1 != fieldsRead ) {
          fieldsRead = readNext();
        }

        this->finishWithFile();

        this->toFront();

        _isOpen = true;
      }
      else {
        _isOpen = false;
      }
    }
  }

  return _isOpen;
}


bool QCsv::openFileAndReadHeader() {
  clearError();

  if( NULL != _srcFile ) {
    delete _srcFile;
  }

  _srcFile = new QFile( _srcFilename );

  bool result = _srcFile->open( QIODevice::ReadOnly | QIODevice::Text );

  if( !result ) {
    _error = ERROR_OPEN;
    _errorMsg = "Can not open the source file";
    delete _srcFile;
    _srcFile = NULL;
  }
  else if( _containsFieldList ) {
    readHeader();
  }

  return result;
}


void QCsv::finishWithFile() {
  if( NULL != _srcFile ) {
    _srcFile->close();
    delete _srcFile;
    _srcFile = NULL;
  }
}


void QCsv::close(){
  clearError();
  finishWithFile();
  _isOpen = false;
}


bool QCsv::toFront() {
  // This function will not work with qCSV_LineByLine mode.
  Q_ASSERT( LineByLine != _mode );
  clearError();

  if( LineByLine == _mode ) {
    setError( ERROR_WRONG_MODE, "Only EntireFile mode may be used with this function." );
    return false;
  }
  else {
    _currentRowNumber = -1;
    return true;
  }
}


//  Returns the number of fields in the row, or -1 at the end of the file.
int QCsv::moveNext() {
  clearError();

  if( LineByLine == _mode ) {
    if( _isOpen )
      return readNext();
    else {
      this->setError( ERROR_OPEN, "Object is not open." );
      return -1;
    }
  }
  else {
    ++_currentRowNumber;

    if( _currentRowNumber < _data.count() ) {
      _fieldData.clear();
      _fieldData = _data.at( _currentRowNumber );
      return _data.at( _currentRowNumber ).count();
    }
    else {
      return -1;
    }
  }
}


QString QCsv::readLine() {
  QByteArray arr;
  int nQuotes = 0;
  QString tmp;
  QString result;

  // The do loop handles situations where end-of-line
  // characters are encountered inside quote marks.
  do {
    arr = _srcFile->readLine();
    arr.replace( '\0', "" );
    tmp = arr;

    // FIXME: Is there a better way to handle delimiters here?
    if( !_delimiter.isSpace() )
      tmp = tmp.trimmed();

    result.append( tmp );

    if( !result.isEmpty() )
      result.append( _eolDelimiter );

    nQuotes = nQuotes + tmp.count( '\"' );
  } while( 0 != nQuotes%2 );

  //qDebug() << "Result: " << result;
  return result;
}


int QCsv::readHeader() {
  int result = -1;
  QStringList fieldList;

  clearError();

  _fieldData.clear();

  _currentLine = readLine();

  if( !_currentLine.isEmpty() ) {
    ++_currentRowNumber;

    // If the user wants to skip any lines, do that here.
    if( _linesSkipped < _linesToSkip ) {
      ++_linesSkipped;
      return readHeader();
    }

    // This next block handles the situation where the file
    // begins with a header (indicated by lines that start with #).
    // These lines should simply be skipped.
    if( _checkForComments && isCommentLine( _currentLine ) ) {
      _comments.append( _currentLine );
      return readHeader();
    }

    if ( _stringsContainDelimiters )
      fieldList = CSV::parseLine( _currentLine, _delimiter );
    else {
      fieldList = _currentLine.split( _delimiter );
      for( int i = 0; i < fieldList.count(); ++i ) {
        if( fieldList.at(i).startsWith( "\"" ) && fieldList.at(i).endsWith( "\"" ) ) {
          fieldList[i] = fieldList.at(i).mid( 1, fieldList.at(i).length() - 2 );
        }
      }
    }

    for ( int i = 0; i < fieldList.size(); i++ ){
      QString tempString = fieldList.at(i);
      tempString = tempString.trimmed();

      _fieldNames.append( tempString );
      _fieldsLookup.insert( tempString.toLower(), i );
    }

    _fieldData.clear();
    result = _fieldNames.count();
  }

  return result;
}


//  Cause a read of a line of data from the csv file.
//  Returns the number of fields read, or -1 at the end of the file.
int QCsv::readNext() {
  int result = -1;
  QStringList fieldList;

  clearError();

  _fieldData.clear();

  _currentLine = readLine();

  if( !_currentLine.isEmpty() ) {
    ++_currentRowNumber;

    if ( _stringsContainDelimiters )
      fieldList = CSV::parseLine( _currentLine, _delimiter );
    else {
      fieldList = _currentLine.split( _delimiter );
      for( int i = 0; i < fieldList.count(); ++i ) {
        if( fieldList.at(i).startsWith( "\"" ) && fieldList.at(i).endsWith( "\"" ) ) {
          fieldList[i] = fieldList.at(i).mid( 1, fieldList.at(i).length() - 2 );
        }
      }
    }

    if( 0 != fieldCount() && ( fieldList.count() != fieldCount() ) ) {
      //qDebug() << "Error!";
      _error = ERROR_INVALID_FIELD_COUNT;
      _errorMsg = QString( "Line %1: %2 fields expected, but %3 fields encountered.  Please check your file format." ).arg( _currentRowNumber ).arg( fieldCount() ).arg( fieldList.count() );
      result = -1;
    }
    else {
      for ( int i = 0; i < fieldList.count(); i++ ){
        QString tempString = fieldList.at(i);
        tempString = tempString.trimmed();

        _fieldData.insert( i, tempString );
      }

      result = _fieldData.count();

      if( EntireFile == _mode ) {
        _data.append( _fieldData );
      }
    }
  }
  else if ( !_srcFile->atEnd() ){
    _error = ERROR_BAD_READ;
    _errorMsg = "Can not read next line.  Last line number was: " + QString::number ( _currentRowNumber ) + ".  Are we at the end of the file?";
  }

  return result;
}


bool QCsv::setFieldFormat( const QString& fieldName, const ColumnFormat columnFmt, const StrUtilsDateFormat dateFmt, const int defaultCentury /* = 2000 */ ) {
  bool result;

  if( !_isOpen ) {
    setError( QCsv::ERROR_OPEN, "File must be open to set a field format." );
    result = false;
  }
  else if( !this->containsFieldList() ) {
    setError( QCsv::ERROR_NO_FIELDLIST, "This file does not have field names." );
    result = false;
  }
  else {
    int fieldIdx = fieldIndexOf( fieldName.trimmed() );
    if( -1 == fieldIdx ) {
      setError( QCsv::ERROR_INVALID_FIELD_NAME, QString( "Field name '%1' does not exist." ).arg( fieldName ) );
      result = false;
    }
    else {
      result = setFieldFormat( fieldIdx, columnFmt, dateFmt, defaultCentury );
    }
  }

  return result;
}

bool QCsv::setFieldFormat( const int fieldIdx, const ColumnFormat columnFmt, const StrUtilsDateFormat dateFmt, const int defaultCentury /* = 2000 */ ) {
  bool result = true; // Until shown otherwise.

  if( !_isOpen ) {
    setError( QCsv::ERROR_OPEN, "File must be open to set a field format." );
    result = false;
  }
  else if( 0 > fieldIdx ) {
    setError( QCsv::ERROR_INDEX_OUT_OF_RANGE, "Negative field index provided." );
    result = false;
  }
  else if( this->fieldCount() <= fieldIdx ) {
    setError( QCsv::ERROR_INDEX_OUT_OF_RANGE, QString( "Field index %1 is out of range." ).arg( fieldIdx ) );
    result = false;
  }
  else {
    Q_ASSERT( columnFmt == DateFormat ); // FIXME: Eventually, support time and date/time formats.

    switch( columnFmt ) {
      case DateFormat:
        if( EntireFile == _mode ) {
          for( int row = 0; row < this->rowCount(); ++row ) {
            QString str = _data.at(row).at(fieldIdx);
            if( !str.isEmpty() ) {
              QDate date = guessDateFromString( str, dateFmt, defaultCentury );

              if( date.isValid() ) {
                _data[row][fieldIdx] = date.toString( "yyyy-MM-dd" );
              }
              else {
                setError( QCsv::ERROR_OTHER, QString( "Format of cell at row %1, column %2 cannot be changed to DateFormat." ).arg( row ).arg( fieldIdx ) );
                result = false;
              }
            }
          }
        }
        else if( LineByLine == _mode ) {
          QString str = _fieldData.at( fieldIdx );
          if( !str.isEmpty() ) {
            QDate date = guessDateFromString( str, dateFmt, defaultCentury );

            if( date.isValid() ) {
              _fieldData[fieldIdx] = date.toString( "yyyy-MM-dd" );
            }
            else {
              setError( QCsv::ERROR_OTHER, QString( "Format of data in field %1 cannot be changed to DateFormat." ).arg( fieldIdx ) );
              result = false;
            }
          }
        }
        else {
          // This should never happen: if a file is open, it's mode will have been set.
          setError( QCsv::ERROR_WRONG_MODE, "Mode must be specified to set a field format." );
          result = false;
        }

        break;
      case TimeFormat: // Fall through, for now.
      case DateTimeFormat: // Fall through, for now.
      default:
        setError( QCsv::ERROR_OTHER, "Specified field format not yet supported." );
        result = false;
        break;
    }
  }

  return result;
}


// Protected members
void QCsv::clearError(){
  _error = ERROR_NONE;
  _errorMsg = "(No error)";
}


bool QCsv::isCommentLine( const QString& line ) {
  bool result = ( '#' == line.at(0) );
  return result;
}


QString QCsv::tableDiv( const int len ) {
  QString result( "--" );
  for( int i = 0; i < len; ++i ) {
    result.append( "-" );
  }

  return result;
}


QString QCsv::tablePadded( const QString& val, const int len ) {
  QString result;
  int i;
  int lenDiff;

  if( val.length() <= len ) {
    // Prepend the leading space
    result = QString( " %1" ).arg( val ); // Note the leading space

    // Add spaces until desired length is reached
    lenDiff = len - val.length();
    for( i = 0; lenDiff > i; ++i ) {
      result.append( " " );
    }

    // Tack on the trailing space
    result.append( " " );
  }
  else {
    result = QString( " %1" ).arg( val ); // Note the leading space
    result = result.left( len - 2 );
    result = result + "... "; // Note the trailing space
  }

  return result;
}


QString QCsv::asTable() {
  if( ERROR_NONE != error() ) {
    return "";
  }

  QString result;
  QList<int> arr;

  if( _containsFieldList ) {
    for( int i = 0; i < _fieldNames.count(); ++i ) {
      arr << _fieldNames.at(i).length();
    }
  }
  else {
    for( int i = 0; i < this->fieldCount(); ++i ) {
      arr << _data.at(0).at(i).length();
    }
  }

  // Loop through the columns once to determine max sizes
  //-----------------------------------------------------
  for( int row = 0; row < this->rowCount(); ++row ) {
    for( int i = 0; i < this->fieldCount(); ++i ) {
      arr[i] = qMax( arr.at(i), _data.at(row).at(i).length() );
    }
  }

  // Start writing
  //--------------
  QStringList list;

  if( _containsFieldList ) {
    list.clear();
    for( int i = 0; i < _fieldNames.count(); ++i ) {
      list.append( QString( "%1" ).arg( tablePadded( _fieldNames.at(i), arr.at(i) ) ) );
    }
    result.append( QString( "|%1|\n" ).arg( list.join( "|" ) ) );

    list.clear();
    for( int i = 0; i < _fieldNames.count(); ++i ) {
      list.append( tableDiv( arr.at(i) ) );
    }
    result.append( QString( "+%1+\n" ).arg( list.join( "+" ) ) );
  }

  for( int row = 0; row < this->rowCount(); ++row ) {
    list.clear();
    for( int i = 0; i < this->fieldCount(); ++i ) {
      list.append( QString( "%1" ).arg( tablePadded( _data.at(row).at(i), arr.at(i) ) ) );
    }
    result.append( QString( "|%1|\n" ).arg( list.join( "|" ) ) );
  }

  return result;
}


QString QCsv::csvQuote( QString s ) {
  if(  s.contains( '"' ) )
    s.replace( '"', "\"\"" );

  s = QString( "\"%1\"" ).arg( s );
  return s;
}


QString QCsv::csvQuote(QStringList s , const QChar delimiter /* = ',' */ ) {
  QString result;

  for( int i = 0; i < s.count() - 1; ++i ) {
    result.append( csvQuote( s.at(i) ) );
    result.append( delimiter );
  }

  result.append( csvQuote( s.last() ) );

  return result;
}


QCsvObject::QCsvObject() : QObject(), QCsv() {
  // Do nothing.
}


QCsvObject::QCsvObject(
  const QString& filename,
  const bool containsFieldList,
  const bool stringsContainDelimiters/*  = true */,
  const QCsvMode mode /* = qCSV::LineByLine */,
  const bool checkForComment /* = false */
) : QObject(), QCsv( filename, containsFieldList, stringsContainDelimiters, mode, checkForComment ) {
  // Do nothing.
}


QCsvObject::QCsvObject( const QStringList& fieldNames ) : QObject(), QCsv( fieldNames ) {
  // Do nothing.
}


QCsvObject::QCsvObject( const QStringList& fieldNames, const QList<QStringList>& data ) : QObject(), QCsv( fieldNames, data ) {
  // Do nothing.
}


QCsvObject::~QCsvObject() {
  // Do nothing.
}


int QCsvObject::readNext() {
  int result = QCsv::readNext();
  emit nBytesRead( _currentLine.toUtf8().size() );
  return result;
}

