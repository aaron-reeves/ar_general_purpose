/*
qdoublevector.h/cpp
-------------------
Begin: 2013/05/21
Author: Aaron Reeves <aaron.reeves@naadsm.org>
------------------------------------------------------
Copyright (C) 2013 University of Calgary Veterinary Medicine

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.
*/

#ifndef _QDOUBLEVECTOR_H_
#define _QDOUBLEVECTOR_H_

#include <QString>
#include <QVector>

typedef QVector<double> DoubleVector;


class QDoubleVector : public DoubleVector {
  public:
    QDoubleVector( void );
    ~QDoubleVector( void );
  
    void append( double val );
    void resize( const int newSize );
  
    void sort( void );
  
    QString asColString( void );
    QString asRowString( QChar delimiter = ',' );
  
    void debug( void );
  
    // What is the mean of the specified items in the array (or the whole array, if startIdx = 0 and endIdx = -1)? 
    double mean( const int startIdx = 0, int endIdx = -1 );

    // What is the standard deviation of the first n items in the array (or the whole array, if startIdx = 0 and endIdx = -1)?
    double stddev( const int startIdx = 0, int endIdx = -1 );

    // What is the indicated quantile of the first n items in the array (or the whole array, if startIdx = 0 and endIdx = -1)?
    double quantile( const double quant, const int startIdx = 0, int endIdx = -1 );

    double quantileSorted( const double quant );

    // What is the high value of the first n items in the array (or the whole array, if startIdx = 0 and endIdx = -1)?
    double low( const int startIdx = 0, int endIdx = -1 );

    // What is the low value of the first n items in the array (or the whole array, if startIdx = 0 and endIdx = -1)?
    double high( const int startIdx = 0, int endIdx = -1 );

    // What is the sum of the first n items in the array (or the whole array, if startIdx = 0 and endIdx = -1)?
    double sum( const int startIdx = 0, int endIdx = -1 );

    bool sorted( void );
    int count( void );
    bool isEmpty( void );
    
  protected:
    bool indicesOK( int startIdx, int endIdx );
    
    bool _sorted;
};

#endif
