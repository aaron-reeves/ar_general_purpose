
#include <ar_general_purpose/qcout.h>
#include <ar_general_purpose/strutils.h>


#include "help.h"


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


void printHelpList( CHelpItemList list ) {
  int i, j;
  int maxPart1Len = 0;
  int nPadding;
  QStringList lines;
  QString line0;

  for( i = 0; i < list.count(); ++i )
    maxPart1Len = std::max( maxPart1Len, list[i].part1().length() );

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
