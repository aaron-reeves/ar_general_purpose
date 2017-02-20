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

class CXlCsv : public QCsv {
  public:
    enum CXlCsvFileFormat {
      FormatUnknown,
      Format2007,   // *.xlsx format, Excel 2007 onward
      Format97_2003 // *.xls format (BIFF5 or BIFF8), Excel 97 - 2003
    };

    CXlCsv(); // Constructs an object, the key properties of which must be set separately before the file can be opened.

    // Constructs AND OPENS the object.
    CXlCsv(
      const CXlCsvFileFormat fileFormat,
      const QString& filename,
      const bool containsFieldList,
      const int nLinesToSkip = 0,
      const QString& sheetname = ""
    );
    ~CXlCsv(); // Basic destructor

    // Opens the file/object for reading/manipulation in EntireFile mode.
    // Format and filename must have been specified.  Sheet name and nCommentRows are optional.
    virtual bool open();

    // The file format
    void setFileFormat( const CXlCsvFileFormat val ) { _fileFormat = val; }
    CXlCsvFileFormat fileFormat() const { return _fileFormat; }

    // The number (0-indexed) of the worksheet to process
    void setSheetIdx( const int val );
    int sheetIdx();

    // The name of the worksheet to process
    void setSheetname( const QString& sheetname );
    QString sheetname();

    // It's not always possible (yet) to automatically determine the intended format of a column of numbers,
    // particularly for dates and times.  These functions, which can be called once a file is open,
    // will properly format a column with the indicated date or time format.  These Excel-specific functions
    // account for 1900 and 1904 date systems.  QCsv also has functions for determining date formats.
    bool setFieldFormatXl( const QString& fieldName, const ColumnFormat fmt );
    bool setFieldFormatXl( const int fieldIdx, const ColumnFormat fmt );

  protected:
    QString sheetToOpen( bool& ok );
    void initializeXl();

    bool openXlsx();
    bool openXls();

    CXlCsvFileFormat _fileFormat;
    QStringList _sheetNames;
    bool _useSheetname;
    int _sheetIdx;
    QString _sheetname;
    bool _is1904DateSystem;
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

