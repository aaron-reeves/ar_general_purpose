/*
qintvector.h/cpp
-----------------
Begin: 2014/05/08
Author: Aaron Reeves <aaron.reeves@naadsm.org>
------------------------------------------------------
Copyright (C) 2014 Scotland's Rural College (SRUC)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.
*/

#include "qintvector.h"

#include <math.h>

#include <QDebug>


QIntVector::QIntVector( void ){
  // Use inherited constructor
  
  _sorted = false;
}


QIntVector::~QIntVector( void ){
  // Use inherited destructor
}
  

void QIntVector::append( int val ){
  static_cast<IntVector*>(this)->append( val );
  _sorted = false;  
}


void QIntVector::resize( const int newSize ){
  static_cast<IntVector*>(this)->resize( newSize );
  _sorted = false;
}  
 

int QIntVector::count( void ) {
  return this->size();
} 
 

bool QIntVector::isEmpty( void ) {
  return ( 0 == this->size() );
}


void QIntVector::sort( void ) {
  std::sort( this->begin(), this->end() );
  _sorted = true;
}


QString QIntVector::asColString( void ){
  int i;
  QString result = QString();
  
  for( i = 0; i < this->size(); ++i ) {
    result.append( QStringLiteral( "%1" ).arg( this->at(i) ) );
    
    if( i < (this->size() - 1) )
      result.append( "\r\n" );
  }

  return result;
}


QString QIntVector::asRowString( QChar delimiter /* = ',' */ ){
  int i;
  QString result = QString();
  
  for( i = 0; i < this->size(); ++i ) {
    result.append( QStringLiteral( "%1" ).arg( this->at(i) ) );
    
    if( i < (this->size() - 1) )
      result.append( delimiter );
  }

  return result;
} 
  
  
void QIntVector::debug( void ){
  int i;

  qDebug() << "--- QIntVector.debug";
  qDebug() << "Number of elements:" << this->count();
  qDebug() << "Sorted:" << this->sorted();
  for( i = 0; i < this->size(); ++i ) {
    qDebug() << this->at(i);
  }
  qDebug() << "--- Done.";
}  
 
 
bool QIntVector::indicesOK( int startIdx, int endIdx ){
  bool result;
  
  if( -1 == endIdx ) 
    endIdx = this->count() - 1;

  if( (startIdx < 0) || (startIdx > (this->count() - 1) ) ) {
    //raise exception.create( 'startIdx ' + intToStr( startIdx ) + ' out of bounds in QIntVector.mean' );
    result = false;
  } else if( (endIdx < 0) || (endIdx > (this->count()-1)) || (endIdx<startIdx) ) {
    //raise exception.create( 'endIdx ' + intToStr( startIdx ) + ' out of bounds in QIntVector.mean' );
    result = false;  
  } else {
    result = true;
  }
  
  return result;
}
 
  
// What is the mean of the specified items in the array (or the whole array, if startIdx = 0 and endIdx = -1)? 
double QIntVector::mean( const int startIdx /*= 0*/, int endIdx /*= -1*/ ){
  int i;
  double sum;
  int cnt;
  double result;

  result = 0.0;
  sum = 0.0;
  cnt = 0;
  
  if( -1 == endIdx )
    endIdx = this->count() - 1; 

  if( indicesOK( startIdx, endIdx ) ) {
    for( i = startIdx; i < endIdx+1; ++i ){
      sum = sum + this->at(i);
      ++cnt;
    }

    result = sum / cnt;
  }
  return result;
}


// What is the standard deviation of the first n items in the array (or the whole array, if startIdx = 0 and endIdx = -1)?
double QIntVector::stddev( const int startIdx /*= 0*/, int endIdx /*= -1*/ ){
  int i;
  double sum;
  int cnt;
  double mean;
  double result;

  result = 0.0;
  
  if( -1 == endIdx )
    endIdx = this->count() - 1;  
  
  if( indicesOK( startIdx, endIdx ) ) {
    mean = this->mean( startIdx, endIdx );
    
    sum = 0.0;
    cnt = 0;
    
    for( i = startIdx; i < endIdx+1; ++i ){
      sum = sum + pow( (mean - this->at(i)), 2 );
      ++cnt;
    }
    
    result = sqrt( sum / (cnt - 1) );
  }
  
  return result;
}


// What is the indicated quantile of the first n items in the array (or the whole array, if startIdx = 0 and endIdx = -1)?
double QIntVector::quantile( const double quant, const int startIdx /*= 0*/, int endIdx /*= -1*/ ){
  QIntVector* v;
  int i;
  double result;
  
  if( -1 == endIdx )
    endIdx = this->count() - 1; 

  result = 0.0;
  
  if( indicesOK( startIdx, endIdx ) ) {
    if( (0 == startIdx) && ( (this->count()-1)==endIdx ) && ( this->sorted() ) ) {
      result = this->quantileSorted( quant );
    } else {
      v = new QIntVector();
      for( i = startIdx; i < endIdx+1; ++i ) {
        v->append( this->at(i) );
      }
      
      v->sort();
      result = v->quantileSorted( quant );
      delete v;
    }
  }  
  
  return result;  
}


double QIntVector::quantileSorted( const double quant ){
  double index;
  int lhs;
  double delta;
  int n;
  double result;

  n = this->count();
  index = quant * ( n - 1 );
  lhs = int(trunc( index ));
  delta = index - lhs;
  
  if( 0 == n ) {
    result = 0.0;
  } else if( n - 1 == lhs ) {
    result = this->at( n - 1);
  } else {
    result = (1-delta) * this->at(lhs) + delta * this->at(lhs+1);
  }
  
  return result;
}


// What is the high value of the first n items in the array (or the whole array, if startIdx = 0 and endIdx = -1)?
double QIntVector::low( const int startIdx /*= 0*/, int endIdx /*= -1*/ ){
  int i;
  double min;

  if( -1 == endIdx )
    endIdx = this->count() - 1;   
  
  min = 0.0;
  
  if( indicesOK( startIdx, endIdx ) ) {
    min = this->at(startIdx);
    for( i = startIdx; i < endIdx+1; ++i) {
      if( min > this->at(i) ) {
        min = this->at(i);
      }
    }
  }
  
  return min;
}


// What is the low value of the first n items in the array (or the whole array, if startIdx = 0 and endIdx = -1)?
double QIntVector::high( const int startIdx /*= 0*/, int endIdx /*= -1*/ ){
  int i;
  double max;

  if( -1 == endIdx )
    endIdx = this->count() - 1;   
  
  max = 0.0;
  
  if( indicesOK( startIdx, endIdx ) ) {
    max = this->at(startIdx);
    for( i = startIdx; i < endIdx+1; ++i) {
      if( max < this->at(i) ) {
        max = this->at(i);
      }
    }
  }
  
  return max;
}


// What is the sum of the first n items in the array (or the whole array, if startIdx = 0 and endIdx = -1)?
double QIntVector::sum( const int startIdx /*= 0*/, int endIdx /*= -1*/ ){
  int i;
  double result;

  if( -1 == endIdx )
    endIdx = this->count() - 1;   
  
  result = 0.0;
  
  if( indicesOK( startIdx, endIdx ) ) {
    for( i = startIdx; i < endIdx+1; ++i) {
      result = result + this->at(i);
    }
  }
  
  return result;
}


bool QIntVector::sorted( void ){
  return _sorted;
}


