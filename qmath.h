/*
qmath.h/cpp
-----------
Begin: 2014-06-13
Author: Aaron Reeves <aaron.reeves@naadsm.org>
---------------------------------------------------
Copyright (C) 2014 - 2016 Aaron Reeves

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#ifndef QMATH_H
#define QMATH_H

#include <QtCore>

#include <math.h>

// Adds 1 to addTo, updating its value.
void inc( int& addTo );

// Adds the value toAdd to addTo, updating the value of addTo.
void inc( int& addTo, const int toAdd );

// Subtracts 1 from subtractFrom, updating its value.
void dec( int& subtractFrom );

void dec( int& subtractFrom, const int toSubstract );

bool qBetterFuzzyCompare( const double p1, const double p2);


double cartesianDistance( const QPoint p1, const QPoint p2 );
double cartesianDistance( const QPointF p1, const QPointF p2 );
double cartesianDistanceSquared(const QPointF p1, const QPointF p2 );

#endif // QMATH_H
