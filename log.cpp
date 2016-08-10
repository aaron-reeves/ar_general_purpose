/*
log.h/cpp
---------
Begin: 2004/09/04
Author: Aaron Reeves <aaron.reeves@naadsm.org>
--------------------------------------------------
Copyright (C) 2004 - 2008 Aaron Reeves

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#include "log.h"

#include <QDateTime>
#include <QFile>
#include <QString>
#include <QTextStream>
#include <QStringList>
#include <QDebug>
#include <QDir>
#include <QFileInfo>

#include <ar_general_purpose/qcout.h>

CAppLog* appLog = NULL;

void logMsg( const QString& msg, const LogLevel logLevel /* = LoggingTypical */ ) {
  if( NULL != appLog )
    appLog->logMessage( msg, logLevel );
}


void logVerbose( const QString& msg ) {
  logMsg( msg, LoggingVerbose );
}


CLogMessage::CLogMessage( const int level, const QString& msg ) {
  _level = level;
  _msg = msg; 
}


CAppLog::CAppLog( void ) {
  initialize();
}


CAppLog::CAppLog( QString fileName, const int logLevel, const FileFrequency freq /* = OneFile */ ) {
  initialize();

  openLog( fileName, logLevel, freq );
}


void CAppLog::initialize() {
  _logOpen = false;
  _logFileName = "";

  _logFile = NULL;
  _logTextStream = NULL;
  _pending = NULL;

  _logLineCount = 0;

  _useStderr = false;
  _autoTruncate = false;

  _freq = OneFile;

  _consoleEcho = false;

  setLogLevel( LoggingPending );
}


CAppLog::~CAppLog( void ) {
  closeLog();
  
  if( NULL != _pending ) {
    qDeleteAll( _pending->begin(), _pending->end() );
    delete _pending;
    _pending = NULL;
  }  
}


void CAppLog::openLog( QString fileName, const int logLevel, const FileFrequency freq /* = OneFile */ ) {
  setFileFrequency( freq );
  setFileName( fileName );
  setLogLevel( logLevel );

  //qDebug() << "Log" << _logFileName << "will open.";
}


void CAppLog::setFileName( QString fileName ) {
  QFileInfo fi( fileName );

  switch ( _freq ) {
    case DailyFiles:
      fileName = QString( "%1/%2-%3" ).arg( fi.absoluteDir().absolutePath() ).arg( QDate::currentDate().toString( "yyyyMMdd" ) ).arg( fi.fileName() );
      break;

    // For now, fall through for all other options.
    case OneFile:
    default:
      break;
  }

  _logFileName = fileName;
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
        // FIXME: For the moment, fail silently.
        //qDebug() << "Log file is not open!";  
      }
    }
  }  
}


bool CAppLog::openLog( void ) {
    _logFile = new QFile( _logFileName );
  
    if( _autoTruncate )
      truncateLogFile();
  
    if( _logFile->open( QIODevice::WriteOnly | QIODevice::Append ) ) {
      _logTextStream = new QTextStream( _logFile );
      *_logTextStream << ::endl << ::flush;
      //qDebug() << "Log file is open.";
      return true;
    }
    else {
      //qDebug() << "Log file did not open!";
      delete _logFile;
      _logFile = NULL;
      _logTextStream = NULL;
      return false;
    }
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
    QString dt = QDateTime::currentDateTime().toString( "yyyy-MM-dd hh:mm:ss.zzz" );
  
    // Write a new log...
    if( _logFile->open( QIODevice::WriteOnly ) ) {
      QTextStream t( _logFile );
  
      t << dt << ": LOG FILE TRUNCATED" << "\r\n" << "\r\n" << ::flush;
  
      for( i = 0; i < list.count(); ++i ) {
        t << list.at( i ) << "\r\n" << ::flush;
      }
      _logFile->close();
    }
  }
  
  _logLineCount = list.count();
}


void CAppLog::logMessage( const QString& message, const int logLevel ) {
  CLogMessage* msg;
  QString dt = QDateTime::currentDateTime().toString( "yyyy-MM-dd hh:mm:ss.zzz" );

  if( _useStderr ) {
    qDebug() << "          (app)" << message;    
  }
  
  if( LoggingPending == _logLevel ) {
    msg = new CLogMessage( logLevel, QString( "%1: %2" ).arg( dt ).arg( message ) );
    _pending->append( msg );    
  }
  else if( _logOpen && ( logLevel <= _logLevel ) ) {
    *_logTextStream << ::endl << dt << ": " << message << ::flush;
    ++_logLineCount;
    if( _autoTruncate && (10000 < _logLineCount) ) {
      truncateLogFile();
    } 
  }
}


CAppLog& CAppLog::operator<<( const QString& message ) {
  this->logMessage( message.trimmed(), LoggingTypical );
  if( _consoleEcho ) {
    cout << message << ::flush;
  }

  return *this;
}


CAppLog& CAppLog::operator<<( const char* message ) {
  this->logMessage( QString( "%1" ).arg( message ), LoggingTypical );
  if( _consoleEcho ) {
    cout << message << ::flush;
  }

  return *this;
}


CAppLog& CAppLog::operator<<( QTextStream&(*f)(QTextStream&) ) {
  if( _consoleEcho ) {
    if( f == ::endl ) {
      cout << ::endl;
    }
    else if( f == ::flush ) {
      cout << ::flush;
    }
  }

  return *this;
}


void CAppLog::processPendingMessages( void ) {
  CLogMessage* msg;

  if( NULL != _pending ) {
    while( !( _pending->isEmpty() ) ) {
      msg = _pending->takeFirst();
      if( _logOpen ) {
        if( msg->_level >= _logLevel ) {
          *_logTextStream << ::endl << msg->_msg << ::flush;
          ++_logLineCount;
          if( _autoTruncate && (10000 < _logLineCount) ) {
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










