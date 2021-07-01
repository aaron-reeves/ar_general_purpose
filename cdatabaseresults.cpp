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

#include <ar_general_purpose/cdatabaseresults.h>

#include <ar_general_purpose/qcout.h>

void CDatabaseResults::initialize() {
  _returnCode = ReturnCode::SUCCESS;
  _nTotalRecords = 0;
  _nProcessedRecords = 0;
  _nSuccesses = 0;
  _nFailures = 0;
}


void CDatabaseResults::assign( const CDatabaseResults& other ) {
  _returnCode = other._returnCode;
  _nTotalRecords =  other._nTotalRecords;
  _nProcessedRecords =  other._nProcessedRecords;
  _nSuccesses =  other._nSuccesses;
  _nFailures =  other._nFailures;
}


void CDatabaseResults::debug() const {
  qDb() << "returnCode:"        << ReturnCode::codeDescr( _returnCode );
  qDb() << "nTotalRecords:"     << _nTotalRecords;
  qDb() << "nProcessedRecords:" << _nProcessedRecords;
  qDb() << "nSuccesses:"        << _nSuccesses;
  qDb() << "nFailures:"         << _nFailures;
}


CDatabaseResults::CDatabaseResults( const QHash<QString, int>& hash ) {
  _returnCode = hash.value( "returnCode" );
  _nTotalRecords = hash.value( "totalRecords" );
  _nProcessedRecords =  hash.value( "totalProcessed" );
  _nSuccesses =  hash.value( "successes" );
  _nFailures =  hash.value( "failures" );
}


QHash<QString, int> CDatabaseResults::asHash() const {
  QHash<QString, int> result;

  result.insert( QStringLiteral("returnCode"), this->returnCode() );
  result.insert( QStringLiteral("totalRecords"), this->_nTotalRecords );
  result.insert( QStringLiteral("totalProcessed"), this->_nProcessedRecords);
  result.insert( QStringLiteral("successes"), this->_nSuccesses );
  result.insert( QStringLiteral("failures"), this->_nFailures );

  return result;
}


QHash<QString, int> CDatabaseResults::resultsTemplate() {
  QHash<QString, int> result;

  result.insert( QStringLiteral("returnCode"), ReturnCode::SUCCESS );
  result.insert( QStringLiteral("totalRecords"), 0 );
  result.insert( QStringLiteral("totalProcessed"), 0 );
  result.insert( QStringLiteral("successes"), 0 );
  result.insert( QStringLiteral("failures"), 0 );

  return result;
}


