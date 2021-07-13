#include "cthreadcompatiblerunner.h"

#include <QDebug>
#include <QCoreApplication>

CThreadCompatibleRunner::CThreadCompatibleRunner( bool* cancelClickedPtr/*, ObjectWithFunction* progressObj, ReportProgressFn progressFunction*/ ) {
  // you could copy data from constructor arguments to internal variables here.
  _ptrCancelClicked = cancelClickedPtr;
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

  if( *_ptrCancelClicked ) {
    emit message( "Processing terminated." );

    _result = ( _result | ReturnCode::PROCESSING_INTERRUPTED );

    emit terminated();
    finalize();
    return true;
  }
  else {
    return false;
  }
}


void CThreadCompatibleRunner::finalize() {
  // Reclaim _myWorker to do whatever comes next.
  this->moveToThread( QCoreApplication::instance()->thread() );
  emit finished();
}
