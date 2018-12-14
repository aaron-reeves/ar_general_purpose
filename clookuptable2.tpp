#include "clookuptable.h" // For convenience only: this would be a circular reference without guards.

template <class KeyType, class ValueType>
CLookupTable<KeyType, ValueType>::CLookupTable() {
  _db = NULL;
}

template <class KeyType, class ValueType>
CLookupTable<KeyType, ValueType>::CLookupTable(
  const QString& tableName,
  const QString& idField,
  const QString& valueField,
  QSqlDatabase* db,
  const bool fillFromDb,
  const bool canGrow
) {
  _tableName = tableName;
  _idField = idField;
  _valueField = valueField;

  _canGrow = canGrow;

  _db = db;

  _nextIndex = 1;

  if( fillFromDb ) {
    QString qStr =
      QString( "SELECT \"%1\", \"%2\" FROM \"%3\" ORDER BY \"%1\"" )
      .arg( _idField )
      .arg( _valueField )
      .arg( _tableName )
    ;
    QSqlQuery query( *_db );
    KeyType idx;

    if( !query.exec( qStr ) ) {
      _error = true;
      _errorMessage = QString( "Query failed: %1, %2" ).arg( query.lastQuery() ).arg( query.lastError().text());
    }
    else {
      while( query.next() ) {
        idx = query.value(_idField).value<KeyType>();
        _map.insert( query.value(_valueField).value<ValueType>(), idx );
        _addRow.insert( idx, false );
        
        if( QVariant::Int == query.value(_idField).type() ) {
          int idxInt = query.value(_idField).toInt();
          if( _nextIndex <= idxInt ) {
            _nextIndex = idxInt + 1;
          }
        }
      }

      _error = false;
    }
  }
}

template <class KeyType, class ValueType>
CLookupTable<KeyType, ValueType>::CLookupTable( const CLookupTable& other ) {
  assign( other );
}

template <class KeyType, class ValueType>
CLookupTable<KeyType, ValueType>& CLookupTable<KeyType, ValueType>::operator=( const CLookupTable& other ) {
  assign( other );
  return *this;
}

template <class KeyType, class ValueType>
void CLookupTable<KeyType, ValueType>::assign( const CLookupTable& other ) {
  _map = other._map;
  _addRow = other._addRow;

  _tableName = other._tableName;
  _idField = other._idField;
  _valueField = other._valueField;

  //_nextIndex = other._nextIndex;
  _canGrow = other._canGrow;

  _db = other._db;

  _error = other._error;
  _errorMessage = other._errorMessage;
}



// FIXME: This function could probably stand to be much more robust.
// At the moment, it does no error checking.
template <class KeyType, class ValueType>
bool CLookupTable<KeyType, ValueType>::merge( const CLookupTable& other ) {
  KeyType idx;
  ValueType val;

  for( int i = 0; i < other._map.count(); ++i ) {
    val = other._map.keyAtIndex(i);
    idx = other._map.valueAtIndex(i);
    _map.insert( val, idx );
    _addRow.insert( idx, false );

    // FIXME: Think about this.
    //if( QVariant::Int == query.value(_idField).type() ) {
    //  int idxInt = idx;
    //  if( _nextIndex <= idxInt ) {
    //    _nextIndex = idxInt + 1;
    //  }
    //}
  }

  return true;
}


template <class KeyType, class ValueType>
CLookupTable<KeyType, ValueType>::~CLookupTable() {
  // nothing to do here
}


template <class KeyType, class ValueType>
KeyType CLookupTable<KeyType, ValueType>::findValue( const ValueType value ) const {
  // This could be a little confusing.
  // Remember that map has keys that are strings (the lookup value itself).
  // Each key is associated with its ID (an integer).
  // the map is backward, then, from the way one might expect.
  // That's why it's a reverse lookup map!
  if( _map.containsKey( value ) )
    return _map.retrieveValue( value );
  else
    return 0;
}


// Does the key exist in the map?
template <class KeyType, class ValueType>
bool CLookupTable<KeyType, ValueType>::hasKey( const KeyType key ) const {
  // Remember: It's backwards!
  return( _map.containsValue( key ) );
}


// Does the value exist in the map?
template <class KeyType, class ValueType>
bool CLookupTable<KeyType, ValueType>::hasValue( const ValueType val ) const {
  // Remember: It's backwards!
  return (_map.containsKey( val ) );
}


template <class KeyType, class ValueType>
KeyType CLookupTable<KeyType, ValueType>::findOrAddValue( const ValueType value ) {
  Q_ASSERT( _canGrow );

  KeyType result = findValue( value );

  if( 0 == result ) {
    if( _canGrow ) {
      _map.insert( value, _nextIndex );
      _addRow.insert( _nextIndex, true );
      result = _nextIndex;
      ++_nextIndex;
    }
  }

  return result;
}


template <class KeyType, class ValueType>
bool CLookupTable<KeyType, ValueType>::populateDatabase( const bool& insertRecords ) {
  int i;
  QVariantList ids, vals;
  QSqlQuery query( *_db );

  // ID numbers start at 1, so remember to offset by 1 here.
  for( i = 1; i < _map.count() + 1; ++i ) {
    if( _addRow.value(i) ) {
      ids.append( i );
      vals.append( _map.retrieveKey(i) );
    }
  }

  if( indexField() == valueField() ) {
    if( !query.prepare(
        QString( "INSERT INTO \"%1\" ( \"%2\" ) SELECT ? WHERE NOT EXISTS ( SELECT \"%2\" FROM \"%1\" WHERE \"%2\" = ? )" )
        .arg( _tableName )
        .arg( _valueField )
      )
    ) {
      _error = true;
      _errorMessage = QString( "CLookupTable query preparation failed: %1, %2" ).arg( query.lastQuery() ).arg( query.lastError().text());
    }
    else {
      _error = false;
      query.addBindValue( vals );
      query.addBindValue( vals );
    }
  }
  else {
    if( !query.prepare(
        QString( "INSERT INTO \"%1\" ( \"%2\", \"%3\" ) SELECT ?, ? WHERE NOT EXISTS( SELECT \"%2\" FROM \"%1\" WHERE \"%2\" = ? )" )
        .arg( _tableName )
        .arg( _idField )
        .arg( _valueField )
      )
    ) {
      _error = true;
      _errorMessage = QString( "CLookupTable query preparation failed: %1, %2" ).arg( query.lastQuery() ).arg( query.lastError().text());
    }
    else {
      _error = false;
      query.addBindValue( ids );
      query.addBindValue( vals );
      query.addBindValue( ids );
    }
  }

  if( insertRecords && !_error ) {
    if( !query.execBatch() ) {
      _error = true;
      _errorMessage = QString( "CLookupTable query execution failed: %1, %2" ).arg( query.lastQuery() ).arg( query.lastError().text());
    }
    else {
      _error = false;
    }
  }

  return !_error;
}


template <class KeyType, class ValueType>
void CLookupTable<KeyType, ValueType>::debug() {
  qDebug() << endl;
  qDebug() << "tableName:" << _tableName;
  qDebug() << "indexField:" << _idField;
  qDebug() << "valueField:" << _valueField;
  qDebug() << "canGrow:" << _canGrow;

  for( int i = 0; i < _map.count(); ++i )
    qDebug() << "  " << _map.keyAtIndex(i) << _map.valueAtIndex(i);
}

