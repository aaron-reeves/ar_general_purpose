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


#include "strutils.h"

#include <qstring.h>
#include <qstringlist.h>
#include <qdebug.h>

#include <ar_general_purpose/qcout.h>

// From https://emailregex.com/
const QRegExp regExpEmail( "(?:[a-z0-9!#$%&'*+/=?^_`{|}~-]+(?:\\.[a-z0-9!#$%&'*+/=?^_`{|}~-]+)*|\"(?:[\\x01-\\x08\\x0b\\x0c\\x0e-\\x1f\\x21\\x23-\\x5b\\x5d-\\x7f]|\\\\[\\x01-\\x09\\x0b\\x0c\\x0e-\\x7f])*\")@(?:(?:[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\\.)+[a-z0-9](?:[a-z0-9-]*[a-z0-9])?|\\[(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?|[a-z0-9-]*[a-z0-9]:(?:[\\x01-\\x08\\x0b\\x0c\\x0e-\\x1f\\x21-\\x5a\\x53-\\x7f]|\\\\[\\x01-\\x09\\x0b\\x0c\\x0e-\\x7f])+)\\])" );

// A simpler version, from a simpler time...
//const QRegExp regExpEmail( "^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,6}$" );


QStringList regularExpressionMatches( const QString& pattern, const QString& subject ) {
  QStringList matches;

  QRegularExpression rex( pattern );
  QRegularExpressionMatchIterator rexIt;

  if( rex.isValid() ) {
    rexIt = rex.globalMatch( subject );
    while( rexIt.hasNext() ) {
      QRegularExpressionMatch match = rexIt.next();
      matches.append( match.captured() );
    }
  }

  return matches;
}


bool isEmailAddress( const QString& val ) {
  return regExpEmail.exactMatch( val );
}


QString abbreviatePath( const QString& path, const int targetLength ) {
  QStringList list = path.split( '/' );
  QString result;


  if( 6 >= list.count() )
    result = path;
  else
    result = QStringLiteral( "%1/%2/%3/.../%4/%5" ).arg( list.at(0), list.at(1), list.at(2), list.at( list.count() - 2), list.at( list.count() - 1 ) );

  if( 0 == targetLength )
    return result;
  else if( targetLength > result.length() )
    return result;
  else if( 5 >= list.count() )
    result = path;
  else
    result = QStringLiteral( "%1/%2/.../%3/%4" ).arg( list.at(0), list.at(1), list.at( list.count() - 2), list.at( list.count() - 1 ) );

  if( targetLength > result.length() )
    return result;
  else if( 4 >= list.count() )
    result = path;
  else
    result = QStringLiteral( "%1/%2/.../%3" ).arg( list.at(0), list.at(1), list.at( list.count() - 1) );

  if( targetLength > result.length() )
    return result;
  else if( 3 >= list.count() )
    result = path;
  else
    result = QStringLiteral( "%1/.../%2" ).arg( list.at(0), list.at( list.count() - 1) );

  return result;
}


QString quoteString( const QString& str, const QChar quoteMark /* = '"' */ ) {
  bool ok;
  QString res;

  str.toDouble( &ok );
  if( !ok )
    str.toInt( &ok );

  if( !ok ) {
    res = str;
    res.replace( quoteMark, QStringLiteral( "%1%1" ).arg( quoteMark ) );
    res = QStringLiteral( "%1%2%1" ).arg( quoteMark ).arg( str );
  }
  else
    res = str;

  return res;
}


QString camelCase( QString str ) {
  str.replace( QRegExp( "[~!@#$%\\^&*()\\-\\+={}\\[\\]|\\:\";'<>?,\\./_]" ), QStringLiteral( " ") );

  QStringList list = str.simplified().split( ' ' );
  for( int i = 0; i < list.count(); ++i )
    list[i] = list.at(i).at(0).toUpper() + list.at(i).mid(1);

  return list.join( QString() );
}


QString postgresCase( QString str ) {
  str.replace( QRegExp( "[~!@#$%\\^&*()\\-\\+={}\\[\\]|\\:\";'<>?,\\./_]" ), QStringLiteral(" ") );

  QStringList list = str.simplified().split( ' ' );
  for( int i = 0; i < list.count(); ++i )
    list[i] = list.at(i).toLower();

  return list.join( '_' );
}


QString titleCase( const QString& strng ){
  QString str = strng.simplified().trimmed();
  QString result;
  QChar c;

  for( int i = 0; i < str.count(); ++i ) {
    c = str.at(i);

    if( 0 == i )
      result.append( c.toUpper() );
    else if( str.at(i-1).isSpace() )
      result.append( c.toUpper() );
    else if( str.at(i-1).isLetter() )
      result.append( c.toLower() );
    else
      result.append( c );
  }

  return result;
}


void toTitleCase( QString& str ) {
  str = titleCase( str );
}


QString boolToStr( const bool val ) {
  if( val )
    return QStringLiteral("-1");
  else
    return QStringLiteral("0");
}


QString boolToText( const bool val ) {
  if( val )
    return QStringLiteral("true");
  else
    return QStringLiteral("false");
}


QString variantBoolToText( const QVariant& val ) {
  if( val.type() != QVariant::Bool )
    return QStringLiteral("invalid");
  else if( val.isNull() )
    return QStringLiteral("null");
  else
    return boolToText( val.toBool() );
}


QString boolToYesNo( const bool val ) {
  if( val )
    return QStringLiteral("Yes");
  else
    return QStringLiteral("No");
}


bool strIsBool( const QString& str ) {
  bool ok;
  strToBool( str, &ok );
  return ok;
}


bool strToBool( QString val, bool* ok /* = nullptr */ ) {
  val = val.trimmed().toLower();

  if(
    "y" == val
    || "yes" == val
    || "t" == val
    || "true" == val
    || "1" == val
    || "-1" == val
    || "positive" == val
    || "pos" == val
  ) {
    if( nullptr != ok )
      *ok = true;
    return true;
  } else if (
    "n" == val
    || "no" == val
    || "f" == val
    || "false" == val
    || "0" == val
    || "negative" == val
    || "neg" == val
  ) {
    if( nullptr != ok )
      *ok = true;
    return false;
  }
  else {
    if( nullptr != ok )
      *ok = false;
    return false;
  }
}


bool strIsInt( const QString& str ) {
  bool ok;
  str.toInt( &ok );
  return ok;
}

bool strIsDouble( const QString& str ) {
  bool ok;
  str.toDouble( &ok );
  return ok;
}

bool strIsNumber( const QString& str ) {
  return( strIsInt( str ) || strIsDouble( str ) );
}


int strToInt( const QString& str, const int defaultVal ) {
  bool ok;

  int result = str.toInt( &ok );

  if( !ok )
    result = defaultVal;

  return result;
}

double strToDouble( const QString& str, const double defaultVal ) {
  bool ok;

  double result = str.toDouble( &ok );

  if( !ok )
    result = defaultVal;

  return result;
}


bool isNullOrEmpty( const QVariant& v ) {
  if( v.isNull() )
    return true;
  else if( ( QVariant::String == v.type() ) && v.toString().isEmpty() )
    return true;
  else
    return false;
}


QString paddedInt( int toPad, const int places, const QChar padChar /* = '0' */ ) {
  QString str;

  str = QStringLiteral( "%1" ).arg( toPad );

  return leftPaddedStr( str, places, padChar );
}


QString leftPaddedStr( QString toPad, const int places, const QChar padChar /* = ' ' */ ) {
  if( places == toPad.length() ) {
    return toPad;
  }
  else {
    int i;
    int origStrLen;

    //Q_ASSERT( str.length() <= places );

    if( toPad.length() > places ) {
      toPad = toPad.left( places - 1 );
    }

    origStrLen = toPad.length();

    if( origStrLen < places ) {
      for( i = 0; i < places - origStrLen; ++i ) {
        toPad.prepend( padChar );
      }
    }

    return toPad;
  }
}


QString rightPaddedStr( QString toPad, const int places, const QChar padChar /* = ' ' */ ) {
  if( places == toPad.length() ) {
    return toPad;
  }
  else {
    int i;
    int origStrLen;

    if( toPad.length() > places )
      qDb() << toPad << places << toPad.length() << (places - toPad.length());

    //Q_ASSERT( toPad.length() <= places );

    if( toPad.length() >= places ) {
      toPad = toPad.left( places - 1 );
    }

    origStrLen = toPad.length();

    if( origStrLen < places ) {
      for( i = 0; i < places - origStrLen; ++i )
        toPad.append( padChar );
    }

    return toPad;
  }
}


// Find and remove any instances of str3 from str1,
// Making sure that str3 isn't just a part of a longer string.
QString findAndRemove( const QString& str3, QString str1 ) {
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
QString findAndReplace( const QString& str3, const QString& str2, QString str1 ) {
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
      str1.insert( pos, str2 );
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
  str1.replace( QRegExp( "\\s" ), QString() );

  return str1;
}

QString removeWhiteSpace( const char* str1 ) {
  return removeWhiteSpace( QString( str1 ) );
}


QString removePunct( QString str ) {
  str.replace( QRegExp( "[~!@#$%\\^&*()\\-\\+={}\\[\\]|\\:\";'<>?,\\./_]" ), QString() );

  for( int i = str.length() - 1; -1 < i; --i ) {
    if( str.at(i).isPunct() )
      str.replace( i, 1, QString() );
  }

  return str;
}


QString trimPunct( QString str ) {
  str = str.trimmed();
  return removePunct( str );
}

QString leftTrimmed( QString str ) {
  return( str.replace( QRegExp( "^\\s+" ), QString() ) );
}


QString rightTrimmed( QString str ) {
  return( str.replace( QRegExp( "\\s+$" ), QString() ) );
}


// Remove any line breaks in the provided string, and replace them with spaces.
QString removeLineBreaks( QString str1 ) {
  str1.replace( QLatin1String("\r\n"), QLatin1String(" ") );
  str1.replace( QLatin1String("\r"), QLatin1String(" ") );
  str1.replace( QLatin1String("\n"), QLatin1String(" ") );
  
  return str1;
}


QString splitNear( const int pos, QString& str, const int maxLenAdd, const bool usePunct, const bool forceBreak ) {
  int i;
  QString tmp;
  QChar ch;
  QString result;

  tmp = str.trimmed();

  // Test 1: is the thing already short enough to return as it is?
  if( tmp.length() <= ( pos + maxLenAdd ) ) {
    result = tmp;
    str = QString();
    return result;
  }

  // Test 2: is there a suitable white space or punctuation mark for breaking the string?
  for( i = pos - 1; i > 0; --i ) {
    ch = tmp.at( i );

    if( ch.isSpace() || ( ch.isPunct() && usePunct ) ) {  // Split here!
      result = tmp.left( i ).trimmed();
      str = tmp.right( tmp.length() - i ).trimmed();
      return result;
    }
  }

  // If we get this far, the string is longer than the max
  // and contains no white spaces or suitable punctuation marks
  // before the requested position.

  // There are two options: if forceBreak,
  // Split it at the requested position and move on.

  // Otherwise, move FORWARD until the first suitable break location.

  if( forceBreak ) {
    result = tmp.left( pos ).trimmed();
    str = tmp.right( tmp.length() - pos ).trimmed();
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



QStringList prettyPrintedList( const QString& srcStr, const int prefLineLen, const bool usePunct, const bool forceBreak, const int indent ) {
  QStringList srcLines, destLines;
  QString padding = QString();
  int tenPct;
  QString str, str2;
  int maxLen;
  int i;

  for( i = 0; i < indent; ++i )
    padding.append( " " );

  tenPct = int ( prefLineLen / 10 );
  maxLen = prefLineLen + tenPct;

  // First, split the source string at specified line breaks.
  srcLines = srcStr.split( '\n' );

  // Split each line again, as close to x characters as possible
  for( i = 0; i < srcLines.count(); ++i ) {
    str = srcLines.at(i);

    do {
      if( str.length() <= maxLen ) {
        destLines.append( str );
        str = QString();
        break;
      }
      else {
        str2 = splitNear( prefLineLen, str, tenPct, usePunct, forceBreak );
        destLines.append( str2 );
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


QString prettyPrint( const QString& srcStr, int prefLineLen, bool usePunct, bool forceBreak, int indent ) {
  QStringList destLines;
  QString result;
  int i;

  destLines = prettyPrintedList( srcStr, prefLineLen, usePunct, forceBreak, indent );

  // Assemble the final result from destLines
  result = QString();
  for( i = 0; i < destLines.count(); ++i ) {
    //result.append( padding ); // Now handled by the function above.
    result.append( destLines.at(i) );
    result.append( "\n" );
  }

  return result;
}


QString truncate( const QString& srcStr, const int prefLineLen, const bool usePunct ) {
  if( srcStr.length() <= prefLineLen ) {
    return srcStr;
  }
  else {
    QStringList destLines;
    QString result;

    destLines = prettyPrintedList( srcStr, prefLineLen - 3, usePunct, true, 0 );

    return QString( "%1..." ).arg( destLines.at(0) );
  }
}


QStringList stringsFromVariants( const QList<QVariant>& variants ) {
  QStringList result;

  for( int i = 0; i < variants.count(); ++i ) {
    result.append( variants.at(i).toString() );
  }

  return result;
}


QStringList stringsFromVariants( const QVector<QVariant>& variants ) {
  QStringList result;

  for( int i = 0; i < variants.length(); ++i ) {
    result.append( variants.at(i).toString() );
  }

  return result;
}


bool isComment(const QString& st ) {
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

  if( debug ) qDb() << "fullLine (before processing):" << fullLine;
  if( debug ) qDb() << QStringLiteral( "Expression %1:" ).arg( counter ) << re.pattern();

  // Start with the entire line, and eliminate characters one at a time until the pattern matches the new string.
  i = 0;
  prevMatchFound = false;
  while( i <= fullLine.length() ) {
    newPart = fullLine.left( fullLine.length() - i );

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

  // Now, read forward again one character at a time for as long as the pattern matches the new string.
  prevMatchFound = false;
  i = newPart.length();
  while( i <= fullLine.length() ) {
    newPart = fullLine.left(i);

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
    if( debug ) qDb() << "MATCH FOUND:" << newPart << endl;

    newList.append( newPart );
    fullLine = fullLine.right( fullLine.length() - newPart.length() );

    // If there was a match, we may now have a leading comma.  Lop it off, if present.
    if( fullLine[0] == QChar( ',' ) )
      fullLine = fullLine.right( fullLine.length() - 1 );

    if( 0 < patternsToMatch.count() ) { // Is there more to do?
      success = reprocessCsv( fullLine, patternsToMatch, newList, nExpectedParts - 1, nTotalParts );
    }
    else {
      if( fullLine.isEmpty() )
        success = true;
      else {
        if( debug ) qDb() << "There are bits left of the string:" << fullLine;
        success = false;
      }
    }
  }
  else {
    // There is nothing more we can do.
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

    newList.append( newPart );
    fullLine = fullLine.right( fullLine.length() - newPart.length() - 1 );// The -1 eliminates what is now a leading comma.

    if( 0 < patternsToMatch.count() ) { // Is there more to do?
      success = reprocessCsv_v1( fullLine, patternsToMatch, newList, nExpectedParts - 1, nTotalParts );
    }
    else {
      if( fullLine.isEmpty() )
        success = true;
      else {
        success = false;
      }
    }
  }
  else {
    // There is nothing more we can do.
    success = false;
    //newList.clear();
  }

  // Do we need to double-check anything here?
  // I don't think so.

  return success;
}


QString csvQuote( QString s ) {
  if(  s.contains( '"' ) )
    s.replace( '"', QLatin1String("\"\"") );

  s = QStringLiteral( "\"%1\"" ).arg( s );
  return s;
}


QString csvQuote( const QStringList& s, const QChar delimiter /* = ',' */ ) {
  QString result;

  for( int i = 0; i < s.count() - 1; ++i ) {
    result.append( csvQuote( s.at(i) ) );
    result.append( delimiter );
  }

  result.append( csvQuote( s.last() ) );

  return result;
}


bool isHexDigit( const QChar c ) {
  bool result;
  if( c.isDigit() )
    return true;
  else {
    ushort s = c.toUpper().unicode();
    result = ( (65 <= s) && (70 >= s) ); // 'A' and 'F'
    return result;
  }
}


QDate guessDateFromString( const QString& dateString, const StrUtilsDateFormat fmt, const int defaultCentury /* = 2000 */ ) {
  QDate result = QDate(); // An invalid date, unless a better one can be assigned.

  QString dateStr = dateString.trimmed().toLower();

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
    result = QDate::fromString( dateStr, QStringLiteral( "yyyy%1MM%1dd" ).arg( separator ) );
  }

  else if( ukDate.exactMatch( dateStr ) && ( UKDateFormat == fmt ) ) {
    result = QDate::fromString( dateStr, QStringLiteral( "dd%1MM%1yyyy" ).arg( separator ) );

    if( !result.isValid() ) {
      result = QDate::fromString( dateStr, QStringLiteral( "d%1M%1yyyy" ).arg( separator ) );
    }
  }
  else if( usDate.exactMatch( dateStr ) && ( USDateFormat == fmt ) ) {
    result = QDate::fromString( dateStr, QStringLiteral( "MM%1dd%1yyyy" ).arg( separator ) );

    if( !result.isValid() ) {
      result = QDate::fromString( dateStr, QStringLiteral( "M%1d%1yyyy" ).arg( separator ) );
    }
  }

  else if( abbrevMonth1.exactMatch( dateStr ) ) {
    result = QDate::fromString( dateStr, QStringLiteral( "dd%1MMM%1yy" ).arg( separator ) );

    if( !result.isValid() ) {
      result = QDate::fromString( dateStr, QStringLiteral( "d%1MMM%1yy" ).arg( separator ) );
    }

    result = result.addYears( defaultCentury - ( QStringLiteral( "%1" ).arg( result.year() ).leftRef(2).toInt() * 100 ) );
  }
  else if( abbrevMonth2.exactMatch( dateStr ) ) {
    result = QDate::fromString( dateStr, QStringLiteral( "dd%1MMM%1yyyy" ).arg( separator ) );

    if( !result.isValid() ) {
      result = QDate::fromString( dateStr, QStringLiteral( "d%1MMM%1yyyy" ).arg( separator ) );
    }
  }
  else if( abbrevMonth3.exactMatch( dateStr ) ) {
    result = QDate::fromString( dateStr, QStringLiteral( "dd%1MMM%1yy" ).arg( separator ) );

    if( !result.isValid() ) {
      result = QDate::fromString( dateStr, QStringLiteral( "d%1MMM%1yy" ).arg( separator ) );
    }

    result = result.addYears( defaultCentury - ( QStringLiteral( "%1" ).arg( result.year() ).leftRef(2).toInt() * 100 ) );
  }
  else if( abbrevMonth4.exactMatch( dateStr ) ) {
    result = QDate::fromString( dateStr, QStringLiteral( "dd%1MMM%1yyyy" ).arg( separator ) );

    if( !result.isValid() ) {
      result = QDate::fromString( dateStr, QStringLiteral( "d%1MMM%1yyyy" ).arg( separator ) );
    }
  }

  else if( ukDateTime.exactMatch( dateStr ) && ( UKDateFormat == fmt ) ) {
    QDateTime dt = QDateTime::fromString( dateStr, QStringLiteral( "dd%1MM%1yyyy hh:mm" ).arg( separator ) );
    if( !dt.isValid() ) {
      dt = QDateTime::fromString( dateStr, QStringLiteral( "d%1M%1yyyy hh:mm" ).arg( separator ) );
    }
    if( !dt.isValid() ) {
      dt = QDateTime::fromString( dateStr, QStringLiteral( "dd%1MM%1yyyy hh:mm:ss" ).arg( separator ) );
    }
    if( !dt.isValid() ) {
      dt = QDateTime::fromString( dateStr, QStringLiteral( "d%1M%1yyyy hh:mm:ss" ).arg( separator ) );
    }
    result = dt.date();
  }

  else if( usDateTime.exactMatch( dateStr ) && ( USDateFormat == fmt ) ) {
    QDateTime dt = QDateTime::fromString( dateStr, QStringLiteral( "MM%1dd%1yyyy hh:mm" ).arg( separator ) );
    if( !dt.isValid() ) {
      dt = QDateTime::fromString( dateStr, QStringLiteral( "M%1d%1yyyy hh:mm" ).arg( separator ) );
    }
    if( !dt.isValid() ) {
      dt = QDateTime::fromString( dateStr, QStringLiteral( "MM%1dd%1yyyy hh:mm:ss" ).arg( separator ) );
    }
    if( !dt.isValid() ) {
      dt = QDateTime::fromString( dateStr, QStringLiteral( "M%1d%1yyyy hh:mm:ss" ).arg( separator ) );
    }
    result = dt.date();
  }

  return result;
}


QString stringListListTableHeader( const QList<int>& arr ) {
  int i, j;
  int spaces;
  QString head;

  for( i = 0; arr.size() > i; ++i ) {
    head.append( "+" );
    spaces = arr.at( i );
    for( j = 0; (spaces+2) > j; ++j ) {
      head.append( "-" );
    }
  }

  head.append( "+" );

  return head;
}


QString stringListListTableRow( const QString& label, const int len ) {
  QString row;
  int i;
  int lenDiff;

  if( label.length() <= len ) {
    // Prepend the leading space
    row = QStringLiteral( " %1" ).arg( label ); // Note the leading space

    // Add spaces until desired length is reached
    lenDiff = len - label.length();
    for( i = 0; lenDiff > i; ++i ) {
      row.append( " " );
    }

    // Tack on the trailing space
    row.append( " " );
  }
  else {
    row = QStringLiteral( " %1" ).arg( label ); // Note the leading space
    row = row.left( len - 2 );
    row = row + "... "; // Note the trailing space
  }

  return row;
}


void stringListListAsTable( const QList<QStringList>& rows, QTextStream* stream, const bool useHeader ) {
  if(( nullptr == stream) ) {
    return;
  }

  QStringList row;
  QList<int> colWidths;

  for( int c = 0; c < rows.at(0).count(); ++c ) {
    colWidths.append( rows.at(0).at(c).length() );
  }

  // Loop through once to determine max column widths
  for( int r = 1; r < rows.count(); ++r ) {
    for( int c = 0; c < rows.at(r).count(); ++c ) {
      if( colWidths.at(c) < rows.at(r).at(c).length() ) {
        colWidths.replace( c, rows.at(r).at(c).length() );
      }
    }
  }

  // Start writing!
  //===============

  // Header row first, if present
  //-----------------------------
  *stream << stringListListTableHeader( colWidths ) << endl;
  int firstRow;

  if( useHeader ) {
    row = rows.at(0);
    for( int c = 0; c < row.count(); ++c ) {
       *stream << "|" << stringListListTableRow( row.at(c), colWidths.at(c) );
    }
    *stream << "|" << endl;
    *stream << stringListListTableHeader( colWidths ) << endl;
    firstRow = 1;
  }
  else {
    firstRow = 0;
  }

  // Then all other rows
  //--------------------
  for( int r = firstRow; r < rows.count(); ++r ) {
    row = rows.at(r);
    for( int c = 0; c < row.count(); ++c ) {
       *stream << "|" << stringListListTableRow( row.at(c), colWidths.at(c) );
    }
    *stream << "|" << endl;
  }

  *stream << stringListListTableHeader( colWidths ) << endl;
  *stream << flush;
}


bool isEmptyStringList( const QStringList& list ) {
  bool result = true;

  for( int i = 0; i < list.count(); ++i ) {
    if( 0 < list.at(i).trimmed().length() ) {
      result = false;
      break;
    }
  }

  return result;
}


bool stringListContainsDuplicates( const QStringList& list, const bool countBlanks ) {
  QSet<QString> strSet;

  for( int i = 0; i < list.count(); ++i ) {
    if( list.at(i).isEmpty() && !countBlanks )
      continue;
    else if( strSet.contains( list.at(i) ) )
      return true;
    else
      strSet.insert( list.at(i) );
  }

  return false;
}


bool stringListContainsBlanks( const QStringList& list ) {
  for( int i = 0; i < list.count(); ++i ) {
    if( list.at(i).isEmpty() )
      return true;
  }

  return false;
}


bool stringlistToFile( const QString& fileName, const QList<QString>& list ) {
  QFile f( fileName );
  if( f.open( QFile::WriteOnly | QFile::Truncate ) ) {
    QTextStream out(&f);
    for( int i = 0; i < list.count(); ++i ) {
      out << list.at(i) << endl;
    }
    return true;
  }
  else {
    return false;
  }
}


#if defined(_WIN32) || defined(WIN32)
// The following functions are adapted from
// http://msdn.microsoft.com/archive/default.asp?url=/archive/en-us/dnarppc2k/html/ppc_ode.asp

void ConvertTToC(char* pszDest, const TCHAR* pszSrc) {
  // FIX ME: zero out the memory for pszDest
  for(unsigned int i = 0; i < _tcslen(pszSrc); i++)
    pszDest[i] =  char( pszSrc[i] );
}



void ConvertCToT(TCHAR* pszDest, const char* pszSrc) {
  // FIX ME: zero out the memory for pszDest
  for(unsigned int i = 0; i < strlen(pszSrc); i++)
    pszDest[i] = TCHAR( pszSrc[i] );
}
#endif
