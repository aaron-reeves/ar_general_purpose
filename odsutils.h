/*
odsutils.h/cpp
---------------
Begin: 2016/11/11
Author: Aaron Reeves <aaron.reeves@sruc.ac.uk>
--------------------------------------------------
Copyright (C) 2016 Scotland's Rural College (SRUC), Epidemiology Research Unit

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#ifndef ODSUTILS_H
#define ODSUTILS_H

#include <ods/odsCore>

#include <ar_general_purpose/csv.h>

namespace ODS {
  QString getCellValue( ods::Cell *cell );
  QString getCellValue( ods::Sheet *sheet, const int colIdx, const int rowIdx );
  QStringList readRow( ods::Sheet *sheet, const int rowIdx, const bool makeLower = false );
  QCsv odsToCsv( const QString& filename );
}

/* SAMPLE CODE
   ===========
  // For ODS file parsing
  //---------------------
  QString GetCellValue(ods::Cell *cell) {
    if (cell->HasFormula()){
      auto *formula = cell->formula();
      if (formula->HasAnError())
        return QString("formula error: ") + formula->error();
      auto &value = formula->value();
      if (value.IsNotSet()) // should never happen
        return "formula has no value";
      if (value.IsDouble() || value.IsPercentage()){
        return QString("formula value: ")
          + QString::number(*value.AsDouble());
      }
      // don't care, just print out as a string
      return QString("formula has a non-number value: ") + value.toString();
    }

    ods::Value &value = cell->value();

    qDebug() << value.valueAsString();

    if (value.IsNotSet())
      return "cell value is empty";
    else if (value.IsDouble())
      return QString("cell value as double: ")
         + QString::number(*value.AsDouble());
    else if (value.IsPercentage())
      return QString("cell value as percentage: ")
        + QString::number(*value.AsPercentage());
    else if (value.IsString())
      return QString("cell value as string: ") + *value.AsString();
    else if( value.IsDate() )
      return QString( "cell value as date: ") + value.AsDate()->toString( "yyyy-MM-dd" );

    return "unknown cell type";
  }

  void Lesson18_ReadFile( const QString& path ) {
    QFile file( path );
    if (!file.exists()) {
      qDebug() << "No such file:" << path;
      return;
    }
    ods::Book book(path);

    ods::Sheet *sheet = book.sheets().at(0);
    //ods::Sheet *sheet = book.sheet( 0 );
    if (sheet == nullptr) {
      qDebug() << "No sheet at 0";
      return;
    }

    //print out the values of the first 9 cells of the 3rd row:
    const int kRow = 2;
    for (int i=0; i <= 8; i++) {
      const int kColumn = i;
      ods::Cell *cell = nullptr;
      auto *row = sheet->row(kRow);
      if (row == nullptr) {
        qDebug() << "No row at " << QString::number(kRow);
        return;
      }
      cell = row->cell(kColumn);

      if (cell == nullptr) {
        qDebug() << "No cell at " << QString::number(kColumn);
        continue;
      }
      QString which_cell = "Cell [" + QString::number(kRow)
        + ":" + QString::number(kColumn) + "]: ";
      QString value_as_string = GetCellValue(cell);
      //qDebug() << which_cell << value_as_string;
    }
  }


  // For testing ODS file parsing.
  int main() {
    QString filename = "C:/Users/areeves/Documents/ResearchProjects/sphn/sampleDataFiles/Salmonella/abatdata/MILLERS - GRANTOWN ON SPEY - SALMONELLA MEAT JUICE SAMPLE COLLECTION SHEET.ods";

    Lesson18_ReadFile( filename );

    return 0;
  }
*/

#endif
