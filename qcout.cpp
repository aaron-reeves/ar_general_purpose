/*
qcout.h/cpp
-----------
Begin: 2007/03/15
Author: Aaron Reeves <development@reevesdigital.com>
--------------------------------------------------
Copyright (C) 2007 - 2008 Aaron Reeves

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#include "qcout.h"

QTextStream cout( stdout, QIODevice::WriteOnly );
QTextStream cin( stdin,  QIODevice::ReadOnly );

void cerr( const QString& msg, const bool silent ) {
  if( !silent )
    cout << msg << endl << flush;
}


void cerr( const char* msg, const bool silent ) {
  if( !silent )
    cout << msg << endl << flush;
}
