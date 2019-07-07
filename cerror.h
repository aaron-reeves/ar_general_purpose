/*
cerror.h/cpp
------------
Begin: 2016/09/17
Authors: Aaron Reeves <aaron.reeves@sruc.ac.uk>
         Julie Stirling <julie.stirling@sruc.ac.uk>
---------------------------------------------------
Copyright (C) 2016 - 2017 Epidemiology Research Unit, Scotland's Rural College (SRUC)

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#ifndef CERROR_H
#define CERROR_H

#include <QtCore>
//#include <QtSql>

#include <ar_general_purpose/returncodes.h>

class CError {
  public:
    enum ErrorType {
      Unspecified,
      Ok,
      Information,
      Question,
      Warning,
      Critical,
      Fatal
    };

    CError();
    CError( const ErrorType type, const QString& msg, const int dataSourceID = -1, const int lineNumber = -1 );
    CError( const CError& other );
    CError& operator=( const CError& other );

    ErrorType type() const { return _type; }
    QString msg() const { return _msg.trimmed(); }
    QString logMessage() const;
    int lineNumber() const { return _lineNumber; }
    int dataSourceID() const { return _dataSourceID; }

    QString typeAsString() const;
    static QString typeAsString( const ErrorType type );

    void debug() const;

  protected:
    ErrorType _type;
    QString _msg;
    int _lineNumber;
    int _dataSourceID;
};

Q_DECLARE_TYPEINFO( CError::ErrorType, Q_PRIMITIVE_TYPE );
Q_DECLARE_TYPEINFO( CError, Q_MOVABLE_TYPE );


class CErrorList {
  public:
    CErrorList();
    CErrorList( const CErrorList& other );
    ~CErrorList();

    enum ErrorFileFormat {
      ErrorFileCSV,
      ErrorFileLog
    };

    bool hasErrors() const;
    bool hasWarnings() const;
    
    QString logMessage() const;
    
    CErrorList( const bool useAppLog );
    void clear();
    int count() const;
    QString messageAt( const int i ) const;
    CError at( const int i ) const;
    void append( const CError& err );
    //void append( CError::ErrorType level, const QString& msg );
    void append( const CErrorList& src );
    QString asText() const;

    bool writeFile( const QString& filename, const ErrorFileFormat fmt );
    
  protected:
    bool _useAppLog;
    QVector<CError> _list;
};

Q_DECLARE_TYPEINFO( QVector<CError>, Q_MOVABLE_TYPE );
Q_DECLARE_TYPEINFO( CErrorList, Q_MOVABLE_TYPE );


#endif // CERROR_H
