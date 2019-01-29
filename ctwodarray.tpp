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
    QString newRowName = QString( "Row_%1" ).arg( _nRows );
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
//----------------------------------------------------------------------------------------------



//----------------------------------------------------------------------------------------------
// Row/column names
//----------------------------------------------------------------------------------------------
template <class T>
void CTwoDArray<T>::setColNames( const QStringList& names ) {
  Q_ASSERT( names.count() == _nCols );
  _colNames = names;
}

template <class T>
void CTwoDArray<T>::setRowNames( const QStringList& names ) {
  Q_ASSERT( names.count() == _nRows );
  _rowNames = names;
}
//----------------------------------------------------------------------------------------------



//----------------------------------------------------------------------------------------------
// Getters and setters for use with row/column names
//----------------------------------------------------------------------------------------------
template <class T>
void CTwoDArray<T>::setValue( const QString& colName, const int r, const T val ) {
  Q_ASSERT( _colNames.contains( colName ) );
  setValue( _colNames.indexOf( colName ), r, val );
}

template <class T>
void CTwoDArray<T>::setValue( const int c, const QString& rowName, const T val ) {
  Q_ASSERT( _rowNames.contains( rowName ) );
  setValue( c, _rowNames.indexOf( rowName ), val );
}

template <class T>
void CTwoDArray<T>::setValue( const QString& colName, const QString& rowName, const T val ) {
  Q_ASSERT( _colNames.contains( colName ) );
  Q_ASSERT( _rowNames.contains( rowName ) );
  setValue( _colNames.indexOf( colName ), _rowNames.indexOf( rowName ), val );
}


template <class T>
T& CTwoDArray<T>::value( const QString& colName, const int r ) {
  Q_ASSERT( _colNames.contains( colName ) );
  return this->value( _colNames.indexOf( colName ), r );
}

template <class T>
T& CTwoDArray<T>::value( const int c, const QString& rowName ) {
  Q_ASSERT( _rowNames.contains( rowName ) );
  return this->value( c, _rowNames.indexOf( rowName ) );
}

template <class T>
T& CTwoDArray<T>::value( const QString& colName, const QString& rowName ) {
  Q_ASSERT( _colNames.contains( colName ) );
  Q_ASSERT( _rowNames.contains( rowName ) );
  return this->value( _colNames.indexOf( colName ), _rowNames.indexOf( rowName ) );
}


template <class T>
const T& CTwoDArray<T>::value( const QString& colName, const int r ) const {
  Q_ASSERT( _colNames.contains( colName ) );
  return this->value( _colNames.indexOf( colName ), r );
}

template <class T>
const T& CTwoDArray<T>::value( const int c, const QString& rowName ) const {
  Q_ASSERT( _rowNames.contains( rowName ) );
  return this->value( c, _rowNames.indexOf( rowName ) );
}

template <class T>
const T& CTwoDArray<T>::value( const QString& colName, const QString& rowName ) const {
  Q_ASSERT( _colNames.contains( colName ) );
  Q_ASSERT( _rowNames.contains( rowName ) );
  return this->value( _colNames.indexOf( colName ), _rowNames.indexOf( rowName ) );
}
//----------------------------------------------------------------------------------------------