/*
cspreadsheetarray.h/cpp
-----------------------
Begin: 2018/09/13
Author: Aaron Reeves <aaron.reeves@sruc.ac.uk>
--------------------------------------------------
Copyright (C) 2018 - 2021 Epidemiology Research Unit, Scotland's Rural College (SRUC)

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/


#include "cspreadsheetarray.h"

#include <QDebug>

#include <ar_general_purpose/strutils.h>
#include <ar_general_purpose/qcout.h>
#include <ar_general_purpose/filemagic.h>
#include <ar_general_purpose/log.h>

typedef uint16_t xlsWORD;

CSpreadsheetCell::CSpreadsheetCell() {
  // _value is initialized by default
  _colSpan = 1;
  _rowSpan = 1;
  _isPartOfMergedRow = false;
  _isPartOfMergedCol = false;
  _originCell = nullptr;
}


CSpreadsheetCell::CSpreadsheetCell( const QVariant& val ) {
  _value = val;
  _colSpan = 1;
  _rowSpan = 1;
  _isPartOfMergedRow = false;
  _isPartOfMergedCol = false;
  _originCell = nullptr;
}


CSpreadsheetCell::CSpreadsheetCell( const QVariant& val, const int colSpan, const int rowSpan ) {
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
  //Q_ASSERT( other._originCell == nullptr );

  _value = other._value;
  _colSpan = other._colSpan;
  _rowSpan = other._rowSpan;
  _isPartOfMergedRow = other._isPartOfMergedRow;
  _isPartOfMergedCol = other._isPartOfMergedCol;

  _originCell = nullptr; // WARNING: This must be set by the spreadsheet, using _originCellRef!

  _originCellRef = other._originCellRef;
  _linkedCellRefs = other._linkedCellRefs;
}


const QXlsx::CellRange CSpreadsheetCell::mergedRange( const int col, const int row ) const {
  QXlsx::CellRange result;

  result.setFirstColumn( col + 1 );
  result.setLastColumn( col + _colSpan );

  result.setFirstRow( row + 1 );
  result.setLastRow( row + _rowSpan );

  return result;
}


bool CSpreadsheetCell::isNumeric() const {
  QVariant::Type type = this->value().type();

  return(
    ( QVariant::Int == type )
    || ( QVariant::Double == type )
    || ( QVariant::UInt == type )
    || ( QVariant::LongLong == type )
    || ( QVariant::ULongLong == type )
  );
}

void CSpreadsheetCell::debug( const int c /* = -1 */, const int r /* = -1 */ ) const {
  QString originStr;

  if( nullptr != this->_originCell ) {
    originStr = this->_originCell->value().toString();
  }

  if( c != -1 ) {
    qDb() << "C" << c << "R" << r << QString::number( qlonglong( this ), 16 )
             << "MergeC" << this->isPartOfMergedCol() << "MergeR" << this->isPartOfMergedRow()
             << "ColSpan" << this->colSpan() << "RowSpan" << this->rowSpan()
             << "Value" << this->value().toString()
             << "nLinked" << this->_linkedCellRefs.count()
             << "OrigC" << QString::number( qlonglong( this->_originCell ), 16 ) << "OrigCVal" << originStr;
  }
  else {
    qDb() /*<< "C" << c << "R" << r*/ << QString::number( qlonglong( this ), 16 )
             << "MergeC" << this->isPartOfMergedCol() << "MergeR" << this->isPartOfMergedRow()
             << "ColSpan" << this->colSpan() << "RowSpan" << this->rowSpan()
             << "Value" << this->value().toString()
             << "nLinked" << this->_linkedCellRefs.count()
             << "OrigC" << QString::number( qlonglong( this->_originCell ), 16 ) << "OrigCVal" << originStr;
  }
}


CSpreadsheet::CSpreadsheet( const bool* terminatedPtr /*= nullptr*/, QObject* parent ) : QObject( parent ), CTwoDArray<CSpreadsheetCell>() {
  initialize();
  _terminatedPtr = terminatedPtr;
}


CSpreadsheet::CSpreadsheet( class CSpreadsheetWorkBook* wb, const bool* terminatedPtr /*= nullptr*/, QObject* parent ) : QObject( parent ), CTwoDArray<CSpreadsheetCell>() {
  initialize();
  _terminatedPtr = terminatedPtr;
  _wb = wb;
}


CSpreadsheet::CSpreadsheet( const QString& fileName, const int sheetIdx, const bool* terminatedPtr /*= nullptr*/, QObject* parent ) : QObject( parent ), CTwoDArray<CSpreadsheetCell>() {
  initialize();
  _terminatedPtr = terminatedPtr;

  QFileInfo fi( fileName );
  if( !fi.exists() || !fi.isReadable() ) {
    _errMsg.append( QStringLiteral("Selected filed does not exist or is not readable.\n") );
    return;
  }

  CSpreadsheetWorkBook::SpreadsheetFileFormat fmt = CSpreadsheetWorkBook::guessFileFormat( fileName, &_errMsg );

  if( !_errMsg.isEmpty() ) {
    return;
  }
  else if( CSpreadsheetWorkBook::FormatCSV == fmt ) {
    if( !this->readCsv( fileName ) ) {
      _errMsg.append( QStringLiteral("CSV file could not be opened.\n") );
      return;
    }
  }
  else {
    CSpreadsheetWorkBook wb( CSpreadsheetWorkBook::ModeOpenExisting, fileName, fmt );
    if( wb.error() ) {
      _errMsg.append( QStringLiteral("Spreadsheet file format is unrecognized.  Please contact the application developers.\n") );
      _errMsg.append( QStringLiteral("Error message:\n") );
      _errMsg.append( wb.errorMessage() );
      return;
    }
    else if( 0 == wb.sheetCount() ) {
      _errMsg.append( QStringLiteral("Spreadsheet appears to have no sheets. Please double-check the file format.\n") );
      return;
    }
    else {
      if( !wb.readSheet( sheetIdx ) ) {
        _errMsg.append( QStringLiteral("Spreadsheet could not be read. There may be a problem with the file.\n") );
        return;
      }
      else {
        this->assign( wb.sheet( sheetIdx ) );
      }
    }
  }
}


CSpreadsheet::CSpreadsheet( const int nCols, const int nRows, const bool* terminatedPtr /*= nullptr*/, QObject* parent ) : QObject( parent ), CTwoDArray<CSpreadsheetCell>( nCols, nRows ) {
  initialize();
  _terminatedPtr = terminatedPtr;
}


CSpreadsheet::CSpreadsheet( const int nCols, const int nRows, const QVariant& defaultVal, const bool* terminatedPtr /*= nullptr*/, QObject* parent )
  : QObject( parent ), CTwoDArray<CSpreadsheetCell>( nCols, nRows )
{
  initialize();
  _terminatedPtr = terminatedPtr;

  for( int c = 0; c < nCols; ++c ) {
    for( int r = 0; r < nRows; ++r ) {
      this->at( c, r ).setValue( defaultVal );
    }
  }
}


CSpreadsheet::CSpreadsheet( const int nCols, const int nRows, const CSpreadsheetCell& defaultVal, const bool* terminatedPtr /*= nullptr*/, QObject* parent )
  : QObject( parent ), CTwoDArray<CSpreadsheetCell>( nCols, nRows, defaultVal )
{
  initialize();
  _terminatedPtr = terminatedPtr;
}


CSpreadsheet::CSpreadsheet( const CTwoDArray<QVariant>& data, const bool* terminatedPtr /*= nullptr*/, QObject* parent ) : QObject( parent ) {
  initialize();
  _terminatedPtr = terminatedPtr;
  setData( data );
}


void CSpreadsheet::initialize() {
  _wb = nullptr;
}


CSpreadsheet::~CSpreadsheet() {
  // Do nothing else
}


CSpreadsheet::CSpreadsheet( const CSpreadsheet& other ) : QObject( nullptr ), CTwoDArray<CSpreadsheetCell>( other ) {
  assign( other );
}


CSpreadsheet& CSpreadsheet::operator=( const CSpreadsheet& other ) {
  CTwoDArray<CSpreadsheetCell>::operator= ( other );

  assign( other );

  return *this;
}


void CSpreadsheet::assign( const CSpreadsheet& other ) {
  CTwoDArray<CSpreadsheetCell>::assign( other );

  _wb = other._wb;
  setParent( nullptr );

  _mergedCellRefs = other._mergedCellRefs;

  for( int r = 0; r < this->nRows(); ++r ) {
    for( int c = 0; c < this->nCols(); ++c ) {
      if( !this->cell(c, r)._originCellRef.isNull() ) {
        this->cell(c, r)._originCell = &( this->cell( this->cell(c, r)._originCellRef ) );
      }
    }
  }
}


void CSpreadsheet::debug( const int padding /* = 10 */) const {
  qDb() << QStringLiteral( "Matrix %1 cols x %2 rows:" ).arg( nCols() ).arg( nRows() );

  for( int r = 0; r < this->nRows(); ++r ) {
    QString str = QStringLiteral( "  Row %1: " ).arg( r );

    for( int c = 0; c < this->nCols(); ++c ) {
      str.append( rightPaddedStr( QStringLiteral( "%1" ).arg( this->at( c, r ).value().toString() ), padding ) );
    }

    qDb() << str;
  }

  qDb();
}


void CSpreadsheet::debugVerbose() const {
  for( int r = 0; r < this->nRows(); ++r ) {
    for( int c = 0; c < this->nCols(); ++c ) {
      this->at( c, r).debug( c, r );
    }
  }
}


QVariant CSpreadsheet::cellValue(const QString& cellLabel ) const {
  QString col, row;
  int c, r;

  if( cellLabel.at(1).isLetter() ) {
    col = cellLabel.left( 2 ).toUpper();
    row = cellLabel.right( cellLabel.length() - 2 );
  }
  else {
    col = cellLabel.at(0).toUpper();
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



void CSpreadsheet::appendColumn( const QVariant& defaultVal ) {
  for( int i = 0; i < this->nRows(); ++i ) {
    _data[i].resize( _nCols + 1 );
    _data[i][_nCols] = CSpreadsheetCell( defaultVal );
  }

  ++_nCols;

  if( this->hasColNames() ) {
    QString newColName = QStringLiteral( "Column_%1" ).arg( _nCols );
    Q_ASSERT( !_colNamesLookup.contains( newColName.toLower().trimmed() ) );
    _colNames.append( newColName );
    _colNamesLookup.insert( newColName.toLower().trimmed(), _nCols - 1 );
  }
}


void CSpreadsheet::appendColumn( const QVector<QVariant>& values ) {
  Q_ASSERT( values.count() == this->nRows() );

  for( int i = 0; i < this->nRows(); ++i ) {
    _data[i].resize( _nCols + 1 );
    _data[i][_nCols] = CSpreadsheetCell( values.at(i) );
  }

  ++_nCols;

  if( this->hasColNames() ) {
    QString newColName = QStringLiteral( "Column_%1" ).arg( _nCols );
    Q_ASSERT( !_colNamesLookup.contains( newColName.toLower().trimmed() ) );
    _colNames.append( newColName );
    _colNamesLookup.insert( newColName.toLower().trimmed(), _nCols - 1 );
  }
}


void CSpreadsheet::appendColumn( const QList<QVariant>& values ) {
  Q_ASSERT( values.count() == this->nRows() );

  for( int i = 0; i < this->nRows(); ++i ) {
    _data[i].resize( _nCols + 1 );
    _data[i][_nCols] = CSpreadsheetCell( values.at(i) );
  }

  ++_nCols;

  if( this->hasColNames() ) {
    QString newColName = QStringLiteral( "Column_%1" ).arg( _nCols );
    Q_ASSERT( !_colNamesLookup.contains( newColName.toLower().trimmed() ) );
    _colNames.append( newColName );
    _colNamesLookup.insert( newColName.toLower().trimmed(), _nCols - 1 );
  }
}


void CSpreadsheet::appendColumn( const QString& colName, const QVariant& defaultVal ) {
  for( int i = 0; i < this->nRows(); ++i ) {
    _data[i].resize( _nCols + 1 );
    if( _useDefaultVal ) {
      _data[i][_nCols] = CSpreadsheetCell( defaultVal );
    }
  }

  ++_nCols;

  Q_ASSERT( !_colNamesLookup.contains( colName.toLower().trimmed() ) );
  _colNames.append( colName );
  _colNamesLookup.insert( colName.toLower().trimmed(), _nCols - 1 );
}


void CSpreadsheet::appendColumn( const QString& colName, const QVector<QVariant>& values ) {
  Q_ASSERT( values.count() == this->nRows() );

  for( int i = 0; i < this->nRows(); ++i ) {
    _data[i].resize( _nCols + 1 );
    _data[i][_nCols] = CSpreadsheetCell( values.at(i) );
  }

  ++_nCols;

  Q_ASSERT( !_colNamesLookup.contains( colName.toLower().trimmed() ) );
  _colNames.append( colName );
  _colNamesLookup.insert( colName.toLower().trimmed(), _nCols - 1 );
}


void CSpreadsheet::appendColumn( const QString& colName, const QList<QVariant>& values ) {
  Q_ASSERT( values.count() == this->nRows() );

  for( int i = 0; i < this->nRows(); ++i ) {
    _data[i].resize( _nCols + 1 );
    _data[i][_nCols] = CSpreadsheetCell( values.at(i) );
  }

  ++_nCols;

  Q_ASSERT( !_colNamesLookup.contains( colName.toLower().trimmed() ) );
  _colNames.append( colName );
  _colNamesLookup.insert( colName.toLower().trimmed(), _nCols - 1 );
}


void CSpreadsheet::setData( const CTwoDArray<QVariant>& data ) {
  int rowOffset = 0;
  int colOffset = 0;

  if( data.hasColNames() ) {
    ++rowOffset;
  }

  if( data.hasRowNames() ) {
    ++colOffset;
  }

  this->setSize( data.nCols()+colOffset, data.nRows()+rowOffset );

  if( data.hasColNames() ) {
    for( int c = 0; c < data.colNames().count(); ++c ) {
      this->setValue( c+colOffset, 0, CSpreadsheetCell( data.colNames().at(c) ) );
    }
  }

  if( data.hasRowNames() ) {
    for( int r = 0; r < data.rowNames().count(); ++r ) {
      this->setValue( 0, r+rowOffset, CSpreadsheetCell( data.rowNames().at(r) ) );
    }
  }

  for( int c = 0; c < data.nCols(); ++c ) {
    for( int r = 0; r < data.nRows(); ++r ) {
      this->setValue( c+colOffset, r+rowOffset, CSpreadsheetCell( data.at( c, r ) ) );
    }
  }
}


bool CSpreadsheet::setDataType( const QMetaType::Type type, const int firstCol /* = 0 */, const int firstRow /* = 0 */ ) {
  bool result = true;

  for( int c = firstCol; c < this->nCols(); ++c ) {
    for( int r = firstRow; r < this->nRows(); ++r ) {
      result = ( result && this->at(c,r).setDataType( type ) );
    }
  }

  return result;
}


bool CSpreadsheet::addCellValues( const CSpreadsheet& other, const int firstCol /* = 0 */, const int firstRow /* = 0 */ ) {
  bool result = true;

  for( int c = firstCol; c < this->nCols(); ++c ) {
    for( int r = firstRow; r < this->nRows(); ++r ) {
      if(  this->value( c, r ).isNumeric() && other.value( c, r ).isNumeric() ) {
        switch( this->cellValue( c, r ).type() ) {
          case QVariant::Int:
            this->setValue( c, r, CSpreadsheetCell( this->cellValue( c, r ).toInt() + other.cellValue( c, r ).toInt() ) );
            break;
          case QVariant::Double:
            this->setValue( c, r, CSpreadsheetCell( this->cellValue( c, r ).toDouble() + other.cellValue( c, r ).toDouble() ) );
            break;
          case QVariant::UInt:
            this->setValue( c, r, CSpreadsheetCell( this->cellValue( c, r ).toUInt() + other.cellValue( c, r ).toUInt() ) );
            break;
          case QVariant::LongLong:
            this->setValue( c, r, CSpreadsheetCell( this->cellValue( c, r ).toLongLong() + other.cellValue( c, r ).toLongLong() ) );
            break;
          case QVariant::ULongLong:
            this->setValue( c, r, CSpreadsheetCell( this->cellValue( c, r ).toULongLong() + other.cellValue( c, r ).toULongLong() ) );
            break;
          default:
            Q_UNREACHABLE();
            _errMsg.append( QStringLiteral("Numeric data type not yet implemented.\n") );
            break;
        }
      }
      else {
        result = false;
      }
    }
  }

  return result;
}


bool CSpreadsheet::subtractCellValues( const CSpreadsheet& other, const int firstCol /* = 0 */, const int firstRow /* = 0 */ ) {
  bool result = true;

  for( int c = firstCol; c < this->nCols(); ++c ) {
    for( int r = firstRow; r < this->nRows(); ++r ) {
      if(  this->value( c, r ).isNumeric() && other.value( c, r ).isNumeric() ) {
        switch( this->cellValue( c, r ).type() ) {
          case QVariant::Int:
            this->setValue( c, r, CSpreadsheetCell( this->cellValue( c, r ).toInt() - other.cellValue( c, r ).toInt() ) );
            break;
          case QVariant::Double:
            this->setValue( c, r, CSpreadsheetCell( this->cellValue( c, r ).toDouble() - other.cellValue( c, r ).toDouble() ) );
            break;
          case QVariant::UInt:
            this->setValue( c, r, CSpreadsheetCell( this->cellValue( c, r ).toUInt() - other.cellValue( c, r ).toUInt() ) );
            break;
          case QVariant::LongLong:
            this->setValue( c, r, CSpreadsheetCell( this->cellValue( c, r ).toLongLong() - other.cellValue( c, r ).toLongLong() ) );
            break;
          case QVariant::ULongLong:
            this->setValue( c, r, CSpreadsheetCell( this->cellValue( c, r ).toULongLong() - other.cellValue( c, r ).toULongLong() ) );
            break;
          default:
            Q_UNREACHABLE();
            _errMsg.append( QStringLiteral("Numeric data type not yet implemented.\n") );
            break;
        }
      }
      else {
        result = false;
      }
    }
  }

  return result;
}


bool CSpreadsheet::multiplyCellValues( const CSpreadsheet& other, const int firstCol /* = 0 */, const int firstRow /* = 0 */ ) {
  bool result = true;

  for( int c = firstCol; c < this->nCols(); ++c ) {
    for( int r = firstRow; r < this->nRows(); ++r ) {
      if(  this->value( c, r ).isNumeric() && other.value( c, r ).isNumeric() ) {
        this->setValue( c, r, CSpreadsheetCell( this->cellValue( c, r ).toDouble() * other.cellValue( c, r ).toDouble() ) );
      }
      else {
        result = false;
      }
    }
  }

  return result;
}


bool CSpreadsheet::divideCellValues( const CSpreadsheet& other, const int firstCol /* = 0 */, const int firstRow /* = 0 */ ) {
  bool result = true;

  for( int c = firstCol; c < this->nCols(); ++c ) {
    for( int r = firstRow; r < this->nRows(); ++r ) {
      if(  this->value( c, r ).isNumeric() && other.value( c, r ).isNumeric() ) {
        this->setValue( c, r, CSpreadsheetCell( this->cellValue( c, r ).toDouble() / other.cellValue( c, r ).toDouble() ) );
      }
      else {
        result = false;
      }
    }
  }

  return result;
}


bool CSpreadsheet::roundCellValues( const int firstCol /* = 0 */, const int firstRow /* = 0 */ ) {
  bool result = true;

  for( int c = firstCol; c < this->nCols(); ++c ) {
    for( int r = firstRow; r < this->nRows(); ++r ) {
      if(  this->value( c, r ).isNumeric() ) {
        if( QVariant::Double == this->cellValue( c, r ).type() ) {
          this->setValue( c, r, CSpreadsheetCell( int( round( this->cellValue( c, r ).toDouble() ) ) ) );
        }
        else {
          // Don't do anything: it's already an integer.
        }
      }
      else {
        result = false;
      }
    }
  }

  return result;
}


bool CSpreadsheet::addToCellValues( const int val, const int firstCol /* = 0 */, const int firstRow /* = 0 */ ) {
  bool result = true;

  for( int c = firstCol; c < this->nCols(); ++c ) {
    for( int r = firstRow; r < this->nRows(); ++r ) {
      if(  this->value( c, r ).isNumeric() ) {
        switch( this->cellValue( c, r ).type() ) {
          case QVariant::Int:
            this->setValue( c, r, CSpreadsheetCell( this->cellValue( c, r ).toInt() + val ) );
            break;
          case QVariant::Double:
            this->setValue( c, r, CSpreadsheetCell( this->cellValue( c, r ).toDouble() + val ) );
            break;
          case QVariant::UInt:
            this->setValue( c, r, CSpreadsheetCell( this->cellValue( c, r ).toUInt() + val ) );
            break;
          case QVariant::LongLong:
            this->setValue( c, r, CSpreadsheetCell( this->cellValue( c, r ).toLongLong() + val ) );
            break;
          case QVariant::ULongLong:
            this->setValue( c, r, CSpreadsheetCell( this->cellValue( c, r ).toULongLong() + val ) );
            break;
          default:
            Q_UNREACHABLE();
            _errMsg.append( QStringLiteral("Numeric data type not yet implemented.\n") );
            break;
        }
      }
      else {
        result = false;
      }
    }
  }

  return result;
}


bool CSpreadsheet::subtractFromCellValues( const int val, const int firstCol /* = 0 */, const int firstRow /* = 0 */ ) {
  return addToCellValues( -1 * val, firstCol, firstRow );
}


void CSpreadsheet::assignColNamesFromRow( const int rowIdx ) {
  Q_ASSERT( rowIdx < this->rowCount() );

  // DO NOT alter the underlying data.  Just set the column names.

  setColNames( rowAsStringList( rowIdx, true ) );
}


CTwoDArray<QVariant> CSpreadsheet::data(const bool firstRowContainsHeader ) {
  CTwoDArray<QVariant> result;

  // firstRowIsHeader indicates whether the first row of a spreadsheet contains column names.

  if(!this->isTidy( firstRowContainsHeader ) ) {
    appLog << "Tidy check failed.";
  }
  else {
    int nCols = this->nCols();
    int nRows = this->nRows();
    int rowOffset = 0;

    if( firstRowContainsHeader ) {
      --nRows;
      ++rowOffset;
      assignColNamesFromRow( 0 );
    }

    result.setSize( nCols, nRows );

    for( int c = 0; c < nCols; ++c ) {
      for( int r = 0; r < nRows; ++r ) {
        result.setValue( c, r, this->cellValue( c, r + rowOffset ) );
      }
    }
  }

  return result;
}


bool CSpreadsheet::isTidy( const bool firstRowContainsHeader, const int firstRowIdx /* = 0 */, const int nRows /* = -1 */ ) {
  bool result = true;

  // Criteria for tidy spreadsheets:
  //  1. No merged cells.
  //  2. If the first row is a header:
  //    a) It should not contain any blanks (blanks at the end are OK)
  //    b) Every value in the first row should be a string(?)
  //    c) For all subsequent rows, there cannot be more columns than in the header row.

  if( this->isEmpty() ) {
    // An empty sheet can be tidy if it does not contain a header row.
    // If an empty sheet should have contained a header row, then it isn't tidy.
    return !firstRowContainsHeader;
  }
  else if( this->hasMergedCells() ) {
    appLog << "Worksheet is not tidy: Merged cells are present.";
    result = false;
  }
  else if( firstRowContainsHeader ) {
    // Check the header row
    //---------------------
    assignColNamesFromRow( firstRowIdx );

    for( int c = 0; c < colNames().length(); ++c ) {
      //qDebug()
      //  << c
      //  << this->at( c, firstRowIdx ).value()
      //  << this->at( c, firstRowIdx ).value().isNull()
      //  << ( QVariant::String != this->at( c, firstRowIdx ).value().type() )
      //  << this->at( c, firstRowIdx ).value().toString().length()
      //;

      if(
            this->at( c, firstRowIdx ).value().isNull()
         || ( QVariant::String != this->at( c, firstRowIdx ).value().type() )
         || ( 0 == this->at( c, firstRowIdx ).value().toString().length() )
       ) {
        result = false;
      }
    }

    if( false == result ) {
      appLog << "Worksheet is not tidy: There is a problem with the header row.";
      return result;
    }

    // Check all subsequent rows
    //--------------------------
    int lastRowIdxPlusOne;

    if( -1 == nRows )
      lastRowIdxPlusOne = this->nRows();
    else
      lastRowIdxPlusOne = firstRowIdx + nRows;


    if( 1 < this->nRows() ) {
      for( int r = firstRowIdx+1; r < lastRowIdxPlusOne; ++r ) {
        if( this->rowAsStringList( r, true ).length() > colNames().length() ) {
          appLog <<"Worksheet is not tidy: Header and data row lengths do not match.";
          result = false;
          break;
        }

      }
    }
  }

  return result;
}


QVariantList CSpreadsheet::rowAsVariantList( const int rowNumber, const bool removeTrailingBlanks /*= false */ ) const {
  QVariantList list;

  for( int c = 0; c < this->nCols(); ++c ) {
    list.append( this->at( c, rowNumber ).value() );
  }

  if( removeTrailingBlanks ) {
    bool ok = false;
    while( !ok ) {
      if( !list.last().isNull() ) {
        ok = true;
      }
      else {
        list.removeLast();
      }
    }
  }

  return list;
}


QStringList CSpreadsheet::rowAsStringList( const int rowNumber, const bool removeTrailingBlanks /*= false */ ) const {
  QStringList list;

  for( int c = 0; c < this->nCols(); ++c ) {
    if( QVariant::DateTime == this->at( c, rowNumber ).value().type() )
      list.append( this->at( c, rowNumber ).value().toDateTime().toString( QStringLiteral("yyyy-MM-dd hh:mm:ss") ) );
    else
      list.append( this->at( c, rowNumber ).value().toString().trimmed() );
  }

  if( removeTrailingBlanks ) {
    bool ok = false;
    while( !ok ) {
      if( 0 < list.last().trimmed().length() ) {
        ok = true;
      }
      else {
        list.removeLast();
      }
    }
  }

  return list;
}


QStringList CSpreadsheet::columnAsStringList( const int colNumber, const bool removeTrailingBlanks /*= false */ ) const {
  QStringList list;

  for( int r = 0; r < this->nRows(); ++r ) {
    if( QVariant::DateTime == this->at( colNumber, r ).value().type() )
      list.append( this->at( colNumber, r ).value().toDateTime().toString( QStringLiteral("yyyy-MM-dd hh:mm:ss") ) );
    else
      list.append( this->at( colNumber, r ).value().toString().trimmed() );
  }

  if( removeTrailingBlanks ) {
    bool ok = false;
    while( !ok ) {
      if( 0 < list.last().trimmed().length() ) {
        ok = true;
      }
      else {
        list.removeLast();
      }
    }
  }

  return list;
}


QVariantList CSpreadsheet::columnAsVariantList( const int colNumber, const bool removeTrailingBlanks /*= false */ ) const {
  QVariantList list;

  for( int r = 0; r < this->nRows(); ++r ) {
    list.append( this->at( colNumber, r ).value() );
  }

  if( removeTrailingBlanks ) {
    bool ok = false;
    while( !ok ) {
      if( !list.last().isNull() ) {
        ok = true;
      }
      else {
        list.removeLast();
      }
    }
  }

  return list;
}


bool CSpreadsheet::writeCsv( const QString& fileName, const bool firstRowContainsHeader /* = true */, const QChar delimiter /* = ',' */ ) {
  _errMsg.clear();

  if( this->isEmpty() ) {
    _errMsg.append( QStringLiteral("Specified worksheet is empty.\n") );
    return false;
  }
  else if( !this->isTidy( firstRowContainsHeader ) ) {
    _errMsg.append( QStringLiteral("Specified worksheet does not have a tidy CSV format.\n") );
    return false;
  }
  else {
    QCsv csv = this->asCsv( firstRowContainsHeader, delimiter );
    return csv.writeFile( fileName );
  }
}


bool CSpreadsheet::displayTable( QTextStream* stream ) {
  QCsv csv = this->asCsv( this->hasColNames() );
  return csv.displayTable( stream );
}


QCsv CSpreadsheet::asCsv( const bool firstRowContainsHeader, const QChar delimiter /* = ',' */, const int firstRowIdx /* = 0 */, const int nRows /* = -1 */ ) {
  QCsv csv;

  if( this->isEmpty() ) {
    csv.setError( QCsv::ERROR_OTHER, QStringLiteral("Specified worksheet is empty.") );
  }

  // if firstRowContainsHeader, isTidy() will set colNames()
  else if( !this->isTidy( firstRowContainsHeader, firstRowIdx, nRows ) ) {
    csv.setError( QCsv::ERROR_OTHER, QStringLiteral("Specified worksheet does not have a tidy CSV format.") );
  }

  else {
    QStringList firstRow;
    QList<QStringList> data;
    int nCols;

    // Determine how many empty columns to trim from the right end of the sheet
    //-------------------------------------------------------------------------
    if( this->hasColNames() ) {
      nCols = this->colNames().count();
      firstRow = this->rowAsStringList( firstRowIdx ).mid( 0, nCols );
    }
    else {
      firstRow = this->rowAsStringList( firstRowIdx, true );
      nCols = firstRow.count();
    }

    // Build the csv object
    //----------------------
    int lastRowIdxPlusOne;

    if( -1 == nRows )
      lastRowIdxPlusOne = this->nRows();
    else
      lastRowIdxPlusOne = firstRowIdx + nRows;

    for( int i = firstRowIdx+1; i < lastRowIdxPlusOne; ++i ) {
      QStringList tmp = this->rowAsStringList( i );
      data.append( tmp.mid( 0, nCols ) );
    }

    if( !firstRowContainsHeader ) {
      data.prepend( firstRow );
    }

    // Trim off trailing empty rows
    while( !data.isEmpty() && isEmptyStringList( data.last() ) ) {
      data.removeLast();
    }

    if( this->hasColNames() ) {
      csv = QCsv( this->colNames(), data );
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


bool CSpreadsheet::writeXlsx( const QString& fileName, const bool treatEmptyStringsAsNull, const bool useColNames, const bool useRowNames ) {
  QXlsx::Document xlsx;

  QXlsx::Format format;
  format.setHorizontalAlignment( QXlsx::Format::AlignLeft );
  format.setVerticalAlignment( QXlsx::Format::AlignTop );

  int firstColIdx = 0;
  int firstRowIdx = 0;

  if( useColNames && this->hasColNames() ) {
    for( int c = 0; c < this->nCols(); ++c ) {
      xlsx.write( 1, c+1, this->colNames().at(c) );
    }
    ++firstRowIdx;
  }

  if( useRowNames && this->hasRowNames() ) {
    ++firstColIdx;
  }

  for( int c = 0; c < this->nCols(); ++c ) {
    for( int r = 0; r < this->nRows(); ++r ) {
      QVariant tmp;

      if( !treatEmptyStringsAsNull || !isNullOrEmpty( this->value( c, r ).value() ) ) {
        tmp = this->value( c, r ).value();
      }

      if( useRowNames && this->hasRowNames() ) {
        xlsx.write( r+1, c+firstColIdx+1, this->rowNames().at(r) );
      }

      xlsx.write( r+firstRowIdx+1, c+firstColIdx+1, tmp );

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


bool CSpreadsheet::readXlsx(
  const QString& sheetName,
  QXlsx::Document* xlsx
  #ifdef DEBUG
    , const bool displayVerboseOutput /* = false */
  #endif
) {
  if( !xlsx->selectSheet( sheetName ) ) {
    #ifdef DEBUG
      if( displayVerboseOutput )
        cout << QStringLiteral( "Specified worksheet (%1) could not be selected." ).arg( sheetName ) << endl;
    #endif
    emit operationError();

    #ifndef QCONCURRENT_USED
      QCoreApplication::processEvents();
    #endif

    return false;
  }
  #ifdef DEBUG
    if( displayVerboseOutput )
      cout << "Worksheet is open." << endl;
  #endif

  QXlsx::CellRange cellRange = xlsx->dimension();
  if( (0 >= cellRange.firstRow()) || (0 >= cellRange.firstColumn()) || (0 >= cellRange.lastRow()) || (0 >= cellRange.lastColumn()) ) {
    qDb() << "Cell range is out of bounds.";
    emit operationError();

    #ifndef QCONCURRENT_USED
      QCoreApplication::processEvents();
    #endif

    return false;
  }
  #ifdef DEBUG
    if( displayVerboseOutput )
      cout << QStringLiteral( "Cell range: rows( %1, %2 ), columns (%3, %4)" )
        .arg( QString::number( cellRange.firstRow() ), QString::number( cellRange.lastRow() ), QString::number( cellRange.firstColumn() ), QString::number( cellRange.lastColumn() ) )
      << endl;
  #endif

  this->setSize( cellRange.lastColumn(), cellRange.lastRow(), CSpreadsheetCell() );

  emit operationStart( QStringLiteral("Reading rows in sheet"), cellRange.lastRow() + 1 );

  #ifndef QCONCURRENT_USED
    QCoreApplication::processEvents();
  #endif

  for( int row = 1; row < (cellRange.lastRow() + 1); ++row ) {
    for( int col = 1; col < (cellRange.lastColumn() + 1); ++col ) {

      QVariant val = xlsx->read( row, col );

      if( val.type() == QVariant::String ) {
        val = val.toString().replace( QLatin1String("_x000D_\n"), QLatin1String("\n") );
      }

      CSpreadsheetCell ssCell( val, 0, 0 );
      this->setValue( col - 1, row - 1, ssCell );
    }

    emit operationProgress( row );
    #ifndef QCONCURRENT_USED
      QCoreApplication::processEvents();
    #endif

    if( ( nullptr != _terminatedPtr ) && *_terminatedPtr ) {
      break;
    }
  }

  // Empty spreadsheets of this type report that they have a single cell, but the cell value is null.
  // If that's the case, make sure that the data structure really is empty.
  if( ( 1 == this->nCols() ) && ( 1 == this->nRows() ) && this->cellValue( 0, 0 ).isNull() ) {
    #ifdef DEBUG
      if( displayVerboseOutput )
        cout << "Worksheet is empty, read successfully." << endl;
    #endif

    this->clear();
    emit operationComplete();

    #ifndef QCONCURRENT_USED
      QCoreApplication::processEvents();
    #endif

    return true;
  }

  emit operationComplete();

  #ifndef QCONCURRENT_USED
    QCoreApplication::processEvents();
  #endif

  if( ( nullptr != _terminatedPtr ) && *_terminatedPtr ) {
    return true;
  }

  // Deal with merged cells
  QList<QXlsx::CellRange> mergedCells = xlsx->currentWorksheet()->mergedCells();

  _mergedCellRefs.clear();

  if( !mergedCells.isEmpty() ) {
    emit operationStart( QStringLiteral("Handling merged ranges in sheet"), mergedCells.count() );

    #ifndef QCONCURRENT_USED
      QCoreApplication::processEvents();
    #endif

    int originCol, originRow;
    int rowSpan, colSpan;

    for( int i = 0; i < mergedCells.count(); ++i ) {
      originRow = mergedCells.at(i).firstRow() - 1;
      originCol = mergedCells.at(i).firstColumn() - 1;

      rowSpan = mergedCells.at(i).lastRow() - originRow;
      colSpan = mergedCells.at(i).lastColumn() - originCol;

      this->value( originCol, originRow )._colSpan = colSpan;
      this->value( originCol, originRow )._rowSpan = rowSpan;

      this->value( originCol, originRow )._isPartOfMergedRow = ( 1 < colSpan );
      this->value( originCol, originRow )._isPartOfMergedCol = ( 1 < rowSpan );

      _mergedCellRefs.insert( CCellRef( originCol, originRow ) );

      emit operationProgress( i );

      #ifndef QCONCURRENT_USED
        QCoreApplication::processEvents();
      #endif
    }

    emit operationComplete();

    #ifndef QCONCURRENT_USED
      QCoreApplication::processEvents();
    #endif

    if( ( nullptr != _terminatedPtr ) && *_terminatedPtr ) {
      return true;
    }

    flagMergedCells();
  }

  #ifdef DEBUG
    if( displayVerboseOutput )
      cout << "Worksheet has been read successfully." << endl;
  #endif

  return true;
}


bool CSpreadsheet::readXls(
  const int sheetIdx,
  xls::xlsWorkBook* pWB
  #ifdef DEBUG
    , const bool displayVerboseOutput /* = false */
  #endif
) {
  // Open and parse the sheet
  //=========================
  xls::xlsWorkSheet* pWS = xls::xls_getWorkSheet( pWB, sheetIdx );
  xls::xls_parseWorkSheet( pWS );

  #ifndef QCONCURRENT_USED
    QCoreApplication::processEvents();
  #endif

  // Process all cells of the sheet
  //===============================
  xlsWORD row, col;

  emit operationStart( QStringLiteral("Reading rows in sheet"), pWS->rows.lastrow + 1 );

  #ifndef QCONCURRENT_USED
    QCoreApplication::processEvents();
  #endif

  this->setSize( pWS->rows.lastcol, pWS->rows.lastrow + 1, CSpreadsheetCell() );

  #ifdef DEBUG
    if( displayVerboseOutput )
      cout << QStringLiteral( "Cell range: rows( %1, %2 ), columns (%3, %4)" )
        .arg( QString::number( 1 ), QString::number( pWS->rows.lastrow + 1 ), QString::number( 1 ), QString::number( pWS->rows.lastcol ) )
      << endl;
  #endif

  _mergedCellRefs.clear();

  for( row = 0; row <= pWS->rows.lastrow; ++row ) {
    for( col = 0; col < pWS->rows.lastcol; ++col ) {

      xls::xlsCell* cell = xls::xls_cell( pWS, row, col );

      if( !cell ) {
        continue;
      }
      else if( cell->isHidden ) {
        continue;
      }
      else {
        #ifdef DEBUG
          QString msg;
        #endif

        QVariant val = processCellXls(
          cell,
          _wb
          #ifdef DEBUG
            , msg
            , displayVerboseOutput
          #endif
        );

        CSpreadsheetCell ssCell( val, cell->colspan, cell->rowspan );
        this->setValue( col, row, ssCell );

        // Make a note if the cell is merged.
        if( ssCell.hasSpan() ) {
          _mergedCellRefs.insert( CCellRef( col, row ) );
        }

        #ifdef DEBUG
          if( displayVerboseOutput ) {
            msg.replace( QLatin1String("CELLCOL"), QString::number( col ), Qt::CaseSensitive );
            msg.replace( QLatin1String("CELLROW"), QString::number( row ), Qt::CaseSensitive );
            cout << msg << endl;
          }
        #endif
      }
    }

    emit operationProgress( row );

    #ifndef QCONCURRENT_USED
      QCoreApplication::processEvents();
    #endif

    if( ( nullptr != _terminatedPtr ) && *_terminatedPtr ) {
      break;
    }
  }

  if( ( nullptr != _terminatedPtr ) && *_terminatedPtr ) {
    return true;
  }

  if( this->hasMergedCells() ) {
    flagMergedCells();
  }

  #ifdef DEBUG
    if( displayVerboseOutput )
      cout << "Worksheet has been read successfully." << endl;
  #endif

  emit operationComplete();

  #ifndef QCONCURRENT_USED
    QCoreApplication::processEvents();
  #endif

  return true;
}


QVariant CSpreadsheet::processCellXls(
    xls::xlsCell* cell,
    CSpreadsheetWorkBook* wb
    #ifdef DEBUG
      , QString& msg
      , const bool displayVerboseOutput
    #endif
) {
  QVariant val;

  // Display the value of the cell (either numeric or string)
  //=========================================================

  // Deal with numbers
  //------------------
  #ifdef DEBUG
    if( displayVerboseOutput ) {
      msg =
        QStringLiteral( "Row: CELLROW, Col: CELLCOL, id: %1, row: %2, col, %3, xf: %4, l: %5, d: %6, str: %7\n" )
        .arg( QStringLiteral( "%1" ).arg( cell->id, 0, 16 ).toUpper().prepend( "0x" ) )
        .arg( cell->row )
        .arg( cell->col )
        .arg( cell->xf )
        .arg( cell->l )
        .arg( cell->d )
        .arg( cell->str )
      ;
    }
  #endif

  // XLS_RECORD_RK = 0x027E
  // XLS_RECORD_NUMBER = 0x203
  // XLS_RECORD_MULRK = 0x00BD
  if( ( XLS_RECORD_RK == cell->id ) || ( XLS_RECORD_MULRK == cell->id ) || ( XLS_RECORD_NUMBER == cell->id ) ) {
    if( wb->isXlsDate( cell->xf, cell->d ) ) {
      val = xlsDate( int( cell->d ), wb->isXls1904DateSystem() );
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

    #ifdef DEBUG
      if( displayVerboseOutput ) {
        msg.append( QStringLiteral( "Row: CELLROW, Col: CELLCOL, Value (numeric): %1, ID: %2" ).arg( val.toString(), QStringLiteral( "%1" ).arg( cell->id, 0, 16 ).toUpper().prepend( "0x" ) ) );
      }
    #endif
  }

  // Deal with formulas
  //-------------------
  // XLS_RECORD_FORMULA = 0x0006
  // XLS_RECORD_FORMULA_ALT = 0x0406
  else if ( (XLS_RECORD_FORMULA == cell->id) || (XLS_RECORD_FORMULA_ALT == cell->id) ) {
    if (cell->l == 0) { // its a number
      val = cell->d;
      #ifdef DEBUG
        if( displayVerboseOutput ) {
          msg.append( QStringLiteral( "Row: CELLROW, Col: CELLCOL, Value (formula numeric): %1, ID: %2" ).arg( val.toString(), QStringLiteral( "%1" ).arg( cell->id, 0, 16 ).toUpper().prepend( "0x" ) ) );
        }
      #endif
    }
    else {
      if (!strcmp( cell->str, "bool")) { // its boolean, and test cell->d
        if( 1 == int( cell->d ) ) {
          val = true;
        }
        else {
          val = false;
        }
        #ifdef DEBUG
          if( displayVerboseOutput ) {
            msg.append( QStringLiteral( "Row: CELLROW, Col: CELLCOL, Value (formula boolean): %1, ID: %2" ).arg( val.toString(), QStringLiteral( "%1" ).arg( cell->id, 0, 16 ).toUpper().prepend( "0x" ) ) );
          }
        #endif
      }
      else if (!strcmp( cell->str, "error" ) ) { // formula is in error
        val = "*error*";
        #ifdef DEBUG
          if( displayVerboseOutput ) {
            msg.append( QStringLiteral( "Row: CELLROW, Col: CELLCOL, Value (formula error): %1, ID: %2" ).arg( val.toString(), QStringLiteral( "%1" ).arg( cell->id, 0, 16 ).toUpper().prepend( "0x" ) ) );
          }
        #endif
      }
      else { // ... cell->str is valid as the result of a string formula.
        val = QStringLiteral( "%1" ).arg( cell->str );
        #ifdef DEBUG
          if( displayVerboseOutput ) {
            msg.append( QStringLiteral( "Row: CELLROW, Col: CELLCOL, Value (formula string): %1" ).arg( val.toString() ) );
          }
        #endif
      }
    }
  }

  // Deal with booleans
  //-------------------
  // XLS_RECORD_BOOLERR = 0x0205
  else if( XLS_RECORD_BOOLERR == cell->id ) {
    if( 0 == int( cell->d ) ) {
      val = false;
    }
    else {
      val = true;
    }
  }


  // Deal with strings
  //------------------
  else if( nullptr != cell->str ) {
     val = QStringLiteral( "%1" ).arg( cell->str );
     #ifdef DEBUG
       if( displayVerboseOutput ) {
        msg.append( QStringLiteral( "Row: CELLROW, Col: CELLCOL, Value (string): %1" ).arg( val.toString() ) );
       }
     #endif
  }

  // Deal with 'empty' cells
  //------------------------
  else {
    #ifdef DEBUG
      if( displayVerboseOutput ) {
        msg.append( QStringLiteral( "Row: CELLROW, Col: CELLCOL, (Empty cell)" ) );
      }
    #endif
  }

  #ifdef DEBUG
    if( displayVerboseOutput ) {
      msg.append( QStringLiteral( ", colspan: %1, rowspan %2" ).arg( cell->colspan ).arg( cell->rowspan ) );
    }
  #endif

  return val;
}


bool CSpreadsheet::readCsv(
  const QString& fileName
  #ifdef DEBUG
    , const bool displayVerboseOutput /* = false */
  #endif
) {
  QCsv csv( fileName, true, true, QCsv::EntireFile );

  return readCsv( csv );
}


bool CSpreadsheet::readCsv(
  QCsv& csv
  #ifdef DEBUG
    , const bool displayVerboseOutput /* = false */
  #endif
) {
  if( !csv.open() ) {
    return false;
  }

  this->setSize( csv.fieldCount(), csv.rowCount() );
  this->setColNames( csv.fieldNames() );
  for( int c = 0; c < csv.fieldCount(); ++c ) {
    for( int r = 0; r < csv.rowCount(); ++r ) {
      this->setValue( c, r, CSpreadsheetCell( csv.field( c, r ) ) );
    }
  }

  return true;
}


void CSpreadsheet::flagMergedCells() {
  // Cells that span multiple columns are part of a merged ROW.
  // Cells that span multiple rows are part of a merged COLUMN.

  int c, r, firstCol, lastCol, firstRow, lastRow;

  emit operationStart( QStringLiteral("Handling merged ranges in sheet"), _mergedCellRefs.count() );

  #ifndef QCONCURRENT_USED
    QCoreApplication::processEvents();
  #endif

  int i = 0;
  foreach( CCellRef ref, _mergedCellRefs ) {
    c = ref.col;
    r = ref.row;

    firstCol = c;
    lastCol = firstCol + this->cell( c, r ).colSpan();
    firstRow = r;
    lastRow = firstRow + this->cell( c, r ).rowSpan();

    for( int cc = firstCol; cc < lastCol; ++cc ) {
      for( int rr = firstRow; rr < lastRow; ++rr ) {
        if( this->cell(c, r).hasColSpan() ) {
          this->at( cc, rr )._isPartOfMergedRow = true;
          this->at( cc, rr )._originCell = &(this->at( c, r ));
          this->at( cc, rr )._originCellRef = CCellRef( c, r );

          // Add this cell to _originCell's collection
          this->at( cc, rr )._originCell->_linkedCellRefs.insert( CCellRef( cc, rr ) );
        }

        if( this->cell(c, r).hasRowSpan() ) {
          this->at( cc, rr )._isPartOfMergedCol = true;
          this->at( cc, rr )._originCell = &(this->at( c, r ));
          this->at( cc, rr )._originCellRef = CCellRef( c, r );

          // Add this cell to _originCell's collection
          this->at( cc, rr )._originCell->_linkedCellRefs.insert(CCellRef( cc, rr ) );
        }
      }
    }

    if( this->at( c, r )._originCell == &(this->at( c, r ) ) ) {
      // Remove this cell from _originCell's collection
      this->at( c, r )._originCell->_linkedCellRefs.remove( CCellRef( c, r ) );
      this->at( c, r )._originCell = nullptr;
      this->at( c, r )._originCellRef = CCellRef();
    }

    emit operationProgress( i );

    #ifndef QCONCURRENT_USED
      QCoreApplication::processEvents();
    #endif

    if( ( nullptr != _terminatedPtr ) && *_terminatedPtr ) {
      break;
    }

    ++i;
  }

  emit operationComplete();

  #ifndef QCONCURRENT_USED
    QCoreApplication::processEvents();
  #endif
}

void CSpreadsheet::unflagMergedCells() {
  int c, r, firstCol, lastCol, firstRow, lastRow;

  foreach( CCellRef ref, _mergedCellRefs ) {
    c = ref.col;
    r = ref.row;

    firstCol = c;
    lastCol = firstCol + this->cell( c, r ).colSpan();
    firstRow = r;
    lastRow = firstRow + this->cell( c, r ).rowSpan();

    if( this->cell(c, r).hasSpan() ) {
      this->at(c, r)._linkedCellRefs.clear();
      this->at( c, r )._originCell = nullptr;
      this->at( c, r )._originCellRef = CCellRef();

      for( int cc = firstCol; cc < lastCol; ++cc ) {
        for( int rr = firstRow; rr < lastRow; ++rr ) {
          this->at( cc, rr )._originCell = nullptr;
          this->at( cc, rr )._originCellRef = CCellRef();
          this->at( cc, rr )._isPartOfMergedRow = false;
          this->at( cc, rr )._isPartOfMergedCol = false;
        }
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

      qDb() << "C" << c << "R" << r << QString::number( qlonglong( &(this->at( c, r ) ) ), 16 )
               << "MergeC" << this->at( c, r ).isPartOfMergedCol() << "MergeR" << this->at( c, r ).isPartOfMergedRow()
               << "ColSpan" << this->at( c, r ).colSpan() << "RowSpan" << this->at( c, r ).rowSpan()
               << "Value" << this->at( c, r ).value().toString()
               << "nLinked" << this->at( c, r )._linkedCellRefs.count()
               << "OrigC" << QString::number( qlonglong( this->at( c, r )._originCell ), 16 ) << "OrigCVal" << originStr;
    }
  }
}


void CSpreadsheet::unmergeColSpans( const bool duplicateValues, QSet<int>* rowsWithMergedCells /* = nullptr */) {
  int firstCol, lastCol, firstRow, lastRow;

  QVector<CCellRef> refsToRemove;
  QVector<CCellRef> refsToAdd;

  // Look for cells that are SPAN MULTIPLE COLUMNS, and duplicate their values across all columns.

  foreach( const CCellRef ref, _mergedCellRefs ) {
    int c = ref.col;
    int r = ref.row;

    firstCol = c;
    lastCol = firstCol + this->cell( c, r ).colSpan();
    firstRow = r;
    lastRow = firstRow + this->cell( c, r ).rowSpan();

    if( this->cell(c, r).hasColSpan() ) {
      if( nullptr != rowsWithMergedCells ) {
        rowsWithMergedCells->insert( r );
      }

      for( int cc = firstCol; cc < lastCol; ++cc ) {
        for( int rr = firstRow; rr < lastRow; ++rr ) {
          if( c != cc ) {
            if( duplicateValues )
              this->at( cc, r )._value = this->at( c, r ).value();
            else
              this->at( cc, r )._value = QVariant();
          }

          this->at( cc, r )._rowSpan = this->at( c, r ).rowSpan(); // Should be same rowspan as parent row
          this->at( cc, rr )._colSpan = 1;
          this->at( cc, rr )._isPartOfMergedRow = false;

          if( this->at( cc, r ).hasSpan() ) {
            refsToAdd.append( CCellRef( cc, r ) );
          }
          if( this->at( cc, rr ).hasSpan() ) {
            refsToAdd.append( CCellRef( cc, rr ) );
          }


          if( rr == firstRow ) {
            // Remove this cell from _originCell's collection
            if( nullptr != this->at( cc, rr )._originCell ) {
              this->at( cc, rr )._originCell->_linkedCellRefs.remove( CCellRef( cc, rr ) );
              this->at( cc, rr )._originCell = nullptr;
              this->at( cc, rr )._originCellRef = CCellRef();
            }
          }
          else {
            this->at( cc, rr )._originCell = &(this->at( cc, r ));
            this->at( cc, rr )._originCellRef = CCellRef( cc, r );
            // Add this cell to _originCell's collection
            this->at( cc, rr )._originCell->_linkedCellRefs.insert( CCellRef( cc, rr ) );
          }
        }
      }
    }

    if( !this->cell(c, r).hasSpan() ) {
      refsToRemove.append( ref );
    }
  }

  foreach( const CCellRef ref, refsToRemove ) {
    _mergedCellRefs.remove( ref );
  }
  foreach( const CCellRef ref, refsToAdd ) {
    _mergedCellRefs.insert( ref );
  }
}



void CSpreadsheet::unmergeRowSpans( const bool duplicateValues, QSet<int>* colsWithMergedCells /* = nullptr */ ) {
  int firstCol, lastCol, firstRow, lastRow;

  QVector<CCellRef> refsToRemove;
  QVector<CCellRef> refsToAdd;

  // Look for cells that are SPAN MULTIPLE ROWS, and duplicate their values across all rows.
  foreach( const CCellRef ref, _mergedCellRefs ) {
    int c = ref.col;
    int r = ref.row;

    firstCol = c;
    lastCol = firstCol + this->cell( c, r ).colSpan();
    firstRow = r;
    lastRow = firstRow + this->cell( c, r ).rowSpan();

    if( this->cell(c, r).hasRowSpan() ) {
      if( nullptr != colsWithMergedCells ) {
        colsWithMergedCells->insert( c );
      }

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

          if( this->at( c, rr ).hasSpan() ) {
            refsToAdd.append( CCellRef( c, rr ) );
          }
          if( this->at( cc, rr ).hasSpan() ) {
            refsToAdd.append( CCellRef( cc, rr ) );
          }

          if( cc == firstCol ) {
            // Remove this cell from _originCell's collection
            if( nullptr != this->at( cc, rr )._originCell ) {
              this->at( cc, rr )._originCell->_linkedCellRefs.remove( CCellRef( cc, rr ) );
              this->at( cc, rr )._originCell = nullptr;
              this->at( cc, rr )._originCellRef = CCellRef();
            }
          }
          else {
            // Add this cell to _originCell's collection
            this->at( cc, rr )._originCell = &(this->at( c, rr ));
            this->at( cc, rr )._originCellRef = CCellRef( c, rr );
            this->at( cc, rr )._originCell->_linkedCellRefs.insert( CCellRef( cc, rr ) );
          }
        }
      }
    }

    if( !this->cell(c, r).hasSpan() ) {
      refsToRemove.append( ref );
    }
  }

  foreach( const CCellRef ref, refsToRemove ) {
    _mergedCellRefs.remove( ref );
  }
  foreach( const CCellRef ref, refsToAdd ) {
    _mergedCellRefs.insert( ref );
  }
}


void CSpreadsheet::unmergeColAndRowSpans(
  const bool duplicateValues,
  QSet<int>* colsWithMergedCells /* = nullptr */,
  QSet<int>* rowsWithMergedCells /* = nullptr */
) {
  unmergeColSpans( duplicateValues, rowsWithMergedCells );
  unmergeRowSpans( duplicateValues, colsWithMergedCells );
}


void CSpreadsheet::unmergeCell( const int c, const int r, const bool duplicateValues ) {
  // This should unmerge every linked cell.
  CSpreadsheetCell* parentCell;

  if( nullptr == this->at( c, r )._originCell )
    parentCell = &( this->at( c, r ) );
  else
    parentCell = this->at( c, r )._originCell;

  QVector<CCellRef> refsToRemove;

  foreach( const CCellRef cellRef, parentCell->_linkedCellRefs ) {
    CSpreadsheetCell* cell = &(this->at( cellRef.col, cellRef.row ) );

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
    cell->_originCellRef = CCellRef();

    refsToRemove.append( cellRef );
  }

  parentCell->_isPartOfMergedCol = false;
  parentCell->_isPartOfMergedRow = false;
  parentCell->_colSpan = 1;
  parentCell->_rowSpan = 1;
  parentCell->_linkedCellRefs.clear();

  refsToRemove.append( CCellRef( c, r ) );

  foreach( const CCellRef ref, refsToRemove ) {
    if( _mergedCellRefs.contains( ref ) ) {
      _mergedCellRefs.remove( ref );
    }
  }
}


void CSpreadsheet::unmergeCellsInRow( const int r, const bool duplicateValues ) {
  for( int c = 0; c < this->nCols(); ++c ) {
    this->unmergeCell( c, r, duplicateValues );
  }
}


bool CSpreadsheet::columnIsEmpty( const int c, const bool excludeHeaderRow /* = false */) {
  bool result = true; // Until shown otherwise.

  int firstRow;
  if( excludeHeaderRow )
    firstRow = 1;
  else
    firstRow = 0;

  for( int r = firstRow; r < this->nRows(); ++r ) {
    QVariant v = this->cellValue( c, r );
    if( !v.isNull() ) {
      if( QVariant::String == v.type() ) {
        if( !(v.toString().isEmpty()) ) {
          //qDb() << "Column" << c << "is not empty: value in row" << r << "(" << v.toString() << ")" << v.type() << v.toString().isEmpty();
          result = false;
          break;
        }
      }
      else {
        //qDb() << "Column" << c << "is not empty: variant is not null" << v.type();
        result = false;
        break;
      }
    }
  }

  return result;
}


bool CSpreadsheet::rowIsEmpty( const int r, const bool trimStrings /* = false */ ) {
  bool result = true; // Until shown otherwise.

  for( int c = 0; c < this->nCols(); ++c ) {
    QVariant v = this->cellValue( c, r );

    if( !v.isNull() ) {
      if( QVariant::String == v.type() ) {
        QString s = v.toString();
        if( trimStrings ) {
          s = s.trimmed();
        }

        if( !(s.isEmpty()) ) {
          //qDb() << "Row" << r << "is not empty: value in column" << c << "(" << v.toString() << ")" << v.type() << v.toString().isEmpty();
          result = false;
          break;
        }
      }
      else {
        //qDb() << "Row" << r << "is not empty: value in column" << c << "is not null" << v.type() << v.toString();
        result = false;
        break;
      }
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


bool CSpreadsheet::hasEmptyRows( const bool trimStrings /* = false */ ) {
  bool result = false; // Until shown otherwise.

  for( int r = 0; r < this->nRows(); ++r ) {
    if( this->rowIsEmpty( r, trimStrings ) ) {
      result = true;
      break;
    }
  }

  return result;
}


void CSpreadsheet::removeRow( const int rowIdx ) {
  unflagMergedCells();

  CTwoDArray<CSpreadsheetCell>::removeRow( rowIdx );

  QSet<CCellRef> newCellRefs;
  int c, r;
  foreach( CCellRef ref, _mergedCellRefs ) {
    c = ref.col;
    r = ref.row;

    if( r >= rowIdx ) {
      --r;
    }

    if( r > -1 ) {
      newCellRefs.insert( CCellRef( c, r ) );
    }
  }

  _mergedCellRefs = newCellRefs;

  flagMergedCells();
}


void CSpreadsheet::removeColumn( const int colIdx ) {
  unflagMergedCells();

  CTwoDArray<CSpreadsheetCell>::removeColumn( colIdx );

  QSet<CCellRef> newCellRefs;
  int c, r;
  foreach( CCellRef ref, _mergedCellRefs ) {
    c = ref.col;
    r = ref.row;

    if( c >= colIdx ) {
      --c;
    }

    if( c > -1 ) {
      newCellRefs.insert( CCellRef( c, r ) );
    }
  }

  _mergedCellRefs = newCellRefs;

  flagMergedCells();
}


void CSpreadsheet::removeEmptyColumns( const bool excludeHeaderRow /* = false */ ) {
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


void CSpreadsheet::removeEmptyRows( const bool trimStrings /* = false */ ) {
  QList<int> emptyRows;

  for( int r = 0; r < this->nRows(); ++r ) {
    if( this->rowIsEmpty( r, trimStrings ) ) {
      emptyRows.prepend( r );
    }
  }

  for( int i = 0; i < emptyRows.count(); ++i ) {
    this->removeRow( emptyRows.at(i) );
  }
}


void CSpreadsheet::trimEmptyRows( const bool trimStrings /* = false */ ) {
  // Remove empty rows from the start of the file
  //---------------------------------------------
  while( !this->isEmpty() ) {
    if( this->rowIsEmpty( 0, trimStrings ) ) {
      this->removeRow( 0 );
    }
    else {
      break;
    }
  }

  // Remove empty rows from the end of the file
  //-------------------------------------------
  while( !this->isEmpty() ) {
    if( this->rowIsEmpty( this->nRows() - 1, trimStrings ) ) {
      this->removeRow(  this->nRows() - 1 );
    }
    else {
      break;
    }
  }
}


void CSpreadsheet::trimEmptyColumns() {
  // Remove empty columns from the start of the file
  //------------------------------------------------
  while( !this->isEmpty() ) {
    if( this->columnIsEmpty( 0 ) ) {
      this->removeColumn( 0 );
    }
    else {
      break;
    }
  }

  // Remove empty columns from the end of the file
  //----------------------------------------------
  while( !this->isEmpty() ) {
    if( this->columnIsEmpty( this->nCols() - 1 ) ) {
      this->removeColumn(  this->nCols() - 1 );
    }
    else {
      break;
    }
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


CSpreadsheetWorkBook::CSpreadsheetWorkBook(
  const WorkBookOpenMode mode,
  const QString& fileName /* = QString() */,
  const SpreadsheetFileFormat fileFormat /* = FormatUnknown */,
  const bool* terminatedPtr /* = nullptr */,
  QObject* parent /* = nullptr */
  #ifdef DEBUG
    , const bool displayVerboseOutput = false
  #endif
) : QObject( parent ) {
  initialize();
  _terminatedPtr = terminatedPtr;

  #ifdef DEBUG
    _displayVerboseOutput = displayVerboseOutput;
  #endif

  // If no file name is specified, the file MUST be 2007 format, and MUST be in create mode.
  if( fileName.isEmpty() ) {
    if( CSpreadsheetWorkBook::ModeCreate != mode ) {
      _ok = false;
      _errMsg.append( QStringLiteral( "Cannot open an existing workbook without a file name.\n") );
    }

    if( CSpreadsheetWorkBook::Format2007 != fileFormat ) {
      _ok = false;
      _errMsg.append( QStringLiteral("Only XLSX-formatted files can be created.\n") );
    }
    else {
      _fileFormat = fileFormat;
      _isWritable = true; // FIXME: Should this be set differently by default?
      _isReadable = false;
    }
  }

  // If a file name is specified, then the format and the file's readability and writeability can be determined.
  else {
    _srcPathName = fileName;

    QFileInfo fi( _srcPathName );

    if( fi.exists() )
      _isWritable = fi.isWritable();
    else
      _isWritable = QFileInfo( fi.path() ).isWritable();

    _isReadable = ( fi.exists() && fi.isReadable() );

    if( CSpreadsheetWorkBook::FormatUnknown == fileFormat ) {
      if( !_isReadable ) {
        _ok = false;
        _errMsg.append( QStringLiteral("Cannot determine file format: file '%1' cannot be read.\n").arg( _srcPathName ) );
      }
      else {
        _fileFormat = guessFileFormat();
      }
    }
    else {
      _fileFormat = fileFormat;
    }
  }

  if( FormatUnknown == _fileFormat ) {
    _ok = false;
    _errMsg.prepend( QStringLiteral( "Cannot determine file format.\n" ) );
  }

  if( ModeUnspecified == mode ) {
    _ok = false;
    _errMsg.append( QStringLiteral("Cannot determine file mode.\n") );
  }

  if( !_ok ) {
    return;
  }

  switch( mode ) {
    // Readable files in either mode can be opened
    case ModeOpenExisting:
      if( _isReadable  ) {
        openWorkbook();
      }
      else {
        _ok = false;
        _errMsg.append( QStringLiteral("File is unreadable and cannot be opened.\n") );
      }
      break;

    // Only files in XLSX mode can be created.
    case ModeCreate:
      if( Format2007 == _fileFormat  ) {
        createWorkbook();
      }
      else {
        _ok = false;
        _errMsg.append( QStringLiteral("File cannot be created.\n") );
      }
      break;

    // Anything else will have been addressed above.
    case ModeUnspecified:
      Q_UNREACHABLE();
      break;
  }

}


void CSpreadsheetWorkBook::initialize() {
  _pWB = nullptr;
  _xlsx = nullptr;

  _fileFormat = FormatUnknown;

  _isOpen = false;
  _isReadable = false;
  _isWritable = false;

  _ok = true;
  _errMsg.clear();
}


void CSpreadsheetWorkBook::openWorkbook() {
  Q_ASSERT( nullptr == _xlsx );
  Q_ASSERT( nullptr == _pWB );

  switch( _fileFormat ) {
    case  Format97_2003:
      _ok = openXlsWorkbook();
      break;
    case Format2007:
      _ok = openXlsxWorkbook();
      break;
    default:
      _errMsg.append( QStringLiteral("Spreadsheet file format cannot be determined.\n") );
      _ok = false;
      break;
  }

  _isOpen = _ok;
}


void CSpreadsheetWorkBook::createWorkbook() {
  Q_ASSERT( nullptr == _xlsx );
  Q_ASSERT( nullptr == _pWB );

  switch( _fileFormat ) {
    case  Format97_2003:
      _errMsg.append( QStringLiteral("Cannot create an XLS format file.\n") );
      _ok = false;
      break;
    case Format2007:
      _ok = true;
      _xlsx = new QXlsx::Document();
      break;
    default:
      _errMsg.append( QStringLiteral("Spreadsheet file format cannot be determined.\n") );
      _ok = false;
      break;
  }

  _isOpen = _ok;
}


CSpreadsheetWorkBook::SpreadsheetFileFormat CSpreadsheetWorkBook::guessFileFormat() {
  return guessFileFormat( _srcPathName, &_errMsg, &_fileTypeDescr, &_ok );
}



CSpreadsheetWorkBook::SpreadsheetFileFormat CSpreadsheetWorkBook::guessFileFormat(
  const QString& fileName,
  QString* errMsg /* = nullptr */,
  QString* fileTypeDescr /* = nullptr */,
  bool* ok /* = nullptr */
 ) {
  SpreadsheetFileFormat fileFormat = FormatUnknown;
  bool error;
  QString fileType = magicFileTypeInfo( fileName, &error );

  if( nullptr != fileTypeDescr ) {
    *fileTypeDescr = fileType;
  }

  if( error ) {
    if( nullptr != errMsg )
      errMsg->append( QStringLiteral( "File type cannot be determined: there is a problem with the filemagic library.\n") );

    if( nullptr != ok )
      *ok = false;
  }

  // CSV files look like plain text
  else if( magicStringShowsAsciiTextFile( fileType ) ) {
    fileFormat = FormatCSV;
  }

  // Excel (*.xls) files look like this to FileMagic
  else if( fileType.contains( QLatin1String("Composite Document File V2 Document") ) || fileType.contains( QLatin1String("CDF V2 Document") ) ) {
    fileFormat = Format97_2003;
  }

  // Excel (*.xlsx) files look like this to FileMagic
  else if(
    ( fileType.startsWith( QLatin1String("Zip archive data") ) && fileName.endsWith( QLatin1String(".xlsx"), Qt::CaseInsensitive ) )
    // || ( fileType.startsWith( "Zip archive data" ) && fileName.endsWith( ".xls", Qt::CaseInsensitive ) ) // I think this was a mistake...
    || ( 0 == fileType.compare( QLatin1String("Microsoft Excel 2007+") ) )
    || ( fileType.contains( QLatin1String("Microsoft OOXML") ) && fileName.endsWith( QLatin1String(".xlsx"), Qt::CaseInsensitive ) )
  ) {
    fileFormat = Format2007;
  }
  else {
    if( nullptr != errMsg )
      errMsg->append( QStringLiteral( "File type cannot be matched. The filemagic library returned an unrecognized type: '%1'.\n").arg( fileType ) );

    if( nullptr != ok )
      *ok = false;
  }

  return fileFormat;
}


bool CSpreadsheetWorkBook::fileFormatIsExcel( const QString& fileName, QString* errMsg /*= nullptr*/, QString* fileTypeDescr/* = nullptr*/,  bool* ok /*= nullptr*/ ) {
  return fileFormatIsExcel( guessFileFormat( fileName, errMsg, fileTypeDescr, ok ) );
}

bool CSpreadsheetWorkBook::fileFormatIsExcel( const SpreadsheetFileFormat fmt ) {
  return ( (CSpreadsheetWorkBook::Format97_2003 == fmt) || (CSpreadsheetWorkBook::Format2007 == fmt) );
}

bool CSpreadsheetWorkBook::fileFormatIsCsv( const QString& fileName, QString* errMsg/* = nullptr*/, QString* fileTypeDescr /*= nullptr*/,  bool* ok /*= nullptr*/ ) {
  return fileFormatIsCsv( guessFileFormat( fileName, errMsg, fileTypeDescr, ok ) );
}

bool CSpreadsheetWorkBook::fileFormatIsCsv( const SpreadsheetFileFormat fmt ) {
  return( CSpreadsheetWorkBook::FormatCSV == fmt );
}



bool CSpreadsheetWorkBook::openXlsxWorkbook() {
  _xlsx = new QXlsx::Document( _srcPathName );

  for( int i = 0; i < _xlsx->sheetNames().count(); ++i ) {
    _sheetNames.insert( i, _xlsx->sheetNames().at(i) );
  }

  _xlsIs1904 = false;

  return true;
}


bool CSpreadsheetWorkBook::openXlsWorkbook() {
  // Open workbook, choose standard conversion
  //------------------------------------------
  QString encoding = QStringLiteral("UTF-8"); // "iso-8859-15//TRANSLIT" UTF-8 seems to be the new standard.
  _pWB = xls::xls_open( _srcPathName.toLatin1().data(), encoding.toLatin1().data() );

  if( nullptr == _pWB ) {
    _errMsg.append( QStringLiteral("Specified file could not be opened.  Wrong format?\n") );
    return false;
  }

  // Generate some helper bits that will let us determine further down which cells contain dates, times, or date/times
  //------------------------------------------------------------------------------------------------------------------
  _xlsIs1904 = ( 1 == _pWB->is1904 );

  _xlFormats.clear();
  for( unsigned int i = 0; i < _pWB->formats.count; ++i ) {
    _xlFormats.insert( _pWB->formats.format[i].index, _pWB->formats.format[i].value );

    #ifdef DEBUG
      if( _displayVerboseOutput )
        cout << "Format: " << "i: " << i << ", idx: " << _pWB->formats.format[i].index << ", val: " << _pWB->formats.format[i].value << endl;
    #endif
  }
  #ifdef DEBUG
    if( _displayVerboseOutput )
      cout << endl;
  #endif

  _xlXFs.clear();
  for( unsigned int i = 0; i < _pWB->xfs.count; ++i ) {
    if( 0 != _pWB->xfs.xf[i].format ) {
      _xlXFs.insert( int( i ), _pWB->xfs.xf[i].format );

      #ifdef DEBUG
        if( _displayVerboseOutput )
          cout << "XFs: " << "i: " << i << ", format: " << _pWB->xfs.xf[i].format << ", type: " << _pWB->xfs.xf[i].type << endl;
      #endif
    }
  }
  #ifdef DEBUG
    if( _displayVerboseOutput )
      cout << endl;
  #endif

  for( unsigned int i = 0; i < _pWB->sheets.count; ++i ) {
    _sheetNames.insert( int( i ), _pWB->sheets.sheet[i].name );

    #ifdef DEBUG
      if( _displayVerboseOutput )
        cout << _pWB->sheets.sheet[i].name << endl;
    #endif
  }
  #ifdef DEBUG
    if( _displayVerboseOutput )
      cout << endl;
  #endif

  return true;
}


CSpreadsheetWorkBook::~CSpreadsheetWorkBook() {
  if( nullptr != _pWB )
    xls::xls_close( _pWB );

  if( nullptr != _xlsx )
    delete _xlsx;
}


QString CSpreadsheetWorkBook::fileFormatAsString() const {
  return fileFormatAsString( _fileFormat );
}


QString CSpreadsheetWorkBook::fileFormatAsString( const SpreadsheetFileFormat fmt ) {
  QString result;

  switch( fmt ) {
    case FormatUnknown: result = QStringLiteral( "Format unknown" ); break;
    case Format2007   : result = QStringLiteral( "Microsoft Excel 2007 or later (xlsx)" ); break;
    case Format97_2003: result = QStringLiteral( "Microsoft Excel 97 - 2003 (xls, BIFF5 or BIFF8)" ); break;
    case FormatCSV    : result = QStringLiteral( "CSV file" ); break;
  }

  return result;
}


bool CSpreadsheetWorkBook::hasSheet( const int idx ) const {
  return _sheetNames.containsKey( idx );
}

bool CSpreadsheetWorkBook::hasSheet( const QString& sheetName ) const {
  return _sheetNames.containsValue( sheetName );
}

int CSpreadsheetWorkBook::sheetIndex( const QString& sheetName ) const {
  if( this->hasSheet( sheetName ) )
    return _sheetNames.retrieveKey( sheetName );
  else
    return -1;
}

QString CSpreadsheetWorkBook::sheetName( const int idx ) const {
  if( this->hasSheet( idx ) )
    return _sheetNames.retrieveValue( idx );
  else
    return QString();
}

QStringList CSpreadsheetWorkBook::sheetNames() const {
  return _sheetNames.values();
}

CSpreadsheet& CSpreadsheetWorkBook::sheet( const int idx ) {
  readSheet( idx );
  return _sheets[idx];
}

CSpreadsheet& CSpreadsheetWorkBook::sheet( const QString& sheetName ) {
  return sheet( _sheetNames.retrieveKey( sheetName ) );
}


bool CSpreadsheetWorkBook::readSheet( const int sheetIdx ) {
  _ok = true; // Until shown otherwise
  _errMsg.clear();

  if( !_isReadable ) {
    _ok = false;
    _errMsg.append( QStringLiteral("Workbook is not open.\n") );
    return false;
  }

  if( !_sheetNames.containsKey( sheetIdx ) ) {
    _ok = false;
    _errMsg.append( QStringLiteral("Specified work sheet does not exist: '%1'.\n" ).arg( sheetIdx ) );
    return false;
  }

  if( _sheets.contains( sheetIdx ) ) {
    _ok = true;
    _errMsg.append( QStringLiteral("The selected sheet has already been read: '%1'.\n" ).arg( sheetIdx ) );
    return true;
  }

  emit sheetReadName( _sheetNames.retrieveValue( sheetIdx ), sheetIdx );

  #ifndef QCONCURRENT_USED
    QCoreApplication::processEvents();
  #endif

  CSpreadsheet sheet( this, _terminatedPtr );

  connect( &sheet, SIGNAL( operationStart(QString,int) ), this, SIGNAL( operationStart(QString,int) ) );
  connect( &sheet, SIGNAL( operationProgress(int) ), this, SIGNAL( operationProgress(int) ) );
  connect( &sheet, SIGNAL( operationComplete() ), this, SIGNAL( operationComplete() ) );
  connect( &sheet, SIGNAL( operationError() ), this, SIGNAL( operationError() ) );

  switch( _fileFormat ) {
    case Format2007:
      _ok = sheet.readXlsx(
        _sheetNames.retrieveValue( sheetIdx ),
        _xlsx
        #ifdef DEBUG
          , _displayVerboseOutput
        #endif
      );
      break;
    case Format97_2003:
      _ok = sheet.readXls(
        sheetIdx,
        _pWB
        #ifdef DEBUG
          , _displayVerboseOutput
        #endif
      );
      break;
    default:
      Q_UNREACHABLE();
      _errMsg.append( QStringLiteral("Format is not specified.\n") );
      _ok = false;
      break;
  }

  if( _ok )
    _sheets.insert( sheetIdx, sheet );

  disconnect( &sheet, SIGNAL( operationStart( QString, int ) ), this, SIGNAL( operationStart( QString, int ) ) );
  disconnect( &sheet, SIGNAL( operationProgress( int ) ), this, SIGNAL( operationProgress( int ) ) );
  disconnect( &sheet, SIGNAL( operationComplete() ), this, SIGNAL( operationComplete() ) );
  disconnect( &sheet, SIGNAL( operationError() ), this, SIGNAL( operationError() ) );

  return _ok;
}


bool CSpreadsheetWorkBook::readSheet( const QString& sheetName ) {
  if( !_sheetNames.containsValue( sheetName ) ) {
    _errMsg.append( QStringLiteral("Specified work sheet does not exist: '%1'.\n" ).arg( sheetName ) );
    return false;
  }
  else {
    return readSheet( _sheetNames.retrieveKey( sheetName ) );
  }
}


bool CSpreadsheetWorkBook::readAllSheets() {
  if( !_isReadable ) {
    _errMsg.append( QStringLiteral("Workbook is not open.\n" ) );
    return false;
  }

  emit readFileStart( _sheetNames.count() );

  #ifndef QCONCURRENT_USED
    QCoreApplication::processEvents();
  #endif

  bool result = true;
  for( int i = 0; i < _sheetNames.count(); ++i ) {
    result = ( result && readSheet( i ) );
  }

  emit readFileComplete();

  #ifndef QCONCURRENT_USED
    QCoreApplication::processEvents();
  #endif

  return result;
}


bool CSpreadsheetWorkBook::isXls1904DateSystem() const {
  if( Format97_2003 != _fileFormat )
    return false;
  else
    return _xlsIs1904;
}


bool CSpreadsheetWorkBook::isXlsDate(const int xf, const double d ) const {
  if( Format97_2003 != _fileFormat ) {
    return false;
  }

  bool result;
  int fmt = _xlXFs.value( xf );

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
      QString fmtStr = _xlFormats.value( fmt );

      looksLikeDate = (
        fmtStr.contains( QLatin1String("yy") )
        || fmtStr.contains( QLatin1String("dd") )
      );

      looksLikeTime = (
        fmtStr.contains( QLatin1String("AM/PM") )
        || fmtStr.contains( QLatin1String("h") )
        || fmtStr.contains( QLatin1String("s") )
      );

      result = ( looksLikeDate && !looksLikeTime );
    }
  }

  return result;
}


bool CSpreadsheetWorkBook::isXlsTime( const int xf, const double d ) const {
  if( Format97_2003 != _fileFormat ) {
    return false;
  }

  bool result;
  int fmt = _xlXFs.value( xf );

  // Check for built-in date formats
  if( (18 <= fmt) && (21 >= fmt) ) { // Default time formats: see FORMAT (p. 174) in http://www.openoffice.org/sc/excelfileformat.pdf
    result = true;
  }
  else {
    bool looksLikeDate, looksLikeTime;

    QString fmtStr = _xlFormats.value( fmt );

    looksLikeDate = (
      ( fmtStr.contains( QLatin1String("yy") ) || fmtStr.contains( QLatin1String("dd") ) )
    );

    looksLikeTime = (
      ( fmtStr.contains( QLatin1String("AM/PM") ) || fmtStr.contains( QLatin1String("h") ) || fmtStr.contains( QLatin1String("s") ) )
      && ( 1.0 > d )
    );

    result = ( !looksLikeDate && looksLikeTime );
  }

  return result;
}


bool CSpreadsheetWorkBook::isXlsDateTime(const int xf, const double d ) const {
  Q_UNUSED( d )

  if( Format97_2003 != _fileFormat ) {
    return false;
  }

  bool result;
  int fmt = _xlXFs.value( xf );
  bool looksLikeDate, looksLikeTime;
  QString fmtStr;

  // Check for built-in date formats
  if( 22 == fmt ) { // Default date/time format: see FORMAT (p. 174) in http://www.openoffice.org/sc/excelfileformat.pdf
    result = true;
  }
  else {
    fmtStr = _xlFormats.value( fmt );

    looksLikeDate = (
      fmtStr.contains( QLatin1String("yy") )
      || fmtStr.contains( QLatin1String("dd") )
    );

    looksLikeTime = (
      fmtStr.contains( QLatin1String("AM/PM") )
      || fmtStr.contains( QLatin1String("h") )
      || fmtStr.contains( QLatin1String("s") )
    );

    result = ( looksLikeDate && looksLikeTime );
  }

  return result;
}


bool CSpreadsheetWorkBook::addSheet( const QString& sheetName /* = QString() */ ) {
  _ok = true; // Until shown otherwise
  _errMsg.clear();

  if( Format2007 != _fileFormat  ) {
    _ok = false;
    _errMsg.append( QStringLiteral("Sheets can only be added to Format2007 files.\n") );
  }
  else if( !sheetName.isEmpty() && this->hasSheet( sheetName ) ) {
    _ok = false;
    _errMsg.append( QStringLiteral("Duplicate sheet name: '%1'.\n").arg( sheetName ) );
  }
  else {
    _ok = _xlsx->addSheet( sheetName );
    if( !_ok ) {
      _errMsg.append( QStringLiteral("Could not insert sheet with name '%1'.\n").arg( sheetName ) );
    }
    else {
      _sheetNames.insert( this->sheetCount(), sheetName );
    }
  }

  return _ok;
}



bool CSpreadsheetWorkBook::deleteSheet( const int sheetIdx ) {
  _ok = true; // Until shown otherwise
  _errMsg.clear();

  if( !_sheetNames.containsKey( sheetIdx ) ) {
    _ok = false;
    _errMsg.append( QStringLiteral("Sheet does not exist: '%1'.\n" ).arg( sheetIdx ) );
  }
  else {
    _ok = this->deleteSheet( _sheetNames.retrieveValue( sheetIdx ) );
  }

  return _ok;
}


bool CSpreadsheetWorkBook::deleteSheet( const QString& sheetName ) {
  _ok = true; // Until shown otherwise
  _errMsg.clear();

  if( Format2007 != _fileFormat  ) {
    _ok = false;
    _errMsg.append( QStringLiteral("Sheets can only be deleted from Format2007 files.\n") );
  }
  else if( !this->hasSheet( sheetName ) ) {
    _ok = false;
    _errMsg.append( QStringLiteral("Sheet does not exist: '%1'.\n" ).arg( sheetName ) );
  }
  else {
    _ok = _xlsx->deleteSheet( sheetName );
    if( !_ok ) {
      _errMsg.append( QStringLiteral("Could not delete sheet '%1'." ).arg( sheetName ) );
    }
    else {
      _sheetNames.removeValue( sheetName );
    }
  }

  return _ok;
}


bool CSpreadsheetWorkBook::writeSheet( const int sheetIdx, const CTwoDArray<QVariant>& data, const bool treatEmptyStringsAsNull ) {
  if( !_isWritable ) {
    _ok = false;
    _errMsg.append( QStringLiteral("Selected workbook cannot be written. Do you have appropriate permissions?\n") );
  }
  else {
    _ok = true; // Until shown otherwise
    _errMsg.clear();

    if( Format2007 != _fileFormat  ) {
      _ok = false;
      _errMsg.append( QStringLiteral("Sheets can only be written to Format2007 files.\n") );
    }
    else if( !_sheetNames.containsKey( sheetIdx ) ) {
      _ok = false;
      _errMsg.append( QStringLiteral("Sheet does not exist: '%1'.\n" ).arg( sheetIdx ) );
    }
    else {
      _ok = this->writeSheet( _sheetNames.retrieveValue( sheetIdx ), data, treatEmptyStringsAsNull );
    }
  }

  return _ok;
}



bool CSpreadsheetWorkBook::writeBigSheet( const QString& sheetName, const CTwoDArray<QVariant>& data, const bool treatEmptyStringsAsNull ) {
  if( 1000000 < data.rowCount() ) {
    int sheetCounter = 1;
    bool result = true;
    int startRow = 0;

    while( true ) {
      int lastRow = std::min( startRow+1000000, data.rowCount() );
      int nRows = lastRow - startRow;
      result = result && writeSheet( QStringLiteral("%1_%2").arg( sheetName ).arg( sheetCounter ), data, startRow, nRows, treatEmptyStringsAsNull );
      startRow = startRow + 1000000;
      ++sheetCounter;

      if( data.rowCount() == lastRow ) {
        break;
      }
    }

    return result;
  }
  else {
    return writeSheet( sheetName, data, 0, data.rowCount(), treatEmptyStringsAsNull );
  }
}



bool CSpreadsheetWorkBook::writeSheet( const QString& sheetName, const CTwoDArray<QVariant>& data, const bool treatEmptyStringsAsNull ) {
  return writeSheet( sheetName, data, 0, data.rowCount(), treatEmptyStringsAsNull );
}


bool CSpreadsheetWorkBook::writeSheet( const QString& sheetName, const CTwoDArray<QVariant>& data, const int startRow, const int nRows, const bool treatEmptyStringsAsNull ) {
  if( !_isWritable ) {
    _ok = false;
    _errMsg.append( QStringLiteral("Selected workbook cannot be written. Do you have appropriate permissions?\n") );
  }
  else {
    _ok = true; // Until shown otherwise
    _errMsg.clear();

    if( Format2007 != _fileFormat  ) {
      _ok = false;
      _errMsg.append( QStringLiteral("Sheets can written to Format2007 files.\n") );
      return _ok;
    }

    if( !_isOpen ) {
      Q_ASSERT( nullptr ==_xlsx );
      _xlsx = new QXlsx::Document( _srcPathName );
      _xlsIs1904 = false;
      _isOpen = true;
    }

    if( !_sheetNames.containsValue( sheetName ) ) {
      _ok = this->addSheet( sheetName );
      if(!_ok ) {
        _errMsg.append( QStringLiteral("Could not add sheet with name '%1'.\n" ).arg( sheetName ) );
      }
    }

    if( _ok ) {
      if( !_xlsx->selectSheet( sheetName ) ) {
        _ok = false;
        _errMsg.append( QStringLiteral("Could not select sheet '%1'.\n" ).arg( sheetName ) );
      }
      else {
        int rowOffset = 1;
        int colOffset = 1;

        if( data.hasRowNames() ) {
          ++colOffset;
        }

        // Write column names, if present
        //-------------------------------
        if( data.hasColNames() ) {
          _ok = true;

          for( int c = 0; c < data.nCols(); ++c ) {
            _ok = ( _ok && _xlsx->write( rowOffset, c + colOffset, data.colNames().at(c) ) );
          }
          ++rowOffset;

          if( !_ok ) {
            _errMsg.append( QStringLiteral("Could not write column names to sheet '%1'.\n" ).arg( sheetName ) );
          }
        }


        // Write row names, if present
        //----------------------------
        if( data.hasRowNames() ) {
          _ok = true;

          int r = 0;
          for( int dataRowIdx = startRow; dataRowIdx < (startRow + nRows); ++dataRowIdx ) {
            _ok = ( _ok && _xlsx->write( r + rowOffset, 1, data.rowNames().at(dataRowIdx) ) );
            ++r;
          }

          if( !_ok ) {
            _errMsg.append( QStringLiteral("Could not write row names to sheet '%1'.\n" ).arg( sheetName ) );
          }
        }


        // Write the data
        //---------------
        _ok = true;

        int row = 0;
        for( int dataRowIdx = startRow; dataRowIdx < (startRow + nRows); ++dataRowIdx ) {
          for( int col = 0; col < data.nCols(); ++col ) {
            QVariant tmp;

            if( !treatEmptyStringsAsNull || !isNullOrEmpty( data.at( col, dataRowIdx ) ) ) {
              tmp = data.at( col, dataRowIdx );
            }
            _ok = _xlsx->write( row + rowOffset, col + colOffset, tmp );
            if( !_ok ) {
              qDebug() << "Could not write data to cell" << row << rowOffset << col << colOffset << tmp;
              break;
            }
          }
          if( !_ok ) {
            break;
          }

          ++row;
        }
      }

      // Final message
      //--------------
      if( !_ok ) {
        _errMsg.append( QStringLiteral("Could not write data to sheet '%1'.\n" ).arg( sheetName ) );
      }
    }
  }

  return _ok;
}


bool CSpreadsheetWorkBook::selectSheet( const int sheetIdx ) {
  _ok = true; // Until shown otherwise
  _errMsg.clear();

  if( Format2007 != _fileFormat  ) {
    _ok = false;
    _errMsg.append( QStringLiteral("Sheets can written to Format2007 files.\n") );
  }
  else {
    QString name = sheetName( sheetIdx );
    if( name.isEmpty() ) {
      _ok = false;
      _errMsg.append( QStringLiteral("No sheet with index '%1' to select.\n" ).arg( sheetIdx ) );
    }
    else {
      _ok = this->selectSheet( name );
    }
  }
  return _ok;
}


bool CSpreadsheetWorkBook::selectSheet( const QString& name ) {
  _ok = true; // Until shown otherwise
  _errMsg.clear();

  if( Format2007 != _fileFormat  ) {
    _ok = false;
    _errMsg.append( QStringLiteral("Sheets can written to Format2007 files.\n") );
  }
  else if( !_sheetNames.containsValue( name ) ) {
    _ok = false;
    _errMsg.append( QStringLiteral("No sheet with name '%1' to select.\n" ).arg( name ) );
  }
  else if( !_xlsx->selectSheet( name ) ) {
    _ok = false;
    _errMsg.append( QStringLiteral("Could not select sheet with name '%1'.\n" ).arg( name ) );
  }

  return _ok;
}



bool CSpreadsheetWorkBook::save() {
  return saveAs( _srcPathName );
}


bool CSpreadsheetWorkBook::saveAs( const QString& filename ) {
  QFileInfo fi( filename );
  bool isWritable;

  if( fi.exists() )
    isWritable = fi.isWritable();
  else
    isWritable = QFileInfo( fi.path() ).isWritable();

  if( !isWritable ) {
    _ok = false;
    _errMsg.append( QStringLiteral("Cannot save workbook: is it not writable.\n") );
    emit fileSaveError();

    #ifndef QCONCURRENT_USED
      QCoreApplication::processEvents();
    #endif
  }
  else {
    emit fileSaveStart();

    #ifndef QCONCURRENT_USED
      QCoreApplication::processEvents();
    #endif

    _ok = true; // Until shown otherwise
    _errMsg.clear();

    if( Format2007 != _fileFormat  ) {
      _ok = false;
      _errMsg.append( QStringLiteral("Sheets can be written only to Format2007 files.\n") );
    }
    else {
      _ok = _xlsx->saveAs( filename );
      if( !_ok ) {
        _errMsg.append( QStringLiteral("File could not be written.\n") );
      }
      else {
        _isWritable = isWritable;
        _srcPathName = filename;
      }
    }

    if( _ok ) {
      _isReadable = true;
    }

    emit fileSaveComplete();

    #ifndef QCONCURRENT_USED
      QCoreApplication::processEvents();
    #endif
  }

  return _ok;
}
