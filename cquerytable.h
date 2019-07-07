#ifndef CQUERYTABLE_H
#define CQUERYTABLE_H

#include <QtCore>
#include <QtSql>

class CQueryTable {
  public:
    CQueryTable();
    CQueryTable( QSqlDatabase* db, const QString& queryStr );
    CQueryTable( QSqlQuery* query );
    CQueryTable( const CQueryTable& other ); // Basic copy constructor
    CQueryTable& operator=( const CQueryTable& other ); // Basic assignment operator
    ~CQueryTable();

    bool exec( const QString& queryStr );

    void printTableFormat( QTextStream* stream );
    void printListFormat( QTextStream* stream );

  protected:
    QSqlDatabase* _db;
    QSqlQuery* _query;
    bool _ownsQuery;

    // Helper functions
    QString header( const QList<int>& arr );
    QString column( const QString& label, const int len );
    QString dbVariantToString( const QVariant& var );
    QString listLabel( QString label, const int desiredLen );
};

#endif // CQUERYTABLE_H
