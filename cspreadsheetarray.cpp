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
#include <ar_general_purpose/filemagic.h>

typedef uint16_t xlsWORD;

CSpreadsheetCell::CSpreadsheetCell() {
  // _value is initialized by default
  _colSpan = 1;
  _rowSpan = 1;
  _isPartOfMergedRow = false;
  _isPartOfMergedCol = false;
  _originCell = nullptr;
}


CSpreadsheetCell::CSpreadsheetCell( const QVariant val ) {
  _value = val;
  _colSpan = 1;
  _rowSpan = 1;
  _isPartOfMergedRow = false;
  _isPartOfMergedCol = false;
  _originCell = nullptr;
}


CSpreadsheetCell::CSpreadsheetCell( const QVariant val, const int colSpan, const int rowSpan ) {
  _value = val;
  _colSpan = qMax( colSpan, 1 );
  _rowSpan = qMax( rowSpan, 1 );
  _isPartOfMergedRow = ( 1 < _colSpan );
  _isPartOfMergedCol = ( 1 < _rowSpan );
  _originCell = nullptr;
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
  _isPartOfMergedRow = other._isPartOfMergedRow;
  _isPartOfMergedCol = other._isPartOfMergedCol;
  _originCell = other._originCell;

  // FIXME: Is this right?
  _linkedCells = other._linkedCells;
}


const QXlsx::CellRange CSpreadsheetCell::mergedRange( const int col, const int row ) const {
  QXlsx::CellRange result;

  result.setFirstColumn( col + 1 );
  result.setLastColumn( col + _colSpan );

  result.setFirstRow( row + 1 );
  result.setLastRow( row + _rowSpan );

  return result;
}

void CSpreadsheetCell::debug() {
  QString originStr;

  if( nullptr != this->_originCell ) {
    originStr = this->_originCell->value().toString();
  }

  qDebug() /*<< "C" << c << "R" << r*/ << QString::number( qlonglong( this ), 16 )
           << "MergeC" << this->isPartOfMergedCol() << "MergeR" << this->isPartOfMergedRow()
           << "ColSpan" << this->colSpan() << "RowSpan" << this->rowSpan()
           << "Value" << this->value().toString()
           << "nLinked" << this->_linkedCells.count()
           << "OrigC" << QString::number( qlonglong( this->_originCell ), 16 ) << "OrigCVal" << originStr;
}


CSpreadsheet::CSpreadsheet() : CTwoDArray<CSpreadsheetCell>() {
  initialize();
}


CSpreadsheet::CSpreadsheet( class CSpreadsheetWorkBook* wb ) : CTwoDArray<CSpreadsheetCell>() {
  initialize();
  _wb = wb;
}


CSpreadsheet::CSpreadsheet( const int nCols, const int nRows ) : CTwoDArray<CSpreadsheetCell>( nCols, nRows ) {
  initialize();
}


CSpreadsheet::CSpreadsheet( const int nCols, const int nRows, const QVariant defaultVal ) : CTwoDArray<CSpreadsheetCell>( nCols, nRows ) {
  initialize();

  for( int c = 0; c < nCols; ++c ) {
    for( int r = 0; r < nRows; ++r ) {
      this->at( c, r ).setValue( defaultVal );
    }
  }
}


CSpreadsheet::CSpreadsheet( const int nCols, const int nRows, const CSpreadsheetCell defaultVal ) : CTwoDArray<CSpreadsheetCell>( nCols, nRows, defaultVal ) {
  initialize();
}


void CSpreadsheet::initialize() {
  _wb = nullptr;
  _hasSpannedCells = false;
}


CSpreadsheet::~CSpreadsheet() {
  // Do nothing else
}


CSpreadsheet::CSpreadsheet( const CSpreadsheet& other ) : CTwoDArray<CSpreadsheetCell>( other ) {
  assign( other );
}


CSpreadsheet& CSpreadsheet::operator=( const CSpreadsheet& other ) {
  CTwoDArray<CSpreadsheetCell>::operator= ( other );

  assign( other );

  return *this;
}


void CSpreadsheet::assign( const CSpreadsheet& other ) {
  _wb = other._wb;
  _hasSpannedCells = other._hasSpannedCells;
}


void CSpreadsheet::debug( const int padding /* = 10 */) const {
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


QVariant CSpreadsheet::cellValue(const QString& cellLabel ) const {
  QString col, row;
  int c, r;

  if( cellLabel.at(1).isLetter() ) {
    col = cellLabel.left( 2 ).toUpper();
    row = cellLabel.right( cellLabel.length() - 2 );
  }
  else {
    col = cellLabel.left( 1 ).toUpper();
    row = cellLabel.right( cellLabel.length() - 1 );
  }

  r = row.toInt() - 1;

  // Remember: A = 65, Z = 90
  // This class expects columns to be 0-indexed, so column A is column 0.
  if( 1 == col.length() ) {
    c = col.at(0).unicode() - 65;
  }
  else {
    c = ( 26 * (col.at(0).unicode() - 64 ) ) + ( col.at(1).unicode() - 65 );
  }

  return this->cellValue( c, r );
}


bool CSpreadsheet::compareCellValue( const int c, const int r, const QString& str, Qt::CaseSensitivity caseSens /* = Qt::CaseInsensitive */ ) {
  return( 0 == this->cellValue( c, r ).toString().trimmed().compare( str, caseSens ) );
}

bool CSpreadsheet::compareCellValue( const QString& cellLabel, const QString& str, Qt::CaseSensitivity caseSens /* = Qt::CaseInsensitive */ ) {
  return( 0 == this->cellValue( cellLabel ).toString().trimmed().compare( str, caseSens ) );
}


bool CSpreadsheet::isTidy( const bool containsHeaderRow ) {
  bool result = true;

  // Criteria for tidy spreadsheets:
  //  1. No merged cells.
  //  2. If the first row is a header:
  //    a) It should not contain any blanks (blanks at the end are OK)
  //    b) Every value in the first row should be a string(?)
  //    c) For all subsequent rows, there cannot be more columns than in the header row.

  if( this->_hasSpannedCells ) {
    result = false;
  }
  else if( containsHeaderRow ) {
    // Check the header row
    //---------------------
    QStringList headers;
    for( int c = 0; c < this->nCols(); ++c ) {
      headers.append( this->at( c, 0 ).value().toString() );
    }

    bool ok = false;
    while( !ok ) {
      if( 0 < headers.last().trimmed().length() ) {
        ok = true;
      }
      else {
        headers.removeLast();
      }
    }

    for( int c = 0; c < headers.length(); ++c ) {
      if(
         this->at( c, 0 ).value().isNull()
         || ( QVariant::String != this->at( c, 0 ).value().type() )
         || ( 0 == this->at( c, 0 ).value().toString().length() )
       ) {
        result = false;
      }
    }

    if( false == result ) {
      return result;
    }

    // Check all subsequent rows
    //--------------------------
    for( int r = 1; r < this->nRows(); ++r ) {
      QStringList data;
      for( int c = 0; c < this->nCols(); ++c ) {
        data.append( this->at( c, r ).value().toString() );
      }

      bool ok = false;
      while( !ok && !data.isEmpty() ) {
        if( 0 < data.last().trimmed().length() ) {
          ok = true;
        }
        else {
          data.removeLast();
        }
      }

      if( data.length() > headers.length() ) {
        result = false;
        break;
      }

    }
  }

  return result;
}


QStringList CSpreadsheet::rowAsStringList( const int rowNumber ) {
  QStringList list;

  for( int c = 0; c < this->nCols(); ++c ) {
    if( QVariant::DateTime == this->at( c, rowNumber ).value().type() )
      list.append( this->at( c, rowNumber).value().toDateTime().toString( "yyyy-MM-dd hh:mm:ss" ) );
    else
      list.append( this->at( c, rowNumber).value().toString().trimmed() );
  }

  return list;
}


QCsv CSpreadsheet::asCsv( const bool containsHeaderRow, const QChar delimiter /* = ',' */ ) {
  QCsv csv;

  if( this->isEmpty() ) {
    qDebug() << "is empty";
    csv.setError( QCsv::ERROR_OTHER, "Specified worksheet is empty." );
  }
  else if( !this->isTidy( containsHeaderRow ) ) {
    qDebug() << "Not tidy";
    csv.setError( QCsv::ERROR_OTHER, "Specified worksheet does not have a tidy CSV format." );
  }
  else {
    QStringList firstRow;
    QList<QStringList> data;

    firstRow = this->rowAsStringList( 0 );

    bool ok = false;
    while( !ok ) {
      if( 0 < firstRow.last().trimmed().length() ) {
        ok = true;
      }
      else {
        firstRow.removeLast();
      }
    }

    for( int i = 1; i < this->nRows(); ++i ) {
      QStringList tmp = this->rowAsStringList( i );
      data.append( tmp.mid( 0, firstRow.length() ) );
    }

    if( !containsHeaderRow ) {
      data.prepend( firstRow );
    }

    // Trim off trailing empty rows
    while( isEmptyStringList( data.last() ) ) {
      data.removeLast();
    }

    if( containsHeaderRow ) {
      csv = QCsv( firstRow, data );
    }
    else {
      csv = QCsv( data );
    }

    csv.open();
  }

  csv.setMode( QCsv::EntireFile );
  csv.setDelimiter( delimiter );
  csv.setCheckForComments( false );
  csv.setStringsContainDelimiters( true );

  if( QCsv::ERROR_NONE == csv.error() ) {
    csv.toFront();
  }

  return csv;
}


bool CSpreadsheet::writeXlsx( const QString& fileName ) {
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


QDate CSpreadsheet::xlsDate( const int val, const bool is1904DateSystem ) {
  QDate result;

  if( is1904DateSystem ) { // Treat the date with the 1904 date system.
    result = QDate( 1904, 1, 1 ).addDays( val );
  }
  else {
    result = QDate( 1899, 12, 31 );

    // There is an intentional leap-year bug in Excel. It thinks that 1900 was a leap year.
    // It wasn't, but the bug was introduced to maintain compatibility with Lotus 1-2-3.
    // Any date beyond Feb 28 1900 will be off by a day, if we just add the number of days.
    if( val < 60 )
      result = result.addDays( val );
    else
      result = result.addDays( val - 1 );
  }

  return result;
}


QTime CSpreadsheet::xlsTime( const double d ) {
  QTime result( 0, 0, 0, 0 );

  double s =  d * 24.0 * 60.0 * 60.0;

  result = result.addSecs( int( s ) );

  int seconds = result.second();
  if( seconds < 30 )
    result = result.addSecs( -1 * seconds );
  else
    result = result.addSecs( 60 - seconds );

  return result;
}


QDateTime CSpreadsheet::xlsDateTime( const double d, const bool is1904DateSystem ) {
  double val = ::floor( d );
  QDate date = xlsDate( int( val ), is1904DateSystem );
  QTime time = xlsTime( d - val );

  QDateTime result;

  result.setTimeSpec( Qt::UTC );
  result.setDate( date );
  result.setTime( time );

  return result;
}


bool CSpreadsheet::readXlsx(const QString& sheetName, QXlsx::Document* xlsx, const bool displayVerboseOutput /* = false */ ) {
  if( !xlsx->selectSheet( sheetName ) ) {
    if( displayVerboseOutput )
      cout << QString( "Specified worksheet (%1) could not be selected." ).arg( sheetName ) << endl;
    return false;
  }
  if( displayVerboseOutput )
    cout << "Worksheet is open." << endl;

  QXlsx::CellRange cellRange = xlsx->dimension();
  if( (0 >= cellRange.firstRow()) || (0 >= cellRange.firstColumn()) || (0 >= cellRange.lastRow()) || (0 >= cellRange.lastColumn()) ) {
    cout << "Cell range is out of bounds." << endl;
    return false;
  }
  if( displayVerboseOutput )
    cout << QString( "Cell range: rows( %1, %2 ), columns (%3, %4)" ).arg( cellRange.firstRow() ).arg( cellRange.lastRow() ).arg( cellRange.firstColumn() ).arg( cellRange.lastColumn() ) << endl;

  this->setSize( cellRange.lastColumn(), cellRange.lastRow(), CSpreadsheetCell() );

  // FIXME: This does not account for merged cells.
  for( int row = 1; row < (cellRange.lastRow() + 1); ++row ) {
    for( int col = 1; col < (cellRange.lastColumn() + 1); ++col ) {

      QVariant val = xlsx->read( row, col );
      if( val.type() == QVariant::String ) {
        val = val.toString().replace( "_x000D_\n", "\n" );
      }

      CSpreadsheetCell ssCell( val, 0, 0 );
      this->setValue( col - 1, row - 1, ssCell );
    }
  }

  if( _hasSpannedCells ) {
    flagMergedCells();
  }

  return true;
}


bool CSpreadsheet::readXls( const int sheetIdx, xls::xlsWorkBook* pWB, const bool displayVerboseOutput /* = false */ ) {
  // Open and parse the sheet
  //=========================
  xls::xlsWorkSheet* pWS = xls::xls_getWorkSheet( pWB, sheetIdx );
  xls::xls_parseWorkSheet( pWS );

  // Process all cells of the sheet
  //===============================
  xlsWORD cellRow, cellCol;

  this->setSize( pWS->rows.lastcol, pWS->rows.lastrow + 1, CSpreadsheetCell() );

  for( cellRow=0; cellRow <= pWS->rows.lastrow; ++cellRow ) {
    for( cellCol=0; cellCol < pWS->rows.lastcol; ++cellCol ) {

      xls::xlsCell* cell = xls::xls_cell( pWS, cellRow, cellCol );

      if( !cell ) {
        //qDebug() << cellRow << cellCol << "Cell not read";
        continue;
      }
      else if( cell->isHidden ) {
        //qDebug() << cellRow << cellCol << "Hidden cell";
        continue;
      }
      else {
        //qDebug() << "In this loop...";
        QString msg;

        QVariant val = processCellXls( cell, msg, _wb );

        CSpreadsheetCell ssCell( val, cell->colspan, cell->rowspan );
        this->setValue( cellCol, cellRow, ssCell );

        // Make a note if the cell is merged.
        _hasSpannedCells = ( _hasSpannedCells || ssCell.hasSpan() );

        if( displayVerboseOutput ) {
          msg.replace( "CELLCOL", QString::number( cellCol ), Qt::CaseSensitive );
          msg.replace( "CELLROW", QString::number( cellRow ), Qt::CaseSensitive );
          cout << msg << endl;
        }
      }
    }
  }

  if( _hasSpannedCells ) {
    flagMergedCells();
  }

  return true;
}


QVariant CSpreadsheet::processCellXls( xls::xlsCell* cell, QString& msg, CSpreadsheetWorkBook* wb ) {
  QVariant val;

  // Display the value of the cell (either numeric or string)
  //=========================================================

  // Deal with numbers
  //------------------
  msg =
    QString( "Row: CELLROW, Col: CELLCOL, id: %1, row: %2, col, %3, xf: %4, l: %5, d: %6, str: %7\n" )
    .arg( QString( "%1" ).arg( cell->id, 0, 16 ).toUpper().prepend( "0x" ) )
    .arg( cell->row )
    .arg( cell->col )
    .arg( cell->xf )
    .arg( cell->l )
    .arg( cell->d )
    .arg( (char*)cell->str )
  ;

  // XLS_RECORD_RK = 0x027E
  // XLS_RECORD_NUMBER = 0x203
  // XLS_RECORD_MULRK = 0x00BD
  if( ( XLS_RECORD_RK == cell->id ) || ( XLS_RECORD_MULRK == cell->id ) || ( XLS_RECORD_NUMBER == cell->id ) ) {
    if( wb->isXlsDate( cell->xf, cell->d ) ) {
      val = xlsDate( (int)cell->d, wb->isXls1904DateSystem() );
    }
    else if( wb->isXlsTime( cell->xf, cell->d ) ) {
      val = xlsTime( cell->d );
    }
    else if( wb->isXlsDateTime( cell->xf, cell->d ) ) {
      val = xlsDateTime( cell->d, wb->isXls1904DateSystem() );
    }
    else {
      val = cell->d;
    }
    msg.append( QString( "Row: CELLROW, Col: CELLCOL, Value (numeric): %1, ID: %2" ).arg( val.toString() ).arg( QString( "%1" ).arg( cell->id, 0, 16 ).toUpper().prepend( "0x" ) ) );
  }

  // Deal with formulas
  //-------------------
  // XLS_RECORD_FORMULA = 0x0006
  // XLS_RECORD_FORMULA_ALT = 0x0406
  else if ( (XLS_RECORD_FORMULA == cell->id) || (XLS_RECORD_FORMULA_ALT == cell->id) ) {
    if (cell->l == 0) { // its a number
      val = cell->d;
      msg.append( QString( "Row: CELLROW, Col: CELLCOL, Value (formula numeric): %1, ID: %2" ).arg( val.toString() ).arg( QString( "%1" ).arg( cell->id, 0, 16 ).toUpper().prepend( "0x" ) ) );
    }
    else {
      if (!strcmp((char *)cell->str, "bool")) { // its boolean, and test cell->d
        if( 1 == (int)cell->d ) {
          val = true;
        }
        else {
          val = false;
        }
        msg.append( QString( "Row: CELLROW, Col: CELLCOL, Value (formula boolean): %1, ID: %2" ).arg( val.toString() ).arg( QString( "%1" ).arg( cell->id, 0, 16 ).toUpper().prepend( "0x" ) ) );
      }
      else if (!strcmp((char *)cell->str, "error")) { // formula is in error
        val = "*error*";
        msg.append( QString( "Row: CELLROW, Col: CELLCOL, Value (formula error): %1, ID: %2" ).arg( val.toString() ).arg( QString( "%1" ).arg( cell->id, 0, 16 ).toUpper().prepend( "0x" ) ) );
      }
      else { // ... cell->str is valid as the result of a string formula.
        val = QString( "%1" ).arg( (char*)cell->str );
        msg.append( QString( "Row: CELLROW, Col: CELLCOL, Value (formula string): %1" ).arg( val.toString() ) );
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
  else if( nullptr != cell->str ) {
     val = QString( "%1" ).arg( (char*)cell->str );
     msg.append( QString( "Row: CELLROW, Col: CELLCOL, Value (string): %1" ).arg( val.toString() ) );
  }

  // Deal with 'empty' cells
  //------------------------
  else {
    msg.append( QString( "Row: CELLROW, Col: CELLCOL, (Empty cell)" ) );
  }

  msg.append( QString( ", colspan: %1, rowspan %2" ).arg( cell->colspan ).arg( cell->rowspan ) );

  return val;
}



void CSpreadsheet::flagMergedCells() {
  // Cells that span multiple columns are part of a merged ROW.
  // Cells that span multiple rows are part of a merged COLUMN.

  int firstCol, lastCol, firstRow, lastRow;

  for( int c = 0; c < this->nCols(); ++c ) {
    for( int r = 0; r < this->nRows(); ++r ) {

      if( this->cell(c, r).hasSpan() ) {
        firstCol = c;
        lastCol = firstCol + this->cell( c, r ).colSpan();
        firstRow = r;
        lastRow = firstRow + this->cell( c, r ).rowSpan();

        for( int cc = firstCol; cc < lastCol; ++cc ) {
          for( int rr = firstRow; rr < lastRow; ++rr ) {
            if( this->cell(c, r).hasColSpan() ) {
              this->at( cc, rr )._isPartOfMergedRow = true;
              this->at( cc, rr )._originCell = &(this->at( c, r ));

              // Add this cell to _originCell's collection
              this->at( cc, rr )._originCell->_linkedCells.insert( &(this->at( cc, rr ) ) );
            }

            if( this->cell(c, r).hasRowSpan() ) {
              this->at( cc, rr )._isPartOfMergedCol = true;
              this->at( cc, rr )._originCell = &(this->at( c, r ));

              // Add this cell to _originCell's collection
              this->at( cc, rr )._originCell->_linkedCells.insert( &(this->at( cc, rr ) ) );
            }
          }
        }

      }

      if( this->at( c, r )._originCell == &(this->at( c, r ) ) ) {
        // Remove this cell from _originCell's collection
        this->at( c, r )._originCell->_linkedCells.remove( &(this->at( c, r ) ) );
        this->at( c, r )._originCell = nullptr;
      }

    }
  }
}


void CSpreadsheet::debugMerges() {
  for( int c = 0; c < this->nCols(); ++c ) {
    for( int r = 0; r < this->nRows(); ++r ) {
      QString originStr;

      if( nullptr != this->at( c, r )._originCell ) {
        originStr = this->at( c, r )._originCell->value().toString();
      }

      qDebug() << "C" << c << "R" << r << QString::number( qlonglong( &(this->at( c, r ) ) ), 16 )
               << "MergeC" << this->at( c, r ).isPartOfMergedCol() << "MergeR" << this->at( c, r ).isPartOfMergedRow()
               << "ColSpan" << this->at( c, r ).colSpan() << "RowSpan" << this->at( c, r ).rowSpan()
               << "Value" << this->at( c, r ).value().toString()
               << "nLinked" << this->at( c, r )._linkedCells.count()
               << "OrigC" << QString::number( qlonglong( this->at( c, r )._originCell ), 16 ) << "OrigCVal" << originStr;
    }
  }
}


void CSpreadsheet::unmergeRows( const bool duplicateValues ) {
  int firstCol, lastCol, firstRow, lastRow;

  // Look for cells that are SPAN MULTIPLE ROWS, and duplicate their values across all columns.
  for( int c = 0; c < this->nCols(); ++c ) {
    for( int r = 0; r < this->nRows(); ++r ) {

      if( this->cell(c, r).hasSpan() ) {
        firstCol = c;
        lastCol = firstCol + this->cell( c, r ).colSpan();
        firstRow = r;
        lastRow = firstRow + this->cell( c, r ).rowSpan();

        if( this->cell(c, r).hasColSpan() ) {
          //qDebug() << "Unmerging row at" << c << r;
          //this->at( c, r ).debug();

          for( int cc = firstCol; cc < lastCol; ++cc ) {
            for( int rr = firstRow; rr < lastRow; ++rr ) {
              //qDebug() << "cc rr" << cc << rr;

              if( c != cc ) {
                if( duplicateValues )
                  this->at( cc, r )._value = this->at( c, r ).value();
                else
                  this->at( cc, r )._value = QVariant();
              }

              this->at( cc, r )._rowSpan = this->at( c, r ).rowSpan(); // Should be same rowspan as parent row
              this->at( cc, rr )._colSpan = 1;
              this->at( cc, rr )._isPartOfMergedRow = false;

              if( rr == firstRow ) {
                // Remove this cell from _originCell's collection
                if( nullptr != this->at( cc, rr )._originCell ) {
                  this->at( cc, rr )._originCell->_linkedCells.remove( &(this->at( cc, rr ) ) );
                  this->at( cc, rr )._originCell = nullptr;
                }
              }
              else {
                this->at( cc, rr )._originCell = &(this->at( cc, r ));
                // Add this cell to _originCell's collection
                this->at( cc, rr )._originCell->_linkedCells.insert( &(this->at( cc, rr )) );
              }
            }
          }
        }
      }

    }
  }
}



void CSpreadsheet::unmergeColumns( const bool duplicateValues ) {
  int firstCol, lastCol, firstRow, lastRow;

  // Look for cells that are SPAN MULTIPLE ROWS, and duplicate their values across all columns.
  for( int c = 0; c < this->nCols(); ++c ) {
    for( int r = 0; r < this->nRows(); ++r ) {

      if( this->cell(c, r).hasSpan() ) {
        firstCol = c;
        lastCol = firstCol + this->cell( c, r ).colSpan();
        firstRow = r;
        lastRow = firstRow + this->cell( c, r ).rowSpan();

        if( this->cell(c, r).hasRowSpan() ) {
          for( int cc = firstCol; cc < lastCol; ++cc ) {
            for( int rr = firstRow; rr < lastRow; ++rr ) {

              if( rr != r ) {
                if( duplicateValues )
                  this->at( c, rr )._value = this->at( c, r ).value();
                else
                  this->at( c, rr )._value = QVariant();
              }

              this->at( c, rr )._colSpan = this->at( c, r ).colSpan(); // Should be same colspan as parent row
              this->at( cc, rr )._rowSpan = 1;
              this->at( cc, rr )._isPartOfMergedCol = false;

              if( cc == firstCol ) {
                // Remove this cell from _originCell's collection
                if( nullptr != this->at( cc, rr )._originCell ) {
                  this->at( cc, rr )._originCell->_linkedCells.remove( &(this->at( cc, rr ) ) );
                  this->at( cc, rr )._originCell = nullptr;
                }
              }
              else {
                // Add this cell to _originCell's collection
                this->at( cc, rr )._originCell = &(this->at( c, rr ));
                this->at( cc, rr )._originCell->_linkedCells.insert( &(this->at( cc, rr )) );
              }

            }
          }
        }
      }

    }
  }
}


void CSpreadsheet::unmergeColumnsAndRows( const bool duplicateValues ) {
  unmergeRows( duplicateValues );
  unmergeColumns( duplicateValues );
}


void CSpreadsheet::unmergeCell( const int c, const int r , const bool duplicateValues ) {
  // This should unmerge every linked cell.
  CSpreadsheetCell* parentCell;

  if( nullptr == this->at( c, r )._originCell )
    parentCell = &( this->at( c, r ) );
  else
    parentCell = this->at( c, r )._originCell;

  foreach( CSpreadsheetCell* cell, parentCell->_linkedCells ) {
    if( cell != parentCell ) {
      if( duplicateValues )
        cell->_value = parentCell->value();
      else
        cell->_value = QVariant();
    }

    cell->_isPartOfMergedCol = false;
    cell->_isPartOfMergedRow = false;
    cell->_colSpan = 1;
    cell->_rowSpan = 1;
    cell->_originCell = nullptr;
  }

  parentCell->_isPartOfMergedCol = false;
  parentCell->_isPartOfMergedRow = false;
  parentCell->_colSpan = 1;
  parentCell->_rowSpan = 1;
  parentCell->_linkedCells.clear();
}


void CSpreadsheet::unmergeCellsInRow( const int r, const bool duplicateValues ) {
  for( int c = 0; c < this->nCols(); ++c ) {
    this->unmergeCell( c, r, duplicateValues );
  }
}


bool CSpreadsheet::columnIsEmpty(const int c , const bool excludeHeaderRow /* = false */) {
  bool result = true; // Until shown otherwise.

  int firstRow;
  if( excludeHeaderRow )
    firstRow = 1;
  else
    firstRow = 0;

  for( int r = firstRow; r < this->nRows(); ++r ) {
    QVariant v = this->cellValue( c, r );
    if( !v.isNull() || ( (QVariant::String == v.type()) && !(v.toString().isEmpty()) ) ) {
      result = false;
      break;
    }
  }

  return result;
}


bool CSpreadsheet::rowIsEmpty( const int r ) {
  bool result = true; // Until shown otherwise.

  for( int c = 0; c < this->nCols(); ++c ) {
    QVariant v = this->cellValue( c, r );
    if( !v.isNull() || ( (QVariant::String == v.type()) && !(v.toString().isEmpty()) ) ) {
      result = false;
      break;
    }
  }

  return result;
}


bool CSpreadsheet::hasEmptyColumns(const bool excludeHeaderRow /* = false */ ) {
  bool result = false; // Until shown otherwise.

  for( int c = 0; c < this->nCols(); ++c ) {
    if( this->columnIsEmpty( c, excludeHeaderRow ) ) {
      result = true;
      break;
    }
  }

  return result;
}


bool CSpreadsheet::hasEmptyRows() {
  bool result = false; // Until shown otherwise.

  for( int r = 0; r < this->nRows(); ++r ) {
    if( this->rowIsEmpty( r ) ) {
      result = true;
      break;
    }
  }

  return result;
}


void CSpreadsheet::removeEmptyColumns(const bool excludeHeaderRow /* = false */ ) {
  QList<int> emptyCols;

  for( int c = 0; c < this->nCols(); ++c ) {
    if( this->columnIsEmpty( c, excludeHeaderRow ) ) {
      emptyCols.prepend( c );
    }
  }

  for( int i = 0; i < emptyCols.count(); ++i ) {
    this->removeColumn( emptyCols.at(i) );
  }
}


void CSpreadsheet::removeEmptyRows() {
  QList<int> emptyRows;

  for( int r = 0; r < this->nCols(); ++r ) {
    if( this->rowIsEmpty( r ) ) {
      emptyRows.prepend( r );
    }
  }

  for( int i = 0; i < emptyRows.count(); ++i ) {
    this->removeColumn( emptyRows.at(i) );
  }
}

void CSpreadsheet::appendRow(const QVariantList& values ) {
  QVector<CSpreadsheetCell> cells( values.count() );

  for( int i = 0; i < values.count(); ++i ) {
    cells[i] = CSpreadsheetCell( values.at(i) );
  }

  CTwoDArray::appendRow( cells );
}


void CSpreadsheet::appendRow( const QStringList& values ) {
  QVector<CSpreadsheetCell> cells( values.count() );

  for( int i = 0; i < values.count(); ++i ) {
    cells[i] = CSpreadsheetCell( values.at(i) );
  }

  CTwoDArray::appendRow( cells );
}


CSpreadsheetWorkBook::CSpreadsheetWorkBook( const SpreadsheetFileFormat fileFormat, const QString& fileName, const bool displayVerboseOutput /* = false */ ) {
  openWorkbook( fileFormat, fileName, displayVerboseOutput );
}


CSpreadsheetWorkBook::CSpreadsheetWorkBook( const QString& fileName, const bool displayVerboseOutput /* = false */ ) {
  SpreadsheetFileFormat fileFormat = guessFileFormat( fileName );

  openWorkbook( fileFormat, fileName, displayVerboseOutput );
}


void CSpreadsheetWorkBook::openWorkbook( const SpreadsheetFileFormat fileFormat, const QString& fileName, const bool displayVerboseOutput ) {
  _srcFileName = fileName;
  _fileFormat = fileFormat;
  _displayVerboseOutput = displayVerboseOutput;
  _pWB = nullptr;
  _xlsx = nullptr;

  switch( fileFormat ) {
    case  Format97_2003:
      _ok = openXlsWorkbook();
      break;
    case Format2007:
      _ok = openXlsxWorkbook();
      break;
    default:
      _errMsg = "Spreadsheet file format cannot be determined.";
      _ok = false;
      break;
  }
}


CSpreadsheetWorkBook::SpreadsheetFileFormat CSpreadsheetWorkBook::guessFileFormat( const QString& fileName ) {
  SpreadsheetFileFormat fileFormat = FormatUnknown;
  bool error;
  QString fileType = magicFileTypeInfo( fileName, &error );

  //qDebug() << fileType;

  if( error ) {
    _errMsg = "File type cannot be determined: there is a problem with the filemagic library.";
    _ok = false;
  }

  // Excel (*.xls) files look like this to FileMagic
  else if( fileType.contains( "Composite Document File V2 Document" ) || fileType.contains( "CDF V2 Document" ) ) {
    fileFormat = Format97_2003;
  }

  // Excel (*.xlsx) files look like this to FileMagic
  else if(
    ( fileType.startsWith( "Zip archive data" ) && fileName.endsWith( ".xlsx", Qt::CaseInsensitive ) )
    // || ( fileType.startsWith( "Zip archive data" ) && fileName.endsWith( ".xls", Qt::CaseInsensitive ) ) // I think this was a mistake...
    || ( 0 == fileType.compare( "Microsoft Excel 2007+" ) )
    || ( fileType.contains( "Microsoft OOXML" ) && fileName.endsWith( ".xlsx", Qt::CaseInsensitive ) )
  ) {
    fileFormat = Format2007;
  }
  else {
    _errMsg = "File type cannot be matched: the filemagic library returned an unrecognized type.";
    _ok = false;
  }

  return fileFormat;
}


bool CSpreadsheetWorkBook::openXlsxWorkbook() {
  _xlsx = new QXlsx::Document( _srcFileName );

  for( int i = 0; i < _xlsx->sheetNames().count(); ++i ) {
    _sheetNames.insert( i, _xlsx->sheetNames().at(i) );
  }

  _xlsIs1904 = false;

  return true;
}


bool CSpreadsheetWorkBook::openXlsWorkbook() {
  // Open workbook, choose standard conversion
  //------------------------------------------
  QString encoding = "UTF-8"; // "iso-8859-15//TRANSLIT" UTF-8 seems to be the new standard.
  _pWB = xls::xls_open( _srcFileName.toLatin1().data(), encoding.toLatin1().data() );

  if( nullptr == _pWB ) {
    _errMsg = "Specified file could not be opened.  Wrong format?";
    return false;
  }

  // Generate some helper bits that will let us determine further down which cells contain dates, times, or date/times
  //------------------------------------------------------------------------------------------------------------------
  _xlsIs1904 = ( 1 == _pWB->is1904 );

  _xlsFormats.clear();
  for( unsigned int i = 0; i < _pWB->formats.count; ++i ) {
    _xlsFormats.insert( _pWB->formats.format[i].index, _pWB->formats.format[i].value );

    if( _displayVerboseOutput )
      cout << "Format: " << "i: " << i << ", idx: " << _pWB->formats.format[i].index << ", val: " << _pWB->formats.format[i].value << endl;
  }
  if( _displayVerboseOutput )
    cout << endl;

  _xlsXFs.clear();
  for( unsigned int i = 0; i < _pWB->xfs.count; ++i ) {
    if( 0 != _pWB->xfs.xf[i].format ) {
      _xlsXFs.insert( i, _pWB->xfs.xf[i].format );

      if( _displayVerboseOutput )
        cout << "XFs: " << "i: " << i << ", format: " << _pWB->xfs.xf[i].format << ", type: " << _pWB->xfs.xf[i].type << endl;
    }
  }
  if( _displayVerboseOutput )
    cout << endl;


  for( unsigned int i = 0; i < _pWB->sheets.count; ++i ) {
    _sheetNames.insert( i, _pWB->sheets.sheet[i].name );

    if( _displayVerboseOutput )
      cout << _pWB->sheets.sheet[i].name << endl;
  }
  if( _displayVerboseOutput )
    cout << endl;

  return true;
}


CSpreadsheetWorkBook::~CSpreadsheetWorkBook() {
  if( nullptr != _pWB )
    xls::xls_close( _pWB );

  if( nullptr != _xlsx )
    delete _xlsx;
}


bool CSpreadsheetWorkBook::hasSheet( const int idx ) {
  return _sheetNames.containsKey( idx );
}

bool CSpreadsheetWorkBook::hasSheet( const QString& sheetName ) {
  return _sheetNames.containsValue( sheetName );
}

int CSpreadsheetWorkBook::sheetIndex( const QString& sheetName ) {
  if( this->hasSheet( sheetName ) )
    return _sheetNames.retrieveKey( sheetName );
  else
    return -1;
}

QString CSpreadsheetWorkBook::sheetName( const int idx ) {
  if( this->hasSheet( idx ) )
    return _sheetNames.retrieveValue( idx );
  else
    return QString();
}

CSpreadsheet& CSpreadsheetWorkBook::sheet( const int idx ) {
  return _sheets[idx];
}

CSpreadsheet& CSpreadsheetWorkBook::sheet( const QString& sheetName ) {
  return _sheets[ _sheetNames.retrieveKey( sheetName ) ];
}


QVariantList CSpreadsheetWorkBook::rowFromSheetXlsx( const int rowIdx, const QString& sheetName ) {
  QVariantList result;

  if( !_xlsx->selectSheet( sheetName ) ) {
    _errMsg = QString( "Specified worksheet (%1) could not be selected." ).arg( sheetName );
    return result;
  }

  QXlsx::CellRange cellRange = _xlsx->dimension();
  if( (0 >= cellRange.firstRow()) || (0 >= cellRange.firstColumn()) || (0 >= cellRange.lastRow()) || (0 >= cellRange.lastColumn()) ) {
    _errMsg = "Sheet dimensions appear to be incorrect.";
    return result;
  }

  int row = rowIdx + 1;

  if( (row < cellRange.firstRow()) || (row > cellRange.lastRow()) ) {
    _errMsg = "Cell range is out of bounds.";
    return result;
  }

  // FIXME: This does not account for merged cells.
  for( int col = 1; col < (cellRange.lastColumn() + 1); ++col ) {

    QVariant val = _xlsx->read( row, col );
    if( val.type() == QVariant::String ) {
      val = val.toString().replace( "_x000D_\n", "\n" );
    }

    result.append( val );
  }

  return result;
}


QVariantList CSpreadsheetWorkBook::rowFromSheetXls( const int rowIdx, const int sheetIdx ) {
  QVariantList result;

  // Open and parse the sheet
  //=========================
  xls::xlsWorkSheet* pWS = xls::xls_getWorkSheet( _pWB, sheetIdx );
  xls::xls_parseWorkSheet( pWS );

  // Process the indicated row
  //==========================
  xlsWORD cellRow, cellCol;
  cellRow = rowIdx;

  for( cellCol=0; cellCol < pWS->rows.lastcol; ++cellCol ) {
    xls::xlsCell* cell = xls::xls_cell( pWS, cellRow, cellCol );

    if( !cell || cell->isHidden ) {
      continue;
    }
    else {
      QString msg;
      QVariant val = CSpreadsheet::processCellXls( cell, msg, this );

      result.append( val );
    }
  }

  return result;
}


QVariantList CSpreadsheetWorkBook::firstRowFromSheet( const int sheetIdx ) {
  return this->rowFromSheet( 0, sheetIdx );
}


QVariantList CSpreadsheetWorkBook::rowFromSheet( const int rowIdx, const int sheetIdx ) {
  QVariantList result;

  if( !_ok ) {
    _errMsg = "Workbook is not open.";
    return result;
  }

  if( !_sheetNames.containsKey( sheetIdx ) ) {
    _errMsg = QString( "Specified work sheet does not exist: %1" ).arg( sheetIdx );
    return result;
  }

  switch( _fileFormat ) {
    case Format2007:
      result = rowFromSheetXlsx( rowIdx, _sheetNames.retrieveValue( sheetIdx ) );
      break;
    case Format97_2003:
      result = rowFromSheetXls( rowIdx, sheetIdx );
      break;
    default:
      Q_UNREACHABLE();
      _errMsg = "Format is not specified.";
      break;
  }

  return result;
}


bool CSpreadsheetWorkBook::readSheet( const int sheetIdx ) {
  if( !_ok ) {
    _errMsg = "Workbook is not open.";
    return false;
  }

  if( !_sheetNames.containsKey( sheetIdx ) ) {
    _errMsg = QString( "Specified work sheet does not exist: %1" ).arg( sheetIdx );
    return false;
  }

  if( _sheets.contains( sheetIdx ) ) {
    _errMsg = QString( "The selected sheet has already been read: %1" ).arg( sheetIdx );
  }

  CSpreadsheet sheet( this );
  bool result;

  switch( _fileFormat ) {
    case Format2007:
      result = sheet.readXlsx( _sheetNames.retrieveValue( sheetIdx ), _xlsx, _displayVerboseOutput );
      break;
    case Format97_2003:
      result = sheet.readXls( sheetIdx, _pWB, _displayVerboseOutput );
      break;
    default:
      Q_UNREACHABLE();
      _errMsg = "Format is not specified.";
      result = false;
      break;
  }

  if( result )
    _sheets.insert( sheetIdx, sheet );

  return result;
}


bool CSpreadsheetWorkBook::readSheet( const QString& sheetName ) {
  if( !_sheetNames.containsValue( sheetName ) ) {
    _errMsg = QString( "Specified work sheet does not exist: %1" ).arg( sheetName );
    return false;
  }
  else {
    return readSheet( _sheetNames.retrieveKey( sheetName ) );
  }
}


bool CSpreadsheetWorkBook::readAllSheets() {
  if( !_ok ) {
    _errMsg = "Workbook is not open.";
    return false;
  }

  bool result = true;
  for( int i = 0; i < _sheetNames.count(); ++i ) {
    result = ( result && readSheet( i ) );
  }

  return result;
}


bool CSpreadsheetWorkBook::isXls1904DateSystem() {
  if( Format97_2003 != _fileFormat )
    return false;
  else
    return _xlsIs1904;
}


bool CSpreadsheetWorkBook::isXlsDate(const int xf, const double d ) {
  if( Format97_2003 != _fileFormat ) {
    return false;
  }

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


bool CSpreadsheetWorkBook::isXlsTime( const int xf, const double d ) {
  if( Format97_2003 != _fileFormat ) {
    return false;
  }

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


bool CSpreadsheetWorkBook::isXlsDateTime(const int xf, const double d ) {
  Q_UNUSED( d );

  if( Format97_2003 != _fileFormat ) {
    return false;
  }

  bool result;
  int fmt = _xlsXFs.value( xf );
  bool looksLikeDate, looksLikeTime;
  QString fmtStr;

  // Check for built-in date formats
  if( 22 == fmt ) { // Default date/time format: see FORMAT (p. 174) in http://www.openoffice.org/sc/excelfileformat.pdf
    result = true;
  }
  else {
    fmtStr = _xlsFormats.value( fmt );

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

  return result;
}


bool CSpreadsheetWorkBook::addSheet( const QString& sheetName /* = QString() */ ) {
  if( Format2007 != _fileFormat  ) {
    _ok = false;
    _errMsg = "Sheets can only be added to Format2007 files";
  }
  else if( !sheetName.isEmpty() && this->hasSheet( sheetName ) ) {
    _ok = false;
    _errMsg = QString( "Duplicate sheet name: '%1'" ).arg( sheetName );
  }
  else {
    _ok = _xlsx->addSheet( sheetName );
    if( !_ok ) {
      _errMsg = QString( "Could not insert sheet with name '%1'" ).arg( sheetName );
    }
    else {
      _sheetNames.insert( this->sheetCount() - 1, sheetName );
    }
  }

  return _ok;
}


bool CSpreadsheetWorkBook::deleteSheet( const int sheetIdx ) {
  if( !_sheetNames.containsKey( sheetIdx ) ) {
    _ok = false;
    _errMsg = QString( "Sheet does not exist: %1" ).arg( sheetIdx );
  }
  else {
    _ok = this->deleteSheet( _sheetNames.retrieveValue( sheetIdx ) );
  }

  return _ok;
}


bool CSpreadsheetWorkBook::deleteSheet( const QString& sheetName ) {
  if( Format2007 != _fileFormat  ) {
    _ok = false;
    _errMsg = "Sheets can only be deleted from Format2007 files";
  }
  else if( !this->hasSheet( sheetName ) ) {
    _ok = false;
    _errMsg = QString( "Sheet does not exist: '%1'" ).arg( sheetName );
  }
  else {
    _ok = _xlsx->deleteSheet( sheetName );
    if( !_ok ) {
      _errMsg = QString( "Could not delete sheet '%1'" ).arg( sheetName );
    }
    else {
      _sheetNames.remove( sheetName );
    }
  }

  return _ok;
}


bool CSpreadsheetWorkBook::writeSheet( const int sheetIdx, const CTwoDArray<QVariant>& data ) {
  if( Format2007 != _fileFormat  ) {
    _ok = false;
    _errMsg = "Sheets can only be written to Format2007 files";
  }
  else if( !_sheetNames.containsKey( sheetIdx ) ) {
    _ok = false;
    _errMsg = QString( "Sheet does not exist: %1" ).arg( sheetIdx );
  }
  else {
    _ok = this->writeSheet( _sheetNames.retrieveValue( sheetIdx ), data );
  }

  return _ok;
}


bool CSpreadsheetWorkBook::writeSheet( const QString& sheetName, const CTwoDArray<QVariant>& data ) {
  if( Format2007 != _fileFormat  ) {
    _ok = false;
    _errMsg = "Sheets can written to Format2007 files";
  }
  if( !_sheetNames.containsValue( sheetName ) ) {
    _ok = this->addSheet( sheetName );
  }

  if( _ok ) {
    if( !_xlsx->selectSheet( sheetName ) ) {
      _ok = false;
      _errMsg = QString( "Could not select sheet %1" ).arg( sheetName );
    }
    else {
      _ok = true;

      int rowOffset = 1;
      int colOffset = 1;

      if( data.hasRowNames() ) {
        ++colOffset;
      }

      if( data.hasColNames() ) {
        for( int c = 0; c < data.nCols(); ++c ) {
          _ok = _xlsx->write( rowOffset, c + colOffset, data.colNames().at(c) );
        }
        ++rowOffset;
      }

      if( data.hasRowNames() ) {
        for( int r = 0; r < data.nRows(); ++r ) {
          _ok = _xlsx->write( r + rowOffset, 1, data.rowNames().at(r) );
        }
      }

      for( int row = 0; row < data.nRows(); ++row ) {
        for( int col = 0; col < data.nCols(); ++col ) {
          _ok = _xlsx->write( row + rowOffset, col + colOffset, data.at( col, row ) );
          if( !_ok ) {
            break;
          }
        }
        if( !_ok ) {
          break;
        }
      }
    }

    if( !_ok ) {
      _errMsg = QString( "Could not write data to sheet %1" ).arg( sheetName );
    }
  }

  return _ok;
}


bool CSpreadsheetWorkBook::save() {
  if( Format2007 != _fileFormat  ) {
    _ok = false;
    _errMsg = "Sheets can written to Format2007 files";
  }
  else {
    _ok = _xlsx->save();
    if( !_ok ) {
      _errMsg = "File could not be written.";
    }
  }
  return _ok;
}


bool CSpreadsheetWorkBook::saveAs( const QString& filename ) {
  if( Format2007 != _fileFormat  ) {
    _ok = false;
    _errMsg = "Sheets can written to Format2007 files";
  }
  else {
    _ok = _xlsx->saveAs( filename );
    if( !_ok ) {
      _errMsg = "File could not be written.";
    }
    else {
      _srcFileName = filename;
    }
  }
  return _ok;
}
