/*
xlutils.h/cpp
-------------
Begin: 2016/09/27
Author: Aaron Reeves <aaron.reeves@sruc.ac.uk>
--------------------------------------------------
Copyright (C) 2016 Scotland's Rural College (SRUC), Epidemiology Research Unit

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/


#include "xlutils.h"

QString _lastErrorMessage;
bool _error;

QString lastErrorMessage() {
  return _lastErrorMessage;
}

bool error() {
  return _error;
}

QStringList XLSX::readRow( QXlsx::Document* xlsx, const int rowIdx, const bool makeLower /* = false */ ) {
  _lastErrorMessage = "";
  _error = false;

  QStringList list;

  QXlsx::CellRange cellRange = xlsx->dimension();
  if( (0 >= cellRange.firstRow()) || (0 >= cellRange.firstColumn()) || (0 >= cellRange.lastRow()) || (0 >= cellRange.lastColumn()) ) {
    _lastErrorMessage = "Cell range is out of bounds in XLSX::readRow()";
    _error = true;
  }
  else {
    int colIdx = 1;
    bool nullFound = false;
    QVariant val;
    QString str;

    // Read the indicated row.
    while( !nullFound ) {
      val = xlsx->read( rowIdx, colIdx );
      if( !val.isValid() )
        nullFound = true;
      else {
        str = val.toString().trimmed();
        if( makeLower )
          str = str.toLower();
        list.append( str );
        ++colIdx;
      }
    }
  }

  return list;
}


qCSV XLSX::xlsxToCsv( const QString& filename, const int nCommentRows /* = 0 */, const QString& sheetname /* = "" */ ) {
  qCSV csv;
  csv.setFilename( filename );
  _lastErrorMessage = "";
  _error = false;

  // Nonexistent files or files that cannot be read will return cell ranges with negative values.
  // Empty files will return cell ranges with values of 1 (which seems weird).
  // Actual files return cell ranges that are somewhat reasonable, but can include empty rows or columns.
  // ODS files can't be read by QXlsx... not a huge surprise.

  QXlsx::Document xlsx( filename );
  if( !sheetname.isEmpty() && !xlsx.selectSheet( sheetname ) ) {
    _lastErrorMessage = QString( "Specified worksheet (%1) could not be selected in XLSX::xlsxToCsv()" ).arg( sheetname );
    _error = true;
    return csv;
  }

  QXlsx::CellRange cellRange = xlsx.dimension();
  if( (0 >= cellRange.firstRow()) || (0 >= cellRange.firstColumn()) || (0 >= cellRange.lastRow()) || (0 >= cellRange.lastColumn()) ) {
    _lastErrorMessage = "Cell range is out of bounds in XLSX::xlsxToCsv()";
    _error = true;
  }
  else {
    int row = 1 + nCommentRows;
    int col = 1;
    bool nullFound = false;
    QStringList list;
    QVariant val;

    // Read the header row first.
    while( !nullFound ) {
      val = xlsx.read( row, col );
      if( !val.isValid() )
        nullFound = true;
      else {
        list.append( val.toString() );
        ++col;
      }
    }

    csv = qCSV( list );

    // Subsequent rows contain data.
    int nCols = list.count();
    int nullsFound;

    while( true ) {
      ++row;
      list.clear();
      nullsFound = 0;
      for( int c = 1; c < (nCols + 1); ++c ) {
        val = xlsx.read( row, c );
        if( !val.isValid() ) {
          ++nullsFound;
        }
        list.append( val.toString() );
      }

      if( nullsFound < nCols ) {
        csv.appendRow( list );
      }
      else {
        break;
      }
    }
  } 
  
  csv.toFront();

  return csv;
}



