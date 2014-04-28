/*
ctimegp.h/cpp
-------------
Begin: 2004/10/03
Author: Aaron Reeves <development@reevesdigital.com>
--------------------------------------------------
Copyright (C) 2004 - 2007 Aaron Reeves

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/


#include "ctimegp.h"

#include <time.h>
#include <qglobal.h>


#ifdef THESE_ARE_UNNECESSARY_IN_QT4
QTime CTimeGP::timeFromString( const QString &s ) {
  /*
  //qDebug( QString( "Parsing time from string %1" ).arg( s ) );
  int sep = s.find( ':' );
  int hour( s.mid( 0, sep ).toInt() );
  int minute( s.mid( sep + 1, 2 ).toInt() );
  int second( s.mid( sep + 4, 2 ).toInt() );
  QString ap( s.mid( sep + 7, 2 ) );
  if( ap.lower() == "pm" && hour != 12 )
    hour = hour + 12;
  if( ap.lower() == "am" && hour == 12 )
    hour = hour - 12;
  return QTime( hour, minute, second, 0 );
  */
  
  return QTime::fromString( s, Qt::ISODate );
}


QDate CTimeGP::dateFromString( const QString &s ) {
  /*
  //qDebug( QString( "Parsing date from string %1" ).arg( s ) ) ;
  int sep = s.find( '-' );
  int year( s.mid( 0, sep ).toInt() );
  QString md( s.mid( sep + 1 ) );
  sep = md.find( '-' );
  int month( md.mid( 0, sep ).toInt() );
  int day( md.mid( sep + 1 ).toInt() );
  return QDate( year, month, day );
  */
  
  return QDate::fromString( s, Qt::ISODate );
}


QDateTime CTimeGP::dateTimeFromString( const QString &s ) {
  /*
  //qDebug( QString( "Datetime is: %1" ).arg( s ) );
  int sep = s.find( ' ' );
  QString ds( s.left( sep ) );
  QString ts( s.mid( sep + 1 ) );
  QDate d = dateFromString( ds );
  QTime t = timeFromString( ts );
  return QDateTime( d, t );
  */
   
  return QDateTime::fromString( s, Qt::ISODate );
}


// Code for this function largely borrowed from Qt 3.x (GPL)
uint CTimeGP::toTime_t( QDateTime dt ) {
  /*
  QDate d = dt.date();
  QTime t = dt.time();

  tm brokenDown;  // tm is defined in <time.h>

  brokenDown.tm_sec = t.second();
  brokenDown.tm_min = t.minute();
  brokenDown.tm_hour = t.hour();
  brokenDown.tm_mday = d.day();
  brokenDown.tm_mon = d.month() - 1;
  brokenDown.tm_year = d.year() - 1900;
  brokenDown.tm_isdst = -1;
  int secsSince1Jan1970UTC = ( int ) mktime( &brokenDown );
  if( secsSince1Jan1970UTC < -1 )
    secsSince1Jan1970UTC = -1;
  return ( uint ) secsSince1Jan1970UTC;
  */
  
  return dt.toTime_t();
}
#endif //THESE_ARE_UNNECESSARY_IN_QT4


float CTimeGP::elapsedTimeHrs( QDateTime time1, QDateTime time2 ) {
  // Seconds since 1/1/70
  quint64 t1 = time1.toTime_t();
  quint64 t2 = time2.toTime_t();

  quint64 dif = t2 - t1;

  quint64 sec, min;
  float hr;

  // How many minutes?
  min = int ( ( double ( dif ) / 60.0 ) );
  sec = dif % 60; // Seconds are ignored.

  // How many (fractional) hours?
  hr = float ( min ) / 60.0;

  return hr;
}
