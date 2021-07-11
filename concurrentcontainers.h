/*
concurrentcontainers.h/tpp
--------------------------
Begin: 2019-06-18
Author: Aaron Reeves <aaron.reeves@sruc.ac.uk>
---------------------------------------------------------
Copyright (C) 2019 - 2021 Scotland's Rural College (SRUC)

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#ifndef CONCURRENTCONTAINERS_H
#define CONCURRENTCONTAINERS_H

#include <QtCore>
#include <QtConcurrent>

#include <ar_general_purpose/returncodes.h>
#include <ar_general_purpose/cconcurrentcontainer.h>
#include <ar_general_purpose/cconcurrentrunner.h>

#include <epic_general_purpose/cepicconfigfile.h>

template <class T>
class CConcurrentVector : public CConcurrentContainer, public QVector<T> {
  public:
    CConcurrentVector() : CConcurrentContainer(), QVector<T>() { /* Nothing else to do here */ }
    CConcurrentVector( const CConcurrentVector& other ) : CConcurrentContainer(), QVector<T>( other ) { assign( other ); }
    CConcurrentVector& operator= ( const CConcurrentVector& other );
    
    virtual ~CConcurrentVector() { /* Nothing else to do here */ }
      
    virtual QHash<QString, int> populateDatabase(
      const CConfigDatabase& cfdb,
      const int startIdx,
      const int endIdx,
      const int threadID,
      const QHash<QString, QVariant>& otherParams
    ) const;

  protected:
    void assign( const CConcurrentVector& other ) { Q_UNUSED( other ); }
};


template <class T>
class CConcurrentStringHash : public CConcurrentContainer, public QHash<QString, T> {
  public:
    CConcurrentStringHash() : CConcurrentContainer(), QHash<QString, T>() { /* Nothing else to do here */ }
    CConcurrentStringHash( const CConcurrentStringHash& other ) : CConcurrentContainer(), QHash<QString, T>( other ) { assign( other ); }
    CConcurrentStringHash& operator= ( const CConcurrentStringHash& other );
    
    virtual ~CConcurrentStringHash() { /* Nothing else to do here */ }

    virtual QHash<QString, int> populateDatabase(
      const CConfigDatabase& cfdb,
      const QHash<QString, QVariant>& otherParams
    ) const = 0;

    virtual QHash<QString, int> populateDatabase(
      const CConfigDatabase& cfdb,
      const QList<QString>& list,
      const int threadID,
      const QHash<QString, QVariant>& otherParams
    ) const;

  protected:
    void assign( const CConcurrentStringHash& other ) { Q_UNUSED( other ); }
};

template <class T>
class CConcurrentIntHash : public CConcurrentContainer, public QHash<int, T> {
  public:
    CConcurrentIntHash() : CConcurrentContainer(), QHash<int, T>() { /* Nothing else to do here */ }
    CConcurrentIntHash( const CConcurrentIntHash& other ) : CConcurrentContainer(), QHash<int, T>( other ) { assign( other ); }
    CConcurrentIntHash& operator= ( const CConcurrentIntHash& other );

    virtual ~CConcurrentIntHash() { /* Nothing else to do here */ }

    virtual QHash<QString, int> populateDatabase(
      CConfigDatabase cfdb,
      const QList<int>& list,
      const int threadID,
      const QHash<QString, QVariant>& otherParams
    ) const;

  protected:
    void assign( const CConcurrentIntHash& other ) { Q_UNUSED( other ); }
};



template <typename T, typename Class>
class CConcurrentProcessor : public QList<CConcurrentRunner*> {
  public:
    CConcurrentProcessor() : QList<CConcurrentRunner*>() { /* Nothing else to do here */ }
    ~CConcurrentProcessor();
      
    QHash<QString, int> populateDatabase(
      const CConcurrentVector<T>* v,
      QHash<QString, int> (Class::*fn)( const CConfigDatabase&, const int, const int, const int, const QHash<QString, QVariant>& ) const,
      const CConfigDatabase& cfdb,
      const QHash<QString, QVariant>& otherParams
    );
  
    QHash<QString, int> populateDatabase(
      const CConcurrentStringHash<T>* h,
      QHash<QString, int> (Class::*fn)( const CConfigDatabase&, const QList<QString>&, const int, const QHash<QString, QVariant>& ) const,
      const CConfigDatabase& cfdb,
      const QHash<QString, QVariant>& otherParams
    );

    QHash<QString, int> populateDatabase(
      const CConcurrentIntHash<T>* h,
      QHash<QString, int> (Class::*fn)( CConfigDatabase, const QList<int>&, const int, const QHash<QString, QVariant>& ) const,
      const CConfigDatabase& cfdb,
      const QHash<QString, QVariant>& otherParams
    );

  protected:
    void waitForFinished();

  private:
    Q_DISABLE_COPY( CConcurrentProcessor )
};


template <typename Class, typename Key, typename T>
class CConcurrentHashProcessor : public QList<CConcurrentRunner*> {
  public:
    CConcurrentHashProcessor() : QList<CConcurrentRunner*>() { /* Nothing else to do here */ }
    ~CConcurrentHashProcessor();

    QHash<QString, int> populateDatabase(
      const CConcurrentStringHash<T>* h,
      QHash<QString, int> (Class::*fn)( const CConfigDatabase&, const QList<QString>&, const int, const QHash<QString, QVariant>& ) const,
      const CConfigDatabase& cfdb,
      const QHash<QString, QVariant>& otherParams
    );

    QHash<QString, int> populateDatabase(
      const CConcurrentIntHash<T>* h,
      QHash<QString, int> (Class::*fn)( CConfigDatabase, const QList<int>&, const int, const QHash<QString, QVariant>& ) const,
      const CConfigDatabase& cfdb,
      const QHash<QString, QVariant>& otherParams
    );

  protected:
    void waitForFinished();

  private:
    Q_DISABLE_COPY( CConcurrentHashProcessor )
};


#include "concurrentcontainers.tpp"

#endif // CONCURRENTCONTAINERS_H
