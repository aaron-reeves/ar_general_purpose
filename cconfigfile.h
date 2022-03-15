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

#ifndef CCONFIGFILE_H
#define CCONFIGFILE_H

#include <QtCore>

#ifdef QSQL_USED
  #include <QSqlDatabase>
#endif

extern const QString APP_PASSWORD_KEY;

QString makeEncryptedPassword( const QString& password );
void appendToMessage( QString* message, const QString& newMessage );

/*
 * Exit codes returned by the application.  Use --help for definitions.
 */
class ConfigReturnCode {
  public:
    enum returnCodes {
      Success,
      SuccessWithBadRows,  // Make sure that this is always the last "success" message.  Other bits of code rely on it!
      BadArguments,
      UnrecognizedFunction,
      MissingConfigFile,
      CannotOpenConfigFile,
      BadConfiguration,
      BadFunctionConfiguration,
      BadDatabaseConfiguration,
      BadDatabaseSchema,
      MissingInstructions,
      EmptyInputFile,
      CannotOpenInputFile,
      QueryDidNotExecute,
      CannotOpenOutputFile,
      BadFileFormat,
      BadDataField,

      // Insert any new codes here.

      LastReturnCode // Not a real return code.  Used to easily iterate over all return codes.
    };

    static QString resultString( const int returnCode );
};


class CConfigItem {
  public:
    CConfigItem( const QString& key, const QString& value ) { _key = key; _value = value; }
    CConfigItem( const CConfigItem& other ) { assign( other ); }
    CConfigItem& operator=( const CConfigItem& other ) { assign( other ); return *this; }
    ~CConfigItem() { /* Nothing to do here */ }

    QString key() const { return _key; }
    QString value() const { return _value; }

  protected:
    void assign( const CConfigItem& other ) { _key = other._key; _value = other._value; }

    QString _key;
    QString _value;
};

inline bool operator==( const CConfigItem& lhs, const CConfigItem& rhs ) {
  return( (lhs.key() == rhs.key()) && (lhs.value() == rhs.value()) );
}
inline bool operator!=( const CConfigItem& lhs, const CConfigItem& rhs ) {return !(lhs == rhs);}
inline bool operator<( const CConfigItem& lhs, const CConfigItem& rhs ) { return( lhs.key() < rhs.key() ); }
inline bool operator>( const CConfigItem& lhs, const CConfigItem& rhs ) { return( lhs.key() > rhs.key() ); }

// Hashing function that allows the use of CCPH as a key type for a QHash or QSet
inline uint qHash( const CConfigItem &key, uint seed ){
  return qHash( key.key(), seed );
}


class CConfigBlock {
  public:
    CConfigBlock();
    CConfigBlock( const QString& name );
    CConfigBlock( const CConfigBlock& other );
    CConfigBlock& operator=( const CConfigBlock& other );
    ~CConfigBlock();

    void insert( const CConfigItem& item );

    // Does the block contain AT LEAST one value with the specified key?
    bool contains( const QString& key );

    // How many values with the specified key?
    int multiContains( const QString& key ) const;

    // Return the FIRST value with the specified key
    QString value( const QString& key ) const;

    // Return a list of ALL values with the specified key, in the order in which they appear in the block
    QStringList values( const QString& key ) const;

    QList<CConfigItem> items() const { return _itemList; }

    QString name() const { return _name; }
    bool removed() const { return _removed; }
    void setRemoved( const bool val ) { _removed = val; }

    void writeToStream( QTextStream* stream );
    void debug() const;

  protected:
    void assign( const CConfigBlock& other );

    QList<CConfigItem> _itemList;
    QMultiHash<QString, CConfigItem> _itemHash;

    QString _name;
    bool _removed;  
};


#ifdef QSQL_USED
class CConfigDatabase {
  public:
    enum DBType {
      DBTypeUnspecified,
      DBTypePostgreSQL,
      DBTypeMySql,
      DBTypeSQLite
    };
    static DBType dbTypeFromString( QString val );
    static QString dbTypeToString( CConfigDatabase::DBType val );

    CConfigDatabase( const QString& name = QString(), const DBType type = DBTypePostgreSQL );
    CConfigDatabase( const QStringList& configBlock, const QString& name = QString(), const DBType type = DBTypePostgreSQL );
    CConfigDatabase( CConfigBlock* block, const QString& name = QString(), const DBType type = DBTypePostgreSQL );
    CConfigDatabase( const CConfigDatabase& other );
    CConfigDatabase& operator=( const CConfigDatabase& other );
    ~CConfigDatabase();

    CConfigDatabase parameters( const int connectionNumber = -1 ) const; // Returns the parameters without an open connection, etc.  Used for concurrent processing when new connections need to be opened.

    // sequenceName is required for Postgres, but not for MySQL
    int lastInsertID( const QString& sequenceName = QString() );

    void writeToStream( QTextStream* stream );
    void debug();

    // Getters/setters
    void setType( const DBType type ) { _type = type; }
    void setName( const QString& name ) { _name = name; }
    void setDbHost( const QString& val ) { _dbHost = val; }
    void setDbPort( const int val ) { _dbPort = val; }
    void setDbName( const QString& val ) { _dbName = val; }
    void setDbPath( const QString&val ) { _dbPath = val; }
    void setDbUser( const QString& val ) { _dbUser = val; }
    void setDbPassword( const QString& val, const bool useEncryption = true );
    void setDbSchema( const QString& val ) { _dbSchema = val; }
    void setDbTable( const QString& val ) { _dbTable = val; }

    DBType type() const { return _type; }
    QString name() const { return _name; }
    QString dbHost() const { return _dbHost; }
    int dbPort() const { return _dbPort; }
    QString dbName() const { return _dbName; }
    QString dbPath() const { return _dbPath; }
    QString dbUser() const { return _dbUser; }
    //QString dbPassword() const { return _dbPassword; }
    QString dbSchema() const { return _dbSchema; }
    QString dbTable() const { return _dbTable; }

    bool openDatabase();
    bool openSchema();
    bool openSchema( const QString& schemaName );
    bool hasTable( QString tableName );
    void closeDatabase();
    QSqlDatabase* database();

    bool isValid( QString* errMsg = nullptr ) const;
    bool isOpen( QString* errMsg = nullptr ) const;
    bool schemaVersionOK( const QString& db_version, const QString& db_version_application, const QString& db_version_id, QString* errMsg = nullptr );

    QString errorMessage() const { return _errorMsg; }

    QString description() const;

  protected:
    void initialize();
    void assign( const CConfigDatabase& other );
    void processPair( const QString& key, const QString& val );

    QSqlDatabase* _db;

    bool _dbIsOpen;
    bool _schemaIsOpen;

    DBType _type;
    QString _name;
    QString _dbHost;
    int _dbPort;
    QString _dbName;
    QString _dbPath;
    QString _dbUser;
    QString _dbPassword;
    QString _dbSchema;
    QString _dbTable;

    QString _connectionName;

    QString _errorMsg;
};

Q_DECLARE_TYPEINFO( CConfigDatabase::DBType, Q_PRIMITIVE_TYPE );
Q_DECLARE_TYPEINFO( CConfigDatabase, Q_COMPLEX_TYPE );


class CConfigDatabaseList : public QVector<CConfigDatabase*> {
  public:
    CConfigDatabaseList();
    CConfigDatabaseList( const CConfigDatabaseList& other );
    CConfigDatabaseList& operator=( const CConfigDatabaseList& other );
    ~CConfigDatabaseList();


    CConfigDatabase* at( const int i ) const { return QVector<CConfigDatabase*>::at( i ); }
    CConfigDatabase* at( const QString& name ) const;

    bool isValid( QString* errMsg = nullptr ) const;
    bool hasDb( const QString& name ) const ;
    void writeToStream( QTextStream* stream );
    void debug() const;

  protected:
    void assign( const CConfigDatabaseList& other );
};
#endif


class CConfigFile {
  public:
    CConfigFile();
    CConfigFile( QStringList* args );
    CConfigFile( const QString& configFileName, const bool allowRepeatedKeys = false );
    //CConfigFile( const CConfigFile& other ); // Polymorphic class shouldn't be copied...
    virtual ~CConfigFile();

    bool allowRepeatedKeys() const { return _allowRepeatedKeys; }
    void setAllowRepeatedKeys( const bool val ) { _allowRepeatedKeys = val; }

    // FIXME: referencing individual blocks by index, rather than by name, may require some work in the code.

    // Does the file contain AT LEAST one block with the indicated name?
    bool contains( const QString& blockName ) const;
    
    // AT LEAST one block with the indicated key?
    bool contains(const QString& blockName, const QString& key ) const;
    
    // AT LEAST one block with the indicated key, set to the indicated value?
    bool contains( const QString& blockName, const QString& key, const QString& value ) const;

    // Return the FIRST block with the indicated name
    CConfigBlock* block( const QString& blockName ) const;
    
    // Return the value from the FIRST block with the indicated name
    QString value( const QString& blockName, const QString& key ) const;

    // How many blocks with the specified name?
    int multiContains( const QString& blockName ) const;
    
    // How many blocks with the specified name contain the key?
    int multiContains( const QString& blockName, const QString& key ) const;

    // Return a list of ALL blocks with the specified name, in the order in which they appear in the file
    QList<CConfigBlock*> blocks( const QString& blockName ) const;
    
    // Return the indicated key value from EVERY block with the specified name, in the order in which blocks appear in the file
    QStringList values( const QString& blockName, const QString& key ) const;

    virtual void debug( const bool showRemovedBlocks = true ) const;

    virtual void writeToStream( QTextStream* stream );

    int result() const { return _returnValue; }
    QString resultString() const { return ConfigReturnCode::resultString( _returnValue ); }

    QString errorMessage() const { return _errorMessage; }

    QString fileName() const { return _fileName; }

    bool setWorkingDirectory();
    
    virtual bool validate();
    
    #ifdef QSQL_USED
      CConfigDatabaseList dbList() const { return _dbList; }
      CConfigDatabase* dbConfig() const { if( 0 == _dbList.count() ) return nullptr; else return _dbList.at( 0 ); }
      CConfigDatabase* dbConfig( const QString& name ) const { if( 0 == _dbList.count() ) return nullptr; else return _dbList.at( name.toLower() ); }
      bool containsDatabase( const QString& name ) const { return _dbList.hasDb( name.toLower() ); }
      bool hasDb( const QString& name ) const { return containsDatabase( name ); }
    #endif
    
  protected:
    void initialize();
    
    #ifdef QSQL_USED
      void buildDatabases();
      CConfigDatabaseList _dbList;
    #endif
    
    void buildBasic( const QString& fn );
    int processBlock( QStringList strList );
    int processFile( QFile* file );
    int fillBlock( CConfigBlock* block, const QStringList& strList );

    QString _fileName;

    QList<CConfigBlock*> _blockList;
    QMultiHash<QString, CConfigBlock*> _blockHash;

    QString _errorMessage;
    int _returnValue;

    bool _allowRepeatedKeys;

  private:
    Q_DISABLE_COPY( CConfigFile )
};


#endif // CCONFIGFILE_H
