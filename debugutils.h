/*
debugutils.h/cpp
----------------
Begin: 2019-05-11
Author: Aaron Reeves <aaron.reeves@sruc.ac.uk>
---------------------------------------------------
Copyright (C) 2019 Scotland's Rural College (SRUC)

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#ifndef DEBUGUTILS_H
#define DEBUGUTILS_H

#include <QtCore>
#include <QDebug>

#include <ar_general_purpose/arcommon.h>
#include <ar_general_purpose/ctwodarray.h>

void debugArray( const CTwoDArray<QVariant>& array );

#endif // DEBUGUTILS_H
