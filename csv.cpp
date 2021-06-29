/*
csv.h/cpp
----------
Begin: 2014/04/11
Author (modified version): Aaron Reeves <aaron.reeves@naadsm.org>
------------------------------------------------------------------------

Original code (name space CSV) by Naohiro Hasegawa, https://github.com/hnaohiro/qt-csv/blob/master/csv.h
Original code (class qCSV) believed to be by Shaun Case, Animal Population Health Institute, Colorado State University.

Modified version Copyright (C) 2014 - 2021 Aaron Reeves

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/


#include "csv.h"

#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QRegExp>
#include <QDebug>

#include <ar_general_purpose/strutils.h>
#include <ar_general_purpose/qcout.h>

QStringList CSV::parseLine( const QString& string, const QChar delimiter /* = ',' */ ) {
  enum State {Normal, Quote} state = Normal;
  QStringList line;
  QString temp, value;

  // FIXME: Is there a better way to handle delimiters here?
  if( !delimiter.isSpace())
    temp = string.trimmed();
  else
    temp = string;

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


QString initString( const QString &string ){
  QString result = string;
  result.replace(QLatin1String("\r\n"), QLatin1String("\n"));

  if( !result.isEmpty() && ( result.at(result.size()-1) != '\n' ) ) {
    result += '\n';
  }
  return result;
}


QList<QStringList> CSV::parseFromString( const QString &string , const QChar delimiter /* = ',' */ ){
  return parse( initString(string), delimiter );
}


QList<QStringList> CSV::parseFromFile( const QString &filename, const QChar delimiter /* = ',' */, const QString &codec ){
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


QStringList CSV::csvStringList( const QStringList& elements, const QChar delimiter /* = ',' */, const StringCase stringCase /* = OriginalCase */ ) {
  QStringList output;

  foreach (QString value, elements) {
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

    value.replace( QLatin1String("\""), QLatin1String("\"\"") );

    if( value.contains( QRegExp( "\"\r\n" ) ) || value.contains( delimiter ) || value.contains( QRegExp( "\\s+" ) ) ) {
      output << ("\"" + value + "\"");
    } else {
      output << value;
    }
  }

  return output;
}


QString CSV::writeLine( const QStringList& elements, const QChar delimiter /* = ',' */, const StringCase stringCase /* = OriginalCase */ ) {
  QStringList list = csvStringList( elements, delimiter, stringCase );

  return( list.join( delimiter ) );
}


bool CSV::write( const QList<QStringList>& data, const QString &filename, const QChar delimiter /* = ',' */, const QString& codec ) {
  QFile file(filename);
  if (!file.open( QFile::WriteOnly | QFile::Text )) {
    return false;
  }

  QTextStream out(&file);
  if( !codec.isEmpty() )
    out.setCodec(codec.toLatin1());

  foreach (const QStringList &line, data) {
    QString output = writeLine( line, delimiter, OriginalCase );
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
    const QString& text,
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
  _isOpen = true;

  QList<QStringList> items = CSV::parseFromString( text, _delimiter );

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

  _isOpen = true;
}


QCsv::QCsv( const QStringList& fieldNames, const QList<QStringList>& data ) {
  initialize();

  setFieldNames( fieldNames );

  for( int i = 0; i < data.count(); ++i ) {
    append( data.at(i) );
  }

  _isOpen = true;
}


QCsv::QCsv( const QStringList& fieldNames, const QStringList& data ) {
  initialize();
  setFieldNames( fieldNames );

  append( data );

  _isOpen = true;
}


QCsv::QCsv( const QList<QStringList>& data ) {
  initialize();
  _mode = EntireFile;
  _containsFieldList = false;

  for( int i = 0; i < data.count(); ++i ) {
    append( data.at(i) );
  }

  _isOpen = true;
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
  _srcFilename = QString();
  _srcFile = nullptr;
  _isOpen = false;

  clearError();

  _currentLine = QString();
  _currentRowNumber = -1;
  _containsFieldList = true;
  _stringsContainDelimiters = true;
  _mode = UnspecifiedMode;
  _eolDelimiter = ' ';
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
  _srcFile = nullptr;
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

  if( nullptr != _srcFile ) {
    _srcFile->close();
    delete _srcFile;
    _srcFile = nullptr;
  }
}


void QCsv::debug( int nLines /* = 0 */ ) {
  qDb() << "qCSV contents:";

  qDb() << "numFields:" << this->fieldCount();
  qDb() << "numFieldNames:" << this->fieldNames().count();
  qDb() << "numRows:" << this->rowCount();
  qDb() << "error code:" << this->error();
  qDb() << "error message:" << this->errorMsg();

  qDb() << this->fieldNames().join( _delimiter ).prepend( "  " );

  if( this->rowCount() > 0 ) {
    if( LineByLine == _mode )
      qDb() << "(There is nothing to display)";
    else {
      if( (1 > nLines) || ( _data.count() < nLines ) )
        nLines = _data.count();

      for( int i = 0; i < nLines; ++i ) {
        qDb() << _data.at(i).join( _delimiter ).prepend( "  " );
      }
    }
  }
  else {
    qDb() << "(Nothing can be displayed)";
  }
}


// Accessors
QString QCsv::currentRow() {
  clearError();
  switch( _mode ) {
    case LineByLine:
      return _currentLine;
    case EntireFile:
      return CSV::writeLine( _data.at( _currentRowNumber ) );
    default:
      return QString();
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
  Q_ASSERT( EntireFile == _mode );
  clearError();

  if( ( 0 > idx ) || ( _data.count() < idx ) ) {
    _error = ERROR_INDEX_OUT_OF_RANGE;
    return QStringList();
  }
  else {
    return _data.at( idx );
  }
}


QStringList QCsv::rowData( const int idx ) const {
  Q_ASSERT( EntireFile == _mode );

  if( ( 0 > idx ) || ( _data.count() < idx ) ) {
    return QStringList();
  }
  else {
    return _data.at( idx );
  }
}


QString QCsv::field( const int index ){
  QStringList* dataList;
  QString ret_val;
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
  QString ret_val;
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
    _errorMsg = QStringLiteral("The current settings do not include a field list.");
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
    _errorMsg = QStringLiteral("The current settings do not include a field list.");
    result = false;
  }

  return result;
}


bool QCsv::setField( const int index, const int rowNumber, const QString& val ) {
  // This function will not work with qCSV_LineByLine mode.
  Q_ASSERT( EntireFile == _mode );
  clearError();
  bool result = true; // until shown otherwise.

  if( LineByLine == _mode ) {
    setError( ERROR_WRONG_MODE, QStringLiteral("Only EntireFile mode may be used with this function.") );
    result = false;
  }
  else if( rowNumber > (_data.count() - 1) ) {
    _error = ERROR_INDEX_OUT_OF_RANGE;
    _errorMsg = QStringLiteral( "There is no row %1" ).arg( rowNumber );
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
  Q_ASSERT( EntireFile == _mode );
  clearError();
  bool result = true; // until shown otherwise.

  if( LineByLine == _mode ) {
    setError( ERROR_WRONG_MODE, QStringLiteral("Only EntireFile mode may be used with this function.") );
    result = false;
  }
  else if ( rowNumber > (_data.count() - 1) ) {
    _error = ERROR_INDEX_OUT_OF_RANGE;
    _errorMsg = QStringLiteral( "There is no row %1" ).arg( rowNumber );
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
  Q_ASSERT( EntireFile == _mode );
  clearError();

  if( LineByLine == _mode ) {
    setError( ERROR_WRONG_MODE, QStringLiteral("Only EntireFile mode may be used with this function.") );
    return QString();
  }
  else if( rowNumber > (_data.count() - 1) ) {
    _error = ERROR_INDEX_OUT_OF_RANGE;
    _errorMsg = QStringLiteral( "There is no row %1" ).arg( rowNumber );
    return QString();
  }
  else {
    _currentRowNumber = rowNumber;
    return field( index );
  }
}

QString QCsv::field( const QString& fieldName, const int rowNumber ) {
  // This function will not work with qCSV_LineByLine mode.
  Q_ASSERT( EntireFile == _mode );
  clearError();

  if( LineByLine == _mode ) {
    setError( ERROR_WRONG_MODE, QStringLiteral("Only EntireFile mode may be used with this function.") );
    return QString();
  }
  else if( rowNumber > (_data.count() - 1) ) {
    _error = ERROR_INDEX_OUT_OF_RANGE;
    _errorMsg = QStringLiteral( "There is no row %1" ).arg( rowNumber );
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


QString QCsv::fieldName( const int index ) const {
  QString ret_val;
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
  Q_ASSERT( EntireFile == _mode );
  clearError();

  if( LineByLine == _mode ) {
    setError( ERROR_WRONG_MODE, QStringLiteral("Only EntireFile mode may be used with this function.") );
    return false;
  }
  else {
    _fieldNames.append( fieldName.trimmed() );
    _fieldsLookup.insert( fieldName.trimmed().toLower(), _fieldNames.count() - 1 );

    for( int i = 0; i < _data.count(); ++i ) {
      _data[i].append( QString() );
    }

    return true;
  }
}


bool QCsv::removeField( const QString& fieldName ) {
  // This function will not work with qCSV_LineByLine mode.
  Q_ASSERT( EntireFile == _mode );
  clearError();

  if( LineByLine == _mode ) {
    setError( ERROR_WRONG_MODE, QStringLiteral("Only EntireFile mode may be used with this function.") );
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
  Q_ASSERT( EntireFile == _mode );
  clearError();

  if( LineByLine == _mode ) {
    setError( ERROR_WRONG_MODE, QStringLiteral("Only EntireFile mode may be used with this function.") );
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
    _errorMsg = QStringLiteral( "Invalid Field Number: %1" ).arg( index );
    return false;
  }
}


bool QCsv::append( const QStringList& values ) {
  // This function only works for qCSV_EntireFile.
  Q_ASSERT( EntireFile == _mode );
  clearError();

  if( LineByLine == _mode ) {
    setError( ERROR_WRONG_MODE, QStringLiteral("Only EntireFile mode may be used with this function.") );
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
    _errorMsg = QStringLiteral( "Field count mismatch: %1 <> %2" ).arg( values.count() ).arg( fieldCount() );
    return false;
  }
}


bool QCsv::identicalFieldNames( const QStringList& otherNames ) const {
  bool result = ( this->fieldCount() == otherNames.count() );

  if( result ) {
    for( int i = 0; i < this->fieldCount(); ++i ) {
      if( this->fieldName( i ) != otherNames.at(i) ) {
        result = false;
        break;
      }
    }
  }

  return result;
}


// Add the contents of other to this.
bool QCsv::append( const QCsv& other ) {
  Q_ASSERT( EntireFile == _mode );
  clearError();

  if( EntireFile != _mode ) {
    setError( ERROR_WRONG_MODE, QStringLiteral("Only EntireFile mode may be used with this function.") );
    return false;
  }

  if( !this->identicalFieldNames( other.fieldNames() ) ) {
    setError( ERROR_INVALID_FIELD_NAME, QStringLiteral("Field names do not match.") );
    return false;
  }

  bool result = true;
  for( int i = 0; i < other.rowCount(); ++i ) {
    result = ( result && this->append( other.rowData( i ) ) );
  }

  return result;
}


// Add items from other that do not already appear in structure to this structure.
bool QCsv::merge( const QCsv& other ) {
  Q_ASSERT( EntireFile == _mode );
  clearError();

  if( EntireFile != _mode ) {
    setError( ERROR_WRONG_MODE, QStringLiteral("Only EntireFile mode may be used with this function.") );
    return false;
  }


  if( !this->identicalFieldNames( other.fieldNames() ) ) {
    setError( ERROR_INVALID_FIELD_NAME, QStringLiteral("Field names do not match.") );
    return false;
  }

  QSet<QString> haystack;
  foreach( QStringList myData, _data ) {
    QString data = myData.join( '|' ) ;
    haystack.insert( data );
  }

  bool result = true;
  for( int i = 0; i < other.rowCount(); ++i ) {
    QString needle =  other.rowData(i).join( '|' );
    if( !haystack.contains( needle ) ) {
      result = ( result && this->append( other.rowData( i ) ) );
    }
  }

  return result;
}



QStringList QCsv::fieldValues( const QString& fieldName, const bool unique /* = false */ ) {
  QStringList result;
  clearError();

  if( !_containsFieldList ) {
    _error = ERROR_NO_FIELDLIST;
    _errorMsg = QStringLiteral("The current settings do not include a field list.");
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
      _errorMsg = QStringLiteral( "There is no column %1" ).arg( index );
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


QCsv QCsv::distinct() {
  // This function will not work with qCSV_LineByLine mode.
  Q_ASSERT( EntireFile == _mode );
  clearError();

  QCsv result;
  if( LineByLine == _mode ) {
    setError( ERROR_WRONG_MODE, QStringLiteral("Only EntireFile mode may be used with this function.") );
    result.setError( ERROR_WRONG_MODE, QStringLiteral("Filtered CSVs can only be created from CSVs in EntireFile mode.") );
  }
  else {
    result = QCsv( this->fieldNames() );

    QSet<QStringList> data;

    for( int i = 0; i < _data.count(); ++i ) {
      if( !data.contains( this->rowData( i ) ) ) {
        data.insert( this->rowData( i ) );
        result.append( _data.at(i) );
      }
    }
  }

  result.toFront();
  return result;
}



QCsv QCsv::filter( const int index, const QString& value, const Qt::CaseSensitivity cs /* = Qt::CaseSensitive */ ) {
  // This function will not work with qCSV_LineByLine mode.
  Q_ASSERT( EntireFile == _mode );
  clearError();

  QCsv result;
  if( LineByLine == _mode ) {
    setError( ERROR_WRONG_MODE, QStringLiteral("Only EntireFile mode may be used with this function.") );
    result.setError( ERROR_WRONG_MODE, QStringLiteral("Filtered CSVs can only be created from CSVs in EntireFile mode.") );
  }
  else {
    result = QCsv( this->fieldNames() );

    for( int i = 0; i < _data.count(); ++i ) {
      if( 0 == value.compare( this->field( index, i ), cs ) ) {
        result.append( _data.at(i) );
      }
    }
  }

  result.toFront();
  return result;
}


QCsv QCsv::filter( const QString& fieldName, const QString& value, const Qt::CaseSensitivity cs /* = Qt::CaseSensitive */ ) {
  QCsv result;

  // This function will not work with qCSV_LineByLine mode.
  Q_ASSERT( EntireFile == _mode );
  clearError();

  if( LineByLine == _mode ) {
    setError( ERROR_WRONG_MODE, QStringLiteral("Only EntireFile mode may be used with this function.") );
    result.setError( ERROR_WRONG_MODE, QStringLiteral("Filtered CSVs can only be created from CSVs in EntireFile mode.") );
  }
  else if( !_containsFieldList ) {
    _error = ERROR_NO_FIELDLIST;
    _errorMsg = QStringLiteral("The current settings do not include a field list.");
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


QCsv QCsv::sorted( const int index ) {
  // This function will not work with qCSV_LineByLine mode.
  Q_ASSERT( EntireFile == _mode );
  clearError();

  QCsv result;
  if( LineByLine == _mode ) {
    setError( ERROR_WRONG_MODE, QStringLiteral("Only EntireFile mode may be used with this function.") );
    result.setError( ERROR_WRONG_MODE, QStringLiteral("Filtered CSVs can only be created from CSVs in EntireFile mode.") );
  }
  else {
    result = QCsv( this->fieldNames() );

    QStringList values = this->fieldValues( index, false );
    QMultiHash<QString, int> mhash;
    for( int i = 0; i < values.count(); ++i ) {
      mhash.insert( values.at(i), i );
    }

    QStringList sortOrder = mhash.keys();
    std::sort( sortOrder.begin(), sortOrder.end() );

    for( int i = 0; i < sortOrder.count(); ++i ) {
      QList<int> rows = mhash.values( sortOrder.at(i) );
      std::sort( rows.begin(), rows.end() );
      for( int j = 0; j < rows.count(); ++j ) {
        result.append( _data.at( rows.at(j) ) );
      }
    }
  }

  result.toFront();
  return result;
}


QCsv QCsv::sorted( const QString& fieldName ) {
  QCsv result;

  // This function will not work with qCSV_LineByLine mode.
  Q_ASSERT( EntireFile == _mode );
  clearError();

  if( LineByLine == _mode ) {
    setError( ERROR_WRONG_MODE, QStringLiteral("Only EntireFile mode may be used with this function.") );
    result.setError( ERROR_WRONG_MODE, QStringLiteral("Filtered CSVs can only be created from CSVs in EntireFile mode.") );
  }
  else if( !_containsFieldList ) {
    _error = ERROR_NO_FIELDLIST;
    _errorMsg = QStringLiteral("The current settings do not include a field list.");
    result.setError( _error, _errorMsg );
  }
  else if( !_fieldsLookup.contains( fieldName.trimmed().toLower() ) ) {
    _error = ERROR_INVALID_FIELD_NAME;
    _errorMsg = "Invalid Field Name: " + fieldName;
     result.setError( _error, _errorMsg );
  }
  else {
    result = sorted( _fieldsLookup.value( fieldName.trimmed().toLower() ) );
  }

  return result;
}


int QCsv::fieldCount() const {
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
    setError( ERROR_WRONG_MODE, QStringLiteral("Only EntireFile mode may be used with this function.") );
    result = -1;
  }
  else
    result = _data.count();

  return result;
}

int QCsv::rowCount() const {
  int result;

  if( LineByLine == _mode )
    result = -1;
  else
    result = _data.count();

  return result;
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
    output = CSV::csvStringList( header, this->delimiter(), CSV::OriginalCase );
    out << output.join( this->delimiter() ) << "\r\n";
  }

  // Then write the data.
  foreach (const QStringList &line, _data) {
    output = CSV::csvStringList( line, this->delimiter(), CSV::OriginalCase );
    out << output.join( this->delimiter() ) << "\r\n";
  }


  file.close();

  return true;
}


bool QCsv::displayTable( QTextStream* stream ) {
  stringListListAsTable( _data, stream, true );
  return true;
}


//  Mutators
void QCsv::setContainsFieldList ( const bool setVal ){
  clearError();
  _containsFieldList = setVal;
}


bool QCsv::setLinesToSkip( const int val ) {
  if( 0 > val ) {
    setError( ERROR_INDEX_OUT_OF_RANGE, QStringLiteral("Number of lines to skip must be 0 or more.") );
    return false;
  }
  else {
    _linesToSkip = val;
    return true;
  }
}


void QCsv::setFilename( const QString& filename ){
  clearError();
  if( nullptr != _srcFile ) {
    _srcFile->close();
    _isOpen = false;
    delete _srcFile;
    _srcFile = nullptr;
  }

  _srcFilename = filename;
}


bool QCsv::open() {
  if( EntireFile != mode() ) {
    _isOpen = openFileAndReadHeader();
  }
  else if( this->_containsFieldList &&  !( this->_fieldNames.isEmpty() && this->_data.isEmpty() ) ) {
    _isOpen = true;
  }
  else if( !this->_containsFieldList && !this->_data.isEmpty() ) {
    _isOpen = true;
  }
  else if( !isOpen() ) {
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
  else {
    // The object is aleady open, so there is no need to do anything else.
  }

  return _isOpen;
}


bool QCsv::openFileAndReadHeader() {
  clearError();

  if( nullptr != _srcFile ) {
    delete _srcFile;
  }

  _srcFile = new QFile( _srcFilename );

  bool result = _srcFile->open( QIODevice::ReadOnly | QIODevice::Text );

  if( !result ) {
    _error = ERROR_OPEN;
    _errorMsg = QStringLiteral("Can not open the source file");
    delete _srcFile;
    _srcFile = nullptr;
  }
  else if( _containsFieldList ) {
    readHeader();
  }

  return result;
}


void QCsv::finishWithFile() {
  if( nullptr != _srcFile ) {
    _srcFile->close();
    delete _srcFile;
    _srcFile = nullptr;
  }
}


void QCsv::close(){
  clearError();
  finishWithFile();
  _isOpen = false;
}


bool QCsv::toFront() {
  // This function will not work with qCSV_LineByLine mode.
  Q_ASSERT( EntireFile == _mode );
  clearError();

  if( LineByLine == _mode ) {
    setError( ERROR_WRONG_MODE, QStringLiteral("Only EntireFile mode may be used with this function.") );
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
      this->setError( ERROR_OPEN, QStringLiteral("Object is not open.") );
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
        if( fieldList.at(i).startsWith( '\"' ) && fieldList.at(i).endsWith( '\"' ) ) {
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
        if( fieldList.at(i).startsWith( '\"' ) && fieldList.at(i).endsWith( '\"' ) ) {
          fieldList[i] = fieldList.at(i).mid( 1, fieldList.at(i).length() - 2 );
        }
      }
    }

    if( 0 != fieldCount() && ( fieldList.count() != fieldCount() ) ) {
      _error = ERROR_INVALID_FIELD_COUNT;
      _errorMsg = QStringLiteral( "Line %1: %2 fields expected, but %3 fields encountered.  Please check your file format." )
        .arg( QString::number( _currentRowNumber ), QString::number( fieldCount() ), QString::number( fieldList.count() ) )
      ;
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
    setError( QCsv::ERROR_OPEN, QStringLiteral("File must be open to set a field format.") );
    result = false;
  }
  else if( !this->containsFieldList() ) {
    setError( QCsv::ERROR_NO_FIELDLIST, QStringLiteral("This file does not have field names.") );
    result = false;
  }
  else {
    int fieldIdx = fieldIndexOf( fieldName.trimmed() );
    if( -1 == fieldIdx ) {
      setError( QCsv::ERROR_INVALID_FIELD_NAME, QStringLiteral( "Field name '%1' does not exist." ).arg( fieldName ) );
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
    setError( QCsv::ERROR_OPEN, QStringLiteral("File must be open to set a field format.") );
    result = false;
  }
  else if( 0 > fieldIdx ) {
    setError( QCsv::ERROR_INDEX_OUT_OF_RANGE, QStringLiteral("Negative field index provided.") );
    result = false;
  }
  else if( this->fieldCount() <= fieldIdx ) {
    setError( QCsv::ERROR_INDEX_OUT_OF_RANGE, QStringLiteral( "Field index %1 is out of range." ).arg( fieldIdx ) );
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
                _data[row][fieldIdx] = date.toString( QStringLiteral("yyyy-MM-dd") );
              }
              else {
                setError( QCsv::ERROR_OTHER, QStringLiteral( "Format of cell at row %1, column %2 cannot be changed to DateFormat." ).arg( row ).arg( fieldIdx ) );
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
              _fieldData[fieldIdx] = date.toString( QStringLiteral("yyyy-MM-dd") );
            }
            else {
              setError( QCsv::ERROR_OTHER, QStringLiteral( "Format of data in field %1 cannot be changed to DateFormat." ).arg( fieldIdx ) );
              result = false;
            }
          }
        }
        else {
          // This should never happen: if a file is open, it's mode will have been set.
          setError( QCsv::ERROR_WRONG_MODE, QStringLiteral("Mode must be specified to set a field format.") );
          result = false;
        }

        break;
      //case TimeFormat: // Fall through, for now.
      //case DateTimeFormat: // Fall through, for now.
      default:
        setError( QCsv::ERROR_OTHER, QStringLiteral("Specified field format not yet supported.") );
        result = false;
        break;
    }
  }

  return result;
}


// Protected members
void QCsv::clearError(){
  _error = ERROR_NONE;
  _errorMsg = QStringLiteral("(No error)");
}


bool QCsv::isCommentLine( const QString& line ) {
  bool result = ( '#' == line.at(0) );
  return result;
}


QString QCsv::tableDiv( const int len ) {
  QString result( QStringLiteral("--") );
  for( int i = 0; i < len; ++i ) {
    result.append( '-' );
  }

  return result;
}


QString QCsv::tablePadded( const QString& val, const int len ) {
  QString result;
  int i;
  int lenDiff;

  if( val.length() <= len ) {
    // Prepend the leading space
    result = QStringLiteral( " %1" ).arg( val ); // Note the leading space

    // Add spaces until desired length is reached
    lenDiff = len - val.length();
    for( i = 0; lenDiff > i; ++i ) {
      result.append( " " );
    }

    // Tack on the trailing space
    result.append( " " );
  }
  else {
    result = QStringLiteral( " %1" ).arg( val ); // Note the leading space
    result = result.left( len - 2 );
    result = result + "... "; // Note the trailing space
  }

  return result;
}


QString QCsv::asTable() {
  if( ERROR_NONE != error() ) {
    return QString();
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
      list.append( QStringLiteral( "%1" ).arg( tablePadded( _fieldNames.at(i), arr.at(i) ) ) );
    }
    result.append( QStringLiteral( "|%1|\n" ).arg( list.join( '|' ) ) );

    list.clear();
    for( int i = 0; i < _fieldNames.count(); ++i ) {
      list.append( tableDiv( arr.at(i) ) );
    }
    result.append( QStringLiteral( "+%1+\n" ).arg( list.join( '+' ) ) );
  }

  for( int row = 0; row < this->rowCount(); ++row ) {
    list.clear();
    for( int i = 0; i < this->fieldCount(); ++i ) {
      list.append( QStringLiteral( "%1" ).arg( tablePadded( _data.at(row).at(i), arr.at(i) ) ) );
    }
    result.append( QStringLiteral( "|%1|\n" ).arg( list.join( '|' ) ) );
  }

  return result;
}


QString QCsv::csvQuote( QString s, const QChar delimiter /* = ',' */ ) {
  if(  s.contains( '"' ) )
    s.replace( '"', QLatin1String("\"\"") );

  if( s.contains( QRegExp( "\\s" ) ) || s.contains( delimiter ) )
    s = QStringLiteral( "\"%1\"" ).arg( s );

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
  const bool checkForComment /* = false */,
  QObject* parent /* = nullptr */
) : QObject( parent ), QCsv( filename, containsFieldList, stringsContainDelimiters, mode, checkForComment ) {
  // Do nothing.
}


QCsvObject::QCsvObject( const QStringList& fieldNames, QObject* parent /* = nullptr */ ) : QObject( parent ), QCsv( fieldNames ) {
  // Do nothing.
}


QCsvObject::QCsvObject( const QStringList& fieldNames, const QList<QStringList>& data, QObject* parent /* = nullptr */ ) : QObject( parent ), QCsv( fieldNames, data ) {
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

