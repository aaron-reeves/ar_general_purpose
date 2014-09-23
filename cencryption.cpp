/*
cencryption.h/cpp
Begin: 2003/07/30
-----------------
Copyright (C) 2003 - 2007 by Aaron Reeves
aaron.reeves@naadsm.org

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#include "cencryption.h"

#include <qstring.h>
#include <QDebug>

QString CEncryption::rc4Encrypt( QString input, QString pwd ) {
  QString temp = "";
  int i;
  int j = 0;
  ushort t;
  ushort tmp, tmp2;
  ushort s[256], k[256];

  for( tmp = 0; tmp < 256; ++tmp ){
    s[tmp] = tmp;
    k[tmp] = pwd.at( tmp % pwd.length() ).unicode();
  }

  for( i = 0; i < 256; ++i ){
    j = (j + s[i] + k[i]) % 256;
    tmp = s[i];
    s[i] = s[j];
    s[j] = tmp;
  }

  i = 0;
  j = 0;
  for( tmp = 0; tmp < input.length(); ++tmp ){
    i = (i + 1) % 256;
    j = (j + s[i]) % 256;
    tmp2=s[i];
    s[i]=s[j];
    s[j]=tmp2;
    t = (s[i] + s[j]) % 256;
    if( QChar( s[t] ) == input.at(tmp) ) {
      temp.append( input.at(tmp) );
    }
    else {
      temp.append( QChar( s[t]^input.at(tmp).unicode() ) );
    }
    //qDebug() << "Char" << tmp << ":" << temp.at(tmp) << temp.at(tmp).unicode();
  }

  return temp;
}


// rc4 encryption function based on code written by Joseph Gama
// See http://www.planet-source-code.com/vb/scripts/ShowCode.asp?txtCodeID=4653&lngWId=3
void CEncryption::rc4( char* ByteInput, char* pwd, char* &ByteOutput ){
  char* temp;
  int i,j=0,t,tmp,tmp2,s[256], k[256];
  for (tmp=0;tmp<256;tmp++){
    s[tmp]=tmp;
    k[tmp]=pwd[(tmp % strlen((char *)pwd))];
  }
  for (i=0;i<256;i++){
    j = (j + s[i] + k[i]) % 256;
    tmp=s[i];
    s[i]=s[j];
    s[j]=tmp;
  }

  temp = new char [ (int)strlen(ByteInput)  + 1 ] ;
  i=j=0;
  for (tmp=0;tmp<(int)strlen(ByteInput);tmp++){
    i = (i + 1) % 256;
    j = (j + s[i]) % 256;
    tmp2=s[i];
    s[i]=s[j];
    s[j]=tmp2;
    t = (s[i] + s[j]) % 256;
    if (s[t]==ByteInput[tmp]) {
      temp[tmp]=ByteInput[tmp];
    }
    else {
      temp[tmp]=s[t]^ByteInput[tmp];
    }
    //qDebug() << "Char" << tmp << ":" << temp[tmp] << (int)temp[tmp];
  }
  temp[tmp]='\0';
  //qDebug() << "temp:" << temp;
  ByteOutput=temp;
}


QString CEncryption::hexEncode( QString temp ) {
  int i;
  unsigned char c;
  QString str;
  QString temp3 = "";
  QString temp4;

  for( i = 0; i < temp.length(); ++i ) {
    c = temp.at(i).cell();

    //qDebug() << c;

    if( c < 16 ) {
      temp4.sprintf( "0%X", c );
    }
    else {
      temp4.sprintf( "%X", c );
    }

    temp3.append( temp4 );
  }

  return temp3;
}


QString CEncryption::hexDecode( QString str ) {
  int i;
  QChar hc, lc;
  QString ret;

  for( i = 0; i < str.length(); i = i + 2 ) {
    hc = str.at( i );
    lc = str.at( i + 1 );
    ret.append( intFromHex( hc, lc ) );
  }

  return ret;
}


// a subroutine that unescapes escaped characters.
int CEncryption::intFromHex(QChar hc, QChar lc ) {
  int Hi;        // holds high byte
  int Lo;        // holds low byte
  int Result;    // holds result
  QChar a = 'a';

  if( hc.isDigit() )
    Hi = hc.digitValue();
  else {
    hc = hc.toLower();
    Hi = hc.unicode() - a.unicode() + 10;
  }

  if( lc.isDigit() )
    Lo = lc.digitValue();
  else {
    lc = lc.toLower();
    Lo = lc.unicode() - a.unicode() + 10;
  }

  Result = Lo + (16 * Hi);

  return (Result);
}
