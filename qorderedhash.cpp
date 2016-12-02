/*
qorderedhash.h/cpp
------------------
Author: Aaron Reeves <aaron.reeves@naadsm.org>
--------------------------------------------------
Copyright (C) 2016 Aaron Reeves

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#include "qorderedhash.h"

void testQOrderedHash() {
  QOrderedHash<QString, int> ohash;

  ohash.insert( "one", 1 );
  ohash.insert( "two", 2 );
  ohash.insert( "three", 3 );
  ohash.insert( "two again", 2 );

  qDebug() <<"ohash.count() should be 4:" << ohash.count();
  qDebug() << "ohash.at(1) should be 2:" << ohash.at(1);
  qDebug() << "ohash.value( \"three\" ) should be 3:" << ohash.value( "three" );
  qDebug() << "ohash.containsKey( \"two\" ) should be true:" << ohash.containsKey( "two" );
  qDebug() << "ohash.containsKey( \"four\" ) should be false:" << ohash.containsKey( "four" );
  qDebug() << "ohash.containsValue( 4 ) should be false:" << ohash.containsValue( 4 );
  qDebug() << "ohash.countByKey( \"three\" ) should be 1:" << ohash.countByKey( "three" );
  qDebug() << "ohash.countByValue( 2 ) should be 2:" << ohash.countByValue( 2 );

  qDebug() << "ohash.keys() should be \"one, two, three, two again\":" << ohash.keys();

  qDebug() << "ohash.removeAllValues( 2 ) should be 2:" << ohash.removeAllValues( 2 );
  qDebug() << "ohash.values() should now be 1, 3:" << ohash.values();

  qDebug() << "ohash.removeKey( \"three\" ) should be 1:" << ohash.removeKey( "three" );
  qDebug() << "ohash.count() should now be 1:" << ohash.count();
  qDebug() << "Remaining item should have key \"one\" and value 1:" << ohash.keys() << ohash.values();

  ohash.clear();
  qDebug() << "ohash.isEmpty() should now be true:" << ohash.isEmpty();
}
