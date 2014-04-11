/*
cencryption.h
Begin: 2003/07/03
Last revision: $Date: 2011-10-25 04:57:17 $ $Author: areeves $
Version: $Revision: 1.3 $
Project: (various)
Website: http://www.aaronreeves.com/qtclasses
-------------------------------
Copyright (C) 2003 - 2007 by Aaron Reeves
aaron@aaronreeves.com

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#ifndef CENCRYPTION_H
#define CENCRYPTION_H

#include <qstring.h>

class CEncryption {
  public:
    static QString rc4Encrypt( QString input, QString pwd );
    static QString hexEncode( QString str );
    static QString hexDecode( QString str );

  protected:
    static void rc4( char* ByteInput, char* pwd, char* &ByteOutput );
    static int intFromHex( QChar hc, QChar lc );
};

#endif
