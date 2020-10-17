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

#include <QtCore>
#include <QDebug>
#include <ar_general_purpose/arcommon.h>

#ifdef QCONCURRENT_USED
  #include <QBasicMutex>
  static QBasicMutex _mutex; // Note: NOT recursive, so don't lock muliple times.
#endif

CAppLog appLog;
CLockFile lockFile;

void logMsg( const char* msg, const LogLevel logLevel /* = LoggingTypical */ ) {
  #ifdef QCONCURRENT_USED
    _mutex.lock();
  #endif

  appLog.logMessage( msg, logLevel );

  #ifdef QCONCURRENT_USED
    _mutex.unlock();
  #endif
}

void logMsg( const QString& msg, const LogLevel logLevel /* = LoggingTypical */ ) {
  #ifdef QCONCURRENT_USED
    _mutex.lock();
  #endif

  appLog.logMessage( msg, logLevel );

  #ifdef QCONCURRENT_USED
    _mutex.unlock();
  #endif
}

void logMsgUnique( const QString& msg, const LogLevel logLevel /* = LoggingTypical */ ) {
  #ifdef QCONCURRENT_USED
    _mutex.lock();
  #endif

  appLog.logMessageUnique( msg, logLevel );

  #ifdef QCONCURRENT_USED
    _mutex.unlock();
  #endif
}

void logMsg( const QStringList& msgs, const LogLevel logLevel /* = LoggingTypical */ ) {
  #ifdef QCONCURRENT_USED
    _mutex.lock();
  #endif

  for( int i = 0; i < msgs.count(); ++i ) {
    appLog.logMessage( msgs.at(i), logLevel );
  }

  #ifdef QCONCURRENT_USED
    _mutex.unlock();
  #endif
}


void logVerbose( const QString& msg ) {
  #ifdef QCONCURRENT_USED
    _mutex.lock();
  #endif

  appLog.logMessage( msg, LoggingVerbose );

  #ifdef QCONCURRENT_USED
    _mutex.unlock();
  #endif
}


void logBlank( const LogLevel logLevel /* = LoggingTypical */ ) {
  #ifdef QCONCURRENT_USED
    _mutex.lock();
  #endif

  appLog.logMessage( QString(), logLevel );

  #ifdef QCONCURRENT_USED
    _mutex.unlock();
  #endif
}


#ifdef QSQL_USED
void logFailedQuery( QSqlQuery* query, const char* description /* = "Query" */ ) {
  logFailedQuery( query, QString( description ) );
}


void logFailedQuery( QSqlQuery* query, const QString& description /* = "Query" */ ) {
  #ifdef QCONCURRENT_USED
    _mutex.lock();
  #endif

  QString msg = QStringLiteral( ">>> %1 failed:\n" ).arg( description );
  msg.append( prettyPrint( query->lastQuery().simplified().trimmed(), 100, false, false, 2 ) );
  msg.append( '\n' );
  msg.append( QStringLiteral( "%1 %2" ).arg( prettyPrint( query->lastError().text(), 100, false, false, 2 ), QStringLiteral("<<< (End)\n") ) );

  appLog.logMessage( msg, LoggingTypical );

  #ifdef QCONCURRENT_USED
    _mutex.unlock();
  #endif
}
#endif


CLogMessage::CLogMessage( const int level, const QString& msg ) {
  _level = level;
  _msg = msg; 
}


CAppLog::CAppLog() : QObject() { 
  initialize(); 
}


CAppLog::CAppLog(
    const QString& fileName,
    const LogLevel logLevel,
    const FileFrequency freq /* = OneFile */,
    const bool userSpacerLine /* = true */,
    QObject* parent /* = nullptr */
) : QObject( parent ) {
  initialize();
  _useSpacerLine = userSpacerLine;

  openLog( fileName, logLevel, freq );
}


void CAppLog::initialize() {
  _logOpen = false;
  _logFileName = QString();
  _logPath = QString();

  _logFile = nullptr;
  _logTextStream = nullptr;
  _pending = nullptr;

  _logLineCount = 0;

  _useStderr = false;
  _autoTruncate = false;

  _freq = OneFile;

  _consoleEcho = false;

  _windowsFriendly = false;

  _useMessageList = false;

  _useSpacerLine = true;

  setLogLevel( LoggingPending );
}


CAppLog::~CAppLog() {
  closeLog();
  
  if( nullptr != _pending ) {
    qDeleteAll( _pending->begin(), _pending->end() );
    delete _pending;
    _pending = nullptr;
  }  
}


bool CAppLog::openLog( const QString& fileName, const LogLevel logLevel, const FileFrequency freq /* = OneFile */ ) {
  bool result = false;

  if( this->isOpen() && ( fileName == _logFileName ) && ( logLevel == _logLevel ) && ( freq == _freq ) ) {
    result = _logOpen;
  }
  else {
    setFileFrequency( freq );
    setFileName( fileName );
    setLogLevel( logLevel );

    result = _logOpen;
  }
  return result;
}


void CAppLog::setFileName( const QString& fileName ) {
  QString fn = fileName;
  QFileInfo fi( fn );

  switch ( _freq ) {
    case NewFileEveryTime:
      fn = QStringLiteral( "%1/%2-%3" ).arg( fi.absolutePath(), QDateTime::currentDateTime().toString( QStringLiteral("yyyyMMddhhmmss") ), fi.fileName() );
      break;
    case DailyFile:
      fn = QStringLiteral( "%1/%2-%3" ).arg( fi.absolutePath(), QDateTime::currentDateTime().toString( QStringLiteral("yyyyMMdd") ), fi.fileName() );
      break;
    case OneFile:
      fn = QStringLiteral( "%1/%2" ).arg( fi.absolutePath(), fi.fileName() );
      break;
  }

  _logFileName = fn;
  _logPath = fi.absolutePath();
}

void CAppLog::setLogLevel( const LogLevel logLevel ) {
  _logLevel = logLevel;
  
  // If logging is set to "pending", set up the message list
  // and store messages until a decision is made about what to do with them.
  if( ( LoggingPending == _logLevel ) ) {
    if( _logOpen ) {
      closeLog();
    }
    if( nullptr == _pending ) {
      _pending = new TLogMessageList();
    }
  }
  
  // If logging is set to "none", close the log file
  // and throw away the list of pending log messages.
  else if( LoggingNone == _logLevel ) {
    if( _logOpen ) {
      closeLog();
    }
    if( nullptr != _pending ) {
      qDeleteAll( _pending->begin(), _pending->end() );
      delete _pending;
      _pending = nullptr;
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
        //qDb() << "Log file is not open!";
      }
    }
  }
}


bool CAppLog::openLog() {
    _logFile = new QFile( _logFileName );

    if( _autoTruncate )
      truncateLogFile();

    if( _logFile->open( QIODevice::WriteOnly | QIODevice::Append ) ) {
      _logTextStream = new QTextStream( _logFile );

      if( _useSpacerLine ) {
        if( _windowsFriendly )
          *_logTextStream << "\r\n" << ::flush;
        else
          *_logTextStream << ::endl << ::flush;
      }

      //qDb() << "Log file is open.";
      return true;
    }
    else {
      //qDb() << "Log file did not open!";
      delete _logFile;
      _logFile = nullptr;
      _logTextStream = nullptr;
      return false;
    }
}


void CAppLog::closeLog() {
  if( nullptr != _logFile ) {
    _logFile->close();
    delete _logFile;
    _logFile = nullptr;
  }
  
  if( nullptr != _logTextStream ) {
    delete _logTextStream;
    _logTextStream = nullptr;
  }
  
  _logOpen = false;
}


void CAppLog::cout() {
  for( int i = 0; i < this->_pending->count(); ++i ) {
    ::cout << _pending->at(i)->_msg << endl << ::flush;
  }
}


// Keep the last 3000 lines of the file, and eliminate the rest.
void CAppLog::truncateLogFile() {
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
    QString dt = QDateTime::currentDateTime().toString( QStringLiteral("yyyy-MM-dd hh:mm:ss.zzz") );
  
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



QString CAppLog::makeWindowsFriendly( QString message ) {
  message.remove( '\r' );
  message.replace( '\n', QLatin1String("\r\n") );

  return message;
}

void CAppLog::logMessageUnique( const QString& message, const LogLevel logLevel ) {
  switch( logLevel ) {
    case LoggingTypical:
      if( !_messagesUniqueTypical.contains( message ) ) {
        _messagesUniqueTypical.insert( message );
        logMessage( message, logLevel );
      }
      break;
    case LoggingVerbose:
      if( !_messagesUniqueVerbose.contains( message ) ) {
        _messagesUniqueVerbose.insert( message );
        logMessage( message, logLevel );
      }
      break;
    case LoggingPending:
      logMessage( message, logLevel );
      break;
    case LoggingNone:
      // Do nothing: Discard the message.
      break;
  }
}


void CAppLog::logMessage( QString message, const LogLevel logLevel ) {
  CLogMessage* msg;
  QString dt = QDateTime::currentDateTime().toString( QStringLiteral("yyyy-MM-dd hh:mm:ss.zzz") );
  QString str;

  if( _useMessageList ) {
    _messageList.append( message );
  }

  if( _useStderr ) {
    qDb() << "          (log)" << message;
  }

  if( _consoleEcho ) {
    ::cout << message << endl << ::flush;
  }
  
  if( _windowsFriendly ) {
    message = makeWindowsFriendly( message );
  }

  if( LoggingPending == _logLevel ) {
    if( !message.isEmpty() ) {      
      str = QStringLiteral( "%1: %2" ).arg( dt, message );
    }
    else {
      str = QString();
    }

    msg = new CLogMessage( logLevel, str );
    _pending->append( msg );    
  }
  else if( _logOpen && ( logLevel <= _logLevel ) ) {
    if( !message.isEmpty() ) {
      if( _windowsFriendly )
        *_logTextStream << "\r\n" << dt << ": " << message << ::flush;
      else
        *_logTextStream << ::endl << dt << ": " << message << ::flush;
    }
    else {
      if( _windowsFriendly )
        *_logTextStream << "\r\n" << message << ::flush;
      else
        *_logTextStream << ::endl << message << ::flush;
    }

    ++_logLineCount;
    if( _autoTruncate && (10000 < _logLineCount) ) {
      truncateLogFile();
    } 
  }

  emit messageLogged( message );
}


CAppLog& CAppLog::operator<<( const QString& message ) {
  _msgInProgress.append( message );

  return *this;
}


CAppLog& CAppLog::operator<<( const char* message ) {
  _msgInProgress.append( message );

  return *this;
}


CAppLog& CAppLog::operator<<( const int number ) {
  _msgInProgress.append( QString::number( number ) );

  return *this;
}


CAppLog& CAppLog::operator<<( const long long number ) {
  _msgInProgress.append( QString::number( number ) );

  return *this;
}


CAppLog& CAppLog::operator<<( const double number ) {
  _msgInProgress.append( QString::number( number ) );

  return *this;
}


CAppLog& CAppLog::operator<<( QTextStream&(*f)(QTextStream&) ) {
  if( (f == ::endl) || (f == ::flush) ) {
    this->logMessage( _msgInProgress, LoggingTypical );
    _msgInProgress.clear();
  }
  else {
    // do nothing.
  }

  return *this;
}


void CAppLog::processPendingMessages() {
  CLogMessage* msg;

  if( nullptr != _pending ) {
    while( !( _pending->isEmpty() ) ) {
      msg = _pending->takeFirst();
      if( _logOpen ) {
        if( msg->_level >= _logLevel ) {
          if( _windowsFriendly ) {
            *_logTextStream << "\r\n" << msg->_msg << ::flush;
          }
          else {
            *_logTextStream << ::endl << msg->_msg << ::flush;
          }

          ++_logLineCount;
          if( _autoTruncate && (10000 < _logLineCount) ) {
            truncateLogFile();
          }  
        }  
      }
      delete msg;  
    } 
    
    delete _pending;
    _pending = nullptr;
  }
}


CLockFile::CLockFile() {
  _useLockFile = false;
  _fileName = QString();
  _path = QString();
}


bool CLockFile::setFileName( const QString& fileName ) {
  _fileName = fileName;
  QFileInfo fi( _fileName );
  _path = fi.absolutePath();
  _useLockFile = true;

  return( !fi.exists() && fi.isWritable() );
}


bool CLockFile::useLockFile() {
  return _useLockFile;
}


bool CLockFile::exists() {
  return QFileInfo::exists( _fileName );
}


bool CLockFile::write() {
  QFile data( _fileName );
  if( data.open( QFile::WriteOnly | QFile::Truncate ) ) {
    QTextStream out( &data );
    out << QDateTime::currentDateTime().toString( QStringLiteral("yyyy-MM-dd hh:mm:ss:zzz") );
    return true;
  }
  else {
    return false;
  }
}


bool CLockFile::remove() {
  return QFile::remove( _fileName );
}


QString CLogFileContents::trimMatch( QString line, const QRegExp& exp ) {
  // Trim off the indicated bits that might appear at the beginning of the line
  //---------------------------------------------------------------------------
  exp.exactMatch( line );
  int matchLen1 = exp.matchedLength();
  line = line.right( line.length() - matchLen1 );

  return line.trimmed();
}


QString CLogFileContents::processLine( const QString& line, const bool useDetails ) {
  if( line.trimmed().isEmpty() )
    return QString();

  QString msg;

  // My log files sometimes indicate multi-line messages
  // with ">>>" at the beginning of a block and "<<< (End)" at the end.
  // This will format such text blocks appropriately.
  //-------------------------------------------------------------------
  if( line.contains( QLatin1String(">>>") ) ) {
    QStringList list = line.split( '\n' );

    if( useDetails && ( 2 < list.count() ) ) {
      msg = QStringLiteral( "%1 | %2" ).arg( list.at(1).trimmed(), list.at(2).trimmed() );
    }
    else {
      msg = list.at(1).trimmed();
    }

    msg = trimMatch( msg, QRegExp( "^ERROR[:]" ) );
  }

  // Otherwise, my log files begin with "Line xxx:" or "Lines xxx, yyy:".
  // This statement strips this bit of noise away.
  //---------------------------------------------------------------------
  else if( line.startsWith( QStringLiteral("Lines") ) ) {
    msg = trimMatch( line, QRegExp( "^Lines\\s[0-9]+\\s?[,]\\s?[0-9]+[:]" ) );
  }
  else if( line.startsWith( QStringLiteral("Line") ) ) {
    msg = trimMatch( line, QRegExp( "^Line\\s[0-9]+[:]" ) );
  }
  else {
    msg = line;
  }

  msg.replace( QRegExp( "line\\s[0-9]+" ), QStringLiteral("line x") );

  msg.replace( QRegExp( "[(]detail:\\s+[0-9a-zA-Z/\\s:=_.-,]+[)]" ), QStringLiteral("(detail: x)") );

  return msg;
}


void CLogFileContents::generateSummary() {
  QMultiHash<int, QString> countHash;
  QVector<int> counts;
  _maxCount = 0;

  // The number of entries required won't be any greater than this.
  counts.reserve( _hash.count() );

  QHashIterator<QString, int> it( _hash );
  while( it.hasNext() ) {
    it.next();
    countHash.insert( it.value(), it.key() );
    _maxCount = std::max( _maxCount,  it.value() );
    if( !counts.contains( it.value() ) )
      counts.append( it.value() );
  }

  counts.squeeze();

  std::sort( counts.begin(), counts.end() );

  for( int i = counts.count() - 1; i > -1; --i ) {
    int key = counts.at( i );
    QList<QString> items = countHash.values( key );
    std::sort( items.begin(), items.end() );
    for( int j = 0; j < items.count(); ++j ) {
      _entryCounts.append( key );
      _entries.append( items.at(j) );
    }
  }
}


CLogFileContents::CLogFileContents( const QString& filename, const bool saveFullContents, const bool includeQueryDetails ) {
  _result = ReturnCode::SUCCESS;

  QFile f( filename );
  if( ! f.open( QFile::ReadOnly ) ) {
    _result = ReturnCode::INPUT_FILE_PROBLEM;
    return;
  }

  QTextStream stream( &f );
  QString origLine, origLine2, line, line2;
  do {
    QRegExp timeStamp( "^[0-9]{4}[-][0-9]{2}[-][0-9]{2}[\\s][0-9]{2}[:][0-9]{2}[:][0-9]{2}[.][0-9]{3}[:]" );

    origLine = stream.readLine();
    line = trimMatch( origLine, timeStamp );

    if( "(No errors encountered)" == line )
      continue;

    // My log files sometimes indicate multi-line messages
    // with ">>>" at the beginning of a block and "<<< (End)" at the end.
    // This will format such text blocks appropriately.
    //--------------------------------------------------------------------
    if( line.contains( QStringLiteral(">>>") ) ) {
      do {
        origLine2 = stream.readLine();
        line2 = trimMatch( origLine2, timeStamp );
        line.append( "\n" );
        line.append( line2 );
        origLine.append( "\n" );
        origLine.append( origLine2 );
      } while( !line2.contains( QStringLiteral("<<<") ) );
    }

    if( saveFullContents )
      _fullContents.append( origLine );

    QString msg = processLine( line, includeQueryDetails );

    if( _hash.contains( msg ) )
      _hash[msg] = _hash.value( msg ) + 1;
    else
      _hash.insert( msg, 1 );

  } while( !line.isNull() );

  generateSummary();
}


void CLogFileContents::writeSummaryToStream( QTextStream* stream ) {
  int maxLen = QStringLiteral( "%1" ).arg( _maxCount ).length();

  QString keyStr;
  for( int i = 0; i < _entries.count(); ++i ) {
    keyStr = keyStr = rightPaddedStr( QString::number( _entryCounts.at(i) ), maxLen );
    *stream << keyStr << ": '" << _entries.at(i) << "'" << endl;
  }
}


void CLogFileContents::writeFilteredToStream( QString filter, QTextStream* stream, const bool useDetails ) {
  bool useStringFilter = false;
  int n = 0;
  bool gt = false;
  bool lt = false;
  bool eq = false;


  if( QRegExp( "^[<>=][0-9]+$" ).exactMatch( filter ) ) {
    gt = filter.startsWith( '>' );
    lt = filter.startsWith( '<' );
    eq = filter.startsWith( '=' );
    n = filter.right( filter.length() - 1 ).toInt();
  }
  else if( filter.startsWith( '=' ) ) {
    useStringFilter = true;
    filter = filter.right( filter.length() - 1 );
  }

  QString str;
  QRegExp timeStamp( "^[0-9]{4}[-][0-9]{2}[-][0-9]{2}[\\s][0-9]{2}[:][0-9]{2}[:][0-9]{2}[.][0-9]{3}[:]" );

  for( int i = 0; i < _fullContents.count(); ++i ) {    
    str = processLine( trimMatch( _fullContents.at(i), timeStamp ), useDetails );

    if( useStringFilter ) {
      if( 0 != filter.compare( str ) ) {
        *stream << _fullContents.at(i) << endl;
      }
    }
    else if( gt ) {
      if( _hash.value( str ) <= n ) {
        *stream << _fullContents.at(i) << endl;
      }
    }
    else if( lt ) {
      if( _hash.value( str ) >= n ) {
        *stream << _fullContents.at(i) << endl;
      }
    }
    else if( eq ) {
      if( _hash.value( str ) != n ) {
        *stream << _fullContents.at(i) << endl;
      }
    }
  }
}


QStringList CLogFileContents::summary() const {
  QStringList result;

  int maxLen = QStringLiteral( "%1" ).arg( _maxCount ).length();

  QString keyStr;
  for( int i = 0; i < _entries.count(); ++i ) {
    keyStr = keyStr = rightPaddedStr( QString::number( _entryCounts.at(i) ), maxLen );
    result.append( QStringLiteral( "%1: %2" ).arg( keyStr, _entries.at(i) )  );
  }

  return result;
}




