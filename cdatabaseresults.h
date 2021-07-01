/*
cdatabaseresults.h/tpp
-----------------------
Begin: 2021-06-30
Author: Aaron Reeves <aaron.reeves@sruc.ac.uk>
---------------------------------------------------------
Copyright (C) 2021 Scotland's Rural College (SRUC)

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#ifndef CDATABASERESULTS_H
#define CDATABASERESULTS_H

#include <QtCore>

#include <ar_general_purpose/returncodes.h>

class CDatabaseResults {
  public:
    CDatabaseResults() { initialize(); }
    CDatabaseResults( const QHash<QString, int>& hash );
    CDatabaseResults( const CDatabaseResults& other ) { assign( other ); }
    CDatabaseResults& operator=( const CDatabaseResults& other ) { assign( other ); return *this; }
    ~CDatabaseResults() { /* Nothing to do here */ }

    static QHash<QString, int> resultsTemplate();
    QHash<QString, int> asHash() const;

    int returnCode() const { return _returnCode; }

    int nTotalRecords() const { return _nTotalRecords; }
    int nProcessedRecords() const { return _nProcessedRecords; }
    int nSuccesses() const { return _nSuccesses; }
    int nFailures() const { return _nFailures; }

    void setNTotalRecords( const int val ) { _nTotalRecords = val; }

    void addRecord() { ++_nTotalRecords; }
    void addFailure() { ++_nProcessedRecords; ++_nFailures; _returnCode = ( _returnCode | ReturnCode::FAILED_DB_QUERY ); }
    void addSuccess() { ++_nProcessedRecords; ++_nSuccesses; }

    void setReturnCode( const int val ) { _returnCode = ( _returnCode | val ); }


    void initialize();

    void debug() const;

  protected:
    void assign( const CDatabaseResults& other );

    int _returnCode;
    int _nTotalRecords;
    int _nProcessedRecords;
    int _nSuccesses;
    int _nFailures;
};

#endif // CDATABASERESULTS_H
