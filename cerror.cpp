/*
cerror.h/cpp
------------
Begin: 2016/09/17
Authors: Aaron Reeves <aaron.reeves@sruc.ac.uk>
         Julie Stirling <julie.stirling@sruc.ac.uk>
---------------------------------------------------
Copyright (C) 2016 Epidemiology Research Unit, Scotland's Rural College (SRUC)

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#include "cerror.h"

#include <ar_general_purpose/qcout.h>

#include "cerror.h"

#include <ar_general_purpose/log.h>

CError::CError(){
  _type = Unspecified;
  _msg = QString();
  _lineNumber = -1;
  _dataSourceID = -1;
}


CError::CError(const ErrorType type, const QString& msg, const int dataSourceID /* = -1 */, const int lineNumber /* = -1 */ ) {
  _type = type;
  _msg = msg.trimmed();
  _lineNumber = lineNumber;
  _dataSourceID = dataSourceID;
}


CError::CError( const CError& other ) {
  assign( other );
}


void CError::assign( const CError& other ) {
  _type = other._type;
  _msg = other._msg;
  _lineNumber = other._lineNumber;
  _dataSourceID = other._dataSourceID;
}


CError& CError::operator=( const CError& other ) {
  assign( other );
  return *this;
}


QString CError::logMessage() const {
  QString str;

  if( !this->_msg.isEmpty() ) {
    if( -1 < _lineNumber )
      str = QStringLiteral( "Line %1: %2:" ).arg( this->_lineNumber ).arg( this->typeAsString().toUpper() );
    else
      str = QStringLiteral( "%1:" ).arg( this->typeAsString().toUpper() );

    if( this->_msg.contains( '\n' ) )
      str.append( QStringLiteral( ">>> %1 <<< (End)" ).arg( this->_msg ) );
    else
      str.append( QStringLiteral( " %1" ).arg( this->_msg ) );
  }

  return str.trimmed();
}


QString CError::typeAsString() const {
  return typeAsString( type() );
}


QString CError::typeAsString( const ErrorType type ) {
  QString typeStr;

  switch( type ) {
    case CError::Ok: typeStr = QStringLiteral("Ok"); break;
    case CError::Information: typeStr = QStringLiteral("Information"); break;
    case CError::Question: typeStr = QStringLiteral("Question"); break;
    case CError::Warning: typeStr = QStringLiteral("Warning"); break;
    case CError::Critical: typeStr = QStringLiteral("Critical"); break;
    case CError::Fatal: typeStr = QStringLiteral("Fatal"); break;
    default:
      qDb() << "Problem encountered in CError::typeAsString()";
      Q_ASSERT( false );
      break;
  }

  return typeStr;
}

void CError::debug() const {
  QString q = QStringLiteral( "%1: %2, %3, %4" ).arg( this->typeAsString(), this->msg(), QString::number( this->dataSourceID() ), QString::number( this->lineNumber() ) );
  qDb() << q;
}


CErrorList::CErrorList( const bool useAppLog ){
  _useAppLog = useAppLog;
}


CErrorList::CErrorList() {
  _useAppLog = false;
}


CErrorList::CErrorList( const CErrorList& other ) {
  assign( other );
}

void CErrorList::assign( const CErrorList& other ) {
  _useAppLog = other._useAppLog;
  _list = other._list;
}

CErrorList& CErrorList::operator=( const CErrorList& other ) {
  assign( other );
  return *this;
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
  if( 0 == _list.count() ) {
    return false;
  }
  else {
    bool result = false;
    for( int i = 0; i < _list.count(); ++i ) {
      if( ( CError::Critical == _list.at(i).type() ) || ( CError::Fatal == _list.at(i).type() ) ) {
        result = true;
        break;
      }
    }

    return result;
  }
}


bool CErrorList::hasWarnings() const {
  if( 0 == _list.count() ) {
    return false;
  }
  else {
    bool result = false;
    for( int i = 0; i < _list.count(); ++i ) {
      if( CError::Warning == _list.at(i).type() ) {
        result = true;
        break;
      }
    }

    return result;
  }
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


void CErrorList::append( const CError& err ) {
  _list.append( err );

  if( _useAppLog ) {
    if( -1 == err.lineNumber() )
      logMsg( QStringLiteral( "??? LINE -1: (%1) %2" ).arg( err.typeAsString(), err.msg() ), LoggingTypical );
    else
      logMsg( QStringLiteral( "Line %1: (%2) %3" ).arg( QString::number( err.lineNumber() ), err.typeAsString(), err.msg() ), LoggingTypical );
  }
}


//void CErrorList::append( CError::ErrorType type, const QString& msg ) {
//  CError err( type, msg );

//  _list.append( err );

//  if( _useAppLog )
//    logMsg( err.logMsg(), LoggingTypical );
//}


void CErrorList::append( const CErrorList& src ) {
  for( int i = 0; i < src.count(); ++i ) {
    _list.append( src.at(i) );

    if( _useAppLog )
      logMsg( src.at(i).logMessage(), LoggingTypical );
  }
}


QString CErrorList::messageAt( const int i ) const {
  return _list.at(i).logMessage();
}


QString CErrorList::asText() const {
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
            out << err.logMessage() << endl;
          break;
      }
    }

    return true;
  }
  else
    return false;
}

