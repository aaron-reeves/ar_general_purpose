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

    QHash<QString, int> asHash() const;
    static QHash<QString, int> mergeHash( QHash<QString, int> results1, QHash<QString, int> results2 );

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
