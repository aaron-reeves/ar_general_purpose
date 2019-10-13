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

#include "debugutils.h"

void debugArray( const CTwoDArray<QVariant>& array ) {
  qDb() << "******************** BEGIN ARRAY with size" << array.nCols() << "x" << array.nRows();

  if( array.hasColNames() )
    qDb() << array.colNames();

  for( int r = 0; r < array.nRows(); ++r ) {
    QString result;

    for( int c = 0; c < array.nCols(); ++c ) {
      result.append( QStringLiteral( "%1 " ).arg( array.at( c, r ).toString() ) );
    }
    result = result.left( result.length() - 1 );

    qDb() << result;
  }

  qDb() << "******************** END ARRAY";
}
