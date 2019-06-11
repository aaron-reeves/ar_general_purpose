#include "debugutils.h"

void debugArray( const CTwoDArray<QVariant>& array ) {
  qDb() << "******************** BEGIN ARRAY";

  for( int r = 0; r < array.nRows(); ++r ) {
    QString result;

    for( int c = 0; c < array.nCols(); ++c ) {
      result.append( QString( "%1 " ).arg( array.at( c, r ).toString() ) );
    }
    result = result.left( result.length() - 1 );

    qDb() << result;
  }

  qDb() << "******************** END ARRAY";
}