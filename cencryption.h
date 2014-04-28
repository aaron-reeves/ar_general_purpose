/*
cencryption.h/cpp
Begin: 2003/07/03
-----------------
Copyright (C) 2003 - 2007 by Aaron Reeves
development@reevesdigital.com

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
