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

#include <QtCore>

#ifdef QSQL_USED
  #include <QSqlQuery>
  #include <QSqlError>
#endif

class CLockFile;

extern CLockFile lockFile;

class CAppLog;

extern CAppLog appLog;

enum LogLevel {
  LoggingPending,
  LoggingNone,
  LoggingTypical,
  LoggingVerbose
};

// The logMsgx functions are thread-safe.
// Do not call any appLog functions directly from different threads.
// See below for more details.
void logMsg( const char* msg, const LogLevel logLevel = LoggingTypical );
void logMsg( const QString& msg, const LogLevel logLevel = LoggingTypical );
void logMsg( const QStringList& msgs, const LogLevel logLevel = LoggingTypical );
void logMsgUnique( const QString& msg, const LogLevel logLevel = LoggingTypical );
void logVerbose( const QString& msg );
void logBlank( const LogLevel logLevel = LoggingTypical );

#ifdef QSQL_USED
// Thread-safe
void logFailedQuery( QSqlQuery* query, const char* description = "Query" );
void logFailedQuery( QSqlQuery* query, const QString& description = QStringLiteral("Query") );
#endif

class CLogMessage {
  public:
    CLogMessage( const int level, const QString& msg );
    int _level;
    QString _msg; 
};


typedef QList<CLogMessage*> TLogMessageList;


// WARNING:
// Do not call any appLog functions directly from different threads.
// Ensure that the master appLog is set up by the main thread, and
// that any other instances are entirely self-contained within a single thread.
// Once established, the master appLog can be accessed via the logMsgx functions
// shown above, which are protected by a mutex and are thread-safe.
class CAppLog : public QObject {
  Q_OBJECT

  public:   
    enum FileFrequency {
      OneFile,
      DailyFile,
      NewFileEveryTime
    };

    // Creates a log that won't actually record anything.
    CAppLog();
    
    // Creates a log with the indicated file name, that will eventually be written to.
    CAppLog(
      const QString& fileName,
      const LogLevel logLevel,
      const FileFrequency freq = OneFile,
      const bool userSpacerLine = true,
      QObject* parent = nullptr
    );
    
    bool openLog( const QString& fileName, const LogLevel logLevel, const FileFrequency freq = OneFile );
    void closeLog();
    
    virtual ~CAppLog();
    
    void setLogLevel(const LogLevel logLevel );
    void setFileFrequency( const FileFrequency freq ) { _freq = freq; }
    void setFileName( const QString& fileName );
    void setUseStderr( const bool val ) { _useStderr = val; }
    void setAutoTruncate( const bool val ) { _autoTruncate = val; }
    void setConsoleEcho( const bool val ) { _consoleEcho = val; }
    void setWindowsFriendly( const bool val ) { _windowsFriendly = val; }
    void setUseSpacerLine( const bool val ) { _useSpacerLine = val; }

    void logMessageUnique( const QString& message, const LogLevel logLevel );
    void logMessage( const char* message, const LogLevel logLevel ) { logMessage( QString( message ), logLevel ); }
    void logMessage( QString message, const LogLevel logLevel );
    void typical( const char* message ) { logMessage( QString( message ), LoggingTypical ); }
    void typical( const QString& message ) { logMessage( message, LoggingTypical ); }
    void verbose( const char* message ) { logMessage( QString( message ), LoggingVerbose ); }
    void verbose( const QString& message ) { logMessage( message, LoggingVerbose ); } 
    
    const QString fileName() const { return _logFileName; }
    const QString path() const { return _logPath; }

    bool isOpen() const { return _logOpen; }

    QStringList messageList() const { return _messageList; }
    void startMessageList() { _useMessageList = true; }
    void stopMessageList() { _useMessageList = false; }
    void clearMessageList() { _messageList.clear(); }
    void clearUniqueMessageLists() { _messagesUniqueTypical.clear(); _messagesUniqueVerbose.clear(); }

    CAppLog& operator<<( const QString& message );
    CAppLog& operator<<( const char* message );
    CAppLog& operator<<( const int number );
    CAppLog& operator<<( const double number );
    CAppLog& operator<<( const long long number );
    CAppLog& operator<<( QTextStream&(*f)(QTextStream&) );

    void cout();

  signals:
    void messageLogged( QString msg );

  protected:
    void initialize();

    bool openLog();
    void truncateLogFile();
    void processPendingMessages();
    QString makeWindowsFriendly( QString message );
    
    QFile* _logFile;
    QTextStream* _logTextStream;
    
    LogLevel _logLevel;
    bool _logOpen;
    int _logLineCount;
    QString _logFileName; 
    QString _logPath;
    TLogMessageList* _pending;
    bool _useStderr;
    bool _autoTruncate;
    FileFrequency _freq;
    bool _consoleEcho;
    bool _windowsFriendly;
    QString _msgInProgress;

    QSet<QString> _messagesUniqueTypical;
    QSet<QString> _messagesUniqueVerbose;

    bool _useMessageList;
    QStringList _messageList;

    bool _useSpacerLine;
};


// Not thread-safe: if needed, set up and remove a lock file from the main thread.
class CLockFile {
  public:
    CLockFile();

    bool setFileName( const QString& fileName );
    bool useLockFile();
    bool exists();
    bool write();
    bool remove();

    QString fileName() const { return _fileName; }
    QString path() const { return _path; }
    bool used() const { return _useLockFile; }

  protected:
    bool _useLockFile;
    QString _fileName;
    QString _path;
};


// Not thread-safe: if needed, set up and use from a single thread.
class CLogFileContents {
  public:
    CLogFileContents( const QString& filename, const bool saveFullContents, const bool includeQueryDetails );

    int result() const { return _result; }
    const QStringList& logContents() const { return _fullContents; }
    QStringList summary() const;
    void writeSummaryToStream( QTextStream* stream );
    void writeFilteredToStream( QString filter, QTextStream* stream , const bool useDetails );

  protected:
    QString processLine( const QString& line, const bool useDetails );
    QString trimMatch( QString line, const QRegExp& exp );
    void generateSummary();

    int _result;
    QStringList _fullContents;

    QVector<int> _entryCounts;
    QStringList _entries;
    int _maxCount;

    QHash<QString, int> _hash;
};


#endif // LOG_H
