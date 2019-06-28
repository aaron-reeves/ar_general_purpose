#include "cconfigfile.h"


QString ConfigReturnCode::resultString( const int& returnCode ) {
  QString result;
  switch( returnCode ) {
    case ConfigReturnCode::Success: result = "Success"; break;
    case ConfigReturnCode::SuccessWithBadRows: result = "Success with bad rows"; break;
    case ConfigReturnCode::BadArguments: result = "Bad arguments"; break;
    case ConfigReturnCode::UnrecognizedFunction: result = "Unrecognized function"; break;
    case ConfigReturnCode::MissingConfigFile: result = "Missing config file"; break;
    case ConfigReturnCode::CannotOpenConfigFile: result = "Cannot open config file"; break;
    case ConfigReturnCode::BadConfiguration: result = "Bad configuration"; break;
    case ConfigReturnCode::BadFunctionConfiguration: result = "Bad function configuration"; break;
    case ConfigReturnCode::BadDatabaseConfiguration: result = "Bad database configuration"; break;
    case ConfigReturnCode::BadDatabaseSchema: result = "Bad database schema"; break;
    case ConfigReturnCode::MissingInstructions: result = "Missing instructions"; break;
    case ConfigReturnCode::EmptyInputFile: result = "Empty input file"; break;
    case ConfigReturnCode::CannotOpenInputFile: result = "Cannot open input file"; break;
    case ConfigReturnCode::QueryDidNotExecute: result = "Query did not execute"; break;
    case ConfigReturnCode::CannotOpenOutputFile: result = "Cannot open output file"; break;
    case ConfigReturnCode::BadFileFormat: result = "Bad file format"; break;
    case ConfigReturnCode::BadDataField: result = "Bad data field"; break;

    default: result = "Undefined error.";
  }

  return result;
}


CConfigBlock::CConfigBlock( const QString& name ) : QMap<QString, QString>() {
  _name = name;
  _removed = false;
}


CConfigBlock::CConfigBlock( const CConfigBlock& other ) : QMap<QString, QString>( other ) {
  _name = other._name;
  _removed = other._removed;
}


CConfigBlock::~CConfigBlock() {
  // Do nothing.
}

void CConfigBlock::writeToStream( QTextStream* stream ) {
  *stream << "[" << this->name() << "]" << endl;

  foreach( const QString& key, this->keys() ) {
    *stream << "  " << key << " <- " << this->value( key ) << endl;
  }
}


void CConfigBlock::debug() {
  qDebug() << QString( "Block '%1':" ).arg( this->name() ) << this->count();

  foreach( const QString& key, this->keys() )
    qDebug() << key << this->value( key );
}


CConfigFile::CConfigFile() {
  _fileName = "";
  _errorMessage = "";
  _returnValue = ConfigReturnCode::Success;
}


CConfigFile::CConfigFile( QStringList* args ) {
  _fileName = "";

  if( 1 != args->count() )
    _returnValue = ConfigReturnCode::BadArguments;
  else
    buildBasic( args->at(0) );
}


CConfigFile::CConfigFile( const QString& configFileName ) {
  _fileName = configFileName;

  buildBasic( configFileName );
}


CConfigFile::CConfigFile( const CConfigFile& other ) {
  foreach( CConfigBlock* block, other._blockList ) {
    CConfigBlock* newBlock = new CConfigBlock( *block );
    _blockList.append( newBlock );
    _blockHash.insert( newBlock->name(), newBlock );
  }

  _fileName = other._fileName;
  _errorMessage = other._errorMessage;
  _returnValue = other._returnValue;
}


CConfigFile::~CConfigFile() {
  // Remember that _blockList and _blockHash refer to the same blocks.
  // It's only necessary to delete them once.
  while( !_blockList.isEmpty() )
    delete _blockList.takeLast();
}


void CConfigFile::buildBasic( const QString& fn ) {
  // Until shown otherwise...
  _returnValue = ConfigReturnCode::Success;
  _errorMessage = "";

  // Attempt to parse the file.
  QFile file( fn );

  if( !file.exists() )
    _returnValue =  ConfigReturnCode::MissingConfigFile;
  else if (!file.open( QIODevice::ReadOnly | QIODevice::Text))
    _returnValue = ConfigReturnCode::CannotOpenConfigFile;
  else
    _returnValue = processFile( &file );
}


bool CConfigFile::contains( QString blockName, QString key ) const {
  // There may be multiple blocks with the same name.
  // This function checks only the FIRST REMAINING block with the indicated name.
  bool result = false;
  key = key.trimmed().toLower();
  blockName = blockName.trimmed().toLower();

  for( int i = 0; i < _blockList.count(); ++i ) {
    CConfigBlock* block = _blockList.at(i);

    if( !block->removed() && (block->name().toLower() == blockName) ) {
      if( block->contains( key ) ) {
        result = true;
      }
      break;
    }
  }

  return result;
}


bool CConfigFile::contains( QString blockName, QString key, QString value ) const {
  // There may be multiple blocks with the same name.
  // This function checks only the FIRST REMAINING block with the indicated name.
  bool result = false;
  key = key.trimmed().toLower();
  blockName = blockName.trimmed().toLower();

  for( int i = 0; i < _blockList.count(); ++i ) {
    CConfigBlock* block = _blockList.at(i);

    if( !block->removed() && (block->name().toLower() == blockName) ) {
      if( block->contains( key ) ) {
        if( value == block->value( key ) )
          result = true;
        else
          result = false;
      }
      break;
    }
  }

  return result;
}


QString CConfigFile::value( QString blockName, QString key ) const {
  // There may be multiple blocks with the same name.
  // This function checks only the FIRST REMAINING block with the indicated name.
  QString result;
  key = key.trimmed().toLower();
  blockName = blockName.trimmed().toLower();

  for( int i = 0; i < _blockList.count(); ++i ) {
    CConfigBlock* block = _blockList.at(i);

    if( !block->removed() && (block->name().toLower() == blockName) ) {
      if( block->contains( key ) ) {
        result = block->value( key );
      }
      break;
    }
  }

  return result;
}


int CConfigFile::multiContains( QString blockName, QString key ) const {
  int result = 0;
  key = key.trimmed().toLower();
  blockName = blockName.trimmed().toLower();

  QList<CConfigBlock*> blocks = _blockHash.values( blockName );
  for( int i = 0; i < blocks.count(); ++i ) {
    CConfigBlock* block = blocks.at(i);

    if( !block->removed() && block->contains( key ) ) {
      ++result;
    }
  }

  return result;
}


QStringList CConfigFile::multiValues( QString blockName, QString key ) const {
  QStringList result;
  key = key.trimmed().toLower();
  blockName = blockName.trimmed().toLower();

  QList<CConfigBlock*> blocks = _blockHash.values( blockName );
  for( int i = 0; i < blocks.count(); ++i ) {
    CConfigBlock* block = blocks.at(i);

    if( !block->removed() && block->contains( key ) ) {
      result.append( block->value( key ) );
    }
  }

  return result;
}


bool CConfigFile::contains( QString blockName ) const {
  bool result = false;
  blockName = blockName.trimmed().toLower();

  QList<CConfigBlock*> blocks = _blockHash.values( blockName );
  for( int i = 0; i < blocks.count(); ++i ) {
    if( !blocks.at(i)->removed() ) {
      result = true;
      break;
    }
  }

  return result;
}


int CConfigFile::multiContains( QString blockName ) const {
  int result = 0;
  blockName = blockName.trimmed().toLower();

  QList<CConfigBlock*> blocks = _blockHash.values( blockName );
  for( int i = 0; i < blocks.count(); ++i ) {
    if( !blocks.at(i)->removed() ) {
      ++result;
    }
  }

  return result;
}


void CConfigFile::debug( const bool showRemovedBlocks /* = true */ ) {
  for( int i = 0; i < _blockList.count(); ++i ) {
    CConfigBlock* block = _blockList.at(i);
    if( !block->removed() || showRemovedBlocks ) {
      block->debug();
      qDebug() << endl;
    }
  }

  qDebug() << "Configuration return code:" << this->_returnValue << ConfigReturnCode::resultString( this->_returnValue );
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


int CConfigFile::fillBlock( CConfigBlock* block, QStringList strList ) {
  int result = ConfigReturnCode::Success; // until shown otherwise

  QStringList lineParts;
  QString key, val;

  for( int i = 0; i < strList.count(); ++i ) {
    lineParts.clear();
    lineParts = strList.at(i).split( "<-" );

    if( 2 != lineParts.count() ) {
      qDebug() << "Wrong number of line parts:" << lineParts;
      result = ConfigReturnCode::BadConfiguration;
    }
    else {
      key = lineParts.at(0).trimmed().toLower();
      val = lineParts.at(1).trimmed();

     if( block->contains( key ) ) {
       qDebug() << "Duplicated block key";
       result = ConfigReturnCode::BadConfiguration;
     }
     else
      block->insert( key, val );
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

  QString line = "";
  QStringList block;
  QTextStream in(file);
  bool inComment = false;

  while (!in.atEnd()) {
    line = in.readLine().trimmed();

    if( line.isEmpty() || line.startsWith('#') )
      continue;
    else if( ( 0 == line.compare( "[startcomment]", Qt::CaseInsensitive ) ) || ( 0 == line.compare( "[begincomment]", Qt::CaseInsensitive ) ) ) {
      inComment = true;
      continue;
    }
    else if( 0 == line.compare( "[endcomment]", Qt::CaseInsensitive ) ) {
      inComment = false;
      continue;
    }
    else if( inComment ) {
      continue;
    }
    else if( !inComment && ( 0 == line.compare( "[endconfig]", Qt::CaseInsensitive ) ) ) {
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
  if( this->contains( "Directories", "WorkingDir" ) ) {
    return QDir::setCurrent( this->value( "Directories", "WorkingDir" ) );
  }
  else {
    return true;
  }
}


