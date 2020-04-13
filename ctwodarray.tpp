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

  _colNamesLookup = other._colNamesLookup;
  _rowNamesLookup = other._rowNamesLookup;
}

template <class T>
void CTwoDArray<T>::initialize() {
  _nCols = 0;
  _nRows = 0;

  _colNames.clear();
  _rowNames.clear();

  _colNamesLookup.clear();
  _rowNamesLookup.clear();

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
  Q_ASSERT( _rowNamesLookup.contains( rowName.toLower().trimmed() ) );
  return row( _rowNamesLookup.value( rowName.toLower().trimmed() ) );
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
  Q_ASSERT( _colNamesLookup.contains( colName.toLower().trimmed() ) );
  return column( _colNamesLookup.value( colName.toLower().trimmed() ) );
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
void CTwoDArray<T>::appendColumn() {
  for( int i = 0; i < this->nRows(); ++i ) {
    _data[i].resize( _nCols + 1 );
    if( _useDefaultVal ) {
      _data[i][_nCols] = _defaultVal;
    }
  }

  ++_nCols;

  if( this->hasColNames() ) {
    QString newColName = QStringLiteral( "Column_%1" ).arg( _nCols );
    Q_ASSERT( !_colNamesLookup.contains( newColName.toLower().trimmed() ) );
    _colNames.append( newColName );
    _colNamesLookup.insert( newColName.toLower().trimmed(), _nCols - 1 );
  }
}


template <class T>
void CTwoDArray<T>::appendColumn( const T defaultVal ) {
  for( int i = 0; i < this->nRows(); ++i ) {
    _data[i].resize( _nCols + 1 );
    _data[i][_nCols] = defaultVal;
  }

  ++_nCols;

  if( this->hasColNames() ) {
    QString newColName = QStringLiteral( "Column_%1" ).arg( _nCols );
    Q_ASSERT( !_colNamesLookup.contains( newColName.toLower().trimmed() ) );
    _colNames.append( newColName );
    _colNamesLookup.insert( newColName.toLower().trimmed(), _nCols - 1 );
  }
}


template <class T>
void CTwoDArray<T>::appendColumn( const QString& colName ) {
  for( int i = 0; i < this->nRows(); ++i ) {
    _data[i].resize( _nCols + 1 );
    if( _useDefaultVal ) {
      _data[i][_nCols] = _defaultVal;
    }
  }

  ++_nCols;

  Q_ASSERT( !_colNamesLookup.contains( colName.toLower().trimmed() ) );
  _colNames.append( colName );
  _colNamesLookup.insert( colName.toLower().trimmed(), _nCols - 1 );
}


template <class T>
void CTwoDArray<T>::appendColumn( const QString& colName, const T defaultVal ) {
  for( int i = 0; i < this->nRows(); ++i ) {
    _data[i].resize( _nCols + 1 );
    if( _useDefaultVal ) {
      _data[i][_nCols] = defaultVal;
    }
  }

  ++_nCols;

  Q_ASSERT( !_colNamesLookup.contains( colName.toLower().trimmed() ) );
  _colNames.append( colName );
  _colNamesLookup.insert( colName.toLower().trimmed(), _nCols - 1 );
}


template <class T>
void CTwoDArray<T>::appendColumn( const QVector<T>& values ) {
  Q_ASSERT( values.count() == this->nRows() );

  for( int i = 0; i < this->nRows(); ++i ) {
    _data[i].resize( _nCols + 1 );
    _data[i][_nCols] = values.at(i);
  }

  ++_nCols;

  if( this->hasColNames() ) {
    QString newColName = QStringLiteral( "Column_%1" ).arg( _nCols );
    Q_ASSERT( !_colNamesLookup.contains( newColName.toLower().trimmed() ) );
    _colNames.append( newColName );
    _colNamesLookup.insert( newColName.toLower().trimmed(), _nCols - 1 );
  }
}


template <class T>
void CTwoDArray<T>::appendColumn( const QString& colName, const QVector<T>& values ) {
  Q_ASSERT( values.count() == this->nRows() );

  for( int i = 0; i < this->nRows(); ++i ) {
    _data[i].resize( _nCols + 1 );
    _data[i][_nCols] = values.at(i);
  }

  ++_nCols;

  Q_ASSERT( !_colNamesLookup.contains( colName.toLower().trimmed() ) );
  _colNames.append( colName );
  _colNamesLookup.insert( colName.toLower().trimmed(), _nCols - 1 );
}


template <class T>
void CTwoDArray<T>::appendColumn( const QList<T>& values ) {
  Q_ASSERT( values.count() == this->nRows() );

  for( int i = 0; i < this->nRows(); ++i ) {
    _data[i].resize( _nCols + 1 );
    _data[i][_nCols] = values.at(i);
  }

  ++_nCols;

  if( this->hasColNames() ) {
    QString newColName = QStringLiteral( "Column_%1" ).arg( _nCols );
    Q_ASSERT( !_colNamesLookup.contains( newColName.toLower().trimmed() ) );
    _colNames.append( newColName );
    _colNamesLookup.insert( newColName.toLower().trimmed(), _nCols - 1 );
  }
}


template <class T>
void CTwoDArray<T>::appendColumn( const QString& colName, const QList<T>& values ) {
  Q_ASSERT( values.count() == this->nRows() );

  for( int i = 0; i < this->nRows(); ++i ) {
    _data[i].resize( _nCols + 1 );
    _data[i][_nCols] = values.at(i);
  }

  ++_nCols;

  Q_ASSERT( !_colNamesLookup.contains( colName.toLower().trimmed() ) );
  _colNames.append( colName );
  _colNamesLookup.insert( colName.toLower().trimmed(), _nCols - 1 );
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
    Q_ASSERT( !_rowNamesLookup.contains( newRowName.toLower().trimmed() ) );
    _rowNames.append( newRowName );
    _rowNamesLookup.insert( newRowName.toLower().trimmed(), _nRows - 1 );
  }
}


template <class T>
void CTwoDArray<T>::appendRow( const QString& rowName ) {
  if( 0 < _nRows ) {
    Q_ASSERT( !_rowNames.isEmpty() );
  }

  Q_ASSERT( !_rowNamesLookup.contains( rowName.toLower().trimmed() ) );

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
    _rowNamesLookup.insert( rowName.toLower().trimmed(), _nRows - 1 );
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
    Q_ASSERT( !_rowNamesLookup.contains( newRowName.toLower().trimmed() ) );
    _rowNames.append( newRowName );
    _rowNamesLookup.insert( newRowName.toLower().trimmed(), _nRows - 1 );
  }
}


template <class T>
void CTwoDArray<T>::appendRow( const QList<T>& values ) {
  Q_ASSERT( values.count() == this->nCols() );

  _data.append( values.toVector() );
  ++_nRows;

  if( this->hasRowNames() ) {
    QString newRowName = QStringLiteral( "Row_%1" ).arg( _nRows );
    Q_ASSERT( !_rowNamesLookup.contains( newRowName.toLower().trimmed() ) );
    _rowNames.append( newRowName );
    _rowNamesLookup.insert( newRowName.toLower().trimmed(), _nRows - 1 );
  }
}


template <class T>
void CTwoDArray<T>::appendRow( const QString& rowName, const QVector<T>& values ) {
  Q_ASSERT( values.count() == this->nCols() );

  if( 0 < _nRows ) {
    Q_ASSERT( !_rowNames.isEmpty() );
  }

  Q_ASSERT( !_rowNamesLookup.contains( rowName.toLower().trimmed() ) );

  if( rowName.isEmpty() ) {
    appendRow( values );
  }
  else {
    _data.append( values );
    ++_nRows;
    _rowNames.append( rowName );
    _rowNamesLookup.insert( rowName.toLower().trimmed(), _nRows - 1 );
  }
}


template <class T>
void CTwoDArray<T>::appendRow( const QString& rowName, const QList<T>& values ) {
  Q_ASSERT( values.count() == this->nCols() );

  if( 0 < _nRows ) {
    Q_ASSERT( !_rowNames.isEmpty() );
  }

  Q_ASSERT( !_rowNamesLookup.contains( rowName.toLower().trimmed() ) );

  if( rowName.isEmpty() ) {
    appendRow( values );
  }
  else {
    _data.append( values.toVector() );
    ++_nRows;
    _rowNames.append( rowName );
    _rowNamesLookup.insert( rowName.toLower().trimmed(), _nRows - 1 );
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
    Q_ASSERT( !_rowNamesLookup.contains( newRowName.toLower().trimmed() ) );
    _rowNames.prepend( newRowName );
    updateRowNames();
  }
}


template <class T>
void CTwoDArray<T>::prependRow( const QString& rowName ) {
  if( 0 < _nRows ) {
    Q_ASSERT( !_rowNames.isEmpty() );
  }

  Q_ASSERT( !_rowNamesLookup.contains( rowName.toLower().trimmed() ) );

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

    if( this->hasRowNames() ) {
      _rowNames.prepend( rowName );
      updateRowNames();
    }
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
    Q_ASSERT( !_rowNamesLookup.contains( newRowName.toLower().trimmed() ) );
    _rowNames.prepend( newRowName );
    updateRowNames();
  }
}


template <class T>
void CTwoDArray<T>::prependRow( const QList<T>& values ) {
  Q_ASSERT( values.count() == this->nCols() );

  _data.prepend( values.toVector() );
  ++_nRows;

  if( this->hasRowNames() ) {
    QString newRowName = QStringLiteral( "Row_%1" ).arg( _nRows );
    Q_ASSERT( !_rowNamesLookup.contains( newRowName.toLower().trimmed() ) );
    _rowNames.prepend( newRowName );
    updateRowNames();
  }
}


template <class T>
void CTwoDArray<T>::prependRow( const QString& rowName, const QVector<T>& values ) {
  Q_ASSERT( values.count() == this->nCols() );

  if( 0 < _nRows ) {
    Q_ASSERT( !_rowNames.isEmpty() );
  }

  Q_ASSERT( !_rowNamesLookup.contains( rowName.toLower().trimmed() ) );

  if( rowName.isEmpty() ) {
    prependRow( values );
  }
  else {
    _data.prepend( values );
    ++_nRows;

    if( this->hasRowNames() ) {
      _rowNames.prepend( rowName );
      updateRowNames();
    }
  }
}


template <class T>
void CTwoDArray<T>::prependRow( const QString& rowName, const QList<T>& values ) {
  Q_ASSERT( values.count() == this->nCols() );

  if( 0 < _nRows ) {
    Q_ASSERT( !_rowNames.isEmpty() );
  }

  Q_ASSERT( !_rowNamesLookup.contains( rowName.toLower().trimmed() ) );

  if( rowName.isEmpty() ) {
    prependRow( values );
  }
  else {
    _data.prepend( values.toVector() );
    ++_nRows;

    if( this->hasRowNames() ) {
      _rowNames.prepend( rowName );
      updateRowNames();
    }
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
    QString name = _rowNames.at( rowIdx );

    _rowNames.removeAt( rowIdx );
    updateRowNames();
  }
  --_nRows;
}


template <class T>
void CTwoDArray<T>::updateRowNames() {
  _rowNamesLookup.clear();
  for( int i = 0; i < _rowNames.count(); ++i ) {
    _rowNamesLookup.insert( _rowNames.at(i).toLower().trimmed(), i );
  }
}


template <class T>
void CTwoDArray<T>::removeRow( const QString& rowName ) {
  removeRow( _rowNamesLookup.value( rowName.toLower().trimmed() ) );
}


template <class T>
void CTwoDArray<T>::removeColumn( const int colIdx ) {
  Q_ASSERT( (colIdx >= 0) && (colIdx < _nCols) );

  for( int r = 0; r < _nRows; ++r ) {
    _data[r].removeAt( colIdx );
  }

  if( this->hasColNames() ) {
    QString colName = _colNames.at( colIdx );
    _colNames.removeAt( colIdx );
    updateColNames();
  }
  --_nCols;
}


template <class T>
void CTwoDArray<T>::updateColNames() {
  _colNamesLookup.clear();
  for( int i = 0; i < _colNames.count(); ++i ) {
    _colNamesLookup.insert( _colNames.at(i).toLower().trimmed(), i );
  }
}


template <class T>
void CTwoDArray<T>::removeColumn( const QString& colName ) {
  Q_ASSERT( _colNamesLookup.contains( colName.toLower().trimmed() ) );
  removeColumn( _colNamesLookup.value( colName.toLower().trimmed() ) );
}
//----------------------------------------------------------------------------------------------



//----------------------------------------------------------------------------------------------
// Row/column names
//----------------------------------------------------------------------------------------------
template <class T>
void CTwoDArray<T>::setColNames( const QStringList& names ) {
  if( ( 0 != _nCols ) || ( 0 != _nRows ) ) {
    Q_ASSERT( names.count() == _nCols );
  }
  else {
    this->setSize( names.count(), 0 );
  }

  for( int i = 0; i < names.count(); ++i ) {
    QString name = names.at(i).trimmed();

    int j = 2;
    while( _colNamesLookup.contains( name.toLower() ) ) {
      name = QStringLiteral("%1_%2").arg( names.at(i).trimmed() ).arg( j );
      ++j;
    }

    _colNames.append( name );
    _colNamesLookup.insert( name.toLower(), i );
  }
}


template <class T>
void CTwoDArray<T>::setRowNames( const QStringList& names ) {
  if( ( 0 != _nCols ) || ( 0 != _nRows ) ) {
    Q_ASSERT( names.count() == _nRows );
  }
  else {
    this->setSize( 0, names.count() );
  }

  for( int i = 0; i < names.count(); ++i ) {
    QString name = names.at(i).trimmed();

    int j = 2;
    while( _rowNamesLookup.contains( name.toLower() ) ) {
      name = QStringLiteral("%1_%2").arg( names.at(i).trimmed() ).arg( j );
      ++j;
    }

    _rowNames.append( name );
    _rowNamesLookup.insert( name.toLower(), i );
  }
}
//----------------------------------------------------------------------------------------------



//----------------------------------------------------------------------------------------------
// Getters and setters for use with row/column names
//----------------------------------------------------------------------------------------------
template <class T>
void CTwoDArray<T>::setValue( const QString& colName, const int r, const T val ) {
  Q_ASSERT_X( _colNamesLookup.contains( colName.toLower().trimmed() ), " CTwoDArray<T>::setValue", QStringLiteral( "Missing column name: %1" ).arg( colName ).toLatin1().data() );
  setValue( _colNamesLookup.value( colName.toLower().trimmed() ), r, val );
}

template <class T>
void CTwoDArray<T>::setValue( const int c, const QString& rowName, const T val ) {
  Q_ASSERT_X( _rowNamesLookup.contains( rowName.toLower().trimmed() ), " CTwoDArray<T>::setValue", QStringLiteral( "Missing row name: %1" ).arg( rowName ).toLatin1().data() );
  setValue( c, _rowNamesLookup.value( rowName.toLower().trimmed() ), val );
}

template <class T>
void CTwoDArray<T>::setValue( const QString& colName, const QString& rowName, const T val ) {
  Q_ASSERT_X( _colNamesLookup.contains( colName.toLower().trimmed() ), " CTwoDArray<T>::setValue", QStringLiteral( "Missing column name: %1" ).arg( colName ).toLatin1().data() );
  Q_ASSERT_X( _rowNamesLookup.contains( rowName.toLower().trimmed() ), " CTwoDArray<T>::setValue", QStringLiteral( "Missing row name: %1" ).arg( rowName ).toLatin1().data() );
  setValue( _colNamesLookup.value( colName.toLower().trimmed() ), _rowNamesLookup.value( rowName.toLower().trimmed() ), val );
}


template <class T>
T& CTwoDArray<T>::value( const QString& colName, const int r ) {
  Q_ASSERT_X( _colNamesLookup.contains( colName.toLower().trimmed() ), " CTwoDArray<T>::value", QStringLiteral( "Missing column name: %1" ).arg( colName ).toLatin1().data() );
  return this->value( _colNamesLookup.value( colName.toLower().trimmed() ), r );
}

template <class T>
T& CTwoDArray<T>::value( const int c, const QString& rowName ) {
  Q_ASSERT_X( _rowNamesLookup.contains( rowName.toLower().trimmed() ), " CTwoDArray<T>::value", QStringLiteral( "Missing row name: %1" ).arg( rowName ).toLatin1().data() );
  return this->value( c, _rowNamesLookup.value( rowName.toLower().trimmed() ) );
}

template <class T>
T& CTwoDArray<T>::value( const QString& colName, const QString& rowName ) {
  Q_ASSERT_X( _colNamesLookup.contains( colName.toLower().trimmed() ), " CTwoDArray<T>::value", QStringLiteral( "Missing column name: %1" ).arg( colName ).toLatin1().data() );
  Q_ASSERT_X( _rowNamesLookup.contains( rowName.toLower().trimmed() ), " CTwoDArray<T>::value", QStringLiteral( "Missing row name: %1" ).arg( rowName ).toLatin1().data() );
  return this->value( _colNamesLookup.value( colName.toLower().trimmed() ), _rowNamesLookup.value( rowName.toLower().trimmed() ) );
}


template <class T>
const T& CTwoDArray<T>::value( const QString& colName, const int r ) const {
  Q_ASSERT_X( _colNamesLookup.contains( colName.toLower().trimmed() ), " CTwoDArray<T>::value", QStringLiteral( "Missing column name: %1" ).arg( colName ).toLatin1().data() );
  return this->value( _colNamesLookup.value( colName.toLower().trimmed() ), r );
}

template <class T>
const T& CTwoDArray<T>::value( const int c, const QString& rowName ) const {
  Q_ASSERT_X( _rowNamesLookup.contains( rowName.toLower().trimmed() ), " CTwoDArray<T>::value", QStringLiteral( "Missing row name: %1" ).arg( rowName ).toLatin1().data() );
  return this->value( c, _rowNamesLookup.value( rowName.toLower().trimmed() ) );
}

template <class T>
const T& CTwoDArray<T>::value( const QString& colName, const QString& rowName ) const {
  Q_ASSERT_X( _colNamesLookup.contains( colName.toLower().trimmed() ), " CTwoDArray<T>::value", QStringLiteral( "Missing column name: %1" ).arg( colName ).toLatin1().data() );
  Q_ASSERT_X( _rowNamesLookup.contains( rowName.toLower().trimmed() ), " CTwoDArray<T>::value", QStringLiteral( "Missing row name: %1" ).arg( rowName ).toLatin1().data() );
  return this->value( _colNamesLookup.value( colName.toLower().trimmed() ), _rowNamesLookup.value( rowName.toLower().trimmed() ) );
}
//----------------------------------------------------------------------------------------------
