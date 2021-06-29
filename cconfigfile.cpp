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

#include "cconfigfile.h"

#include <ar_general_purpose/arcommon.h>

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

  if( 1 != args->count() )
    _returnValue = ConfigReturnCode::BadArguments;
  else
    buildBasic( args->at(0) );
}


CConfigFile::CConfigFile( const QString& configFileName, const bool allowRepeatedKeys /* = false */ ) {
  initialize();

  _allowRepeatedKeys = allowRepeatedKeys;

  _fileName = configFileName;

  buildBasic( configFileName );
}

// Polymorphic classes should not be copied.
//CConfigFile::CConfigFile( const CConfigFile& other ) {
//  foreach( CConfigBlock* block, other._blockList ) {
//    CConfigBlock* newBlock = new CConfigBlock( *block );
//    _blockList.append( newBlock );
//    _blockHash.insert( newBlock->name(), newBlock );
//  }

//  _fileName = other._fileName;
//  _errorMessage = other._errorMessage;
//  _returnValue = other._returnValue;
//}


CConfigFile::~CConfigFile() {
  // Remember that _blockList and _blockHash refer to the same blocks.
  // It's only necessary to delete them once.
  while( !_blockList.isEmpty() )
    delete _blockList.takeLast();
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


