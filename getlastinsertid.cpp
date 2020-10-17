/*
getlastinsertid.h/cpp
---------------------
Begin: 2014-06-30
Author: Aaron Reeves <aaron.reeves@naadsm.org>
---------------------------------------------------
Copyright (C) 2016 Aaron Reeves

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#include "getlastinsertid.h"

int getLastInsertId( const QSqlDatabase* db, const char* sequenceName ) {
  return getLastInsertId( db, QString( sequenceName ) );
}


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
