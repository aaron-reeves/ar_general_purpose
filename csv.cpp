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


qCSV::qCSV() : QObject() {
  initialize();
}


qCSV::qCSV (
  const QString& filename,
  const bool containsFieldList,
  const QChar& stringToken /* = '\0' */,
  const bool stringsContainDelimiters /* = true */,
  const int readMode /* = qCsv::ReadLineByLine */,
  const bool checkForComment /* = false */
) : QObject() {
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
    this->open();

    //qDebug() << "Reading entire file...";
    int fieldsRead = 0;
    while( -1 != fieldsRead ) {
      //qDebug() << fieldsRead;
      fieldsRead = moveNext();
    }

    this->close();
    //qDebug() << "Done.";
  }
}


void qCSV::initialize() {
  _srcFilename = "";
  _currentLine = "";
  _currentLineNumber = 0;
  _error = qCSV_ERROR_NONE;
  _errorMsg = "";
  _stringToken = '\0';
  _usesStringToken = false;
  _containsFieldList = false;
  _stringsContainDelimiters = true;
  _concatenateDanglingEnds = false;
  _readMode = qCSV_ReadLineByLine;
  _eolDelimiter = " ";
  _delimiter = ',';
  _firstDataRowEncountered = false;
  _checkForComment = false;
  _nCommentRows = 0;
}


qCSV::~qCSV() {
  if ( _srcFile.isOpen() )
    _srcFile.close();

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


void qCSV::debug() {
  qDebug() << "qCSV contents:";

  qDebug() << "numFields:" << this->fieldCount();
  qDebug() << "numFieldNames:" << this->fieldNames().count();
  qDebug() << "numRows:" << this->rowCount();

  qDebug() << this->fieldNames().join( _delimiter ).prepend( "  " );

  if( qCSV_ReadLineByLine == _readMode )
    qDebug() << "(There is nothing to display)";
  else {
    for( int i = 0; i < _data.count(); ++i ) {
      qDebug() << _data.at(i).join( _delimiter ).prepend( "  " );
    }
  }
}


void qCSV::setField( const int index, const QString& val ) {
  QStringList dataList;
  clearError();

  if( qCSV_ReadLineByLine == _readMode )
    dataList = _fieldData;
  else
    dataList = _data.at( _currentLineNumber );

  if( 0 < dataList.size() ) {
    if ( index < dataList.size() ) {
      if( qCSV_ReadLineByLine == _readMode )
        _fieldData[index] = val;
      else
        _data[_currentLineNumber][index] = val;
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
QString qCSV::field( int index ){
  QStringList dataList;
  QString ret_val = "";
  clearError();

  if( qCSV_ReadLineByLine == _readMode )
    dataList = _fieldData;
  else
    dataList = _data.at( _currentLineNumber );

  if ( dataList.size() > 0 ){
    if ( dataList.size() > index ){
      ret_val = dataList[index];
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
  QStringList dataList;
  clearError();

  if( qCSV_ReadLineByLine == _readMode )
    dataList = _fieldData;
  else
    dataList = _data.at( _currentLineNumber );

  if ( _containsFieldList ){
    if ( dataList.size() > 0 ){
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

  QStringList dataList;
  QString ret_val = "";
  clearError();

  if( qCSV_ReadLineByLine == _readMode )
    dataList = _fieldData;
  else
    dataList = _data.at( _currentLineNumber );

  if ( _containsFieldList ){
    if ( dataList.size() > 0 ){
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


QVariantList qCSV::fields( QString fName ) {
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


void qCSV::setFilename ( QString filename ){
  clearError();
  if ( _srcFile.isOpen() )
  {
    _srcFile.close();
  };

  _srcFilename = filename;
}


bool qCSV::open(){
  bool ret_val = false;

  clearError();

  if ( _srcFile.isOpen() ){
    ret_val = true;
  }
  else{
    _srcFile.setFileName ( _srcFilename );
    if ( ! ( ret_val = _srcFile.open( QIODevice::ReadOnly | QIODevice::Text ) ) ){
      _error = qCSV_ERROR_OPEN;
      _errorMsg = "Can not open the source file";
    }
  }

  return ret_val;
}


bool qCSV::close(){
  bool ret_val = false;
  clearError();

  if ( !_srcFile.isOpen() ){
    ret_val = true;
  }
  else{
    _srcFile.close();
    ret_val = true;
  }

  return ret_val;
}


//  Also acts as movefirst...cause a read of a line of data from the csv file.
//  Returns the number of fields read, or -1 at the end of the file.
int qCSV::moveNext(){
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
    tmp = _srcFile.readLine();
    tmp = tmp.trimmed();
    if( !_currentLine.isEmpty() )
      _currentLine.append( _eolDelimiter );

    _currentLine.append( tmp );
    nQuotes = nQuotes + tmp.count( '\"' );
  } while( 0 != nQuotes%2 );


  if( !_currentLine.isEmpty() ) {
    _currentLineNumber++;

    // This next statement handles the situation where the file
    // begins with a header (indicated by lines that start with #).
    // These lines should simply be skipped.
    if( _checkForComment && isCommentLine( _currentLine ) ) {
      ++_nCommentRows;
      return moveNext();
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
        _fieldNames.append( tempString.toLower() );
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
      index = ret_val = moveNext();
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
    if ( !_srcFile.atEnd() ){
      _error = qCSV_ERROR_BAD_READ;
      _errorMsg = "Can not read next line.  Last line number was: " + QString::number ( _currentLineNumber ) + ".  Are we at the end of the file?";
    }
  }

  emit nBytesRead( _currentLine.toUtf8().size() );

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

