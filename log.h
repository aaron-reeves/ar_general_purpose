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

#ifndef LOG_H
#define LOG_H

#include <qfile.h>
#include <qstring.h>
#include <qtextstream.h>
#include <qlist.h>

class CAppLog;

extern CAppLog appLog;

enum LogLevel {
  LoggingPending,
  LoggingNone,
  LoggingTypical,
  LoggingVerbose
};


void logMsg( const QString& msg, const LogLevel logLevel = LoggingTypical );
void logMsg( const QStringList& msgs, const LogLevel logLevel = LoggingTypical );
void logVerbose( const QString& msg );
void logBlank( const LogLevel logLevel = LoggingTypical );


class CLogMessage {
  public:
    CLogMessage( const int level, const QString& msg );
    int _level;
    QString _msg; 
};


typedef QList<CLogMessage*> TLogMessageList;

class CAppLog {
  public:
    
    enum FileFrequency {
      OneFile,
      DailyFiles
    };

    // Creates a log that won't actually record anything.
    CAppLog( void );
    
    // Creates a log with the indicated file name, that will eventually be written to.
    CAppLog( QString fileName, const int logLevel, const FileFrequency freq = OneFile );
    
    void openLog( QString fileName, const int logLevel, const FileFrequency freq = OneFile );
    
    virtual ~CAppLog( void );
    
    void setLogLevel( const int logLevel );
    void setFileFrequency( const FileFrequency freq ) { _freq = freq; }
    void setFileName( QString fileName );
    void setUseStderr( const bool& val ) { _useStderr = val; }
    void setAutoTruncate( const bool& val ) { _autoTruncate = val; }
    void setConsoleEcho( const bool& val ) { _consoleEcho = val; }
    
    void logMessage( const QString& message, const int logLevel ); 
    void typical( const QString& message ) { logMessage( message, LoggingTypical ); }
    void verbose( const QString& message ) { logMessage( message, LoggingVerbose ); } 
    
    CAppLog& operator<<( const QString& message );
    CAppLog& operator<<( const char* message );
    CAppLog& operator<<( const int number );
    CAppLog& operator<<( QTextStream&(*f)(QTextStream&) );

  protected:
    void initialize();

    bool openLog( void );
    void closeLog( void ); 
    void truncateLogFile( void );
    void processPendingMessages( void );
    
    QFile* _logFile;
    QTextStream* _logTextStream;
    
    int _logLevel;
    bool _logOpen;
    int _logLineCount;
    QString _logFileName; 
    TLogMessageList* _pending;
    bool _useStderr;
    bool _autoTruncate;
    FileFrequency _freq;
    bool _consoleEcho;
    QString _msgInProgress;
};

#endif // LOG_H
