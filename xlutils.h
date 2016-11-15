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

#ifndef XLUTILS_H
#define XLUTILS_H

#include <QtCore>
#include <QtXlsx>

#include "csv.h"

namespace XLSX {
  QStringList readRow( QXlsx::Document* xlsx, const int rowIdx, const bool makeLower = false );
  qCSV xlsxToCsv( const QString& filename, const int nCommentRows = 0, const QString& sheetname = "" );
  QString lastErrorMessage();
  bool error();
}


/* SAMPLE CODE
   ===========
  // For Excel file parsing
  //-----------------------
  int main() {
    //QString filename = "C:/Users/areeves/Documents/ResearchProjects/sphn/sampleDataFiles/Salmonella/labdata/salmonella results - 20072016.XLSX";

    qCSV csv = XLSX::xlsxToCsv( filename );

    csv.debug();

  //  QStringList sampleIDs = csv.fieldValues( "sampnum", true );

  //  for( int i = 0; i < sampleIDs.count(); ++i ) {
  //    qCSV filtered = csv.filter( "sampnum", sampleIDs.at( i ) );
  //    qDebug() << filtered.rowCount();
  //  }

    return 0;
  }
*/

#endif


