/*
cxmldom.h
---------
Begin: 2004/10/03
Author: Aaron Reeves <aaron.reeves@naadsm.org>
--------------------------------------------------
Copyright (C) 2004 - 2007 Aaron Reeves

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#ifndef CXMLDOM_H
#define CXMLDOM_H

#include <qdom.h>
#include <qmap.h>
#include <qstring.h>

typedef QMap<QString, QString> TStringDict;

class CXmlDom {
  public:
    void showElements( const QDomNode& parentElem, const bool recurse = false, const int indent = 0 );
    
    int elementCount( const QDomNode& node, const QString& name );
    QDomElement findElement( const QDomNode& node, const QString& name, const int idx = 0 );
    QDomElement findFirstElementByName( const QDomNode& node, const QString& childName, const bool recurse = false );
    	
    QDomAttr findAttribute( const QDomNode& node, const QString& name );
    
    void fillDict( TStringDict* d, const QDomNode& node );
};

#endif
