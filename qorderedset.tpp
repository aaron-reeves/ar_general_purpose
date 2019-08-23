/*
qorderedset.h/tpp
-----------------
Author: Aaron Reeves <aaron.reeves@sruc.ac.uk>
--------------------------------------------------
Copyright (C) 2019 Scotland's Rural College (SRUC)

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#include "qorderedset.h" // For convenience only: this would be a circular reference without guards.

template <class T>
QOrderedSet<T>::QOrderedSet() : QMap<T, int>() {
  // Nothing else to do here
}


template <class T>
QOrderedSet<T>::QOrderedSet( const QOrderedSet<T>& other ) : QMap<T, int>( other ) {
  // Nothing else to do here
}


template <class T>
QOrderedSet<T>& QOrderedSet<T>::operator=( const QOrderedSet<T>& other ) {
  QMap<T, int>::operator=( other );
  
  return *this;
}


template <class T>
typename QMap<T, int>::iterator QOrderedSet<T>::insert( const T val ) {
  return QMap<T, int>::insert( val, 0 );
}


template <class T>
QList<T> QOrderedSet<T>::values() {
  return QMap<T, int>::keys();
}


