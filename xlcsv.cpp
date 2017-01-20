/*
xlcsv.h/cpp
-----------
Begin: 2017/01/10
Author: Aaron Reeves <aaron.reeves@sruc.ac.uk>
--------------------------------------------------
Copyright (C) 2017 Epidemiology Research Unit, Scotland's Rural College (SRUC)

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#include "xlcsv.h"

CXlCsv::CXlCsv( const QString& filename, const int nCommentRows /* = 0 */, const QString& sheetname /* = "" */ ) : qCSV() {
  QFileInfo fi( filename );
  if( !fi.exists() || !fi.isReadable() ) {
    _error = qCSV::qCSV_ERROR_OPEN;
    _errorMsg = "Specified input file does not exist or cannot be read.";
    return;
  }

  this->setFilename( filename );

  // Nonexistent files or files that cannot be read will return cell ranges with negative values.
  // Empty files will return cell ranges with values of 1 (which seems weird).
  // Actual files return cell ranges that are somewhat reasonable, but can include empty rows or columns.
  // ODS files can't be read by QXlsx... not a huge surprise.

  QXlsx::Document xlsx( filename );
  if( !sheetname.isEmpty() && !xlsx.selectSheet( sheetname ) ) {
    _error = qCSV::qCSV_ERROR_OTHER;
    _errorMsg = QString( "Specified worksheet (%1) could not be selected." ).arg( sheetname );
    return;
  }

  QXlsx::CellRange cellRange = xlsx.dimension();
  if( (0 >= cellRange.firstRow()) || (0 >= cellRange.firstColumn()) || (0 >= cellRange.lastRow()) || (0 >= cellRange.lastColumn()) ) {
    _error = qCSV::qCSV_ERROR_OTHER;
    _errorMsg = "Cell range is out of bounds.";
    return;
  }

  // If we get this far, populate the data object.
  //----------------------------------------------
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

  setFieldNames( list );

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
      this->appendRow( list );
    }
    else {
      break;
    }
  }

  this->toFront();
}


CXlCsv::~CXlCsv() {
  // Do nothing.
}
