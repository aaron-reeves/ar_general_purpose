/*
ctimegp.cpp
-----------
Begin: 2004/10/03
Last revision: $Date: 2011-10-25 04:57:18 $ $Author: areeves $
Version number: $Revision: 1.3 $
Project: Atriplex Distributed Computing System
Website: 
Author: Aaron Reeves <aaron@aaronreeves.com>
--------------------------------------------------
Copyright (C) 2003 - 2007 Aaron Reeves

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#ifndef CTIMEGP_H
#define CTIMEGP_H

#include <qstring.h>
#include <qdatetime.h>

class CTimeGP {
  public:
    // In Qt4, these functions are no longer necessary.
    //static QTime timeFromString( const QString & s );
    //static QDate dateFromString( const QString & s );
    //static QDateTime dateTimeFromString( const QString & s );
    //static uint toTime_t( QDateTime dt );
    
    static float elapsedTimeHrs( QDateTime time1, QDateTime time2 );
};

#endif // CTIMEGP_H
