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
