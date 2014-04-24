#ifndef HELP_H
#define HELP_H

#include <QtCore>

class CHelpItem {
  public:
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
};

void printHelpList( CHelpItemList list );

#endif // HELP_H
