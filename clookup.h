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

#include <qhash.h>

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
        _forwardHash.insert( key, value );
        _reverseHash.insert( value, key );
      }

      inline T retrieveValue( const K& key ) const {
        return _forwardHash.find( key ).value();
      }

      inline K retrieveKey( const T& value ) {
        return _reverseHash.find( value ).value();
      }

      inline bool containsValue( const T& value ) const {
        return _reverseHash.contains( value );
      }

      inline bool containsKey( const K& key ) const {
        return _forwardHash.contains( key );
      }

      inline T valueAtIndex( const int i ) const {
        return _forwardHash.values().at(i);
      }

      inline K keyAtIndex( const int i ) const {
        return _forwardHash.keys().at(i);
      }

      void clear( void ) {
        _forwardHash.clear();
        _reverseHash.clear();
      }

      int count( void ) const {
        return _forwardHash.count();
      }

      QList<K> keys() const { return _forwardHash.keys(); }
      QList<T> values() const { return _forwardHash.values(); }

      void debug() {
        int i;
        qDebug() << QString( "Forward:" );
        for( i = 0; i < _forwardHash.count(); ++i )
          qDebug() << "  " << _forwardHash.keys().at(i) << _forwardHash.values().at(i);
        qDebug() << "Backward:";
        for( i = 0; i < _reverseHash.count(); ++i )
          qDebug() << "  " << _reverseHash.keys().at(i) << _reverseHash.values().at(i);
      }

    protected:
      void assign( const CLookup& other ) {
        _forwardHash = other._forwardHash;
        _reverseHash = other._reverseHash;
      }

      QHash<K, T> _forwardHash;
      QHash<T, K> _reverseHash;
};

#endif // CLOOKUP_H
