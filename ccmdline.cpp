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
#include <ar_general_purpose/help.h>

CCmdLine::CCmdLine() {
  _originalString = QString();
}


CCmdLine::CCmdLine( int argc, char** argv, bool clearArgs /* = true */ ) {
  _originalString = QString();
  splitLine( argc, argv, clearArgs );
}

    
CCmdLine::CCmdLine( const QString& fileName ) {
  _originalString = QString();
  splitFile( fileName );
}


void CCmdLine::assign( const CCmdLine& other ) {
  this->_originalString = other._originalString;
  this->_arguments = other._arguments;
}


QString CCmdLine::formatArg( QString arg ) const {
  // Strip off any dashes that might be present...
  while( arg.startsWith( '-' ) ) {
    arg = arg.right( arg.length() - 1 );
  }

  // ...then add back the right number.
  if( 1 == arg.length() )
    arg.prepend( '-' );
  else if( 1 < arg.length() )
    arg.prepend( "--" );

  return arg;
}


int CCmdLine::processList( QStringList list ) {
  int i;
  char** argv;
  char* arg;
  unsigned int size;

  list.prepend( QStringLiteral("dummy") );
  
  // Construct a char** from the string list
  argv = new char*[list.count()];
  for( i = 0; i < list.count(); ++i ) {
    size = strlen( static_cast<char*>(list.at(i).toLatin1().data()) ) + 1;
    arg = new char[ size ];
    strcpy( arg, static_cast<char*>(list.at(i).toLatin1().data()) );
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
  if( file.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
    QTextStream in( &file );
    while( !in.atEnd() ) {
      s = in.readLine();
      s = s.trimmed();
      if( ( 0 < s.length() ) && ( !( isComment( s ) ) ) ) {
        list.append( s );
      }
    }
  }
  else {
    qDb() << "Cannot open specified file.";
  }
  
  return processList( list ); 
}


/*------------------------------------------------------
  int CCmdLine::SplitLine(int argc, char **argv)

  parse the command line into switches and arguments

  returns number of switches found
------------------------------------------------------*/
int CCmdLine::splitLine( const int argc, char** argv, bool clearArgs /* = true */ ) {
	QString arg;
  QString curParam; // current argv[x]
	int i;

   if( clearArgs ) {
    _arguments.clear();
    _originalString = QString();
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
  for( i = 1; i < argc; ++i ) {
     // if it's a switch, start a new CCmdLine
     if( isSwitch( argv[i] ) ) {
        curParam = formatArg( argv[i] );

         // Clear args between switches to prevent problems
         arg = QString();

        // look at next input string to see if it's a switch or an argument
        if (i + 1 < argc) {
           if (!isSwitch(argv[i + 1])) {
              // it's an argument, not a switch
              arg = argv[i + 1];

              // skip to next
              ++i;
           }
           else {
              arg = QString();
           }
        }

        // add it
        QStringList cmd;

        // only add non-empty args
        if (!arg.isEmpty()) {
           cmd.append(arg);
        }

        // add the CCmdParam to 'this'
        _arguments.insert( curParam, cmd );
     }
     else {
        // it's not a new switch, so it must be more stuff for the last switch.
        // ...let's add it.
        _arguments[ curParam ].append( argv[i] );
     }
  }

  return _arguments.count();
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
bool CCmdLine::hasSwitch( const QString& pSwitch ) const {
  return _arguments.contains( formatArg( pSwitch ) );
}


bool CCmdLine::hasSwitch( const QStringList& switches ) const {
  bool result = false;

  for( int i = 0; i < switches.count(); ++i ) {
    if( hasSwitch( switches.at(i) ) ) {
      result = true;
      break;
    }
  }

  return result;
}


bool CCmdLine::isAmbiguous( const QStringList& pSwitches ) const {
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
QString CCmdLine::safeArgument( const QString& pSwitch, int iIdx, const QString& pDefault ) const {
  QString sRet;

  if( !pDefault.isEmpty() ) {
    sRet = pDefault;
  }

  if( hasSwitch( pSwitch ) ) {
    try {
      sRet = argument(pSwitch, iIdx);
    }
      catch (...) {
    }
  }

  return sRet;
}


QString CCmdLine::safeArgument( const QString& pSwitch, const QString& name, const QString& pDefault ) const {
  QString sRet;

  if( !pDefault.isEmpty() ) {
    sRet = pDefault;
  }

  if( hasSwitch( pSwitch ) ) {
    try {
      sRet = argument( pSwitch, name );
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
QString CCmdLine::argument( const QString& pSwitch, const int iIdx ) const {
  QString arg = formatArg( pSwitch );

  if( this->hasSwitch( arg ) && ( iIdx < this->argumentCount( arg ) ) )
    return _arguments.value( arg ).at( iIdx );

   throw int(0);
}


QString CCmdLine::argument( const QString& pSwitch, const QString& name ) const {
  QString arg = formatArg( pSwitch );

  if( this->hasSwitch( arg ) ) {
    for( int i = 0; i < this->argumentCount( arg ); ++i ) {
      if( this->arguments( arg ).at(i).startsWith( name, Qt::CaseInsensitive ) ) {
        QStringList list = this->arguments( arg ).at(i).split( '=' );
        if( 2 == list.count() ) {
          return list.at(1);
        }
      }
    }

    throw int(0);
  }

  throw int(0);
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
int CCmdLine::argumentCount( const QString& pSwitch ) const {
  QString arg = formatArg( pSwitch );

  if( hasSwitch( arg ) )
    return _arguments.value( arg ).count();
  else
    return -1;
}

int CCmdLine::argumentCount( const QStringList& pSwitches ) const {
  int result = -1;

  for( int i = 0; i < pSwitches.count(); ++i ) {
    if( hasSwitch( pSwitches.at(i) ) ) {
      result = argumentCount( pSwitches.at(i) );
      break;
    }
  }

  return result;
}


QStringList CCmdLine::arguments( const QString& pSwitch ) const {
  QString arg = formatArg( pSwitch );

  QStringList list;

  if( this->hasSwitch( arg ) )
    list = _arguments.value( arg );

  return list;
}


/* Returns true if any of the following switches is present: -h, --help, -? */
bool  CCmdLine::hasHelp() const {
  return(
    this->hasSwitch( QStringLiteral("-h") )
    || this->hasSwitch( QStringLiteral("--help") )
    || this->hasSwitch( QStringLiteral("-?") )
  );
}

/* Returns true if any of the following switches is present: -v, --version */
bool  CCmdLine::hasVersion() const {
  return(
    this->hasSwitch( QStringLiteral("-v") )
    || this->hasSwitch( QStringLiteral("--version") )
  );
}


QString CCmdLine::asString() const {
  return this->_originalString;
}


QString CCmdLine::asString( const QString& pSwitch ) const {
  QString result;
  QStringList list;

  QString arg = formatArg( pSwitch );

  if( this->hasSwitch( arg ) ) {
    for( int i = 0; i < this->argumentCount( arg ); ++i ) {
      list.append( this->argument( arg, i ) );
    }

    result = list.join( QStringLiteral(" ") );
  }

  return result;
}


void CCmdLine::debug() const {
  QHashIterator<QString, QStringList> it( _arguments );

  while( it.hasNext() ) {
    it.next();
    qDb() << "Key " << it.key() << " values " << it.value().join( QStringLiteral(", ") );
  }
}


bool CCmdLine::pair( const QString& str1, const QString& str2 ) {
  if( this->hasSwitch( str1 ) && this->hasSwitch( str2 ) )
    return false;
  else {
    if( this->hasSwitch( str1 ) )
      _arguments.insert( formatArg( str2 ), _arguments.value( formatArg( str1 ) ) );
    else if( this->hasSwitch( str2 ) )
      _arguments.insert( formatArg( str1 ), _arguments.value( formatArg( str2 ) ) );

    return true;
  }
}


void CCmdLineWithQOptions::initialize() {
  _helpFn = nullptr;
  _versionFn = nullptr;
  _hasHelpOption = false;
  _hasVersionOption = false;
}


void CCmdLineWithQOptions::assign( const CCmdLineWithQOptions& other ) {
  CCmdLine::assign( other );

  _optionList = other._optionList;

  _helpFn = other._helpFn;
  _versionFn = other._versionFn;

  _hasHelpOption = other._hasHelpOption;
  _hasVersionOption = other._hasVersionOption;

  _acceptedArgNames = other._acceptedArgNames;
}


void CCmdLineWithQOptions::addHelpOption( DisplayMessageFn helpFn /* = nullptr */ ) {
  _helpFn = helpFn;
  _hasHelpOption = true;
  addOption(
    QARCommandLineOption(
      QStringList() << QStringLiteral("--help") << QStringLiteral("-h") << QStringLiteral("-?"),
      QStringLiteral("Display this help message.")
    )
  );
}


void CCmdLineWithQOptions::addVersionOption( DisplayMessageFn versionFn /* = nullptr */ ) {
  _versionFn = versionFn;
  _hasVersionOption = true;
  addOption(
    QARCommandLineOption(
      QStringList() << QStringLiteral("--version") << QStringLiteral("-v"),
      QStringLiteral("Display the application version.")
    )
  );
}


void CCmdLineWithQOptions::addOption( const QARCommandLineOption& opt ) {
  foreach( const QString& argName, opt.names() ) {
    _acceptedArgNames.insert( formatArg( argName ) );
  }

  _optionList.append( opt );
}


void CCmdLineWithQOptions::showVersion() const {
  if( nullptr != _versionFn ) {
    _versionFn();
    return;
  }

  cout << QCoreApplication::applicationName() << " " << QCoreApplication::applicationVersion() << endl;
}


void CCmdLineWithQOptions::showHelp() const {
  if( nullptr != _helpFn ) {
    _helpFn();
    return;
  }

  showVersion();

  CHelpItemList hList;

  for( int i = 0; i < _optionList.count(); ++i ) {
    QARCommandLineOption opt = _optionList.at(i);

    QStringList optNames;
    QString optNamesStr;
    for( int j = 0; j < opt.names().count(); ++j ) {
      QString optName =  opt.names().at(j);

      // I don't think this block is necessary any more.
      // FIXME: delete this the next time through.
      //if( 1 == optName.length() ) {
      //  optName.prepend( '-' );
      //}
      //else {
      //  optName.prepend( "--" );
      //}

      QStringList valueNames;
      QString valueNamesStr;
      for( int k = 0; k < opt.valueNames().count(); ++k ) {
        if( !opt.valueNames().at(k).trimmed().isEmpty() ) {
          valueNames.append( QStringLiteral( "<%1>" ).arg( opt.valueNames().at(k).trimmed() ) );
        }
      }
      valueNamesStr = valueNames.join( ' ' );
      if( !valueNamesStr.isEmpty() ) {
        optNames.append( QStringLiteral( "%1 %2" ).arg( optName, valueNamesStr ) );
      }
      else {
        optNames.append( optName );
      }
    }
    optNamesStr = optNames.join( QStringLiteral( ", ") );

    hList.append( optNamesStr, opt.description() );
  }

  hList.printHelpList();
}


void CCmdLineWithQOptions::generatePairs() {
  foreach( const QARCommandLineOption& opt, _optionList ) {
    QStringList args = opt.names();

    QList<QString>::const_iterator str1;
    QList<QString>::const_iterator str2;
    for( str1 = args.constBegin(); str1 != args.constEnd(); ++str1 ) {
      for( str2 = args.constBegin(); str2 != args.constEnd(); ++str2 ) {
        pair( *str1, *str2 );
      }
    }
  }
}


bool CCmdLineWithQOptions::process( const QCoreApplication& app ) {
  QStringList args = app.arguments();

  int argc = args.count();

  char** argv = new char* [ argc ];

  for( int i = 0; i < argc; ++i ) {
    argv[i] = new char[args.at(i).length()+1];
    strcpy( argv[i], args.at(i).toLatin1().data() );
  }

  splitLine( argc, argv, false );

  //this->debug();

  delete [] argv;

  bool argsOK = true;
  QStringList providedArgNames = _arguments.keys();

  // Check for any unrecognized arguments.  If found, return false.
  foreach( const QString& arg, providedArgNames ) {
    if( !_acceptedArgNames.contains( formatArg( arg ) ) ) {
      cout << "Unrecognized argument: " << formatArg( arg ) << endl;
      argsOK = false;
    }
  }

  // Check for any duplications.  If found, return false.
  int nMatches;
  foreach( const QARCommandLineOption& opt, _optionList ) {
    QStringList args = opt.names();

    nMatches = 0;
    foreach( const QString& arg, args ) {
      if( providedArgNames.contains( arg ) ) {
        ++nMatches;
      }
    }
    if( 1 < nMatches ) {
      cout << "Ambiguous or unnecessary argument duplication: " << opt.names().at(0) << endl;
      argsOK = false;
    }
  }

  if( !argsOK ) {
    return false;
  }
  else if( _hasHelpOption && hasHelp() ) {
    showHelp();
    exit( 0 );
  }
  else if( _hasVersionOption && hasVersion() ) {
    showVersion();
    exit( 0 );
  }

  // Generate pairs so that the application recognizes, e.g., "--version" and "-v" as the same thing.
  generatePairs();

  return true;
}

