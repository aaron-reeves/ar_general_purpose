/*
qcout.h
-------
Begin: 2007/03/15
Last revision: $Date: 2011-11-24 04:26:20 $ $Author: areeves $
Version number: $Revision: 1.6 $
Project: Atriplex Distributed Computing System
Website: 
Author: Aaron Reeves <aaron@aaronreeves.com>
--------------------------------------------------
Copyright (C) 2007 - 2008 Aaron Reeves

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#ifndef _QCOUT_H_
#define _QCOUT_H_

//#include <QtCore/qtextstream.h>

#include <QtCore/QTextStream>
#include <QtCore/QString>

extern QTextStream cout; //(stdout, QIODevice::WriteOnly);
extern QTextStream cin;

void cerr( const QString& msg, const bool silent );
void cerr( const char* msg, const bool silent );

#endif // _QCOUT_H_
