/*
xlcsv.h/cpp
-----------
Begin: 2017/01/10
Author: Aaron Reeves <aaron.reeves@sruc.ac.uk>
--------------------------------------------------
Copyright (C) 2017 - 2018 Epidemiology Research Unit, Scotland's Rural College (SRUC)

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/


#include <ar_general_purpose/qcout.h>

#include "xlcsv.h"


CXlCsv::CXlCsv() : QCsv() {
  // Accept default parameters specified by QCsv::initialize().

  initialize();

  // File will NOT be opened by default.
}


CXlCsv::CXlCsv(
  const CSpreadsheetWorkBook::SpreadsheetFileFormat format,
  const QString& filename,
  const bool containsFieldList,
  const int nLinesToSkip /* = 0 */,
  const QString& sheetname /* = "" */ ) : QCsv()
{
  initialize();

  setFileFormat( format );
  setFilename( filename );
  setContainsFieldList( containsFieldList );
  setSheetname( sheetname );
  setLinesToSkip( nLinesToSkip );

  open();
}


void CXlCsv::initialize() {
  QCsv::initialize();

  _sheetname = "";
  _sheetIdx = 0;
  _useSheetname = false;

  _errorOnOpen = false;

  _fileFormat = CSpreadsheetWorkBook::FormatUnknown;

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

  if( _errorOnOpen ) {
    return false;
  }

  if( CSpreadsheetWorkBook::FormatUnknown == _fileFormat ) {
    setError( QCsv::ERROR_OTHER, "Spreadsheet file format must be specified." );
    _errorOnOpen = true;
    return false;
  }

  if( _srcFilename.isEmpty() ) {
    setError( QCsv::ERROR_OPEN, "No input file specified." );
    _errorOnOpen = true;
    return false;
  }

  QFileInfo fi( _srcFilename );
  if( !fi.exists() || !fi.isReadable() ) {
    setError( QCsv::ERROR_OPEN, "Specified input file does not exist or cannot be read." );
    _errorOnOpen = true;
    return false;
  }

  if( 0 > _linesToSkip ) {
    setError( ERROR_INDEX_OUT_OF_RANGE, "Number of lines to skip must be 0 or more." );
    _errorOnOpen = true;
    return false;
  }

  bool result;
  clearError();

  switch ( _fileFormat ) {
    case CSpreadsheetWorkBook::Format97_2003:
      result = openXls();
      break;
    case CSpreadsheetWorkBook::Format2007:
      result = openXlsx();
      break;
    default:
      setError( QCsv::ERROR_OTHER, "Specified spreadsheet file format is not supported." );
      result = false;
      break;
  }

  if( !result ) {
    _errorOnOpen = true;
  }

  return result;
}


bool CXlCsv::openXls() {
  CSpreadsheetWorkBook wb( CSpreadsheetWorkBook::Format97_2003, this->filename() );

  if( _useSheetname ) {
    if( !wb.hasSheet( _sheetname ) ) {
      setError( ERROR_OTHER, QString( "Specified worksheet (%1) could not be selected." ).arg( _sheetname ) );
      return false;
    }
    else {
      _sheetIdx = wb.sheetIndex( _sheetname );
    }
  }

  if( !wb.hasSheet( _sheetIdx ) ) {
    setError( ERROR_OTHER, QString( "Specified worksheet (%1) could not be selected." ).arg( _sheetIdx ) );
    return false;
  }

  if( !wb.readSheet( _sheetIdx ) ) {
    setError( ERROR_OTHER, "Specified worksheet could not be read." );
    return false;
  }

  CSpreadsheet sheet = wb.sheet( _sheetIdx );

  if( !sheet.isTidy( this->containsFieldList() ) ) {
    setError( ERROR_OTHER, "Specified worksheet does not have a tidy CSV format." );
    return false;
  }

  int starti = this->_linesToSkip;
  if( this->containsFieldList() ) {
    this->setFieldNames( sheet.rowAsStringList( starti ) );
    ++starti;
  }

  for( int i = starti; i < sheet.nRows(); ++i ) {
    this->append( sheet.rowAsStringList( i ) );
  }

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

  // Read the first row (which may be a header row) to establish the number of columns.
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
    this->append( list );
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

      if( QVariant::DateTime == val.type() )
        list.append( val.toDateTime().toString( "yyyy-MM-dd hh:mm:ss" ) );
      else
        list.append( val.toString() );
    }

    if( nullsFound < nCols ) {
      this->append( list );
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

    if( DateFormat == fmt ) {
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
    }
    else {
      // FIXME: TimeFormat and DateTimeFormat are not yet handled.
      setError( QCsv::ERROR_OTHER, "Field format not yet supported." );
      result = false;
    }
  }

  return result;
}



