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

// https://wiki.qt.io/Browser_for_QDebug_output

#include <ar_general_purpose/logbrowser.h>
 
#include <QMetaType>
 
#include <ar_general_purpose/logbrowserdialog.h>
 
LogBrowser::LogBrowser(QObject *parent /* = nullptr */) : QObject( parent ) {
  qRegisterMetaType<QtMsgType>("QtMsgType");
  browserDialog = new LogBrowserDialog;
  connect(this, SIGNAL(sendMessage(QtMsgType,QString)), browserDialog, SLOT(outputMessage(QtMsgType,QString)), Qt::QueuedConnection);
  browserDialog->show();
}
 
 
LogBrowser::~LogBrowser() {
  delete browserDialog;
}
 
 
void LogBrowser::outputMessage(QtMsgType type, const QString &msg) {
  emit sendMessage( type, msg );
}
