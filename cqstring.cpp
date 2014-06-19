/*
cqstring.h/cpp
--------------
Begin: 2003/04/09
Author: Aaron Reeves <aaron.reeves@naadsm.org>
--------------------------------------------------
Copyright (C) 2003 - 2007 Aaron Reeves

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#include "cqstring.h"

#include <qglobal.h>
#include <qstringlist.h>

#include <stddef.h>

CQString::CQString( void ):QString() {
  // do nothing else
}


CQString::CQString( QString str ):QString( str ) {
  // do nothing else
}


CQString::CQString( const char *charString ):QString( charString ) {
  // do nothing else
}

#ifdef UNDEFINED
#ifdef  _WIN32  // MFC
CQString::CQString( CString cstr ):QString( ( const char * ) cstr ) {
  // do nothing else
}


CString CQString::toCString( void ) {
  // FIX ME: this approach probably screws up unicode support: check it out in more detail at some point.
  return CString( this->latin1() );
}
#endif
#endif

CQString CQString::value( void ) {
  return *this;
}


CQString CQString::toString( void ) {
  return this->value();
}


int CQString::getInt( QString splitter ) {
  // Split the string at the splitter
  QStringList split = this->split( splitter );

  // Convert whatever follows the splitter to an integer and return it
  int i = split[1].toInt();
  return i;
}


double CQString::getDouble( QString splitter ) {
  // Split the string at the splitter
  QStringList split = this->split( splitter );

  // Convert whatever follows the splitter to a double and return it
  double d = split[1].toDouble();
  return d;
}
