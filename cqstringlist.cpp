/*
cqstringlist.cpp
----------------
Begin: 2003/04/09
Last revision: $Date: 2011-10-25 04:57:17 $ $Author: areeves $
Version: $Revision: 1.7 $
Project: JetSQLConsole
Website: http://www.aaronreeves.com/jetsqlconsole
Author: Aaron Reeves <aaron@aaronreeves.com>
--------------------------------------------------
Copyright (C) 2003 - 2008 Aaron Reeves

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#include "cqstringlist.h"

#include <QDebug>

CQStringList::CQStringList() {
  _isAutoDelete = false;
}


CQStringList::~CQStringList() {
  if( _isAutoDelete ) {
    while (!this->isEmpty()) {
      delete this->takeFirst();
    }
  }
}


void CQStringList::setAutoDelete( bool val ) {
  _isAutoDelete = val;
}


/**
@fn bool CQStringList::contains( const QString str )
This function is used to determine whether the list contains the specified string.

@param str the QString that the list will be searched for
@return true if str is found in the list, false if it is not.
*/
bool CQStringList::contains( const QString str ) {
    QString* member;
    bool match = false;
    int i;

    qDebug() << this->count();
    for( i = 0; i < this->count(); ++i ) {
      member = this->at(i);
      qDebug() << i << *member << str;
      
      if( 0 == member->compare( QString( str ) /*, Qt::CaseSensitive */ ) ) { // AR 7/20/11: Commented out due to compile problems.
          match = true;
          break;
      }
    }

    return match;
}


/**
@fn void CQStringList::explode( QString str, QChar splitter )

Splits a string (str) into pieces and adds the pieces to this string list.  The character
'splitter' indicates where the string should be split.

@param str the string to explode
@param splitter the character indicating where to break the string
*/
void CQStringList::explode( QString str, QChar splitter ) {
  QString* s = NULL;
  int unsigned i;
  int unsigned len =  str.length();
  QChar c = QChar();
  QChar prevc = QChar();
  bool inQuotes = false;
  s = new QString();

  for( i = 0; i < len; ++i ) {
    c = str.at( i );

    if( ( c == '\"' && inQuotes ) || ( c == '\'' && inQuotes && prevc != '\\' ) ) {
      inQuotes = false;
    }
    else if( ( c == '\"' && !inQuotes )  || ( c == '\'' && !inQuotes && prevc !='\\' ) ) {
      inQuotes = true;
    }

    if( c == splitter && prevc != '\\' && !( inQuotes ) ) {
      this->append( s );
      s = new QString();
    }
    else {
      s->append( c );
    }
    prevc = c;
  }

  if( s->length() > 0 ) {
    this->append( s );
  }
  else {
    delete s;
  }
}


/**
@fn void CQStringList::debug( void )

Prints the contents of this list to the debug console.
*/
void CQStringList::debug( void ) {
  QString* str;
  int i;

  for( i = 0; i < this->count(); ++i ) {
    str = this->at( i );
    qDebug() << *str;
  }
}



