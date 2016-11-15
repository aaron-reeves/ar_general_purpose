#include <ar_general_purpose/filemagic.h>

#include <magic.h>

#include <qcoreapplication.h>
#include <qdebug.h>
#include <qfile.h>


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
    if( NULL != error )
      *error = true;
    if( NULL != errorMessage )
      *errorMessage = "Could not find magic file.";
  }

  return magicFile;
}


QString magicFileTypeInfo( QString fileName, bool* error /* = NULL */, QString* errorMessage /* = NULL */ ) {
  // Determine which magic file to use
  //----------------------------------
  QString magicFile = setMagicPath( error, errorMessage );

  if( magicFile.isEmpty() ) {
    return QString();
  }

  // Set up magic
  //-------------
  struct magic_set* magic = NULL;
  int flags = 0;
  QString errMsg;

  magic = magicLoadMagic( magicFile, flags, errMsg );
  if( NULL == magic ) {
    if( NULL != error )
      *error = true;
    if( NULL != errorMessage )
      *errorMessage = QString( "magicLoadMagic failed: %1" ).arg( errMsg );

    return "";
  }

  // Check the file type.
  //--------------------
  QString result;
  QString fileTypeInfo;
  bool magicOK = magicProcess( magic, fileName, fileTypeInfo, errMsg );

  if( magicOK ) {
    if( NULL != error )
      *error = false;

    result = fileTypeInfo;
  }
  else {
    if( NULL != error )
      *error = true;
    if( NULL != errorMessage )
      *errorMessage = QString( "magicProcess failed: %1" ).arg( errMsg );

    result = "";
  }

  magicCloseMagic( magic );

  return result;
}


bool magicIsAsciiTextFile( QString fileName, bool* error /* = NULL */, QString* returnTypeInfo /* = NULL */, QString* errorMessage /* = NULL */ ) {
  // Determine which magic file to use
  //----------------------------------
  QString magicFile = setMagicPath( error, errorMessage );

  if( magicFile.isEmpty() ) {
    return false;
  }

  // Set up magic
  //-------------
  struct magic_set* magic = NULL;
  int flags = 0;
  QString errMsg;

  magic = magicLoadMagic( magicFile, flags, errMsg );

  if( NULL == magic ) {
    if( NULL != error )
      *error = true;    
    if( NULL != errorMessage )
      *errorMessage = QString( "magicLoadMagic failed: %1" ).arg( errMsg );

    return false;
  }

  // Check the file type.
  //--------------------
  bool result;
  QString fileTypeInfo;

  bool magicOK = magicProcess( magic, fileName, fileTypeInfo, errMsg );

  if( NULL != returnTypeInfo )
    *returnTypeInfo = fileTypeInfo;

  if( magicOK ) {
    if( NULL != error )
      *error = false;

    result = ( 0 == QRegExp( "^(ASCII)[\\s]+[\\sA-Za-z]*(text)" ).indexIn( fileTypeInfo ) );

    //    result = (
    //      fileTypeInfo.startsWith( "ASCII text" )
    //      | fileTypeInfo.startsWith( "ASCII English text" )
    //      | fileTypeInfo.startsWith( "ASCII C program text" )
    //    );
  }
  else {
    if( NULL != error )
      *error = true;
    if( NULL != errorMessage )
      *errorMessage = QString( "magicProcess failed: %1" ).arg( errMsg );

    result = false;
  }

  magicCloseMagic( magic );

  return result;
}


magic_set* magicLoadMagic( QString magicFile, int flags, QString& errMsg ) {
  struct magic_set* magic = magic_open( flags );

  if( NULL == magic ) {
    errMsg = "Could not open magic.";
    return NULL;
  }
  else {
    if( -1 == magic_load( magic,  magicFile.toLatin1().data() ) ) {
      errMsg = "Could not load magic.";
      magic_close( magic );
      return NULL;
    }
    else {
      return magic;
    }
  }
}


bool magicProcess( struct magic_set* ms, QString fileName, QString& fileTypeInfo, QString& errMsg ) {
  const char* type = magic_file( ms, fileName.toLatin1().data() );

  if( NULL == type ) {
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
