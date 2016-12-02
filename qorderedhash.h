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

#ifndef QORDEREDHASH_H
#define QORDEREDHASH_H

#include <QtCore>

template <class Key, class T>
class QOrderedHash {
  public:
    QOrderedHash() {
      // Nothing to do here.
    }
    QOrderedHash( const QOrderedHash<Key, T>& other ) {
      this->_hash = other._hash;
      this->_values = other._values;
      this->_keys = other._keys;
    }

    ~QOrderedHash() {
      // Nothing to do here.
    }

    const T at( int i ) const { return _hash.value( _keys.at(i) ); }
    void clear() { _keys.clear(); _values.clear(); _hash.clear(); }
    bool containsKey( const Key& key ) const { return _hash.contains( key ); }
    bool containsValue( const T& value ) const { return _values.contains( value ); }
    int count() const { return _hash.count(); }
    int countByKey( const Key& key ) const { return _hash.count( key ); }
    int countByValue( const T& value ) const { return _values.count( value ); }
    bool isEmpty() const { return _hash.isEmpty(); }
    QList<Key> keys() const { return _keys; }
    const T value( const Key& key ) const { return _hash.value( key ); }
    QList<T>values() const { return _values; }

    void insert( const Key& key, const T& value ) {
      int idx = _keys.indexOf( key );
      if( -1 != idx ) {
        _keys.removeAt( idx );
        _values.removeAt( idx );
      }

      _hash.insert( key, value );
      _keys.append( key );
      _values.append( value );
    }

    int removeKey( const Key& key ) {
      int idx = _keys.indexOf( key );
      if( -1 != idx ) {
        _keys.removeAt( idx );
        _values.removeAt( idx );
      }

      return _hash.remove( key );
    }

    int removeAllValues( const T& value ) {
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

    T takeFirst() {
      Q_ASSERT( !this->isEmpty() );

      QString key = _keys.first();
      _keys.removeFirst();
      _values.removeFirst();

      return _hash.take( key );
    }

    T takeLast() {
      Q_ASSERT( !this->isEmpty() );

      QString key = _keys.last();
      _keys.removeLast();
      _values.removeLast();

      return _hash.take( key );
    }

    T takeAt( const int i ) {
      Q_ASSERT( !this->isEmpty() );

      QString key = _keys.at(i);
      _keys.removeAt(i);
      _values.removeAt(i);

      return _hash.take( key );
    }

  protected:
    QList<Key> _keys;
    QList<T> _values;
    QHash<Key, T> _hash;
};

//template <class Key, class T>
//class QOrderedHashIterator {
//  public:
//    QOrderedHashIterator();

//};

void testQOrderedHash();

#endif // QORDEREDHASH_H
