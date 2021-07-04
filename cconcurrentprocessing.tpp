/*
cconcurrentprocessing.h/tpp
---------------------------
Begin: 2019-06-18
Author: Aaron Reeves <aaron.reeves@sruc.ac.uk>
---------------------------------------------------------
Copyright (C) 2019 - 2021 Scotland's Rural College (SRUC)

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#include "cconcurrentprocessing.h" // For convenience only: this would be a circular reference without guards.

//-------------------------------------------------------------------------------------
// Container classes: dummy implementations of virtual functions
//-------------------------------------------------------------------------------------
template <class T>
QHash<QString, int> CConcurrentProcessingList<T>::resultsTemplate() const {
  Q_ASSERT_X( false, "CConcurrentProcessingList:resultsTemplate", "Function does not exist." );

  return QHash<QString, int>();
}

template <class T>
QHash<QString, int> CConcurrentProcessingList<T>::dbProcessDynamic( const CConfigDatabase* dbConfig, const int threadID ) {
  Q_UNUSED( dbConfig );
  Q_UNUSED( threadID );
  Q_ASSERT_X( false, "CConcurrentProcessingList::dbProcessDynamic (2 params)", "Function does not exist." );

  return QHash<QString, int>();
}

template <class T>
QHash<QString, int> CConcurrentProcessingList<T>::dbProcessDynamic( const CConfigDatabase* dbConfig, const int threadID, const QHash<QString, QVariant>& params ) {
  Q_UNUSED( dbConfig );
  Q_UNUSED( threadID );
  Q_UNUSED( params );
  Q_ASSERT_X( false, "CConcurrentProcessingList::dbProcessDynamic (3 params)", "Function does not exist." );

  return QHash<QString, int>();
}

template <class T>
QHash<QString, int> CConcurrentProcessingList<T>::dbPopulateDynamic( const CConfigDatabase* dbConfig, const int threadID ) {
  Q_UNUSED( dbConfig );
  Q_UNUSED( threadID );
  Q_ASSERT_X( false, "CConcurrentProcessingList::dbPopulateDynamic (2 params)", "Function does not exist." );

  return QHash<QString, int>();
}

template <class T>
QHash<QString, int> CConcurrentProcessingList<T>::dbPopulateDynamic( const CConfigDatabase* dbConfig, const int threadID, const QHash<QString, QVariant>& params ) {
  Q_UNUSED( dbConfig );
  Q_UNUSED( threadID );
  Q_UNUSED( params );
  Q_ASSERT_X( false, "CConcurrentProcessingList::dbPopulateDynamic (3 params)", "Function does not exist." );

  return QHash<QString, int>();
}

template <class T>
QHash<QString, int> CConcurrentProcessingList<T>::dbPopulateStatic(
  const CConfigDatabase* dbConfig,
  const int startIdx,
  const int length,
  const int threadID
) const {
  Q_UNUSED( dbConfig );
  Q_UNUSED( startIdx );
  Q_UNUSED( length );
  Q_UNUSED( threadID );
  Q_ASSERT_X( false, "CConcurrentProcessingList::dbPopulateStatic (4 params)", "Function does not exist." );

  return QHash<QString, int>();
}

template <class T>
QHash<QString, int> CConcurrentProcessingList<T>::dbPopulateStatic(
  const CConfigDatabase* dbConfig,
  const int startIdx,
  const int length,
  const int threadID,
  const QHash<QString, QVariant>& otherParams
) const {
  Q_UNUSED( dbConfig );
  Q_UNUSED( startIdx );
  Q_UNUSED( length );
  Q_UNUSED( threadID );
  Q_UNUSED( otherParams );
  Q_ASSERT_X( false, "CConcurrentProcessingList::dbPopulateStatic (5 params)", "Function does not exist." );

  return QHash<QString, int>();
}


template <class T>
QHash<QString, int> CConcurrentProcessingVector<T>::resultsTemplate() const {
  Q_ASSERT_X( false, "CConcurrentProcessingVector:resultsTemplate", "Function does not exist." );

  return QHash<QString, int>();
}

template <class T>
QHash<QString, int> CConcurrentProcessingVector<T>::dbProcessDynamic( const CConfigDatabase* dbConfig, const int threadID ) {
  Q_UNUSED( dbConfig );
  Q_UNUSED( threadID );
  Q_ASSERT_X( false, "CConcurrentProcessingVector::dbProcessDynamic (2 params)", "Function does not exist." );

  return QHash<QString, int>();
}

template <class T>
QHash<QString, int> CConcurrentProcessingVector<T>::dbProcessDynamic( const CConfigDatabase* dbConfig, const int threadID, const QHash<QString, QVariant>& params ) {
  Q_UNUSED( dbConfig );
  Q_UNUSED( threadID );
  Q_UNUSED( params );
  Q_ASSERT_X( false, "CConcurrentProcessingVector::dbProcessDynamic (3 params)", "Function does not exist." );

  return QHash<QString, int>();
}

template <class T>
QHash<QString, int> CConcurrentProcessingVector<T>::dbPopulateDynamic( const CConfigDatabase* dbConfig, const int threadID ) {
  Q_UNUSED( dbConfig );
  Q_UNUSED( threadID );
  Q_ASSERT_X( false, "CConcurrentProcessingVector::dbPopulateDynamic (2 params)", "Function does not exist." );

  return QHash<QString, int>();
}

template <class T>
QHash<QString, int> CConcurrentProcessingVector<T>::dbPopulateDynamic( const CConfigDatabase* dbConfig, const int threadID, const QHash<QString, QVariant>& params ) {
  Q_UNUSED( dbConfig );
  Q_UNUSED( threadID );
  Q_UNUSED( params );
  Q_ASSERT_X( false, "CConcurrentProcessingVector::dbPopulateDynamic (3 params)", "Function does not exist." );

  return QHash<QString, int>();
}

template <class T>
QHash<QString, int> CConcurrentProcessingVector<T>::dbPopulateStatic(
  const CConfigDatabase* dbConfig,
  const int startIdx,
  const int length,
  const int threadID
) const {
  Q_UNUSED( dbConfig );
  Q_UNUSED( startIdx );
  Q_UNUSED( length );
  Q_UNUSED( threadID );
  Q_ASSERT_X( false, "CConcurrentProcessingVector::dbPopulateStatic (4 params)", "Function does not exist." );

  return QHash<QString, int>();
}

template <class T>
QHash<QString, int> CConcurrentProcessingVector<T>::dbPopulateStatic(
  const CConfigDatabase* dbConfig,
  const int startIdx,
  const int length,
  const int threadID,
  const QHash<QString, QVariant>& otherParams
) const {
  Q_UNUSED( dbConfig );
  Q_UNUSED( startIdx );
  Q_UNUSED( length );
  Q_UNUSED( threadID );
  Q_UNUSED( otherParams );
  Q_ASSERT_X( false, "CConcurrentProcessingVector::dbPopulateStatic (5 params)", "Function does not exist." );

  return QHash<QString, int>();
}


template <class T>
QHash<QString, int> CConcurrentProcessingStringHash<T>::resultsTemplate() const {
  Q_ASSERT_X( false, "CConcurrentProcessingStringHash:resultsTemplate", "Function does not exist." );

  return QHash<QString, int>();
}

template <class T>
QHash<QString, int> CConcurrentProcessingStringHash<T>::dbProcessStatic( const CConfigDatabase* dbConfig, const QList<QString>& keys, const int threadID ) const {
  Q_UNUSED( dbConfig );
  Q_UNUSED( keys );
  Q_UNUSED( threadID );
  Q_ASSERT_X( false, "CConcurrentProcessingStringHash::dbProcessStatic (3 params)", "Function does not exist." );

  return QHash<QString, int>();
}

template <class T>
QHash<QString, int> CConcurrentProcessingStringHash<T>::dbProcessStatic( const CConfigDatabase* dbConfig, const QList<QString>& keys, const int threadID, const QHash<QString, QVariant>& params ) const {
  Q_UNUSED( dbConfig );
  Q_UNUSED( keys );
  Q_UNUSED( threadID );
  Q_UNUSED( params );
  Q_ASSERT_X( false, "CConcurrentProcessingStringHash::dbProcessStatic (4 params)", "Function does not exist." );

  return QHash<QString, int>();
}

template <class T>
QHash<QString, int> CConcurrentProcessingStringHash<T>::dbPopulateStatic( const CConfigDatabase* dbConfig, const QList<QString>& keys, const int threadID ) const {
  Q_UNUSED( dbConfig );
  Q_UNUSED( keys );
  Q_UNUSED( threadID );
  Q_ASSERT_X( false, "CConcurrentProcessingStringHash::dbPopulateStatic (3 params)", "Function does not exist." );

  return QHash<QString, int>();
}

template <class T>
QHash<QString, int> CConcurrentProcessingStringHash<T>::dbPopulateStatic( const CConfigDatabase* dbConfig, const QList<QString>& keys, const int threadID, const QHash<QString, QVariant>& params ) const {
  Q_UNUSED( dbConfig );
  Q_UNUSED( keys );
  Q_UNUSED( threadID );
  Q_UNUSED( params );
  Q_ASSERT_X( false, "CConcurrentProcessingStringHash::dbPopulateStatic (4 params)", "Function does not exist." );

  return QHash<QString, int>();
}


template <class T>
QHash<QString, int> CConcurrentProcessingIntHash<T>::resultsTemplate() const {
  Q_ASSERT_X( false, "CConcurrentProcessingIntHash:resultsTemplate", "Function does not exist." );

  return QHash<QString, int>();
}

template <class T>
QHash<QString, int> CConcurrentProcessingIntHash<T>::dbProcessStatic( const CConfigDatabase* dbConfig, const QList<int>& keys, const int threadID ) const {
  Q_UNUSED( dbConfig );
  Q_UNUSED( keys );
  Q_UNUSED( threadID );
  Q_ASSERT_X( false, "CConcurrentProcessingIntHash::dbProcessStatic (3 params)", "Function does not exist." );

  return QHash<QString, int>();
}

template <class T>
QHash<QString, int> CConcurrentProcessingIntHash<T>::dbProcessStatic( const CConfigDatabase* dbConfig, const QList<int>& keys, const int threadID, const QHash<QString, QVariant>& params ) const {
  Q_UNUSED( dbConfig );
  Q_UNUSED( keys );
  Q_UNUSED( threadID );
  Q_UNUSED( params );
  Q_ASSERT_X( false, "CConcurrentProcessingIntHash::dbProcessStatic (4 params)", "Function does not exist." );

  return QHash<QString, int>();
}

template <class T>
QHash<QString, int> CConcurrentProcessingIntHash<T>::dbPopulateStatic( const CConfigDatabase* dbConfig, const QList<int>& keys, const int threadID ) const {
  Q_UNUSED( dbConfig );
  Q_UNUSED( keys );
  Q_UNUSED( threadID );
  Q_ASSERT_X( false, "CConcurrentProcessingIntHash::dbPopulateStatic (3 params)", "Function does not exist." );

  return QHash<QString, int>();
}


template <class T>
QHash<QString, int> CConcurrentProcessingIntHash<T>::dbPopulateStatic( const CConfigDatabase* dbConfig, const QList<int>& keys, const int threadID, const QHash<QString, QVariant>& params ) const {
  Q_UNUSED( dbConfig );
  Q_UNUSED( keys );
  Q_UNUSED( threadID );
  Q_UNUSED( params );
  Q_ASSERT_X( false, "CConcurrentProcessingIntHash::dbPopulateStatic (4 params)", "Function does not exist." );

  return QHash<QString, int>();
}
//-------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------
// Class CConcurrentProcessingManager
//-------------------------------------------------------------------------------------
template <class T>
CConcurrentProcessingManager<T>::CConcurrentProcessingManager(
    const QHash<QString, int>& resultsTemplate,
    const int initialMaxListSize /* = 0 */,
    const bool autoAdjustMaxListSize /* = true */
) {
  _results = resultsTemplate;

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
void CConcurrentProcessingManager<T>::mergeResults( QHash<QString, int> results2 ) {
  QList<QString> keys = _results.keys();

  foreach( QString key, keys ) {
    if( "returnCode" == key ) {
      _results.insert( key, ( _results.value( key ) | results2.value( key ) ) );
    }
    else {
      _results.insert( key, ( _results.value( key ) + results2.value( key ) ) );
    }
  }
}


template <class T>
void CConcurrentProcessingManager<T>::processList(
  CConcurrentProcessingList<T>* list,
  QHash<QString, int>(CConcurrentProcessingList<T>::*fn)( const CConfigDatabase*, const int ),
  const CConfigDatabase* dbConfig
) {
  if( 0 < list->count() ) {
    // If all threads are in use, wait until one finishes before starting another one.
    //--------------------------------------------------------------------------------
    checkThreadsForUse();

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
    checkForFinishedThreads();
  }
}

template <class T>
void CConcurrentProcessingManager<T>::processVector(
  CConcurrentProcessingVector<T>* vector,
  QHash<QString, int>(CConcurrentProcessingVector<T>::*fn)( const CConfigDatabase*, const int ),
  const CConfigDatabase* dbConfig
) {
  if( 0 < vector->count() ) {
    // If all threads are in use, wait until one finishes before starting another one.
    //--------------------------------------------------------------------------------
    checkThreadsForUse();

    // Put the new batch of data in the queue for processing
    //------------------------------------------------------
    //qDebug() << "Spinning up thread" << _threadID << "with list of size" << list->count();
    _runners.append(
      new CConcurrentProcessingRunner<T>(
        vector,
        QFuture< QHash<QString, int> >( QtConcurrent::run( vector, fn, dbConfig, _threadID ) )
      )
    );

    // See if any items in the queue are finished
    //-------------------------------------------
    checkForFinishedThreads();
  }
}


template <class T>
void CConcurrentProcessingManager<T>::processList(
  CConcurrentProcessingList<T>* list,
  QHash<QString, int>(CConcurrentProcessingList<T>::*fn)( const CConfigDatabase*, const int, const QHash<QString, QVariant>& ),
  const CConfigDatabase* dbConfig,
  const QHash<QString, QVariant>& params
) {
  if( 0 < list->count() ) {
    // If all threads are in use, wait until one finishes before starting another one.
    //--------------------------------------------------------------------------------
    checkThreadsForUse();

    // Put the new batch of data in the queue for processing
    //------------------------------------------------------
    //qDebug() << "Spinning up thread" << _threadID << "with list of size" << list->count();
    _runners.append(
      new CConcurrentProcessingRunner<T>(
        list,
        QFuture< QHash<QString, int> >( QtConcurrent::run( list, fn, dbConfig, _threadID, params ) )
      )
    );

    // See if any items in the queue are finished
    //-------------------------------------------
    checkForFinishedThreads();
  }
}


template <class T>
void CConcurrentProcessingManager<T>::processVector(
  CConcurrentProcessingVector<T>* vector,
  QHash<QString, int>(CConcurrentProcessingVector<T>::*fn)( const CConfigDatabase*, const int, const QHash<QString, QVariant>& ),
  const CConfigDatabase* dbConfig,
  const QHash<QString, QVariant>& params
) {
  if( 0 < vector->count() ) {
    // If all threads are in use, wait until one finishes before starting another one.
    //--------------------------------------------------------------------------------
    checkThreadsForUse();

    // Put the new batch of data in the queue for processing
    //------------------------------------------------------
    //qDebug() << "Spinning up thread" << _threadID << "with list of size" << list->count();
    _runners.append(
      new CConcurrentProcessingRunner<T>(
        vector,
        QFuture< QHash<QString, int> >( QtConcurrent::run( vector, fn, dbConfig, _threadID, params ) )
      )
    );

    // See if any items in the queue are finished
    //-------------------------------------------
    checkForFinishedThreads();
  }
}


template <class T>
void CConcurrentProcessingManager<T>::processStatic(
  const CConcurrentProcessingList<T>* list,
  QHash<QString, int>(CConcurrentProcessingList<T>::*fn)( const CConfigDatabase* dbConfig, const int startIdx, const int length, const int threadID ) const,
  const CConfigDatabase* dbConfig
) {
  if( 0 < list->count() ) {
    int listSize;
    int startIdx = 0;
    int length = 0;
    while( startIdx < list->count() ) {
      listSize = this->maxListSize();
      length = std::min( listSize, list->count() - startIdx );

      // If all threads are in use, wait until one finishes before starting another one.
      checkThreadsForUse();

      // Put the new batch of data in the queue for processing
      //qDebug() << "Spinning up thread" << _threadID << ", startIdx:" << startIdx << ", length: " << length;
      _runners.append(
        new CConcurrentProcessingRunner<T>(
          QFuture< QHash<QString, int> >( QtConcurrent::run( list, fn, dbConfig, startIdx, length, _threadID ) )
        )
      );

      // See if any items in the queue are finished
      checkForFinishedThreads();

      startIdx = startIdx + length;
    }

    this->waitForFinished();
  }
}


template <class T>
void CConcurrentProcessingManager<T>::processStatic(
  const CConcurrentProcessingList<T>* list,
  QHash<QString, int>(CConcurrentProcessingList<T>::*fn)( const CConfigDatabase* dbConfig, const int startIdx, const int length, const int threadID, const QHash<QString, QVariant>& params ) const,
  const CConfigDatabase* dbConfig,
  const QHash<QString, QVariant>& params
) {
  if( 0 < list->count() ) {
    int listSize;
    int startIdx = 0;
    int length = 0;
    while( startIdx < list.count() ) {
      listSize = this->maxListSize();
      length = std::min( listSize, list.count() - startIdx );

      // If all threads are in use, wait until one finishes before starting another one.
      checkThreadsForUse();

      // Put the new batch of data in the queue for processing
      //qDebug() << "Spinning up thread" << _threadID << "with list of size" << list->count();
      _runners.append(
        new CConcurrentProcessingRunner<T>(
          QFuture< QHash<QString, int> >( QtConcurrent::run( list, fn, dbConfig, startIdx, length, _threadID, params ) )
        )
      );

      // See if any items in the queue are finished
      checkForFinishedThreads();

      startIdx = startIdx + length;
    }

    waitForFinished();
  }
}


template <class T>
void CConcurrentProcessingManager<T>::processStatic(
  const CConcurrentProcessingVector<T>* vec,
  QHash<QString, int>(CConcurrentProcessingVector<T>::*fn)( const CConfigDatabase*, const int, const int, const int ) const,
  const CConfigDatabase* dbConfig
) {
  if( 0 < vec->count() ) {
    int listSize;
    int startIdx = 0;
    int length = 0;
    while( startIdx < vec->count() ) {
      listSize = this->maxListSize();
      length = std::min( listSize, vec->count() - startIdx );

      // If all threads are in use, wait until one finishes before starting another one.
      checkThreadsForUse();

      // Put the new batch of data in the queue for processing
      //qDebug() << "Spinning up thread" << _threadID << "with list of size" << list->count();
      _runners.append(
        new CConcurrentProcessingRunner<T>(
          QFuture< QHash<QString, int> >( QtConcurrent::run( vec, fn, dbConfig, startIdx, length, _threadID ) )
        )
      );

      // See if any items in the queue are finished
      checkForFinishedThreads();

      startIdx = startIdx + length;
    }

    waitForFinished();
  }
}


template <class T>
void CConcurrentProcessingManager<T>::processStatic(
  const CConcurrentProcessingVector<T>* vec,
  QHash<QString, int>(CConcurrentProcessingVector<T>::*fn)( const CConfigDatabase*, const int, const int, const int, const QHash<QString, QVariant>& ) const,
  const CConfigDatabase* dbConfig,
  const QHash<QString, QVariant>& params
) {
  if( 0 < vec->count() ) {
    int listSize;
    int startIdx = 0;
    int length = 0;
    while( startIdx < vec.count() ) {
      listSize = this->maxListSize();
      length = std::min( listSize, vec.count() - startIdx );

      // If all threads are in use, wait until one finishes before starting another one.
      checkThreadsForUse();

      // Put the new batch of data in the queue for processing
      //qDebug() << "Spinning up thread" << _threadID << "with list of size" << list->count();
      _runners.append(
        new CConcurrentProcessingRunner<T>(
          QFuture< QHash<QString, int> >( QtConcurrent::run( vec, fn, dbConfig, startIdx, length, _threadID, params ) )
        )
      );

      // See if any items in the queue are finished
      checkForFinishedThreads();

      startIdx = startIdx + length;
    }

    waitForFinished();
  }
}


template <class T>
void CConcurrentProcessingManager<T>::processStatic(
  const CConcurrentProcessingStringHash<T>* hash,
  QHash<QString, int>(CConcurrentProcessingStringHash<T>::*fn)( const CConfigDatabase* dbConfig, const QList<QString>& keys, const int threadID ) const,
  const CConfigDatabase* dbConfig
) {
  if( 0 < hash->count() ) {
    QList<QString> masterKeys = hash->keys();

    int listSize;
    int startIdx = 0;
    int length = 0;
    while( startIdx < masterKeys.count() ) {
      listSize = this->maxListSize();
      length = std::min( listSize, masterKeys.count() - startIdx );

      // If all threads are in use, wait until one finishes before starting another one.
      checkThreadsForUse();

      // Put the new batch of data in the queue for processing
      //qDebug() << "Spinning up thread" << _threadID << "with list of size" << list->count();
      _runners.append(
        new CConcurrentProcessingRunner<T>(
          QFuture< QHash<QString, int> >( QtConcurrent::run( hash, fn, dbConfig, masterKeys.mid( startIdx, length ), _threadID ) )
        )
      );

      // See if any items in the queue are finished
      checkForFinishedThreads();

      startIdx = startIdx + listSize;
    }

    waitForFinished();
  }
}


template <class T>
void CConcurrentProcessingManager<T>::processStatic(
  const CConcurrentProcessingStringHash<T>* hash,
  QHash<QString, int>(CConcurrentProcessingStringHash<T>::*fn)( const CConfigDatabase*, const QList<QString>&, const int, const QHash<QString, QVariant>& ) const,
  const CConfigDatabase* dbConfig,
  const QHash<QString, QVariant>& params
) {
  if( 0 < hash->count() ) {
    QList<QString> masterKeys = hash->keys();

    int listSize;
    int startIdx = 0;
    int length = 0;
    while( startIdx < masterKeys.count() ) {
      listSize = this->maxListSize();
      length = std::min( listSize, masterKeys.count() - startIdx );

      // If all threads are in use, wait until one finishes before starting another one.
      checkThreadsForUse();

      // Put the new batch of data in the queue for processing
      //qDebug() << "Spinning up thread" << _threadID << "with list of size" << list->count();
      _runners.append(
        new CConcurrentProcessingRunner<T>(
          QFuture< QHash<QString, int> >( QtConcurrent::run( hash, fn, dbConfig, masterKeys.mid( startIdx, length ), _threadID, params ) )
        )
      );

      // See if any items in the queue are finished
      checkForFinishedThreads();

      startIdx = startIdx + listSize;
    }

    waitForFinished();
  }
}


template <class T>
void CConcurrentProcessingManager<T>::processStatic(
  const CConcurrentProcessingIntHash<T>* hash,
  QHash<QString, int>(CConcurrentProcessingIntHash<T>::*fn)( const CConfigDatabase*, const QList<int>&, const int ) const,
  const CConfigDatabase* dbConfig
) {
  if( 0 < hash->count() ) {
    QList<int> masterKeys = hash->keys();

    int listSize;
    int startIdx = 0;
    int length = 0;
    while( startIdx < masterKeys.count() ) {
      listSize = this->maxListSize();
      length = std::min( listSize, masterKeys.count() - startIdx );

      // If all threads are in use, wait until one finishes before starting another one.
      checkThreadsForUse();

      // Put the new batch of data in the queue for processing
      //qDebug() << "Spinning up thread" << _threadID << "with list of size" << list->count();
      _runners.append(
        new CConcurrentProcessingRunner<T>(
          QFuture< QHash<QString, int> >( QtConcurrent::run( hash, fn, dbConfig, masterKeys.mid( startIdx, length ), _threadID ) )
        )
      );

      // See if any items in the queue are finished
      checkForFinishedThreads();

      startIdx = startIdx + listSize;
    }

    waitForFinished();
  }
}

template <class T>
void CConcurrentProcessingManager<T>::processStatic(
  const CConcurrentProcessingIntHash<T>* hash,
  QHash<QString, int>(CConcurrentProcessingIntHash<T>::*fn)( const CConfigDatabase*, const QList<int>&, const int, const QHash<QString, QVariant>& ) const,
  const CConfigDatabase* dbConfig,
  const QHash<QString, QVariant>& params
) {
  if( 0 < hash->count() ) {
    QList<QString> masterKeys = hash->keys();

    int listSize;
    int startIdx = 0;
    int length = 0;
    while( startIdx < masterKeys.count() ) {
      listSize = this->maxListSize();
      length = std::min( listSize, masterKeys.count() - startIdx );

      // If all threads are in use, wait until one finishes before starting another one.
      checkThreadsForUse();

      // Put the new batch of data in the queue for processing
      //qDebug() << "Spinning up thread" << _threadID << "with list of size" << list->count();
      _runners.append(
        new CConcurrentProcessingRunner<T>(
          QFuture< QHash<QString, int> >( QtConcurrent::run( hash, fn, dbConfig, masterKeys.mid( startIdx, length ), _threadID, params ) )
        )
      );

      // See if any items in the queue are finished
      checkForFinishedThreads();

      startIdx = startIdx + listSize;
    }

    waitForFinished();
  }
}


template <class T>
void CConcurrentProcessingManager<T>::checkThreadsForUse() {
  ++_threadID;

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
}


template <class T>
void CConcurrentProcessingManager<T>::checkForFinishedThreads() {
  QList<int> runnersToDelete;
  for( int runnerIdx = 0; runnerIdx < _runners.count(); ++runnerIdx ) {
    if( _runners[runnerIdx]->isFinished() ) {
      if( ReturnCode::SUCCESS != _runners.at(runnerIdx)->result().value( QStringLiteral("ReturnCode") ) ) {
        logMsg( QStringLiteral( "Error encountered in runner %1: %2" ).arg( runnerIdx ).arg( ReturnCode::codeDescr( _runners.at(runnerIdx)->result().value( QStringLiteral("ReturnCode") ) ) ) );
      }   
      mergeResults( _runners.at(runnerIdx)->result() );

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
    mergeResults( _runners.at(runnerIdx)->result() );
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
  _vector = nullptr;
  _future = f;
  _ownsContainer = true;
}


template <class T>
CConcurrentProcessingRunner<T>::CConcurrentProcessingRunner( CConcurrentProcessingVector<T>* vector, const QFuture< QHash<QString, int> >& f ) {
  _timer.start();
  _timeInMsec = 0;
  _hasFinished = false;
  _list = nullptr;
  _vector = vector;
  _future = f;
  _ownsContainer = true;
}


template <class T>
CConcurrentProcessingRunner<T>::CConcurrentProcessingRunner( const QFuture<QHash<QString, int> >& f ) {
  _timer.start();
  _timeInMsec = 0;
  _hasFinished = false;
  _list = nullptr;
  _vector = nullptr;
  _future = f;
  _ownsContainer = false;
}


template <class T>
CConcurrentProcessingRunner<T>::~CConcurrentProcessingRunner() {
  if( !_hasFinished ) {
    qDb() << "CConcurrentRunner::~CConcurrentRunner(): _hasFinished is false.  Is this really what you want to do?";

    if( _ownsContainer ) {
      delete _list;
      delete _vector;
    }
  }
  else if( _ownsContainer ) {
    if( ( nullptr != _list ) || ( nullptr != _vector ) ) {
      qDb() << "CConcurrentRunner::~CConcurrentRunner(): Deleting container that's not null.  Did you forget to clean up?";
      delete _list;
      delete _vector;
    }
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
  if( _ownsContainer ) {
    if( nullptr != _list ) {
      delete _list;
      _list = nullptr;
    }
    if( nullptr != _vector ) {
      delete _vector;
      _vector = nullptr;
    }
  }
}


template <class T>
QHash<QString, int> CConcurrentProcessingRunner<T>::result() const {
  return _future.result();
}
//-------------------------------------------------------------------------------------

