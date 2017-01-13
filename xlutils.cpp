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

QString XLSX::lastErrorMessage() {
  return _lastErrorMessage;
}

bool XLSX::error() {
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





