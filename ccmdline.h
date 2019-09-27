/*
ccmdine.h/cpp
-------------
Begin: 2007/02/17
Author (for modifications): Aaron Reeves <aaron.reeves@naadsm.org>
-------------------------------------------------------------------------

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

/* This code has been modified from the source listed below.  As per the
   terms of the original licence agreement, the original information is below:

  ------------------------------------------------------
   CCmdLine

   A utility for parsing command lines.

   Copyright (C) 1999 Chris Losinger, Smaller Animals Software.
   http://www.smalleranimals.com

   This software is provided 'as-is', without any express
   or implied warranty.  In no event will the authors be
   held liable for any damages arising from the use of this software.

   Permission is granted to anyone to use this software
   for any purpose, including commercial applications, and
   to alter it and redistribute it freely, subject to the
   following restrictions:

     1. The origin of this software must not be misrepresented;
   you must not claim that you wrote the original software.
   If you use this software in a product, an acknowledgment
   in the product documentation would be appreciated but is not required.

     2. Altered source versions must be plainly marked as such,
   and must not be misrepresented as being the original software.

     3. This notice may not be removed or altered from any source
   distribution.
  ------------------------------------------------------
*/

/*
   Example :

   Our example application uses a command line that has two
   required switches and two optional switches. The app should abort
   if the required switches are not present and continue with default
   values if the optional switches are not present.

      Sample command line :
      MyApp.exe -p1 text1 text2 -p2 "this is a big argument" -opt1 -55 -opt2

      Switches -p1 and -p2 are required.
      p1 has two arguments and p2 has one.

      Switches -opt1 and -opt2 are optional.
      opt1 requires a numeric argument.
      opt2 has no arguments.

      Also, assume that the app displays a 'help' screen if the '-h' switch
      is present on the command line.

   #include "ccmdline.h"

   void main(int argc, char **argv)
   {
      // our cmd line parser object
      CCmdLine cmdLine;

      // parse argc,argv
      if (cmdLine.splitLine(argc, argv) < 1) {
         // no switches were given on the command line, abort
         ASSERT(0);
         exit(-1);
      }

      // test for the 'help' case
      if (cmdLine.hasSwitch("-h")) {
         show_help();
         exit(0);
      }

      // get the required arguments
      QString p1_1, p1_2, p2_1;
      try {
         // if any of these fail, we'll end up in the catch() block
         p1_1 = cmdLine.getArgument("-p1", 0);
         p1_2 = cmdLine.getArgument("-p1", 1);
         p2_1 = cmdLine.getArgument("-p2", 0);

      }
      catch (...) {
         // one of the required arguments was missing, abort
         ASSERT(0);
         exit(-1);
      }

      // get the optional parameters

      // convert to an int, default to '100'
      int iOpt1Val =    atoi(cmdLine.getSafeArgument("-opt1", 0, 100));

      // since opt2 has no arguments, just test for the presence of
      // the '-opt2' switch
      bool bOptVal2 =   cmdLine.hasSwitch("-opt2");

      .... and so on....

   }

   If this is an MFC app, you can use the __argc and __argv macros from
   you CYourWinApp::InitInstance() function in place of the standard argc
   and argv variables.
*/

#ifndef CCMDLINE_H_
#define CCMDLINE_H_

#include <qstring.h>
#include <qstringlist.h>
#include <qhash.h>

// the command line parser class
class CCmdLine {
  public:
    CCmdLine();
    CCmdLine( int argc, char** argv, bool clearArgs = true );
    CCmdLine( const QString& fileName ); 
    CCmdLine( const CCmdLine& other ) { assign( other ); }
    CCmdLine& operator=( const CCmdLine& other ) { assign( other ); return *this; }
    ~CCmdLine() { /* Nothing to do here */ }
  
    /* Parses the command line into switches and arguments. Returns number of switches found. */
    int splitLine( const int argc, char** argv, bool clearArgs = true );
    
    int splitString( QString str );
    
    int splitFile( const QString& fileName );
    
    int processList( QStringList list );
    
    /* How many switches are there? */
    int switchCount() const { return _arguments.count(); }
    bool hasSwitches() const { return 0 < _arguments.count(); }

    /* Was the switch found on the command line? */
    bool hasSwitch( const char* pSwitch ) const { return hasSwitch( QString( pSwitch ) ); }
    bool hasSwitch( const QString& pSwitch ) const;
    bool hasSwitch( const QStringList& pSwitches ) const;
    bool isSet( const QString& pSwitch) const { return hasSwitch( pSwitch ); }
    bool isSet( const char* pSwitch ) const { return hasSwitch( QString( pSwitch ) ); }

    bool isAmbiguous( const QStringList& pSwitches ) const;
    
    /* Fetch an argument associated with a switch. Return the default if not found. */
    QString safeArgument( const char* pSwitch, const int iIdx, const char* pDefault ) const { return safeArgument( QString( pSwitch ), iIdx, QString( pDefault ) ); }
    QString safeArgument( const char* pSwitch, const int iIdx, const QString& pDefault ) const { return safeArgument( QString( pSwitch ), iIdx, pDefault ); }
    QString safeArgument( const QString& pSwitch, const int iIdx, const QString& pDefault ) const;
    QString safeArgument( const char* pSwitch, const  char* name, const char* pDefault ) const { return safeArgument( QString( pSwitch ), QString( name ), QString( pDefault ) ); }
    QString safeArgument( const char* pSwitch, const  char* name, const QString& pDefault ) const { return safeArgument( QString( pSwitch ), QString( name ), pDefault ); }
    QString safeArgument( const QString& pSwitch, const QString& name, const QString& pDefault ) const;
    
    /* Fetch an argument associated with a switch. Throw an exception if not found. */
    QString argument( const char* pSwitch, const int iIdx) const { return argument( QString( pSwitch ), iIdx ); }
    QString argument( const QString& pSwitch, const int iIdx) const;
    QString argument( const char* pSwitch, const char* name ) const { return argument( QString( pSwitch ), QString( name ) ); }
    QString argument( const QString& pSwitch, const QString& name ) const;

    QString value( const char* pSwitch ) const { return argument( QString( pSwitch ), 0 ); }
    QString value( const char* pSwitch, const int iIdx ) const { return argument( QString( pSwitch ), iIdx ); }

    QString value( const QString& pSwitch ) const { return argument( pSwitch, 0 ); }
    QString value( const QString& pSwitch, const int iIdx ) const { return argument( pSwitch, iIdx ); }

    QString value( const QString& pSwitch, const int iIdx, const QString& defaultVal ) { return safeArgument( pSwitch, iIdx, defaultVal ); }
    QString value( const char* pSwitch, const int iIdx, const char* defaultVal ) { return safeArgument( QString( pSwitch ), iIdx, QString( defaultVal ) ); }

    QString argument( const QStringList& pSwitches, int iIdx );
    
    /* Returns the number of arguments found for a given switch, or -1 if not found. */
    int argumentCount(const char* pSwitch ) const { return argumentCount( QString( pSwitch ) ); }
    int argumentCount(const QString& pSwitch ) const;
    int argumentCount( const QStringList& pSwitches ) const;

    bool pair( const char* str1, const char* str2 )  { return pair( QString( str1 ), QString( str2 ) ); }
    bool pair( const QString& str1, const QString& str2 );

    /* Returns true if any of the following switches is present: -h, --help, -? */
    bool hasHelp() const;

    /* Returns true if any of the following switches is present: -v, --version */
    bool hasVersion() const;

    /* Returns a list of arguments associated with a switch. */
    QStringList arguments( const char* pSwitch ) const { return arguments( QString( pSwitch ) ); }
    QStringList arguments( const QString& pSwitch ) const;

    /* Returns the original string that contained switches and arguments. Useful for logging. */
    QString asString() const;
    QString asString( const char* pSwitch ) const { return asString( QString( pSwitch ) ); }
    QString asString( const QString& pSwitch ) const;

    void debug() const;
    
    /* 
    Useful inherited functions:
    ---------------------------
    // Returns the number of switches.  From QHash.
    int count( void );
    
    // Clears all items.  From QHash.
    void clear( void );
    */
  protected:
    void assign( const CCmdLine& other );

    /* Test a parameter to see if it's a switch (form "-x"). */
    bool isSwitch( const QString& pParam );

    QString formatArg( QString arg ) const;

    QHash<QString, QStringList> _arguments;

    QString _originalString;
};





#endif //CCMDLINE_H_

