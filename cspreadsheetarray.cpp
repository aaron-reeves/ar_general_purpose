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

typedef uint16_t xlsWORD;

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
  _wb = NULL;
  _hasMergedCells = false;
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
  _hasMergedCells = other._hasMergedCells;
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


bool CSpreadsheet::isTidy( const bool containsHeaderRow ) {
  bool result = true;

  // Criteria for tidy spreadsheets:
  //  1. No merged cells.
  //  2. If the first row is a header:
  //    a) It should not contain any blanks.
  //    b) Every value in the first row should be a string(?)

  if( this->_hasMergedCells ) {
    result = false;
  }
  else if( containsHeaderRow ) {
    for( int c = 0; c < this->nCols(); ++c ) {
      if(
         this->at( c, 0 ).value().isNull()
         || ( QVariant::String != this->at( c, 0 ).value().type() )
         || ( 0 == this->at( c, 0 ).value().toString().length() )
       ) {
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
      list.append( this->at( c, rowNumber).value().toString() );
  }

  return list;
}


QCsv CSpreadsheet::asCsv( const bool containsHeaderRow, const QChar delimiter /* = ',' */ ) {
  QCsv csv;

  if( this->isEmpty() ) {
    csv.setError( QCsv::ERROR_OTHER, "Specified worksheet is empty." );
  }
  else if( !this->isTidy( containsHeaderRow ) ) {
    csv.setError( QCsv::ERROR_OTHER, "Specified worksheet does not have a tidy CSV format." );
  }
  else {
    QStringList headerRow;
    QList<QStringList> data;

    int startRow = 0;
    if( containsHeaderRow ) {
      headerRow = this->rowAsStringList( 0 );
      startRow = 1;
    }
    for( int i = startRow; i < this->nRows(); ++i ) {
      data.append( this->rowAsStringList( i ) );
    }

    if( containsHeaderRow ) {
      csv = QCsv( headerRow, data );
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
  csv.toFront();

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

  result = result.addSecs( s );

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
          if( _wb->isXlsDate( cell->xf, cell->d ) ) {
            val = xlsDate( (int)cell->d, _wb->isXls1904DateSystem() );
          }
          else if( _wb->isXlsTime( cell->xf, cell->d ) ) {
            val = xlsTime( cell->d );
          }
          else if( _wb->isXlsDateTime( cell->xf, cell->d ) ) {
            val = xlsDateTime( cell->d, _wb->isXls1904DateSystem() );
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

        CSpreadsheetCell ssCell( val, cell->colspan, cell->rowspan );
        this->setValue( cellCol, cellRow, ssCell );

        // Make a note if the cell is merged.
        _hasMergedCells = ( _hasMergedCells || ssCell.isMerged() );

        if( displayVerboseOutput ) {
          cout << msg << endl;
        }
      }
    }
  }

  return true;
}



CSpreadsheetWorkBook::CSpreadsheetWorkBook( const SpreadsheetFileFormat fileFormat, const QString& fileName, const bool displayVerboseOutput /* = false */ ) {
  _srcFileName = fileName;
  _fileFormat = fileFormat;
  _displayVerboseOutput = displayVerboseOutput;
  _pWB = NULL;
  _xlsx = NULL;

  switch( fileFormat ) {
    case  Format97_2003:
      _ok = openXlsWorkbook();
      break;
    case Format2007:
      _ok = openXlsxWorkbook();
      break;
    default:
      Q_UNREACHABLE();
      _ok = false;
      break;
  }
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

  if( NULL == _pWB ) {
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
  if( NULL != _pWB )
    xls::xls_close( _pWB );

  if( NULL != _xlsx )
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


QVariantList CSpreadsheetWorkBook::rowXlsx( const int rowIdx, const QString& sheetName ) {
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


QVariantList CSpreadsheetWorkBook::firstRow( const int sheetIdx ) {
  return this->row( 0, sheetIdx );
}


QVariantList CSpreadsheetWorkBook::row( const int rowIdx, const int sheetIdx ) {
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
      result = rowXlsx( rowIdx, _sheetNames.retrieveValue( sheetIdx ) );
      break;
    case Format97_2003:
      // FIXME: Write this function.
      qDebug() << "Format is not supported.";
      Q_UNREACHABLE();
      _errMsg = "Format is not supported.";
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
