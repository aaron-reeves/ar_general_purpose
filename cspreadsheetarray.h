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
#include <ar_general_purpose/creverselookupmap.h>
#include <ar_general_purpose/csv.h>

#include <xls.h>


class CSpreadsheetWorkBook;


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
    bool isMerged() const { return hasSpan(); }
    const QXlsx::CellRange mergedRange( const int col, const int row ) const;

    void setValue( QVariant value ) { _value = value; }
    const QVariant value() const { return _value; }

  protected:
    void assign( const CSpreadsheetCell& other );

    QVariant _value;
    int _colSpan;
    int _rowSpan;
};


class CSpreadsheet : public CTwoDArray<CSpreadsheetCell> {
  public:
    CSpreadsheet();
    CSpreadsheet( CSpreadsheetWorkBook* wb );
    CSpreadsheet( const int nCols, const int nRows );
    CSpreadsheet( const int nCols, const int nRows, const QVariant defaultVal );
    CSpreadsheet( const int nCols, const int nRows, const CSpreadsheetCell defaultVal );
    CSpreadsheet( const CSpreadsheet& other );
    CSpreadsheet& operator=( const CSpreadsheet& other );

    ~CSpreadsheet();

    bool isTidy( const bool containsHeaderRow );
    QStringList rowAsStringList( const int rowNumber );
    QCsv asCsv( const bool containsHeaderRow, const QChar delimiter = ',' );

    bool readXls( const int sheetIdx, xls::xlsWorkBook* pWB, const bool displayVerboseOutput = false );
    bool readXlsx( const QString& sheetName, QXlsx::Document* xlsx, const bool displayVerboseOutput = false );
    bool writeXlsx( const QString& fileName );

    void debug( const int padding = 10 ) const;

  protected:
    void initialize();

    CSpreadsheetWorkBook* _wb;
    bool _hasMergedCells;

    void assign( const CSpreadsheet& other );

    // Convert numbers derived from old-fashioned Excel spreadsheets to Qt objects
    QDate xlsDate( const int val, const bool is1904DateSystem );
    QTime xlsTime( const double d );
    QDateTime xlsDateTime( const double d, const bool is1904DateSystem );
};


class CSpreadsheetWorkBook {
  public:
    enum  SpreadsheetFileFormat {
      FormatUnknown,
      Format2007,   // *.xlsx format, Excel 2007 onward
      Format97_2003 // *.xls format (BIFF5 or BIFF8), Excel 97 - 2003
    };

    CSpreadsheetWorkBook( const SpreadsheetFileFormat fileFormat, const QString& fileName, const bool displayVerboseOutput = false );
    ~CSpreadsheetWorkBook();

    bool readSheet( const int sheetIdx );
    bool readSheet( const QString& sheetName );
    bool readAllSheets();

    QVariantList firstRowFromSheet( const int sheetIdx );
    QVariantList rowFromSheet( const int rowIdx, const int sheetIdx );

    bool error() const { return !_ok; }
    QString erroMessage() const { return _errMsg; }

    bool hasSheet( const int idx );
    bool hasSheet( const QString& sheetName );
    int sheetIndex( const QString& sheetName );
    QString sheetName( const int idx );
    CSpreadsheet& sheet( const int idx );
    CSpreadsheet& sheet( const QString& sheetName );

    bool isXls1904DateSystem();
    bool isXlsDate( const int xf, const double d );
    bool isXlsTime( const int xf, const double d );
    bool isXlsDateTime( const int xf, const double d );

  protected:
    bool openXlsWorkbook();
    bool openXlsxWorkbook();

    QVariantList rowFromSheetXlsx( const int rowIdx, const QString& sheetName );

    QString _srcFileName;
    SpreadsheetFileFormat _fileFormat;
    bool _displayVerboseOutput;

    QHash<int, CSpreadsheet> _sheets; // Sheets don't have to be read consecutively.  Key is the sheet index, value is the sheet itself.
    CReverseLookupMap<int, QString> _sheetNames;

    bool _ok; // True if the file could be read, etc.
    QString _errMsg;

    QXlsx::Document* _xlsx;
    xls::xlsWorkBook* _pWB;

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

    // Note: In this version, at least, date/time stamps are accurate only to the
    // nearest minute, and time zones/daylight savings time are ignored completely.
    // (The time zone was when/where a spreadsheet was created can't be determined.)
    //---------------------------------------------------------------------------------
    QHash<int, int> _xlsXFs; // key = xf index, value = format index
    QHash<int, QString> _xlsFormats; // key = format index, value = string format
    bool _xlsIs1904;
    //---------------------------------------------------------------------------------
};

#endif // CSPREADSHEETARRAY_H

