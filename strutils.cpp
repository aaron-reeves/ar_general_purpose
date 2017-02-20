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


#include "strutils.h"

#include <qstring.h>
#include <qstringlist.h>
#include <qdebug.h>


QString quoteString( const QString& str, const QChar quoteMark /* = '"' */ ) {
  bool ok;
  QString res;

  str.toDouble( &ok );
  if( !ok )
    str.toInt( &ok );

  if( !ok ) {
    res = str;
    res.replace( quoteMark, QString( "%1%1" ).arg( quoteMark ) );
    res = QString( "%1%2%1" ).arg( quoteMark ).arg( str );
  }
  else
    res = str;

  return res;
}


QString camelCase( const QString& str ) {
  QString tmp = str;
  tmp.replace( QRegExp( "[~!@#$%\\^&*()\\-\\+={}\\[\\]|\\:\";'<>?,\\./_]" ), " " );

  QStringList list = tmp.simplified().split( ' ' );
  for( int i = 0; i < list.count(); ++i )
    list[i] = list.at(i).left(1).toUpper() + list.at(i).mid(1);

  return list.join( "" );
}


QString postgresCase( const QString& str ) {
  QString tmp = str;
  tmp.replace( QRegExp( "[~!@#$%\\^&*()\\-\\+={}\\[\\]|\\:\";'<>?,\\./_]" ), " " );

  QStringList list = tmp.simplified().split( ' ' );
  for( int i = 0; i < list.count(); ++i )
    list[i] = list.at(i).toLower();

  return list.join( "_" );
}


QString titleCase( QString str ){
  str = str.simplified().trimmed();
  QString result;
  QChar c;

  for( int i = 0; i < str.count(); ++i ) {
    c = str.at(i);

    if( 0 == i )
      result.append( c.toUpper() );
    else if( str.at(i-1).isLetter() )
      result.append( c.toLower() );
    else
      result.append( c );
  }

  return result;

//  QStringList list = str.simplified().split( ' ' );
//  for( int i = 0; i < list.count(); ++i )
//    list[i] = list.at(i).left(1).toUpper() + list.at(i).mid(1).toLower();

//  return list.join( " " );
}


QString boolToStr( const bool val ) {
  if( val )
    return "-1";
  else
    return "0";
}


QString boolToText( const bool val ) {
  if( val )
    return "true";
  else
    return "false";
}


QString variantBoolToText( const QVariant& val ) {
  if( val.type() != QVariant::Bool )
    return "invalid";
  else if( val.isNull() )
    return "null";
  else
    return boolToText( val.toBool() );
}


QString boolToYesNo( const bool val ) {
  if( val )
    return "Yes";
  else
    return "No";
}


bool strToBool( QString val, bool* ok /* = NULL */ ) {
  val = val.trimmed().toLower();

  if(
    "y" == val
    || "yes" == val
    || "t" == val
    || "true" == val
    || "1" == val
    || "-1" == val
  ) {
    if( NULL != ok )
      *ok = true;
    return true;
  } else if (
    "n" == val
    || "no" == val
    || "f" == val
    || "false" == val
    || "0" == val
  ) {
    if( NULL != ok )
      *ok = true;
    return false;
  }
  else {
    if( NULL != ok )
      *ok = false;
    return false;
  }
}


QString paddedInt( int toPad, const int places, const QChar padChar /* = '0' */ ) {
  QString str;

  str = QString( "%1" ).arg( toPad );

  return leftPaddedStr( str, places, padChar );
}


QString leftPaddedStr( QString toPad, const int places, const QChar padChar /* = ' ' */ ) {
  QString str;
  int i;
  int origStrLen;

  str = QString( "%1" ).arg( toPad );

  Q_ASSERT( str.length() <= places );

  //qDebug() << places << str.length() << (places - str.length());

  origStrLen = str.length();

  if( origStrLen < places ) {
    for( i = 0; i < places - origStrLen; ++i ) {
      str.prepend( padChar );
      //qDebug() << i << str;
    }
  }

  return str;
}


QString rightPaddedStr( QString toPad, const int places, const QChar padChar /* = ' ' */ ) {
  int i;
  int origStrLen;

  if( toPad.length() > places )
    qDebug() << toPad << places << toPad.length() << (places - toPad.length());

  Q_ASSERT( toPad.length() <= places );

  origStrLen = toPad.length();

  if( origStrLen < places ) {
    for( i = 0; i < places - origStrLen; ++i )
      toPad.append( padChar );
  }

  return toPad;
}


// Find and remove any instances of str3 from str1,
// Making sure that str3 isn't just a part of a longer string.
QString findAndRemove( QString str3, QString str1 ) {
  int pos;
  bool test1, test2, test3, test4;

  pos = str1.indexOf( str3 );

  while( pos > -1 ) {
    test1 = ( str1.at( pos - 1 ).isSpace() && str1.at( pos + str3.length() ).isSpace() );
    test2 = ( ( pos - 1 < 0 ) && str1.at( pos + str3.length() ).isSpace() );
    test3 = ( str1.at( pos - 1 ).isSpace() && ( pos + str3.length() == str1.length() ) );
    test4 = ( str1.trimmed() == str3 );

    if( test1 || test2 || test3 || test4 ) {
      str1.remove( pos, str3.length() + 1 );
      pos = str1.indexOf( str3, 0 );
    }
    else {
      pos = str1.indexOf( str3, pos + 1 );
    }
  }

  return str1;
}


// Find all instances of str3 in str1, and replace them with str2,
// making sure that str3 isn't just part of a longer string.
QString findAndReplace( QString str3, QString str2, QString str1 ) {
  int pos;
  bool test1, test2, test3, test4;

  pos = str1.indexOf( str3 );

  while( pos > -1 ) {
    test1 = ( str1.at( pos - 1 ).isSpace() && str1.at( pos + str3.length() ).isSpace() );
    test2 = ( ( pos - 1 < 0 ) && str1.at( pos + str3.length() ).isSpace() );
    test3 = ( str1.at( pos - 1 ).isSpace() && ( pos + str3.length() == str1.length() ) );
    test4 = ( str1.trimmed() == str3 );

    if( test1 || test2 || test3 || test4 ) {
      qDebug() << "Passed a test";
      str1.remove( pos, str3.length() + 1 );
      qDebug() << str1;
      str1.insert( pos, str2 );
      qDebug() << str1;
      pos = str1.indexOf( str3, 0 );
    }
    else {
      pos = str1.indexOf( str3, pos + 1 );
    }
  }

  return str1;
}


QString removeDelimiters( const QString& val, QChar delim ) {
  QString result = val.trimmed();

  if( val.endsWith( delim ) )
    result = val.left( val.length() - 1 );

  if( val.startsWith( delim ) )
    result = result.right(result.length() - 1 );

  return result;
}


QString removeWhiteSpace( QString str1 ) {
  str1 = str1.trimmed().simplified();
  str1.replace( QRegExp( "\\s" ), "" );

  return str1;
}


QString trimPunct( QString str ) {
  str = str.trimmed();
  str.replace( QRegExp( "[~!@#$%\\^&*()\\-\\+={}\\[\\]|\\:\";'<>?,\\./_]" ), "" );
  return str;
}


// Remove any line breaks in the provided string, and replace them with spaces.
QString removeLineBreaks( QString str1 ) {
  str1.replace( "\r\n", " " );
  str1.replace( "\r", " " ); 
  str1.replace( "\n", " " );
  
  return str1;
}


QString splitNear( int pos, QString & str, int maxLenAdd, bool usePunct, bool forceBreak ) {
  int i;
  QString tmp;
  QChar ch;
  QString result;

  tmp = str.trimmed();

  // Test 1: is the thing already short enough to return as it is?
  if( tmp.length() <= ( pos + maxLenAdd ) ) {
    result = tmp;
    str = "";
    return result;
  }

  //qDebug() << "TEST2";

  // Test 2: is there a suitable white space or punctuation mark for breaking the string?
  for( i = pos - 1; i > 0; --i ) {
    ch = tmp.at( i );

    if( ch.isSpace() || ( ch.isPunct() && usePunct ) ) {  // Split here!
      result = tmp.left( i ).trimmed();
      str = tmp.right( tmp.length() - i ).trimmed();
      //qDebug() << QString( "RESULT: %1" ).arg( result );
      //qDebug() << QString( "STR: %1" ).arg( str );
      return result;
    }
  }

  //qDebug() << "---TEST3";
  // If we get this far, the string is longer than the max
  // and contains no white spaces or suitable punctuation marks
  // before the requested position.

  // There are two options: if forceBreak,
  // Split it at the requested position and move on.

  // Otherwise, move FORWARD until the first suitable break location.

  if( forceBreak ) {
    result = tmp.left( pos ).trimmed();
    //qDebug() << QString( "Result is %1" ).arg( result );
    str = tmp.right( tmp.length() - pos ).trimmed();
    //qDebug() << QString( "str is %1" ).arg( str );
  }
  else {
    for( i = 0; i < tmp.length(); ++i ) {
      ch = tmp.at( i );
      if( ch.isSpace() || ( ch.isPunct() && usePunct ) ) {  // Split here!
        result = tmp.left( i ).trimmed();
        str = tmp.right( tmp.length() - i ).trimmed();
        return result;
      }
    }
  }
  return result;
}



QStringList prettyPrintedList( const QString srcStr, int prefLineLen, bool usePunct, bool forceBreak, int indent ) {
  QStringList srcLines, destLines;
  QString padding = "";
  int tenPct;
  QString str, str2;
  int maxLen;
  int i;

  for( i = 0; i < indent; ++i )
    padding.append( " " );

  tenPct = int ( prefLineLen / 10 );
  maxLen = prefLineLen + tenPct;

  //qDebug() << srcStr;

  // First, split the source string at specified line breaks.
  srcLines = srcStr.split( '\n' );

  // Split each line again, as close to x characters as possible
  for( i = 0; i < srcLines.count(); ++i ) {
    str = srcLines.at(i);

    do {
      if( str.length() <= maxLen ) {
        destLines.append( str );
        //qDebug() << QString( "SHRTSTR: %1" ).arg( str );
        str = "";
        break;
      }
      else {
        str2 = splitNear( prefLineLen, str, tenPct, usePunct, forceBreak );
        destLines.append( str2 );
        //qDebug() << QString( "LOOPING: %1" ).arg( str2 );
      }
    } while( ( str.length() > maxLen ) || ( str2 == str ) );

    if( 0 < str.trimmed().length() ) {
      destLines.append( str.trimmed() );
    }
  }

  for( i = 0; i < destLines.count(); ++i ) {
    destLines[i].prepend( padding );
  }

  return destLines;
}


QString prettyPrint( const QString srcStr, int prefLineLen, bool usePunct, bool forceBreak, int indent ) {
  QStringList destLines;
  QString result;
  int i;

  destLines = prettyPrintedList( srcStr, prefLineLen, usePunct, forceBreak, indent );

  // Assemble the final result from destLines
  result = "";
  for( i = 0; i < destLines.count(); ++i ) {
    //result.append( padding ); // Now handled by the function above.
    result.append( destLines.at(i) );
    result.append( "\n" );
  }

  return result;
}


bool isComment( const QString st ) {
  bool result;
  QString s = st.trimmed();

  if(
    ( '%' == s.at(0) )
  ||
    ( '\'' == s.at(0) )
  ||
    ( '#' == s.at(0) )
  ||
    ( "//" == s.left( 2 ) )
  ) {
    result = true;
  }
  else {
    result = false;
  }

  return result;
}


bool reprocessCsv( QString fullLine, QList<QRegExp> patternsToMatch, QStringList& newList, const int nExpectedParts, const int nTotalParts ) {
  QString newPart;
  int i;
  QRegExp re;
  bool success;
  bool prevMatchFound;
  bool debug = false;

  int counter = nTotalParts - patternsToMatch.count();

  re = patternsToMatch.takeAt(0);

  if( debug ) qDebug() << "fullLine (before processing):" << fullLine;
  if( debug ) qDebug() << QString( "Expression %1:" ).arg( counter ) << re.pattern();

  // Start with the entire line, and eliminate characters one at a time until the pattern matches the new string.
  i = 0;
  prevMatchFound = false;
  while( i <= fullLine.length() ) {
    newPart = fullLine.left( fullLine.length() - i );

    //if( debug ) qDebug() << "newPart (1):" << newPart;

    if( re.exactMatch( newPart) ) {
      // We found a match.  Keep going until we run out of it.
      prevMatchFound = true;
    }
    else {
      if( prevMatchFound ) {
        // We've shortened the line by one character too many.
        // Back off, and get out of the loop.
        --i;
        break;
      }
    }
    ++i;
  }

  //if( debug ) qDebug() << "Initial match:" << newPart;

  // Now, read forward again one character at a time for as long as the pattern matches the new string.
  prevMatchFound = false;
  i = newPart.length();
  while( i <= fullLine.length() ) {
    newPart = fullLine.left(i);

    //if( debug ) qDebug() << "newPart(2):" << newPart;

    if( !re.exactMatch( newPart ) ) {
      if( prevMatchFound ) {
        // We've read one character too many.
        // Back off, and get out of the loop.
        --i;
        break;
      }
    }
    else {
      // We (still) have a match.  Keep going until we run out of it.
      prevMatchFound = true;
    }

    ++i;
  }

  newPart = fullLine.left(i);
  if( re.exactMatch( newPart ) ) {
    if( debug ) qDebug() << "MATCH FOUND:" << newPart << endl;

    newList.append( newPart );
    //if( debug ) qDebug() << "Remaining line (before snip):" << fullLine.right( fullLine.length() - newPart.length() );

    //if( debug ) qDebug() << fullLine.length() << newPart.length();

    fullLine = fullLine.right( fullLine.length() - newPart.length() );

    // If there was a match, we may now have a leading comma.  Lop it off, if present.
    if( fullLine[0] == QChar( ',' ) )
    //if( !fullLine.isEmpty() && !newPart.isEmpty() )
      fullLine = fullLine.right( fullLine.length() - 1 );

    //if( debug ) qDebug() << "Remaining line:" << fullLine;

    if( 0 < patternsToMatch.count() ) { // Is there more to do?
      success = reprocessCsv( fullLine, patternsToMatch, newList, nExpectedParts - 1, nTotalParts );
    }
    else {
      if( fullLine.isEmpty() )
        success = true;
      else {
        if( debug ) qDebug() << "There are bits left of the string:" << fullLine;
        success = false;
      }
    }
  }
  else {
    // There is nothing more we can do.
    //qDebug() << "Failure trigger: 1";
    success = false;
    //newList.clear();
  }

  // Do we need to double-check anything here?
  // I don't think so.

  return success;
}


bool reprocessCsv_v1( QString fullLine, QList<QRegExp> patternsToMatch, QStringList& newList, const int nExpectedParts, const int nTotalParts ) {
  QString newPart;
  int i;
  QRegExp re;
  bool success;
  bool prevMatchFound;

  // Read one character at a time to assemble a new string.
  // Keep reading as long as the nth pattern matches the new string.
  i = 1;
  re = patternsToMatch.takeAt(0);
  prevMatchFound = false;
  while( i < fullLine.length() ) {
    newPart = fullLine.left(i);

    //qDebug() << "newPart:" << newPart;

    if( !re.exactMatch( newPart ) ) {
      if( prevMatchFound ) {
        // We've read one character too many.
        // Back off, and get out of the loop.
        --i;
        break;
      }
    }
    else {
      // We (still) have a match.  Keep going until we run out of it.
      prevMatchFound = true;
    }

    ++i;
  }

  newPart = fullLine.left(i);
  if( re.exactMatch( newPart ) ) {
    //qDebug() << "MATCH FOUND:" << newPart;

    newList.append( newPart );
    fullLine = fullLine.right( fullLine.length() - newPart.length() - 1 );// The -1 eliminates what is now a leading comma.

    if( 0 < patternsToMatch.count() ) { // Is there more to do?
      success = reprocessCsv_v1( fullLine, patternsToMatch, newList, nExpectedParts - 1, nTotalParts );
    }
    else {
      if( fullLine.isEmpty() )
        success = true;
      else {
        //qDebug() << "There are bits left of the string:" << fullLine;
        success = false;
      }
    }
  }
  else {
    // There is nothing more we can do.
    //qDebug() << "Failure trigger: 1";
    success = false;
    //newList.clear();
  }

  // Do we need to double-check anything here?
  // I don't think so.

  return success;
}


QString csvQuote( QString s ) {
  if(  s.contains( '"' ) )
    s.replace( '"', "\"\"" );

  s = QString( "\"%1\"" ).arg( s );
  return s;
}


bool isHexDigit( const QChar& c ) {
  bool result;
  if( c.isDigit() )
    return true;
  else {
    ushort s = c.toUpper().unicode();
    result = ( (65 <= s) && (70 >= s) ); // 'A' and 'F'
    return result;
  }
}


bool isEmailAddress( const QString& str ) {
  QRegExp re( "^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,6}$" );
  return re.exactMatch( str );
}


QDate guessDateFromString( QString dateStr, const StrUtilsDateFormat fmt, const int defaultCentury /* = 2000 */ ) {
  QDate result = QDate(); // An invalid date, unless a better one can be assigned.

  dateStr = dateStr.trimmed().toLower();

  // "yyyy-MM-dd"
  QRegExp basic( "^[0-9]{4}[-/]{1}[0-1]?[0-9]{1}[-/]{1}[0-3]?[0-9]{1}$" );

  // "dd/MM/yyyy"
  QRegExp ukDate( "^[0-3]?[0-9]{1}[-/]{1}[0-1]?[0-9]{1}[-/]{1}[0-9]{4}$" );

  // "MM/dd/yyyy"
  QRegExp usDate( "^[0-1]?[0-9]{1}[-/]{1}[0-3]?[0-9]{1}[-/]{1}[0-9]{4}$" );

  // 01-Jan-15
  QRegExp abbrevMonth1( "^[0-3]{1}[0-9]{1}[-/]{1}(jan|feb|mar|apr|may|jun|jul|aug|sep|oct|nov|dec)[-/][0-9]{2}$" );

  // 01-Jan-2015
  QRegExp abbrevMonth2( "^[0-3]{1}[0-9]{1}[-/]{1}(jan|feb|mar|apr|may|jun|jul|aug|sep|oct|nov|dec)[-/][0-9]{4}$" );

  // 1-Jan-15
  QRegExp abbrevMonth3( "^[1-3]?[0-9]{1}[-/]{1}(jan|feb|mar|apr|may|jun|jul|aug|sep|oct|nov|dec)[-/][0-9]{2}$" );

  // 1-Jan-2015
  QRegExp abbrevMonth4( "^[1-3]?[0-9]{1}[-/]{1}(jan|feb|mar|apr|may|jun|jul|aug|sep|oct|nov|dec)[-/][0-9]{4}$" );

  // "dd/MM/yyyy 00:00:00" (seconds optional)
  QRegExp ukDateTime( "^[0-3]?[0-9]{1}[-/]{1}[0-1]?[0-9]{1}[-/]{1}[0-9]{4}[\\s]+[0-9]{2}(:[0-9]{2}){1,2}$" );

  // "MM/dd/yyyy 00:00:00" (seconds optional)
  QRegExp usDateTime( "^[0-1]?[0-9]{1}[-/]{1}[0-3]?[0-9]{1}[-/]{1}[0-9]{4}[\\s]+[0-9]{2}(:[0-9]{2}){1,2}$" );

  QChar separator;
  if( dateStr.contains( '-') )
    separator = '-';
  else
    separator = '/';

  if( basic.exactMatch( dateStr ) ) {
    result = QDate::fromString( dateStr, QString( "yyyy%1MM%1dd" ).arg( separator ) );
  }

  else if( ukDate.exactMatch( dateStr ) && ( UKDateFormat == fmt ) ) {
    result = QDate::fromString( dateStr, QString( "dd%1MM%1yyyy" ).arg( separator ) );

    if( !result.isValid() ) {
      result = QDate::fromString( dateStr, QString( "d%1M%1yyyy" ).arg( separator ) );
    }
  }
  else if( usDate.exactMatch( dateStr ) && ( USDateFormat == fmt ) ) {
    result = QDate::fromString( dateStr, QString( "MM%1dd%1yyyy" ).arg( separator ) );

    if( !result.isValid() ) {
      result = QDate::fromString( dateStr, QString( "M%1d%1yyyy" ).arg( separator ) );
    }
  }

  else if( abbrevMonth1.exactMatch( dateStr ) ) {
    result = QDate::fromString( dateStr, QString( "dd%1MMM%1yy" ).arg( separator ) );

    if( !result.isValid() ) {
      result = QDate::fromString( dateStr, QString( "d%1MMM%1yy" ).arg( separator ) );
    }

    result = result.addYears( defaultCentury - ( QString( "%1" ).arg( result.year() ).left(2).toInt() * 100 ) );
  }
  else if( abbrevMonth2.exactMatch( dateStr ) ) {
    result = QDate::fromString( dateStr, QString( "dd%1MMM%1yyyy" ).arg( separator ) );

    if( !result.isValid() ) {
      result = QDate::fromString( dateStr, QString( "d%1MMM%1yyyy" ).arg( separator ) );
    }
  }
  else if( abbrevMonth3.exactMatch( dateStr ) ) {
    result = QDate::fromString( dateStr, QString( "dd%1MMM%1yy" ).arg( separator ) );

    if( !result.isValid() ) {
      result = QDate::fromString( dateStr, QString( "d%1MMM%1yy" ).arg( separator ) );
    }

    result = result.addYears( defaultCentury - ( QString( "%1" ).arg( result.year() ).left(2).toInt() * 100 ) );
  }
  else if( abbrevMonth4.exactMatch( dateStr ) ) {
    result = QDate::fromString( dateStr, QString( "dd%1MMM%1yyyy" ).arg( separator ) );

    if( !result.isValid() ) {
      result = QDate::fromString( dateStr, QString( "d%1MMM%1yyyy" ).arg( separator ) );
    }
  }

  else if( ukDateTime.exactMatch( dateStr ) && ( UKDateFormat == fmt ) ) {
    QDateTime dt = QDateTime::fromString( dateStr, QString( "dd%1MM%1yyyy hh:mm" ).arg( separator ) );
    if( !dt.isValid() ) {
      dt = QDateTime::fromString( dateStr, QString( "d%1M%1yyyy hh:mm" ).arg( separator ) );
    }
    if( !dt.isValid() ) {
      dt = QDateTime::fromString( dateStr, QString( "dd%1MM%1yyyy hh:mm:ss" ).arg( separator ) );
    }
    if( !dt.isValid() ) {
      dt = QDateTime::fromString( dateStr, QString( "d%1M%1yyyy hh:mm:ss" ).arg( separator ) );
    }
    result = dt.date();
  }

  else if( usDateTime.exactMatch( dateStr ) && ( USDateFormat == fmt ) ) {
    QDateTime dt = QDateTime::fromString( dateStr, QString( "MM%1dd%1yyyy hh:mm" ).arg( separator ) );
    if( !dt.isValid() ) {
      dt = QDateTime::fromString( dateStr, QString( "M%1d%1yyyy hh:mm" ).arg( separator ) );
    }
    if( !dt.isValid() ) {
      dt = QDateTime::fromString( dateStr, QString( "MM%1dd%1yyyy hh:mm:ss" ).arg( separator ) );
    }
    if( !dt.isValid() ) {
      dt = QDateTime::fromString( dateStr, QString( "M%1d%1yyyy hh:mm:ss" ).arg( separator ) );
    }
    result = dt.date();
  }

  return result;
}


#if defined(_WIN32) || defined(WIN32)
// The following functions are adapted from
// http://msdn.microsoft.com/archive/default.asp?url=/archive/en-us/dnarppc2k/html/ppc_ode.asp

void ConvertTToC(char* pszDest, const TCHAR* pszSrc) {
  // FIX ME: zero out the memory for pszDest
  for(unsigned int i = 0; i < _tcslen(pszSrc); i++)
    pszDest[i] = (char) pszSrc[i];
}



void ConvertCToT(TCHAR* pszDest, const char* pszSrc) {
  // FIX ME: zero out the memory for pszDest
  for(unsigned int i = 0; i < strlen(pszSrc); i++)
    pszDest[i] = (TCHAR) pszSrc[i];
}
#endif
