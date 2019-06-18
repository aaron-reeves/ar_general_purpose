#include "cconcurrentrunner.h"


//-------------------------------------------------------------------------------------
// Class CConcurrentList
//-------------------------------------------------------------------------------------
CConcurrentList::CConcurrentList() {
  // Do nothing here.
}


CConcurrentList::~CConcurrentList() {
  // Do nothing here.
}


QHash<QString, int> CConcurrentList::populateDatabase( CConfigDatabase cfdb, const int& dataSourceID, const bool& insertRecords ) {
  Q_UNUSED( cfdb );
  Q_UNUSED( dataSourceID );
  Q_UNUSED( insertRecords );

  return QHash<QString, int>();
}
//-------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------
// Class CCtsRunner
//-------------------------------------------------------------------------------------
CConcurrentRunner::CConcurrentRunner(CConcurrentList* list, QFuture< QHash<QString, int> > f ) {
  _timer.start();
  _timeInMsec = 0;
  _hasFinished = false;
  _list = list;
  _future = f;
}


CConcurrentRunner::~CConcurrentRunner() {
  if( !_hasFinished ) {
    qDebug() << "CConcurrentRunner::~CConcurrentRunner(): _hasFinished is false.  Is this really what you want to do?";
    delete _list;
  }

  if( nullptr != _list ) {
    qDebug() << "CConcurrentRunner::~CConcurrentRunner(): Deleting list that's not null.  Did you forget to clean up?";
    delete _list;
  }
}


void CConcurrentRunner::waitForFinished() {
  _future.waitForFinished();
  _hasFinished = true;
  this->cleanup();
  _timeInMsec = _timer.elapsed();
}


bool CConcurrentRunner::isFinished() {
  bool retval = _future.isFinished();

  if( retval ) {
    _hasFinished = true;
      _timeInMsec = _timer.elapsed();
    this->cleanup();
  }

  return retval;
}


qint64 CConcurrentRunner::runtime() const {
  if( !_hasFinished )
    return -1;
  else
    return _timeInMsec;
}


void CConcurrentRunner::cleanup() {
  delete _list;
  _list = nullptr;
}


QHash<QString, int> CConcurrentRunner::result() const {
  return _future.result();
}


int CConcurrentRunner::adjustMaxListSize(const int& maxListSize, const int& idealThreadCount, const QList<int>& threadsInUse, const QList<qint64>& processingTime ) {
  Q_UNUSED( idealThreadCount );
  Q_UNUSED( threadsInUse );
  Q_UNUSED( processingTime );

  return maxListSize;
}
//-------------------------------------------------------------------------------------
