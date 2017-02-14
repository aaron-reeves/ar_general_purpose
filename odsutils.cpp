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

#include "odsutils.h"

QCsv ODS::odsToCsv( const QString& filename ) {
  QCsv csv;

  ods::Book book( filename );
  ods::Sheet *sheet = book.sheets().at(0);

  if( nullptr != sheet ) {
    int row = 0;
    QStringList list;
    QString val;

    // Read the header row first.
    list = ODS::readRow( sheet, row, true );
    if( !list.isEmpty() ) {
      csv = QCsv( list );
    }

    // Subsequent rows contain data.
    int nCols = list.count();
    int nullsFound;

    while( true ) {
      ++row;
      list.clear();
      nullsFound = 0;
      for( int col = 0; col < nCols; ++col ) {
        val = ODS::getCellValue( sheet, col, row );
        if( val.isEmpty() ) {
          ++nullsFound;
        }
        list.append( val );
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


QString ODS::getCellValue( ods::Sheet *sheet, const int colIdx, const int rowIdx ) {
  QString result;

  auto *row = sheet->row( rowIdx );

  if( nullptr != row ) {
    ods::Cell *cell = row->cell( colIdx );

    if( nullptr != cell ) {
      result = getCellValue( cell );
    }
  }

  return result;
}


QString ODS::getCellValue( ods::Cell *cell ) {
  if( cell->HasFormula() ) {
    auto *formula = cell->formula();
    if (formula->HasAnError())
      return QString("(formula error: %1)").arg( formula->error() );

    auto &value = formula->value();
    if (value.IsNotSet()) // should never happen
      return "(formula has no value)";

    //if (value.IsDouble() || value.IsPercentage()) {
    //  return QString("formula value: ")
    //    + QString::number(*value.AsDouble());
    //}

    // don't care, just print out as a string
    return value.toString();
  }
  else {
    ods::Value &value = cell->value();
    return value.valueAsString();
  }

  //qDebug() << value.valueAsString();

  //if (value.IsNotSet())
  //  return "cell value is empty";
  //else if (value.IsDouble())
  //  return QString("cell value as double: ")
  //     + QString::number(*value.AsDouble());
  //else if (value.IsPercentage())
  //  return QString("cell value as percentage: ")
  //    + QString::number(*value.AsPercentage());
  //else if (value.IsString())
  //  return QString("cell value as string: ") + *value.AsString();
  //else if( value.IsDate() )
  //  return QString( "cell value as date: ") + value.AsDate()->toString( "yyyy-MM-dd" );

  //return "unknown cell type";
}


QStringList ODS::readRow( ods::Sheet *sheet, const int rowIdx, const bool makeLower /* = false */ ) {
  QStringList result;

  auto *row = sheet->row( rowIdx );

  if( nullptr != row ) {
    ods::Cell *cell = nullptr;
    int col = 0;
    QString cellVal;

    while( true ) {
      cell = row->cell( col );
      if( nullptr == cell ) {
        break;
      }
      else {
        cellVal = ODS::getCellValue( cell );
        cellVal = cellVal.trimmed();
        if( makeLower )
          cellVal = cellVal.toLower();

        if( cellVal.isEmpty() ) {
          break;
        }
        else {
          result.append( cellVal );
          ++col;
        }
      }
    }
  }

  return result;
}

