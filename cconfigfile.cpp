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

CConfigFile::CConfigFile( QStringList* args ) {
  if( 1 != args->count() )
    _returnValue = ConfigReturnCode::BadArguments;
  else
    buildBasic( args->at(0) );
}


CConfigFile::CConfigFile( const QString& configFileName ) {
  buildBasic( configFileName );
}


CConfigFile::~CConfigFile() {
  foreach( const QString& key, _blocks->keys() )
    delete _blocks->take( key );

  delete _blocks;
}


void CConfigFile::buildBasic( const QString& fn ) {
  QFile* file = NULL;

  _blocks = new QHash< QString, QHash<QString, QString>* >();

  // Until shown otherwise...
  _returnValue = ConfigReturnCode::Success;
  _errorMessage = "";

  // Attempt to parse the file.
  file = new QFile( fn );

  if( !file->exists() )
    _returnValue =  ConfigReturnCode::MissingConfigFile;
  else if (!file->open( QIODevice::ReadOnly | QIODevice::Text))
    _returnValue = ConfigReturnCode::CannotOpenConfigFile;
  else
    _returnValue = processFile( file );

  delete file;
}


bool CConfigFile::contains( const QString& blockName, const QString& key ) const {
  if( !_blocks->contains( blockName.toLower() ) )
    return false;
  else if( !_blocks->value( blockName.toLower() )->contains( key.toLower() ) )
    return false;
  else
    return true;
}


QString CConfigFile::value( const QString& blockName, const QString& key ) const {
  if( !_blocks->contains( blockName.toLower() ) )
    return "";
  else if( !_blocks->value( blockName.toLower() )->contains( key.toLower() ) )
    return "";
  else
    return _blocks->value( blockName.toLower() )->value( key.toLower() );
}


void CConfigFile::debug() {
  foreach( const QString& blockKey, _blocks->keys() ) {
    qDebug() << QString( "Block '%1':" ).arg( blockKey ) << _blocks->value( blockKey )->count();

    foreach( const QString& key, _blocks->value( blockKey )->keys() )
      qDebug() << key << _blocks->value( blockKey )->value( key );
    qDebug() << endl;
  }

  qDebug() << "Configuration return code:" << this->_returnValue << ConfigReturnCode::resultString( this->_returnValue );
}


int CConfigFile::fillBlock( QHash<QString, QString>* block, QStringList strList ) {
  int result = ConfigReturnCode::Success; // until shown otherwise

  QStringList lineParts;
  QString key, val;

  for( int i = 0; i < strList.count(); ++i ) {
    lineParts.clear();
    lineParts = strList.at(i).split( "<-" );

    if( 2 != lineParts.count() )
      result = ConfigReturnCode::BadDatabaseConfiguration;
    else {
      key = lineParts.at(0).trimmed().toLower();
      val = lineParts.at(1).trimmed();

     if( block->contains( key ) )
       result = ConfigReturnCode::BadDatabaseConfiguration;
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

  while (!in.atEnd()) {
    line = in.readLine().trimmed();

    if( line.isEmpty() || line.startsWith('#') )
      continue;
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
  QHash<QString, QString>* block = new QHash<QString, QString>();
  result = fillBlock( block, strList );
  _blocks->insert( blockName, block );

  return result;
}


