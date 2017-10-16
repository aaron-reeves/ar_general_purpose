/*
cfilelist2.h/cpp
Begin: 2003/06/11
-----------------
Copyright (C) 2003 - 2006 by Aaron Reeves
aaron.reeves@naadsm.org

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#include "cfilelist2.h"

#include <qdebug.h>
#include <qdir.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qregexp.h>
#include <qstring.h>

// Set to true to enable debugging messages for this file
#define DEBUG true

CPathString::CPathString( void ) : QString( ) {
  // Nothing to do here.
}

CPathString::CPathString( const QString& str ) : QString( str ) {
	// Nothing to do here.
}

CPathString::CPathString( const char* charstr ) : QString( charstr ) {
	// Nothing to do here.
}

QString CPathString::longFileName() const {
	return *this;
}

QString CPathString::shortFileName() const {
  QFileInfo fi( *this );
  return fi.fileName();
}


QString CPathString::fileType() const {
  QFileInfo fi( *this );
  return fi.suffix();
}


QString CPathString::baseName() const {
  QFileInfo fi( *this );
  return fi.baseName();
}

QString CPathString::completeBaseName() const {
  QFileInfo fi( *this );
  return fi.completeBaseName();
}

QString CPathString::pathTrimmed( const int nPathsToTrim ) {
  QString result;

  QString temp = this->replace( '\\', '/' );
  QStringList templ = temp.split( '/' );

  if( nPathsToTrim > templ.count() ) {
    result = "";
    qFatal( "Wrong number in CPathString::pathTrimmed()" );
  }
  else {
    for( int i = 0; i < nPathsToTrim; ++i )
      templ.takeAt(0);
    result = templ.join( '/' );
  }

  return result;
}


QDir CPathString::dir() const {
  QFileInfo finfo( *this );
  return finfo.dir();
}

QString CPathString::directory() const {
  return dir().absolutePath().append( "/" );
}


QString CPathString::shortDirectory() const {
	CPathString path = CPathString( this->directory() );
	path.truncate( path.length() - 1 );

	QString dir = path.shortFileName();

	return dir;
}


QString CPathString::removeRoot( QString oldRoot ) {
  oldRoot = oldRoot.trimmed();
  oldRoot = oldRoot.replace( '\\', '/' );

  if( 0 == longFileName().indexOf( oldRoot.trimmed() ) ) {
    return longFileName().remove( 0, oldRoot.trimmed().length() + 1 );
  }
  else {
    return longFileName();
  }
}


QString CPathString::replaceRoot( QString oldRoot, QString newRoot ) {
  QString result;

  if( DEBUG ) qDebug() << "--- CQPathString::replaceRoot";

  oldRoot = oldRoot.trimmed();
  oldRoot = oldRoot.replace( '\\', '/' );

  newRoot = newRoot.trimmed();
  if( '/' == newRoot.at( newRoot.length() - 1 ) ) {
    newRoot.truncate( newRoot.length() - 1 );
  }

  if( DEBUG ) qDebug() << "oldRoot is" << oldRoot;
  if( DEBUG ) qDebug() << "newRoot is" << newRoot;

  if( 0 == longFileName().indexOf( oldRoot ) ) {
    if( DEBUG ) qDebug() << "oldRoot found and will be replaced.";
    result = removeRoot( oldRoot );
    result.prepend( "/" );
    result.prepend( newRoot );
    if( DEBUG ) qDebug() << "replaceRoot will return" << result;
    return result;
  }
  else {
    if( DEBUG ) qDebug() << "oldRoot not found.";
    return longFileName();
  }
}



CFileList::CFileList() : QList<CPathString>() {
	_startingDir = "";

}

CFileList::CFileList( const QString& path, const QString& filter, const bool recurse ) : QList<CPathString>() {
	//qDebug( "Constructor called" );
	_startingDir = path;
  _dirList = new CFileList( false );
  _fileList = new CFileList( false );
	getFileNames( path, filter, recurse );
	//qDebug( "Done with CQFileList::CQFileList" );
}


CFileList::CFileList( const CFileList& other ) : QList<CPathString>( other ) {
  _startingDir = other._startingDir;

  if( NULL != other._dirList )
    _dirList = new CFileList( other._dirList );
  else
    _dirList = NULL;

  if( NULL != other._fileList )
    _fileList = new CFileList( other._fileList );
  else
    _fileList = NULL;
}

CFileList::CFileList( bool createDirList ) {
  Q_UNUSED( createDirList );

  _dirList = NULL;
  _fileList = NULL;
}


CFileList& CFileList::operator=( const CFileList& other ) {
  QList<CPathString>::operator=( other );

  _startingDir = other._startingDir;

  if( NULL != other._dirList )
    _dirList = new CFileList( other._dirList );
  else
    _dirList = NULL;

  if( NULL != other._fileList )
    _fileList = new CFileList( other._fileList );
  else
    _fileList = NULL;

  return *this;
}


CFileList::~CFileList() {
  if( NULL != _dirList ) {
    _dirList->clear();
    delete _dirList;
  }

  if( NULL != _fileList ) {
    _fileList->clear();
    delete _fileList;
  }
}


void CFileList::getFileNames( const QString& dirName, const QString& filter, const bool recurse ) {
  QFileInfo finfo;
  unsigned int i;
  QString completePath;
  QString str;
  CPathString listItem;
  QStringList filters;

  filters = filter.split( ';', QString::SkipEmptyParts );

  for( int i = 0; i < filters.count(); ++i ) {
    if( filters.at(i).startsWith( "*." ) )
      filters[i] = filters.at(i).right( filters.at(i).length() - 1 );
  }

	QDir dir( dirName );
	dir.setFilter( QDir::Files | QDir::Dirs | QDir::Hidden );

	for( i = 0; i < dir.count(); ++i ) {
    //qDebug() << dir[i];

		// Skip the directories "." and ".."
		if( ( ".." == dir[i] ) || ( "." == dir[i] ) ) {
			continue;
		}
		else {
			completePath = dir.absolutePath() + "/" + dir[i];
      //qDebug() << QString( "Complete path is %1" ).arg( completePath );
      finfo = QFileInfo( completePath );

      if( finfo.isDir() ) {
        if( NULL != _dirList )
          _dirList->append( CPathString( finfo.filePath() ) );
        if( recurse )
          getFileNames( finfo.filePath(), filter, recurse );
			}
			else {
        str = finfo.filePath().toLower();

        // Check for the wildcard that matches all files.
        if( filters.contains( ".*" ) ) {
          listItem = CPathString( finfo.filePath() );
          if( NULL != _fileList )
            _fileList->append( listItem );
					this->append( listItem );
				}

        // If there is no wildcard, go through the filters one at a time to look for matches.
        else {
          for( int i = 0; i < filters.count(); ++i ) {
            if( str.endsWith( filters.at(i) ) ) {
              listItem = CPathString( finfo.filePath() );
              if( NULL != _fileList )
                _fileList->append( listItem );
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


void CFileList::debugList() {
	CPathString strp;
	int i;
	int count = this->count();

	qDebug() << "Starting debug....";
	qDebug() << "Items in list:" << count;

	if( 0 < count ) {
    for( i = 0; i < count; ++i ) {
      strp = this->at(i);
			qDebug() << strp.longFileName();
      //qDebug() << "Short file name:" << strp.shortFileName();
		}
	}

  qDebug() << "Done debugging!" << endl;
}


void CFileList::merge( CFileList subList ) {
  CPathString strp;
  int i;

  for( i = 0; i < subList.count(); ++i ) {
    strp = subList.at(i);
    this->append( strp );
  }
}


QStringList CFileList::qStringList() const {
  QStringList sl;
  CPathString strp;
  int i;

  for( i = 0; i < this->count(); ++i ) {
    strp = this->at(i);
    strp.replace( "//", "/" );
    strp.replace( "\\\\", "\\" );
    sl.append( strp );
  }
  return sl;
}

void CFileList::clear() {
  if( NULL != _dirList )
    _dirList->clear();

  if( NULL != _fileList )
    _fileList->clear();

  QList<CPathString>::clear();
}


CFileList CFileList::directories() const {
  CFileList result;
  if( NULL != _dirList )
    result.merge( *_dirList );
  return result;
}


CFileList CFileList::files() const {
  CFileList result;
  if( NULL != _fileList )
    result.merge( *_fileList );
  return result;
}


void testFileList( const QString& directoryName, const QString& filter ) {
  qDebug() << "--- No recursion:";
  CFileList listA( directoryName, filter, false );
  listA.debug();

  qDebug() << endl << "--- With recursion:";
  CFileList listB = CFileList( directoryName, filter, true );
  listB.debug();

  CFileList listC = listA.directories();
  listC.debug();

  CFileList listD = listA.files();
  listD.debug();
}
