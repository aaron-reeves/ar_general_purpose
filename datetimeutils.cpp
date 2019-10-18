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

#include "datetimeutils.h"
#include "strutils.h"

struct STimeBits {
  int hour;
  int minute;
  int second;
  int msec;
};

STimeBits timeBits( const qint64 ms ) {
  STimeBits result;

  int hours = int( trunc( ms / 1000.0 / 60.0 / 60.0 ) );
  qint64 remainder = ms - ( hours * 60 * 60 * 1000 );

  //qDebug() << "Hours:" << hours;
  //qDebug() << "Remainder after hours:" << remainder;

  int minutes = int( trunc( remainder / 60.0 / 1000.0 ) );
  remainder = remainder - ( minutes * 60 * 1000 );

  //qDebug() << "Minutes:" << minutes;
  //qDebug() << "Remainder after minutes:" << remainder;

  int seconds = int( trunc( remainder / 1000.0 ) );

  //qDebug() << "Seconds:" << seconds;
  //qDebug() << "Remainder after seconds:" << remainder - ( seconds * 1000 );

  int msec = int( remainder - ( seconds * 1000 ) );

  result.hour = hours;
  result.minute = minutes;
  result.second = seconds;
  result.msec = msec;

  return result;
}


// Format: hh:mm:ss(.zzz)
QString elapsedTimeToString( const qint64 msec, const bool includeMSecs /* = false */ ) {
  STimeBits tb = timeBits( msec );

  if( includeMSecs )
    return QStringLiteral( "%1:%2:%3.%4" ).arg( QString::number( tb.hour ), paddedInt( tb.minute, 2 ), paddedInt( tb.second, 2 ), rightPaddedStr( paddedInt( tb.msec, 3 ), 3, '0' ) );
  else
    return QStringLiteral( "%1:%2:%3" ).arg( QString::number( tb.hour ), paddedInt( tb.minute, 2 ) , paddedInt( tb.second, 2 ) );
}
