#include "cthreadcompatiblerunner.h"

#include <QDebug>
#include <QCoreApplication>

CThreadCompatibleRunner::CThreadCompatibleRunner( bool* terminatedPtr, QObject* parent /*, ObjectWithFunction* progressObj, ReportProgressFn progressFunction*/ )
  : QObject( parent )
{
  // you could copy data from constructor arguments to internal variables here.
  _ptrTerminated = terminatedPtr;
  _result = ReturnCode::UNKNOWN_RESULT;

//  _progressObj = progressObj;
//  _progressFunction = progressFunction;
}


CThreadCompatibleRunner::~CThreadCompatibleRunner() {
  qDebug() << "CThreadCompatibleRunner deleted!";
}


bool CThreadCompatibleRunner::checkForTerminated() {
  //Q_ASSERT( nullptr != _progressObj );
  //Q_ASSERT( nullptr != _progressFunction );
  //std::invoke( _progressFunction, _progressObj, _max );

  if( ( nullptr != _ptrTerminated ) && *_ptrTerminated ) {
    _result = ( _result | ReturnCode::PROCESSING_INTERRUPTED );

    finalize();
    return true;
  }
  else {
    return false;
  }
}


void CThreadCompatibleRunner::finalize() {
  emit finished( _result );

  // Reclaim the runner to do whatever comes next.
  this->moveToThread( QCoreApplication::instance()->thread() );
}
