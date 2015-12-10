#include "cerror.h"

CError::CError(){
  _level = Unspecified;
  _msg = "";
}


CError::CError( const ErrorLevel type, const QString& msg ) {
  _level = type;
  _msg = msg;
}


CError::CError( const CError& other ) {
  _level = other._level;
  _msg = other._msg;
}


CError& CError::operator=( const CError& other ) {
  _level = other._level;
  _msg = other._msg;

  return *this;
}


CErrorList::CErrorList(){
  // Nothing to do here.
}


void CErrorList::clear() {
  _list.clear();
}


int CErrorList::count() {
  return _list.count();
}


CError CErrorList::itemAt( const int i ) {
  return _list.at(i);
}


void CErrorList::append( CError err ) {
  _list.append( err );
}


void CErrorList::append( CError::ErrorLevel type, const QString& msg ) {
  _list.append( CError( type, msg ) );
}


void CErrorList::append( CErrorList src ) {
  for( int i = 0; i < src.count(); ++i )
    _list.append( src.itemAt(i) );
}


QString CErrorList::at( const int i ) {
  return _list.at(i).msg();
}


QString CErrorList::asText() {
  QString result;
  for( int i = 0; i < count(); ++i ) {
    result.append( _list.at(i).msg() );
    result.append( '\n' );
  }
  
  return result;
}


QString CError::levelAsString() {
  return levelAsString( level() );
}


QString CError::levelAsString( const ErrorLevel type ) {
  QString typeStr;

  switch( type ) {
    case CError::Information: typeStr = "Information"; break;
    case CError::Question: typeStr = "Question"; break;
    case CError::Warning: typeStr = "Warning"; break;
    case CError::Critical: typeStr = "Critical"; break;
    case CError::Fatal: typeStr = "Fatal"; break;
    default:
      qDebug() << "Problem encountered in CError::typeAsString()";
      break;
  }

  return typeStr;
}


CErrorHandler::CErrorHandler( const bool writeErrorLog, const QString& errorLogFilename, const bool autoWriteErrorLog /* = true */ ) {
  _maxErrorLevel = CError::NoError;
  _writeErrorLog = writeErrorLog;
  _errorLogFilename = errorLogFilename;
  _autoWriteErrorLog = autoWriteErrorLog;
  _result = ErrorReturnValue::NO_ERROR;
}


CErrorHandler::~CErrorHandler() {
  if( _autoWriteErrorLog && _writeErrorLog )
    writeErrorLogFile();
}


QString CErrorHandler::errors() {
  if( ErrorReturnValue::NO_ERROR == _result )
    return( "No errors" );
  else {
    QString result;

    if( _result & ErrorReturnValue::UNSPECIFIED_ERROR )
      result.append( "UNSPECIFIED_ERROR & ");
    if( _result & ErrorReturnValue::BAD_COMMAND )
      result.append( "BAD_COMMAND & ");
    if( _result & ErrorReturnValue::INPUT_FILE_PROBLEM )
      result.append( "INPUT_FILE_PROBLEM & ");
    if( _result & ErrorReturnValue::DATA_VALIDATION_PROBLEM )
      result.append( "DATA_VALIDATION_PROBLEM & ");
    if( _result & ErrorReturnValue::OUTPUT_FILE_PROBLEM )
      result.append( "OUTPUT_FILE_PROBLEM & ");
    if( _result & ErrorReturnValue::ERROR_LOG_PROBLEM )
      result.append( "ERROR_LOG_PROBLEM & ");
    if( _result & ErrorReturnValue::ERROR_VARIANT_CONVERSION )
      result.append( "ERROR_VARIANT_CONVERSION & ");
    if( _result & ErrorReturnValue::PROCESSING_INTERRUPTED )
      result.append( "PROCESSING_INTERRUPTED & ");
    if( _result & ErrorReturnValue::FAILED_DB_QUERY )
      result.append( "FAILED_DB_QUERY & ");
    if( _result & ErrorReturnValue::BAD_CONFIG )
      result.append( "BAD_CONFIG & ");
    if( _result & ErrorReturnValue::FILE_SYSTEM_PROBLEM )
      result.append( "FILE_SYSTEM_PROBLEM & ");
    if( _result & ErrorReturnValue::REQUIRED_FIELDS_MISSING )
      result.append( "REQUIRED_FIELDS_MISSING & ");
    if( _result & ErrorReturnValue::BAD_DATABASE )
      result.append( "BAD_DATABASE & ");

    return result.left( result.length() - 3 );
  }
}


void CErrorHandler::writeErrorLogFile() {
  if( !_writeErrorLog ) {
    // Signals?
    return;
  }

  //emit setProgressLabelText( "Writing validation log..." );
  //emit setProgressMax( _errMsgs.count() );
  //emit setProgressValue( 0 );

  QFile errorFile( _errorLogFilename );
  if( !errorFile.open( QFile::WriteOnly | QFile::Truncate ) ) {
    qDebug() << QString( "Cannot open error log for writing: %1" ).arg( _errorLogFilename );
    handleError( CError::Critical, QString( "Cannot open error log for writing: %1" ).arg( _errorLogFilename ) );
    _result = ( _result | ErrorReturnValue::ERROR_LOG_PROBLEM );
  }
  else {
    QTextStream out( &errorFile );

    if( 0 == _errMsgs.count() ) {
      out << "(No errors encountered)\r\n";
    }
    else {
      for( int i = 0; i < _errMsgs.count(); ++i ) {
        out << _errMsgs.at(i) << "\r\n";
        //emit setProgressValue( i+1 );
      }
    }

    out << flush;
    errorFile.close();
  }
}


void CErrorHandler::handleError( CError::ErrorLevel level, const QString& msg, const ErrorReturnValue::Result result /* = ErrorReturnValue::UNSPECIFIED_ERROR */ ) {
  _maxErrorLevel = qMax( _maxErrorLevel, int( level ) );
  _result = result;
  _errMsgs.append( CError( level, msg ) );
}


//void CErrorHandler::handleError( const CErrorList& msgs ) {
//  _errMsgs.append( msgs );
//}
