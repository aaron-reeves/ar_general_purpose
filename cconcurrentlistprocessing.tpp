/*
cconcurrentlistprocessing.h/tpp
-------------------------------
Begin: 2019-06-18
Author: Aaron Reeves <aaron.reeves@sruc.ac.uk>
---------------------------------------------------------
Copyright (C) 2019 - 2021 Scotland's Rural College (SRUC)

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#include "cconcurrentlistprocessing.h" // For convenience only: this would be a circular reference without guards.



//-------------------------------------------------------------------------------------
// Class CConcurrentProcessingManager
//-------------------------------------------------------------------------------------
template <class T>
CConcurrentProcessingManager<T>::CConcurrentProcessingManager( const int initialMaxListSize /* = 0 */, const bool autoAdjustMaxListSize /* = true */ ) {
  _threadID = 0;
  _idealThreadCount = QThread::idealThreadCount() - 1; // Keep a thread free to deal with this stuff...
  _autoAdjustMaxListSize = autoAdjustMaxListSize;
  _threadsFull = false;

  if( 0 != initialMaxListSize ) {
    _maxListSize = initialMaxListSize;
  }
  else {
    _maxListSize = 250;
  }
}


template <class T>
void CConcurrentProcessingManager<T>::appendListForDatabasePopulation(
  CConcurrentProcessingList<T>* list,
  QHash<QString, int>(CConcurrentProcessingList<T>::*fn)( CConfigDatabase*, int ),
  CConfigDatabase* dbConfig
) {
  ++_threadID;

  // If all threads are in use, wait until one finishes before starting another one.
  //--------------------------------------------------------------------------------
  _threadsFull = false;

  if( QThreadPool::globalInstance()->activeThreadCount() == QThread::idealThreadCount() ) {
    _threadsFull = true;
    QElapsedTimer waitTimer;
    _backlog.append( true );
    if( !_runners.isEmpty() ) {
      bool threadIsFinished = false;
      while( !threadIsFinished ) {
        for( int i = 0; i < _runners.count(); ++i ) {
          if( _runners[i]->isFinished() ) {
            threadIsFinished = true;
            break;
          }
        }
        if( !threadIsFinished ) {
          qDb() << "All threads in use, waiting 10 seconds...";
          QThread::sleep( 10 );
        }
      }
      _waitTime.append( waitTimer.elapsed() );
    }
  }
  else {
    _backlog.append( false );
    _waitTime.append( 0 );
  }

  _threadsInUse.append( QThreadPool::globalInstance()->activeThreadCount() );

  // Put the new batch of data in the queue for processing
  //------------------------------------------------------
  //qDebug() << "Spinning up thread" << _threadID << "with list of size" << list->count();
  _runners.append(
    new CConcurrentProcessingRunner<T>(
      list,
      QFuture< QHash<QString, int> >( QtConcurrent::run( list, fn, dbConfig, _threadID ) )
    )
  );

  // See if any items in the queue are finished
  //-------------------------------------------
  QList<int> runnersToDelete;
  for( int runnerIdx = 0; runnerIdx < _runners.count(); ++runnerIdx ) {
    if( _runners[runnerIdx]->isFinished() ) {
      if( ReturnCode::SUCCESS != _runners.at(runnerIdx)->result().value( QStringLiteral("ReturnCode") ) ) {
        logMsg( QStringLiteral( "Error encountered in runner %1: %2" ).arg( runnerIdx ).arg( ReturnCode::codeDescr( _runners.at(runnerIdx)->result().value( QStringLiteral("ReturnCode") ) ) ) );
      }
      _results = CDatabaseResults::mergeHash( _results, _runners.at(runnerIdx)->result() );
      runnersToDelete.append( runnerIdx );
    }
  }
  for( int j = 0; j < runnersToDelete.count(); ++j ) {
    delete _runners.takeAt( runnersToDelete.at(j) );
  }

  // Adjust max list size, if requested
  //-----------------------------------
  if( _autoAdjustMaxListSize ) {
    adjustMaxListSize();
  }
}


template <class T>
void CConcurrentProcessingManager<T>::waitForFinished() {
  for( int i = 0; i < _runners.count(); ++i ) {
    _runners[i]->waitForFinished();
  }

  for( int runnerIdx = 0; runnerIdx < _runners.count(); ++runnerIdx ) {
    if( ReturnCode::SUCCESS != _runners.at(runnerIdx)->result().value( QStringLiteral("ReturnCode") ) ) {
      logMsg( QStringLiteral( "Error encountered in runner %1: %2" ).arg( runnerIdx ).arg( ReturnCode::codeDescr( _runners.at(runnerIdx)->result().value( QStringLiteral("ReturnCode") ) ) ) );
    }
    _results = CDatabaseResults::mergeHash( _results, _runners.at(runnerIdx)->result() );
  }
  while( !_runners.isEmpty() ) {
    delete _runners.takeFirst();
  }

  //CConcurrentRunner::writeUsage( maxListSize, backlog, waitTime, threadsInUse );
}


template <class T>
void CConcurrentProcessingManager<T>::adjustMaxListSize() {
  // If there are lots of unused threads, make the list a lot smaller to keep them all busy.
  // If there are a few unused threads, make the next list a little smaller to keep them all busy.
  // If there was a backlog, make the next list a little bigger to give the other threads time to finish before adding more to the existing workload.
  // If there was no backlog and there is exactly one thread available, then we're in the sweet spot.

  _maxListSizes.append( _maxListSize );

  int total = 0;
  int avgListSize;
  for( int i = 0; i < _maxListSizes.count(); ++i ) {
    total = total + _maxListSizes.at(i);
  }
  avgListSize = int( total / _maxListSizes.count() );

  if( !_threadsFull ) {
    std::uniform_int_distribution<> dist( avgListSize - int( 0.25 * avgListSize ), avgListSize + int( 0.25 * avgListSize ) );
    _maxListSize = dist( *QRandomGenerator::global() );
  }
  else if( _backlog.last() ) {
    _maxListSize = int( 1.2 * avgListSize );
  }
  else {
    int availableThreads = _idealThreadCount - _threadsInUse.last();

    if( 1 == availableThreads ) {
      _maxListSize = avgListSize;
    }
    else {
      int halfIdealThreadCount = int( 0.5 * _idealThreadCount );

      if( availableThreads > halfIdealThreadCount ) {
        _maxListSize = int( 0.75 * avgListSize );
      }
      else {
        _maxListSize = int ( 0.9 * avgListSize );
      }
    }
  }
}


//template <class T>
//void CConcurrentProcessingManager<T>::writeUsage() {
//  // FIXME: Write this some day.
//  _maxListSize...
//  _backlog...
//  _waitTime...
//  _threadsInUse...
//}
//-------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------
// Class CConcurrentProcessingRunner
//-------------------------------------------------------------------------------------
template <class T>
CConcurrentProcessingRunner<T>::CConcurrentProcessingRunner( CConcurrentProcessingList<T>* list, const QFuture< QHash<QString, int> >& f ) {
  _timer.start();
  _timeInMsec = 0;
  _hasFinished = false;
  _list = list;
  _future = f;
}


template <class T>
CConcurrentProcessingRunner<T>::CConcurrentProcessingRunner( const QFuture< QHash<QString, int> >& f ) {
  _timer.start();
  _timeInMsec = 0;
  _hasFinished = false;
  _list = nullptr;
  _future = f;
}


template <class T>
CConcurrentProcessingRunner<T>::~CConcurrentProcessingRunner() {
  if( !_hasFinished ) {
    qDb() << "CConcurrentRunner::~CConcurrentRunner(): _hasFinished is false.  Is this really what you want to do?";
    delete _list;
  }
  else if( nullptr != _list ) {
    qDb() << "CConcurrentRunner::~CConcurrentRunner(): Deleting list that's not null.  Did you forget to clean up?";
    delete _list;
  }
}


template <class T>
void CConcurrentProcessingRunner<T>::waitForFinished() {
  _future.waitForFinished();
  _hasFinished = true;
  this->cleanup();
  _timeInMsec = _timer.elapsed();
}


template <class T>
bool CConcurrentProcessingRunner<T>::finished() const {
  return _future.isFinished();
}


template <class T>
bool CConcurrentProcessingRunner<T>::isFinished() {
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


template <class T>
qint64 CConcurrentProcessingRunner<T>::runtime() const {
  if( !_hasFinished )
    return -1;
  else
    return _timeInMsec;
}


template <class T>
void CConcurrentProcessingRunner<T>::cleanup() {
  if( nullptr != _list ) {
    delete _list;
    _list = nullptr;
  }
}


template <class T>
QHash<QString, int> CConcurrentProcessingRunner<T>::result() const {
  return _future.result();
}
//-------------------------------------------------------------------------------------

