/*
csql.h/cpp
----------
Begin: 2004/07/28
Author: Aaron Reeves <aaron.reeves@naadsm.org>
--------------------------------------------------
Copyright (C) 2004 - 2014 Aaron Reeves

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

// FIX ME: for some reason, T/F fields show up as "T" or "F", instead of "1" or "0"

#include "csql.h"

#include <QCoreApplication>
#include <qdebug.h>
#include <qregexp.h>
#include <qfileinfo.h>

#ifdef __linux__
#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qsqlselectcursor.h>
#endif


//*****************************************************************************
// CSqlRow
//*****************************************************************************
CSqlRow::CSqlRow( TFieldMap* fm ) {
  _fm = fm;
}


CSqlRow::~CSqlRow( void ) {
  while (!this->isEmpty()) {
    delete this->takeFirst();
  }
}


void CSqlRow::debug( void ) {
  QString* str;
  QString output;
  int i;

  for( i = 0; i < this->count(); ++i ) {
    str = this->at(i);
    output.append( *str );
    output.append( ", " );
  }
  output = output.left( output.length() - 2 );

  qDebug() << output;

}


QString* CSqlRow::field( int fieldIndex ) {
  if( fieldIndex < this->count() ) {
    return (QString*)this->at( fieldIndex );
  }
  else {
    return NULL;
  }
}


QString* CSqlRow::field( const QString& fldName ) {
  return field(  _fm->retrieveValue( fldName ) );
}
//*****************************************************************************



//*****************************************************************************
// CSqlResult
//*****************************************************************************
CSqlResult::CSqlResult( void ) {
  initialize();
}


void CSqlResult::initialize( void ) {
  rs = new TRowList();
  fieldMap = new TFieldMap();
  _affectedRows = 0;
  _numRows = 0;
  _numFields = 0;
  _success = false;
  _conn = NULL;
}

CSqlResult::CSqlResult( CSqlDatabase* dbConn ) {
  initialize();
  _conn = dbConn;  
}

CSqlResult::CSqlResult(  const char* query, CSqlDatabase* dbConn ) {
  QString qs;
  initialize();
  _conn = dbConn;
  qs = QString( query );
  if( qs.length() > 0 ) runQuery( QString( query ), dbConn );
}


CSqlResult::CSqlResult( const QString &query, CSqlDatabase* dbConn ) {
  initialize();
  _conn = dbConn;

  if( query.length() > 0 ) {
    if( !_conn->isOpen() ){
      qDebug() << "Database is not open. Result set will not be properly constructed.";
    } else {
      runQuery( query, dbConn );
    }
  }
}


void CSqlResult::clear( void ) {
    rs->clear();  // the row list (record set)
    fieldMap->clear(); // clear the field map
    _affectedRows = 0;
    _numRows = 0;
    _numFields = 0;
    _success = false;
}

void CSqlResult::runQuery( const char* query ) {
  if( _conn != NULL ) {
    runQuery( QString( query ), _conn );
  }
  else {
    qDebug() << "No database connection set!";
  }
}

void CSqlResult::runQuery( const QString &query ) {
  if( _conn != NULL ) {
    runQuery( query, _conn );
  }
  else {
    qDebug() << "No database connection set!";
  }
}

void CSqlResult::runQuery( const QString& query, CSqlDatabase* dbConn ) {

  clear();

  if( CSqlQueryString::isSelect( query ) ) {
    runSelectQuery( query, dbConn );
  }
  else if( isOther( query ) ) {
    runOtherQuery( query, dbConn );
  }
  else {
    qDebug() << "Bad query.";
    _success = false;
  }
}


QString CSqlResult::fieldName( int unsigned fieldIndex ) {
  return fieldMap->retrieveKey( fieldIndex );
}


void CSqlResult::runOtherQuery( const QString &query, CSqlDatabase* dbConn ) {
  #ifdef _WIN32
   // use the database's query capability
  if(  adoDatabase_Execute( dbConn->_adoConn, query.toLatin1().data() ) ) {
     _affectedRows = adoDatabase_GetRecordsAffected( dbConn->_adoConn );
     _success = true;
  }
  else {
    _success = false;
  }
  #endif

  #ifdef __linux__
  QSqlQuery* qQuery = new QSqlQuery( query, dbConn->_qConn );

  if( qQuery->isActive() ) {
    _affectedRows = qQuery->numRowsAffected();
    _success = true;
  }
  else {
    _success = false;
  }

  delete qQuery;
  #endif
}


#ifdef __linux__
void CSqlResult::runSelectQueryLinux( const QString &query, CSqlDatabase* dbConn ) {

  int unsigned i;
  bool recordExists;
  CSqlRow* row;
  QString* cqstr;
  QString str;

  QSqlSelectCursor* qQuery = new QSqlSelectCursor( query, dbConn->_qConn );
  //qQuery->exec( query );

  if( qQuery->isActive() ) {
    // get field names and fill the map
    _numFields = qQuery->count();

    for( i = 0; i < _numFields; ++i ) {
      fieldMap->insert( qQuery->fieldName( i ), i );
    }

    _numRows = qQuery->size();

    if( !qQuery->isEmpty() ) {
      for( recordExists = qQuery->first(); recordExists == true; recordExists = qQuery->next() ) {
        row = new CSqlRow( fieldMap );
  
        for( i = 0; i < _numFields; ++i ) {
          str = qQuery->value(i).toString();
          cqstr = new QString( str );
          row->append( cqstr );
        }
        rs->append( row );
      }
    }
	}

  else {
    _success = false;
  }

  delete qQuery;
}
#endif



void CSqlResult::foreignKeys( void ) {
  this->keys( CSqlDatabase::DBKeyTypeForeign );
}

void CSqlResult::primaryKeys( void ) {
  this->keys( CSqlDatabase::DBKeyTypePrimary );
}

void CSqlResult::indices( void ) {
  this->keys( CSqlDatabase::DBKeyTypeIndex );
}


void CSqlResult::keys( const int keyType ) {
  int unsigned i;
  SAdoFieldInfo fldInfo;
  CSqlRow* row;
  QString* cqstr;
  char strVal[32767];
  bool test = false; // until shown otherwise.

  switch( keyType ) {
    case CSqlDatabase::DBKeyTypeForeign:
      test = adoRecordset_ForeignKeys( _conn->_adoRS );
      //if( test )
      //  qDebug() << "CSqlResult::foreignKeys:" << adoRecordset_GetRecordCount( _conn->_adoRS ) << "items.";
      break;
    case CSqlDatabase::DBKeyTypePrimary:
      test = adoRecordset_PrimaryKeys( _conn->_adoRS );
      break;
    case CSqlDatabase::DBKeyTypeIndex:
      test = adoRecordset_Indices( _conn->_adoRS );
      break;
  }
  
  if( test ) {
    // get field names and fill the map
    _numFields = adoRecordset_GetFieldCount( _conn->_adoRS );

    for( i = 0; i < _numFields; ++i ) {
      adoRecordset_GetFieldInfo( _conn->_adoRS, i, &fldInfo );
      fieldMap->insert( QString( fldInfo.m_strName ), i );
    }

    _numRows = adoRecordset_GetRecordCount( _conn->_adoRS );

    // fill the record set
    while( !( adoRecordset_IsEof( _conn->_adoRS ) ) ) {
      row = new CSqlRow( fieldMap );

      for( i = 0; i < _numFields; ++i ) {
        if( adoRecordset_IsFieldNull( _conn->_adoRS, i ) ) {
          row->append( new QString() );  // For this QString, isNull is true.  See Qt documentation.
        }
        else {
          adoRecordset_GetFieldValue( _conn->_adoRS, i, strVal );
          cqstr = new QString( strVal );
          row->append( cqstr );
        }
      }

      rs->append( row );

      adoRecordset_MoveNext( _conn->_adoRS );
    }    
    
    _success = true; 
  }
  else {
    //qDebug() << "CSqlResult::keys failed.";
    _success = false;    
  }  
}


QString CSqlResult::lastError( void) {
  char tmp[1024];
  adoRecordset_LastError( _conn->_adoRS, tmp );
  
  return QString( tmp );
}


void CSqlResult::runSelectQuery( const QString &query, CSqlDatabase* dbConn ) {
  #ifdef _WIN32
    // use a recordset
    TAdoCommand cmd;
    int unsigned i;
    SAdoFieldInfo fldInfo;
    CSqlRow* row;
    QString* cqstr;
    char strVal[32767];
    int unsigned counter;

    /*
    QList<short> typeList;
    int intVal;
    double dbVal;
    QDateTime dtVal;
    */

    cmd = adoCommand_new( dbConn->_adoConn, query.toLatin1().data() );

    if( adoRecordset_Execute( dbConn->_adoRS, cmd ) ) {
      // get field names and fill the map
      _numFields = adoRecordset_GetFieldCount( dbConn->_adoRS );

      for( i = 0; i < _numFields; ++i ) {
        adoRecordset_GetFieldInfo( dbConn->_adoRS, i, &fldInfo );
        fieldMap->insert( QString( fldInfo.m_strName ), i );

        // See big comment below
        // Also keep track of data type for each field: this will be useful in a bit
        //typeList.append( fldInfo.m_nType );
      }

      _numRows = adoRecordset_GetRecordCount( dbConn->_adoRS );

      // fill the record set
      counter = 0;
      while( !( adoRecordset_IsEof( dbConn->_adoRS ) ) ) {
        row = new CSqlRow( fieldMap );

        for( i = 0; i < _numFields; ++i ) {
          if( adoRecordset_IsFieldNull( dbConn->_adoRS, i ) ) {
            row->append( new QString() );  // For this QString, isNull is true.  See Qt documentation.
          }
          else {
            adoRecordset_GetFieldValue( dbConn->_adoRS, i, strVal );
            cqstr = new QString( strVal );
            row->append( cqstr );
          }
          /*
          // This is mostly unnecessary: see programming log 3.78 - 3.79
          // If I ever wanted to use a QVariant instead of QString, I would need this block again.
          // But for now, I'm not using it.
          switch( *(typeList.at( i ) ) ) {

            // Integer types
            case CADORecordset::typeBoolean : // Bit (Jet)
              dbConn->_adoRS->GetFieldValue( i, intVal );
              cqstr = new QString( QString( "%1" ).arg( intVal ) );
              break;
            case CADORecordset::typeUnsignedTinyInt :// Byte (Jet)
              dbConn->_adoRS->GetFieldValue( i, intVal );
              cqstr = new QString( QString( "%1" ).arg( intVal ) );
              break;
            case CADORecordset::typeInteger : // Integer (Jet)
              dbConn->_adoRS->GetFieldValue( i, intVal );
              cqstr = new QString( QString( "%1" ).arg( intVal ) );
              break;
            case CADORecordset::typeSmallInt : // Short (Jet)
              dbConn->_adoRS->GetFieldValue( i, intVal );
              cqstr = new QString( QString( "%1" ).arg( intVal ) );
              break;

            // Real types
            case CADORecordset::typeSingle : // Single (Jet)
              dbConn->_adoRS->GetFieldValue( i, dbVal );
              cqstr = new QString( QString( "%1" ).arg( dbVal ) );
              break;
            case CADORecordset::typeDouble : // Double (Jet)
              dbConn->_adoRS->GetFieldValue( i, dbVal );
              cqstr = new QString( QString( "%1" ).arg( dbVal ) );
              break;

            // String types
            case CADORecordset::typeLongVarWChar : // Memo (Jet)
              dbConn->_adoRS->GetFieldValue( i, strVal );
              cqstr = new QString( strVal );
              break;
            case CADORecordset::typeWChar : // Char (Jet)
              dbConn->_adoRS->GetFieldValue( i, strVal );
              cqstr = new QString( strVal );
              break;
            case CADORecordset::typeVarWChar : // Varchar (Jet)
              dbConn->_adoRS->GetFieldValue( i, strVal );
              cqstr = new QString( strVal );
              break;

            // Dates
            case CADORecordset::typeDate : // Date (Jet)
              // Qt::ISODate - ISO 8601 extended format (YYYY-MM-DD, or with time, YYYY-MM-DDTHH:MM:SS)
              dbConn->_adoRS->GetFieldValue( i, strVal, "%Y-%m-%d %H:%M%S" );
              cqstr = new QString( strVal );
              break;

            // Simple screwy types
            case CADORecordset::typeNumeric : // Decimal (Jet)
              qDebug() << "Unhandled data type";
              // learn to throw exceptions
              break;
            case CADORecordset::typeCurrency : // Currency (Jet)
              qDebug() << "Unhandled data type";
              // learn to throw exceptions
              break;

            // Hard screwy types
            default:
              qDebug() << "Unhandled data type";
              // learn to throw exceptions
              break;
          }
          */
        }

        rs->append( row );

        ++counter;
        if( 0 == counter%100 ) {
          QCoreApplication::processEvents();
        }

        adoRecordset_MoveNext( dbConn->_adoRS );
      }

      _success = true;
    }
    else {
      //qDebug() << "Query failed.";
      _success = false;
    }

    adoCommand_free( cmd );
    cmd = NULL;
  #endif

  #ifdef __linux__
  runSelectQueryLinux( query, dbConn );
  #endif
}

void CSqlResult::debug( void ) {
  CSqlRow* row;
   
  qDebug() << endl << "-------- CSqlResult";
  qDebug() << "numRows:" << this->numRows() << "numFields:" << this->numFields();

  row = this->fetchArrayFirst();
  while( NULL != row ) {
    row->debug();
    row = this->fetchArrayNext(); 
  }
  qDebug() << "-------- end" << endl;
}

CSqlRow* CSqlResult::fetchArrayFirst( void ) {
  _currentIndex = 0;
  
  if( rs->isEmpty() )
    return NULL;
  else
  return rs->first();
}


CSqlRow* CSqlResult::fetchArrayNext( void ) {
  if( rs->count() > _currentIndex + 1 ) {
    ++_currentIndex;
    return rs->at(_currentIndex);
  }
  else {
    return NULL;
  }
}

CSqlRow* CSqlResult::fetchArrayLast( void ) {
  _currentIndex = rs->count() - 1;
  return rs->last();
}

CSqlRow* CSqlResult::fetchArray( int unsigned index ) {
  return rs->at( index );
}


bool CSqlResult::isOther( const QString& query ) {
  QString test = query.trimmed();

  if( ( false == test.contains( QRegExp( "^insert", Qt::CaseInsensitive ) ) )
    || ( false == test.contains( QRegExp( "^update", Qt::CaseInsensitive ) ) )
    || ( false == test.contains( QRegExp( "^delete", Qt::CaseInsensitive ) ) )
    || ( false == test.contains( QRegExp( "^create table", Qt::CaseInsensitive ) ) )
    || ( false == test.contains( QRegExp( "^alter table", Qt::CaseInsensitive ) ) )
  ) {
    return true;
  }
  else {
    return false;
  }

  /*
  if( ( query.contains( "INSERT", false ) > 0 )
    || ( query.contains( "UPDATE", false ) > 0 )
    || ( query.contains( "DELETE", false ) > 0 )
    || ( query.contains( "CREATE TABLE", false ) > 0 )
    || ( query.contains( "ALTER TABLE", false ) > 0 ) )
  {
    return true;
  }
  else {
    return false;
  }
  */
}


int unsigned CSqlResult::fieldCount( void ) {
  return fieldMap->count();
}


CSqlResult::~CSqlResult( void ) {
  // qDebug() << Destructor called??";

  while (!rs->isEmpty()) {
    delete rs->takeFirst();
  }

  delete rs;
  delete fieldMap;
}
//*****************************************************************************



//*****************************************************************************
// CSqlFieldInfo
//*****************************************************************************
CSqlFieldInfo::CSqlFieldInfo( void ) {
  setFieldName( "" );
  setFieldType( "" );
  setFieldSize( -1 );
  setAutoIncrement( false );
}


QString CSqlFieldInfo::fieldTypeDescr( void ) {
  QString descr;

  if( autoIncrement() ) {
    descr = "Counter";
  }
  else if( -1 != fieldSize() ) {
    descr = QString( "%1(%2)" ).arg( fieldType() ).arg( fieldSize() );
  }
  else {
    descr = fieldType();
  }

  return descr;
}
//*****************************************************************************



//*****************************************************************************
// CSqlQueryString
//*****************************************************************************
bool CSqlQueryString::isSelect( const QString& query ) {
  QString test = query.trimmed();

  return test.contains( QRegExp( "^select", Qt::CaseInsensitive ) );
}
//*****************************************************************************



//*****************************************************************************
// CSqlDatabase
//*****************************************************************************
/**
  Constructs but does not open a database.  Use of this constructor requires properties
  such as dbType, dbName, etc. (see below) to be set before the database may be opened or created.
*/
CSqlDatabase::CSqlDatabase( void ) {
  initialize();
}


/**
  Constructs an instance of CSqlDatabase of type dbType (see enum DatabaseTypes).
  if dbAction is set to DBOpen (default), an existing database is opened.  If dbAction is DBCreate,
  an attempt is made to create a new database (for MS Access).

  @param dbType
  @param dbName
  @param dbAction
  @param dbUser
  @param dbPassword
  @param dbHost
*/
CSqlDatabase::CSqlDatabase( 
  int dbType, 
  const QString& dbName, 
  int dbAction, 
  const QString& dbUser, 
  const QString& dbPassword, 
  const QString& dbHost, 
  const int& dbPort 
) {
  initialize();

  setType( dbType );
  setName( dbName );
  setAction( dbAction );
  setUser( dbUser );
  setPassword( dbPassword );
  setHost( dbHost );
  setPort( dbPort );

  open();
}


CSqlDatabase::CSqlDatabase( CSqlDatabaseConnectionParams* params ) {
  initialize();
 
  setType( params->dbType() );        
  setAction( params->dbAction() );      
  setName( params->dbName() );    
  setUser( params->dbUser() );    
  setPassword( params->dbPassword() );
  setHost( params->dbHost() );    
  setPort( params->dbPort() );
  
  if( params->isComplete() && ( DBOpen == params->dbAction() ) ) {
    open();
  }
}


void CSqlDatabase::initialize( void ) {
  _isOpen = false;
  setType( DBUnspecified );
  setAction( DBNoAction );
  setName( "" );
  setUser( "" );
  setPassword( "" );
  setHost( "" );

  #ifdef _WIN32
    _adoConn = NULL;
    _adoCat = NULL;
    _adoRS = NULL;
    //_adoCmd = NULL; // Currently unused
    _adoConnStr = "";
  #endif

  #ifdef __linux__
    _qConn = NULL;
  #endif
}


QString CSqlDatabase::delimitedName( const QString val ) {
  return delimitedName( val, this->type() );
}


QString CSqlDatabase::sqlQuote( const QString val ) {
  return sqlQuote( val, this->type() ); 
}


QString CSqlDatabase::delimitedName( const QString val, const int dbFormat ) {
  QString result = val;
  switch( dbFormat ) {
    // in MS Access, table/field names are delimited with ` marks.
    case CSqlDatabase::DBMSAccess:
      result = result.prepend( "`" );
      result = result.append( "`" );
      break;

    // MySQL is the same as MS Access.
    case CSqlDatabase::DBMySQL:
      result = result.prepend( "`" );
      result = result.append( "`" );
      break;

    // Postgres uses " to delimit table and field names.
    case CSqlDatabase::DBPostgres:
      result = result.prepend( "\"" );
      result = result.append( "\"" );
      break;

    default:
      result = result;
      break;
  }

  return( result );
}


QString CSqlDatabase::sqlQuote( const QString val, const int dbFormat ) {
  QString result = val;

  switch( dbFormat ) {
    // In MSAccess, double quotes (") are used to indicate strings.
    // Quote marks inside a string are escaped with another quote mark: ""
    case CSqlDatabase::DBMSAccess:
      result = result.replace( QRegExp( "\"" ), "\"\"" );
      result = result.prepend( "\"" );
      result = result.append( "\"" );
      break;
      
    // In MySql, single quotes (') are used to indicate strings.
    // Quote marks inside a string are escaped with a back slash: \'      
    case CSqlDatabase::DBMySQL:
      result = result.replace( QRegExp( "'" ), "\\'" );
      result = result.prepend( "'" );
      result = result.append( "'" );
      break;
    
    // In Sqlite, single quotes (') are used to indicate strings.
    // Quote marks inside a string are escaped with a another single quote: ''
    case CSqlDatabase::DBSqlite:    
      result = result.replace( QRegExp( "'" ), "''" ); 
      result = result.prepend( "'" );
      result = result.append( "'" );      
      break;
    
    // In Postgres, single quotes (') are used to indicate strings.
    // Quote marks inside a string are escaped with a another single quote: ''
    case CSqlDatabase::DBPostgres:
      result = result.replace( QRegExp( "'" ), "''" );
      result = result.prepend( "'" );
      result = result.append( "'" );
      break;

    // FIX ME: Not sure what to do for DBF files.  For now, follow Access.  
    case CSqlDatabase::DBDbf:
      result = result.replace( QRegExp( "\"" ), "\"\"" );
      result = result.prepend( "\"" );
      result = result.append( "\"" );
      break;    
    
    default: // Follow MSAccess for now.
      qDebug() << "Default case triggered in CSqlDatabase::sqlQuote()";
      result = result.replace( QRegExp( "\"" ), "\"\"" );
      result = result.prepend( "\"" );
      result = result.append( "\"" );
      break;   
    }
  
	return result;
}

QString CSqlDatabase::sqlBool( const bool val ) {
  return sqlBool( val, this->type() ); 
}


QString CSqlDatabase::sqlBool( const bool val, const int dbFormat ) {
  QString result;
  
  switch( dbFormat ) {
    case CSqlDatabase::DBMSAccess:
      if( val ) { 
        result = "true";
      } else {
        result = "false";
      }
      break;
    case CSqlDatabase::DBMySQL:
      if( val ) { 
        result = "true";
      } else {
        result = "false";
      }    
      break;
    case CSqlDatabase::DBPostgres:
      if( val ) {
        result = "true";
      } else {
        result = "false";
      }
      break;
    case CSqlDatabase::DBSqlite:
      if( val ) { 
        result = "1";
      } else {
        result = "0";
      }    
      break;
    case CSqlDatabase::DBDbf: // FIX ME: Not sure about this...
      if( val ) { 
        result = "true";
      } else {
        result = "false";
      }     
      break;
    default:
      qDebug() << "Default case triggered in CSqlDatabase::sqlBool()";
      if( val ) { 
        result = "true";
      } else {
        result = "false";
      }    
      break;  
  }
  
  return result;
}


QString CSqlDatabase::writeQuery(
	const QString tableName,
	const int queryType,
	TQueryMap map,
	const QString whereClause )
{
  QString table = "";
	QString values = "";
	QString fields = "";
	QString q;
	TQueryMap::Iterator it;
	int i = 0;
	QString field;

	if( CSqlDatabase::DBInsert == queryType ) {
		for ( it = map.begin(); it != map.end(); ++it ) {
			++i;
      switch( this->type() ) {
        case DBMSAccess:
          field = QString( "`%1`" ).arg( it.key() );
          break;
        case DBMySQL:
          field = QString( "`%1`" ).arg( it.key() );
          break;
        case DBPostgres:
          field = QString( "\"%1\"" ).arg( it.key() );
          break;
        default:
          field = QString( "\"%1\"" ).arg( it.key() );
          break;
      }

			fields.append( QString( "%1" ).arg( field ) );
			values.append( it.value() );

			if( i < map.count() ) {
				fields.append( ", " );
				values.append( ", " );
			}
		}

    // Quote the table name properly
    switch( this->type() ) {
      case DBMSAccess:
        table = QString( "`%1`" ).arg( tableName );
        break;
      case DBMySQL:
        table = QString( "`%1`" ).arg( tableName );
        break;
      case DBPostgres:
        table = QString( "\"%1\"" ).arg( tableName );
        break;
      default:
        table = QString( "\"%1\"" ).arg( tableName );
        break;
    }

    q = QString( "INSERT INTO %1 ( %2 ) VALUES( %3 )" ).arg( table ).arg( fields ).arg( values );
	}
	else if( CSqlDatabase::DBUpdate == queryType ) {
		for ( it = map.begin(); it != map.end(); ++it ) {
			++i;
			values.append( QString( "`%1`, %2" ).arg( it.key() ).arg( it.value() ) );
			if( i < map.count() ) values.append( ", " );

      q = QString( "UPDATE %1 SET %2 %3" ).arg( table ).arg( values ).arg( whereClause );
		}
	}
	return q;
}


int unsigned CSqlDatabase::recordID( const QString tableName, const QString idFieldName, TQueryMap map, QString* returnQuery ) {
  QString query;
  QString criteria = "";
  int i;
  TQueryMap::Iterator it;
  uint result;
  CSqlResult* res;
  QString field;
  
  i = 0;
  for( it = map.begin(); it != map.end(); ++it ) {
    ++i;
    field = QString( "`%1`" ).arg( it.key() );

    if( 255 > it.value().length() ) {
      criteria.append( QString( "%1 = %2" ).arg( field ).arg( it.value() ) );
    }
    else {
      criteria.append( QString( "%1 LIKE %2" ).arg( field ).arg( it.value() ) ); 
    }
    if( i < map.count() ) {
      criteria.append( " AND " ); 
    }  
  }

  query = QString( "SELECT `%1` FROM `%2` WHERE %3" ).arg( idFieldName ).arg( tableName ).arg( criteria );
  //qDebug() << query;
  
  res = new CSqlResult( query, this );  
  
  if( 1 == res->numRows() ) {
    result = res->fetchArrayFirst()->field( idFieldName )->toInt();
  }
  else if( 0 == res->numRows() ) {
    query = writeQuery( tableName, DBInsert, map );
    //qDebug() << query;
    if( execute( query ) ) {
      result = lastInsertID();
    }
    else {
      result = 0;
    } 
  }
  else {
    result = 0;  
  }
  
  delete res;
  
  if( NULL != returnQuery ) {
    *returnQuery = query; 
  }
  return result;   
}


void CSqlDatabase::open() {
  QString createStr;
  QFileInfo* fi;

  #ifdef _WIN32
  // Does the file exist?
  if( ( type() == DBMSAccess ) || ( type() == DBDbf ) ){
    if( !QFile::exists( this->name() ) ){
      qDebug() << "The specified file" << name() << "does not exist.  Database will not open.";
      return;
    }
  }

  _adoConn = adoDatabase_new();
  _adoCat = adoxCatalog_new();

  switch( type() ) {
    case DBMSAccess:
      // FIX ME: make sure that the file exists

      // try connecting to the database
      _adoConnStr = "Provider=Microsoft.Jet.OLEDB.4.0; Data Source=" + name() + ";Persist Security Info=False";

      if( CSqlDatabase::DBCreate == action() ) {
        createStr = _adoConnStr;
        createStr = createStr.append( ";Jet OLEDB:Engine Type=5;" );
        adoxCatalog_CreateDatabase( _adoCat, createStr.toLatin1().constData() );
      }
      break;
    case DBMySQL:
      _adoConnStr = "Driver={mySQL ODBC 3.51 Driver};Server=" + host() + ";Port=" + QString( "%1" ).arg( port() ) + ";Database=" + name() + ";Uid=" + user() + ";Pwd=" + password() + ";";
      //qDebug() << _adoConnStr;
      break;
    case DBPostgres:
      //Driver={PostgreSQL};Server=IP address;Port=5432;Database=myDataBase;Uid=myUsername;Pwd=myPassword;
      _adoConnStr = "Driver={PostgreSQL};Server=" + host() + ";Port=" + QString( "%1" ).arg( port() ) + ";Database=" + name() + ";Uid=" + user() + ";Pwd=" + password() + ";";
      //qDebug() << _adoConnStr;
      break;
    case DBDbf:
      fi = new QFileInfo( name() );
      if( fi->exists() ) {
        if( fi->isDir() ) { // Use the whole path
           //_adoConnStr = "Provider=Microsoft.ACE.OLEDB.12.0;Data Source=" + fi->absoluteFilePath() + ";Extended Properties=dBASE IV;User ID=Admin;";
           _adoConnStr = "Provider=Microsoft.JET.OLEDB.4.0;Data Source=" + fi->absoluteFilePath() + ";Extended Properties=dBase IV;User ID=Admin;Password=;";
        } else { // Trim the file name from the path, and use tha path.
          //_adoConnStr = "Provider=Microsoft.ACE.OLEDB.12.0;Data Source=" + fi->absolutePath() + ";Extended Properties=dBASE IV;User ID=Admin;";
          _adoConnStr = "Provider=Microsoft.JET.OLEDB.4.0;Data Source=" + fi->absolutePath() + ";Extended Properties=dBase IV;User ID=Admin;Password=;";
        }
      } else {
        qDebug() << "There is a problem with the specified DBF file name:" << name();
      }
      break;
  }

  //qDebug() << _adoConnStr;

  adoDatabase_SetConnectionString( _adoConn, _adoConnStr.toLatin1().data() );
  adoDatabase_SetConnectionModeShareDenyNone( _adoConn ); // FIX ME: some day, this might be a parameter that can be specified.

  if( adoDatabase_Open( _adoConn ) ) {
    _adoRS = adoRecordset_new( _adoConn );
    adoxCatalog_Open( _adoCat, _adoConnStr.toLatin1().data() );
    _isOpen = true;
  }
  else {
    //qDebug() << "Database didn't open.";

    adoDatabase_free( _adoConn );
    _adoConn = NULL;
    adoxCatalog_free( _adoCat );
    _adoCat = NULL;
    _isOpen = false;
  }
  #endif

  #ifdef __linux__
  if( DBMySQL == type() ) {
    _qConn = QSqlDatabase::addDatabase( "QMYSQL3" );
    _qConn->setDatabaseName( name() );
    _qConn->setHostName( host() );
    _qConn->setPort( port() );
    _qConn->setPassword( password() );
    _qConn->setUserName( user() );
    _qConn->open();

    if( _qConn->isOpen() ) {
      qDebug() << "Database was opened successfully";
      _isOpen = true;
    }
    else {
      //qDebug() << "Database didn't open";
      delete _qConn;
      _isOpen = false;
    }
  }
  else {
    qDebug() << "Unsupported database type for Linux";
  }
  #endif

}


void CSqlDatabase::close( void ) {
  #ifdef _WIN32
    if( _adoRS != NULL ) {
      if( adoRecordset_IsOpen( _adoRS ) )
        adoRecordset_Close( _adoRS );
    }
    if( _adoConn != NULL ) {
      if( adoDatabase_IsOpen( _adoConn ) )
        adoDatabase_Close( _adoConn );
    }
    // FIX ME: this block causes a seg fault when used on Windows with MySQL.
    // I have no idea why.
    if( DBMySQL != type() ) {
      if( _adoCat != NULL ) {
        if( adoxCatalog_IsOpen( _adoCat ) )
          adoxCatalog_Close( _adoCat );
      }
    }
  #endif

  #ifdef __linux__
    if( _qConn != NULL ) {
      if( _qConn->isOpen() ) _qConn->close();
    }
  #endif

  _isOpen = false;
}


bool CSqlDatabase::lockTableRead( const QString& tableName ) {
  #ifdef __linux__
    qDebug() << "CSqlDatabase::lockTable isn't supported yet on Linux";
    return false;
  #endif
  
  #ifdef _WIN32
    if( DBMySQL == type() ) {  
      return( this->execute( QString( "LOCK TABLES %1 READ" ).arg( tableName ) ) );
    }
    else {
    qDebug() << "CSqlDatabase::lockTable does not work with other databases";
    return false;       
    }
  #endif 
}


bool CSqlDatabase::lockTableWrite( const QString& tableName ) {
  #ifdef __linux__
    qDebug() << "CSqlDatabase::lockTable isn't supported yet on Linux";
    return false;
  #endif
  
  #ifdef _WIN32
    if( DBMySQL == type() ) {  
      return( this->execute( QString( "LOCK TABLES %1 WRITE" ).arg( tableName ) ) );
    }
    else {
    qDebug() << "CSqlDatabase::lockTable does not work with other databases";
    return false;       
    }
  #endif 
}


bool CSqlDatabase::unlockTables( void ) {
  #ifdef __linux__
    qDebug() << "CSqlDatabase::unlockTables isn't supported yet on Linux";
    return false;
  #endif
  
  #ifdef _WIN32
    if( DBMySQL == type() ) {  
      return( this->execute( "UNLOCK TABLES" ) );
    }
    else {
    qDebug() << "CSqlDatabase::unlockTables does not work with other databases";
    return false;       
    }
  #endif    
}

bool CSqlDatabase::execute( const QString &query, int unsigned* affectedRows ) {
  #ifdef __linux__
    qDebug() << "CSqlDatabase::execute isn't supported yet on Linux";
    return false;
  #endif

  #ifdef _WIN32
    bool result = adoDatabase_Execute( _adoConn, query.toLatin1().data() );
    
    if( NULL != affectedRows ) {
      *affectedRows = adoDatabase_GetRecordsAffected( _adoConn );
    }
    
    return result;
  #endif
}


int CSqlDatabase::numRows( const QString& query ) {
  //qDebug() << "++++ Creating object with query" << query;
  CSqlResult* res = new CSqlResult( query, this );
  int result = res->numRows();
  //qDebug() << "numRows:" << result;
  //qDebug() << "---- Deleting object";
  delete res;

  return result; 
}


int unsigned CSqlDatabase::lastInsertID( void ) {
  QString q;
  int unsigned result;

  switch( _type ) {
    case DBMySQL:
      q = "SELECT last_insert_id()";
      break;
    case DBMSAccess:
      q = "SELECT @@IDENTITY";
      break;
    case DBPostgres:
      q = "SELECT LASTVAL()";
      break;
  }

  CSqlResult* res = new CSqlResult( q, this );
	
	// FIX ME: add an ASSERT or something if result is empty
  CSqlRow* row = res->fetchArrayFirst();

  switch( _type ) {
    case DBMySQL:
      result = int( row->field(0)->toFloat() );
      break;
    case DBMSAccess:
      result = row->field( 0 )->toInt();
      break;
    case DBPostgres:
      result = row->field( 0 )->toInt();
      break;
    default:
      qDebug() << "Undefined database type in CSqlDatabase::lastInsertID!";
      result = 0;
  }
  
  delete res;
  return result;
}


QString CSqlDatabase::lastError( void ) {
  char tmp[1024];
  adoDatabase_LastError( _adoConn, tmp );
  
  return QString( tmp );
}

bool CSqlDatabase::tableExists( const QString& tableName ) {
  bool result;
  QStringList* tables = newTableList();
  qDebug() << "Looking for" << tableName << "in list of" << tables->count() << "items";
  result = tables->contains( tableName );
  delete tables; 
  return result;
}


QStringList* CSqlDatabase::newTableList( void ) {
  #ifdef _WIN32
    long tableCnt;
    long i;
    char tmpTableName[1024];
    QString tableName;
    QStringList* tableList = new QStringList();

    // These two lines will cause newly created tables to show, and newly deleted tables not to.
    this->close();
    this->open();

    tableCnt = adoxCatalog_GetTableCount( _adoCat );

    for( i = 0; tableCnt > i; ++i ) {
      adoxCatalog_GetTableName( _adoCat, i, tmpTableName );
      tableName = tmpTableName;
      if( "MSys" != tableName.left( 4 ) && "~TMP" != tableName.left( 4 ) ) {
        tableList->append( tableName );
      }
    }
    return tableList;
  #endif

  #ifdef __linux__
    qDebug() << "This function is not yet defined.";
    return NULL;
  #endif
}


QStringList* CSqlDatabase::newSystemTableList( void ) {
  #ifdef _WIN32
    long tableCnt;
    long i;
    char tmpTableName[1024];
    QString tableName;
    QStringList* tableList = new QStringList();

    this->close();
    this->open();

    tableCnt = adoxCatalog_GetTableCount( _adoCat );


    for( i = 0; tableCnt > i; ++i ) {
      adoxCatalog_GetTableName( _adoCat, i, tmpTableName );
      tableName = tmpTableName;
      if( "MSys" == tableName.left( 4 ) ) {
        tableList->append( tableName );
      }
    }
    return tableList;
  #endif

  #ifdef __linux__
    qDebug() << "This function is not yet defined.";
    return NULL;
  #endif
}


QString CSqlDatabase::fieldTypeDescr( short nType ) {
  QString val;

  #ifdef _WIN32
    val = adoRecordset_GetFieldType( nType );
  #endif

  return val;
}


CFieldInfoList* CSqlDatabase::newFieldInfoList( QString tableName, bool* success ) {
  #ifdef _WIN32
    CSqlResult* res;
    QString q;
    CFieldInfoList* fieldList;
    SAdoFieldInfo fldInfo;
    CSqlFieldInfo* fi;
    int unsigned i;
    TAdoxTable adoxTable;
    char tmp[1024];

    fieldList = new CFieldInfoList();

    q = QString( "SELECT TOP 1 * from [%1]" ).arg( tableName );

    res = new CSqlResult( q, this );

    if( NULL != success ) *success = res->success();

    adoxTable =  adoxTable_new( this->_adoCat );
    adoxTable_Open( adoxTable, tableName.toLatin1().data() );

    for( i = 0; res->fieldCount() > i; ++i ) {
      adoRecordset_GetFieldInfo( _adoRS, i, &fldInfo );

      fi = new CSqlFieldInfo();
      fi->setFieldName( QString( fldInfo.m_strName ) );
      fi->setFieldType( fieldTypeDescr( fldInfo.m_nType ) );
      fi->setAutoIncrement( adoxTable_IsAutoincrement( adoxTable, fldInfo.m_strName ) );
      adoxTable_GetDescription( adoxTable, fldInfo.m_strName, tmp );
      fi->setFieldDescr( QString( tmp ) );

      if( "Char" == fi->fieldType() || "Text" == fi->fieldType() ) {
        fi->setFieldSize( fldInfo.m_lDefinedSize );
      }
      fieldList->append( fi );
    }

    adoxTable_free( adoxTable );
    delete res;

    return fieldList;
  #endif

  #ifdef __linux__
    qDebug() << "This function is not yet defined.";
    return NULL;
  #endif
}


void CSqlDatabase::debug( void ) {
  qDebug() << "Type:" << type();
  qDebug() << "Action:" << action();
  qDebug() << "Name:" << name();
  qDebug() << "User:" << user();
  qDebug() << "Password:" << password();
  qDebug() << "Host:" << host();
  qDebug() << "Port:" << port();
  qDebug() << "Is open:" << this->isOpen();
}


/**
  Closes the database (if open) and deletes it.
*/
CSqlDatabase::~CSqlDatabase() {
  if( this->isOpen() ) {
    this->close();
  }
    
  #ifdef _WIN32
    if( NULL != _adoConn ) {
      adoDatabase_free( _adoConn );
      _adoConn = NULL;
    }

    if( NULL != _adoCat ) {
      adoxCatalog_free( _adoCat );
      _adoCat = NULL;
    }

    if( NULL != _adoRS ) {
      adoRecordset_free( _adoRS );
      _adoRS = NULL;
    }

    //delete _adoCmd; _adoCmd = NULL; // Currently unused
  #endif

  #ifdef __linux__
    delete _qConn; _qConn = NULL;
  #endif
}
//*****************************************************************************



//*****************************************************************************
// CFieldInfoList
//*****************************************************************************
CFieldInfoList::~CFieldInfoList( void ) {
  while (!this->isEmpty()) {
    delete this->takeFirst();
  }
}
//*****************************************************************************



//*****************************************************************************
// CSqlDatabaseConnectionParams
//*****************************************************************************
CSqlDatabaseConnectionParams::CSqlDatabaseConnectionParams( void ) {
  setDbType( CSqlDatabase::DBUnspecified );
  setDbAction( CSqlDatabase::DBNoAction );
  setDbName( "" );
  setDbUser( "" );
  setDbPassword( "" );
  setDbHost( "" );
  setDbPort( -1 );
}



CSqlDatabaseConnectionParams::~CSqlDatabaseConnectionParams( void ) {
  // Do nothing.
}


bool CSqlDatabaseConnectionParams::isComplete( void ) {
  bool result = true;

  if( CSqlDatabase::DBUnspecified == dbType() ) {
    result = false;
  }
  if( CSqlDatabase::DBUnspecified == dbType() ) {
    result = false;  
  }
  if( CSqlDatabase::DBNoAction == dbAction() ) {
    result = false; 
  }
  if( dbName().isEmpty() ) {
    result = false; 
  }

  if( ( CSqlDatabase::DBMySQL == dbType() ) || ( CSqlDatabase::DBPostgres == dbType() ) ) {
    if( 0 == dbPort() ) {
      result = false;  
    }
    if( 0 == dbHost().length() ) {
      result = false; 
    }
    if( 0 == dbUser().length() ) {
      result = false;   
    }
    if( 0 == dbPassword().length() ) {
      result = false;   
    }
  }

  return result;
}


void CSqlDatabaseConnectionParams::debug( void ) {
  qDebug() << endl;
  qDebug() << "--------------------------- CSqlDatabaseConnectionParams";
  qDebug() << "dbType:" << pDbType;
  qDebug() << "dbName:" << pDbName;
  qDebug() << "dbAction:" << pDbAction;
  qDebug() << "dbUser:" << pDbUser;
  qDebug() << "dbPassword:" << pDbPassword;
  qDebug() << "dbHost:" << pDbHost;
  qDebug() << "dbPort:" << pDbPort;
  qDebug() << "isComplete:" << this->isComplete();
  qDebug() << "------------------------END CSqlDatabaseConnectionParams";
  qDebug() << endl;
}
//*****************************************************************************


