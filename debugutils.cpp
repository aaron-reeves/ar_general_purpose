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
