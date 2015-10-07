#include "cdberrorhandler.h"

CDBErrorHandler::CDBErrorHandler( const bool writeErrorLog, const QString& errorLogFilename ) : CErrorHandler( writeErrorLog, errorLogFilename ) {
  // Nothing else to do here.
}


CDBErrorHandler::~CDBErrorHandler() {
  // Nothing else to do here.
}


void CDBErrorHandler::handleError( const CError::ErrorLevel type, const QSqlQuery* query, const QString& additionalInfo /* = "" */ ) {
  QString msg;
  if( query->isSelect() )
    msg = ">>> SELECT failure";
  else
    msg = ">>> INSERT/UPDATE failure";

  if( !additionalInfo.isEmpty() )
    msg.append( QString( " (%1)" ).arg( additionalInfo ) );

  handleError( type, msg, ErrorReturnValue::FAILED_DB_QUERY );
  handleError( type, query->lastQuery().simplified() );
  handleError( type, query->lastError().text(), ErrorReturnValue::FAILED_DB_QUERY );
  handleError( type, "<<< (End)", ErrorReturnValue::FAILED_DB_QUERY );
}


void CDBErrorHandler::handleError( const CError::ErrorLevel type, const QSqlDatabase* database, const QString& additionalInfo /* = "" */ ) {
  QString msg = ">>> DATABASE failure";

  if( !additionalInfo.isEmpty() )
    msg.append( QString( " (%1)" ).arg( additionalInfo ) );

  handleError( type, msg, ErrorReturnValue::BAD_DATABASE );
  handleError( type, database->lastError().text(), ErrorReturnValue::BAD_DATABASE );
  handleError( type, "<<< (End)", ErrorReturnValue::BAD_DATABASE );
}
//-----------------------------------------------------------------------------
