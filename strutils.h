/*
strutils.h/cpp
--------------
Author: Aaron Reeves <aaron.reeves@naadsm.org>
--------------------------------------------------
Copyright (C) 2007 - 2014 Aaron Reeves

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#ifndef STRUTILS_H
#define STRUTILS_H

#ifdef WINDOWS_OR_WHATEVER_IT_IS
#include <tchar.h>
#endif

#include <QtCore>

QString quoteString( const QString& str, const QChar quoteMark = '"' );
QString camelCase( const QString& str );
QString postgresCase( const QString& str );

QString findAndRemove( QString str3, QString str1 );
QString findAndReplace( QString str3, QString str2, QString str1 );

QString removeLineBreaks( QString str1 );

QString removeDelimiters( const QString& val, QChar delim );

QString boolToStr( bool val ); // returns "-1" or "0"
QString boolToText( bool val ); // returns "true" or "false"
QString boolToYesNo( bool val ); // returns "Yes" or "No"
bool strToBool( QString val, bool* ok = NULL );

QString paddedInt( int toPad, const int places, const QChar padChar = '0' );
QString leftPaddedStr( QString toPad, const int places, const QChar padChar = ' ' );

QString toTitleCase( QString str );

QString splitNear( int pos, QString & str, int maxLenAdd = 0, bool usePunct = true, bool forceBreak = true );
QStringList prettyPrintedList( const QString srcStr, int prefLineLen = 50, bool usePunct = true, bool forceBreak = true, int indent = 0 );
QString prettyPrint( const QString srcStr, int prefLineLen = 50, bool usePunct = true, bool forceBreak = true, int indent = 0 );

bool isComment( const QString st );

/* This function attempts to rebuild a properly formatted comma-separated list from one that
 * was poorly formatted.  The function needs to know how many parts were expected and must have
 * a list of regular expressions for matching each part.
 *
 * If a new list can be constructed that matches all of the regular expressions, then success is set to true.
 */
bool reprocessCsv( QString fullLine, QList<QRegExp> patternsToMatch, QStringList& newList, const int nExpectedParts );
bool reprocessCsv_v1( QString fullLine, QList<QRegExp> patternsToMatch, QStringList& newList, const int nExpectedParts );


// Replaces single quotes " with double quotes "" and wraps s in double quotes.
QString csvQuote( QString s );

bool isHexDigit( const QChar& c );

#ifdef WINDOWS_OR_WHATEVER_IT_IS
void ConvertTToC(char* pszDest, const TCHAR* pszSrc);
void ConvertCToT(TCHAR* pszDest, const char* pszSrc);
#endif

#endif // STRUTILS_H
