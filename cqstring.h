/*
cqstring.h/cpp
--------------
Begin: 2003/04/09
Author: Aaron Reeves <aaron.reeves@naadsm.org>
--------------------------------------------------
Copyright (C) 2003 - 2007 Aaron Reeves

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

As a special exception, the copyright holder gives permission to link this program with Qt non-commercial
edition, and distribute the resulting executable, without distributing the source code for the Qt
non-commercial edition in the source distribution.
*/

#ifndef CQSTRING_H
#define CQSTRING_H

#include <qstring.h>

#ifdef UNDEFINED
#ifdef _WIN32 // MFC
  #include <afx.h>  // for CStrings
#endif
#endif

/**
This class provides adds some new functionality to typical QStrings.  The functions of this class are
pretty simplistic, but they get the job done...

@short A class that provides QStrings with some new functionality.
*/
class CQString:public QString {
  public:
    /**
    Converts a typical QString to a CQString

    @param str QString to convert to a CQString
    */
    CQString( QString str );

    /**
    This version of the constructor creates an empty CQString
    */
    CQString( void );

     /**
     Converts a typical char* to a CQString

     @param str char* to convert to a CQString
    */
    CQString( const char *charString );

#ifdef UNDEFINED
    #ifdef _WIN32 //MFC
      /**
      Converts a Windows CString to a CQString

      @param str CString to convert to a CQString
      */
      CQString( CString cstr );

      /**
      Converts a CQString to a Windows CString

      @return CString value
      */
      CString toCString( void );
    #endif
#endif

    /**
    Equivalent to 'this', but added for convenience.

    @return a CQString value
    */
    CQString value( void );

    /**
    Equivalent to 'this' or value(), but added for convenience.

    @return a CQString value
    */
    CQString toString( void );

    /**
    A simplistic function for extracting a POSITIVE integer from the CQString.  For example, the number 7 could be extracted
    from the CQStrings "Number=7" or "The total is 7".  This function returns the number that immediately follows a
    designated character or substring (the "splitter").  In the first example, a splitter of "=" can be used.  In the second, the
    splitter needs to be "is ". (Note the space in this second splitter: it may be important).

    WARNING: this function shouldn't be used for sophisticated parsing, but can be used to extract information where
    the string pattern is very predictable (as in output from another program).

    WARNING: this function will return only one number: more sophisticated steps will be needed to extract multiple
    numbers from a single string.

    FIX ME: what was the rational for extracting only positive integers?

    @param splitter The character or substring to use to separate the number component from the rest of the string
    @return The positive integer value (a Q_UINT64, or unsigned 64-bit integer) extracted from the string
    @see CQString#getDouble
    */
    int getInt( QString splitter );


    /**
    A simplistic function for extracting a double (a floating point number) from the CQString.  For example, the number 7.45 could be extracted
    from the CQStrings "Number=7.45" or "The total is 7.45".  This function returns the number that immediately follows a
    designated character or substring (the "splitter").  In the first example, a splitter of "=" can be used.  In the second, the
    splitter needs to be "is ". (Note the space in this second splitter: it may be important).

    WARNING: this function shouldn't be used for sophisticated parsing, but can be used to extract information where
    the string pattern is very predictable (as in output from another program).

    WARNING: this function will return only one number: more sophisticated steps will be needed to extract multiple
    numbers from a single string.

    @param splitter The character or substring to use to separate the number component from the rest of the string
    @return The floating point value (a double) extracted from the string
     @see CQString#getInt
    */
    double getDouble( QString splitter );
};

#endif //CQSTRING_H
