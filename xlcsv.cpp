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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <ar_general_purpose/xlcsv/xls.h>

//#pragma pack()

#include <ar_general_purpose/qcout.h>

#include "xlcsv.h"

CXlCsv::CXlCsv() : QCsv() {
  // Accept default parameters specified by QCsv::initialize().

  initializeXl();

  // File will NOT be opened by default.
}


CXlCsv::CXlCsv(
  const CXlCsvFileFormat format,
  const QString& filename,
  const bool containsFieldList,
  const int nLinesToSkip /* = 0 */,
  const QString& sheetname /* = "" */ ) : QCsv()
{
  initializeXl();

  setFileFormat( format );
  setFilename( filename );
  setContainsFieldList( containsFieldList );
  setSheetname( sheetname );
  setLinesToSkip( nLinesToSkip );

  open();
}


void CXlCsv::initializeXl() {
  setSheetname( "" );
  setFileFormat( FormatUnknown );

  setMode( QCsv::EntireFile );

  _is1904DateSystem = false;
}

CXlCsv::~CXlCsv() {
  // Do nothing.
}


void CXlCsv::setSheetIdx( const int val ) {
  _sheetIdx = val;
  _useSheetname = false;
  _sheetname = "";
}


void CXlCsv::setSheetname( const QString& sheetname ) {
  if( sheetname.trimmed().isEmpty() ) {
    _sheetname = "";
    _useSheetname = false;
    _sheetIdx = 0;
  }
  else {
    _sheetname = sheetname;
    _useSheetname = true;
    _sheetIdx = -1;
  }
}


int CXlCsv::sheetIdx() {
  if( !_useSheetname ) {
    return _sheetIdx;
  }
  else if( _sheetNames.contains( _sheetname ) ) {
    return _sheetNames.indexOf( _sheetname );
  }
  else {
    setError( ERROR_INVALID_FIELD_NAME, QString( "Workbook does not contain sheet '%1'" ).arg( _sheetname ) );
    return -1;
  }
}

QString CXlCsv::sheetname() {
  if( _useSheetname ) {
    return _sheetname;
  }
  else if( _sheetNames.count() > _sheetIdx ) {
    return _sheetNames.at( _sheetIdx );
  }
  else {
    setError( ERROR_INDEX_OUT_OF_RANGE, QString( "Workbook does not contain sheet '%1'" ).arg( _sheetIdx ) );
    return "";
  }
}


QString CXlCsv::sheetToOpen( bool& ok ) {
  QString result;
  ok = true; // until shown otherwise

  if( _useSheetname ) {
    if( !_sheetNames.contains( _sheetname ) )
      ok = false;
    else
      result = _sheetname;
  }
  else {
    if( _sheetNames.count() > _sheetIdx )
      result = _sheetNames.at( _sheetIdx );
    else
      ok = false;
  }

  return result;
}


bool CXlCsv::open() {
  if( _isOpen ) {
    clearError();
    this->toFront();
    return true;
  }

  if( FormatUnknown == _fileFormat ) {
    setError( QCsv::ERROR_OTHER, "Spreadsheet file format must be specified." );
    return false;
  }

  if( _srcFilename.isEmpty() ) {
    setError( QCsv::ERROR_OPEN, "No input file specified." );
    return false;
  }

  QFileInfo fi( _srcFilename );
  if( !fi.exists() || !fi.isReadable() ) {
    setError( QCsv::ERROR_OPEN, "Specified input file does not exist or cannot be read." );
    return false;
  }

  if( 0 > _linesToSkip ) {
    setError( ERROR_INDEX_OUT_OF_RANGE, "Number of lines to skip must be 0 or more." );
    return false;
  }

  switch ( _fileFormat ) {
    case Format97_2003:
      return openXls();
      break;
    case Format2007:
      return openXlsx();
      break;
    default:
      setError( QCsv::ERROR_OTHER, "Specified spreadsheet file format is not supported." );
      return false;
      break;
  }
}


bool CXlCsv::openXls() {
  // Open workbook, choose standard conversion
  //------------------------------------------
  xlsWorkBook* pWB = xls_open( this->filename().toLatin1().data(), "iso-8859-15//TRANSLIT" );

  if( NULL == pWB ) {
    setError( QCsv::ERROR_OPEN, "Specified file could not be opened.  Wrong format?" );
    return false;
  }

  if( 1 == pWB->is1904datesystem ) {
    _is1904DateSystem = true;
  }

  //for( int i = 0; i < pWB->formats.count; ++i ) {
  //  cout << "Format:" << "idx: " << pWB->formats.format[i].index << ", val: " << pWB->formats.format[i].value << endl;
  //}

  //for( int i = 0; i < pWB->xfs.count; ++i ) {
  //  cout << "XFS: " << "idx: " << pWB->xfs.xf[i].format << ", type: " << pWB->xfs.xf[i].type << ", value: " << pWB->xfs.xf[i].format << endl;
  //}

  _sheetNames.clear();
  for( int i = 0; i < pWB->sheets.count; ++i ) {
    _sheetNames.append( pWB->sheets.sheet[i].name );
  }

  bool ok;
  QString sheetToOpen = this->sheetToOpen( ok );
  if( !ok ) {
    xls_close( pWB );
    _error = QCsv::ERROR_OTHER;
    if( _useSheetname )
      _errorMsg = QString( "Specified worksheet (%1) could not be selected." ).arg( _sheetname );
    else
      _errorMsg = QString( "Specified worksheet (%1) could not be selected." ).arg( _sheetIdx );
    return false;
  }

  // Open and parse the sheet
  //-------------------------
  xlsWorkSheet* pWS = xls_getWorkSheet( pWB, _sheetNames.indexOf( sheetToOpen ) );
  xls_parseWorkSheet( pWS );

  // Process all rows of the sheet
  //------------------------------
  struct st_row_data* row;
  XLSWORD t, tt;
  bool firstRowFinished = false;
  QStringList list;
  bool lineWritten = false;

  qDebug() << "rows:" <<  pWS->rows.lastrow << ", cols:" << pWS->rows.lastcol;

  for( t=0; t <= pWS->rows.lastrow; ++t ) {
    row = &pWS->rows.row[t];

    // process cells
    if( lineWritten ) {
      if( this->containsFieldList() && !firstRowFinished ) {
        this->setFieldNames( list );
        //qDebug() << "Header:" << list.count() << list;
        firstRowFinished = true;
      }
      else {
        //qDebug() << "Body:" << list.count() << list;
        this->appendRow( list );
      }
      list.clear();
    }
    else {
      lineWritten = true;
    }

    for( tt=0; tt < pWS->rows.lastcol; ++tt ) {
      if( !row->cells.cell[tt].ishidden ) {
        // Display the value of the cell (either numeric or string)
        //---------------------------------------------------------

        // AR: Not sure what's up with the magic constants here... Something about establishing
        // that a cell contains a number, from the look of it.
        // FIXME: I need to know dates!
        if( ( 0x27E == row->cells.cell[tt].id ) || ( 0x0BD == row->cells.cell[tt].id ) || ( 0x203 == row->cells.cell[tt].id ) ) {
           char arstr[255];
           sprintf( arstr, "%.15g", row->cells.cell[tt].d );
           //qDebug() << "Value:" << arstr << ", id:" << QString( "%1" ).arg( row->cells.cell[tt].id, 0, 16 ).toUpper().prepend( "0x" );
           list.append( arstr );
        }

        // A string
        else if( NULL != row->cells.cell[tt].str ) {
           list.append( row->cells.cell[tt].str );
        }

        // An empty cell
        else {
           list.append( QString() );
        }
      }
    }
  }

  // Don't forget the last row.
  this->appendRow( list );

  xls_close( pWB );

  _isOpen = true;

  this->toFront();

  return true;
}


bool CXlCsv::openXlsx() {
  // Nonexistent files or files that cannot be read will return cell ranges with negative values.
  // Empty files will return cell ranges with values of 1 (which seems weird).
  // Actual files return cell ranges that are somewhat reasonable, but can include empty rows or columns.
  // ODS files can't be read by QXlsx... not a huge surprise.

  QXlsx::Document xlsx( _srcFilename );

  _sheetNames = xlsx.sheetNames();

  bool ok;
  QString sheetToOpen = this->sheetToOpen( ok );
  if( !ok || !xlsx.selectSheet( sheetToOpen ) ) {
    _error = QCsv::ERROR_OTHER;
    if( _useSheetname )
      _errorMsg = QString( "Specified worksheet (%1) could not be selected." ).arg( _sheetname );
    else
      _errorMsg = QString( "Specified worksheet (%1) could not be selected." ).arg( _sheetIdx );
    return false;
  }

  QXlsx::CellRange cellRange = xlsx.dimension();
  if( (0 >= cellRange.firstRow()) || (0 >= cellRange.firstColumn()) || (0 >= cellRange.lastRow()) || (0 >= cellRange.lastColumn()) ) {
    _error = QCsv::ERROR_OTHER;
    _errorMsg = "Cell range is out of bounds.";
    return false;
  }

  // If we get this far, populate the data object.
  //----------------------------------------------
  int row = 1 + _linesToSkip;
  int col = 1;
  bool nullFound = false;
  QStringList list;
  QVariant val;

  // Read the first row (which may be a header row) to estableish the number of columns.
  while( !nullFound ) {
    val = xlsx.read( row, col );
    if( !val.isValid() )
      nullFound = true;
    else {
      list.append( val.toString() );
      ++col;
    }
  }

  if( _containsFieldList ) {
    setFieldNames( list );
  }
  else {
    this->appendRow( list );
  }

  // Subsequent rows definitely contain data.
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

  _isOpen = true;

  this->toFront();

  return true;
}


bool CXlCsv::setFieldFormatXl( const QString& fieldName, const ColumnFormat fmt ) {
  if( !_isOpen ) {
    setError( QCsv::ERROR_OPEN, "File must be open to set a field format." );
    return false;
  }
  else if( EntireFile != this->mode() ) {
    setError( QCsv::ERROR_WRONG_MODE, "Mode must be EntireFile to set a field format." );
    return false;
  }
  else if( !this->containsFieldList() ) {
    setError( QCsv::ERROR_NO_FIELDLIST, "This file does not have field names." );
    return false;
  }
  else {
    int fieldIdx = fieldIndexOf( fieldName.trimmed() );
    if( -1 == fieldIdx ) {
      setError( QCsv::ERROR_INVALID_FIELD_NAME, QString( "Field name '%1' does not exist." ).arg( fieldName ) );
      return false;
    }
    else {
      return setFieldFormatXl( fieldIdx, fmt );
    }
  }
}


bool CXlCsv::setFieldFormatXl( const int fieldIdx, const ColumnFormat fmt ) {
  bool result = true; // Until shown otherwise.

  if( !_isOpen ) {
    setError( QCsv::ERROR_OPEN, "File must be open to set a field format." );
    result = false;
  }
  else if( EntireFile != this->mode() ) {
    setError( QCsv::ERROR_WRONG_MODE, "Mode must be EntireFile to set a field format." );
    result = false;
  }
  else if( 0 > fieldIdx ) {
    setError( QCsv::ERROR_INDEX_OUT_OF_RANGE, "Negative field index provided." );
    result = false;
  }
  else if( this->fieldCount() <= fieldIdx ) {
    setError( QCsv::ERROR_INDEX_OUT_OF_RANGE, QString( "Field index %1 is out of range." ).arg( fieldIdx ) );
    result = false;
  }
  else {
    Q_ASSERT( fmt == DateFormat ); // FIXME: Eventually, support time and date/time formats.

    QDate date;

    switch( fmt ) {
      case DateFormat:
        for( int row = 0; row < this->rowCount(); ++row ) {
          QString str = _data.at(row).at(fieldIdx);
          bool isInt;
          int val = str.toInt( &isInt );

          if( str.isEmpty() ) {
            // Do nothing: leave the empty value.
          }
          else if( isInt ) {
            if( _is1904DateSystem ) { // Treat the date with the 1904 date system.
              date = QDate( 1904, 1, 1 ).addDays( val );
            }
            else { // Use the 1900 date system, with the leap year bug.
              date = QDate( 1899, 12, 31 );

              // There is an intentional leap-year bug in Excel. It thinks that 1900 was a leap year.
              // It wasn't, but the bug was introduced to maintain compatibility with Lotus 1-2-3.
              // Any date beyond Feb 28 1900 will be off by a day, if we just add the number of days.
              if( val < 60 )
                date = date.addDays( val );
              else
                date = date.addDays( val - 1 );
            }

            if( date.isValid() ) {
              _data[row][fieldIdx] = date.toString( "yyyy-MM-dd" );
            }
            else {
              setError( QCsv::ERROR_OTHER, QString( "Format of cell at row %1, column %2 cannot be changed to DateFormat." ).arg( row ).arg( fieldIdx ) );
              result = false;
            }
          }
          else { // There is nothing more that can be done here.  Maybe try QCsv::setFieldFormat()
            setError( QCsv::ERROR_OTHER, QString( "Format of cell at row %1, column %2 cannot be changed to DateFormat." ).arg( row ).arg( fieldIdx ) );
            result = false;
          }
        }

        break;
      case TimeFormat: // Fall through, for now.
      case DateTimeFormat: // Fall through, for now.
      default:
        setError( QCsv::ERROR_OTHER, "Field format not yet supported." );
        result = false;
        break;
    }
  }

  return result;
}



