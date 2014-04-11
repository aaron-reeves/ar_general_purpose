#ifndef CXMLDOM_H
#define CXMLDOM_H

#include <qdom.h>
#include <qmap.h>
#include <qstring.h>

typedef QMap<QString, QString> TStringDict;

class CXmlDom {
  public:
    void showElements( QDomNode parentElem, bool recurse = false, uint indent = 0 );
    
    int elementCount( QDomNode node, const QString& name ); 		
    QDomElement findElement( QDomNode node, const QString& name, const int idx = 0 );
    QDomElement findFirstElementByName( QDomNode node, const QString& childName, bool recurse = false );
    	
    QDomAttr findAttribute( QDomNode node, const QString& name );
    
    void fillDict( TStringDict* d, QDomNode node ); 
};

#endif
