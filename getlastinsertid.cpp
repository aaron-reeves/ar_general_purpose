#include "getlastinsertid.h"

int getLastInsertId( const QSqlDatabase* db, const QString& sequenceName ) {
  QSqlQuery q( *db );

  if( q.exec( QStringLiteral( "SELECT last_value FROM %1" ).arg( sequenceName ) ) ) {
    q.next();
    return q.value(0).toInt();
  }
  else {
    return 0;
  }
}
