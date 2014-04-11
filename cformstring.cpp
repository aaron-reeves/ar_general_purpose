/*
cformstring.cpp
Begin: 2003/06/11 (formerly cqformstring.cpp)
Last revision: $Date: 2011-10-25 04:57:17 $ $Author: areeves $
Version: $Revision: 1.3 $
Project: (various)
Website: http://www.aaronreeves.com/qtclasses
-------------------------------
Copyright (C) 2003 - 2007 by Aaron Reeves
aaron@aaronreeves.com

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#include "cformstring.h"

#include <ctype.h>

#include <qregexp.h>
#include <qstring.h>


/**
Creates a new CFormString from a standard QString.  By default, the new CFormString is marked as unencoded,
but this behavior may be changed by setting parameter isEncoded to 'true'.

@param str QString to convert to a CQFormString
@param isEncoded bool indicates whether the new CQFormString is encoded (true) or unencoded (false)
*/
CFormString::CFormString( bool isEncoded ) : QString() {
  isEncoded_p = isEncoded;
}


/**
Create an empty CQFormString.  By default, the CQFormString is marked as unencoded.

@param isEncoded bool indicates whether the new CQFormString is encoded (true) or unencoded (false)
*/
CFormString::CFormString( QString str, bool isEncoded ) : QString( str ) {
  isEncoded_p = isEncoded;
}


/**
Creates a new CFormString from a char*.  By default, the new CQFormString is marked as unencoded,
but this behavior may be changed by setting parameter isEncoded to 'true'.

@param charString char* to convert to a CQFormString
@param isEncoded bool indicates whether the new CQFormString is encoded (true) or unencoded (false)
*/
CFormString::CFormString( const char* charString, bool isEncoded ) : QString( charString ) {
  isEncoded_p = isEncoded;
}


/**
Replaces every instance of a single character with a different character.  Used to swap spaces and plus signs.

@param cBad char to replace
@param cGood replacement char
*/
void CFormString::swapChar( char cBad, char cGood ) {
  QString bad, good;
  bad = "\\";
  bad.append( cBad );
  good = cGood;

  this->replace( QRegExp( bad ), good );
}



/**
This function is used to unescape escaped characters.  Parameters are used to indicate the high and low bytes of
the hex encoding to unencode.  For the hex value 0x21, for example, hc is 2 and lc is 1.

@param hc char representing the high byte of a hex value
@param lc char representing the low byte of a hex value
@return int The decimal integer value represented by the hex characters.
*/
int CFormString::intFromHex(char hc, char lc ) {
  int Hi;        // holds high byte
  int Lo;        // holds low byte
  int Result;    // holds result

  // Get the value of the first byte to Hi

  Hi = hc;
  if ('0' <= Hi && Hi <= '9') {
    Hi -= '0';
  } else
  if ('a' <= Hi && Hi <= 'f') {
    Hi -= ('a'-10);
  } else
  if ('A' <= Hi && Hi <= 'F') {
    Hi -= ('A'-10);
  }

  // Get the value of the second byte to Lo

  Lo = lc;
  if ('0' <= Lo && Lo <= '9') {
    Lo -= '0';
  } else
  if ('a' <= Lo && Lo <= 'f') {
    Lo -= ('a'-10);
  } else
  if ('A' <= Lo && Lo <= 'F') {
    Lo -= ('A'-10);
  }
  Result = Lo + (16 * Hi);
  return (Result);
}



/**
Encodes this string according to the 'x-www-form-urlencoded'  rules shown here.
The argument encodeSpaces determines whether spaces are changes to plus signs
(encodeSpaces = false, the default) or are encoded as "&20" (encodeSpaces = true).

(all)		0x00 - 0x1F
(sp.)		0x20	 CHANGED TO PLUS (+) SYMBOL OR ENCODED, DEPENDING ON encodeSpaces
!			0x21
"			0x22
#	          0x23
$		0x24
%		0x25
&		0x26
'			0x27
(			0x28
)			0x29
* 		0x2A UNCHANGED
+ 		0x2B
,			0x2C
-		0x2D UNCHANGED
.			0x2E UNCHANGED
/			0x2F
0-9		0x30 - 0x39 UNCHANGED
:			0x3A
;			0x3B
<		0x3C
=		0x3D
>		0x3E
?			0x3F
@		0x40
A-Z	        	0x41 - 0x5A	UNCHANGED
[			0x5B
\			0x5C
]			0x5D
^		0x5E
_			0x5F UNCHANGED
`		0x60
a-z	        	0x61 - 0x7A UNCHANGED
{			0x7B
|			0x7C
}			0x7D
~		0x7E
(Del)		0x7F
(all)		0x80 - 0xFF

@param encodeSpaces bool indicating whether to replace (false) or encode (true) spaces
*/
void CFormString::urlDecode( void ) {
  CFormString temp;

  // First, change those pesky plusses to spaces
  this->swapChar( '+', ' '); // single quotes apparently mean something different than double quotes.  Very interesting!
  
  // Now, loop through looking for escapes
  int i;
  QChar hc, lc;
  
  for( i = 0; i < this->length(); ++i ) {
    if( this->mid( i, 1 ) == "%" ) {
      // A percent sign followed by two hex digits means
      // that the digits represent an escaped character.
      // We must decode it.
      
      if( ( this->length() - 1 ) < ( i + 2 ) ) { // This is a problem: the message is incomplete.
        // This mechanism doesn't solve the problem, but at least it should 
        // keep the application from failing with the following assertion:
        // ASSERT: "i >= 0 && i < size()" in file C:/Qt/4.1.4/include/QtCore/../../src/corelib/tools/qstring.h, line 543
        hc = '0';
        lc = '0';  
      }
      else {
        hc = this->at( i + 1 );
        lc = this->at( i + 2 );
      }
      
      if( isxdigit( hc.toLatin1() ) && isxdigit( lc.toLatin1() ) ) {
        temp.append( (char) intFromHex( hc.toLatin1(), lc.toLatin1() ) );
        i = i + 2;
      }
    }
    else {
      temp.append( this->at( i ) );
    }
  }
  
  isEncoded_p = false;
  *this = temp;
}



/**
Returns the hex encoding for the specified char.

@param c char to encode
@return hex-encoded character
*/
QString CFormString::hexFromInt( char c ) {
  QString str;
  
  if( c < 16 ) {
    str.sprintf( "%%0%X", c );
  }
  else {
    str.sprintf( "%%%X", c );
  }
  return str;
}



/**
All characters above 127 (7F hex) or below 33 (21 hex) are escaped.
This includes the space character, which is escaped as %20.
Also, the plus sign (+) needs to be interpreted as a space character.
*/
void CFormString::urlEncode( bool encodeSpaces ) {
  int i;
  CFormString temp;
  char c;

  for( i = 0; i < this->length(); ++i ) {
    c = this->at( i ).toLatin1();  // convert the QChar to a char (which is an int)

    if( c >= 0x00 && c <= 0x1F ) {
      temp.append( hexFromInt( c ) );
    }
    else if ( c == 0x20 && encodeSpaces ) {  // space
      temp.append( hexFromInt( c ) );
    }
    else if ( c >= 0x21 && c <= 0x29 ) { // !"#$%&'()
      temp.append( hexFromInt( c ) );
    }
    // * is OK
    else if ( c == 0x2B || c == 0x2C ) { //+,
      temp.append( hexFromInt( c ) );
    }
    // -. are OK
    else if ( c == 0x2F ) { // /
      temp.append( hexFromInt( c ) );
    }
    // 0123456789 are OK
    else if ( c >=0x3A && c <=0x40 ) {  // :;<=>?@
      temp.append( hexFromInt( c ) );
    }
    // ABCDEFGHIJKLMNOPQRSTUVWXYZ are OK
    else if ( c >= 0x5B && c <=0x5E ) {  // [\]^
      temp.append( hexFromInt( c ) );
    }
    // _ is OK
    else if ( c == 0x60 ) { // `
       temp.append( hexFromInt( c ) );
    }
    // abcdefghijklmnopqrstuvwxyz are OK
    else if ( c >= 0x7B ) {
      temp.append( hexFromInt( c ) );
    }
    else { // This is a safe, unencoded, OK character
      temp.append( c );
    }
  }

  if( !encodeSpaces ) {
    temp.swapChar( ' ', '+' ); 
  }

  isEncoded_p = true;
  *this = temp;
}


