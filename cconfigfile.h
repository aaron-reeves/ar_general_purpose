/*
cconfigfile.h/cpp
-----------------
Begin: 2015-06-24
Author: Aaron Reeves <aaron.reeves@sruc.ac.uk>
---------------------------------------------------
Copyright (C) 2015 - 2019 Scotland's Rural College (SRUC)

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#ifndef CCONFIGFILE_H
#define CCONFIGFILE_H

#include <QtCore>

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

  protected:
    void initialize();

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
