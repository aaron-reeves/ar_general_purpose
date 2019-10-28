/*
cfilelist.h/cpp
Begin: 2003/06/11
-----------------
Copyright (C) 2003 - 2017 by Aaron Reeves
aaron.reeves@naadsm.org

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#include "cfilelist.h"

#include <QtCore>
#include <QtDebug>

#include <ar_general_purpose/arcommon.h>

// Set to true to enable debugging messages for this file
#define DEBUG true

QString formatFileName( QString filename ) {
  filename = filename.trimmed();
  filename.replace( '\\', '/' );
  filename.replace( QRegExp( "[/]+" ), QStringLiteral("/") );
  return filename;
}

QString formatDirName( QString dirname ) {
  dirname = formatFileName( dirname );

  if( !dirname.endsWith( '/' ) )
    dirname.append( '/' );

  return dirname;
}

CPathString::CPathString() : QString( ) {
  // Nothing to do here.
}

CPathString::CPathString( const QString& str ) : QString( str ) {
	// Nothing to do here.
}

CPathString::CPathString( const char* charstr ) : QString( charstr ) {
	// Nothing to do here.
}

CPathString CPathString::longFileName() const {
  return CPathString( *this );
}

CPathString CPathString::shortFileName() const {
  return CPathString( QFileInfo( *this ).fileName() );
}


QString CPathString::fileType() const {
  return QFileInfo( *this ).suffix();
}


QString CPathString::baseName() const {
  return QFileInfo( *this ).baseName();
}

QString CPathString::completeBaseName() const {
  return QFileInfo( *this ).completeBaseName();
}

int CPathString::nElements() {
  return formatFileName( *this ).split( '/', QString::SkipEmptyParts ).count();
}

QString CPathString::firstElement() {
  return formatFileName( *this ).split( '/', QString::SkipEmptyParts ).first();
}

QString CPathString::lastElement() {
  return formatFileName( *this ).split( '/', QString::SkipEmptyParts ).last();
}

QString CPathString::element( const int idx ) {
  return formatFileName( *this ).split( '/', QString::SkipEmptyParts ).at( idx );
}


CPathString CPathString::parent() {
  return pathTrimmedRight( 1 );
}


CPathString CPathString::pathTrimmedRight( const int nElementsToTrim ) {
  QString result;

  QStringList templ = formatFileName( *this ).split( '/', QString::SkipEmptyParts );
  if( nElementsToTrim > templ.count() ) {
    result = QString();
    qFatal( "Wrong number in CPathString::pathTrimmedRight()" );
  }
  else {
    for( int i = 0; i < nElementsToTrim; ++i )
      templ.removeLast();
    result = templ.join( '/' ).append( '/' );
  }

  return CPathString( result );
}


CPathString CPathString::pathTrimmedLeft( const int nElementsToTrim ) {
  QString result;

  QStringList templ = formatFileName( *this ).split( '/', QString::SkipEmptyParts );

  if( nElementsToTrim > templ.count() ) {
    result = QString();
    qFatal( "Wrong number in CPathString::pathTrimmedLeft()" );
  }
  else {
    for( int i = 0; i < nElementsToTrim; ++i )
      templ.removeFirst();
    result = templ.join( '/' );
  }

  return CPathString( result );
}


QDir CPathString::dir() const {
  return QFileInfo( *this ).dir();
}

CPathString CPathString::directory() const {
  return CPathString( dir().absolutePath().append( "/" ) );
}


QString CPathString::shortDirectory() const {
	CPathString path = CPathString( this->directory() );
	path.truncate( path.length() - 1 );

	QString dir = path.shortFileName();

	return dir;
}


QString CPathString::removeRoot( QString oldRoot ) {
  oldRoot = formatFileName( oldRoot );

  if( 0 == longFileName().indexOf( oldRoot.trimmed() ) ) {
    return longFileName().remove( 0, oldRoot.trimmed().length() + 1 );
  }
  else {
    return longFileName();
  }
}


QString CPathString::replaceRoot( QString oldRoot, QString newRoot ) {
  QString result;

  if( DEBUG ) qDb() << "--- CQPathString::replaceRoot";

  oldRoot = formatFileName( oldRoot );

  newRoot = newRoot.trimmed();
  if( '/' == newRoot.at( newRoot.length() - 1 ) ) {
    newRoot.truncate( newRoot.length() - 1 );
  }

  if( DEBUG ) qDb() << "oldRoot is" << oldRoot;
  if( DEBUG ) qDb() << "newRoot is" << newRoot;

  if( 0 == longFileName().indexOf( oldRoot ) ) {
    if( DEBUG ) qDb() << "oldRoot found and will be replaced.";
    result = removeRoot( oldRoot );
    result.prepend( "/" );
    result.prepend( newRoot );
    if( DEBUG ) qDb() << "replaceRoot will return" << result;
    return result;
  }
  else {
    if( DEBUG ) qDb() << "oldRoot not found.";
    return longFileName();
  }
}



CFileList::CFileList() :
  QList<CPathString>(),
  _recurse( false )
{
  // Nothing more to do here
}

CFileList::CFileList( const QString& path, const QString& filter, const bool recurse ) :
  QList<CPathString>(),
  _startingDir( path ),
  _recurse( recurse )
{
	getFileNames( path, filter, recurse );
}


CFileList::CFileList( const CFileList& other ) :
  QList<CPathString>( other ),
  _startingDir( other._startingDir ),
  _filter( other._filter ),
  _recurse( other._recurse ),
  _omittedDirs( other._omittedDirs )
{
  // Nothing more to do here
}


CFileList& CFileList::operator=( const CFileList& other ) {
  QList<CPathString>::operator=( other );

  _startingDir = other._startingDir;
  _filter = other._filter;
  _recurse = other._recurse;
  _omittedDirs = other._omittedDirs;

  return *this;
}


CFileList::~CFileList() {
  // Nothing to do here.
}


void CFileList::omitDir( const QString& dir ) {
  _omittedDirs.insert( dir );
}


void CFileList::getFileNames( const QString& dirName, const QString& filter, const bool recurse ) {
  QFileInfo finfo;
  QString completePath;
  QString str;
  CPathString listItem;
  QStringList tmp, filters;

  _filter = filter;
  _recurse = recurse;

  tmp = filter.split( ';', QString::SkipEmptyParts );
  foreach( const QString& str, tmp ) {
    filters.append( str.trimmed() );
  }

  bool dirIsOmitted = ( _omittedDirs.contains( dirName ) || _omittedDirs.contains( QFileInfo( dirName ).fileName() ) );
  if( dirIsOmitted ) {
    return;
  }

  for( int i = 0; i < filters.count(); ++i ) {
    if( filters.at(i).startsWith( QStringLiteral("*.") ) )
      filters[i] = filters.at(i).right( filters.at(i).length() - 1 );
  }

  QDir dir( dirName );
	dir.setFilter( QDir::Files | QDir::Dirs | QDir::Hidden );

  for( int i = 0; i < dir.entryList().count(); ++i ) {
    //qDb() << dir[i];

		// Skip the directories "." and ".."
    if( ( ".." == dir[i] ) || ( "." == dir[i] ) ) {
			continue;
		}
		else {
			completePath = dir.absolutePath() + "/" + dir[i];
      //qDb() << QString( "Complete path is %1" ).arg( completePath );
      finfo = QFileInfo( completePath );

      if( finfo.isDir() ) {
        if( recurse )
          getFileNames( finfo.filePath(), filter, recurse );
			}
			else {
        str = finfo.filePath().toLower();

        // Check for the wildcard that matches all files.
        if( filters.contains( QStringLiteral(".*") ) ) {
          listItem = CPathString( finfo.filePath() );
					this->append( listItem );
				}

        // If there is no wildcard, go through the filters one at a time to look for matches.
        else {
          for( int i = 0; i < filters.count(); ++i ) {
            if( str.endsWith( filters.at(i) ) ) {
              listItem = CPathString( finfo.filePath() );
              this->append( listItem );
              break;
            }
          }
        }

			}
		}
  }
	//qDebug( QString( "Done with  %1" ).arg( dirName ) );
}


bool CFileList::containsShortFileName( const QString& shortFileName ) const {
  bool result = false;

  #ifdef Q_OS_WIN
    Qt::CaseSensitivity sens = Qt::CaseInsensitive;
  #else
    Qt::CaseSensitivity sens = Qt::CaseSensitive;
  #endif

  foreach( const CPathString& str, *this ) {
    if( 0 == str.shortFileName().compare( shortFileName, sens ) ) {
      result = true;
      break;
    }
  }

  return result;
}


void CFileList::debug() const {
	CPathString strp;
	int i;
	int count = this->count();

  qDb() << "Starting debug....";
  qDb() << "Items in list:" << count;

	if( 0 < count ) {
    for( i = 0; i < count; ++i ) {
      strp = this->at(i);
      qDb() << strp.longFileName();
      //qDb() << "Short file name:" << strp.shortFileName();
		}
	}

  qDb() << "Done debugging!" << endl;
}


void CFileList::toStream( QTextStream* stream, const bool abbrevPath ) {
  for( int i = 0; i < this->count(); ++i ) {
    if( abbrevPath )
      *stream << abbreviatePath( this->at(i), 80 ) << endl;
    else
      *stream << this->at(i) << endl;
  }
}


void CFileList::removeFile( const QString &filename ) {
  CFileList toRemove;
  for( int i = 0; i < this->count(); ++i ) {
    if( this->at(i).endsWith( QStringLiteral("/%1").arg( filename ) ) || ( 0 == this->at(i).compare( filename ) ) ) {
      toRemove.append( this->at(i) );
    }
  }

  this->removeFiles( toRemove );
}


void CFileList::removeFiles( const CFileList &toRemove ) {
  for( int i = 0; i < toRemove.count(); ++i ) {
    this->removeAt( this->indexOf( toRemove.at(i) ) );
  }
}


void CFileList::removeDirectory( const QString& dirname ) {
  this->removeAt( this->indexOf( formatDirName( dirname ) ) );
}


void CFileList::removeFilesInDir( const QString& dir ) {
  CFileList toRemove;

  for( int i = 0; i < this->count(); ++i ) {
    QFileInfo fi( this->at(i) );
    if( fi.path().endsWith( QStringLiteral("/%1").arg( dir ) ) || ( 0 == fi.path().compare( dir ) ) ) {
      toRemove.getFileNames( fi.path(), this->_filter, this->_recurse );
    }
  }

  this->removeFiles( toRemove );
}


void CFileList::merge( const CFileList& subList ) {
  CPathString strp;
  int i;

  for( i = 0; i < subList.count(); ++i ) {
    strp = subList.at(i);
    this->append( strp );
  }
}


CFileList CFileList::directories() const {
  CFileList result;

  for( int i = 0; i < this->count(); ++i ) {
    QFileInfo fi( this->at(i) );
    QString dir;

    if( fi.isDir() )
      dir = this->at(i);
    else if( fi.isFile() )
      dir = this->at(i).directory();

   Q_ASSERT( !dir.isEmpty() );

    if( !result.contains( dir ) )
      result.append( dir );
  }

  return result;
}


CFileList CFileList::files() const {
  CFileList result;

  for( int i = 0; i < this->count(); ++i ) {
    QFileInfo fi( this->at(i) );
    if( fi.isFile() )
      result.append( this->at(i) );
  }

  return result;
}


QStringList CFileList::toStringList() const {
  QStringList result;

  for( int i = 0; i < this->count(); ++i ) {
    CPathString strp = this->at(i);
    strp.replace( QStringLiteral("//"), QStringLiteral("/") );
    strp.replace( QStringLiteral("\\\\"), QStringLiteral("\\") );
    result.append( strp );
  }

  return result;
}


void testFileList( const QString& directoryName, const QString& filter ) {
  qDb() << "--- No recursion:";
  CFileList listA( directoryName, filter, false );
  listA.debug();

  qDb() << endl << "--- With recursion:";
  CFileList listB = CFileList( directoryName, filter, true );
  listB.debug();

  qDb() << endl << "--- Directories only (from recursive list):";
  CFileList listC = listB.directories();
  listC.debug();

  qDb() << endl << "--- Files only (from recursive list):";
  CFileList listD = listB.files();
  listD.debug();

   qDb() << endl << "--- Copy constructor (original list without recursion):";
  CFileList listE( listA );
  listE.debug();

    qDb() << endl << "--- Assignment operator (directories only from recursive list):";
  CFileList listF = listC;
  listF.debug();
}
