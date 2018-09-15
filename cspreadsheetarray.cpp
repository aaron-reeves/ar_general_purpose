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


#include "cspreadsheetarray.h"

#include <QDebug>

#include <ar_general_purpose/strutils.h>
#include <ar_general_purpose/qcout.h>


CSpreadsheetCell::CSpreadsheetCell() {
  // _value is initialized by default
  _colSpan = 0;
  _rowSpan = 0;
}


CSpreadsheetCell::CSpreadsheetCell( const QVariant val ) {
  _value = val;
  _colSpan = 0;
  _rowSpan = 0;
}


CSpreadsheetCell::CSpreadsheetCell( const QVariant val, const int colSpan, const int rowSpan ) {
  _value = val;
  _colSpan = colSpan;
  _rowSpan = rowSpan;
}


CSpreadsheetCell::CSpreadsheetCell( const CSpreadsheetCell& other ) {
  assign( other );
}


CSpreadsheetCell& CSpreadsheetCell::operator=( const CSpreadsheetCell& other ) {
  assign( other );

  return *this;
}


CSpreadsheetCell::~CSpreadsheetCell() {
  // Do nothing else
}


void CSpreadsheetCell::assign( const CSpreadsheetCell& other ) {
  _value = other._value;
  _colSpan = other._colSpan;
  _rowSpan = other._rowSpan;
}


const QXlsx::CellRange CSpreadsheetCell::mergedRange( const int col, const int row ) const {
  QXlsx::CellRange result;

  result.setFirstColumn( col + 1 );
  result.setLastColumn( col + _colSpan );

  result.setFirstRow( row + 1 );
  result.setLastRow( row + _rowSpan );

  return result;
}


CSpreadsheetCellArray::CSpreadsheetCellArray() : CTwoDArray<CSpreadsheetCell>() {
  // Do nothing else
}


CSpreadsheetCellArray::CSpreadsheetCellArray( const int nCols, const int nRows ) : CTwoDArray<CSpreadsheetCell>( nCols, nRows ) {
  // Do nothing else
}


CSpreadsheetCellArray::CSpreadsheetCellArray( const int nCols, const int nRows, const QVariant defaultVal ) : CTwoDArray<CSpreadsheetCell>( nCols, nRows ) {
  for( int c = 0; c < nCols; ++c ) {
    for( int r = 0; r < nRows; ++r ) {
      this->at( c, r ).setValue( defaultVal );
    }
  }
}


CSpreadsheetCellArray::CSpreadsheetCellArray( const int nCols, const int nRows, const CSpreadsheetCell defaultVal ) : CTwoDArray<CSpreadsheetCell>( nCols, nRows, defaultVal ) {
  // Do nothing else
}


CSpreadsheetCellArray::~CSpreadsheetCellArray() {
  // Do nothing else
}


void CSpreadsheetCellArray::debug( const int padding /* = 10 */) const {
  qDebug() << QString( "Matrix %1 cols x %2 rows:" ).arg( nCols() ).arg( nRows() );

  for( int r = 0; r < this->nRows(); ++r ) {
    QString str = QString( "  Row %1: " ).arg( r );

    for( int c = 0; c < this->nCols(); ++c ) {
      str.append( rightPaddedStr( QString( "%1" ).arg( this->at( c, r ).value().toString() ), padding ) );
    }

    qDebug() << str;
  }

  qDebug();
}



bool CSpreadsheetCellArray::writeXlsx( const QString& fileName ) {
  QXlsx::Document xlsx;

  QXlsx::Format format;
  format.setHorizontalAlignment( QXlsx::Format::AlignLeft );
  format.setVerticalAlignment( QXlsx::Format::AlignTop );

  for( int c = 0; c < this->nCols(); ++c ) {
    for( int r = 0; r < this->nRows(); ++r ) {
      xlsx.write( r+1, c+1, this->value( c, r ).value() );

      if( this->value(c, r).hasSpan() ) {
        xlsx.mergeCells( this->value(c, r).mergedRange(c, r), format );
      }
    }
  }

  return xlsx.saveAs( fileName );
}


bool CSpreadsheetCellArray::isXlsDate(const int xf, const double d ) {
  bool result;
  int fmt = _xlsXFs.value( xf );

  // Check for built-in date formats
  if(
    ((14 <= fmt) && (17 >= fmt)) // Default date formats: see FORMAT (p. 174) in http://www.openoffice.org/sc/excelfileformat.pdf
    || ((27 <= fmt) && (36 >= fmt)) // Special default Japanese date formats: see FORMAT (p. 175) in http://www.openoffice.org/sc/excelfileformat.pdf
    || ((50 <= fmt) && (58 >= fmt)) // More special default Japanese date formats: see FORMAT (p. 175) in http://www.openoffice.org/sc/excelfileformat.pdf
  ) {
    result = true;
  }
  else {
    bool looksLikeDate, looksLikeTime;

    double wholeNumberPart = ::floor( d );
    bool isRemainder = !qFuzzyCompare( (0.0 + 1.0), ( d - wholeNumberPart + 1.0 ) );

    if( isRemainder ) {
      result = false;
    }
    else {
      QString fmtStr = _xlsFormats.value( fmt );

      looksLikeDate = (
        fmtStr.contains( "yy" )
        || fmtStr.contains( "dd" )
      );

      looksLikeTime = (
        fmtStr.contains( "AM/PM" )
        || fmtStr.contains( "h" )
        || fmtStr.contains( "s" )
      );

      result = ( looksLikeDate && !looksLikeTime );
    }
  }

  return result;
}


bool CSpreadsheetCellArray::isXlsTime( const int xf, const double d ) {
  bool result;
  int fmt = _xlsXFs.value( xf );

  // Check for built-in date formats
  if( (18 <= fmt) && (21 >= fmt) ) { // Default time formats: see FORMAT (p. 174) in http://www.openoffice.org/sc/excelfileformat.pdf
    result = true;
  }
  else {
    bool looksLikeDate, looksLikeTime;

    QString fmtStr = _xlsFormats.value( fmt );

    looksLikeDate = (
      ( fmtStr.contains( "yy" ) || fmtStr.contains( "dd" ) )
    );

    looksLikeTime = (
      ( fmtStr.contains( "AM/PM" ) || fmtStr.contains( "h" ) || fmtStr.contains( "s" ) )
      && ( 1.0 > d )
    );

    result = ( !looksLikeDate && looksLikeTime );
  }

  return result;
}


bool CSpreadsheetCellArray::isXlsDateTime(const int xf, const double d ) {
  bool result;
  int fmt = _xlsXFs.value( xf );

  // Check for built-in date formats
  if( 22 == fmt ) { // Default date/time format: see FORMAT (p. 174) in http://www.openoffice.org/sc/excelfileformat.pdf
    result = true;
  }
  else {
    bool looksLikeDate, looksLikeTime;

    double wholeNumberPart = ::floor( d );
    bool isRemainder = !qFuzzyCompare( (0.0 + 1.0), ( d - wholeNumberPart + 1.0 ) );

    if( ( 1.0 < d ) && isRemainder ) {
      result = true;
    }
    else {
      QString fmtStr = _xlsFormats.value( fmt );

      looksLikeDate = (
        fmtStr.contains( "yy" )
        || fmtStr.contains( "dd" )
      );

      looksLikeTime = (
        fmtStr.contains( "AM/PM" )
        || fmtStr.contains( "h" )
        || fmtStr.contains( "s" )
      );

      result = ( looksLikeDate && looksLikeTime );
    }
  }

  return result;
}


QDate CSpreadsheetCellArray::xlsDate( const int val ) {
  int startDate;
  if( _xlsIs1904 )
    startDate = QDate( 1904, 1, 1 ).toJulianDay() - 1;
  else
    startDate = QDate( 1900, 1, 1 ).toJulianDay() - 1;

  return QDate::fromJulianDay( startDate + val - 1 );
}


QTime CSpreadsheetCellArray::xlsTime( const double d ) {
  QTime result( 0, 0, 0, 0 );

  double ms = d * 24.0 * 60.0 * 60.0 * 1000.0;

  return result.addMSecs( ms );
}


QDateTime CSpreadsheetCellArray::xlsDateTime( const double d ) {
  double val = ::floor( d );
  QDate date = xlsDate( int( val ) );
  QTime time = xlsTime( d - val );

  QDateTime result;

  result.setDate( date );
  result.setTime( time );

  return result;
}


bool CSpreadsheetCellArray::readXls( const QString& fileName, const bool displayVerboseOutput /* = false */ ) {
  // Open workbook, choose standard conversion
  //==========================================
  QString encoding = "UTF-8"; // "iso-8859-15//TRANSLIT" UTF-8 seems to be the new standard.
  xls::xlsWorkBook* pWB = xls::xls_open( fileName.toLatin1().data(), encoding.toLatin1().data() );

  if( NULL == pWB ) {
    qDebug() << "Specified file could not be opened.  Wrong format?";
    return false;
  }

  // Generate some helper bits that will let us determine further down which cells contain dates, times, or date/times
  //------------------------------------------------------------------------------------------------------------------
  _xlsIs1904 = ( 1 == pWB->is1904 );

  _xlsFormats.clear();
  for( unsigned int i = 0; i < pWB->formats.count; ++i ) {
    _xlsFormats.insert( pWB->formats.format[i].index, pWB->formats.format[i].value );

    if( displayVerboseOutput )
      cout << "Format: " << "i: " << i << ", idx: " << pWB->formats.format[i].index << ", val: " << pWB->formats.format[i].value << endl;
  }
  if( displayVerboseOutput )
    cout << endl;

  _xlsXFs.clear();
  for( unsigned int i = 0; i < pWB->xfs.count; ++i ) {
    if( 0 != pWB->xfs.xf[i].format ) {
      _xlsXFs.insert( i, pWB->xfs.xf[i].format );

      if( displayVerboseOutput )
        cout << "XFs: " << "i: " << i << ", format: " << pWB->xfs.xf[i].format << ", type: " << pWB->xfs.xf[i].type << endl;
    }
  }
  if( displayVerboseOutput )
    cout << endl;

  //qDebug() << "Sheet names:";
  //for( int i = 0; i < pWB->sheets.count; ++i ) {
  //  qDebug() << pWB->sheets.sheet[i].name;
  //}
  //qDebug();


  // Open and parse the sheet
  //=========================
  xls::xlsWorkSheet* pWS = xls::xls_getWorkSheet( pWB, 0 );
  xls::xls_parseWorkSheet( pWS );


  // Process all cells of the sheet
  //===============================
  WORD cellRow, cellCol;

  //qDebug() << "cols:" <<  pWS->rows.lastcol << ", rows:" << pWS->rows.lastrow;
  this->setSize( pWS->rows.lastcol, pWS->rows.lastrow + 1, CSpreadsheetCell() );

  for( cellRow=0; cellRow <= pWS->rows.lastrow; ++cellRow ) {
    for( cellCol=0; cellCol < pWS->rows.lastcol; ++cellCol ) {
      xls::xlsCell* cell = xls::xls_cell( pWS, cellRow, cellCol );

      if( !cell || cell->isHidden ) {
        continue;
      }
      else {
        QString msg;
        QVariant val;

        // Display the value of the cell (either numeric or string)
        //========================================================

        // Deal with numbers
        //------------------
        if( displayVerboseOutput ) {
          cout <<
            QString( "Row: %1, Col: %2, id: %3, row: %4, col, %5, xf: %6, l: %7, d: %8, str: %9" )
            .arg( cellRow )
            .arg( cellCol )
            .arg( QString( "%1" ).arg( cell->id, 0, 16 ).toUpper().prepend( "0x" ) )
            .arg( cell->row )
            .arg( cell->col )
            .arg( cell->xf )
            .arg( cell->l )
            .arg( cell->d )
            .arg( (char*)cell->str ) << endl
          ;
        }

        // XLS_RECORD_RK = 0x027E
        // XLS_RECORD_NUMBER = 0x203
        // XLS_RECORD_MULRK = 0x00BD
        if( ( XLS_RECORD_RK == cell->id ) || ( XLS_RECORD_MULRK == cell->id ) || ( XLS_RECORD_NUMBER == cell->id ) ) {
          if( isXlsDate( cell->xf, cell->d ) ) {
            val = xlsDate( (int)cell->d );
          }
          else if( isXlsTime( cell->xf, cell->d ) ) {
            val = xlsTime( cell->d );
          }
          else if( isXlsDateTime( cell->xf, cell->d ) ) {
            val = xlsDateTime( cell->d );
          }
          else {
            val = cell->d;
          }
          msg = QString( "Row: %1, Col: %2, Value (numeric): %3, ID: %4" ).arg( cellRow ).arg( cellCol ).arg( val.toString() ).arg( QString( "%1" ).arg( cell->id, 0, 16 ).toUpper().prepend( "0x" ) );
        }

        // Deal with formulas
        //-------------------
        // XLS_RECORD_FORMULA = 0x0006
        // XLS_RECORD_FORMULA_ALT = 0x0406
        else if ( (XLS_RECORD_FORMULA == cell->id) || (XLS_RECORD_FORMULA_ALT == cell->id) ) {
          if (cell->l == 0) { // its a number
            val = cell->d;
            msg = QString( "Row: %1, Col: %2, Value (formula numeric): %3, ID: %4" ).arg( cellRow ).arg( cellCol ).arg( val.toString() ).arg( QString( "%1" ).arg( cell->id, 0, 16 ).toUpper().prepend( "0x" ) );
          }
          else {
            if (!strcmp((char *)cell->str, "bool")) { // its boolean, and test cell->d
              if( 1 == (int)cell->d ) {
                val = true;
              }
              else {
                val = false;
              }
              msg = QString( "Row: %1, Col: %2, Value (formula boolean): %3, ID: %4" ).arg( cellRow ).arg( cellCol ).arg( val.toString() ).arg( QString( "%1" ).arg( cell->id, 0, 16 ).toUpper().prepend( "0x" ) );
            }
            else if (!strcmp((char *)cell->str, "error")) { // formula is in error
              val = "*error*";
              msg = QString( "Row: %1, Col: %2, Value (formula error): %3, ID: %4" ).arg( cellRow ).arg( cellCol ).arg( val.toString() ).arg( QString( "%1" ).arg( cell->id, 0, 16 ).toUpper().prepend( "0x" ) );
            }
            else { // ... cell->str is valid as the result of a string formula.
              val = QString( "%1" ).arg( (char*)cell->str );
              msg = QString( "Row: %1, Col: %2, Value (formula string): %3" ).arg( cellRow ).arg( cellCol ).arg( val.toString() );
            }
          }
        }

        // Deal with booleans
        //-------------------
        // XLS_RECORD_BOOLERR = 0x0205
        else if( XLS_RECORD_BOOLERR == cell->id ) {
          if( 0 == (int)cell->d ) {
            val = false;
          }
          else {
            val = true;
          }
        }


        // Deal with strings
        //------------------
        else if( NULL != cell->str ) {
           val = QString( "%1" ).arg( (char*)cell->str );
           msg = QString( "Row: %1, Col: %2, Value (string): %3" ).arg( cellRow ).arg( cellCol ).arg( val.toString() );
        }

        // Deal with 'empty' cells
        //------------------------
        else {
          msg = QString( "Row: %1, Col: %2, (Empty cell)" ).arg( cellRow ).arg( cellCol );
        }

        msg.append( QString( ", colspan: %1, rowspan %2" ).arg( cell->colspan ).arg( cell->rowspan ) );
        this->setValue( cellCol, cellRow, CSpreadsheetCell( val, cell->colspan, cell->rowspan ) );

        if( displayVerboseOutput ) {
          cout << msg << endl;
        }
      }
    }
  }

  xls_close( pWB );

  return true;
}
