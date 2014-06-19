/*
cformstring.h/cpp
Begin: 2003/06/11 (formerly cqformstring.cpp)
-------------------------------
Copyright (C) 2003 - 2007 by Aaron Reeves
aaron.reeves@naadsm.org

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/


#ifndef CFORMSTRING_H
#define CFORMSTRING_H

#include <qstring.h>

/**
This class provides functions to encode and decode strings according to the"x-www-form-urlencoded"  specification.
Several functions included in this class are based on Special Edition Using CGI, by Jeffry Dwight and Michael Erwin,
with Tobin Anthony, Danny Brands, Rod Clark, Mike Ellsworth, David Geller, Galen A. Grimes, Matthew D. Healy,
Greg Knauss, Robert Niles, Bill Schongar, Crispen A. Scott, K. Mitchell Thompson, and Matt Wright.
See http://docs.rinet.ru:8083/CGI_Programming/ch22.htm.

@short A class for form-encoding and unencoding strings.
@author Aaron Reeves (areeves@lamar.colostate.edu or aaron.reeves@naadsm.org)
@version 1.0
*/
class CFormString : public QString  {
  public:
    /** Creates a new CFormString from a standard QString. */
    CFormString( QString str, bool isEncoded = false );
    
    /** Create an empty CQFormString. */
    CFormString( bool isEncoded = false  );
    
    /** Creates a new CFormString from a char*. */
    CFormString( const char* charString, bool isEncoded = false );
    
    //virtual ~CFormString();
    
    
    /** Encodes this string according to'x-www-form-urlencoded' rules. */
    void urlEncode( bool encodeSpaces = false );
    
    /** Decodes this string according to'x-www-form-urlencoded' rules. */
    void urlDecode( void );
    
    /** Indicates whether this string is in its encoded state (true) or its decoded state (false). */
    inline bool isEncoded( void ) { return isEncoded_p; };
    
    /** Indicates whether this string is in its encoded state (false) or its decoded state (true). */
    inline bool isDecoded( void ) { return !isEncoded_p; };
    
  protected:
    /** Replaces every instance of a single character with a different character.  
    Used to swap spaces and plus signs. */
    void swapChar( char cBad, char cGood);
    
    /** Used to unescape escaped characters.  Parameters are the high and low bytes of
    the hex encoding to unencode.  For the hex value 0x21, for example, hc is 2 and lc is 1. */
    int intFromHex(char hc, char lc ) ;
    
    /** Returns the hex encoding for the specified char. */
    QString hexFromInt( char c );
    
    /** Property to store encoding state. */
    bool isEncoded_p;
};

#endif //CQFORMSTRING_H


