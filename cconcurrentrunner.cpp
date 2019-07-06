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
CConcurrentRunner::CConcurrentRunner( CConcurrentList* list, QFuture< QHash<QString, int> > f ) {
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


bool CConcurrentRunner::finished() const {
  return _future.isFinished();
}


bool CConcurrentRunner::isFinished() {
  if( !_future.isFinished() ) {
    return false;
  }
  else {
    _hasFinished = true;
    _timeInMsec = _timer.elapsed();
    this->cleanup();
    return true;
  }
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


int CConcurrentRunner::adjustMaxListSize(
    const bool& threadsFull,
    const QList<int>& maxListSize,
    const int& idealThreadCount,
    const QList<int>& threadsInUse,
    const QList<bool>& backlog
) {
  // If there are lots of unused threads, make the list a lot smaller to keep them all busy.
  // If there are a few unused threads, make the next list a little smaller to keep them all busy.
  // If there was a backlog, make the next list a little bigger to give the other threads time to finish before adding more to the existing workload.
  // If there was no backlog and there is exactly one thread available, then we're in the sweet spot.

  int total = 0;
  int avgListSize;
  for( int i = 0; i < maxListSize.count(); ++i ) {
    total = total + maxListSize.at(i);
  }
  avgListSize = int( total / maxListSize.count() );

  if( !threadsFull ) {
    std::uniform_int_distribution<> dist( avgListSize - int( 0.25 * avgListSize ), avgListSize + int( 0.25 * avgListSize ) );
    return dist( *QRandomGenerator::global() );
  }
  else if( backlog.last() ) {
    return int( 1.2 * avgListSize );
  }
  else {
    int availableThreads = idealThreadCount - threadsInUse.last();

    if( 1 == availableThreads ) {
      return avgListSize;
    }
    else {
      int halfIdealThreadCount = int( 0.5 * idealThreadCount );

      if( availableThreads > halfIdealThreadCount ) {
        return int( 0.75 * avgListSize );
      }
      else {
        return int ( 0.9 * avgListSize );
      }
    }
  }
}


void CConcurrentRunner::writeUsage(
  const QList<int>& maxListSize,
  const QList<bool>& backlog,
  const QList<qint64>& waitTime,
  const QList<int>& threadsInUse
) {
  // FIXME: Write this some day.
  Q_UNUSED( maxListSize );
  Q_UNUSED( backlog );
  Q_UNUSED( waitTime );
  Q_UNUSED( threadsInUse );
}
//-------------------------------------------------------------------------------------
