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
      CReverseLookupMap() {
        /* nothing to do here */
      }

      CReverseLookupMap( const CReverseLookupMap& other ) {
        assign( other );
      }

      CReverseLookupMap& operator=( const CReverseLookupMap& other ) {
        assign( other );
        return *this;
      }

      ~CReverseLookupMap() {
        /* nothing to do here */
      }

      inline void insert( const K& key, const T& value ) {
        forwardMap.insert( key, value );
        reverseMap.insert( value, key );
      }

      inline void removeKey( const K& key ) {
        T value = forwardMap.value( key );
        forwardMap.remove( key );
        reverseMap.remove(value );
      }

      inline void removeValue( const T& value ) {
        K key = reverseMap.value( value );
        forwardMap.remove( key );
        reverseMap.remove( value );
      }

      inline T retrieveValue( const K& key ) const {
        return forwardMap.find( key ).value();
      }

      inline K retrieveKey( const T& value ) {
        return reverseMap.find( value ).value();
      }

      inline bool containsValue( const T& value ) const {
        return reverseMap.contains( value );
      }

      inline bool containsKey( const K& key ) const {
        return forwardMap.contains( key );
      }

      inline T valueAtIndex( const int i ) const {
        return forwardMap.values().at(i);
      }

      inline K keyAtIndex( const int i ) const {
        return forwardMap.keys().at(i);
      }

      void clear( void ) {
        forwardMap.clear();
        reverseMap.clear();
      }

      int count( void ) const {
        return forwardMap.count();
      }

      QList<K> keys() const { return forwardMap.keys(); }
      QList<T> values() const { return forwardMap.values(); }

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
      void assign( const CReverseLookupMap& other ) {
        forwardMap = other.forwardMap;
        reverseMap = other.reverseMap;
      }

      QMap<K, T> forwardMap;
      QMap<T, K> reverseMap;
};

#endif // CREVERSELOOKUPMAP_H
