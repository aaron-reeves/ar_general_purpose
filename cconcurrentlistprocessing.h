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

#ifndef CCONCURRENTLISTPROCESSING_H
#define CCONCURRENTLISTPROCESSING_H

#include <QtCore>
#include <QtConcurrent>

#include <ar_general_purpose/log.h>
#include <ar_general_purpose/qcout.h>
#include <ar_general_purpose/returncodes.h>

#include <epic_general_purpose/cepicconfigfile.h>

#include "cdatabaseresults.h"


template <class T>
class CConcurrentProcessingList : public QList<T> {
  public:
    virtual ~CConcurrentProcessingList() { /* Nothing special to do here */ }

    // These functions must be overridden to do anything useful
    virtual QHash<QString, int> populateDatabaseThreaded( CConfigDatabase* dbConfig, const int threadID ) = 0;
};


// A forward declaration for the sake of CConcurrentProcessingManager
template <class T>
class CConcurrentProcessingRunner;


template <class T>
class CConcurrentProcessingManager {
  public:
    CConcurrentProcessingManager( const int initialMaxListSize = 0, const bool autoAdjustMaxListSize = true );

    void appendListForDatabasePopulation(
      CConcurrentProcessingList<T>* list,
      QHash<QString, int>(CConcurrentProcessingList<T>::*fn)( CConfigDatabase*, int ),
      CConfigDatabase* dbConfig
    );

    //void appendListForProcessing( CConcurrentProcessingList<T>* list );

    ~CConcurrentProcessingManager() { /* Nothing to do here */ }

    void waitForFinished();

    int maxListSize() const { return _maxListSize; }
    QHash<QString, int> results() const { return _results; }

    // FIXME: Consider writing this some day.
    //void writeUsage();

  protected:
    void adjustMaxListSize();

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
// FIXME: Consider moving this to the tpp file, so it is treated as a "private" implementation.
template <class T>
class CConcurrentProcessingRunner {
  public:
    CConcurrentProcessingRunner( CConcurrentProcessingList<T>* list, const QFuture<QHash<QString, int> >& f );
    CConcurrentProcessingRunner( const QFuture<QHash<QString, int> >& f );

    ~CConcurrentProcessingRunner();

    qint64 runtime() const; // in milliseconds. -1 if process hasn't yet ended.

    void waitForFinished();
    bool finished() const;
    bool isFinished();
    QHash<QString, int> result() const;

  protected:
    void cleanup();

    QFuture< QHash<QString, int> > _future;
    CConcurrentProcessingList<T>* _list;
    bool _hasFinished;
    QElapsedTimer _timer;
    qint64 _timeInMsec;

  private:
    Q_DISABLE_COPY( CConcurrentProcessingRunner )
};




#include "cconcurrentlistprocessing.tpp"

#endif // CCONCURRENTLISTPROCESSING_H
