/*
csql.h
------
Begin: 2004/07/28
Last revision: $Date: 2011-10-25 04:57:17 $ $Author: areeves $
Version: $Revision: 1.8 $
Project: JetSQLConsole
Website: http://www.aaronreeves.com/jetsqlconsole
Author: Aaron Reeves <aaron@aaronreeves.com>
--------------------------------------------------
Copyright (C) 2004 - 2014 Aaron Reeves

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#ifndef CSQL_H
#define CSQL_H

#include <qlist.h>
#include <qmap.h>
#include <qstring.h>
#include <qstringlist.h>

#ifdef __linux__
#include <qsqldatabase.h>
#endif

#ifdef _WIN32
#include <cadosql.h>
#endif

#include "cqstringlist.h"
#include "creverselookupmap.h"

class CSqlDatabase;
class CSqlResult;
class CSqlRow;
class CSqlDatabaseConnectionParams;

typedef CReverseLookupMap<QString, int unsigned> TFieldMap;

typedef QList<CSqlRow*> TRowList;


// This seems a little silly, but without it, I get several linker errors on Windows:
// "QMap<QString, QString> already defined" (or something to that effect).
class QAnotherString : public QString{
  public:
    QAnotherString( void ) : QString() {}
    QAnotherString( char* val ) : QString( val ) {}
    QAnotherString( QString val ) : QString( val ) {}
};

typedef QMap<QAnotherString, QAnotherString> TQueryMap;



class CSqlFieldInfo {
  public:
    CSqlFieldInfo( void );
    // use default destructor

    inline void setFieldName( QString val ) { _fieldName = val; }
    inline void setFieldType( QString val ) { _fieldType = val; }
    inline void setFieldSize( int val ) { _fieldSize = val; }
    inline void setAutoIncrement( bool val ) { _autoIncrement = val; }
    inline void setFieldDescr( QString val ) { _fieldDescr = val; }

    inline QString fieldName( void ) { return _fieldName; }
    inline QString fieldType( void ) { return _fieldType; }
    inline int fieldSize( void ) { return _fieldSize; }
    inline bool autoIncrement( void ) { return _autoIncrement; }
    inline QString fieldDescr( void ) { return _fieldDescr; }

    QString fieldTypeDescr( void );
  protected:
    QString _fieldName;
    QString _fieldType;
    int _fieldSize;
    bool _autoIncrement;
    QString _fieldDescr;
};


class CFieldInfoList : public QList<CSqlFieldInfo*> {
  public:
    virtual ~CFieldInfoList( void );  
};

/**
CSqlDatabase and related classes provide a unified interface to Microsoft Access and MySQL databases
from Windows and Linux (MySQL only).  The interface is intended to mimic the PHP database API.

CSqlDatabase provides functions that are effective at the level of the entire database.  An instance
of CSqlDatabase is required for all other CSql* classes.

@short A cross-platform database API patterned on PHP database functions.
@author Aaron Reeves (aaron@aaronreeves.com)
@version 0.9.1
*/
class CSqlDatabase {
  public:

    enum DatabaseTypes {
      DBUnspecified,
      DBMySQL,
      DBMSAccess,
      DBSqlite,
      DBDbf,
      DBPostgres
    };

    enum DatabaseActions {
      DBNoAction,
      DBOpen,
      DBCreate
    };

    enum DatabaseQueryTypes {
      DBUnspecifiedQuery,
      DBInsert,
      DBUpdate
    };

    // Constructs but does not open a database.  Requires use of properties (dbType, dbName, etc.)
    CSqlDatabase( void );

    // Constructs an instance of type dbType (see enum DatabaseTypes).
    // if dbAction is set to DBOpen, an existing database is opened.  If dbAction is DBCreate,
    // an attempt is made to create a new database (for MS Access).
    CSqlDatabase( 
      int dbType, 
      const QString& dbName, 
      const int dbAction = 0, 
      const QString& dbUser = "", 
      const QString& dbPassword = "", 
      const QString& dbHost = "", 
      const int& dbPort = 3306 
    );
    
    CSqlDatabase( CSqlDatabaseConnectionParams* params );

		virtual ~CSqlDatabase( void );

    void open();
    void close( void );

    bool lockTableRead( const QString& tableName );
    bool lockTableWrite( const QString& tableName );
    bool unlockTables( void );

    int unsigned lastInsertID( void );
    
    int unsigned recordID( const QString tableName, const QString idFieldName, TQueryMap map, QString* returnQuery = NULL ); 

    inline bool isOpen( void ) { return _isOpen; }

    bool execute( const QString &query, int unsigned* affectedRows = NULL );

    QStringList* newTableList( void );

    QStringList* newSystemTableList( void );

    CFieldInfoList* newFieldInfoList( QString tableName, bool* success = NULL );

    QString sqlQuote( const QString val );
    static QString sqlQuote( const QString val, const int dbFormat );
    
    QString delimitedName( const QString val );
    static QString delimitedName( const QString val, const int dbFormat );

    QString sqlBool( const bool val );
    static QString sqlBool( const bool val, const int dbFormat );
    
    QString writeQuery(
    	const QString tableName,
    	const int queryType,
    	TQueryMap map,
    	const QString whereClause = ""
    );

    QString lastError( void );

    int numRows( const QString& query );

    //QString tableDescr( QString tableName );
    bool tableExists( const QString& tableName );

    inline int type( void ) { return _type; }
    inline int action( void ) { return _action; }
    inline QString name( void ) { return _name; }
    inline QString user( void ) { return _user; }
    inline QString password( void ) { return _password; }
    inline QString host( void ) { return _host; }
    inline int port() { return _port; }
    inline void setType( int val ) { _type = val; }
    inline void setAction( int val ) { _action = val; }
    inline void setName( QString val ) { _name = val; }
    inline void setUser( QString val ) { _user = val; }
    inline void setPassword( QString val ) { _password = val; }
    inline void setHost( QString val ) { _host = val; }
    inline void setPort( int val ) { _port = val; }

  protected:
    void initialize( void );

    QString fieldTypeDescr( short nType );

    bool _isOpen;
    int _type;
    int _action;
    int _port;
    QString _name;
    QString _user;
    QString _password;
    QString _host;

    #ifdef _WIN32
    TAdoDatabase _adoConn; // alias for void*: see sqldll.h
    TAdoxCatalog _adoCat; // alias for void*: see sqldll.h
    TAdoRecordset _adoRS; // alias for void*: see sqldll.h
    //TAdoCommand _adoCmd; // currently unused
    QString _adoConnStr;
    #endif

    #ifdef __linux__
    QSqlDatabase* _qConn;
    #endif

  friend class CSqlResult;
};



class CSqlRow : public CQStringList  {
  public:
    CSqlRow( TFieldMap* fm );

		virtual ~CSqlRow( void ); // DON'T delete the field map!

    QString* field( int fieldIndex );
    QString* field( const QString& fldName );

    void debug( void );

  private:
    TFieldMap* _fm;
};



typedef QListIterator<QString*> CSqlRowIterator;


class CSqlQueryString {
  public:
    static bool isSelect( const QString &query );
};


class CSqlResult {
  public:  
    CSqlResult( const QString &query, CSqlDatabase* dbConn );
    CSqlResult( const char* query, CSqlDatabase* dbConn );
    CSqlResult( CSqlDatabase* dbConn );
    CSqlResult( void );

		virtual ~CSqlResult( void ); // There will be a lot of lists to clear out.

    // Number of rows fetched by a SELECT
    long numRows( void ) { return _numRows; }

    // Number of rows affected by an INSERT, UPDATE, or DELETE
    int unsigned affectedRows( void ) { return _affectedRows; }

    // Number of fields in a record set
    int unsigned numFields( void ) { return _numFields; }

    CSqlRow* fetchArrayFirst( void );
    CSqlRow* fetchArrayNext( void );
    CSqlRow* fetchArrayLast( void );

    // I'm not sure that I'm crazy about these two options: think about them a little more.
    //CSqlRow* fetchArray( void ); // fetch the current array, whatever it is, or the first one if there is no current array.
    CSqlRow* fetchArray( int unsigned index );

    bool success( void ) { return _success; }

    inline void setConnection( CSqlDatabase* conn ) { _conn = conn; }
    inline CSqlDatabase* connection( void )  { return _conn; }

    // Shortcut, to avoid having to create a new instance of CSqlResult every time
    void runQuery( const QString &query );

    // Shortcut, to avoid having to create a new instance of CSqlResult every time
    void runQuery( char* query );

    int unsigned fieldCount( void );

    QString fieldName( int unsigned fieldIndex );
    //QString fieldType( int fieldIndex );

    void primaryKeys( void );
    void foreignKeys( void );
    void indices( void );

    QString lastError( void );
    
    void debug( void );
    
  private:
    enum KeyTypes {
      keyTypePrimary,
      keyTypeForeign,
      keyTypeIndex
    };

    // mySQLQuery( const QString& query, bool* success = NULL );
    // jetSQLQuery( const QString& query, bool* success = NULL );

    void initialize( void );

    void clear( void );

    void runQuery( const QString &query, CSqlDatabase* dbConn );
    void runSelectQuery( const QString &query, CSqlDatabase* dbConn );
    void runOtherQuery( const QString &query, CSqlDatabase* dbConn );

    #ifdef __linux__
    void runSelectQueryLinux( const QString &query, CSqlDatabase* dbConn );
    #endif

    bool isOther( const QString &query );

    void keys( const int keyType );

    TRowList* rs;
    int _currentIndex;

    TFieldMap* fieldMap; // build the field map before populating rows

    long _numRows;
    int unsigned _affectedRows;
    int unsigned _numFields;
    bool _success;
    CSqlDatabase* _conn;

  friend class CSqlRow; // needs access to fieldMap
};


class CSqlDatabaseConnectionParams {
  public:
    CSqlDatabaseConnectionParams( void );
    virtual ~CSqlDatabaseConnectionParams( void );

    void debug( void );

    bool isComplete( void );

    void setDbType( int val ) { pDbType = val; }
    void setDbName( QString val ) { pDbName = val; }
    void setDbAction( int val ) { pDbAction = val; }
    void setDbUser( QString val ) { pDbUser = val; }
    void setDbPassword( QString val ) { pDbPassword = val; }
    void setDbHost( QString val ) { pDbHost = val; }
    void setDbPort( int val ) { pDbPort = val; }

    int dbType( void ) { return pDbType; }
    QString dbName( void ) { return pDbName; }
    int dbAction( void ) { return pDbAction; }
    QString dbUser( void ) { return pDbUser; }
    QString dbPassword( void ) { return pDbPassword; }
    QString dbHost( void ) { return pDbHost; }
    int dbPort( void ) { return pDbPort; }

  protected:
    int pDbType;
    QString pDbName;
    int pDbAction;
    QString pDbUser;
    QString pDbPassword;
    QString pDbHost;
    int pDbPort;
};

#endif // CSQL_H




