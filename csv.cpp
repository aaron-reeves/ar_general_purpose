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


QString CSV::writeLine( const QStringList& line, const QChar delimiter /* = ',' */ ) {
  QStringList output;

  foreach (QString value, line) {
    value.replace( "\"", "\"\"" );

    if( value.contains( QRegExp( "\"\r\n") ) || value.contains( delimiter ) || value.contains( QRegExp( "\\s+" ) ) ) {
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


qCSV::qCSV() {
  initialize();
}


qCSV::qCSV (
  const QString& filename,
  const bool containsFieldList,
  const QChar& stringToken /* = '\0' */,
  const bool stringsContainDelimiters /* = true */,
  const int readMode /* = qCsv::ReadLineByLine */,
  const bool checkForComment /* = false */
) {
  initialize();

  _srcFilename = filename;
  _stringToken = stringToken;
  _containsFieldList = containsFieldList;
  _checkForComment = checkForComment;

  if ( stringToken != '\0' )
    _usesStringToken = true;

  _stringsContainDelimiters = stringsContainDelimiters;
  _readMode = readMode;

  if( qCSV_ReadEntireFile == _readMode ) {
    this->reallyOpen( true );

    //qDebug() << "Reading entire file...";
    int fieldsRead = 0;
    while( -1 != fieldsRead ) {
      //qDebug() << fieldsRead;
      fieldsRead = readNext();
    }

    this->close();
    //qDebug() << "Done.";
  }
}


qCSV::qCSV(
  const int dummy,
  QString text,
  const bool containsFieldList,
  const QChar& stringToken /* = '\0' */,
  const bool stringsContainDelimiters /* = true */
) {
  Q_UNUSED( dummy );

  initialize();

  _srcFilename = QString(); // There is no source file.
  _stringToken = stringToken;
  _containsFieldList = containsFieldList;
  _checkForComment = false; // There is no allowance for comments here.

  if ( stringToken != '\0' )
    _usesStringToken = true;

  _stringsContainDelimiters = stringsContainDelimiters;
  _readMode = qCSV_ReadEntireFile;

  QList<QStringList> items = CSV::parseFromString( text );


  QString str;
  for( int i = 0; i < items.at(0).count(); ++i ) {
    str = items.at(0).at(i).trimmed();
    _fieldNames.append( str );
    _fieldsLookup.insert( str.toLower(), i );
  }

  for( int i = 1; i < items.count(); ++i ) {
    _data.append( items.at( i ) );
  }
}

qCSV::qCSV( const QStringList& fieldNames ) {
  initialize();
  _readMode = qCSV_ReadEntireFile;

  _containsFieldList = true;

  QString str;
  for( int i = 0; i < fieldNames.count(); ++i ) {
     str = fieldNames.at(i).trimmed();
    _fieldNames.append( str );
    _fieldsLookup.insert( str.toLower(), i );
  }
}


qCSV::qCSV( const QStringList& fieldNames, const QList<QStringList>& data ) {
  initialize();
  _readMode = qCSV_ReadEntireFile;

  _containsFieldList = true;

  QString str;
  for( int i = 0; i < fieldNames.count(); ++i ) {
     str = fieldNames.at(i).trimmed();
    _fieldNames.append( str );
    _fieldsLookup.insert( str.toLower(), i );
  }

  for( int i = 0; i < data.count(); ++i ) {
    appendRow( data.at(i) );
  }
}


void qCSV::initialize() {
  _srcFilename = "";
  _srcFile = NULL;
  _isOpen = false;

  _currentLine = "";
  _currentLineNumber = 0;
  _error = qCSV_ERROR_NONE;
  _errorMsg = "";
  _stringToken = '\0';
  _usesStringToken = false;
  _containsFieldList = false;
  _stringsContainDelimiters = true;
  _concatenateDanglingEnds = false;
  _readMode = qCSV_UnspecifiedMode;
  _eolDelimiter = " ";
  _delimiter = ',';
  _firstDataRowEncountered = false;
  _checkForComment = false;
  _nCommentRows = 0;

  _linesToSkip = 0;
  _linesSkipped = 0;
}

qCSV::qCSV( const qCSV& other ) {
  _srcFilename = other._srcFilename;
  _srcFile = NULL;
  _isOpen = other._isOpen;

  _currentLine = other._currentLine;
  _currentLineNumber = other._currentLineNumber;
  _error = other._error;
  _errorMsg = other._errorMsg;
  _stringToken = other._stringToken;
  _usesStringToken = other._usesStringToken;
  _containsFieldList = other._containsFieldList;
  _stringsContainDelimiters = other._stringsContainDelimiters;
  _concatenateDanglingEnds = other._concatenateDanglingEnds;
  _readMode = other._readMode;
  _eolDelimiter = other._eolDelimiter;
  _delimiter = other._delimiter;
  _firstDataRowEncountered = other._firstDataRowEncountered;
  _checkForComment = other._checkForComment;
  _nCommentRows = other._nCommentRows;

  _linesToSkip = other._linesToSkip;
  _linesSkipped = other._linesToSkip;

  _fieldsLookup = other._fieldsLookup;
  _fieldNames = other._fieldNames;
  _fieldData = other._fieldData;
  _data = other._data;

  if( other._isOpen && ( qCSV_ReadLineByLine == other._readMode ) ) {
    this->open();
  }
}


qCSV::~qCSV() {
  if( NULL != _srcFile ) {
    _srcFile->close();
    _isOpen = false;
    delete _srcFile;
    _srcFile = NULL;
  }

  _fieldsLookup.clear();
  _fieldNames.clear();
  _data.clear();
  _fieldData.clear();

  _srcFilename = "";
  _currentLine = "";
  _currentLineNumber = -1;
  _error = qCSV_ERROR_NONE;
  _errorMsg = "";
  _stringToken = '\0';
  _usesStringToken = false;
  _containsFieldList = false;
}


void qCSV::debug( int nLines /* = 0 */ ) {
  qDebug() << "qCSV contents:";

  qDebug() << "numFields:" << this->fieldCount();
  qDebug() << "numFieldNames:" << this->fieldNames().count();
  qDebug() << "numRows:" << this->rowCount();

  qDebug() << this->fieldNames().join( _delimiter ).prepend( "  " );

  if( qCSV_ReadLineByLine == _readMode )
    qDebug() << "(There is nothing to display)";
  else {
    if( (1 > nLines) || ( _data.count() < nLines ) )
      nLines = _data.count();

    for( int i = 0; i < nLines; ++i ) {
      qDebug() << _data.at(i).join( _delimiter ).prepend( "  " );
    }
  }
}


void qCSV::setField( const int index, const QString& val ) {
  QStringList* dataList;
  clearError();

  if( qCSV_ReadLineByLine == _readMode )
    dataList = &_fieldData;
  else
    dataList = &( _data[ _currentLineNumber ] );

  if( 0 < dataList->size() ) {
    if ( index < dataList->size() ) {
      if( qCSV_ReadLineByLine == _readMode )
        _fieldData[index] = val.trimmed();
      else
        _data[_currentLineNumber][index] = val.trimmed();
    }
    else {
      _error = qCSV_ERROR_INDEX_OUT_OF_RANGE;
      _errorMsg = "For File Linenumber: " + QString::number ( _currentLineNumber ) + ", Field index, " + QString::number ( index ) + ", out of range";
    }
  }
  else{
    _error = qCSV_ERROR_LINE_EMPTY;
    _errorMsg = "The current line, " + QString::number ( _currentLineNumber ) + " is empty.  Did you read a line first?";
  }
}



// Accessors
QString qCSV::field( const int index ){
  QStringList* dataList;
  QString ret_val = "";
  clearError();

  if( qCSV_ReadLineByLine == _readMode )
    dataList = &_fieldData;
  else
    dataList = &( _data[ _currentLineNumber ] );

  if ( dataList->size() > 0 ){
    if ( dataList->size() > index ){
      ret_val = dataList->at( index ).trimmed();
    }
    else{
      _error = qCSV_ERROR_INDEX_OUT_OF_RANGE;
      _errorMsg = "For File Linenumber: " + QString::number ( _currentLineNumber ) + ", Field index, " + QString::number ( index ) + ", out of range";
    }
  }
  else{
    _error = qCSV_ERROR_LINE_EMPTY;
    _errorMsg = "The current line, " + QString::number ( _currentLineNumber ) + " is empty.  Did you read a line first?";
  }

  return ret_val;
}


void qCSV::setField( QString fName, const QString& val ) {
  fName = fName.toLower();
  QStringList* dataList;
  clearError();

  if( qCSV_ReadLineByLine == _readMode )
    dataList = &_fieldData;
  else
    dataList = & (_data[ _currentLineNumber ] );

  if ( _containsFieldList ){
    if ( dataList->size() > 0 ){
      if ( _fieldsLookup.contains( fName ) ){
        int index = _fieldsLookup.value( fName );

        setField( index, val );
      }
      else{
        _error = qCSV_ERROR_INVALID_FIELD_NAME;
        _errorMsg = "Invalid Field Name: " + fName;
      }
    }
    else{
      _error = qCSV_ERROR_LINE_EMPTY;
      _errorMsg = "The current line, " + QString::number ( _currentLineNumber ) + " is empty.  Did you read a line first?";
    }
  }
  else {
    _error = qCSV_ERROR_NO_FIELDLIST;
    _errorMsg = "The current settings do not include a field list.";
  }
}

QString qCSV::field( QString fName ){
  fName = fName.toLower();

  QStringList* dataList;
  QString ret_val = "";
  clearError();

  if( qCSV_ReadLineByLine == _readMode )
    dataList = &_fieldData;
  else
    dataList = &( _data[ _currentLineNumber ] );

  if ( _containsFieldList ){
    if ( dataList->size() > 0 ){
      if ( _fieldsLookup.contains( fName ) ){
        int index = _fieldsLookup.value( fName );

        ret_val = field( index );
      }
      else{
        _error = qCSV_ERROR_INVALID_FIELD_NAME;
        _errorMsg = "Invalid Field Name: " + fName;
      }
    }
    else{
      _error = qCSV_ERROR_LINE_EMPTY;
      _errorMsg = "The current line, " + QString::number ( _currentLineNumber ) + " is empty.  Did you read a line first?";
    }
  }
  else {
    _error = qCSV_ERROR_NO_FIELDLIST;
    _errorMsg = "The current settings do not include a field list.";
  }

  return ret_val;
}


void qCSV::setField( const int index, const int rowNumber, const QString& val ) {
  if( rowNumber > (_data.count() - 1) ) {
    _error = qCSV_ERROR_INDEX_OUT_OF_RANGE;
    _errorMsg = QString( "There is no row %1" ).arg( rowNumber );
  }
  else {
    _currentLineNumber = rowNumber;
    setField( index, val );
  }
}

void qCSV::setField( QString fName, const int rowNumber, const QString& val ) {
  if( rowNumber > (_data.count() - 1) ) {
    _error = qCSV_ERROR_INDEX_OUT_OF_RANGE;
    _errorMsg = QString( "There is no row %1" ).arg( rowNumber );
  }
  else {
    _currentLineNumber = rowNumber;
    setField( fName, val );
  }
}

QString qCSV::field( const int index, const int rowNumber ) {
  if( rowNumber > (_data.count() - 1) ) {
    _error = qCSV_ERROR_INDEX_OUT_OF_RANGE;
    _errorMsg = QString( "There is no row %1" ).arg( rowNumber );
    return QString();
  }
  else {
    _currentLineNumber = rowNumber;
    return field( index );
  }
}

QString qCSV::field( QString fName, const int rowNumber ) {
  if( rowNumber > (_data.count() - 1) ) {
    _error = qCSV_ERROR_INDEX_OUT_OF_RANGE;
    _errorMsg = QString( "There is no row %1" ).arg( rowNumber );
    return QString();
  }
  else {
    _currentLineNumber = rowNumber;
    return field( fName );
  }
}

QVariantList qCSV::fields( QString fName ) {
  fName = fName.toLower();
  QVariantList result;
  clearError();

  if ( _containsFieldList ){
    if ( _fieldsLookup.contains( fName ) ){
      int index = _fieldsLookup.value( fName );

      result = fields( index );
    }
    else{
      _error = qCSV_ERROR_INVALID_FIELD_NAME;
      _errorMsg = "Invalid Field Name: " + fName;
    }
  }
  else {
    _error = qCSV_ERROR_NO_FIELDLIST;
    _errorMsg = "The current settings do not include a field list.";
  }

  return result;
}


QVariantList qCSV::fields( int index ) {
  QVariantList result;

  if( qCSV_ReadLineByLine == _readMode )
    result.append( this->field( index ) );
  else {
    for( int i = 0; i < _data.count(); ++ i )
      result.append( _data.at(i).at(index) );
  }

  return result;
}


QString qCSV::fieldName( int index ){
  QString ret_val = "";
  if ( _containsFieldList ){
    ret_val = _fieldNames.at( index );
  }

  return ret_val;
}


bool qCSV::renameFields( const QStringList& newFieldNames ) {
  if( newFieldNames.count() != _fieldNames.count() )
    return false;
  else {
    bool result = true;
    for( int i = 0; i < newFieldNames.count(); ++i ) {
      result = result && renameField( _fieldNames.at(i), newFieldNames.at(i) );
    }
    return result;
  }
}


int qCSV::fieldIndexOf( QString fieldName ) {
  fieldName = fieldName.trimmed();
  int result = -1;

  for( int i = 0; i < _fieldNames.count(); ++i ) {
    if( 0 == fieldName.compare( _fieldNames.at(i), Qt::CaseInsensitive ) ) {
      result = i;
      break;
    }
  }

  return result;
}


bool qCSV::containsFieldName( QString fieldName ) {
  return _fieldsLookup.contains( fieldName.trimmed().toLower() );
}


bool qCSV::renameField( QString oldName, QString newName ) {
  newName = newName.trimmed();
  oldName = oldName.trimmed();

  if( oldName.toLower() == newName.toLower() ) {
    // This could be a change of case.  Go through the motions, just in case.
    int idx = fieldIndexOf( oldName );

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
    int idx = fieldIndexOf( oldName );

    _fieldNames.replace( idx, newName );
    int hashVal = _fieldsLookup.value( oldName.toLower() );
    _fieldsLookup.remove( oldName.toLower() );
    _fieldsLookup.insert( newName.toLower(), hashVal );

    return true;
  }
}


bool qCSV::appendField( const QString& fieldName ) {
  // FIXME: For now, this function only works for qCSV_ReadEntireFile.
  // Think about how it might work for qCSV_ReadLineByLine.
  Q_ASSERT( qCSV_ReadEntireFile == _readMode );

  if( qCSV_ReadLineByLine == _readMode )
    return false;
  else {
    _fieldNames.append( fieldName.trimmed() );
    _fieldsLookup.insert( fieldName.trimmed().toLower(), _fieldNames.count() - 1 );

    for( int i = 0; i < _data.count(); ++i ) {
      _data[i].append( "" );
    }

    return true;
  }
}


bool qCSV::removeField( const QString& fieldName ) {
  // FIXME: For now, this function only works for qCSV_ReadEntireFile.
  // Think about how it might work for qCSV_ReadLineByLine.
  Q_ASSERT( qCSV_ReadEntireFile == _readMode );

  if( qCSV_ReadLineByLine == _readMode )
    return false;
  else {
    if( _fieldsLookup.contains( fieldName ) )
      return removeField( _fieldsLookup.value( fieldName ) );
    else {
      _error = qCSV_ERROR_INVALID_FIELD_NAME;
      _errorMsg = "Invalid Field Name: " + fieldName;
      return false;
    }
  }
}


bool qCSV::removeField( const int fieldNumber ) {
  // FIXME: For now, this function only works for qCSV_ReadEntireFile.
  // Think about how it might work for qCSV_ReadLineByLine.
  Q_ASSERT( qCSV_ReadEntireFile == _readMode );

  if( qCSV_ReadLineByLine == _readMode )
    return false;
  else if( fieldNumber < _fieldNames.count() ) {
    _fieldNames.removeAt( fieldNumber );

    QList<QString> keys = _fieldsLookup.keys( fieldNumber );
    for( int i = 0; i < keys.count(); ++i ) {
      _fieldsLookup.remove( keys.at(i) );
    }

    for( int i = 0; i < _data.count(); ++i ) {
      _data[i].removeAt( fieldNumber );
    }
    return true;
  }
  else {
    _error = qCSV_ERROR_INDEX_OUT_OF_RANGE;
    _errorMsg = QString( "Invalid Field Number: %1" ).arg( fieldNumber );
    return false;
  }
}


bool qCSV::appendRow( const QStringList& values ) {
  // FIXME: For now, this function only works for qCSV_ReadEntireFile.
  // Think about how it might work for qCSV_ReadLineByLine.
  Q_ASSERT( qCSV_ReadEntireFile == _readMode );

  if( qCSV_ReadLineByLine == _readMode )
    return false;
  else if( values.count() == _fieldNames.count() ) {
    QStringList trimmedVals;
    for( int i = 0; i < values.count(); ++i ) {
      trimmedVals.append( values.at(i).trimmed() );
    }

    _data.append( trimmedVals );
    return true;
  }
  else {
    _error = qCSV_ERROR_INDEX_OUT_OF_RANGE;
    _errorMsg = QString( "Field count mismatch: %1 <> %2" ).arg( values.count() ).arg( _fieldNames.count() );
    return false;
  }
}


QStringList qCSV::fieldValues( const QString& fieldName, const bool unique /* = false */ ) {
  QStringList result;

  if ( _fieldsLookup.contains( fieldName ) ){
    int index = _fieldsLookup.value( fieldName );

    result  = fieldValues( index, unique );
  }
  else{
    _error = qCSV_ERROR_INVALID_FIELD_NAME;
    _errorMsg = "Invalid Field Name: " + fieldName;
  }

  return result;
}


QStringList qCSV::fieldValues( const int fieldNumber, const bool unique /* = false */ ) {
  QStringList result;

  // FIXME: For now, this function only works for qCSV_ReadEntireFile.
  // Think about how it might work for qCSV_ReadLineByLine.
  Q_ASSERT( qCSV_ReadEntireFile == _readMode );

  if( qCSV_ReadEntireFile == _readMode ) {
    if( fieldNumber > (_fieldNames.count() - 1) ) {
      _error = qCSV_ERROR_INDEX_OUT_OF_RANGE;
      _errorMsg = QString( "There is no column %1" ).arg( fieldNumber );
    }
    else {
      for( int i = 0; i < _data.count(); ++i ) {

        if( unique ) {
          if( !result.contains( _data.at( i ).at( fieldNumber ) ) )
            result.append( _data.at( i ).at( fieldNumber ) );
        }
        else {
          result.append( _data.at( i ).at( fieldNumber ) );
        }
      }
    }
  }

  return result;
}


qCSV qCSV::filter( const QString& fieldName, const QString& value, const Qt::CaseSensitivity cs /* = Qt::CaseSensitive */ ) {
  qCSV result( this->fieldNames() );

  for( int i = 0; i < _data.count(); ++i ) {
    if( 0 == value.compare( this->field( fieldName, i ), cs ) ) {
      result.appendRow( _data.at(i) );
    }
  }

  result.toFront();
  return result;
}


int qCSV::rowCount(){
  int result;

  if( _readMode == qCSV_ReadLineByLine )
    result = -1;
  else
    result = _data.count();

  return result;
}


QStringList qCSV::writeLine( const QStringList& line ) {
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


bool qCSV::writeFile( const QString &filename, const QString &codec ) {
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
void qCSV::setContainsFieldList ( bool setVal ){
  clearError();
  _containsFieldList = setVal;
}


void qCSV::setFilename( QString filename ){
  clearError();
  if( NULL != _srcFile ) {
    _srcFile->close();
    _isOpen = false;
    delete _srcFile;
    _srcFile = NULL;
  }

  _srcFilename = filename;
}


bool qCSV::open() {
  return reallyOpen( false );
}


bool qCSV::reallyOpen( const bool force ) {
  clearError();

  if( !force && ( _readMode == qCSV_ReadEntireFile ) ) {
    return true;
  }

  if( !_isOpen ) {
    if( NULL == _srcFile ) {
      _srcFile = new QFile( _srcFilename );
    }

    _isOpen = _srcFile->open( QIODevice::ReadOnly | QIODevice::Text );

    if( !_isOpen ) {
      _error = qCSV_ERROR_OPEN;
      _errorMsg = "Can not open the source file";
      delete _srcFile;
      _srcFile = NULL;
      _isOpen = false;
    }
  }

  return _isOpen;
}


void qCSV::close(){
  clearError();

  if( NULL != _srcFile ) {
    _srcFile->close();
    delete _srcFile;
    _srcFile = NULL;
    _isOpen = false;
  }
}


void qCSV::toFront() {
  _currentLineNumber = -1;
}


//  Returns the number of fields in the row, or -1 at the end of the file.
int qCSV::moveNext(){
  if( qCSV_ReadLineByLine == _readMode ) {
    return readNext();
  }
  else {
    ++_currentLineNumber;

    if( _currentLineNumber < _data.count() ) {
      _fieldData.clear();
      _fieldData = _data.at( _currentLineNumber );
      return _data.at( _currentLineNumber ).count();
    }
    else {
      return -1;
    }
  }
}


//  Also acts as movefirst...cause a read of a line of data from the csv file.
//  Returns the number of fields read, or -1 at the end of the file.
int qCSV::readNext() {
  int ret_val = -1;
  int index = 0;
  QStringList fieldList;
  QString tmp;
  int nQuotes;

  clearError();

  _fieldData.clear();

  _currentLine.clear();
  nQuotes = 0;

  // The do loop handles situations where end-of-line
  // characters are encountered inside quote marks.
  do {
    tmp = _srcFile->readLine();

    // FIXME: Is there a better way to handle delimiters here?
    if( !_delimiter.isSpace() )
      tmp = tmp.trimmed();

    if( !_currentLine.isEmpty() )
      _currentLine.append( _eolDelimiter );

    _currentLine.append( tmp );
    nQuotes = nQuotes + tmp.count( '\"' );
  } while( 0 != nQuotes%2 );


  if( !_currentLine.isEmpty() ) {
    _currentLineNumber++;

    // If the user wants to skip any lines, do that here.
    if( _linesSkipped < _linesToSkip ) {
      ++_linesSkipped;
      return readNext();
    }

    // This next block handles the situation where the file
    // begins with a header (indicated by lines that start with #).
    // These lines should simply be skipped.
    if( _checkForComment && isCommentLine( _currentLine ) ) {
      ++_nCommentRows;
      return readNext();
    }

    //qDebug() << _currentLine;
    //qDebug() << "Check 0" << _stringsContainDelimiters << _delimiter;

    if ( _stringsContainDelimiters )
      fieldList = CSV::parseLine( _currentLine, _delimiter );
    else
      fieldList = _currentLine.split( _delimiter );

    for ( int i = 0; i < fieldList.size(); i++ ){
      index++;
      QString tempString = fieldList.at(i);
      tempString = tempString.trimmed();

      if ( _usesStringToken ){
        tempString = tempString.replace( _stringToken, "" );
      }

      if ( _containsFieldList && ( !_firstDataRowEncountered ) ){
        _fieldNames.append( tempString );
        _fieldsLookup.insert( tempString.toLower(), i );
      }
      else if ( _containsFieldList && _concatenateDanglingEnds ){
        if ( i < _columnCount )
          _fieldData.insert( i, tempString );
        else{
          QString tempStr = _fieldData[ _columnCount ];
          tempStr += ", " + tempString;
          _fieldData.insert( _columnCount, tempStr );
        }
      }
      else
        _fieldData.insert( i, tempString );
    }

    if ( _containsFieldList && ( !_firstDataRowEncountered ) ){
      _columnCount = index;
      _fieldData.clear();
      _firstDataRowEncountered = true;
      index = ret_val = readNext();
    }
    else {
      ret_val = index;
      if( qCSV_ReadEntireFile == _readMode ) {
        _data.append( _fieldData );
      }
    }

    /*  Save this, below, for a switch to check field list lengths later on....some csv formats,
        such as those created by Microsoft products, unfortunately, make the strings 
        too short if the last few fields are empty...i.e. they don't save empty strings, or empty 
        comma separated strings, (such as ",,,,") if they are at the end of a line, and have
        no data somewhere after them...bad news, if you want to verify fields....

        (AR: Not sure about the above: it no longer seems to be the case.)
     */

    /*
    if ( _containsFieldList )
    {
     if ( ( ( index != _fields.size() ) || ( _columnCount != _fields.size()) ) && ((_columnCount - 1) > index) )
     {
       ret_val = -1;
       //  NOTE:  Could be caused by a line exceeding _qCSV_MAX_LINE_LENGTH also...
       _error = qCSV_ERROR_INVALID_FIELD_COUNT;
       _errorMsg = "At File Linenumber: " + QString::number ( _currentLineNumber ) + "  Invalid field count while reading data from file.  Should have been" + QString::number ( _fields.size() ) + " but was " + QString::number ( index ) + ".  Check   _qCSV_MAX_LINE_LENGTH, and validate the line's content in the file" ;
     };
    }
    else
    {
     if ( _columnCount > 0 )
     {
       if ( ( index != _columnCount ) && (( _columnCount - 1 ) > index )  )
       {
         ret_val = -1;
         //  NOTE:  Could be caused by a line exceeding _qCSV_MAX_LINE_LENGTH also...
         _error = qCSV_ERROR_INVALID_FIELD_COUNT;
         _errorMsg = "At File Linenumber: " + QString::number ( _currentLineNumber ) + "  Invalid field count while reading data from file.  Should have been" + QString::number ( _columnCount ) + " but was " + QString::number ( index ) + ".  Check   _qCSV_MAX_LINE_LENGTH, and validate the line's content in the file" ;
       };
     }
     else
     {
       _columnCount = index;
     };    
    };
    */
  }
  else {
    if ( !_srcFile->atEnd() ){
      _error = qCSV_ERROR_BAD_READ;
      _errorMsg = "Can not read next line.  Last line number was: " + QString::number ( _currentLineNumber ) + ".  Are we at the end of the file?";
    }
  }

  return ret_val;
}


void qCSV::setStringToken ( QChar token ){
  _stringToken = token;
  clearError();

  if ( _stringToken != '\0' )
    _usesStringToken = true;
  else
    _usesStringToken = false;
}


// Protected members
void qCSV::clearError(){
  _error = qCSV_ERROR_NONE;
  _errorMsg = "";
}


bool qCSV::isCommentLine( const QString& line ) {
  bool result = ( '#' == line.at(0) );
  return result;
}


QCsvObject::QCsvObject() : QObject(), qCSV() {
  // Do nothing.
}


QCsvObject::QCsvObject(
  const QString& filename,
  const bool containsFieldList,
  const QChar& stringToken /* = '\0' */,
  const bool stringsContainDelimiters/*  = true */,
  const int readMode /* = qCSV::qCSV_ReadLineByLine */,
  const bool checkForComment /* = false */
) : QObject(), qCSV( filename, containsFieldList, stringToken, stringsContainDelimiters, readMode, checkForComment ) {
  // Do nothing.
}


QCsvObject::QCsvObject(
  const int dummy,
  QString text,
  const bool containsFieldList,
  const QChar& stringToken /* = '\0' */,
  const bool stringsContainDelimiters /* = true */
) : QObject(), qCSV( dummy, text, containsFieldList, stringToken, stringsContainDelimiters ) {
  // Do nothing.
}


QCsvObject::QCsvObject( const QStringList& fieldNames ) : QObject(), qCSV( fieldNames ) {
  // Do nothing.
}


QCsvObject::QCsvObject( const QStringList& fieldNames, const QList<QStringList>& data ) : QObject(), qCSV( fieldNames, data ) {
  // Do nothing.
}


QCsvObject::~QCsvObject() {
  // Do nothing.
}


int QCsvObject::readNext() {
  int result = qCSV::readNext();
  emit nBytesRead( _currentLine.toUtf8().size() );
  return result;
}

