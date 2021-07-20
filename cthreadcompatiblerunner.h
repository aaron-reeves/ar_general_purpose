#ifndef CTHREADCOMPATIBLERUNNER_H
#define CTHREADCOMPATIBLERUNNER_H

#include <QObject>

#include <ar_general_purpose/returncodes.h>

class CThreadCompatibleRunner : public QObject {
  Q_OBJECT

  public:
      CThreadCompatibleRunner( bool* cancelClickedPtr/*, ObjectWithFunction* progressObj, ReportProgressFn progressFunction*/ );
      virtual ~CThreadCompatibleRunner();

      bool processingInterrupted() const { return ( _result & ReturnCode::PROCESSING_INTERRUPTED ); }

  public slots:
      // Override or overload this function to do anything useful.
      virtual void process() = 0; // process() MUST call finalize() as its last action!

  signals:
      void message( QString err );
      void finished( int result );

  protected:
    virtual bool checkForTerminated(); // Always call this from an overridden version, but it's possible to override to do more stuff
    virtual void finalize(); // Always call this from an overridden version, but it's possible to override to do more stuff

    int _result;
    QString _errorMessage;

    // Pointers owned and accessible by a GUI object.
    // It must either be treated as read-only by CThreadCompatibleRunner,
    // or guarded by a mutex if its value is changed by CThreadCompatibleRunner.
    const bool* _ptrCancelClicked;

    //ObjectWithFunction* _progressObj;
    //ReportProgressFn _progressFunction;

  private:
    Q_DISABLE_COPY( CThreadCompatibleRunner )
};

#endif // CTHREADCOMPATIBLERUNNER_H
