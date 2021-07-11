/*
concurrentcontainers.h/tpp
--------------------------
Begin: 2019-06-18
Author: Aaron Reeves <aaron.reeves@sruc.ac.uk>
---------------------------------------------------
Copyright (C) 2019 - 2021 Scotland's Rural College (SRUC)

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#include "concurrentcontainers.h" // For convenience only: this would be a circular reference without guards.


//-------------------------------------------------------------------------------------------------
// CConcurrentVector
//-------------------------------------------------------------------------------------------------
  template <class T>
  CConcurrentVector<T>& CConcurrentVector<T>::operator= ( const CConcurrentVector<T>& other ) {
    CConcurrentContainer::operator=( other ); 
    QVector<T>::operator=( other );  
    assign( other ); 
    return *this; 
  }


  template <class T>
  QHash<QString, int> CConcurrentVector<T>::populateDatabase(
    const CConfigDatabase& cfdb,
    const int startIdx,
    const int endIdx,
    const int threadID,
    const QHash<QString, QVariant>& otherParams
  ) const {
    Q_UNUSED( cfdb );
    Q_UNUSED( startIdx );
    Q_UNUSED( endIdx );
    Q_UNUSED( threadID );
    Q_UNUSED( otherParams );
    return QHash<QString, int>();
  }
  
  
  template <typename T, typename Class>
  QHash<QString, int> CConcurrentProcessor<T, Class>::populateDatabase(
    const CConcurrentVector<T>* v,
    QHash<QString, int> (Class::*fn)( const CConfigDatabase&, const int, const int, const int, const QHash<QString, QVariant>& ) const,
    const CConfigDatabase& cfdb,
    const QHash<QString, QVariant>& otherParams
  ) {
    QHash<QString, int> results = v->resultsTemplate();

    int nThreads = ( QThread::idealThreadCount() - 2 ); // Reserve one thread for management, and one for the last batch
    int nItemsPerThread = v->count() / nThreads;

    qDebug() << v->count() << nThreads << nItemsPerThread;

    int threadID = 0;
    int startIdx = 0;
    int endIdx = 0;

    for( int i = 0; i < nThreads; ++i ) {
      endIdx = startIdx + nItemsPerThread;
      // Put the new batch of data in the queue for processing
      qDebug() << "Spinning up thread" << threadID << "with list of size" << nItemsPerThread;
      this->append(
        new CConcurrentRunner(
          QFuture< QHash<QString, int> >( QtConcurrent::run( *v, &CConcurrentVector<T>::populateDatabase, cfdb.parameters( threadID ), startIdx, endIdx, threadID, otherParams ) ) // Compiles but doesn't work
        )
      );
      ++threadID;
      startIdx = endIdx;
    }

    // Don't forget the last batch.
    ++threadID;
    this->append(
      new CConcurrentRunner(
        QFuture< QHash<QString, int> >( QtConcurrent::run( *v, &CConcurrentVector<T>::populateDatabase, cfdb.parameters( threadID ), startIdx, v->count(), threadID, otherParams ) ) // Compiles but doesn't work
      )
    );

    this->waitForFinished();

    for( int i = 0; i < this->count(); ++i ) {
      results = v->mergeResults( results, this->at(i)->result() );
    }

    return results;
  }
//-------------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------------------
// CConcurrentStringHash
//-------------------------------------------------------------------------------------------------
  template <class T>
  CConcurrentStringHash<T>& CConcurrentStringHash<T>::operator= ( const CConcurrentStringHash<T>& other ) {
    CConcurrentContainer::operator=( other );
    QHash<QString, T>::operator=( other );
    assign( other ); return *this;
  }


  template <class T>
  QHash<QString, int> CConcurrentStringHash<T>::populateDatabase(
    const CConfigDatabase& cfdb,
    const QList<QString>& list,
    const int threadID,
    const QHash<QString, QVariant>& otherParams
  ) const {
    qDebug() << "++++++++++++++++ CConcurrentStringHash<T>::populateDatabase()";

    Q_UNUSED( cfdb );
    Q_UNUSED( list );
    Q_UNUSED( threadID );
    Q_UNUSED( otherParams );
    return QHash<QString, int>();
  }
  
  
  template <typename T, typename Class>
  QHash<QString, int> CConcurrentProcessor<T, Class>::populateDatabase(
    const CConcurrentStringHash<T>* h,
    QHash<QString, int> (Class::*fn)( const CConfigDatabase&, const QList<QString>&, const int, const QHash<QString, QVariant>& ) const,
    const CConfigDatabase& cfdb,
    const QHash<QString, QVariant>& otherParams
  ) {
    QHash<QString, int> results = h->resultsTemplate();

    int nThreads = ( QThread::idealThreadCount() - 2 ); // Reserve one thread for management, and one for the last batch
    int nItemsPerThread = h->count() / nThreads;

    qDebug() << h->count() << nThreads << nItemsPerThread;

    int threadID = 0;
    int startIdx = 0;
    QList<QString> list;

    QList<QString> keys = h->keys();

    for( int i = 0; i < nThreads; ++i ) {
      list = keys.mid( startIdx, nItemsPerThread );

      // Put the new batch of data in the queue for processing
      qDebug() << "Spinning up thread" << threadID << "with list of size" << nItemsPerThread;
      this->append(
        new CConcurrentRunner(
          QFuture< QHash<QString, int> >( QtConcurrent::run( h, &CConcurrentStringHash<T>::populateDatabase, cfdb.parameters( threadID ), list, threadID, otherParams ) )  // Compiles but doesn't work
        )
      );
      ++threadID;
      startIdx = startIdx + nItemsPerThread;
    }

    // Don't forget the last batch.
    ++threadID;
    list = keys.mid( startIdx, -1 );

    this->append(
      new CConcurrentRunner(
        QFuture< QHash<QString, int> >( QtConcurrent::run( h, &CConcurrentStringHash<T>::populateDatabase, cfdb.parameters( threadID ), list, threadID, otherParams ) ) // Compiles but doesn't work
      )
    );

    this->waitForFinished();

    for( int i = 0; i < this->count(); ++i ) {
      results = h->mergeResults( results, this->at(i)->result() );
    }

    return results;
  }
//-------------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------------------
// CConcurrentIntHash
//-------------------------------------------------------------------------------------------------
  template <class T>
  CConcurrentIntHash<T>& CConcurrentIntHash<T>::operator= ( const CConcurrentIntHash<T>& other ) {
    CConcurrentContainer::operator=( other ); 
    QHash<int, T>::operator=( other );
    assign( other ); return *this;
  }

  template <class T>
  QHash<QString, int> CConcurrentIntHash<T>::populateDatabase(
    CConfigDatabase cfdb,
    const QList<int>& list,
    const int threadID,
    const QHash<QString, QVariant>& otherParams
  ) const {
    Q_UNUSED( cfdb );
    Q_UNUSED( list );
    Q_UNUSED( threadID );
    Q_UNUSED( otherParams );
    return QHash<QString, int>();
  }
  
  template <typename T, typename Class>
  QHash<QString, int> CConcurrentProcessor<T, Class>::populateDatabase(
    const CConcurrentIntHash<T>* h,
    QHash<QString, int> (Class::*fn)(CConfigDatabase, const QList<int>&, const int, const QHash<QString, QVariant>&) const,
    const CConfigDatabase& cfdb,
    const QHash<QString, QVariant>& otherParams
  ) {
    QHash<QString, int> results = h->resultsTemplate();

    int nThreads = ( QThread::idealThreadCount() - 2 ); // Reserve one thread for management, and one for the last batch
    int nItemsPerThread = h->count() / nThreads;

    qDebug() << h->count() << nThreads << nItemsPerThread;

    int threadID = 0;
    int startIdx = 0;
    QList<int> list;

    QList<int> keys = h->keys();

    for( int i = 0; i < nThreads; ++i ) {
      list = keys.mid( startIdx, nItemsPerThread );

      // Put the new batch of data in the queue for processing
      qDebug() << "Spinning up thread" << threadID << "with list of size" << nItemsPerThread;
      this->append(
        new CConcurrentRunner(
          QFuture< QHash<QString, int> >( QtConcurrent::run( h, &CConcurrentIntHash<T>::populateDatabase, cfdb.parameters( threadID ), list, threadID, otherParams ) ) // compiles but doesn't work
          //QFuture< QHash<QString, int> >( QtConcurrent::run( h, fn, cfdb.parameters( threadID ), list, threadID, otherParams ) ) // Doesn't compile
        )
      );
      ++threadID;
      startIdx = startIdx + nItemsPerThread;
    }

    // Don't forget the last batch.
    ++threadID;
    list = keys.mid( startIdx, -1 );

    this->append(
      new CConcurrentRunner(
        QFuture< QHash<QString, int> >( QtConcurrent::run( h, &CConcurrentIntHash<T>::populateDatabase, cfdb.parameters( threadID ), list, threadID, otherParams ) ) // compiles but doesn't work
      )
    );

    this->waitForFinished();

    for( int i = 0; i < this->count(); ++i ) {
      results = h->mergeResults( results, this->at(i)->result() );
    }

    return results;
  }
//-------------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------------------
// CConcurrentProcessor
//-------------------------------------------------------------------------------------------------
  template <typename T, typename Class>
  CConcurrentProcessor<T, Class>::~CConcurrentProcessor() {
    while( !this->isEmpty() ) {
      delete this->takeFirst();
    }
  }
  
  
  template <typename T, typename Class>
  void CConcurrentProcessor<T, Class>::waitForFinished() {
    for( int i = 0; i < this->count(); ++i ) {
      (*this)[i]->waitForFinished();
    }
  }
//-------------------------------------------------------------------------------------------------

