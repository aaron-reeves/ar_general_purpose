/*
filemagic.h/cpp
---------------
Begin: 2015-07-09
Author: Aaron Reeves <aaron.reeves@naadsm.org>
---------------------------------------------------
Copyright (C) 2015 - 2019 Aaron Reeves

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

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

  QString myLibPath;
  #ifdef MINGW64
    myLibPath = QStringLiteral("C:/libs/C_libs-x64/bin/magic");
  #else
    myLibPath = QStringLiteral("C:/libs/C_libs/bin/magic");
  #endif

  if( QFile::exists( QStringLiteral( "%1/magic" ).arg( QCoreApplication::applicationDirPath() ) ) )
    magicFile = QStringLiteral( "%1/magic" ).arg( QCoreApplication::applicationDirPath() );
  else if( QFile::exists( myLibPath ) )
    magicFile = myLibPath;
  else if( QFile::exists( QStringLiteral("/usr/share/file/magic") ) )
    magicFile = QStringLiteral("/usr/share/file/magic");
  else if( QFile::exists( QStringLiteral("/etc/magic") ) )
    magicFile = QStringLiteral("/etc/magic");
  else {
    if( nullptr != error )
      *error = true;
    if( nullptr != errorMessage )
      *errorMessage = QStringLiteral("Could not find magic file.");
  }

  return magicFile;
}


QString magicFileTypeInfo( const QString& fileName, bool* error /* = nullptr */, QString* errorMessage /* = nullptr */ ) {
  // Determine which magic file to use
  //----------------------------------
  QString magicFile = setMagicPath( error, errorMessage );

  if( magicFile.isEmpty() ) {
    if( nullptr != error )
      *error = true;
    if( nullptr != errorMessage )
      *errorMessage = QStringLiteral( "magicFile is empty" );

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
      *errorMessage = QStringLiteral( "magicLoadMagic failed: %1" ).arg( errMsg );

    return QString();
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
      *errorMessage = QStringLiteral( "magicProcess failed: %1" ).arg( errMsg );

    return QString();
  }

  magicCloseMagic( magic );

  return result;
}


bool magicStringShowsAsciiTextFile( const QString& fileTypeInfo ) {
  bool result = ( 0 == QRegExp( "^(ASCII)[\\s]+[\\sA-Za-z]*(text)" ).indexIn( fileTypeInfo ) );
  //    bool result = (
  //      fileTypeInfo.startsWith( "ASCII text" )
  //      || fileTypeInfo.startsWith( "ASCII English text" )
  //      || fileTypeInfo.startsWith( "ASCII C program text" )
  //    );

  return result;
}


bool magicStringShowsXlsxFile( const QString& fileTypeInfo, const QString& fileName ) {
  bool result = (
    ( fileTypeInfo.startsWith( QLatin1String("Zip archive data") ) && fileName.endsWith( QLatin1String(".xlsx"), Qt::CaseInsensitive ) )
    || ( 0 == fileTypeInfo.compare( QLatin1String("Microsoft Excel 2007+") ) )
    || ( fileTypeInfo.contains( QLatin1String("Microsoft OOXML") ) && fileName.endsWith( QLatin1String(".xlsx"), Qt::CaseInsensitive ) )
    || ( fileTypeInfo.contains( QLatin1String("Microsoft OOXML") ) && fileName.endsWith( QLatin1String(".xls"), Qt::CaseInsensitive ) )
  );

  return result;
}


bool magicStringShowsXlsFile( const QString& fileTypeInfo ) {
  bool result = (
    fileTypeInfo.contains( QLatin1String("Composite Document File V2 Document") )
    || fileTypeInfo.contains( QLatin1String("CDF V2 Document") )
  );

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
      *errorMessage = QStringLiteral( "magicLoadMagic failed: %1" ).arg( errMsg );

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
          result = magicStringShowsAsciiTextFile( fileTypeInfo );
        break;
      case CHECKXLSX:
          result = magicStringShowsXlsxFile( fileTypeInfo, fileName );
        break;
      case CHECKXLS:
        result = magicStringShowsXlsFile( fileTypeInfo );
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
      *errorMessage = QStringLiteral( "magicProcess failed: %1" ).arg( errMsg );

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


magic_set* magicLoadMagic( const QString& magicFile, const int flags, QString& errMsg ) {
  struct magic_set* magic = magic_open( flags );

  if( nullptr == magic ) {
    errMsg = QStringLiteral("Could not open magic.");
    return nullptr;
  }
  else {
    if( -1 == magic_load( magic,  magicFile.toLatin1().data() ) ) {
      errMsg = QStringLiteral("Could not load magic.");
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
    errMsg = QStringLiteral( "%1" ).arg( magic_error( ms ) );
    fileTypeInfo = QString();
    return false;
  }
  else {
    fileTypeInfo = QStringLiteral( "%1" ).arg( type );

    if( fileTypeInfo.startsWith( QStringLiteral("cannot open") ) ) {
      errMsg = fileTypeInfo;
      fileTypeInfo = QString();
      return false;
    }
    else {
      errMsg =  QString();
      return true;
    }
  }
}


void magicCloseMagic( struct magic_set* ms ) {
  magic_close( ms );
}
