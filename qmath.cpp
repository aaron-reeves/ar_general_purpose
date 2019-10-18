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

#include <qmath.h>
#include <QtCore>

void inc( int& addTo ) {
  ++addTo;
}

void dec( int& subtractFrom ) {
  --subtractFrom;
}

void inc( int& addTo, const int toAdd ) {
  addTo = addTo + toAdd;
}

void dec( int& subtractFrom, const int toSubstract ) {
  subtractFrom = subtractFrom - toSubstract;
}


bool qBetterFuzzyCompare( const double p1, const double p2){
  if (qFuzzyIsNull(p1)) {
    return qFuzzyIsNull(p2);
  } else if (qFuzzyIsNull(p2)) {
    return false;
  } else {
    return (qAbs(p1 - p2) <= 0.000000000001 * qMin(qAbs(p1), qAbs(p2)));
  }
}


double cartesianDistanceSquared( const QPointF p1, const QPointF p2 ) {
  double asq, bsq;

  asq = pow( p1.x() - p2.x(), 2.0 );
  bsq = pow( p1.y() - p2.y(), 2.0 );

  return( asq + bsq );
}


double cartesianDistance( const QPoint p1, const QPoint p2 ) {
  return sqrt( cartesianDistanceSquared( QPointF( p1.x(), p1.y() ), QPointF( p2.x(), p2.y() ) ) );
}


double cartesianDistance(const QPointF p1, const QPointF p2 ) {
  return sqrt( cartesianDistanceSquared( p1, p2 ) );
}

