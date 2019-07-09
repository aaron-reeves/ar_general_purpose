/*
cqstringlist.h/cpp
------------------
Begin: 2003/04/09
Author: Aaron Reeves <aaron.reeves@naadsm.org>
--------------------------------------------------
Copyright (C) 2003 - 2008 Aaron Reeves

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/


#ifndef CQSTRINGLIST_H
#define CQSTRINGLIST_H

#include <qdebug.h>
#include <qlist.h>
#include <qstring.h>

/**
@brief A pointer-based linked list of instances of QString

This class provides an customized QList containing pointers to QStrings.  See Qt documentation for QList (QPtrList) for more details.

@short
@author Aaron Reeves (aaron.reeves@naadsm.org)
@version 040904
*/
class CQStringList : public QList<QString*>  {

    public:
      CQStringList();

      ~CQStringList();

      bool contains( const QString& str );

      void explode( const QString& str, QChar splitter );

      void debug();

      void setOwnsObjects( const bool val );

    protected:
      bool _ownsObjects;
      unsigned int _currentIndex;

    private:
      Q_DISABLE_COPY( CQStringList )
};

#endif




