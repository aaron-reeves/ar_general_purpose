#include "clookuptable.h"

//-----------------------------------------------------------------------------
// CLookupTable
//-----------------------------------------------------------------------------
CLookupTable::CLookupTable() {
  _db = NULL;
}


CLookupTable::CLookupTable(
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
    int idx;

    if( !query.exec( qStr ) ) {
      _error = true;
      _errorMessage = QString( "Query failed: %1, %2" ).arg( query.lastQuery() ).arg( query.lastError().text());
    }
    else {
      while( query.next() ) {
        idx = query.value(_idField).toInt();
        _map.insert( query.value(_valueField).toString(), idx );
        _addRow.insert( idx, false );
        if( _nextIndex <= idx ) {
          _nextIndex = idx + 1;
        }
      }

      _error = false;
    }
  }
}

CLookupTable::CLookupTable( const CLookupTable& other ) {
  assign( other );
}

CLookupTable& CLookupTable::operator=( const CLookupTable& other ) {
  assign( other );
  return *this;
}


void CLookupTable::assign( const CLookupTable& other ) {
  _map = other._map;
  _addRow = other._addRow;

  _tableName = other._tableName;
  _idField = other._idField;
  _valueField = other._valueField;

  _nextIndex = other._nextIndex;
  _canGrow = other._canGrow;

  _db = other._db;

  _error = other._error;
  _errorMessage = other._errorMessage;
}


CLookupTable::~CLookupTable() {
  // nothing to do here
}


int CLookupTable::findValue( const QString& value ) const {
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
bool CLookupTable::hasKey( const int& key ) const {
  // Remember: It's backwards!
  return( _map.containsValue( key ) );
}

// Does the value exist in the map?
bool CLookupTable::hasValue( const QString& val ) const {
  // Remember: It's backwards!
  return (_map.containsKey( val ) );
}


int CLookupTable::findOrAddValue( const QString& value ) {
  Q_ASSERT( _canGrow );

  int result = findValue( value );

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


bool CLookupTable::populateDatabase( const bool& insertRecords ) {
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


void CLookupTable::debug() {
  qDebug() << endl;
  qDebug() << "tableName:" << _tableName;
  qDebug() << "indexField:" << _idField;
  qDebug() << "valueField:" << _valueField;
  qDebug() << "canGrow:" << _canGrow;

  for( int i = 0; i < _map.count(); ++i )
    qDebug() << "  " << _map.keyAtIndex(i) << _map.valueAtIndex(i);
}
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// CStringIndexedLookupTable
//-----------------------------------------------------------------------------
CStringIndexedLookupTable::CStringIndexedLookupTable() {
  _db = NULL;
}

CStringIndexedLookupTable::CStringIndexedLookupTable(
  const QString& tableName,
  const QString& idField,
  const QString& valueField,
  QSqlDatabase* db,
  const bool fillFromDb,
  const bool canGrow
) {
  QString idx;
  _tableName = tableName;
  _idField = idField;
  _valueField = valueField;

  _canGrow = canGrow;

  _db = db;

  if( fillFromDb ) {
    QString qStr =
      QString( "SELECT \"%1\", \"%2\" FROM \"%3\" ORDER BY \"%1\"" )
      .arg( _idField )
      .arg( _valueField )
      .arg( _tableName )
    ;
    QSqlQuery query( *_db );

    if( !query.exec( qStr ) ) {
      _error = true;
      _errorMessage = QString( "Query failed: %1, %2" ).arg( query.lastQuery() ).arg( query.lastError().text());
    }
    else {
      while( query.next() ) {
        idx = query.value(_idField).toString();
        _map.insert( query.value(_valueField).toString(), idx );
        _addRow.insert( idx, false );
      }

      _error = false;
    }
  }
}


CStringIndexedLookupTable::CStringIndexedLookupTable( const CStringIndexedLookupTable& other ) {
  assign( other );
}


CStringIndexedLookupTable& CStringIndexedLookupTable::operator=( const CStringIndexedLookupTable& other ) {
  assign( other );
  return *this;
}


void CStringIndexedLookupTable::assign( const CStringIndexedLookupTable& other ) {
  _map = other._map;
  _addRow = other._addRow;

  _tableName = other._tableName;
  _idField = other._idField;
  _valueField = other._valueField;

  _canGrow = other._canGrow;

  _db = other._db;

  _error = other._error;
  _errorMessage = other._errorMessage;
}


CStringIndexedLookupTable::~CStringIndexedLookupTable() {
  // Nothing to do here
}


QString CStringIndexedLookupTable::findValue( const QString& value ) {
  // This could be a little confusing.
  // Remember that map has keys that are strings (the lookup value itself).
  // Each key is associated with its ID (another string).
  // the map is backward, then, from the way one might expect.
  // That's why it's a reverse lookup map!
  if( _map.containsKey( value ) )
    return _map.retrieveValue( value );
  else
    return QString();
}



bool CStringIndexedLookupTable::hasKey( const QString& key ) const {
  // Remember: it's backward.
  return _map.containsValue( key );
}


bool CStringIndexedLookupTable::hasValue( const QString& val ) {
  // Remember: it's backward.
  return _map.containsKey( val );
}


QString CStringIndexedLookupTable::findOrAddValue( const QString& value, const QString& index ) {
  Q_ASSERT( _canGrow );

  QString result = findValue( value );

  if( result.isEmpty() ) {
    if( _canGrow ) {
      _map.insert( value, index );
      _addRow.insert( index, true );
      result = index;
    }
  }

  return result;
}


bool CStringIndexedLookupTable::populateDatabase( const bool& insertRecords ) {
  int i;
  QString id;
  QVariantList ids, vals;
  QSqlQuery query( *_db );

  for( i = 0; i < _map.count(); ++i ) {
    id = _map.valueAtIndex(i);
    if( _addRow.value(id) ) {
      ids.append( id );
      vals.append( _map.retrieveKey(id) );
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
      _errorMessage = QString( "CStringIndexedLookupTable query preparation failed: %1, %2" ).arg( query.lastQuery() ).arg( query.lastError().text());
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
      _errorMessage = QString( "CStringIndexedLookupTable query preparation failed: %1, %2" ).arg( query.lastQuery() ).arg( query.lastError().text());
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
      _errorMessage = QString( "CStringIndexedLookupTable query execution failed: %1, %2" ).arg( query.lastQuery() ).arg( query.lastError().text());
    }
    else {
      _error = false;
    }
  }

  return !_error;
}


void CStringIndexedLookupTable::debug() {
  qDebug() << endl;
  qDebug() << "tableName:" << _tableName;
  qDebug() << "indexField:" << _idField;
  qDebug() << "valueField:" << _valueField;
  qDebug() << "canGrow:" << _canGrow;

  for( int i = 0; i < _map.count(); ++i )
    qDebug() << "  " << _map.keyAtIndex(i) << _map.valueAtIndex(i);
}
//-----------------------------------------------------------------------------
