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

//-----------------------------------------------------------------------------
// CLookupTable
//-----------------------------------------------------------------------------
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
    int findValue( const QString& value ) const;

    // Returns the index (ID number) of an existing value, or if not found,
    // adds a new value and returns its index.
    int findOrAddValue( const QString& value );

    // Does the key exist in the map?
    bool hasKey(const int key ) const;

    // Does the value exist in the map?
    bool hasValue( const QString& val ) const;

    // Adds any new values with their associated IDs to the database
    bool populateDatabase(const bool insertRecords );

    // Properties
    QString tableName( void ) { return _tableName; }
    QString indexField( void ) { return _idField; }
    QString valueField( void ) { return _valueField; }
    bool canGrow( void ) { return _canGrow; }

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

    CReverseLookupMap<QString, int> _map;
    QMap<int, bool> _addRow;

    QString _tableName;
    QString _idField;
    QString _valueField;

    int _nextIndex;
    bool _canGrow;

    QSqlDatabase* _db;

    bool _error;
    QString _errorMessage;
};
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// CStringIndexedLookupTable
//-----------------------------------------------------------------------------
class CStringIndexedLookupTable {
  public:
    CStringIndexedLookupTable();
    CStringIndexedLookupTable(
      const QString& tableName,
      const QString& idField,
      const QString& valueField,
      QSqlDatabase* db,
      const bool fillFromDb,
      const bool canGrow
    );
    CStringIndexedLookupTable( const CStringIndexedLookupTable& other );
    CStringIndexedLookupTable& operator=( const CStringIndexedLookupTable& other );
    ~CStringIndexedLookupTable();

    // Returns the index of an existing value, or an empty string if not found
    QString findValue( const QString& value );

    // Does the key exist in the map?
    bool hasKey( const QString& key ) const;

    // Does the value exist in the map?
    bool hasValue( const QString& val );

    // Returns the index of an existing value, or if not found,
    // adds a new value with the designated index.
    QString findOrAddValue( const QString& value, const QString& index );

    // Adds any new values with their associated IDs to the database
    bool populateDatabase( const bool insertRecords );

    // Properties
    QString tableName( void ) { return _tableName; }
    QString indexField( void ) { return _idField; }
    QString valueField( void ) { return _valueField; }
    bool canGrow( void ) { return _canGrow; }

    bool error() const { return _error; }
    QString errorMessage() const { return _errorMessage; }

    /*
    void setTableName( QString val ) { _tableName = val; }
    void setIndexField( QString val ) { _indexField = val; }
    void setValueField( QString val ) { _valueField = val; }
    void setCanGrow( bool val ) { _canGrow = val; }
    */

    void debug();

  protected:
    void assign( const CStringIndexedLookupTable& other );

    CReverseLookupMap<QString, QString> _map;
    QMap<QString, bool> _addRow;

    QString _tableName;
    QString _idField;
    QString _valueField;

    bool _canGrow;

    QSqlDatabase* _db;

    bool _error;
    QString _errorMessage;
};
//-----------------------------------------------------------------------------




#endif // CLOOKUPTABLE_H
