/*
clookuptable2.h/tpp
-------------------
Begin: 2018-12-14
Author: Aaron Reeves <aaron.reeves@sruc.ac.uk>
---------------------------------------------------
Copyright (C) 2018 Epidemiology Research Unit, Scotland's Rural College (SRUC)

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

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
 * Every lookup value is a short text string associated with (usually) an 
 * integer ID.  In some cases, lookup values come from a defined table.  In 
 * these cases, the database will be populated with information from that 
 * table, and all records that refer to a particular table must use the 
 * indices and values it defines.  No other values are allowed.
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

#include "clookuptable2.tpp"


#endif // CLOOKUPTABLE_H
