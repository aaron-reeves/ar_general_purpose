/*
ctwodarray.h/cpp
------------------
Author: Aaron Reeves <aaron.reeves@sruc.ac.uk>
--------------------------------------------------
Copyright (C) 2018 SRUC, Scotland's Rural College

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#ifndef CTWODARRAY_H
#define CTWODARRAY_H

template <class T>
class CTwoDArray {
  public:
    CTwoDArray() {
      setSize( 0, 0 );
    }

    CTwoDArray( const int nCols, const int nRows ) {
      setSize( nCols, nRows );
    }

    CTwoDArray( const int nCols, const int nRows, const T defaultVal ) {
      setSize( nCols, nRows, defaultVal );
    }

    CTwoDArray( const CTwoDArray& other ) {
      _nCols = other._nCols;
      _nRows = other._nRows;
      _data = other._data;
    }

    CTwoDArray& operator= ( const CTwoDArray& other ) {
      _nCols = other._nCols;
      _nRows = other._nRows;
      _data = other._data;

      return* this;
    }

    ~CTwoDArray() {
      // Do nothing else
    }

    void setSize( const int nCols, const int nRows ) {
      _data.clear();

      _nCols = nCols;
      _nRows = nRows;

      for( int c = 0; c < nCols; ++c ) {
        _data.append( QVector<T>( _nRows ) );
      }
    }

    void setSize( const int nCols, const int nRows, const T defaultVal ) {
      _data.clear();

      _nCols = nCols;
      _nRows = nRows;

      for( int c = 0; c < nCols; ++c ) {
        QVector<T> row( _nRows );
        row.fill( defaultVal );
        _data.append( row );
      }
    }


    void setValue( const int c, const int r, const T val ) {
      _data[c][r] = val;
    }

    const T& value( const int c, const int r ) const {
      return _data.at(c).at(r);
    }

    T& value( const int c, const int r ) {
      return _data[c][r];
    }

    const T& at( const int c, const int r ) const {
      return this->value( c, r );
    }

    T& at( const int c, const int r ) {
      return _data[c][r];
    }

    int nCols() const { return _nCols; }
    int nRows() const { return _nRows; }
    bool isEmpty() const { return( (0 == _nCols) && (0 == _nRows) ); }

  protected:
    int _nCols;
    int _nRows;

    // Each vector represents a column with size of _nRows.
    // The list represents the columns.
    QList< QVector<T> > _data;
};



#endif // CTWODARRAY_H
