/*
ctwodarray.h/tpp
------------------
Author: Aaron Reeves <aaron.reeves@sruc.ac.uk>
--------------------------------------------------
Copyright (C) 2018 - 2019 Scotland's Rural College (SRUC)

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#include "ctwodarray.h" // For convenience only: this would be a circular reference without guards.


//----------------------------------------------------------------------------------------------
// Construction/initialization/assignment/destruction
//----------------------------------------------------------------------------------------------
template <class T>
CTwoDArray<T>::CTwoDArray() {
  initialize();
}

template <class T>
CTwoDArray<T>::CTwoDArray( const int nCols, const int nRows ) {
  initialize();
  setSize( nCols, nRows );
}

template <class T>
CTwoDArray<T>::CTwoDArray( const int nCols, const int nRows, const T defaultVal ) {
  initialize();
  _useDefaultVal = true;
  _defaultVal = defaultVal;
  setSize( nCols, nRows );
}

template <class T>
CTwoDArray<T>::CTwoDArray( const CTwoDArray& other ) {
  assign( other );
}

template <class T>
CTwoDArray<T>& CTwoDArray<T>::operator= ( const CTwoDArray& other ) {
  assign( other );
  return* this;
}

template <class T>
CTwoDArray<T>::~CTwoDArray() {
  // Do nothing else
}

template <class T>
void CTwoDArray<T>::assign( const CTwoDArray& other ) {
  _nCols = other._nCols;
  _nRows = other._nRows;
  _data = other._data;

  _useDefaultVal = other._useDefaultVal;
  _defaultVal = other._defaultVal;

  _colNames = other._colNames;
  _rowNames = other._rowNames;
}

template <class T>
void CTwoDArray<T>::initialize() {
  _nCols = 0;
  _nRows = 0;

  _colNames.clear();
  _rowNames.clear();

  _useDefaultVal = false;
}
//----------------------------------------------------------------------------------------------



//----------------------------------------------------------------------------------------------
// Basic getters and setters
//----------------------------------------------------------------------------------------------
template <class T>
void CTwoDArray<T>::setValue( const int c, const int r, const T val ) {
  Q_ASSERT( ( c >= 0 ) && ( c < _nCols ) );
  Q_ASSERT( ( r >= 0 ) && ( r < _nRows ) );

  _data[r][c] = val;
}

template <class T>
const T& CTwoDArray<T>::value( const int c, const int r ) const {
  Q_ASSERT( ( c >= 0 ) && ( c < _nCols ) );
  Q_ASSERT( ( r >= 0 ) && ( r < _nRows ) );

  return _data.at(r).at(c);
}

template <class T>
T& CTwoDArray<T>::value( const int c, const int r ) {
  Q_ASSERT( ( c >= 0 ) && ( c < _nCols ) );
  Q_ASSERT( ( r >= 0 ) && ( r < _nRows ) );

  return _data[r][c];
}

template <class T>
QVector<T> CTwoDArray<T>::row( const int rowIdx ) const {
  Q_ASSERT( ( rowIdx >= 0 ) && ( rowIdx < _nRows ) );
  return _data.at( rowIdx );
}

template <class T>
QVector<T> CTwoDArray<T>::row( const QString& rowName ) const {
  Q_ASSERT( _rowNames.contains( rowName ) );
  return row( _rowNames.indexOf( rowName ) );
}

template <class T>
QVector<T> CTwoDArray<T>::column( const int colIdx ) const {
  Q_ASSERT( ( colIdx >= 0 ) && ( colIdx < _nCols ) );

  QVector<T> result( this->nRows() );
  for( int r = 0; r < this->nRows(); ++r ) {
    result[r] = this->value( colIdx, r );
  }

  return result;
}

template <class T>
QVector<T> CTwoDArray<T>::column( const QString& colName ) const {
  Q_ASSERT( _rowNames.contains( colName ) );
  return column( _colNames.indexOf( colName ) );
}
//----------------------------------------------------------------------------------------------



//----------------------------------------------------------------------------------------------
// Sizing
//----------------------------------------------------------------------------------------------
template <class T>
void CTwoDArray<T>::setSize( const int nCols, const int nRows ) {
  // FIXME: This currently assumes that the object is empty.
  Q_ASSERT( 0 == _nCols );
  Q_ASSERT( 0 == _nRows );

  _nCols = nCols;
  _nRows = nRows;

  // Add r rows, each with c elements
  for( int r = 0; r < nRows; ++r ) {
    QVector<T> row( _nCols );

    if( _useDefaultVal ) {
      row.fill( _defaultVal );
    }

    _data.append( row );
  }

  // Do this some day, if sizing becomes dynamic.
  //resizeNames();
}


template <class T>
void CTwoDArray<T>::setSize( const int nCols, const int nRows, const T defaultVal ) {
  _useDefaultVal = true;
  _defaultVal = defaultVal;
  this->setSize( nCols, nRows );
}


template <class T>
void CTwoDArray<T>::fill( const T val ) {
  for( int r = 0; r < _nRows; ++r ) {
    fillRow( r, val );
  }
}


template <class T>
void CTwoDArray<T>::fillRow( const int rowIdx, const T val ) {
  Q_ASSERT( (rowIdx >= 0) && (rowIdx < _nRows) );
  _data[rowIdx].fill( val );
}


template <class T>
void CTwoDArray<T>::appendRow() {
  QVector<T> row( _nCols );

  if( _useDefaultVal ) {
    row.fill( _defaultVal );
  }

  _data.append( row );
  ++_nRows;

  if( this->hasRowNames() ) {
    QString newRowName = QStringLiteral( "Row_%1" ).arg( _nRows );
    Q_ASSERT( !_rowNames.contains( newRowName ) );
    _rowNames.append( newRowName );
  }
}


template <class T>
void CTwoDArray<T>::appendRow( const QString& rowName ) {
  if( 0 < _nRows ) {
    Q_ASSERT( !_rowNames.isEmpty() );
  }

  Q_ASSERT( !_rowNames.contains( rowName ) );

  if( rowName.isEmpty() ) {
    appendRow();
  }
  else {
    QVector<T> row( _nCols );

    if( _useDefaultVal ) {
      row.fill( _defaultVal );
    }

    _data.append( row );
    ++_nRows;
    _rowNames.append( rowName );
  }
}


template <class T>
void CTwoDArray<T>::appendRow( const T defaultVal ) {
  _useDefaultVal = true;
  _defaultVal = defaultVal;
  appendRow();
}


template <class T>
void CTwoDArray<T>::appendRow(  const QString& rowName, const T defaultVal ) {
  _useDefaultVal = true;
  _defaultVal = defaultVal;
  appendRow( rowName );
}


template <class T>
void CTwoDArray<T>::appendRow(const QVector<T>& values ) {
  Q_ASSERT( values.count() == this->nCols() );

  _data.append( values );
  ++_nRows;

  if( this->hasRowNames() ) {
    QString newRowName = QStringLiteral( "Row_%1" ).arg( _nRows );
    Q_ASSERT( !_rowNames.contains( newRowName ) );
    _rowNames.append( newRowName );
  }
}


template <class T>
void CTwoDArray<T>::appendRow( const QList<T>& values ) {
  Q_ASSERT( values.count() == this->nCols() );

  _data.append( values.toVector() );
  ++_nRows;

  if( this->hasRowNames() ) {
    QString newRowName = QStringLiteral( "Row_%1" ).arg( _nRows );
    Q_ASSERT( !_rowNames.contains( newRowName ) );
    _rowNames.append( newRowName );
  }
}


template <class T>
void CTwoDArray<T>::appendRow( const QString& rowName, const QVector<T>& values ) {
  Q_ASSERT( values.count() == this->nCols() );

  if( 0 < _nRows ) {
    Q_ASSERT( !_rowNames.isEmpty() );
  }

  Q_ASSERT( !_rowNames.contains( rowName ) );

  if( rowName.isEmpty() ) {
    appendRow( values );
  }
  else {
    _data.append( values );
    ++_nRows;
    _rowNames.append( rowName );
  }
}


template <class T>
void CTwoDArray<T>::appendRow( const QString& rowName, const QList<T>& values ) {
  Q_ASSERT( values.count() == this->nCols() );

  if( 0 < _nRows ) {
    Q_ASSERT( !_rowNames.isEmpty() );
  }

  Q_ASSERT( !_rowNames.contains( rowName ) );

  if( rowName.isEmpty() ) {
    appendRow( values );
  }
  else {
    _data.append( values.toVector() );
    ++_nRows;
    _rowNames.append( rowName );
  }
}


template <class T>
void CTwoDArray<T>::append( const CTwoDArray<T> array ) {
  for( int r = 0; r < array.nRows(); ++r ) {
    this->appendRow( array.row( r ) );
  }
}


template <class T>
void CTwoDArray<T>::prependRow() {
  QVector<T> row( _nCols );

  if( _useDefaultVal ) {
    row.fill( _defaultVal );
  }

  _data.prepend( row );
  ++_nRows;

  if( this->hasRowNames() ) {
    QString newRowName = QStringLiteral( "Row_%1" ).arg( _nRows );
    Q_ASSERT( !_rowNames.contains( newRowName ) );
    _rowNames.prepend( newRowName );
  }
}


template <class T>
void CTwoDArray<T>::prependRow( const QString& rowName ) {
  if( 0 < _nRows ) {
    Q_ASSERT( !_rowNames.isEmpty() );
  }

  Q_ASSERT( !_rowNames.contains( rowName ) );

  if( rowName.isEmpty() ) {
    prependRow();
  }
  else {
    QVector<T> row( _nCols );

    if( _useDefaultVal ) {
      row.fill( _defaultVal );
    }

    _data.prepend( row );
    ++_nRows;
    _rowNames.prepend( rowName );
  }
}


template <class T>
void CTwoDArray<T>::prependRow( const T defaultVal ) {
  _useDefaultVal = true;
  _defaultVal = defaultVal;
  prependRow();
}


template <class T>
void CTwoDArray<T>::prependRow(  const QString& rowName, const T defaultVal ) {
  _useDefaultVal = true;
  _defaultVal = defaultVal;
  prependRow( rowName );
}


template <class T>
void CTwoDArray<T>::prependRow(const QVector<T>& values ) {
  Q_ASSERT( values.count() == this->nCols() );

  _data.prepend( values );
  ++_nRows;

  if( this->hasRowNames() ) {
    QString newRowName = QStringLiteral( "Row_%1" ).arg( _nRows );
    Q_ASSERT( !_rowNames.contains( newRowName ) );
    _rowNames.prepend( newRowName );
  }
}


template <class T>
void CTwoDArray<T>::prependRow( const QList<T>& values ) {
  Q_ASSERT( values.count() == this->nCols() );

  _data.prepend( values.toVector() );
  ++_nRows;

  if( this->hasRowNames() ) {
    QString newRowName = QStringLiteral( "Row_%1" ).arg( _nRows );
    Q_ASSERT( !_rowNames.contains( newRowName ) );
    _rowNames.prepend( newRowName );
  }
}


template <class T>
void CTwoDArray<T>::prependRow( const QString& rowName, const QVector<T>& values ) {
  Q_ASSERT( values.count() == this->nCols() );

  if( 0 < _nRows ) {
    Q_ASSERT( !_rowNames.isEmpty() );
  }

  Q_ASSERT( !_rowNames.contains( rowName ) );

  if( rowName.isEmpty() ) {
    prependRow( values );
  }
  else {
    _data.prepend( values );
    ++_nRows;
    _rowNames.prepend( rowName );
  }
}


template <class T>
void CTwoDArray<T>::prependRow( const QString& rowName, const QList<T>& values ) {
  Q_ASSERT( values.count() == this->nCols() );

  if( 0 < _nRows ) {
    Q_ASSERT( !_rowNames.isEmpty() );
  }

  Q_ASSERT( !_rowNames.contains( rowName ) );

  if( rowName.isEmpty() ) {
    prependRow( values );
  }
  else {
    _data.prepend( values.toVector() );
    ++_nRows;
    _rowNames.prepend( rowName );
  }
}


template <class T>
void CTwoDArray<T>::prepend( const CTwoDArray<T> array ) {
  for( int r = array.nRows() - 1; r >= 0 ; --r ) {
    this->prependRow( array.row( r ) );
  }
}


template <class T>
void CTwoDArray<T>::removeRow( const int rowIdx ) {
  Q_ASSERT( (rowIdx >= 0) && (rowIdx < _nRows) );
  _data.removeAt( rowIdx );
  if( this->hasRowNames() ) {
    _rowNames.removeAt( rowIdx );
  }
  --_nRows;
}


template <class T>
void CTwoDArray<T>::removeRow( const QString& rowName ) {
  removeRow( rowName, Qt::CaseSensitive );
}

template <class T>
void CTwoDArray<T>::removeRow( const QString& rowName, const Qt::CaseSensitivity sens ) {
  QVector<int> toRemove;

  for( int i = 0; i < _rowNames.count(); ++i ) {
    if( 0 == _rowNames.at(i).compare( rowName, sens ) )
      toRemove.append( i );
  }

  for( int i = toRemove.count() - 1; i > -1; --i ) {
    removeRow( toRemove.at(i) );
  }
}


template <class T>
void CTwoDArray<T>::removeColumn( const int colIdx ) {
  Q_ASSERT( (colIdx >= 0) && (colIdx < _nCols) );

  for( int r = 0; r < _nRows; ++r ) {
    _data[r].removeAt( colIdx );
  }

  if( this->hasColNames() ) {
    _colNames.removeAt( colIdx );
  }
  --_nCols;
}


template <class T>
void CTwoDArray<T>::removeColumn( const QString& colName ) {
  removeColumn( colName, Qt::CaseSensitive );
}


template <class T>
void CTwoDArray<T>::removeColumn( const QString& colName, const Qt::CaseSensitivity sens ) {
  QVector<int> toRemove;

  for( int i = 0; i < _colNames.count(); ++i ) {
    if( 0 == _colNames.at(i).compare( colName, sens ) )
      toRemove.append( i );
  }

  for( int i = toRemove.count() - 1; i > -1; --i ) {
    removeColumn( toRemove.at(i) );
  }
}
//----------------------------------------------------------------------------------------------



//----------------------------------------------------------------------------------------------
// Row/column names
//----------------------------------------------------------------------------------------------
template <class T>
void CTwoDArray<T>::setColNames( const QStringList& names ) {
  Q_ASSERT( names.count() == _nCols );

  for( int i = 0; i < names.count(); ++i ) {
    _colNames.append( names.at(i).trimmed() );
  }
}


template <class T>
void CTwoDArray<T>::setRowNames( const QStringList& names ) {
  Q_ASSERT( names.count() == _nRows );

  for( int i = 0; i < names.count(); ++i ) {
    _rowNames.append( names.at(i).trimmed() );
  }
}
//----------------------------------------------------------------------------------------------



//----------------------------------------------------------------------------------------------
// Getters and setters for use with row/column names
//----------------------------------------------------------------------------------------------
template <class T>
void CTwoDArray<T>::setValue( const QString& colName, const int r, const T val ) {
  Q_ASSERT_X( _colNames.contains( colName ), " CTwoDArray<T>::setValue", QStringLiteral( "Missing column name: %1" ).arg( colName ).toLatin1().data() );
  setValue( _colNames.indexOf( colName ), r, val );
}

template <class T>
void CTwoDArray<T>::setValue( const int c, const QString& rowName, const T val ) {
  Q_ASSERT_X( _rowNames.contains( rowName ), " CTwoDArray<T>::setValue", QStringLiteral( "Missing row name: %1" ).arg( rowName ).toLatin1().data() );
  setValue( c, _rowNames.indexOf( rowName ), val );
}

template <class T>
void CTwoDArray<T>::setValue( const QString& colName, const QString& rowName, const T val ) {
  Q_ASSERT_X( _colNames.contains( colName ), " CTwoDArray<T>::setValue", QStringLiteral( "Missing column name: %1" ).arg( colName ).toLatin1().data() );
  Q_ASSERT_X( _rowNames.contains( rowName ), " CTwoDArray<T>::setValue", QStringLiteral( "Missing row name: %1" ).arg( rowName ).toLatin1().data() );
  setValue( _colNames.indexOf( colName ), _rowNames.indexOf( rowName ), val );
}


template <class T>
T& CTwoDArray<T>::value( const QString& colName, const int r ) {
  Q_ASSERT_X( _colNames.contains( colName ), " CTwoDArray<T>::value", QStringLiteral( "Missing column name: %1" ).arg( colName ).toLatin1().data() );
  return this->value( _colNames.indexOf( colName ), r );
}

template <class T>
T& CTwoDArray<T>::value( const int c, const QString& rowName ) {
  Q_ASSERT_X( _rowNames.contains( rowName ), " CTwoDArray<T>::value", QStringLiteral( "Missing row name: %1" ).arg( rowName ).toLatin1().data() );
  return this->value( c, _rowNames.indexOf( rowName ) );
}

template <class T>
T& CTwoDArray<T>::value( const QString& colName, const QString& rowName ) {
  Q_ASSERT_X( _colNames.contains( colName ), " CTwoDArray<T>::value", QStringLiteral( "Missing column name: %1" ).arg( colName ).toLatin1().data() );
  Q_ASSERT_X( _rowNames.contains( rowName ), " CTwoDArray<T>::value", QStringLiteral( "Missing row name: %1" ).arg( rowName ).toLatin1().data() );
  return this->value( _colNames.indexOf( colName ), _rowNames.indexOf( rowName ) );
}


template <class T>
const T& CTwoDArray<T>::value( const QString& colName, const int r ) const {
  Q_ASSERT_X( _colNames.contains( colName ), " CTwoDArray<T>::value", QStringLiteral( "Missing column name: %1" ).arg( colName ).toLatin1().data() );
  return this->value( _colNames.indexOf( colName ), r );
}

template <class T>
const T& CTwoDArray<T>::value( const int c, const QString& rowName ) const {
  Q_ASSERT_X( _rowNames.contains( rowName ), " CTwoDArray<T>::value", QStringLiteral( "Missing row name: %1" ).arg( rowName ).toLatin1().data() );
  return this->value( c, _rowNames.indexOf( rowName ) );
}

template <class T>
const T& CTwoDArray<T>::value( const QString& colName, const QString& rowName ) const {
  Q_ASSERT_X( _colNames.contains( colName ), " CTwoDArray<T>::value", QStringLiteral( "Missing column name: %1" ).arg( colName ).toLatin1().data() );
  Q_ASSERT_X( _rowNames.contains( rowName ), " CTwoDArray<T>::value", QStringLiteral( "Missing row name: %1" ).arg( rowName ).toLatin1().data() );
  return this->value( _colNames.indexOf( colName ), _rowNames.indexOf( rowName ) );
}
//----------------------------------------------------------------------------------------------
