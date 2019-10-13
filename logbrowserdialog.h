/*
logbrowserdialog.h/cpp
----------------------
Begin: 2014-04-11
Author: Aaron Reeves <aaron.reeves@naadsm.org>
---------------------------------------------------
Copyright (C) 2014 - 2016 Aaron Reeves

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

// See https://wiki.qt.io/Browser_for_QDebug_output

#ifndef LOGBROWSERDIALOG_H
#define LOGBROWSERDIALOG_H
 
#include <QDialog>
 
class QTextBrowser;
class QPushButton;
 
class LogBrowserDialog : public QDialog
{
    Q_OBJECT
 
public:
    LogBrowserDialog(QWidget *parent = nullptr );
    ~LogBrowserDialog();
 
public slots:
    void outputMessage( QtMsgType type, const QString &msg );
 
protected slots:
    void save();
 
protected:
    virtual void keyPressEvent( QKeyEvent *e );
    virtual void closeEvent( QCloseEvent *e );
 
    QTextBrowser *browser;
    QPushButton *clearButton;
    QPushButton *saveButton;
};
 
#endif // LOGBROWSERDIALOG_H
