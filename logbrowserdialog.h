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
