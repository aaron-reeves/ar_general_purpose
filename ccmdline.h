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
      if (cmdLine.SplitLine(argc, argv) < 1) {
         // no switches were given on the command line, abort
         ASSERT(0);
         exit(-1);
      }

      // test for the 'help' case
      if (cmdLine.HasSwitch("-h")) {
         show_help();
         exit(0);
      }

      // get the required arguments
      QString p1_1, p1_2, p2_1;
      try {
         // if any of these fail, we'll end up in the catch() block
         p1_1 = cmdLine.GetArgument("-p1", 0);
         p1_2 = cmdLine.GetArgument("-p1", 1);
         p2_1 = cmdLine.GetArgument("-p2", 0);

      }
      catch (...) {
         // one of the required arguments was missing, abort
         ASSERT(0);
         exit(-1);
      }

      // get the optional parameters

      // convert to an int, default to '100'
      int iOpt1Val =    atoi(cmdLine.GetSafeArgument("-opt1", 0, 100));

      // since opt2 has no arguments, just test for the presence of
      // the '-opt2' switch
      bool bOptVal2 =   cmdLine.HasSwitch("-opt2");

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
#include <qmap.h>


// handy little container for our argument vector
struct CCmdParam {
  QStringList m_strings;
};


// this class is actually a map of strings to vectors
typedef QMap<QString, CCmdParam> _CCmdLine;


// the command line parser class
class CCmdLine : public _CCmdLine {
  public:
    CCmdLine( int argc, char** argv, bool clearArgs = true );
    CCmdLine( const QString& fileName ); 
  
    /* Parses the command line into switches and arguments. Returns number of switches found. */
    int SplitLine( int argc, char** argv, bool clearArgs = true );
    int splitLine( int argc, char** argv, bool clearArgs = true ) { return SplitLine( argc, argv, clearArgs ); }
    
    int SplitString( QString str );
    int splitString( QString str ) { return SplitString( str ); }
    
    int SplitFile( const QString& fileName );
    int splitFile( const QString& fileName ) { return SplitFile( fileName ); }
    
    int ProcessList( QStringList list );
    int processList( QStringList list ) { return ProcessList( list ); }
    
    /* How many switches are there? */
    int GetSwitchCount() { return count(); }
    int getSwitchCount() { return count(); }
    bool HasSwitches() { return 0 < count(); }
    bool hasSwitches() { return HasSwitches(); }

    /* Was the switch found on the command line? */
    bool HasSwitch( const QString& pSwitch );
    bool hasSwitch( const QString& pSwitch ){ return HasSwitch( pSwitch ); }
    
    /* Fetch an argument associated with a switch. Return the default if not found. */
    QString GetSafeArgument( const QString& pSwitch, int iIdx, const QString& pDefault);
    QString getSafeArgument( const QString& pSwitch, int iIdx, const QString& pDefault) { return GetSafeArgument( pSwitch, iIdx, pDefault ); }
    
    /* Fetch an argument associated with a switch. Throw an exception if not found. */
    QString GetArgument( const QString& pSwitch, int iIdx);
    QString getArgument( const QString& pSwitch, int iIdx) { return GetArgument( pSwitch, iIdx ); }
    
    /* Returns the number of arguments found for a given switch, or -1 if not found. */
    int GetArgumentCount(const QString& pSwitch );
    int getArgumentCount(const QString& pSwitch ) { return GetArgumentCount( pSwitch ); }

    /* Returns true if any of the following switches is present: -h, --help, -? */
    bool hasHelp();

    /* Returns true if any of the following switches is present: -v, --version */
    bool hasVersion();

    /* Returns a list of arguments associated with a switch. */
    QStringList arguments( const QString& pSwitch );

    /* Returns the original string that contained switches and arguments. */
    QString asString();

    void debug( void );
    
    /* 
    Useful inherited functions:
    ---------------------------
    // Returns the number of switches.  From QMap.
    int count( void );
    
    // Clears all items.  From QMap.
    void clear( void );
    */
  protected:
    /* Test a parameter to see if it's a switch (form "-x"). */
    bool IsSwitch(const QString& pParam);

    QString _originalString;
};


#endif //CCMDLINE_H_

