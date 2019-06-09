#include <ar_general_purpose/filemagic.h>

#include <magic.h>

#include <qcoreapplication.h>
#include <qdebug.h>
#include <qfile.h>

enum CheckFileTypes {
  CHECKTEXT,
  CHECKXLSX,
  CHECKXLS
};


QString setMagicPath( bool* error, QString* errorMessage ) {
  QString magicFile;

  if( QFile::exists( "C:/libs/C_libs/bin/magic" ) )
    magicFile = "C:/libs/C_libs/bin/magic";
  else if( QFile::exists(  QString( "%1/magic" ).arg( QCoreApplication::applicationDirPath() ) ) )
    magicFile = QString( "%1/magic" ).arg( QCoreApplication::applicationDirPath() );
  else if( QFile::exists( "/usr/share/file/magic" ) )
    magicFile = "/usr/share/file/magic";
  else if( QFile::exists( "/etc/magic" ) )
    magicFile = "/etc/magic";
  else {
    if( nullptr != error )
      *error = true;
    if( nullptr != errorMessage )
      *errorMessage = "Could not find magic file.";
  }

  return magicFile;
}


QString magicFileTypeInfo( const QString& fileName, bool* error /* = nullptr */, QString* errorMessage /* = nullptr */ ) {
  // Determine which magic file to use
  //----------------------------------
  QString magicFile = setMagicPath( error, errorMessage );

  if( magicFile.isEmpty() ) {
    return QString();
  }

  // Set up magic
  //-------------
  struct magic_set* magic = nullptr;
  int flags = 0;
  QString errMsg;

  magic = magicLoadMagic( magicFile, flags, errMsg );
  if( nullptr == magic ) {
    if( nullptr != error )
      *error = true;
    if( nullptr != errorMessage )
      *errorMessage = QString( "magicLoadMagic failed: %1" ).arg( errMsg );

    return "";
  }

  // Check the file type.
  //--------------------
  QString result;
  QString fileTypeInfo;
  bool magicOK = magicProcess( magic, fileName, fileTypeInfo, errMsg );

  if( magicOK ) {
    if( nullptr != error )
      *error = false;

    result = fileTypeInfo;
  }
  else {
    if( nullptr != error )
      *error = true;
    if( nullptr != errorMessage )
      *errorMessage = QString( "magicProcess failed: %1" ).arg( errMsg );

    result = "";
  }

  magicCloseMagic( magic );

  return result;
}


bool _magicIsType( const int type, const QString& fileName, bool* error /* = nullptr */, QString* returnTypeInfo /* = nullptr */, QString* errorMessage /* = nullptr */ ) {
  // Determine which magic file to use
  //----------------------------------
  QString magicFile = setMagicPath( error, errorMessage );

  if( magicFile.isEmpty() ) {
    return false;
  }

  // Set up magic
  //-------------
  struct magic_set* magic = nullptr;
  int flags = 0;
  QString errMsg;

  magic = magicLoadMagic( magicFile, flags, errMsg );

  if( nullptr == magic ) {
    if( nullptr != error )
      *error = true;    
    if( nullptr != errorMessage )
      *errorMessage = QString( "magicLoadMagic failed: %1" ).arg( errMsg );

    return false;
  }

  // Check the file type.
  //--------------------
  bool result;
  QString fileTypeInfo;

  bool magicOK = magicProcess( magic, fileName, fileTypeInfo, errMsg );

  if( nullptr != returnTypeInfo )
    *returnTypeInfo = fileTypeInfo;

  if( magicOK ) {
    if( nullptr != error )
      *error = false;

    switch( type ) {
      case CHECKTEXT:
          result = ( 0 == QRegExp( "^(ASCII)[\\s]+[\\sA-Za-z]*(text)" ).indexIn( fileTypeInfo ) );
          //    result = (
          //      fileTypeInfo.startsWith( "ASCII text" )
          //      || fileTypeInfo.startsWith( "ASCII English text" )
          //      || fileTypeInfo.startsWith( "ASCII C program text" )
          //    );
        break;
      case CHECKXLSX:
          result = (
            ( fileTypeInfo.startsWith( "Zip archive data" ) && fileName.endsWith( ".xlsx", Qt::CaseInsensitive ) )
            || ( 0 == fileTypeInfo.compare( "Microsoft Excel 2007+" ) )
            || ( fileTypeInfo.contains( "Microsoft OOXML" ) && fileName.endsWith( ".xlsx", Qt::CaseInsensitive ) )
          );
        break;
      case CHECKXLS:
        result = (
          fileTypeInfo.contains( "Composite Document File V2 Document" )
          || fileTypeInfo.contains( "CDF V2 Document" )
        );
        break;
      default:
          Q_ASSERT( false );
          result = false;
        break;
    }

  }
  else {
    if( nullptr != error )
      *error = true;
    if( nullptr != errorMessage )
      *errorMessage = QString( "magicProcess failed: %1" ).arg( errMsg );

    result = false;
  }

  magicCloseMagic( magic );

  return result;
}

bool magicIsXlsFile( const QString& fileName, bool* error /* = nullptr */, QString* returnTypeInfo /* = nullptr */, QString* errorMessage /* = nullptr */ ) {
  return _magicIsType( CHECKXLS, fileName, error, returnTypeInfo, errorMessage );
}


bool magicIsAsciiTextFile( const QString& fileName, bool* error /* = nullptr */, QString* returnTypeInfo /* = nullptr */, QString* errorMessage /* = nullptr */ ) {
  return _magicIsType( CHECKTEXT, fileName, error, returnTypeInfo, errorMessage );
}


bool magicIsXlsxFile( const QString& fileName, bool* error /* = nullptr */, QString* returnTypeInfo /* = nullptr */, QString* errorMessage /* = nullptr */ ) {
  return _magicIsType( CHECKXLSX, fileName, error, returnTypeInfo, errorMessage );
}

bool looksLikeTextFile( const QString& fileName ) {
  QFile f( fileName );

  if( f.open( QFile::ReadOnly ) ) {
    QByteArray sample = f.read( 512 );
    int nonPrintables = 0;

    for( int i = 0; i < sample.count(); ++i ) {
      QChar c( sample.at(i) );

      if( !c.isPrint() && !( '\n' == c ) && !( '\r' == c ) ) {
        ++nonPrintables;
      }
    }

    // Be strict for now, and don't allow any non-printable characters.
    return( 0 == nonPrintables );
  }
  else {
    return false;
  }
}


magic_set* magicLoadMagic( const QString& magicFile, int flags, QString& errMsg ) {
  struct magic_set* magic = magic_open( flags );

  if( nullptr == magic ) {
    errMsg = "Could not open magic.";
    return nullptr;
  }
  else {
    if( -1 == magic_load( magic,  magicFile.toLatin1().data() ) ) {
      errMsg = "Could not load magic.";
      magic_close( magic );
      return nullptr;
    }
    else {
      return magic;
    }
  }
}


bool magicProcess( struct magic_set* ms, const QString& fileName, QString& fileTypeInfo, QString& errMsg ) {
  const char* type = magic_file( ms, fileName.toLatin1().data() );

  if( nullptr == type ) {
    errMsg = QString( "%1" ).arg( magic_error( ms ) );
    fileTypeInfo = "";
    return false;
  }
  else {
    fileTypeInfo = QString( "%1" ).arg( type );
    errMsg = "";
    return true;
  }
}


void magicCloseMagic( struct magic_set* ms ) {
  magic_close( ms );
}
