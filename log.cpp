/*
log.cpp
-------
Begin: 2004/09/04
Last revision: $Date: 2011-10-25 04:57:18 $ $Author: areeves $
Version number: $Revision: 1.4 $
Project: Atriplex Distributed Computing System
Website: 
Author: Aaron Reeves <aaron@aaronreeves.com>
--------------------------------------------------
Copyright (C) 2004 - 2008 Aaron Reeves

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#include "log.h"

#include <qdatetime.h>
#include <qfile.h>
#include <qstring.h>
#include <qtextstream.h>
#include <qstringlist.h>
#include <qdebug.h>

CAppLog* log;

CLogMessage::CLogMessage( const int level, const QString& msg ) {
  _level = level;
  _msg = msg; 
}


CAppLog::CAppLog( void ) {
  _logOpen = false;
  _logFileName = ""; 
  
  _logFile = NULL;
  _logTextStream = NULL;
  _pending = NULL;
 
  _logLineCount = 0;
  
  #ifdef LOG_NO_DEBUG_OUTPUT
    _debugging = false;
  #else
    _debugging = true;
  #endif
  
  setLogLevel( LoggingNone );   
}


CAppLog::CAppLog( const QString& fileName ) {
  _logOpen = false;
  _logFileName = fileName; 
  
  _logFile = NULL;
  _logTextStream = NULL;
  _pending = NULL;
 
  _logLineCount = 0;
    
  #ifdef LOG_NO_DEBUG_OUTPUT
    _debugging = false;
  #else
    _debugging = true;
  #endif
  
  setLogLevel( LoggingPending );  
}


CAppLog::CAppLog( const QString& fileName, const int logLevel ) {
  _logOpen = false;
  _logFileName = fileName; 
  
  _logFile = NULL;
  _logTextStream = NULL;
  _pending = NULL;
 
  _logLineCount = 0;
      
  #ifdef LOG_NO_DEBUG_OUTPUT
    _debugging = false;
  #else
    _debugging = true;
  #endif
  
  setLogLevel( logLevel );  
}


CAppLog::~CAppLog( void ) {
  closeLog();
  
  if( NULL != _pending ) {
    qDeleteAll( _pending->begin(), _pending->end() );
    delete _pending;
    _pending = NULL;
  }  
}


void CAppLog::setLogLevel( const int logLevel ) {
  _logLevel = logLevel;
  
  // If logging is set to "pending", set up the message list
  // and store messages until a decision is made about what to do with them.
  if( ( LoggingPending == _logLevel ) ) {
    if( _logOpen ) {
      closeLog();
    }
    if( NULL == _pending ) {
      _pending = new TLogMessageList();
    }
  }
  
  // If logging is set to "none", close the log file
  // and throw away the list of pending log messages.
  else if( LoggingNone == _logLevel ) {
    if( _logOpen ) {
      closeLog();
    }
    if( NULL != _pending ) {
      qDeleteAll( _pending->begin(), _pending->end() );
      delete _pending;
      _pending = NULL;  
    }  
  }
  
  // If logging is set to some real value,
  // make sure that the log file is open and process any pending messages. 
  else {
    if( !( _logOpen ) ) {
      _logFile = new QFile( _logFileName );
      
      _logOpen = openLog();
      
      if( _logOpen ) {
        processPendingMessages();  
      }
      else {
        qDebug() << "Log file is not open!";  
      }
    }
  }  
}


bool CAppLog::openLog( void ) {
  #ifndef NO_LOGGING
    _logFile = new QFile( _logFileName );
  
    truncateLogFile();
  
    if( _logFile->open( QIODevice::WriteOnly | QIODevice::Append ) ) {
      _logTextStream = new QTextStream( _logFile );
      *_logTextStream << "\r\n" << flush;
      qDebug() << "Log file is open.";
      return true;
    }
    else {
      qDebug() << "Log file did not open!";
      delete _logFile;
      _logFile = NULL;
      _logTextStream = NULL;
      return false;
    }
  #else
    return false;
  #endif
}


void CAppLog::closeLog( void ) {
  if( NULL != _logFile ) {
    _logFile->close();
    delete _logFile;
    _logFile = NULL;    
  }
  
  if( NULL != _logTextStream ) {
    delete _logTextStream;
    _logTextStream = NULL;
  }
  
  _logOpen = false;
}


// Keep the last 3000 lines of the file, and eliminate the rest.
void CAppLog::truncateLogFile( void ) {
  QStringList list;
  QString readln;
  int i;

  bool truncNeeded = false;

  // Read the old log into memory...
  if( _logFile->open( QIODevice::ReadOnly ) ) {
    QTextStream t( _logFile );

    while( !( t.atEnd() ) ) {
      readln = t.readLine();
      list.append( QString( readln ) );
      if( 3000 < list.count() ) {
        truncNeeded = true;
        list.removeFirst(); 
      }
    }

    _logFile->close();
  }

  if( truncNeeded ) {
    QString dt = QDateTime::currentDateTime().toString();
  
    // Write a new log...
    if( _logFile->open( QIODevice::WriteOnly ) ) {
      QTextStream t( _logFile );
  
      t << dt << ": LOG FILE TRUNCATED" << "\r\n" << "\r\n" << flush;
  
      for( i = 0; i < list.count(); ++i ) {
        t << list.at( i ) << "\r\n" << flush;
      }
      _logFile->close();
    }
  }
  
  _logLineCount = list.count();
}


void CAppLog::typical( const QString& message ) {
  CLogMessage* msg;
  QString dt = QDateTime::currentDateTime().toString();

  if( _debugging ) {
    qDebug() << "          (app)" << message;    
  }
  
  if( LoggingPending == _logLevel ) {
    msg = new CLogMessage( LoggingTypical, QString( "%1: %2" ).arg( dt ).arg( message ) );
    _pending->append( msg );    
  }
  else if( _logOpen && ( LoggingTypical <= _logLevel ) ) {
    *_logTextStream << "\r\n" << dt << ": " << message << flush;
    ++_logLineCount;
    if( 10000 < _logLineCount ) {
      truncateLogFile();
    } 
  }
}


void CAppLog::verbose( const QString& message ) {
  CLogMessage* msg;
  QString dt = QDateTime::currentDateTime().toString();

  if( _debugging ) {
    qDebug() << message;    
  }
  
  if( LoggingPending == _logLevel ) {
    msg = new CLogMessage( LoggingVerbose, QString( "%1: %2" ).arg( dt ).arg( message ) );
    _pending->append( msg );    
  }
  else if( _logOpen && ( LoggingVerbose <= _logLevel ) ) {
    *_logTextStream << "\r\n" << dt << ": " << message << flush; 
    ++_logLineCount;
    if( 10000 < _logLineCount ) {
      truncateLogFile();
    }    
  }
}


void CAppLog::processPendingMessages( void ) {
  CLogMessage* msg;

  if( NULL != _pending ) {
    while( !( _pending->isEmpty() ) ) {
      msg = _pending->takeFirst();
      if( _logOpen ) {
        if( msg->_level >= _logLevel ) {
          *_logTextStream << "\r\n" << msg->_msg << flush;
          ++_logLineCount;
          if( 10000 < _logLineCount ) {
            truncateLogFile();
          }  
        }  
      }
      delete msg;  
    } 
    
    delete _pending;
    _pending = NULL; 
  }
}










