/*
cdberrorhandler.h/cpp
---------------------
Begin: 2015-10-07
Author: Aaron Reeves <aaron.reeves@sruc.ac.uk>
---------------------------------------------------
Copyright (C) 2015 Scotland's Rural College (SRUC)

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

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
