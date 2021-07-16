#ifndef CTHREADCOMPATIBLERUNNER_H
#define CTHREADCOMPATIBLERUNNER_H

#include <QObject>

#include <ar_general_purpose/returncodes.h>

class CThreadCompatibleRunner : public QObject {
    Q_OBJECT

    public:
        CThreadCompatibleRunner( bool* cancelClickedPtr/*, ObjectWithFunction* progressObj, ReportProgressFn progressFunction*/ );
        ~CThreadCompatibleRunner();

        bool processingInterrupted() const { return ( _result & ReturnCode::PROCESSING_INTERRUPTED ); }

    public slots:
        // Override or overload this function to do anything useful.
        virtual void process() = 0; // process() MUST call finalize() as its last action!

    signals:
        void message( QString err );
        void terminated();
        void finished();

    protected:
      bool checkForTerminated();
      void finalize();

      int _result;
      QString _errorMessage;

      // Pointers owned and accessible by a GUI object.
      // It must either be treated as read-only by CThreadCompatibleRunner,
      // or guarded by a mutex if its value is changed by CThreadCompatibleRunner.
      const bool* _ptrCancelClicked;

//    ObjectWithFunction* _progressObj;
//    ReportProgressFn _progressFunction;
};

#endif // CTHREADCOMPATIBLERUNNER_H
