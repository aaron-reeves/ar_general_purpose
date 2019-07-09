#ifndef CLOOKUPTABLE_H
#define CLOOKUPTABLE_H

#include <QtCore>
#include <QtSql>

#include <ar_general_purpose/creverselookupmap.h>

/* clookuptable.h/cpp
 * The class defined here is used for populating lookup tables in databases,
 * and for retrieving information from databases into the in-memory
 * lookup structure.
 *
 * Every lookup value is a short text string associated with an integer
 * ID.  In some cases, lookup values come from a defined table.  In these
 * cases, the database will be populated with information from that table,
 * and all records that refer to a particular table must use the indices
 * and values it defines.  No other values are allowed.
 *
 * In other cases, a lookup table may not exist, but will be defined
 * on the fly from values implied by some other data table.  In these
 * situations, the number of values included in a lookup table may grow.
 *
 * The value for a row in a lookup table may be retrieved by its index, or
 * vice versa.
*/

template <class KeyType, class ValueType>
class CLookupTable {
  public:
    CLookupTable();
    CLookupTable(
      const QString& tableName,
      const QString& idField,
      const QString& valueField,
      QSqlDatabase* db,
      const bool fillFromDb,
      const bool canGrow
    );
    CLookupTable( const CLookupTable& other );
    CLookupTable& operator=( const CLookupTable& other );
    ~CLookupTable();

    // Returns the index (ID number) of an existing value, or 0 if not found
    KeyType findValue( const ValueType value ) const;

    // Returns the index (ID number) of an existing value, or if not found,
    // adds a new value and returns its index.
    KeyType findOrAddValue( const ValueType value );

    // Does the key exist in the map?
    bool hasKey( const KeyType key ) const;

    // Does the value exist in the map?
    bool hasValue( const ValueType val ) const;

    // Adds any new values with their associated IDs to the database
    bool populateDatabase(const bool insertRecords );

    bool merge( const CLookupTable& other );

    // Properties
    QString tableName() { return _tableName; }
    QString indexField() { return _idField; }
    QString valueField() { return _valueField; }
    bool canGrow() { return _canGrow; }

    /*
    void setTableName( QString val ) { _tableName = val; }
    void setIndexField( QString val ) { _indexField = val; }
    void setValueField( QString val ) { _valueField = val; }
    void setCanGrow( bool val ) { _canGrow = val; }
    */

    void debug();

    bool error() const { return _error; }
    QString errorMessage() const { return _errorMessage; }

  protected:
    void assign( const CLookupTable& other );

    CReverseLookupMap<ValueType, KeyType> _map;
    QMap<KeyType, bool> _addRow;

    QString _tableName;
    QString _idField;
    QString _valueField;

    int _nextIndex;
    bool _canGrow;

    QSqlDatabase* _db;

    bool _error;
    QString _errorMessage;
};

//#include "clookuptable2.tpp"

template <class KeyType, class ValueType>
CLookupTable<KeyType, ValueType>::CLookupTable() {
  _db = nullptr;
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
    QString qStr = QStringLiteral( "SELECT \"%1\", \"%2\" FROM \"%3\" ORDER BY \"%1\"" ).arg( _idField,_valueField, _tableName );
    QSqlQuery query( *_db );
    KeyType idx;

    if( !query.exec( qStr ) ) {
      _error = true;
      _errorMessage = QStringLiteral( "Query failed: %1, %2" ).arg( query.lastQuery(), query.lastError().text());
    }
    else {
      while( query.next() ) {
        idx = query.value(_idField).template value<KeyType>();
        _map.insert( query.value(_valueField).template value<ValueType>(), idx );
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
bool CLookupTable<KeyType, ValueType>::populateDatabase( const bool insertRecords ) {
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
        QStringLiteral( "INSERT INTO \"%1\" ( \"%2\" ) SELECT ? WHERE NOT EXISTS ( SELECT \"%2\" FROM \"%1\" WHERE \"%2\" = ? )" ).arg( _tableName, _valueField ) )
    ) {
      _error = true;
      _errorMessage = QStringLiteral( "CLookupTable query preparation failed: %1, %2" ).arg( query.lastQuery(), query.lastError().text());
    }
    else {
      _error = false;
      query.addBindValue( vals );
      query.addBindValue( vals );
    }
  }
  else {
    if( !query.prepare(
        QStringLiteral( "INSERT INTO \"%1\" ( \"%2\", \"%3\" ) SELECT ?, ? WHERE NOT EXISTS( SELECT \"%2\" FROM \"%1\" WHERE \"%2\" = ? )" ).arg( _tableName, _idField, _valueField )
      )
    ) {
      _error = true;
      _errorMessage = QStringLiteral( "CLookupTable query preparation failed: %1, %2" ).arg( query.lastQuery() ).arg( query.lastError().text());
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
      _errorMessage = QStringLiteral( "CLookupTable query execution failed: %1, %2" ).arg( query.lastQuery(), query.lastError().text());
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
  qDebug() << "nItems:" << _map.count();
  qDebug() << "error:" << _error;
  qDebug() << "errorMsg:" << _errorMessage;

  for( int i = 0; i < _map.count(); ++i )
    qDebug() << "  " << _map.keyAtIndex(i) << _map.valueAtIndex(i);
}




#endif // CLOOKUPTABLE_H
