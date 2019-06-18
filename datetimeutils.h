// Begin 2019-06-18

#ifndef DATETIMEUTILS_H
#define DATETIMEUTILS_H

#include <QtCore>

// Format: hh:mm:ss(.zzz)
QString elapsedTimeToString( const qint64 msec, const bool includeMSecs = false );


#endif // DATETIMEUTILS_H
