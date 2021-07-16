/*
qorderedhash.h/cpp
------------------
Author: Aaron Reeves <aaron.reeves@naadsm.org>
--------------------------------------------------
Copyright (C) 2016 - 2021 Aaron Reeves

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#ifndef QORDEREDHASH_H
#define QORDEREDHASH_H

#include <QtCore>

/* 
 * This class operates like a hash table, with the additional feature that values can be retrieved
 * in the original order in which they were inserted.
 * 
 * Values may be retrieved by key using value( key ), or by index using at( index )
 */
template <class Key, class T>
class QOrderedHash {
  public:
    QOrderedHash() { /* Nothing to do here. */ }
    QOrderedHash( const QOrderedHash<Key, T>& other ) { assign( other ); }
    QOrderedHash& operator=( const QOrderedHash<Key, T>& other ) { assign( other ); return *this; }
    ~QOrderedHash() { /* Nothing to do here. */ }

    const T at( const int idx ) const { return _hash.value( _keys.at(idx) ); }
    const Key keyAt( const int idx ) const { return _keys.at(idx); }
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

    void insert( const Key& key, const T& value );

    int removeKey( const Key& key );
    int removeAllValues( const T& value );

    T takeFirst();
    T takeLast();
    T takeAt( const int i );

  protected:
    void assign( const QOrderedHash<Key, T>& other );

    QList<Key> _keys;
    QList<T> _values;
    QHash<Key, T> _hash;
};

// FIXME: Some day, think about writing an iterator, if it's ever necessary.
// For most purposes, though, a simple for loop ought to be sufficient.
//template <class Key, class T>
//class QOrderedHashIterator {
//  public:
//    QOrderedHashIterator();

//};

void testQOrderedHash(); // Defined in qorderedhash.cpp.  Used for testing the class.

#include "qorderedhash.tpp"

#endif // QORDEREDHASH_H
