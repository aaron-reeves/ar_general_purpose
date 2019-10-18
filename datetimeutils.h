/*
datetimeutils.h/cpp
-------------------
Begin: 2019-06-18
Author: Aaron Reeves <aaron.reeves@sruc.ac.uk>
---------------------------------------------------
Copyright (C) 2019 Epidemiology Research Unit, Scotland's Rural College (SRUC)

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#ifndef DATETIMEUTILS_H
#define DATETIMEUTILS_H

#include <QtCore>

// Format: hh:mm:ss(.zzz)
QString elapsedTimeToString( const qint64 msec, const bool includeMSecs = false );


#endif // DATETIMEUTILS_H
