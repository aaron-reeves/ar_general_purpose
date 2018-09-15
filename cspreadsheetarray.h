/*
cspreadsheetarray.h/cpp
-----------------------
Begin: 2018/09/13
Author: Aaron Reeves <aaron.reeves@sruc.ac.uk>
--------------------------------------------------
Copyright (C) 2018 Epidemiology Research Unit, Scotland's Rural College (SRUC)

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/


#ifndef CSPREADSHEETARRAY_H
#define CSPREADSHEETARRAY_H

#include <QtCore>
#include <QtXlsx>

#include <ar_general_purpose/ctwodarray.h>

#include <xls.h>


class CSpreadsheetCell {
  public:
    CSpreadsheetCell();
    CSpreadsheetCell( const QVariant val );
    CSpreadsheetCell( const QVariant val, const int colSpan, const int rowSpan );
    CSpreadsheetCell( const CSpreadsheetCell& other );
    CSpreadsheetCell& operator=( const CSpreadsheetCell& other );
    ~CSpreadsheetCell();

    void setSpan( const int colSpan, const int rowSpan ) { _colSpan = colSpan; _rowSpan = rowSpan; }
    bool hasSpan() const { return ( (0 != _colSpan) || (0 != _rowSpan) ); }
    const QXlsx::CellRange mergedRange( const int col, const int row ) const;

    void setValue( QVariant value ) { _value = value; }
    const QVariant value() const { return _value; }

  protected:
    void assign( const CSpreadsheetCell& other );

    QVariant _value;
    int _colSpan;
    int _rowSpan;
};


class CSpreadsheetCellArray : public CTwoDArray<CSpreadsheetCell> {
  public:
    CSpreadsheetCellArray();
    CSpreadsheetCellArray( const int nCols, const int nRows );
    CSpreadsheetCellArray( const int nCols, const int nRows, const QVariant defaultVal );
    CSpreadsheetCellArray( const int nCols, const int nRows, const CSpreadsheetCell defaultVal );
    ~CSpreadsheetCellArray();

    bool readXls(const QString& fileName, const bool displayVerboseOutput = false );
    bool writeXlsx( const QString& fileName );

    void debug( const int padding = 10 ) const;

  protected:
    //---------------------------------------------------------------------------------
    // It's not straightforward in old Excel files to distinguish dates and times from
    // numbers, because dates and times in Excel are internally represented as numbers.
    // These variables and functions are needed to determine whether a number is actually
    // a date, time, or date/time.  They are used as helpers for readXls().
    // Some useful documentation can be found in
    // http://www.openoffice.org/sc/excelfileformat.pdf,particularly sections
    // 4.6 (Cell Formatting) and 5.49 (FORMAT).

    // Basically: remember that each cell holds a reference to its extended format (xf),
    // which in turns holds a reference to an actual format (format).
    // libxls structures st_cell, st_xf, and st_format store this information,
    // which is recreated in these two hash tables for convenience.
    //---------------------------------------------------------------------------------
    QHash<int, int> _xlsXFs; // key = xf index, value = format index
    QHash<int, QString> _xlsFormats; // key = format index, value = string format
    bool _xlsIs1904;

    bool isXlsDate( const int xf, const double d );
    bool isXlsTime( const int xf, const double d );
    bool isXlsDateTime( const int xf, const double d );

    QDate xlsDate( const int val );
    QTime xlsTime( const double d );
    QDateTime xlsDateTime( const double d );
    //---------------------------------------------------------------------------------
};

#endif // CSPREADSHEETARRAY_H

