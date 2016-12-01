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

template <typename Key, typename T>
QOrderedHash<Key, T>::QOrderedHash() {
  // Nothing to do here.
}

template <typename Key, typename T>
QOrderedHash<Key, T>::QOrderedHash( const QOrderedHash<Key, T>& other ) {
  this->_hash = other._hash;
  this->_values = other._values;
  this->_keys = other._keys;
}

template <typename Key, typename T>
QOrderedHash<Key, T>::~QOrderedHash() {
  // Nothing to do here.
}

template <typename Key, typename T> const T QOrderedHash<Key, T>::at( int i ) const { return _hash.value( _keys.at(i) ); }
template <typename Key, typename T> void QOrderedHash<Key, T>::clear() { _keys.clear(); _values.clear(); _hash.clear(); }
template <typename Key, typename T> bool QOrderedHash<Key, T>::containsKey( const Key& key ) const { return _hash.contains( key ); }
template <typename Key, typename T> bool QOrderedHash<Key, T>::containsValue( const T& value ) const { return _values.contains( value ); }
template <typename Key, typename T> int QOrderedHash<Key, T>::count() const { return _hash.count(); }
template <typename Key, typename T> int QOrderedHash<Key, T>::countByKey( const Key& key ) const { return _hash.count( key ); }
template <typename Key, typename T> int QOrderedHash<Key, T>::countByValue( const T& value ) const { return _values.count( value ); }
template <typename Key, typename T> bool QOrderedHash<Key, T>::isEmpty() const { return _hash.isEmpty(); }
template <typename Key, typename T> QList<Key> QOrderedHash<Key, T>::keys() const { return _keys; }
template <typename Key, typename T> const T QOrderedHash<Key, T>::value( const Key& key ) const { return _hash.value( key ); }
template <typename Key, typename T> QList<T> QOrderedHash<Key, T>::values() const { return _values; }


template <typename Key, typename T>
void QOrderedHash<Key, T>::insert( const Key& key, const T& value ) {
  int idx = _keys.indexOf( key );
  if( -1 != idx ) {
    _keys.removeAt( idx );
    _values.removeAt( idx );
  }

  _hash.insert( key, value );
  _keys.append( key );
  _values.append( value );
}


template <typename Key, typename T>
int QOrderedHash<Key, T>::removeKey( const Key& key ) {
  int idx = _keys.indexOf( key );
  if( -1 != idx ) {
    _keys.removeAt( idx );
    _values.removeAt( idx );
  }

  return _hash.remove( key );
}


template <typename Key, typename T>
int QOrderedHash<Key, T>::removeAllValues( const T& value ) {
  int result = 0;
  int idx = _values.indexOf( value );
  while( -1 != idx ) {
    Key key = _keys.at( idx );
    _keys.removeAt( idx );
    _values.removeAt( idx );
    _hash.remove( key );

    idx = _values.indexOf( value );

    ++result;
  }

  return result;
}


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
