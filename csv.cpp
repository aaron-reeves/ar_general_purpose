/*
csv.h/cpp
----------
Begin: 2014/04/11
Author (modified version): Aaron Reeves <development@reevesdigital.com>
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


qCSV::qCSV() {
  initialize();
}


qCSV::qCSV ( const QString& filename, const bool containsFieldList, const QChar& stringToken, const bool stringsContainCommas ) {
  initialize();

  _srcFilename = filename;
  _stringToken = stringToken;
  _containsFieldList = containsFieldList;

  if ( stringToken != '\0' )
    _usesStringToken = true;

  _stringsContainCommas = stringsContainCommas;
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
  _stringsContainCommas = true;
  _concatenateDanglingEnds = false;
}


qCSV::~qCSV() {
  if ( _srcFile.isOpen() )
    _srcFile.close();

  _fields.clear();

  _srcFilename = "";
  _currentLine = "";
  _currentLineNumber = -1;
  _error = qCSV_ERROR_NONE;
  _errorMsg = "";
  _stringToken = '\0';
  _usesStringToken = false;
  _containsFieldList = false;
}


// Accessors
QString qCSV::field ( int index ){
  QString ret_val = "";
  clearError();

  if ( _fieldData.size() > 0 ){
    if ( _fieldData.contains ( index ) ){
      ret_val = _fieldData[index];
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


QString qCSV::field ( QString fName ){
  QString ret_val = "";
  clearError();

  if ( _containsFieldList ){
    if ( _fieldData.size() > 0 ){
      if ( _fields.contains ( fName ) ){
        int index =  _fields[fName];

        ret_val = field ( index );
        /*
        if ( index < _fieldData.size() )
        {
          ret_val = _fieldData[index];
        }
        else
        {
          _error = qCSV_ERROR_INDEX_OUT_OF_RANGE;
          _errorMsg = "For File Linenumber: " + QString::number( _currentLineNumber ) + ".  Field index, " + QString::number( index ) + " for field: " + fName + ", out of range";
        }
        */
        if ( ret_val.size() <= 0 ){
          _errorMsg = " Error finding field name: " + fName + ".  " + _errorMsg;
        }
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


QString qCSV::fieldName ( int index ){
  QString ret_val = "";
  if ( _containsFieldList ){
    if ( !_fields.key ( index ).isEmpty() ){
      ret_val = _fields.key ( index );
    }
  }

  return ret_val;
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
    if ( ! ( ret_val = _srcFile.open ( QIODevice::ReadOnly ) ) ){
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
//  Returns the number of fields read.
int qCSV::moveNext(){
  int ret_val = -1;
  int index = 0;
  QStringList fieldList;

  clearError();

  _fieldData.clear();

  _currentLine.clear();
  _currentLine = _srcFile.readLine ();
  if ( !_currentLine.isEmpty() ){
    _currentLineNumber++;

    if ( _stringsContainCommas )
      fieldList = CSV::parseLine( _currentLine );
    else
      fieldList = _currentLine.split ( ',' );


    for ( int i = 0; i < fieldList.size(); i++ ){
      index++;
      QString tempString = fieldList[i];
      tempString = tempString.trimmed();

      if ( _usesStringToken ){
        tempString = tempString.replace ( _stringToken, "" );
      }

      if ( _containsFieldList && ( _currentLineNumber == 1 ) ){
        _fields.insert ( tempString, index );
      }
      else if ( _containsFieldList && _concatenateDanglingEnds ){
        if ( i < _columnCount )
          _fieldData.insert ( index, tempString );
        else{
          QString tempStr = _fieldData[ _columnCount ];
          tempStr += ", " + tempString;
          _fieldData.insert ( _columnCount, tempStr );
        }
      }
      else
        _fieldData.insert ( index, tempString );
    }

    if ( _containsFieldList && ( _currentLineNumber == 1 ) ){
      _columnCount = index;
      index = ret_val = moveNext();
    }
    else
      ret_val = index;

    /*  Save this, below, for a switch to check field list lengths later on....some csv formats,
        such as those created by Microsoft products, unfortunately, make the strings 
        too short if the last few fields are empty...i.e. they don't save empty strings, or empty 
        comma separated strings, (such as ",,,,") if they are at the end of a line, and have
        no data somewhere after them...bad news, if you want to verify fields....
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


