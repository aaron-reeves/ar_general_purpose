/*
creverselookupmap.h/cpp
-----------------------
Begin: 2004/02/15
Author: Aaron Reeves <aaron.reeves@naadsm.org>
--------------------------------------------------
Copyright (C) 2004 - 2014 Aaron Reeves

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/


#ifndef CLOOKUP_H
#define CLOOKUP_H

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
class CLookup {
    public:
      CLookup() {
        /* nothing to do here */
      }

      CLookup( const CLookup& other ) {
        assign( other );
      }

      CLookup& operator=( const CLookup& other ) {
        assign( other );
        return *this;
      }

      ~CLookup() {
        /* nothing to do here */
      }

      inline void insert( const K& key, const T& value ) {
        _forwardMap.insert( key, value );
        _reverseMap.insert( value, key );
      }

      inline T retrieveValue( const K& key ) const {
        return _forwardMap.find( key ).value();
      }

      inline K retrieveKey( const T& value ) {
        return _reverseMap.find( value ).value();
      }

      inline bool containsValue( const T& value ) const {
        return _reverseMap.contains( value );
      }

      inline bool containsKey( const K& key ) const {
        return _forwardMap.contains( key );
      }

      inline T valueAtIndex( const int i ) const {
        return _forwardMap.values().at(i);
      }

      inline K keyAtIndex( const int i ) const {
        return _forwardMap.keys().at(i);
      }

      void clear( void ) {
        _forwardMap.clear();
        _reverseMap.clear();
      }

      int count( void ) const {
        return _forwardMap.count();
      }

      QList<K> keys() const { return _forwardMap.keys(); }
      QList<T> values() const { return _forwardMap.values(); }

      void debug() {
        int i;
        qDebug() << QString( "Forward:" );
        for( i = 0; i < _forwardMap.count(); ++i )
          qDebug() << "  " << _forwardMap.keys().at(i) << _forwardMap.values().at(i);
        qDebug() << "Backward:";
        for( i = 0; i < _reverseMap.count(); ++i )
          qDebug() << "  " << _reverseMap.keys().at(i) << _reverseMap.values().at(i);
      }

    protected:
      void assign( const CLookup& other ) {
        _forwardMap = other._forwardMap;
        _reverseMap = other._reverseMap;
      }

      QMap<K, T> _forwardMap;
      QMap<T, K> _reverseMap;
};

#endif // CLOOKUP_H
