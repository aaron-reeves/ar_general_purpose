/*
cconcurrentcontainer.h/cpp
--------------------------
Begin: 2020-04-24
Author: Aaron Reeves <aaron.reeves@sruc.ac.uk>
---------------------------------------------------------
Copyright (C) 2020 - 2021 Scotland's Rural College (SRUC)

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#include "cconcurrentcontainer.h"

#include <ar_general_purpose/returncodes.h>

//-------------------------------------------------------------------------------------------------
// CConcurrentContainer - the base class
//-------------------------------------------------------------------------------------------------
  QHash<QString, int> CConcurrentContainer::resultsTemplate() const {
    QHash<QString, int> result;

    result.insert( QStringLiteral("returnCode"), ReturnCode::SUCCESS );
    result.insert( QStringLiteral("totalRecords"), 0 );
    result.insert( QStringLiteral("totalProcessed"), 0 );
    result.insert( QStringLiteral("insertFailures"), 0 );

    return result;
  }
  
  
  QHash<QString, int> CConcurrentContainer::mergeResults( const QHash<QString, int>& results1, const QHash<QString, int>& results2 ) const {
    QHash<QString, int> results;

    QList<QString> keys = results1.keys();

    foreach( QString key, keys ) {
      if( "returnCode" == key ) {
        results.insert( key, ( results1.value( key ) | results2.value( key ) ) );
      }
      else {
        results.insert( key, ( results1.value( key ) + results2.value( key ) ) );
      }
    }

    return results;
  }
//-------------------------------------------------------------------------------------------------
