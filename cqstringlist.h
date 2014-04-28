/*
cqstringlist.h/cpp
------------------
Begin: 2003/04/09
Author: Aaron Reeves <development@reevesdigital.com>
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
@author Aaron Reeves (development@reevesdigital.com)
@version 040904
*/
class CQStringList : public QList<QString*>  {

    public:
      CQStringList();

      virtual ~CQStringList();

      bool contains( const QString str );

      void explode( QString str, QChar splitter );

      void debug( void );

      void setAutoDelete( bool val );

    protected:
      bool _isAutoDelete;
      unsigned int _currentIndex;
};

#endif




