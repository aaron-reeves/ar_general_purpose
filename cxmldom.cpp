/*
cxmldom.h/cpp
-------------
Begin: 2004/10/03
Author: Aaron Reeves <development@reevesdigital.com>
--------------------------------------------------
Copyright (C) 2004 - 2007 Aaron Reeves

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#include "cxmldom.h"

#include <qfile.h>
#include <qdom.h>
#include <qmap.h>
#include <qstring.h>
#include <qdebug.h>

#include "qcout.h"

void CXmlDom::showElements( QDomNode parentElem, bool recurse, uint indent ) {
	QString spacer = "";
	uint i;

	for( i = 0; i < indent; ++i ) {
		spacer.append( " " );
	}

	QDomElement e;
	QDomComment c;
	QDomNode n = parentElem.firstChild();
	QDomText t;
	QDomAttr a;
	QDomNamedNodeMap attr;

	while( !n.isNull() ) {
    if( n.isComment() ) {
      cout << "COMMENT!!!" << endl;
    }
    c = n.toComment();
    if( !c.isNull() ) { // the node really was a comment
      cout << "Comment" << endl;
    }
    
    
		e = n.toElement(); // try to convert the node to an element.
		if( !e.isNull() ) { // the node was really an element.
			cout << spacer << "TAG " << e.tagName() << ":" << endl;

			attr = e.attributes();
			for( i = 0; i < attr.length(); ++i ) {
				a = attr.item( i ).toAttr();
				if( !( a.isNull() ) ) {
					cout << spacer << "  " << "ATTRIBUTE " << a.name() << ": " << a.value().trimmed() << endl;
				}
			}

			if( recurse ) showElements( e, recurse, indent+2 );
		}

		t = n.toText();
		if( !t.isNull() ) { // the node contained text
			cout << spacer << "TEXT VALUE: " << t.data().trimmed() << endl;
		}

		n = n.nextSibling();
	}
}



QDomElement CXmlDom::findElement( QDomNode node, const QString& name, const int idx ) {
	QDomElement result;
  QDomElement e;
  int nFound = 0;
	QDomNode n = node.firstChild();
	
	while( !( n.isNull() ) ) {
    if( n.isElement() ) {
      e = n.toElement();
      
      if( !( e.isNull() ) ) {
    		if( name == e.tagName() ) {
          ++nFound;
          
          if( nFound > idx ) {
    			 result = e;
    			 break;
          }
    		}
      }
    }
		n = n.nextSibling();
	}

	return result;
}



int CXmlDom::elementCount( QDomNode node, const QString& name ) {
	QDomElement e;
	QDomNode n= node.firstChild();
  int result = 0;
	
	while( !( n.isNull() ) ) { 
    if( n.isElement() ) {
  		e = n.toElement();
  		if( name == e.tagName() ) {
        ++result;
  		}
    }
    
		n = n.nextSibling();
	}

	return result;     
}



QDomAttr CXmlDom::findAttribute( QDomNode node, const QString& name ) {
  unsigned int i;
  QDomAttr a, b;  
  QDomNamedNodeMap attr;
  
  if( !node.isNull() ) {
  	attr = node.attributes();
  	for( i = 0; i < attr.length(); ++i ) {
  		b = attr.item( i ).toAttr();
  		if( !( b.isNull() ) ) {
        if( name == b.name() ) {
          a = b;
          break;
        }
  		}
    }  
  }
  
  return a;
}



QDomElement CXmlDom::findFirstElementByName( QDomNode node, const QString& childName, bool recurse ) {
	QDomElement temp, result;
	QDomNode n;
  QDomElement e;
  
  n = node.firstChild();
	
	while( !( n.isNull() ) ) {
    if( n.isElement() ) {
      e = n.toElement();
  
      if( !( e.isNull() ) ) { // the node really was an element 
    		//cout << e.tagName() << endl;
    		if( childName == e.tagName() ) {
    			result = e;
    			break;
    		}
  
    		if( recurse ) {
    			temp = findFirstElementByName( e, childName, recurse ); 
    			if( !( temp.isNull() ) ) {
    				result = temp;
    				break;
    			}
    		}
      }
    }
		n = n.nextSibling();
	}

	return result;
}



void CXmlDom::fillDict( TStringDict* d, QDomNode node ) {
	QDomNode n = node.firstChild();
	QDomElement e = n.toElement();
	QDomText t;
	QDomAttr a;
	QDomNamedNodeMap attr;
  QString key;
  unsigned int i;
  
  if( !( node.toElement().isNull() ) )
    key = node.toElement().tagName();

	attr = node.attributes();
	for( i = 0; i < attr.length(); ++i ) {
		a = attr.item( i ).toAttr();
		if( !( a.isNull() ) ) {
			d->insert( QString( "ATTR_%1" ).arg( a.name() ), a.value().trimmed() );
		}
	}

	while( !n.isNull() ) {
    if( n.isElement() ) {
  		e = n.toElement(); 
  		if( !e.isNull() ) {     
        t = e.firstChild().toText();
        if( !t.isNull() ) {
          d->insert( e.tagName(), t.data().trimmed() );
        }
  		}
  
  		t = n.toText();
  		if( !t.isNull() ) {
  			d->insert( key, t.data().trimmed() );
  			//cout << "TAG: " << lastKey << ": " << t.data().stripWhiteSpace() << endl;
  		}
    }

		n = n.nextSibling();
	}
}

/*
TStringDict* CXmlDom::makeDictNew( QDomNode node ) {
	TStringDict* d = new TStringDict();
	fillDict( d, node, "" );
	return d;
}
*/
