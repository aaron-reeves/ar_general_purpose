/*
strutils.h/cpp
--------------
Author: Aaron Reeves <aaron.reeves@naadsm.org>
--------------------------------------------------
Copyright (C) 2007 - 2018 Aaron Reeves

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

QStringList regularExpressionMatches( const QString& pattern, const QString& subject );

QString abbreviatePath( const QString& path , const int targetLength = 0 ); // if targetLength > 0, aim for that.  Otherwise, just do what you can.

QString quoteString( const QString& str, const QChar quoteMark = '"' );
QString camelCase( const QString& str );
QString postgresCase( const QString& str );
QString titleCase( QString str );

QString findAndRemove( QString str3, QString str1 );
QString findAndReplace( QString str3, QString str2, QString str1 );

QString removeLineBreaks( QString str1 );
QString removeDelimiters( const QString& val, QChar delim );
QString removeWhiteSpace( QString str1 );
QString removeWhiteSpace( const char* str1 );
QString trimPunct( QString str );
QString leftTrimmed( QString str );
QString rightTrimmed( QString str );

bool strIsBool( const QString& str );
QString boolToStr( const bool val ); // returns "-1" or "0"
QString boolToText( const bool val ); // returns "true" or "false"
QString boolToYesNo( const bool val ); // returns "Yes" or "No"
bool strToBool( QString val, bool* ok = nullptr );
QString variantBoolToText( const QVariant& val ); // Returns "invalid", "null", "true", or "false"

bool strIsInt( const QString& str );
bool strIsDouble( const QString& str );
bool strIsNumber( const QString& str );
int strToInt(const QString& str, const int defaultVal );
double strToDouble( const QString& str, const double defaultVal );


QString paddedInt( int toPad, const int places, const QChar padChar = '0' );
QString leftPaddedStr( QString toPad, const int places, const QChar padChar = ' ' );
QString rightPaddedStr( QString toPad, const int places, const QChar padChar = ' ' );

QString splitNear( int pos, QString & str, int maxLenAdd = 0, bool usePunct = true, bool forceBreak = true );
QStringList prettyPrintedList( const QString srcStr, int prefLineLen = 50, bool usePunct = true, bool forceBreak = true, int indent = 0 );
QString prettyPrint( const QString srcStr, int prefLineLen = 50, bool usePunct = true, bool forceBreak = true, int indent = 0 );

QStringList stringsFromVariants( const QList<QVariant>& variants );
QStringList stringsFromVariants( const QVector<QVariant>& variants );

bool isComment( const QString st );

void stringListListAsTable( const QList<QStringList>& rows, QTextStream* stream, const bool useHeader );
bool isEmptyStringList( const QStringList& list );
bool stringListContainsDuplicates( const QStringList& list, const bool countBlanks );
bool stringListContainsBlanks( const QStringList& list );

/* This function attempts to rebuild a properly formatted comma-separated list from one that
 * was poorly formatted.  The function needs to know how many parts were expected and must have
 * a list of regular expressions for matching each part.
 *
 * If a new list can be constructed that matches all of the regular expressions, then success is set to true.
 */
bool reprocessCsv( QString fullLine, QList<QRegExp> patternsToMatch, QStringList& newList, const int nExpectedParts, const int nTotalParts );
bool reprocessCsv_v1( QString fullLine, QList<QRegExp> patternsToMatch, QStringList& newList, const int nExpectedParts, const int nTotalParts );


// Replaces single quotes " with double quotes "" and wraps s in double quotes.
QString csvQuote( QString s );
QString csvQuote( QStringList s, const QChar delimiter = ',' );

bool isHexDigit( const QChar& c );

bool isEmailAddress( const QString& str );

enum StrUtilsDateFormat {
  USDateFormat,
  UKDateFormat
};

QDate guessDateFromString( QString dateStr, const StrUtilsDateFormat fmt, const int defaultCentury = 2000 );


#if defined(_WIN32) || defined(WIN32)
#include <tchar.h>
void ConvertTToC(char* pszDest, const TCHAR* pszSrc);
void ConvertCToT(TCHAR* pszDest, const char* pszSrc);
#endif

#endif // STRUTILS_H
