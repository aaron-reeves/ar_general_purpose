/*
cconfigfile.h/cpp
-----------------
Begin: 2015-06-24
Author: Aaron Reeves <aaron.reeves@sruc.ac.uk>
---------------------------------------------------------
Copyright (C) 2015 - 2021 Scotland's Rural College (SRUC)

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#include <QDebug>

#ifdef QSQL_USED
  #include <QSqlQuery>
  #include <QSqlError>
#endif

#include "cconfigfile.h"

#include <ar_general_purpose/cencryption.h>
#include <ar_general_purpose/strutils.h>
#include <ar_general_purpose/csv.h>


#include <ar_general_purpose/arcommon.h>

#ifdef QSQL_USED
#include <ar_general_purpose/getlastinsertid.h>
#endif

const QString APP_PASSWORD_KEY = QStringLiteral("ds098Lju3K24");


QString makeEncryptedPassword( const QString& password ) {
  return CEncryption::hexEncode( CEncryption::rc4Encrypt( password, APP_PASSWORD_KEY ) );
}

void appendToMessage( QString* message, const QString& newMessage ) {
  if( nullptr != message )
    message->append( newMessage );
}

QString ConfigReturnCode::resultString( const int returnCode ) {
  QString result;
  switch( returnCode ) {
    case ConfigReturnCode::Success: result = QStringLiteral("Success"); break;
    case ConfigReturnCode::SuccessWithBadRows: result = QStringLiteral("Success with bad rows"); break;
    case ConfigReturnCode::BadArguments: result = QStringLiteral("Bad arguments"); break;
    case ConfigReturnCode::UnrecognizedFunction: result = QStringLiteral("Unrecognized function"); break;
    case ConfigReturnCode::MissingConfigFile: result = QStringLiteral("Missing config file"); break;
    case ConfigReturnCode::CannotOpenConfigFile: result = QStringLiteral("Cannot open config file"); break;
    case ConfigReturnCode::BadConfiguration: result = QStringLiteral("Bad configuration"); break;
    case ConfigReturnCode::BadFunctionConfiguration: result = QStringLiteral("Bad function configuration"); break;
    case ConfigReturnCode::BadDatabaseConfiguration: result = QStringLiteral("Bad database configuration"); break;
    case ConfigReturnCode::BadDatabaseSchema: result = QStringLiteral("Bad database schema"); break;
    case ConfigReturnCode::MissingInstructions: result = QStringLiteral("Missing instructions"); break;
    case ConfigReturnCode::EmptyInputFile: result = QStringLiteral("Empty input file"); break;
    case ConfigReturnCode::CannotOpenInputFile: result = QStringLiteral("Cannot open input file"); break;
    case ConfigReturnCode::QueryDidNotExecute: result = QStringLiteral("Query did not execute"); break;
    case ConfigReturnCode::CannotOpenOutputFile: result = QStringLiteral("Cannot open output file"); break;
    case ConfigReturnCode::BadFileFormat: result = QStringLiteral("Bad file format"); break;
    case ConfigReturnCode::BadDataField: result = QStringLiteral("Bad data field"); break;

    default: result = QStringLiteral("Undefined error.");
  }

  return result;
}

//=============================================================================
// CConfigBlock
//=============================================================================
CConfigBlock::CConfigBlock() {
  // _name will be empty
  _removed = false;
}


CConfigBlock::CConfigBlock( const QString& name ) {
  _name = name;
  _removed = false;
}


CConfigBlock::CConfigBlock( const CConfigBlock& other ) {
  assign( other );
}


CConfigBlock& CConfigBlock::operator=( const CConfigBlock& other ) {
  assign( other );
  return *this;
}


void CConfigBlock::assign( const CConfigBlock& other ) {
  _name = other._name;
  _removed = other._removed;
  _itemList = other._itemList;
  _itemHash = other._itemHash;
}


CConfigBlock::~CConfigBlock() {
  // Do nothing.
}


void CConfigBlock::insert( const CConfigItem& item ) {
  _itemList.append( item );
  _itemHash.insert( item.key().trimmed().toLower(), item );
}


// Does the block contain AT LEAST one value with the specified key?
bool CConfigBlock::contains( const QString& key ) {
  return _itemHash.contains( key.trimmed().toLower() );
}


// How many values with the specified key?
int CConfigBlock::multiContains( const QString& key ) const {
  return _itemHash.count( key.trimmed().toLower() );
}


// Return the FIRST value with the specified key
QString CConfigBlock::value( const QString& key ) const {
  QString result;

  for( int i = 0; i < _itemList.count(); ++i ) {
    if( 0 == _itemList.at(i).key().compare( key.trimmed().toLower() ) ) {
      result = _itemList.at(i).value();
      break;
    }
  }

  return result;
}


// Return a list of ALL values with the specified key, in the order in which they appear in the block
QStringList CConfigBlock::values( const QString& key ) const {
  QStringList result;

  for( int i = 0; i < _itemList.count(); ++i ) {
    if( 0 == _itemList.at(i).key().compare( key.trimmed().toLower() ) ) {
      result.append( _itemList.at(i).value() );
    }
  }

  return result;
}


void CConfigBlock::writeToStream( QTextStream* stream ) {
  *stream << "[" << this->name() << "]" << endl;

  for( int i = 0; i < _itemList.count(); ++i ) {
    *stream << "  " << _itemList.at(i).key() << " <- " << _itemList.at(i).value() << endl;
  }
}


void CConfigBlock::debug() const {
  qDb() << QStringLiteral( "Block '%1':" ).arg( this->name() ) << _itemList.count();

  for( int i = 0; i < _itemList.count(); ++i ) {
    qDb() << "  " << _itemList.at(i).key() << _itemList.at(i).value();
  }
}
//=============================================================================



#ifdef QSQL_USED
//=============================================================================
// CConfigDatabase
//=============================================================================
CConfigDatabase::DBType CConfigDatabase::dbTypeFromString( QString val ) {
  val = val.trimmed().toLower();

  if( 0 == val.compare( QLatin1String("postgresql"), Qt::CaseInsensitive ) )
    return DBTypePostgreSQL;
  else if( 0 == val.compare( QLatin1String("mysql"), Qt::CaseInsensitive ) )
    return DBTypeMySql;
  else if( 0 == val.compare( QLatin1String("sqlite"), Qt::CaseInsensitive ) )
    return DBTypeSQLite;

  // QSql driver names start with "q"
  else if( 0 == val.compare( QLatin1String("qpsql"), Qt::CaseInsensitive ) )
    return DBTypePostgreSQL;
  else if( 0 == val.compare( QLatin1String("qmysql"), Qt::CaseInsensitive ) )
    return DBTypeMySql;
  else if( 0 == val.compare( QLatin1String("qsqlite"), Qt::CaseInsensitive ) )
    return DBTypeSQLite;

  else
    return DBTypeUnspecified;
}


QString CConfigDatabase::dbTypeToString( CConfigDatabase::DBType val ) {
  QString result;

  switch( val ) {
    case DBTypePostgreSQL:
      result = QStringLiteral("PostgreSQL");
      break;
    case DBTypeMySql:
      result = QStringLiteral("MySQL");
      break;
    case DBTypeSQLite:
      result = QStringLiteral("SQLite");
      break;
    default:
      result = QStringLiteral("UnspecifiedDatabaseType");
      break;
  }

  return result;
}


CConfigDatabase::CConfigDatabase( const QString& name /* = "" */, const DBType type /* = DBTypePostgreSQL */ ) {
  initialize();
  _name = name;
  _type = type;
}


CConfigDatabase::CConfigDatabase( CConfigBlock* block, const QString& name /* = "" */, const DBType type /* = DBTypePostgreSQL */ ) {
  initialize();
  _name = name;
  _type = type;

  QList<CConfigItem> list = block->items();

  for( int i = 0; i < list.count(); ++i ) {
    processPair( list.at(i).key(), list.at(i).value() );
  }
}


CConfigDatabase::CConfigDatabase( const QStringList& configBlock, const QString& name /* = "" */, const DBType type /* = DBTypePostgreSQL */ ) {
  initialize();
  _name = name;
  _type = type;

  int i;
  QStringList lineParts;
  QString key, val;

  for( i = 0; i < configBlock.count(); ++i ) {
    lineParts.clear();
    lineParts = configBlock.at(i).split( QStringLiteral("<-") );

    if( 2 != lineParts.count() ) {
      initialize();
      // FIX ME: This should set an error flag.
      return;
    }

    key = lineParts.at(0).trimmed().toLower();
    val = lineParts.at(1).trimmed();

    processPair( key, val );
  }
}


CConfigDatabase::CConfigDatabase( const CConfigDatabase& other ) {
  assign( other );
}


CConfigDatabase& CConfigDatabase::operator=( const CConfigDatabase& other ) {
  assign( other );
  return *this;
}


CConfigDatabase CConfigDatabase::parameters( const int connectionNumber /* = -1 */ ) const {
  CConfigDatabase result;

  result._dbHost = this->_dbHost;
  result._dbPort = this->_dbPort;
  result._dbName = this->_dbName;
  result._dbPath = this->_dbPath;
  result._dbUser = this->_dbUser;
  result._dbPassword = this->_dbPassword;
  result._dbSchema = this->_dbSchema;
  result._dbTable = this->_dbTable;

  result._name = this->_name;
  result._type = this->_type;

  if( -1 != connectionNumber ) {
    result._name.append( QStringLiteral( "_dbconn_%1" ).arg( connectionNumber ) );
  }

  return result;
}


QString CConfigDatabase::description() const {
  QString result = QStringLiteral( "host: %1 database: %2" ).arg( _dbHost, _dbName );

  if( !_dbPath.isEmpty() )
    result.append( QStringLiteral( " path: %1" ).arg( _dbPath ) );

  if( !_dbSchema.isEmpty() )
    result.append( QStringLiteral( " schema: %1" ).arg( _dbSchema ) );

  return result;
}


void CConfigDatabase::assign( const CConfigDatabase& other ) {
  _db = other._db;

  _dbIsOpen = other._dbIsOpen;
  _schemaIsOpen = other._schemaIsOpen;

  _type = other._type;
  _name = other._name;
  _dbHost = other._dbHost;
  _dbPort = other._dbPort;
  _dbName = other._dbName;
  _dbPath = other._dbPath;
  _dbUser = other._dbUser;
  _dbPassword = other._dbPassword;
  _dbSchema = other._dbSchema;
  _dbTable = other._dbTable;

  _connectionName = other._connectionName;
}


void CConfigDatabase::processPair( const QString& key, const QString& val ) {
  if( 0 == key.compare( QLatin1String("databasetype") ) )
    setType( dbTypeFromString( val ) );
  else if( 0 == key.compare( QLatin1String("databasehost") ) )
    setDbHost( val );
  else if( 0 == key.compare( QLatin1String("databaseport") ) )
    setDbPort( val.toInt() );
  else if( 0 == key.compare( QLatin1String("databasepath") ) )
    setDbPath( val );
  else if( 0 == key.compare( QLatin1String("databasename") ) )
    setDbName( val );
  else if( 0 == key.compare( QLatin1String("databaseschemaname") ) )
    setDbSchema( val );
  else if( 0 == key.compare( QLatin1String("databaseschema") ) )
    setDbSchema( val );
  else if( 0 == key.compare( QLatin1String("databasetablename") ) )
    setDbTable( val );
  else if( 0 == key.compare( QLatin1String("databaseuser") ) )
    setDbUser( val );
  else if( 0 == key.compare( QLatin1String("databasepassword") ) )
    setDbPassword( val, true );
  else if( 0 == key.compare( QLatin1String("databasepasswordencrypted") ) )
    setDbPassword( val, true );
  else if( 0 == key.compare( QLatin1String("databasepasswordplain") ) )
    setDbPassword( val, false );
  else {
    // FIX ME: Should this set an error flag?
    qDebug() << "Unsupported key value encountered in CConfigDatabase::processPair:" << key;
  }
}


void CConfigDatabase::initialize() {
  setName( QString() );
  setType( DBTypePostgreSQL );
  setDbHost( QString() );
  setDbPort( 5432 );
  setDbName( QString() );
  setDbUser( QString() );
  setDbPassword( QString(), false );
  setDbSchema( QString() );
  setDbTable( QString() );

  _db = nullptr;
  _dbIsOpen = false;
  _schemaIsOpen = false;
}


CConfigDatabase::~CConfigDatabase() {
  closeDatabase();
}


bool CConfigDatabase::isValid( QString* errMsg /* = nullptr */ ) const {
  bool result = true; // Until shown otherwise.
  bool test;

  // The following fields are required for a valid database:
  if( DBTypeUnspecified == _type ) {
    appendToMessage( errMsg, QStringLiteral("Database type is not specified.\n") );
    result = false;
  }

  // Path is required for SQLite
  if( DBTypeSQLite == _type ) {
    test = !_dbPath.isEmpty();
    if( !test ) {
      appendToMessage( errMsg, QStringLiteral("Database path is not specified.\n") );
      result = false;
    }
  }

  // Host, name, port, user, and password are required for PostgreSQL and MySQL, but not for SQLite.
  else {
    test = !_dbName.isEmpty();
    if( !test ) {
      appendToMessage( errMsg, QStringLiteral("Database name is not specified.\n") );
      result = false;
    }

    test = !_dbHost.isEmpty();
    if( !test ) {
      appendToMessage( errMsg, QStringLiteral("Database host name is not specified.\n") );
      result = false;
    }

    test = !( 0 == _dbPort );
    if( !test ) {
      appendToMessage( errMsg, QStringLiteral("Database port is not specified.\n") );
      result = false;
    }

    test = !_dbUser.isEmpty();
    if( !test ) {
      appendToMessage( errMsg, QStringLiteral("Database user name is not specified.\n") );
      result = false;
    }

    test = !_dbPassword.isEmpty();
    if( !test ) {
      appendToMessage( errMsg, QStringLiteral("Database password is not specified.\n") );
      result = false;
    }
  }

  // Schema name is currently optional.

  return result;
}


bool CConfigDatabase::isOpen( QString* errMsg /* = nullptr */ ) const {
  return( isValid( errMsg ) && _dbIsOpen );
}


int CConfigDatabase::lastInsertID( const QString& sequenceName /* = "" */ ) {
  Q_ASSERT( this->_type != DBTypeUnspecified );

  int result = 0;

  switch( this->_type ) {
    case DBTypeUnspecified:
      Q_UNREACHABLE();
      break;
    case DBTypePostgreSQL:
      Q_ASSERT( !sequenceName.isEmpty() );
      result = getLastInsertId( this->database(), sequenceName );
      break;
    case DBTypeMySql: {
        QSqlQuery query( *( this->database() ) );
        query.exec( QStringLiteral("SELECT last_insert_id()") );
        query.first();
        result = query.value( 0 ).toInt();
      }
      break;
    case DBTypeSQLite: {
      QSqlQuery query( *( this->database() ) );
      query.exec( QStringLiteral("SELECT last_insert_rowid()") );
      query.first();
      result = query.value( 0 ).toInt();
    }
    break;
  }

  return result;
}


bool CConfigDatabase::schemaVersionOK( const QString& db_version, const QString& db_version_application, const QString& db_version_id, QString* errMsg /* = nullptr */ ) {
  bool result = true; // Until shown otherwise.

  if( !_dbIsOpen )
    result = openDatabase();

  if( result && !_schemaIsOpen )
    result = openSchema();

  if( result ) {
    QString q = QStringLiteral("SELECT version_number, version_application, version_date, version_id FROM db_schema_version");

    QSqlQuery query( *_db );

    if( !query.exec( q ) ) {
      appendToMessage( errMsg, QStringLiteral( "db_schema_version query could not be executed: %1\n%2\n" ).arg( query.lastQuery(), query.lastError().text() ) );
      result =  false;
    }
    else if( 1 != query.size() ) {
      appendToMessage( errMsg, QStringLiteral( "Wrong number of records (%1) in table db_schema_version.\n" ).arg( query.size() ) );
      result =  false;
    }
    else {
      query.first();

      result = (
        ( query.value( QStringLiteral("version_number") ).toString() == QString( db_version ) )
        && ( query.value( QStringLiteral("version_application") ).toString() == db_version_application )
        && ( query.value( QStringLiteral("version_id") ).toString() == db_version_id )
      );
    }
  }

  return result;
}


QSqlDatabase* CConfigDatabase::database() {
  if( nullptr == _db )
    openDatabase();

  return _db;
}


void CConfigDatabase::setDbPassword( const QString& val, const bool useEncryption /* = true */ ) {
  if( useEncryption ) {
    _dbPassword = CEncryption::rc4Encrypt( CEncryption::hexDecode( val ), APP_PASSWORD_KEY );
  }
  else {
    _dbPassword = val;
  }
}


bool CConfigDatabase::openDatabase() {
  bool result;

  if( _dbIsOpen )
    result = true;
  else {
    result = isValid( &_errorMsg );

    if( result ) {

      // Get an available connection name
      if( this->name().isEmpty() )
        _connectionName = QStringLiteral("epicprocDb");
      else
        _connectionName = QStringLiteral( "configDb%1" ).arg( this->name() );

      while( QSqlDatabase::connectionNames().contains( _connectionName ) ) {
        _connectionName.append( QString( "%1" ).arg( qrand() ) );
      }

      // Set up the QSqlDatabase
      switch( _type ) {
        case DBTypePostgreSQL:
          _db = new QSqlDatabase( QSqlDatabase::addDatabase( QStringLiteral("QPSQL"), _connectionName ) );
          break;
        case DBTypeMySql:
          _db = new QSqlDatabase( QSqlDatabase::addDatabase( QStringLiteral("QMYSQL"), _connectionName ) );
          break;
        case DBTypeSQLite:
          _db = new QSqlDatabase( QSqlDatabase::addDatabase( QStringLiteral("QSQLITE"), _connectionName ) );
          break;
        default:
          Q_ASSERT_X( false, "dbtype", "Unsupported database type" );
          break;
      }

      if( DBTypeSQLite == _type ) {
        _db->setDatabaseName( _dbPath );
      }
      else {
        _db->setDatabaseName( _dbName );
        _db->setHostName( _dbHost );
        _db->setPort( _dbPort );
        _db->setUserName( _dbUser );
        _db->setPassword( _dbPassword );
      }

      result = _db->open();

      _dbIsOpen = result;

      if( !_db->isOpen() ) {
        appendToMessage( &_errorMsg, _db->lastError().text() );

        delete _db;
        _db = nullptr;
        QSqlDatabase::removeDatabase( _connectionName );
      }
    }
  }

  return result;
}


void CConfigDatabase::closeDatabase() {
  bool wasOpen;

  if( nullptr != _db ) {
    wasOpen = _db->isOpen();

    if( _db->isOpen() )
      _db->close();
    delete _db;
    _db = nullptr;
  }
  else
    wasOpen = false;

  if( wasOpen )
    QSqlDatabase::removeDatabase( _connectionName );

  _dbIsOpen = false;
  _schemaIsOpen = false;
}


bool CConfigDatabase::openSchema() { 
  return openSchema( this->dbSchema() );
}


bool CConfigDatabase::openSchema( const QString& schemaName ) { 
  bool result = true; // until shown otherwise
  QString qstr;
  QSqlQuery* query = nullptr;

  if( !_dbIsOpen )
    result = openDatabase();

  switch( _type ) {
    case DBTypePostgreSQL:

      if( result && !schemaName.isEmpty() ) {
        query = new QSqlQuery( *_db );

        // See http://stackoverflow.com/questions/7016419/postgresql-check-if-schema-exists for discussion
        //result = query->exec( QString( "SELECT EXISTS( select schema_name FROM information_schema.schemata WHERE schema_name = '%1' )" ).arg( schemaName ) );
        result = query->exec( QStringLiteral( "SELECT EXISTS(SELECT 1 FROM pg_namespace WHERE nspname = '%1')" ).arg( schemaName ) );

        if( result )
          result = query->next();

        if( result )
          result = query->value( 0 ).toBool();

        if( result ) {
          qstr = QStringLiteral( "SET search_path TO \"%1\"" ).arg( schemaName );
          result = query->exec( qstr );
        }
      }
      else {
        result = false;
      }
      delete query;

      break;
    case DBTypeMySql:
      result = true;
      break;
    case DBTypeSQLite:
      result = true;
      break;
    default:
      result = false;
      break;
  }


  _schemaIsOpen = result;

  return result;
}


bool CConfigDatabase::hasTable( QString tableName ) {
  if( !this->dbSchema().isEmpty() && ( DBTypePostgreSQL == _type ) )
    tableName = QStringLiteral( "%1.%2" ).arg( this->dbSchema(), tableName.trimmed() );

  return this->database()->tables().contains( tableName.trimmed() );
}


void CConfigDatabase::writeToStream( QTextStream* stream ) {
  if( !_name.isEmpty() )
    *stream << "[Database " << _name << "]" << endl;
  else
    *stream << "[Database]" << endl;

  *stream << "  DatabaseType <- " << dbTypeToString( _type ) << endl;

  if( !_dbHost.isEmpty() )
    *stream << "  DatabaseHost <- " << _dbHost << endl;

  if( !_dbUser.isEmpty() )
    *stream << "  DatabaseUser <- " << _dbUser << endl;

  if( _type != DBTypeSQLite )
    *stream << "  DatabasePort <- " << _dbPort << endl;

  if( !_dbPassword.isEmpty() )
    *stream << "  DatabasePasswordEncrypted <- " << CEncryption::hexEncode( CEncryption::rc4Encrypt( _dbPassword, APP_PASSWORD_KEY ) ) << endl;

  if( !_dbPath.isEmpty() )
    *stream << "  DatabasePath <- " << _dbPath << endl;

  if( !_dbName.isEmpty() )
    *stream << "  DatabaseName <- " << _dbName << endl;

  if( !_dbSchema.isEmpty() )
    *stream << "  DatabaseSchema <- " << _dbSchema << endl;

  if( !_dbTable.isEmpty() )
    *stream << "  DatabaseTable <- " << _dbTable << endl;

  *stream << endl;
}


void CConfigDatabase::debug() {
  qDb() << "  Database configuration:";
  qDb() << "    type:" << dbTypeToString( _type );
  qDb() << "    name:" << _name;
  qDb() << "    dbHost:" << _dbHost;
  qDb() << "    dbPort:" << _dbPort;
  qDb() << "    dbPath:" << _dbPath;
  qDb() << "    dbName:" << _dbName;
  qDb() << "    dbUser:" << _dbUser;
  qDb() << "    dbPassword:" << _dbPassword;
  qDb() << "    dbSchema:" << _dbSchema;
  qDb() << "    dbTable:" << _dbTable;
  qDb() << "    isOpen:" << this->isOpen();
  qDb() << endl;
}
//=============================================================================



//=============================================================================
// CConfigDatabaseList
//=============================================================================
CConfigDatabaseList::CConfigDatabaseList() {
  // Nothing to do here at the moment.
}

CConfigDatabaseList::CConfigDatabaseList( const CConfigDatabaseList& other ) : QVector<CConfigDatabase*>() {
  assign( other );
}

CConfigDatabaseList& CConfigDatabaseList::operator=( const CConfigDatabaseList& other ) {
  assign( other );
  return *this;
}

void CConfigDatabaseList::assign( const CConfigDatabaseList& other ) {
  this->clear();
  for( int i = 0; i < other.count(); ++i ) {
    this->append( new CConfigDatabase( *other.at(i) ) );
  }
}


CConfigDatabaseList::~CConfigDatabaseList() {
  while( !this->isEmpty() ) {
    delete this->takeLast();
  }
}


bool CConfigDatabaseList::isValid( QString* errMsg ) const {
  int i;
  bool result = true;

  for( i = 0; i < this->count(); ++i )
    result = result && this->at(i)->isValid( errMsg );

  return result;
}


bool CConfigDatabaseList::hasDb( const QString& name ) const {
  int i;
  bool result = false;

  for( i = 0; i < this->count(); ++i ) {
    if( 0 == this->at(i)->name().compare( name, Qt::CaseInsensitive ) ) {
      result = true;
      break;
    }
  }

  return result;
}


CConfigDatabase* CConfigDatabaseList::at( const QString& name ) const {
  CConfigDatabase* result = nullptr;

  for( int i = 0; i < this->count(); ++i ) {
    if( this->at(i)->name() == name ) {
      result = this->at(i);
      break;
    }
  }

  return result;
}



void CConfigDatabaseList::writeToStream( QTextStream* stream ) {
  if( nullptr == stream )
    return;

  for( int i = 0; i < this->count(); ++i )
    this->at(i)->writeToStream( stream );
}


void CConfigDatabaseList::debug() const {
  qDebug() << QStringLiteral( "Database list: %1 items" ).arg( this->count() );
  for( int i = 0; i < this->count(); ++i ) {
    qDebug() << "  Database " << i;
    this->at(i)->debug();
  }
}
//=============================================================================
#endif



//=============================================================================
// CConfigFile
//=============================================================================
CConfigFile::CConfigFile() {
  initialize();
}

void CConfigFile::initialize() {
  _allowRepeatedKeys = false;
  _fileName = QString();
  _errorMessage = QString();
  _returnValue = ConfigReturnCode::Success;
}


CConfigFile::CConfigFile( QStringList* args ) {
  initialize();

  _fileName = QString();

  if( 1 != args->count() ) {
    _returnValue = ConfigReturnCode::BadArguments;
  }
  else {
    buildBasic( args->at(0) );
    
    #ifdef QSQL_USED
      buildDatabases();
    #endif
  }
  
}


CConfigFile::CConfigFile( const QString& configFileName, const bool allowRepeatedKeys /* = false */ ) {
  initialize();

  _allowRepeatedKeys = allowRepeatedKeys;

  _fileName = configFileName;

  buildBasic( configFileName );
  
  #ifdef QSQL_USED
    buildDatabases();
  #endif
}


CConfigFile::~CConfigFile() {
  // Remember that _blockList and _blockHash refer to the same blocks.
  // It's only necessary to delete them once.
  while( !_blockList.isEmpty() )
    delete _blockList.takeLast();
}


bool CConfigFile::validate() {
  bool result = ( ConfigReturnCode::Success == _returnValue );
  bool tmp2;

  #ifdef QSQL_USED
    tmp2 = _dbList.isValid( &_errorMessage );

    if( !tmp2 ) {
      _returnValue = ConfigReturnCode::BadDatabaseConfiguration;
    }
  #else
    tmp2 = true;
  #endif
  
  result = ( result && tmp2 );

  return result;
}


void CConfigFile::buildBasic( const QString& fn ) {
  // Until shown otherwise...
  _returnValue = ConfigReturnCode::Success;
  _errorMessage = QString();

  // Attempt to parse the file.
  QFile file( fn );

  if( !file.exists() )
    _returnValue =  ConfigReturnCode::MissingConfigFile;
  else if (!file.open( QIODevice::ReadOnly | QIODevice::Text))
    _returnValue = ConfigReturnCode::CannotOpenConfigFile;
  else
    _returnValue = processFile( &file );
}


#ifdef QSQL_USED
void CConfigFile::buildDatabases() {
  CConfigBlock* block;
  QString key;

  for( int i = 0; i < _blockList.count(); ++i ) {
    block = _blockList.at(i);
    key = block->name();

    if( 0 == key.leftRef(8).compare( QLatin1String("database") ) ) {
      QString name;
      QStringList pieces = key.split( QRegExp( "\\s+") );
      if( 2 == pieces.length() )
        name = pieces.at(1);
      else
        name = QString();

      _dbList.append( new CConfigDatabase( block, name ) );
      block->setRemoved( true );
    }
  }
}
#endif


bool CConfigFile::contains( const QString& blockName, const QString& key ) const {
  // There may be multiple blocks with the same name.
  // This function checks only the FIRST REMAINING block with the indicated name.
  bool result = false;
  QString keyX = key.trimmed().toLower();
  QString blockNameX = blockName.trimmed().toLower();

  for( int i = 0; i < _blockList.count(); ++i ) {
    CConfigBlock* block = _blockList.at(i);

    if( !block->removed() && (block->name().toLower() == blockNameX) ) {
      if( block->contains( keyX ) ) {
        result = true;
      }
      break;
    }
  }

  return result;
}


bool CConfigFile::contains( const QString& blockName, const QString& key, const QString& value ) const {
  // There may be multiple blocks with the same name.
  // This function checks only the FIRST REMAINING block with the indicated name.
  bool result = false;
  QString keyX = key.trimmed().toLower();
  QString blockNameX = blockName.trimmed().toLower();

  for( int i = 0; i < _blockList.count(); ++i ) {
    CConfigBlock* block = _blockList.at(i);

    if( !block->removed() && (block->name().toLower() == blockNameX) ) {
      if( block->contains( keyX ) ) {
        if( value == block->value( keyX ) )
          result = true;
        else
          result = false;
      }
      break;
    }
  }

  return result;
}


QString CConfigFile::value( const QString& blockName, const QString& key ) const {
  // There may be multiple blocks with the same name.
  // This function checks only the FIRST REMAINING block with the indicated name.
  QString result;
  QString keyX = key.trimmed().toLower();
  QString blockNameX = blockName.trimmed().toLower();

  for( int i = 0; i < _blockList.count(); ++i ) {
    CConfigBlock* block = _blockList.at(i);

    if( !block->removed() && (block->name().toLower() == blockNameX) ) {
      if( block->contains( keyX ) ) {
        result = block->value( keyX );
      }
      break;
    }
  }

  return result;
}


int CConfigFile::multiContains( const QString& blockName, const QString& key ) const {
  int result = 0;
   QString keyX = key.trimmed().toLower();

  QList<CConfigBlock*> blocks = _blockHash.values( blockName.trimmed().toLower() );
  for( int i = 0; i < blocks.count(); ++i ) {
    CConfigBlock* block = blocks.at(i);

    if( !block->removed() && block->contains( keyX ) ) {
      ++result;
    }
  }

  return result;
}


QStringList CConfigFile::values( const QString& blockName, const QString& key ) const {
  QStringList result;
   QString keyX = key.trimmed().toLower();

  QList<CConfigBlock*> blocks = this->blocks( blockName );
  for( int i = 0; i < blocks.count(); ++i ) {
    CConfigBlock* block = blocks.at(i);

    if( !block->removed() && block->contains( keyX ) ) {
      result.append( block->value( keyX ) );
    }
  }

  return result;
}


bool CConfigFile::contains( const QString& blockName ) const {
  bool result = false;

  QList<CConfigBlock*> blocks = _blockHash.values( blockName.trimmed().toLower() );
  for( int i = 0; i < blocks.count(); ++i ) {
    if( !blocks.at(i)->removed() ) {
      result = true;
      break;
    }
  }

  return result;
}


int CConfigFile::multiContains( const QString& blockName ) const {
  int result = 0;

  QList<CConfigBlock*> blocks = _blockHash.values( blockName.trimmed().toLower() );
  for( int i = 0; i < blocks.count(); ++i ) {
    if( !blocks.at(i)->removed() ) {
      ++result;
    }
  }

  return result;
}


CConfigBlock* CConfigFile::block( const QString& blockName ) const {
  QList<CConfigBlock*> blocks = this->blocks( blockName );
  for( int i = 0; i < blocks.count(); ++i ) {
    if( !blocks.at(i)->removed() ) {
      return blocks.at(i);
    }
  }

  return nullptr;
}


QList<CConfigBlock*> CConfigFile::blocks( const QString& blockName ) const {
  QList<CConfigBlock*> list;

  for( int i = 0; i < _blockList.count(); ++i ) {
    if( 0 == _blockList.at(i)->name().compare( blockName.trimmed(), Qt::CaseInsensitive ) ) {
      list.append( _blockList.at(i) );
    }
  }

  return list;
}


void CConfigFile::debug( const bool showRemovedBlocks /* = true */ ) const {
  for( int i = 0; i < _blockList.count(); ++i ) {
    CConfigBlock* block = _blockList.at(i);
    if( !block->removed() || showRemovedBlocks ) {
      block->debug();
      qDb() << endl;
    }
  }

  #ifdef QSQL_USED
  _dbList.debug();
  #endif

  qDb() << "Configuration return code:" << this->_returnValue << ConfigReturnCode::resultString( this->_returnValue );
}


void CConfigFile::writeToStream( QTextStream* stream ) {
  if( !_blockList.isEmpty() && ( nullptr != stream ) ) {
    for( int i = 0; i < _blockList.count(); ++i ) {
      CConfigBlock* block = _blockList.at(i);
      if( !block->removed() ) {
        block->writeToStream( stream );
        *stream << endl;
      }
    }
  }
  
  #ifdef QSQL_USED
    if( !_dbList.isEmpty() ) {
      _dbList.writeToStream( stream );
    }
  #endif
}


int CConfigFile::fillBlock( CConfigBlock* block, const QStringList& strList ) {
  int result = ConfigReturnCode::Success; // until shown otherwise

  QStringList lineParts;
  QString key, val;

  for( int i = 0; i < strList.count(); ++i ) {
    lineParts.clear();
    lineParts = strList.at(i).split( QStringLiteral("<-") );

    if( 2 != lineParts.count() ) {
      qDb() << "Wrong number of line parts:" << lineParts;
      result = ConfigReturnCode::BadConfiguration;
    }
    else {
      key = lineParts.at(0).trimmed().toLower();
      val = lineParts.at(1).trimmed();

      if( block->contains( key ) && !_allowRepeatedKeys ) {
        qDb() << "Duplicated block key:" << key;
        result = ConfigReturnCode::BadConfiguration;
      }
      else {
        block->insert( CConfigItem( key, val ) );
      }
    }
  }

  return result;
}


int CConfigFile::processFile( QFile* file ) {
  // Read the contents of the file, line-by-line and block-by-block,
  // discarding any comments.
  // Once a block has been assembled, send it to the appropriate object
  // to be constructed.

  int result = ConfigReturnCode::Success; // until shown otherwise

  QString line = QString();
  QStringList block;
  QTextStream in(file);
  bool inComment = false;

  while (!in.atEnd()) {
    line = in.readLine().trimmed();

    if( line.isEmpty() || line.startsWith('#') )
      continue;
    else if( ( 0 == line.compare( QLatin1String("[startcomment]"), Qt::CaseInsensitive ) ) || ( 0 == line.compare( QLatin1String("[begincomment]"), Qt::CaseInsensitive ) ) ) {
      inComment = true;
      continue;
    }
    else if( 0 == line.compare( QLatin1String("[endcomment]"), Qt::CaseInsensitive ) ) {
      inComment = false;
      continue;
    }
    else if( inComment ) {
      continue;
    }
    else if( !inComment && ( 0 == line.compare( QLatin1String("[endconfig]"), Qt::CaseInsensitive ) ) ) {
      break;
    }
    else {
      line = line.left( line.indexOf('#') - 1 );

      // Are we starting a new block?
      if( line.startsWith('[') ) {
        if( block.isEmpty() ) {
          block.append( line );
        }
        else {
          result = processBlock( block );

          if( ConfigReturnCode::Success == result ) {
            block.clear();
            block.append( line );
          }
          else {
            break;
          }
        }
      }
      else {
        block.append( line );
      }
    }
  }

  // Process the last block
  if( ConfigReturnCode::Success == result )
    result = processBlock( block );

  return result;
}


int CConfigFile::processBlock( QStringList strList ) {
  int result = ConfigReturnCode::Success; // until shown otherwise

  QString line0 = strList.takeFirst().toLower();

  QString blockName = line0.mid( 1, line0.length() - 2 );
  CConfigBlock* block = new CConfigBlock( blockName );
  result = fillBlock( block, strList );
  _blockList.append( block );
  _blockHash.insert( blockName, block );

  return result;
}


bool CConfigFile::setWorkingDirectory() {
  if( this->contains( QStringLiteral("Directories"), QStringLiteral("WorkingDir") ) ) {
    return QDir::setCurrent( this->value( QStringLiteral("Directories"), QStringLiteral("WorkingDir") ) );
  }
  else {
    return true;
  }
}
//=============================================================================


