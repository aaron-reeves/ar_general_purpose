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

#include "ccmdline.h"

#include <ctype.h>

#include <qdebug.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qstringlist.h>

#include <ar_general_purpose/qcout.h>
#include <ar_general_purpose/strutils.h>

CCmdLine::CCmdLine() {
  _originalString = "";
}


CCmdLine::CCmdLine( int argc, char** argv, bool clearArgs ) {
  _originalString = "";
  splitLine( argc, argv, clearArgs );
}

    
CCmdLine::CCmdLine( const QString& fileName ) {
  _originalString = "";
  splitFile( fileName );
}


int CCmdLine::processList( QStringList list ) {
  int i;
  char** argv;
  char* arg;
  int size;

  list.prepend( "dummy" );
  
  // Construct a char** from the string list
  argv = new char*[list.count()];
  for( i = 0; i < list.count(); ++i ) {
    size = strlen( (char *)(list.at(i).toLatin1().data()) ) + 1;
    arg = new char[ size ];
    strcpy( arg, (char*)(list.at(i).toLatin1().data()) );
    argv[i] = arg;
  }
  
  // Parse it
  return splitLine( list.count(), argv, false );
}


int CCmdLine::splitString( QString str ) {
  QStringList list, list2;
  QString s;
  int i;
  
  // Split the string into pieces
  str = str.trimmed(); 
  list = str.split( QRegExp("\\s+"), QString::SkipEmptyParts );
  
  for( i = 0; i < list.count(); ++i ) {
    s = list.at(i).trimmed();
    if( ( 0 < s.length() ) && ( !( isComment( s ) ) ) ) {
      list2.append( s );
    }  
  }
  
  return processList( list2 );
}



int CCmdLine::splitFile( const QString& fileName ) {
  QStringList list;
  QString s;
  
  // Read the contents of the file into a string
  QFile file( fileName );
  if( !file.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
    qDebug() << "Cannot open specified file.";
  }
  else {
    QTextStream in( &file );
    while( !in.atEnd() ) {
      s = in.readLine();
      s = s.trimmed();
      if( ( 0 < s.length() ) && ( !( isComment( s ) ) ) ) {
        list.append( s );
      }
    }
  }
  
  return processList( list ); 
}


/*------------------------------------------------------
  int CCmdLine::SplitLine(int argc, char **argv)

  parse the command line into switches and arguments

  returns number of switches found
------------------------------------------------------*/
int CCmdLine::splitLine( int argc, char** argv, bool clearArgs ) {
	QString arg;
  QString curParam; // current argv[x]
	int i;

   if( clearArgs ) {
    _hash.clear();
    _originalString = "";
  }

  // Build the original string, which might be requested later.
  //-----------------------------------------------------------
  // skip the exe name (start with i = 1)
  for( i = 1; i < argc; ++i ){
    if( !_originalString.isEmpty() )
      _originalString = _originalString.append( " " );

    _originalString = _originalString.append( argv[i] );
  }

  // Process the arguments.
  //-----------------------
  // skip the exe name (start with i = 1)
  for (i = 1; i < argc; i++) {
     // if it's a switch, start a new CCmdLine
     if (isSwitch(argv[i])) {
        curParam = argv[i];

         // Clear args between switches to prevent problems
         arg = "";

        // look at next input string to see if it's a switch or an argument
        if (i + 1 < argc) {
           if (!isSwitch(argv[i + 1])) {
              // it's an argument, not a switch
              arg = argv[i + 1];

              // skip to next
              i++;
           }
           else {
              arg = "";
           }
        }

        // add it
        QStringList cmd;

        // only add non-empty args
        if (arg != "") {
           cmd.append(arg);
        }

        // add the CCmdParam to 'this'
        _hash.insert(curParam, cmd);
     }
     else {
        // it's not a new switch, so it must be more stuff for the last switch.
        // ...let's add it.
        _hash[ curParam ].append( argv[i] );
     }
  }

  return _hash.count();
}


/*------------------------------------------------------
   protected member function
   test a parameter to see if it's a switch :

   switches are of the form : -x
   where 'x' is one or more characters.
   the first character of a switch must be non-numeric!
------------------------------------------------------*/
bool CCmdLine::isSwitch( const QString& pParam ) {
   if (pParam.isEmpty())
      return false;

   // switches must non-empty
   // must have at least one character after the '-'
   int len = pParam.length() ;
   if (len <= 1) {
      return false;
   }

   // switches always start with '-'
   if (pParam[0]=='-') {
      // allow negative numbers as arguments.
      // ie., don't count them as switches
      return( !pParam[1].isNumber() );
   }
   else {
      return false;
   }
}


/*------------------------------------------------------
   bool CCmdLine::HasSwitch(const QString& pSwitch)

   was the switch found on the command line ?

   ex. if the command line is : app.exe -a p1 p2 p3 -b p4 -c -d p5

   call                          return
   ----                          ------
   cmdLine.HasSwitch("-a")       true
   cmdLine.HasSwitch("-z")       false
------------------------------------------------------*/
bool CCmdLine::hasSwitch( const QString& pSwitch ) {
  return _hash.contains( pSwitch );
}


bool CCmdLine::hasSwitch( const QStringList& switches ) {
  bool result = false;

  for( int i = 0; i < switches.count(); ++i ) {
    if( hasSwitch( switches.at(i) ) ) {
      result = true;
      break;
    }
  }

  return result;
}


bool CCmdLine::isAmbiguous( const QStringList& pSwitches ) {
  int n = 0;

  for( int i = 0; i < pSwitches.count(); ++i ) {
    if( hasSwitch( pSwitches.at(i) ) ) {
      ++n;
    }
  }

  return ( n > 1 );
}


/*------------------------------------------------------
   QString CCmdLine::GetSafeArgument(const QString& pSwitch, int iIdx, const QString& pDefault)

   fetch an argument associated with a switch . if the parameter at
   index iIdx is not found, this will return the default that you
   provide.

   example :
  
   command line is : app.exe -a p1 p2 p3 -b p4 -c -d p5

   call                                      return
   ----                                      ------
   cmdLine.GetSafeArgument("-a", 0, "zz")    p1
   cmdLine.GetSafeArgument("-a", 1, "zz")    p2
   cmdLine.GetSafeArgument("-b", 0, "zz")    p4
   cmdLine.GetSafeArgument("-b", 1, "zz")    zz
------------------------------------------------------*/
QString CCmdLine::safeArgument( const QString& pSwitch, int iIdx, const QString& pDefault ) {
   QString sRet;
   
   if (!pDefault.isEmpty())
      sRet = pDefault;

  if( hasSwitch( pSwitch ) ) {
     try {
        sRet = argument(pSwitch, iIdx);
     }
     catch (...) {
     }
  }

   return sRet;
}


/*------------------------------------------------------
   QString CCmdLine::GetArgument(const QString& pSwitch, int iIdx)

   fetch a argument associated with a switch. throws an exception 
   of (int)0, if the parameter at index iIdx is not found.

   example :
  
   command line is : app.exe -a p1 p2 p3 -b p4 -c -d p5

   call                             return
   ----                             ------
   cmdLine.GetArgument("-a", 0)     p1
   cmdLine.GetArgument("-b", 1)     throws (int)0, returns an empty string
------------------------------------------------------*/
QString CCmdLine::argument( const QString& pSwitch, int iIdx ) {
  if( this->hasSwitch( pSwitch ) && ( iIdx < this->argumentCount( pSwitch ) ) )
    return _hash.value( pSwitch ).at( iIdx );

   throw (int)0;

   return "";
}


QString CCmdLine::argument( const QStringList& pSwitches, int iIdx ) {
  QString result;

  if( !isAmbiguous( pSwitches ) ) {
    for( int i = 0; i < pSwitches.count(); ++i ) {
      if( this->hasSwitch( pSwitches.at(i) ) ) {
        result = this->argument( pSwitches.at(i), iIdx );
        break;
      }
    }
  }

  return result;
}


/*------------------------------------------------------
   int CCmdLine::GetArgumentCount(const QString& pSwitch)

   returns the number of arguments found for a given switch.

   returns -1 if the switch was not found
------------------------------------------------------*/
int CCmdLine::argumentCount( const QString& pSwitch ) {
  if( hasSwitch( pSwitch ) )
    return _hash.value( pSwitch ).count();
  else
    return -1;
}

int CCmdLine::argumentCount( const QStringList& pSwitches ) {
  int result = -1;

  for( int i = 0; i < pSwitches.count(); ++i ) {
    if( hasSwitch( pSwitches.at(i) ) ) {
      result = argumentCount( pSwitches.at(i) );
      break;
    }
  }

  return result;
}


QStringList CCmdLine::arguments(  const QString& pSwitch ) {
  QStringList list;

  if( this->hasSwitch( pSwitch ) )
    list = _hash.value( pSwitch );

  return list;
}


/* Returns true if any of the following switches is present: -h, --help, -? */
bool  CCmdLine::hasHelp() {
  return(
    this->hasSwitch( "-h" )
    || this->hasSwitch( "--help" )
    || this->hasSwitch( "-?" )
  );
}

/* Returns true if any of the following switches is present: -v, --version */
bool  CCmdLine::hasVersion() {
  return(
    this->hasSwitch( "-v" )
    || this->hasSwitch( "--version" )
  );
}


QString CCmdLine::asString(){
  return this->_originalString;
}


void CCmdLine::debug( void ) {
  QHashIterator<QString, QStringList> it( _hash );

  while( it.hasNext() ) {
    it.next();
    qDebug() << "Key " << it.key() << " values " << it.value().join( ", " );
  }
}


bool CCmdLine::pair( const QString& str1, const QString& str2 ) {
  if( this->hasSwitch( str1 ) && this->hasSwitch( str2 ) )
    return false;
  else {
    if( this->hasSwitch( str1 ) )
      _hash.insert( str2, _hash.value( str1 ) );
    else if( this->hasSwitch( str2 ) )
      _hash.insert( str1, _hash.value( str2 ) );

    return true;
  }
}

