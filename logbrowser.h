/*
logbrowser.h/cpp
----------------
Begin: 2014-04-11
Author: Aaron Reeves <aaron.reeves@naadsm.org>
---------------------------------------------------
Copyright (C) 2014 - 2016 Aaron Reeves

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

// See https://wiki.qt.io/Browser_for_QDebug_output

#ifndef LOGBROWSER_H
#define LOGBROWSER_H
 
#include <QObject>
 
class LogBrowserDialog;
 
class LogBrowser : public QObject
{
    Q_OBJECT
public:
    explicit LogBrowser( QObject *parent = nullptr );
    ~LogBrowser();
 
public slots:
    void outputMessage( QtMsgType type, const QString &msg );
 
signals:
    void sendMessage( QtMsgType type, const QString &msg );
 
private:
    LogBrowserDialog *browserDialog;
 
};

#endif // LOGBROWSER_H
