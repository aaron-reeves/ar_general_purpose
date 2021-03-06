/*
help.h/cpp
----------
Begin: 2014/04/25
Author: Aaron Reeves <aaron.reeves@naadsm.org>
----------------------------------------------------
Copyright (C) 2014 Aaron Reeves

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#include <ar_general_purpose/qcout.h>
#include <ar_general_purpose/strutils.h>


#include "help.h"

void showSampleHelpMessage() {
  CHelpItemList list;

  // Leave the first part empty to display a line that starts on the left and fills both columns.
  list.append( "", "Command line arguments:" );

  // The first part will be displayed in the first column.
  // The second part will be displayed in the second column, and will be divided and wrapped as needed.
  // Note the manual indentation and punctuation in the first part.  This could be more automated,
  // but at the expense of having less flexibility.
  list.append( "  --help, -h, -?:", "Display this help message." );
  list.append( "  --version, -v:", "Display version information." );

  // Unusually long first parts will span both columns, with the second part beginning on the
  // next line in the second column.
  list.append( "  --database <filename>, -d <filename>:", "If --database is the only switch provided, the specified database will be opened for interactive use." );

  // Show a blank line:
  list.append( "", "" );

  // Start over again in a new block:
  list.append( "", "Interactive mode commands:" );
  list.append( "  show tables:", "Displays a list of tables in the database, excluding Microsoft Access system tables." );
  list.append( "  describe <tablename>:", "Describes all tables, excluding system tables." );

  // Display the message.
  list.printHelpList();
}


CHelpItem::CHelpItem() {
  // Nothing to do here
}


CHelpItem::CHelpItem( const QString& part1, const QString& part2 ) :
  _part1( part1 ),
  _part2( part2 )
{
  // Nothing else to do here
}


CHelpItem::CHelpItem( const char* part1, const char* part2 ) :
  _part1( part1 ),
  _part2( part2 )
{
  // Nothing else to do here
}


CHelpItemList::CHelpItemList() : QVector<CHelpItem>() {
  // Nothing else to do here
}


void CHelpItemList::append() {
  QVector<CHelpItem>::append( CHelpItem( "", "" ) );
}


void CHelpItemList::append( const char* part1, const char* part2 ) {
  QVector<CHelpItem>::append( CHelpItem( part1, part2 ) );
}


void CHelpItemList::append( const QString& part1, const QString& part2 ) {
  QVector<CHelpItem>::append( CHelpItem( part1, part2 ) );
}


void CHelpItemList::append( const CHelpItemList& otherList ) {
  int i;
  CHelpItem hi;

  for( i = 0; i < otherList.count(); ++i ) {
    hi = otherList.at(i);
    this->append( hi.part1(), hi.part2() );
  }
}


void CHelpItemList::printHelpList( const int breakAtColumn /* = 55 */, const int extraPadding /* = 0 */ ) const {
  int i, j;
  int maxPart1Len = 0;
  int nPadding;
  QStringList lines;
  QString line0;

  // Ignore any first part with a length over 20 characters,
  // since such items will span both columns anyway.
  for( i = 0; i < this->count(); ++i ) {
    if( 20 > this->at(i).part1().length() )
      maxPart1Len = std::max( maxPart1Len, this->at(i).part1().length() );
  }

  if( 20 < maxPart1Len )
    maxPart1Len = 20;

  nPadding = maxPart1Len + 1;

  for( i = 0; i < this->count(); ++i ) {
    if( 0 == this->at(i).part1().length() )
      cout << prettyPrint( this->at(i).part2(), 75 );
    else {

      // Make bulleted lists look cool.
      if( this->at(i).part2().startsWith( '-' ) ) {
        QStringList tmpLines = prettyPrintedList( this->at(i).part2(), (breakAtColumn - 2), false, true, nPadding + 2 );
        lines.clear();
        lines.append( tmpLines.at(0) );
        for( int k = 1; k < tmpLines.count(); ++k ) {
          lines.append( QStringLiteral( "  %1" ).arg( tmpLines.at(k) ) );
        }
      }
      else {
        lines = prettyPrintedList( this->at(i).part2(), breakAtColumn, false, true, nPadding );
      }

      if( this->at(i).part1().length() > maxPart1Len ) {
        cout << this->at(i).part1() << endl;
        for( j = 0; j < lines.count(); ++j ) {
          for( int k = 0; k < extraPadding; ++k )
            cout << " ";
          cout << lines.at(j) << endl;
        }
      }
      else {
        line0 = lines.at(0).right( lines.at(0).length() - (this->at(i).part1().length() + 1) );
        cout << this->at(i).part1() << " " << line0 << endl;
        for( j = 1; j < lines.count(); ++j )
          cout << lines.at(j) << endl;
      }
    }
  }

  cout << endl << flush;
}
