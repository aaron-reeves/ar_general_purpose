/*
cfilelist.h/cpp
Begin: 2003/06/11
-----------------
Copyright (C) 2003 - 2006 by Aaron Reeves
aaron.reeves@naadsm.org

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#include "cfilelist.h"

#include <qdebug.h>
#include <qdir.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qregexp.h>
#include <qstring.h>

// Set to true to enable debugging messages for this file
#define DEBUG true

CPathString::CPathString( QString str ) : QString( str ) {
	// Nothing actually has to happen in here
}


CPathString::CPathString( char* charstr ) : QString( charstr ) {
	// Nothing actually has to happen in here
}


QString CPathString::longFileName( void ) {
	return *this;
}

QString CPathString::shortFileName( void ) {
  int xs;
  QString xstr, xstr1;

  xs = 0;
  xstr1 = "";

  while( (xstr1 != "/") && (xstr1 != "\\") && (xs != this->length() + 1) ) {
		++xs;
		xstr = this->right( xs );
		xstr1 = xstr.left(1);
  }

  return( this->right( xs - 1) );
}


QString CPathString::fileType( void ) {
  return this->mid( this->lastIndexOf( '.' ) + 1 );
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


QString CPathString::directory( void ) {
	QFileInfo* finfo;
	QString sfn;
	QString result;

	finfo = new QFileInfo( this->longFileName() );

	if( finfo->isDir() ) {
		result = this->longFileName();
	}
	else {
		sfn = this->shortFileName();
		result = this->left( this->length() - sfn.length() );
	}

	delete finfo;
	return result;
}


QString CPathString::shortDirectory( void ) {
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



CFileList::CFileList( void ) {
	_startingDir = "";
	_dirList = new CFileList( false );
	_fileList = new CFileList( false );
}

CFileList::CFileList( QString path, QString filter, bool recurse ) {
	//qDebug( "Constructor called" );
	_startingDir = path;
	_dirList = new CFileList( false );
	_fileList = new CFileList( false );
	getFileNames( path, filter, recurse );
	//qDebug( "Done with CQFileList::CQFileList" );
}


CFileList::CFileList( bool createDirList ) {
  if( createDirList ) {
    // Do nothing.  This is essentially a dummy parameter for now.
    // This block just avoids a compiler warning.
  }
	_dirList = NULL;
	_fileList = NULL;
}


CFileList::~CFileList( void ) {
  if( NULL != _dirList ) {
    _dirList->clear( true );
    delete _dirList;
  }
  if( NULL != _fileList ) {
    _fileList->clear( true );
    delete _fileList;
  }
}


void CFileList::getFileNames( QString dirName, QString filter, bool recurse ) {
  QDir* dir;
  QFileInfo* finfo;
  unsigned int i;
  QString completePath;
  int j;
  QString str;
  CPathString* listItem;

	dir = new QDir( dirName );
	dir->setFilter( QDir::Files | QDir::Dirs | QDir::Hidden );

	/*
	// Debugging code
	for( i = 0; i < dir->count(); ++i ) {
    completePath = dir->absPath() + "/" + (*dir)[i];
		//qDebug( QString( "Complete path is %1" ).arg( completePath ) );
    finfo = new QFileInfo( completePath );
		qDebug( finfo->filePath() );
		delete finfo;
	}
	return;
	*/

	for( i = 0; i < dir->count(); ++i ) {
		// Skip the directories "." and ".."
		if( ( ".." == (*dir)[i] ) || ( "." == (*dir)[i] ) ) {
			continue;
		}
		else {
			completePath = dir->absolutePath() + "/" + (*dir)[i];
			//qDebug( QString( "Complete path is %1" ).arg( completePath ) );
			finfo = new QFileInfo( completePath );

			if( finfo->isDir() && recurse ) {
				if( NULL != _dirList ) _dirList->append( new CPathString( finfo->filePath() ) );
				getFileNames( finfo->filePath(), filter, recurse );
			}
			else {
				str = finfo->fileName();
				j = str.indexOf( QRegExp( filter, Qt::CaseInsensitive, QRegExp::Wildcard ), 0 );

				if ( j > -1 ) { // match found.
					// Add item to list
					//qDebug( QString( "Adding file %1 to the list." ).arg( finfo->filePath() ) );
					listItem = new CPathString( finfo->filePath() );
					if( NULL != _fileList) _fileList->append( listItem );
					this->append( listItem );
				}
			}
			delete finfo;
		}
  }
	//qDebug( QString( "Done with  %1" ).arg( dirName ) );
}


void CFileList::debugList( void ) {
	CPathString* strp;
	int i;
	int count = this->count();

	qDebug() << "Starting debug....";
	qDebug() << "Items in list:" << count;

	if( 0 < count ) {
    for( i = 0; i < count; ++i ) {
      strp = this->at(i);
		//for( strp = this->first(); strp != NULL; strp = this->next() ) {
			qDebug() << strp->longFileName();
			qDebug() << "Short file name:" << strp->shortFileName();
		}
	}

	qDebug() << "Done debugging!";
}


void CFileList::merge( CFileList subList ) {
  CPathString* strp;
  int i;

  for( i = 0; i < subList.count(); ++i ) {
  //for( strp = subList.first(); strp != NULL; strp = subList.next() ) {
    strp = subList.at(i);
    this->append( strp );
  }
}


QStringList CFileList::qStringList( void ) {
  QStringList sl;
  CPathString* strp;
  int i;

  for( i = 0; i < this->count(); ++i ) {
  //for ( strp = this->first(); strp != NULL; strp = this->next() ) {
    strp = this->at(i);
    sl.append( *strp );
  }
  return sl;
}


void CFileList::clear( bool deleteElements ) {
  int i;
  CPathString* ps;
  for( i = 0; i < this->count(); ++i ) {
    ps = this->at(i);
    if( deleteElements )
      delete ps;
    this->removeAt(i);
  }
}


CFileList* CFileList::directories( void ) {
	return _dirList;
}


CFileList* CFileList::files( void ) {
	return _fileList;
}
