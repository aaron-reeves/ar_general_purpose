/*
cconcurrentrunner.h/cpp
-----------------------
Begin: 2019-06-18
Author: Aaron Reeves <aaron.reeves@sruc.ac.uk>
---------------------------------------------------
Copyright (C) 2019 - 2020 Scotland's Rural College (SRUC)

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#ifndef CCONCURRENTRUNNER_H
#define CCONCURRENTRUNNER_H

#include <QtCore>
#include <QtConcurrent>

#include <ar_general_purpose/returncodes.h>
#include <epic_general_purpose/cepicconfigfile.h>

class CConcurrencyManager {
  public:
    CConcurrencyManager();
    virtual ~CConcurrencyManager();

    void waitForOneToFinish();

    // Override this function to do anything useful.
    virtual QHash<QString, int> populateDatabase( const CConfigDatabase& cfdb, const int dataSourceID, const bool insertRecords );

  private:
    Q_DISABLE_COPY( CConcurrencyManager )
};


class CConcurrentRunner {
  public:
    CConcurrentRunner( CConcurrencyManager* list, const QFuture<QHash<QString, int> >& f );
    CConcurrentRunner( const QFuture<QHash<QString, int> >& f );
    ~CConcurrentRunner();

    qint64 runtime() const; // in milliseconds. -1 if process hasn't yet ended.

    void waitForFinished();
    bool finished() const;
    bool isFinished();
    QHash<QString, int> result() const;

    static int adjustMaxListSize(
      const bool threadsFull,
      const QList<int>& maxListSize,
      const int idealThreadCount,
      const QList<int>& threadsInUse,
      const QList<bool>& backlog
    );

    static void writeUsage(
      const QList<int>& maxListSize,
      const QList<bool>& backlog,
      const QList<qint64>& waitTime,
      const QList<int>& threadsInUse
    );

  protected:
    void cleanup();

    QFuture< QHash<QString, int> > _future;
    CConcurrencyManager* _list;
    bool _hasFinished;
    QElapsedTimer _timer;
    qint64 _timeInMsec;

  private:
    Q_DISABLE_COPY( CConcurrentRunner )
};


class CConcurrentContainer {
  public:
    //CConcurrentContainer() { /* Nothing to do here */ }
    //CConcurrentContainer( const CConcurrentContainer& other ) { /* Nothing to do here */ }
    //CConcurrentContainer& operator=( const CConcurrentContainer& other ) { /* Nothing to do here */ }
    //~CConcurrentContainer() { /* Nothing to do here */ }

    virtual QHash<QString, int> resultsTemplate() const {
      QHash<QString, int> result;

      result.insert( QStringLiteral("returnCode"), ReturnCode::SUCCESS );
      result.insert( QStringLiteral("totalRecords"), 0 );
      result.insert( QStringLiteral("totalProcessed"), 0 );
      result.insert( QStringLiteral("insertFailures"), 0 );

      return result;
    }

    virtual QHash<QString, int> mergeResults( const QHash<QString, int>& results1, const QHash<QString, int>& results2 ) const {
      QHash<QString, int> results;

      QList<QString> keys = results1.keys();

      foreach( QString key, keys ) {
        if( "returnCode" == key ) {
          results.insert( key, ( results1.value( key ) | results2.value( key ) ) );
        }
        else {
          results.insert( key, ( results1.value( key ) + results2.value( key ) ) );
        }
      }

      return results;
    }
};


template <class T>
class CConcurrentVector : public CConcurrentContainer, public QVector<T> {
  public:
    CConcurrentVector() : CConcurrentContainer(), QVector<T>() { /* Nothing else to do here */ }
    CConcurrentVector( const CConcurrentVector& other ) : CConcurrentContainer(), QVector<T>( other ) { assign( other ); }
    CConcurrentVector& operator= ( const CConcurrentVector& other ) { CConcurrentContainer::operator=( other ); QVector<T>::operator=( other );  assign( other ); return *this; }
    
    virtual ~CConcurrentVector() { /* Nothing else to do here */ }
      
    virtual QHash<QString, int> populateDatabase(
      CConfigDatabase cfdb,
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

  protected:
    void assign( const CConcurrentVector& other ) { Q_UNUSED( other ); }
};


template <class T>
class CConcurrentStringHash : public CConcurrentContainer, public QHash<QString, T> {
  public:
    CConcurrentStringHash() : CConcurrentContainer(), QHash<QString, T>() { /* Nothing else to do here */ }
    CConcurrentStringHash( const CConcurrentStringHash& other ) : CConcurrentContainer(), QHash<QString, T>( other ) { assign( other ); }
    CConcurrentStringHash& operator= ( const CConcurrentStringHash& other ) { CConcurrentContainer::operator=( other ); QHash<QString, T>::operator=( other );  assign( other ); return *this; }

    virtual ~CConcurrentStringHash() { /* Nothing else to do here */ }

    virtual QHash<QString, int> populateDatabase(
      CConfigDatabase cfdb,
      const QList<QString>& list,
      const int threadID,
      const QHash<QString, QVariant>& otherParams
    ) const {
      Q_UNUSED( cfdb );
      Q_UNUSED( list );
      Q_UNUSED( threadID );
      Q_UNUSED( otherParams );
      return QHash<QString, int>();
    }

  protected:
    void assign( const CConcurrentStringHash& other ) { Q_UNUSED( other ); }
};

template <class T>
class CConcurrentIntHash : public CConcurrentContainer, public QHash<int, T> {
  public:
    CConcurrentIntHash() : CConcurrentContainer(), QHash<int, T>() { /* Nothing else to do here */ }
    CConcurrentIntHash( const CConcurrentIntHash& other ) : CConcurrentContainer(), QHash<int, T>( other ) { assign( other ); }
    CConcurrentIntHash& operator= ( const CConcurrentIntHash& other ) { CConcurrentContainer::operator=( other ); QHash<int, T>::operator=( other );  assign( other ); return *this; }

    virtual ~CConcurrentIntHash() { /* Nothing else to do here */ }

    virtual QHash<QString, int> populateDatabase(
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

  protected:
    void assign( const CConcurrentIntHash& other ) { Q_UNUSED( other ); }
};


template <class T>
class CConcurrentProcessor : public QList<CConcurrentRunner*> {
  public:
    CConcurrentProcessor() : QList<CConcurrentRunner*>() { /* Nothing else to do here */ }
    ~CConcurrentProcessor() {
      while( !this->isEmpty() ) {
        delete this->takeFirst();
      }
    }
      
    QHash<QString, int> populateDatabase(const CConcurrentVector<T>& v, const CConfigDatabase& cfdb, const QHash<QString, QVariant>& otherParams ) {
      QHash<QString, int> results = v.resultsTemplate();

      int nThreads = ( QThread::idealThreadCount() - 2 ); // Reserve one thread for management, and one for the last batch
      int nItemsPerThread = this->count() / nThreads;

      int threadID = 0;
      int startIdx = 0;
      int endIdx = 0;

      for( int i = 0; i < nThreads; ++i ) {
        endIdx = startIdx + nItemsPerThread;
        // Put the new batch of data in the queue for processing
        qDebug() << "Spinning up thread" << threadID << "with list of size" << nItemsPerThread;
        this->append(
          new CConcurrentRunner(
            QFuture< QHash<QString, int> >( QtConcurrent::run( v, &CConcurrentVector<T>::populateDatabase, cfdb.parameters( threadID ), startIdx, endIdx, threadID, otherParams ) )
          )
        );
        ++threadID;
        startIdx = endIdx;
      }

      // Don't forget the last batch.
      ++threadID;
      this->append(
        new CConcurrentRunner(
          QFuture< QHash<QString, int> >( QtConcurrent::run( v, &CConcurrentVector<T>::populateDatabase, cfdb.parameters( threadID ), startIdx, this->count(), threadID, otherParams ) )
        )
      );

      this->waitForFinished();

      for( int i = 0; i < this->count(); ++i ) {
        results = v.mergeResults( results, this->at(i)->result() );
      }

      return results;
    }
  
    QHash<QString, int> populateDatabase(const CConcurrentStringHash<T>& h, const CConfigDatabase& cfdb, const QHash<QString, QVariant>& otherParams ) {
      QHash<QString, int> results = h.resultsTemplate();

      int nThreads = ( QThread::idealThreadCount() - 2 ); // Reserve one thread for management, and one for the last batch
      int nItemsPerThread = this->count() / nThreads;

      int threadID = 0;
      int startIdx = 0;
      QList<QString> list;

      QList<QString> keys = h.keys();

      for( int i = 0; i < nThreads; ++i ) {
        list = keys.mid( startIdx, nItemsPerThread );

        // Put the new batch of data in the queue for processing
        qDebug() << "Spinning up thread" << threadID << "with list of size" << nItemsPerThread;
        this->append(
          new CConcurrentRunner(
            QFuture< QHash<QString, int> >( QtConcurrent::run( h, &CConcurrentStringHash<T>::populateDatabase, cfdb.parameters( threadID ), list, threadID, otherParams ) )
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
          QFuture< QHash<QString, int> >( QtConcurrent::run( h, &CConcurrentStringHash<T>::populateDatabase, cfdb.parameters( threadID ), list, threadID, otherParams ) )
        )
      );

      this->waitForFinished();

      for( int i = 0; i < this->count(); ++i ) {
        results = h.mergeResults( results, this->at(i)->result() );
      }

      return results;
    }

    QHash<QString, int> populateDatabase(const CConcurrentIntHash<T>& h, const CConfigDatabase& cfdb, const QHash<QString, QVariant>& otherParams ) {
      QHash<QString, int> results = h.resultsTemplate();

      int nThreads = ( QThread::idealThreadCount() - 2 ); // Reserve one thread for management, and one for the last batch
      int nItemsPerThread = this->count() / nThreads;

      int threadID = 0;
      int startIdx = 0;
      QList<int> list;

      QList<int> keys = h.keys();

      for( int i = 0; i < nThreads; ++i ) {
        list = keys.mid( startIdx, nItemsPerThread );

        // Put the new batch of data in the queue for processing
        qDebug() << "Spinning up thread" << threadID << "with list of size" << nItemsPerThread;
        this->append(
          new CConcurrentRunner(
            QFuture< QHash<QString, int> >( QtConcurrent::run( h, &CConcurrentIntHash<T>::populateDatabase, cfdb.parameters( threadID ), list, threadID, otherParams ) )
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
          QFuture< QHash<QString, int> >( QtConcurrent::run( h, &CConcurrentIntHash<T>::populateDatabase, cfdb.parameters( threadID ), list, threadID, otherParams ) )
        )
      );

      this->waitForFinished();

      for( int i = 0; i < this->count(); ++i ) {
        results = h.mergeResults( results, this->at(i)->result() );
      }

      return results;
    }

  protected:
    void waitForFinished() {
      for( int i = 0; i < this->count(); ++i ) {
        (*this)[i]->waitForFinished();
      }
    }

  private:
    Q_DISABLE_COPY( CConcurrentProcessor )
};

#endif // CCONCURRENTRUNNER_H
