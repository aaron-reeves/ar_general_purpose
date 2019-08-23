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

#ifndef QORDEREDSET_H
#define QORDEREDSET_H

#include <QtCore>

template <class T>
class QOrderedSet : public QMap<T, int> {
  public:
    QOrderedSet();
    QOrderedSet( const QOrderedSet<T>& other );
    QOrderedSet& operator=( const QOrderedSet& other );
    ~QOrderedSet() { /* Nothing more to do here */ }

    typename QMap<T, int>::iterator insert( const T val );

    QList<T> values();
};

// FIXME: This needs some proper iterators

#include "qorderedset.tpp"

#endif // QORDEREDSET_H
