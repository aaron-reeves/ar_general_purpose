/*
cencryption.h/cpp
Begin: 2003/07/03
-----------------
Copyright (C) 2003 - 2007 by Aaron Reeves
aaron.reeves@naadsm.org

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#ifndef CENCRYPTION_H
#define CENCRYPTION_H

#include <qstring.h>

class CEncryption {
  public:
    static QByteArray rc4Encrypt( const QString& input, const QString& pwd );
    static QString hexEncode( const QString& str );
    static QString hexDecode( const QString& str );

  protected:
    static void rc4( const char* ByteInput, const char* pwd, char*& ByteOutput );
    static int intFromHex( const QChar hc, const QChar lc );
};

#endif
