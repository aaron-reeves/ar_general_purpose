/*
help.h/cpp
----------
Begin: 2014/04/25
Author: Aaron Reeves <development@reevesdigital.com>
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
  printHelpList( list );
}


CHelpItem::CHelpItem() {
  _part1 = QString();
  _part2 = QString();
}


CHelpItem::CHelpItem( QString part1, QString part2 ) {
  _part1 = part1;
  _part2 = part2;
}


CHelpItem::CHelpItem( const char* part1, const char* part2 ) {
  _part1 = QString( part1 );
  _part2 = QString( part2 );
}


CHelpItemList::CHelpItemList() : QList<CHelpItem>() {
  // Nothing else to do here
}


void CHelpItemList::append( const char* part1, const char* part2 ) {
  QList<CHelpItem>::append( CHelpItem( part1, part2 ) );
}


void CHelpItemList::append( const QString& part1, const QString& part2 ) {
  QList<CHelpItem>::append( CHelpItem( part1, part2 ) );
}


void CHelpItemList::append( const CHelpItemList& otherList ) {
  int i;
  CHelpItem hi;

  for( i = 0; i < otherList.count(); ++i ) {
    hi = otherList.at(i);
    this->append( hi.part1(), hi.part2() );
  }
}


void printHelpList( CHelpItemList list ) {
  int i, j;
  int maxPart1Len = 0;
  int nPadding;
  QStringList lines;
  QString line0;

  // Ignore any first part with a length over 20 characters,
  // since such items will span both columns anyway.
  for( i = 0; i < list.count(); ++i ) {
    if( 20 > list[i].part1().length() )
      maxPart1Len = std::max( maxPart1Len, list[i].part1().length() );
  }

  if( 20 < maxPart1Len )
    maxPart1Len = 20;

  nPadding = maxPart1Len + 1;

  for( i = 0; i < list.count(); ++i ) {
    if( 0 == list[i].part1().length() )
      cout << prettyPrint( list[i].part2() );
    else {
      lines = prettyPrintedList( list[i].part2(), 50, true, true, nPadding );
      if( list[i].part1().length() > maxPart1Len ) {
        cout << list[i].part1() << endl;
        for( j = 0; j < lines.count(); ++j )
          cout << lines.at(j) << endl;
      }
      else {
        line0 = lines.at(0).right( lines.at(0).length() - (list[i].part1().length() + 1) );
        cout << list[i].part1() << " " << line0 << endl;
        for( j = 1; j < lines.count(); ++j )
          cout << lines.at(j) << endl;
      }
    }
  }

  cout << endl << flush;
}
