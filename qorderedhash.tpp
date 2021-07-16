/*
qorderedhash.h/tpp/cpp
----------------------
Author: Aaron Reeves <aaron.reeves@naadsm.org>
--------------------------------------------------
Copyright (C) 2016 - 2021 Aaron Reeves

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#include "qorderedhash.h" // For convenience only: this would be a circular reference without guards.

template <class Key, class T>
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


template <class Key, class T>
int QOrderedHash<Key, T>::removeKey( const Key& key ) {
  int idx = _keys.indexOf( key );
  if( -1 != idx ) {
    _keys.removeAt( idx );
    _values.removeAt( idx );
  }

  return _hash.remove( key );
}


template <class Key, class T>
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


template <class Key, class T>
T QOrderedHash<Key, T>::takeFirst() {
  Q_ASSERT( !this->isEmpty() );

  QString key = _keys.first();
  _keys.removeFirst();
  _values.removeFirst();

  return _hash.take( key );
}


template <class Key, class T>
T QOrderedHash<Key, T>::takeLast() {
  Q_ASSERT( !this->isEmpty() );

  QString key = _keys.last();
  _keys.removeLast();
  _values.removeLast();

  return _hash.take( key );
}


template <class Key, class T>
T QOrderedHash<Key, T>::takeAt( const int i ) {
  Q_ASSERT( !this->isEmpty() );

  QString key = _keys.at(i);
  _keys.removeAt(i);
  _values.removeAt(i);

  return _hash.take( key );
}


template <class Key, class T>
void QOrderedHash<Key, T>::assign( const QOrderedHash<Key, T>& other ) {
  this->_hash = other._hash;
  this->_values = other._values;
  this->_keys = other._keys;
}
