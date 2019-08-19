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

#ifndef QARCOMMANDLINEOPTION_H
#define QARCOMMANDLINEOPTION_H

#include <QtCore>

class QARCommandLineOption {
  public:
    enum {
      ParseAsLongOptions // Currently a nonfunctional dummy that just preserves the appearance of compatibility with QCommandLineParser
    };

    QARCommandLineOption(
      const QString &name,
      const QString &description = QString(),
      const QString &valueName = QString(),
      const QString &defaultValue = QString()
    );
    QARCommandLineOption(
      const QString &name,
      const QString &description,
      const QStringList &valueNames,
      const QStringList &defaultValues = QStringList()
    );
    QARCommandLineOption(
      const QStringList &names,
      const QString &description = QString(),
      const QString &valueName = QString(),
      const QString &defaultValue = QString()
    );
    QARCommandLineOption(
      const QStringList &names,
      const QString &description,
      const QStringList &valueNames,
      const QStringList &defaultValues = QStringList()
    );

    
    QARCommandLineOption(const QARCommandLineOption &other);

    ~QARCommandLineOption();

    QARCommandLineOption &operator=(const QARCommandLineOption &other);

    QStringList names() const { return _names; }

    void setValueName(const QString &valueName) { _valueNames.clear(); _valueNames.append( valueName ); }
    void setValueNames(const QStringList &valueNames) { _valueNames = valueNames; }
    QStringList valueNames() const { return _valueNames; }

    void setDescription(const QString &description) { _description = description; }
    QString description() const { return _description; }

    void setDefaultValue(const QString &defaultValue) { _defaultValues.clear(); _defaultValues.append( defaultValue ); }
    void setDefaultValues(const QStringList &defaultValues) { _defaultValues = defaultValues; }
    QStringList defaultValues() const { return _defaultValues; }
    
  protected:
    void addName( const QString& name );
    void assign( const QARCommandLineOption& other );

    QStringList _names;
    QString _description;
    QStringList _valueNames;
    QStringList _defaultValues;
};

#endif // QARCOMMANDLINEOPTION_H
