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

#ifndef XLCSV_H
#define XLCSV_H

#include <QtCore>
#include <QtXlsx>
#include "csv.h"

class CXlCsv : public qCSV {
  public:
    CXlCsv( const QString& filename, const int nCommentRows = 0, const QString& sheetname = "" );
    ~CXlCsv();
};

/* SAMPLE CODE
   ===========
  // For Excel file parsing
  //-----------------------
  int main() {
    //QString filename = "C:/Users/areeves/Documents/ResearchProjects/sphn/sampleDataFiles/Salmonella/labdata/salmonella results - 20072016.XLSX";

    CXlCsv csv( filename );

    csv.debug();

  //  QStringList sampleIDs = csv.fieldValues( "sampnum", true );

  //  for( int i = 0; i < sampleIDs.count(); ++i ) {
  //    qCSV filtered = csv.filter( "sampnum", sampleIDs.at( i ) );
  //    qDebug() << filtered.rowCount();
  //  }

    return 0;
  }
*/

#endif // XLCSV

