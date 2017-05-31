#include "cerror.h"

/*
cerror.h/cpp
------------
Begin: 2016/09/17
Authors: Aaron Reeves <aaron.reeves@sruc.ac.uk>
         Julie Stirling <julie.stirling@sruc.ac.uk>
---------------------------------------------------
Copyright (C) 2016 Epidemiology Research Unity, Scotland's Rural College (SRUC)

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#include "cerror.h"

#include <ar_general_purpose/log.h>

CError::CError(){
  _type = Unspecified;
  _msg = "";
  _lineNumber = -1;
  _dataSourceID = -1;
}


CError::CError( const ErrorType type, const QString& msg, const int dataSourceID /* = -1 */, const int lineNumber /* = -1 */ ) {
  _type = type;
  _msg = msg.trimmed();
  _lineNumber = lineNumber;
  _dataSourceID = dataSourceID;
}


CError::CError( const CError& other ) {
  _type = other._type;
  _msg = other._msg;
  _lineNumber = other._lineNumber;
  _dataSourceID = other._dataSourceID;
}


CError& CError::operator=( const CError& other ) {
  _type = other._type;
  _msg = other._msg;
  _lineNumber = other._lineNumber;
  _dataSourceID = other._dataSourceID;

  return *this;
}


QString CError::logMessage() const {
  QString str;

  if( !this->_msg.isEmpty() ) {
    if( -1 < _lineNumber )
      str = QString( "Line %1: %2:" ).arg( this->_lineNumber ).arg( this->typeAsString().toUpper() );
    else
      str = QString( "%1:" ).arg( this->typeAsString().toUpper() );

    if( this->_msg.contains( "\n" ) )
      str.append( QString( ">>> %1 <<< (End)" ).arg( this->_msg ) );
    else
      str.append( QString( " %1" ).arg( this->_msg ) );
  }

  return str.trimmed();
}


QString CError::typeAsString() const {
  return typeAsString( type() );
}


QString CError::typeAsString( const ErrorType type ) {
  QString typeStr;

  switch( type ) {
    case CError::Ok: typeStr = "Ok"; break;
    case CError::Information: typeStr = "Information"; break;
    case CError::Question: typeStr = "Question"; break;
    case CError::Warning: typeStr = "Warning"; break;
    case CError::Critical: typeStr = "Critical"; break;
    case CError::Fatal: typeStr = "Fatal"; break;
    default:
      qDebug() << "Problem encountered in CError::typeAsString()";
      Q_ASSERT( false );
      break;
  }

  return typeStr;
}

void CError::debug() const {
  QString q = QString( "%1: %2, %3, %4" ).arg( this->typeAsString() ).arg( this->msg() ).arg( this->dataSourceID() ).arg( this->lineNumber() );
  qDebug() << q;
}


CErrorList::CErrorList( const bool useAppLog ){
  _useAppLog = useAppLog;
}


CErrorList::CErrorList() {
  _useAppLog = false;
}


CErrorList::CErrorList( const CErrorList& other ) {
  _useAppLog = other._useAppLog;
  _list = other._list;
}


CErrorList::~CErrorList() {
  // Nothing to do here.
}


void CErrorList::clear() {
  _list.clear();
}


int CErrorList::count() const {
  return _list.count();
}


bool CErrorList::hasErrors() const {
  bool result = false;
  for( int i = 0; i < _list.count(); ++i ) {
    if( ( CError::Critical == _list.at(i).type() ) || ( CError::Fatal == _list.at(i).type() ) ) {
      result = true;
      break;
    }
  }

  return result;
}


bool CErrorList::hasWarnings() const {
  bool result = false;
  for( int i = 0; i < _list.count(); ++i ) {
    if( CError::Warning == _list.at(i).type() ) {
      result = true;
      break;
    }
  }

  return result;
}

QString CErrorList::logMessage() const {
  QString result;

  for( int i = 0; i < _list.count(); ++i ) {
    if( !_list.at(i).logMessage().isEmpty() ) {
      result.append( _list.at(i).logMessage() );
      result.append( "\r\n" );
    }
  }
  
  return result.trimmed();
}
  

CError CErrorList::at( const int i ) const {
  return _list.at(i);
}


void CErrorList::append( CError err ) {
  _list.append( err );

  if( _useAppLog ) {
    if( -1 == err.lineNumber() )
      logMsg( err.msg(), LoggingTypical );
    else
      logMsg( QString( "Line %1: %2" ).arg( err.lineNumber() ).arg( err.msg() ), LoggingTypical );
  }
}


//void CErrorList::append( CError::ErrorType type, const QString& msg ) {
//  CError err( type, msg );

//  _list.append( err );

//  if( _useAppLog )
//    logMsg( err.logMsg(), LoggingTypical );
//}


void CErrorList::append( CErrorList src ) {
  for( int i = 0; i < src.count(); ++i ) {
    _list.append( src.at(i) );

    if( _useAppLog )
      logMsg( src.at(i).logMessage(), LoggingTypical );
  }
}


QString CErrorList::messageAt( const int i ) {
  return _list.at(i).logMessage();
}


QString CErrorList::asText() {
  QString result;
  for( int i = 0; i < count(); ++i ) {
    result.append( _list.at(i).msg() );
    result.append( '\n' );
  }
  
  return result;
}


bool CErrorList::writeFile( const QString& filename, const ErrorFileFormat fmt ) {
  QFile data( filename );
  if( data.open( QFile::WriteOnly | QFile::Truncate ) ) {
    QTextStream out( &data );

    // Header row
    //-----------
    switch( fmt ) {
      case ErrorFileCSV:
        out << "Line number, Error message, Error type" << endl;
        break;
      default: // ErrorFileLog
        if( _list.isEmpty() ) {
          out << "(No errors reported)" << endl;
        }
        break;
    }

    // Data
    //-----
    for( int i = 0; i < _list.count(); ++i ) {
      CError err = _list.at(i);

      switch( fmt ) {
        case ErrorFileCSV:
          out << err.lineNumber() << ", " << err.msg() << ", " << err.typeAsString() << endl;
          break;
        default: // ErrorFileLog
          if( !err.logMessage().isEmpty() )
            out << err.logMessage();
          break;
      }
    }

    return true;
  }
  else
    return false;
}








#ifdef UNDEFINED
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
#endif //UNDEFINED
