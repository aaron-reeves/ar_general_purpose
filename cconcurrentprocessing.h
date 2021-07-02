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

#ifndef CCONCURRENTPROCESSING_H
#define CCONCURRENTPROCESSING_H

#include <QtCore>
#include <QtConcurrent>

#include <ar_general_purpose/log.h>
#include <ar_general_purpose/qcout.h>
#include <ar_general_purpose/returncodes.h>

#include <epic_general_purpose/cepicconfigfile.h>

#include <ar_general_purpose/cdatabaseresults.h>


// The basic base class containers
//--------------------------------
template <class T>
class CConcurrentProcessingList : public QList<T> {
  public:
    virtual ~CConcurrentProcessingList() { /* Nothing special to do here */ }

    // These functions must be overridden to do anything useful
    virtual QHash<QString, int> resultsTemplate() const;
    virtual QHash<QString, int> dbProcessDynamic( const CConfigDatabase& dbConfig, const int threadID, const QHash<QString, QVariant>& params );
    virtual QHash<QString, int> dbProcessDynamic( const CConfigDatabase& dbConfig, const int threadID );
    virtual QHash<QString, int> dbPopulateDynamic( const CConfigDatabase& dbConfig, const int threadID );

    virtual QHash<QString, int> dbPopulateStatic(
      const CConfigDatabase& dbConfig,
      const int startIdx,
      const int endIdx,
      const int threadID
    ) const;

    virtual QHash<QString, int> dbPopulateStatic(
      const CConfigDatabase& dbConfig,
      const int startIdx,
      const int endIdx,
      const int threadID,
      const QHash<QString, QVariant>& otherParams
    ) const;
};


template <class T>
class CConcurrentProcessingVector : public QVector<T> {
  public:
    virtual ~CConcurrentProcessingVector() { /* Nothing special to do here */ }

    // These functions must be overridden to do anything useful
    virtual QHash<QString, int> resultsTemplate() const;
    virtual QHash<QString, int> dbProcessDynamic( const CConfigDatabase& dbConfig, const int threadID, const QHash<QString, QVariant>& params );
    virtual QHash<QString, int> dbProcessDynamic( const CConfigDatabase& dbConfig, const int threadID );
    virtual QHash<QString, int> dbPopulateDynamic( const CConfigDatabase& dbConfig, const int threadID );

    virtual QHash<QString, int> dbPopulateStatic(
      const CConfigDatabase& dbConfig,
      const int startIdx,
      const int endIdx,
      const int threadID
    ) const;

    virtual QHash<QString, int> dbPopulateStatic(
      const CConfigDatabase& dbConfig,
      const int startIdx,
      const int endIdx,
      const int threadID,
      const QHash<QString, QVariant>& otherParams
    ) const;
};


template <class T>
class CConcurrentProcessingStringHash : public QHash<QString, T> {
  public:
    virtual ~CConcurrentProcessingStringHash() { /* Nothing special to do here */ }

    // These functions must be overridden to do anything useful
    virtual QHash<QString, int> resultsTemplate() const;
    virtual QHash<QString, int> dbProcessStatic( const CConfigDatabase& dbConfig, const QList<QString>& keys, const int threadID, const QHash<QString, QVariant>& params ) const;
    virtual QHash<QString, int> dbProcessStatic( const CConfigDatabase& dbConfig, const QList<QString>& keys, const int threadID ) const;
    virtual QHash<QString, int> dbPopulateStatic( const CConfigDatabase& dbConfig, const QList<QString>& keys, const int threadID, const QHash<QString, QVariant>& params ) const;
};


template <class T>
class CConcurrentProcessingIntHash : public QHash<int, T> {
  public:
    virtual ~CConcurrentProcessingIntHash() { /* Nothing special to do here */ }

    // These functions must be overridden to do anything useful
    virtual QHash<QString, int> resultsTemplate() const;
    virtual QHash<QString, int> dbProcessStatic( const CConfigDatabase& dbConfig, const QList<int>& keys, const int threadID, const QHash<QString, QVariant>& params ) const;
    virtual QHash<QString, int> dbProcessStatic( const CConfigDatabase& dbConfig, const QList<int>& keys, const int threadID ) const;
    virtual QHash<QString, int> dbPopulateStatic( const CConfigDatabase& dbConfig, const QList<int>& keys, const int threadID, const QHash<QString, QVariant>& params ) const;
};


// A forward declaration for the sake of CConcurrentProcessingManager
template <class T>
class CConcurrentProcessingRunner;


template <class T>
class CConcurrentProcessingManager {
  public:
    CConcurrentProcessingManager( const QHash<QString, int>& resultsTemplate, const int initialMaxListSize = 0, const bool autoAdjustMaxListSize = true );

    void processList(
      CConcurrentProcessingList<T>* list,
      QHash<QString, int>(CConcurrentProcessingList<T>::*fn)( const CConfigDatabase&, const int ),
      const CConfigDatabase& dbConfig
    );

    void processList(
      CConcurrentProcessingList<T>* list,
      QHash<QString, int>(CConcurrentProcessingList<T>::*fn)( const CConfigDatabase&, const int, const QHash<QString, QVariant>& ),
      const CConfigDatabase& dbConfig,
      const QHash<QString, QVariant>& params
    );

    void processVector(
      CConcurrentProcessingVector<T>* vector,
      QHash<QString, int>(CConcurrentProcessingVector<T>::*fn)( const CConfigDatabase&, const int ),
      const CConfigDatabase& dbConfig
    );

    void processVector(
       CConcurrentProcessingVector<T>* vec,
       QHash<QString, int>(CConcurrentProcessingVector<T>::*fn)( const CConfigDatabase&, const int, const QHash<QString, QVariant>& ),
       const CConfigDatabase& dbConfig,
       const QHash<QString, QVariant>& params
    );

    void processStatic(
      const CConcurrentProcessingVector<T>& vec,
      QHash<QString, int>(CConcurrentProcessingVector<T>::*fn)( const CConfigDatabase&, const int, const int, const int ) const,
      const CConfigDatabase& dbConfig
    );

    void processStatic(
      const CConcurrentProcessingVector<T>& vec,
      QHash<QString, int>(CConcurrentProcessingVector<T>::*fn)( const CConfigDatabase&, const int, const int, const int, const QHash<QString, QVariant>& ) const,
      const CConfigDatabase& dbConfig,
      const QHash<QString, QVariant>& params
    );

    void processStatic(
      const CConcurrentProcessingStringHash<T>& stringHash,
      QHash<QString, int>(CConcurrentProcessingStringHash<T>::*fn)( const CConfigDatabase&, const QList<QString>&, const int ) const,
      const CConfigDatabase& dbConfig
    );

    void processStatic(
       const CConcurrentProcessingStringHash<T>& stringHash,
       QHash<QString, int>(CConcurrentProcessingStringHash<T>::*fn)( const CConfigDatabase&,  const QList<QString>&, const int, const QHash<QString, QVariant>& ) const,
       const CConfigDatabase& dbConfig,
       const QHash<QString, QVariant>& params
    );

    void processStatic(
      const CConcurrentProcessingIntHash<T>& intHash,
      QHash<QString, int>(CConcurrentProcessingIntHash<T>::*fn)( const CConfigDatabase&, const QList<int>&, const int ) const,
      const CConfigDatabase& dbConfig
    );

    void processStatic(
       const CConcurrentProcessingIntHash<T>& intHash,
       QHash<QString, int>(CConcurrentProcessingIntHash<T>::*fn)( const CConfigDatabase&,  const QList<int>&, const int, const QHash<QString, QVariant>& ) const,
       const CConfigDatabase& dbConfig,
       const QHash<QString, QVariant>& params
    );

    ~CConcurrentProcessingManager() { /* Nothing to do here */ }

    void waitForFinished();

    int maxListSize() const { return _maxListSize; }
    QHash<QString, int> results() const { return _results; }

    // FIXME: Consider writing this some day.
    //void writeUsage();

  protected:
    void checkThreadsForUse();
    void checkForFinishedThreads();
    void adjustMaxListSize();

    void mergeResults( QHash<QString, int> results2 );

    QHash<QString, int> _results;

    QList<CConcurrentProcessingRunner<T>* > _runners;

    int _maxListSize;
    int _threadID;
    int _idealThreadCount;
    bool _autoAdjustMaxListSize;
    bool _threadsFull;

    QList<int> _maxListSizes;
    QList<bool> _backlog;
    QList<qint64> _waitTime;
    QList<int> _threadsInUse;

  private:
    Q_DISABLE_COPY( CConcurrentProcessingManager )
};


// If everything is working properly, there is no reason for an end user
// to access CConcurrentProcessingRunner directly.  Just use CConcurrentProcessingManager.
template <class T>
class CConcurrentProcessingRunner {
  public:
    CConcurrentProcessingRunner( CConcurrentProcessingList<T>* list, const QFuture<QHash<QString, int> >& f );
    CConcurrentProcessingRunner( CConcurrentProcessingVector<T>* vector, const QFuture<QHash<QString, int> >& f );
    CConcurrentProcessingRunner( const QFuture<QHash<QString, int> >& f );

    ~CConcurrentProcessingRunner();

    qint64 runtime() const; // in milliseconds. -1 if process hasn't yet ended.

    void waitForFinished();
    bool finished() const;
    bool isFinished();
    QHash<QString, int> result() const;

  protected:
    void cleanup();

    bool _ownsContainer;
    QFuture< QHash<QString, int> > _future;
    CConcurrentProcessingList<T>* _list;
    CConcurrentProcessingVector<T>* _vector;
    bool _hasFinished;
    QElapsedTimer _timer;
    qint64 _timeInMsec;

  private:
    Q_DISABLE_COPY( CConcurrentProcessingRunner )
};


#include "cconcurrentprocessing.tpp"

#endif // CCONCURRENTPROCESSING_H
