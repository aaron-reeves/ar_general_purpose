/*
qarcommandlineoption.h/cpp
--------------------------
Begin: 2019/08/17
Author: Aaron Reeves <aaron.reeves@sruc.ac.uk>
----------------------------------------------
Copyright (C) 2019 Epidemiology Research Unit, Scotland's Rural College (SRUC)

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#include "qarcommandlineoption.h"


void QARCommandLineOption::addName( const QString& name ) {
  QString n = name;

  // Strip off any dashes that might be present...
  while( n.startsWith( '-' ) ) {
    n = n.right( n.length() - 1 );
  }

  // ...then add back the right number.
  if( 1 == n.length() )
    n.prepend( '-' );
  else if( 1 < n.length() )
    n.prepend( "--" );

  if( !n.isEmpty() ) {
    _names.append( n );
  }
}


QARCommandLineOption::QARCommandLineOption(
  const QString &name,
  const QString &description,  /* = QString() */
  const QString &valueName, /* = QString() */
  const QString &defaultValue /* = QString() */
) :
  _description( description )
{
  addName( name );

  if( !valueName.isEmpty() )
    _valueNames.append( valueName );

  if( !defaultValue.isEmpty() )
    _defaultValues.append( defaultValue );
}


QARCommandLineOption::QARCommandLineOption(
  const QString &name,
  const QString &description,
  const QStringList &valueNames,
  const QStringList &defaultValues /* = QStringList() */
) :
  _description( description ),
  _valueNames( valueNames ),
  _defaultValues( defaultValues )
{
  addName( name );
}


QARCommandLineOption::QARCommandLineOption(
  const QStringList &names,
  const QString &description, /* = QString() */
  const QString &valueName, /* = QString() */
  const QString &defaultValue /* = QString() */
) :
  _description( description )
{
  foreach( const QString& n, names ) {
    addName( n );
  }

  if( !valueName.isEmpty() )
    _valueNames.append( valueName );

  if( !defaultValue.isEmpty() )
    _defaultValues.append( defaultValue );
}


QARCommandLineOption::QARCommandLineOption(
  const QStringList &names,
  const QString &description,
  const QStringList &valueNames,
  const QStringList &defaultValues /* = QStringList() */
) :
  _description( description ),
  _valueNames( valueNames ),
  _defaultValues( defaultValues )
{
  foreach( const QString& n, names ) {
    addName( n );
  }
  _description = description;
  _valueNames = valueNames;
  _defaultValues = defaultValues;
}


void QARCommandLineOption::assign( const QARCommandLineOption& other ) {
  _names = other._names;
  _description = other._description;
  _valueNames = other._valueNames;
  _defaultValues = other._defaultValues;
}


QARCommandLineOption::QARCommandLineOption(const QARCommandLineOption &other) :
  _names( other._names ),
  _description( other._description ),
  _valueNames( other._valueNames ),
  _defaultValues( other._defaultValues )
{
  // Nothing else to do here
}


QARCommandLineOption::~QARCommandLineOption() {
  // Nothing to do here
}


QARCommandLineOption& QARCommandLineOption::operator=(const QARCommandLineOption &other) {
  assign( other );
  return *this;
}

