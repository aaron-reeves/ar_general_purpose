#ifndef CDBERRORHANDLER_H
#define CDBERRORHANDLER_H

#include <QtSql>

#include "cerror.h"

class CDBErrorHandler : public CErrorHandler {
  public:
    CDBErrorHandler( const bool writeErrorLog, const QString& errorLogFilename );
    ~CDBErrorHandler();

    // Funny business happens when overloading a function in a derived class:
    // http://stackoverflow.com/questions/3202234/overloaded-functions-are-hidden-in-derived-class
    // The "using" statement avoids this.
    using CErrorHandler::handleError;
    void handleError( const CError::ErrorLevel type, const QSqlQuery* query, const QString& additionalInfo = "" );
    void handleError( const CError::ErrorLevel type, const QSqlDatabase* database, const QString& additionalInfo = "" );
};

#endif // CDBERRORHANDLER_H
