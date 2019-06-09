#ifndef DEBUGUTILS_H
#define DEBUGUTILS_H

#include <QtCore>
#include <QDebug>

#include <ar_general_purpose/arcommon.h>
#include <ar_general_purpose/ctwodarray.h>

void debugArray( const CTwoDArray<QVariant>& array );

#endif // DEBUGUTILS_H
