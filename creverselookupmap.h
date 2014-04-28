/*
creverselookupmap.h/cpp
-----------------------
Begin: 2004/02/15
Author: Aaron Reeves <development@reevesdigital.com>
--------------------------------------------------
Copyright (C) 2004 - 2014 Aaron Reeves

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/


#ifndef CREVERSELOOKUPMAP_H
#define CREVERSELOOKUPMAP_H

#include <QDebug>

//-----------------------------------------------------------------------------
// Example
//-----------------------------------------------------------------------------
/*
int main(int argc, char** argv) {

    CReverseLookupMap<int, QString> map;

    map.insert( 1, "abc" );
    map.insert( 2, "def" );
    map.insert( 3, "ghi" );

    cout << map.retrieveKey( "abc" ) << endl; // writes "1"
    cout << map.retrieveValue( 2 ) << endl; // writes "def"

  return 0;
}
*/
//-----------------------------------------------------------------------------

#include <qmap.h>

template <class K, class T>
class CReverseLookupMap {
    public:
    // use default construction, destruction

    // I don't really know how to write templates, and my attempts to put these
    // functions in the source file were unsuccessful.  Maybe some day,
    // someone should figure it out...
    inline void insert( const K& key, const T& value ) {
      forwardMap.insert( key, value );
      reverseMap.insert( value, key );
    }

    inline T retrieveValue( const K& key ) {
      return forwardMap.find( key ).value();
    }

    inline K retrieveKey( const T& value ) {
      return reverseMap.find( value ).value();
    }

    inline bool containsValue( const T& value ){
      return reverseMap.contains( value );
    }

    inline bool containsKey( const K& key ) {
      return forwardMap.contains( key );
    }

    inline T valueAtIndex( const int i ) {
      return forwardMap.values().at(i);
    }

    inline K keyAtIndex( const int i ) {
      return forwardMap.keys().at(i);
    }

    void clear( void ) {
      forwardMap.clear();
      reverseMap.clear();
    }

    int count( void ) {
      return forwardMap.count();
    }

    void debug() {
      int i;
      qDebug() << QString( "Forward:" );
      for( i = 0; i < forwardMap.count(); ++i )
        qDebug() << "  " << forwardMap.keys().at(i) << forwardMap.values().at(i);
      qDebug() << "Backward:";
      for( i = 0; i < reverseMap.count(); ++i )
        qDebug() << "  " << reverseMap.keys().at(i) << reverseMap.values().at(i);
    }

    protected:
        QMap<K, T> forwardMap;
        QMap<T, K> reverseMap;
};

#endif
