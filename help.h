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

#ifndef HELP_H
#define HELP_H

#include <QtCore>


/* A simple class and related functions for displaying nicely formatted messages to the console.
 *
 * See function showSampleHelpMessage() (reproduced below) for an example.
 *
 * void showSampleHelpMessage() {
 *   CHelpItemList list;
 *
 *   // Leave the first part empty to display a line that starts on the left and fills both columns.
 *   list.append( "", "Command line arguments:" );
 *
 *   // The first part will be displayed in the first column.
 *   // The second part will be displayed in the second column, and will be divided and wrapped as needed.
 *   // Note the manual indentation and punctuation in the first part.  This could be more automated,
 *   // but at the expense of having less flexibility.
 *   list.append( "  --help, -h, -?:", "Display this help message." );
 *   list.append( "  --version, -v:", "Display version information." );
 *
 *   // Unusually long first parts will span both columns, with the second part beginning on the
 *   // next line in the second column.
 *   list.append( "  --database <filename>, -d <filename>:", "If --database is the only switch provided, the specified database will be opened for interactive use." );
 *
 *   // Show a blank line:
 *   list.append( "", "" );
 *
 *   // Start over again in a new block:
 *   list.append( "", "Interactive mode commands:" );
 *   list.append( "  show tables:", "Displays a list of tables in the database, excluding Microsoft Access system tables." );
 *   list.append( "  describe <tablename>:", "Describes all tables, excluding system tables." );
 *
 *   // Another new block, with a bulleted list (note the hyphens):
 *   list.append( "", "Simpsons characters:" );
 *   list.append( "  Adults:", "These are the grownups." );
 *   list.append( "  ", "- Homer" );
 *   list.append( "  ", "- Marge" );
 *   list.append( "  Kids:", "These are the Kids." );
 *   list.append( "  ", "- Bart" );
 *   list.append( "  ", "- Lisa" );
 *   list.append( "  ", "- Maggie" );
 *
 *   // Display the message.
 *   printHelpList( list );
 * }
 *
 */
class CHelpItem {
  public:
    CHelpItem();
    CHelpItem( QString part1, QString part2 );
    CHelpItem( const char* part1, const char* part2 );

    QString part1() { return _part1; }
    QString part2() { return _part2; }

  protected:
    QString _part1;
    QString _part2;
};


class CHelpItemList : public QList<CHelpItem> {
  public:
    CHelpItemList();
    void append( const char* part1, const char* part2 );
    void append( const QString& part1, const QString& part2 );
    void append( const CHelpItemList& otherList );
};

void printHelpList( CHelpItemList list, const int extraPadding = 0 );

void showSampleHelpMessage();

#endif // HELP_H
