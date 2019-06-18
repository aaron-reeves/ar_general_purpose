// Begin 2019-06-18

#include "datetimeutils.h"
#include "strutils.h"

struct STimeBits {
  int hour;
  int minute;
  int second;
  int msec;
};

STimeBits timeBits( const qint64& ms ) {
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

  if( includeMSecs ) {
    return
      QString( "%1:%2:%3.%4" )
        .arg( tb.hour )
        .arg( paddedInt( tb.minute, 2 ) )
        .arg( paddedInt( tb.second, 2 )
        .arg( rightPaddedStr( paddedInt( tb.msec, 3 ), 3, '0' ) ) )
    ;
  }
  else
    return QString( "%1:%2:%3" ).arg( tb.hour ).arg( paddedInt( tb.minute, 2 ) ).arg( paddedInt( tb.second, 2 ) );
}
