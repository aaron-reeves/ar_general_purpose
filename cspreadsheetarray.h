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
  friend class CSpreadsheet;
  public:
    CSpreadsheetCell();
    CSpreadsheetCell( const QVariant val );
    CSpreadsheetCell( const QVariant val, const int colSpan, const int rowSpan );
    CSpreadsheetCell( const CSpreadsheetCell& other );
    CSpreadsheetCell& operator=( const CSpreadsheetCell& other );
    ~CSpreadsheetCell();

    bool isNull() const { return this->value().isNull(); }
    bool isEmpty() const { return ( this->isNull() || ( (QVariant::String == this->value().type()) && ( this->value().toString().isEmpty() ) ) ); }

    // FIXME: Move this functionality to CSpreadsheet, so that _originCell can be set.
    //void setSpan( const int colSpan, const int rowSpan ) { _colSpan = colSpan; _rowSpan = rowSpan; }

    // Only the first cell in a merged range will have a span.
    // Other cells in the range will know that they are merged, but only the first cell knows the extent of the range.
    bool hasRowSpan() const { return (1 < _rowSpan); }
    bool hasColSpan() const { return (1 < _colSpan); }
    bool hasSpan() const { return ( hasColSpan() || hasRowSpan() ); }

    // Cells that span multiple rows are part of a merged COLUMN.
    // Cells that span multiple columns are part of a merged ROW.
    bool isPartOfMergedRow() { return _isPartOfMergedRow; }
    bool isPartOfMergedCol() { return _isPartOfMergedCol; }
    bool isPartOfMergedRange() { return ( isPartOfMergedCol() || isPartOfMergedRow() ); }

    const QXlsx::CellRange mergedRange( const int col, const int row ) const;
    int colSpan() const { return _colSpan; }
    int rowSpan() const { return _rowSpan; }

    void setValue( QVariant value ) { _value = value; }
    const QVariant value() const { return _value; }

    void debug();

  protected:
    void assign( const CSpreadsheetCell& other );

    QVariant _value;
    int _colSpan;
    int _rowSpan;

    // When cells are merged, all but the first cell in the range will appear to be empty.
    // Other cells have no knowledge that they are actually merged, unless these flags are set.
    // When these flags are set, then it's possible to work backward to the first cell in the range.
    bool _isPartOfMergedRow;
    bool _isPartOfMergedCol;

    // The "origin" cell knows which other cells are merged with it, but it's not easy to get from a merged cell back to its "origin".
    // That's what this pointer is for.
    // It will be set by CSpreadsheet.flagMergedCells() and changed as needed by other CSpreadsheet merge/unmerge functions.
    CSpreadsheetCell* _originCell;

    QSet<CSpreadsheetCell*> _linkedCells;
};


class CSpreadsheet : public CTwoDArray<CSpreadsheetCell> {
  friend class CSpreadsheetWorkBook;
  public:
    CSpreadsheet();
    CSpreadsheet( CSpreadsheetWorkBook* wb );
    CSpreadsheet( const int nCols, const int nRows );
    CSpreadsheet( const int nCols, const int nRows, const QVariant defaultVal );
    CSpreadsheet( const int nCols, const int nRows, const CSpreadsheetCell defaultVal );
    CSpreadsheet( const CSpreadsheet& other );
    CSpreadsheet& operator=( const CSpreadsheet& other );

    ~CSpreadsheet();

    CSpreadsheetCell& cell( const int c, const int r ) { return this->value( c, r ); }
    const CSpreadsheetCell& cell( const int c, const int r ) const { return this->value( c, r ); }

    QVariant cellValue( const int c, const int r ) const { return this->value( c, r ).value(); }
    QVariant cellValue( const QString& cellLabel ) const;

    bool compareCellValue( const int c, const int r, const QString& str, Qt::CaseSensitivity caseSens = Qt::CaseInsensitive );
    bool compareCellValue( const QString& cellLabel, const QString& str, Qt::CaseSensitivity caseSens = Qt::CaseInsensitive );

    bool isTidy( const bool containsHeaderRow );
    QStringList rowAsStringList( const int rowNumber );
    QCsv asCsv( const bool containsHeaderRow, const QChar delimiter = ',' );

    bool readXls( const int sheetIdx, xls::xlsWorkBook* pWB, const bool displayVerboseOutput = false );
    bool readXlsx( const QString& sheetName, QXlsx::Document* xlsx, const bool displayVerboseOutput = false );
    bool writeXlsx( const QString& fileName );

    void unmergeColumns( const bool duplicateValues );
    void unmergeRows( const bool duplicateValues );
    void unmergeColumnsAndRows( const bool duplicateValues );
    void unmergeCell( const int c, const int r, const bool duplicateValues );
    void unmergeCellsInRow( const int r, const bool duplicateValues );

    bool columnIsEmpty( const int c, const bool excludeHeaderRow = false );
    bool rowIsEmpty( const int r );
    bool hasEmptyColumns( const bool excludeHeaderRow = false );
    bool hasEmptyRows();
    void removeEmptyColumns( const bool excludeHeaderRow = false );
    void removeEmptyRows();

    void appendRow( const QVariantList& values );
    void appendRow( const QStringList& values );

    void debug( const int padding = 10 ) const;
    void debugMerges();

    CTwoDArray<QVariant> data( const bool containsHeaderRow );

    static QDateTime adjustDateTime( const QDateTime& val ) { return val.toUTC().addSecs( 3 ); }

  protected:
    void initialize();
    void flagMergedCells();

    CSpreadsheetWorkBook* _wb;
    bool _hasSpannedCells;

    void assign( const CSpreadsheet& other );

    static QVariant processCellXls( xls::xlsCell* cellValue, QString& msg, CSpreadsheetWorkBook* wb );

    // Convert numbers derived from old-fashioned Excel spreadsheets to Qt objects
    static QDate xlsDate( const int val, const bool is1904DateSystem );
    static QTime xlsTime( const double d );
    static QDateTime xlsDateTime( const double d, const bool is1904DateSystem );
};


class CSpreadsheetWorkBook {
  public:
    enum  SpreadsheetFileFormat {
      FormatUnknown,
      Format2007,   // *.xlsx format, Excel 2007 onward
      Format97_2003 // *.xls format (BIFF5 or BIFF8), Excel 97 - 2003
    };

    CSpreadsheetWorkBook( const SpreadsheetFileFormat fileFormat, const QString& fileName, const bool displayVerboseOutput = false );
    CSpreadsheetWorkBook( const QString& fileName, const bool displayVerboseOutput = false );
    ~CSpreadsheetWorkBook();

    bool readSheet( const int sheetIdx );
    bool readSheet( const QString& sheetName );
    bool readAllSheets();

    QVariantList firstRowFromSheet( const int sheetIdx );
    QVariantList rowFromSheet( const int rowIdx, const int sheetIdx );

    bool ok() const { return _ok; }
    bool error() const { return !_ok; }
    QString errorMessage() const { return _errMsg; }

    int sheetCount() const { return _sheets.count(); }
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


    // Functions for manipulating and writing workbooks
    //-------------------------------------------------
    bool addSheet( const QString& sheetName = QString() );
    bool deleteSheet( const int sheetIdx );
    bool deleteSheet( const QString& sheetName );

    bool writeSheet( const int sheetIdx, const CTwoDArray<QVariant>& data );
    bool writeSheet( const QString& sheetName, const CTwoDArray<QVariant>& data );

    // Consider writing these functions some day...
    //bool writeSheet( const int sheetIdx, QCsv* data );
    //bool writeSheet( const QString& sheetName, QCsv* data );
    //bool writeSheet( const int sheetIdx, CSpreadsheet data );
    //bool writeSheet( const QString& sheetName, CSpreadsheet data );

    bool selectSheet( const int sheetIdx );
    bool selectSheet( const QString& name );

    bool save();
    bool saveAs( const QString& filename );

    static SpreadsheetFileFormat guessFileFormat( const QString& fileName, QString* errMsg = nullptr, bool* ok = nullptr );

  protected:
    void openWorkbook( const SpreadsheetFileFormat fileFormat, const QString& fileName, const bool displayVerboseOutput );
    SpreadsheetFileFormat guessFileFormat();

    bool openXlsWorkbook();
    bool openXlsxWorkbook();

    QVariantList rowFromSheetXlsx( const int rowIdx, const QString& sheetName );
    QVariantList rowFromSheetXls( const int rowIdx, const QString& sheetName );
    QVariantList rowFromSheetXls( const int rowIdx, const int sheetIdx );

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

