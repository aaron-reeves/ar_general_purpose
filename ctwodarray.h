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

#include <QtCore>

template <class T>
class CTwoDArray {
  public:
    CTwoDArray();
    CTwoDArray( const int nCols, const int nRows );
    CTwoDArray( const int nCols, const int nRows, const T defaultVal );
    CTwoDArray( const CTwoDArray& other );

    CTwoDArray& operator= ( const CTwoDArray& other );
    void assign( const CTwoDArray& other );

    ~CTwoDArray();

    // Sizing
    //-------
    void setSize( const int nCols, const int nRows ); // This currently assumes that the object is empty.
    void setSize( const int nCols, const int nRows, const T defaultVal ); // This currently assumes that the object is empty.
    void fill( const T val ); // Will overwrite existing data
    void fillRow( const int rowIdx, const T val ); // Will overwrite existing data

    void appendRow();
    void appendRow( const T defaultVal );
    void appendRow( const QString& rowName );
    void appendRow(  const QString& rowName, const T defaultVal );

    void appendRow(const QVector<T>& values );
    void appendRow( const QString& rowName, const QVector<T>& values );
    void appendRow(const QList<T>& values );
    void appendRow( const QString& rowName, const QList<T>& values );


    void removeRow( const int rowIdx );
    void removeRow( const QString& rowName );

    void removeColumn( const int colIdx );
    void removeColumn( const QString& colName );

    // Basic setter and getters
    //-------------------------
    void setValue( const int c, const int r, const T val );
    const T& value( const int c, const int r ) const;
    T& value( const int c, const int r );

    // at() is a synonym for value(), because I'm forgetful.
    const T& at( const int c, const int r ) const { return this->value( c, r ); }
    T& at( const int c, const int r ) { return this->value( c, r ); }

    int nCols() const { return _nCols; }
    int nRows() const { return _nRows; }
    int colCount() const { return _nCols; }
    int rowCount() const { return _nRows; }

    bool isEmpty() const { return( (0 == _nCols) && (0 == _nRows) ); }


    // Column and row names
    //---------------------
    bool hasColNames() const { return !_colNames.isEmpty(); }
    bool hasRowNames() const { return !_rowNames.isEmpty(); }

    bool hasRowName( const QString& rowName ) const { return _rowNames.contains( rowName ); }
    bool hasColumnName( const QString& colName ) const { return _colNames.contains( colName ); }

    void setColNames( const QStringList& names );
    void setRowNames( const QStringList& names );

    const QStringList& colNames() const { return _colNames; }
    const QStringList& rowNames() const { return _rowNames; }


    // Setters, getters, and at() for use with cell and row names
    //-----------------------------------------------------------
    void setValue( const QString& colName, const int r, const T val );
    void setValue( const int c, const QString& rowName, const T val );
    void setValue( const QString& colName, const QString& rowName, const T val );

    T& value( const QString& colName, const int r );
    T& value( const int c, const QString& rowName );
    T& value( const QString& colName, const QString& rowName );

    const T& value( const QString& colName, const int r ) const;
    const T& value( const int c, const QString& rowName ) const;
    const T& value( const QString& colName, const QString& rowName ) const;

    T& at( const QString& colName, const int r ) { return this->value( colName, r ); }
    T& at( const int c, const QString& rowName ) { return this->value( c, rowName ); }
    T& at( const QString& colName, const QString& rowName ) { return this->value( colName, rowName ); }

    const T& at( const QString& colName, const int r ) const { return this->value( colName, r ); }
    const T& at( const int c, const QString& rowName ) const { return this->value( c, rowName ); }
    const T& at( const QString& colName, const QString& rowName ) const { return this->value( colName, rowName ); }

    QVector<T> row( const int rowIdx ) const;
    QVector<T> row( const QString& rowName ) const;
    QVector<T> column( const int colIdx ) const;
    QVector<T> column( const QString& colName ) const;

  protected:
    void initialize();

    // Do this some day, if sizing becomes dynamic.
    //void resizeNames();

    int _nCols;
    int _nRows;

    bool _useDefaultVal;
    T _defaultVal;

    bool _hasColNames;
    bool _hasRowNames;
    QStringList _colNames;
    QStringList _rowNames;

    // Each vector represents a column with size of _nRows.
    // The list represents the columns.
    QList< QVector<T> > _data;
};

#include "ctwodarray.tpp"

#endif // CTWODARRAY_H
