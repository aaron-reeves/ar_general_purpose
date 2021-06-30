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


QHash<QString, int> CDatabaseResults::asHash() const {
  QHash<QString, int> result;

  result.insert( QStringLiteral("returnCode"), this->returnCode() );
  result.insert( QStringLiteral("totalRecords"), this->_nTotalRecords );
  result.insert( QStringLiteral("totalProcessed"), this->_nProcessedRecords);
  result.insert( QStringLiteral("successes"), this->_nSuccesses );
  result.insert( QStringLiteral("failures"), this->_nFailures );

  return result;
}


CDatabaseResults::CDatabaseResults( const QHash<QString, int>& hash ) {
  _returnCode = hash.value( "returnCode" );
  _nTotalRecords = hash.value( "totalRecords" );
  _nProcessedRecords =  hash.value( "totalProcessed" );
  _nSuccesses =  hash.value( "successes" );
  _nFailures =  hash.value( "failures" );
}


QHash<QString, int> CDatabaseResults::mergeHash( QHash<QString, int> results1, QHash<QString, int> results2 ) {
  QHash<QString, int> results;

  if( results1.isEmpty() ) {
    results1 = CDatabaseResults().asHash();
  }
  else {
    Q_ASSERT( results1.contains( "returnCode" ) );
    Q_ASSERT( results1.contains( "totalRecords" ) );
    Q_ASSERT( results1.contains( "totalProcessed" ) );
    Q_ASSERT( results1.contains( "successes" ) );
    Q_ASSERT( results1.contains( "failures" ) );
  }

  if( results2.isEmpty() ) {
    results2 = CDatabaseResults().asHash();
  }
  else {
    Q_ASSERT( results2.contains( "returnCode" ) );
    Q_ASSERT( results2.contains( "totalRecords" ) );
    Q_ASSERT( results2.contains( "totalProcessed" ) );
    Q_ASSERT( results2.contains( "successes" ) );
    Q_ASSERT( results2.contains( "failures" ) );
  }

  QList<QString> keys = results1.keys();

  foreach( QString key, keys ) {
    if( "returnCode" == key ) {
      results.insert( key, ( results1.value( key ) | results2.value( key ) ) );
    }
    else {
      results.insert( key, ( results1.value( key ) + results2.value( key ) ) );
    }
  }

  return results;
}
