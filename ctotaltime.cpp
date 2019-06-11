/*
ctotaltime.h/cpp
----------------
Begin: 2019/05/18
Author: Aaron Reeves <aaron.reeves@sruc.ac.uk>
--------------------------------------------------
Copyright (C) 2019 Epidemiology Research Unit, Scotland's Rural College (SRUC)

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

// FIXME: Replace the innards of this class with QElapsedTimer

#include "ctotaltime.h"

#include "arcommon.h"

CTotalTime::CTotalTime() {
  _isRunning = false;
  _totalMilliseconds = 0;
}

CTotalTime::CTotalTime( const QDateTime& startTime ) {
  _isRunning = true;
  _totalMilliseconds = 0;
  _startTime = startTime;
}

CTotalTime::CTotalTime( const qint64 msecs ) {
  _isRunning = false;
  _totalMilliseconds = msecs;
}

CTotalTime::CTotalTime( const int hours, const int minutes, const int seconds, const int milliseconds ) {
  _isRunning = false;
  _totalMilliseconds = 0;
  addHours( hours );
  addMinutes( minutes );
  addSecs( seconds );
  addMSecs( milliseconds );
}

CTotalTime::CTotalTime( const CTotalTime& other ) {
  assign( other );
}

CTotalTime& CTotalTime::operator=( const CTotalTime& other ) {
  assign( other );
  return *this;
}

CTotalTime::~CTotalTime() {
  // Nothing to do here.
}

void CTotalTime::assign( const CTotalTime& other ) {
  _isRunning = other._isRunning;
  _totalMilliseconds = other._totalMilliseconds;
  _startTime = other._startTime;
}


void CTotalTime::start() {
  _startTime = QDateTime::currentDateTime();
  _isRunning = true;
  // Don't change _totalMilliseconds: leave this as a running count.
}

void CTotalTime::reset() {
  _startTime = QDateTime::currentDateTime();
  _totalMilliseconds = 0;
  // Don't change _isRunning: leave it alone.
}

void CTotalTime::stop() {
  if( _isRunning ) {
    this->addTime( this->elapsed() );
  }
  _isRunning = false;
}

void CTotalTime::addTime( const CTotalTime& other ) {
  //Q_ASSERT( !_isRunning );
  this->addMSecs( other._totalMilliseconds );
}

void CTotalTime::addHours( const int nHours ) {
  //Q_ASSERT( !_isRunning );
  this->addMSecs( nHours * 60 * 60 * 1000 );
}

void CTotalTime::addMinutes( const int nMinutes ) {
  //Q_ASSERT( !_isRunning );
  this->addMSecs( nMinutes * 60 * 1000 );
}

void CTotalTime::addSecs( const int nSeconds ) {
  //Q_ASSERT( !_isRunning );
  this->addMSecs( nSeconds * 1000 );
}

void CTotalTime::addMSecs( const qint64 nMilliseconds ) {
  //Q_ASSERT( !_isRunning );
  _totalMilliseconds = _totalMilliseconds + nMilliseconds;
}


STimeBits CTotalTime::timeBits() const {
  STimeBits result;

  qint64 ms = totalMilliseconds();

  int hours = int( trunc( ms / 1000.0 / 60.0 / 60.0 ) );
  qint64 remainder = ms - ( hours * 60 * 60 * 1000 );

  //qDebug() << "Hours:" << hours;
  //qDebug() << "Remainder after hours:" << remainder;

  int minutes = int( trunc( remainder / 60.0 / 1000.0 ) );
  remainder = remainder - ( minutes * 60 * 1000 );

  //qDebug() << "Minutes:" << minutes;
  //qDebug() << "Remainder after minutes:" << remainder;

  int seconds = int( trunc( remainder / 1000.0 ) );

  //qDebug() << "Seconds:" << seconds;
  //qDebug() << "Remainder after seconds:" << remainder - ( seconds * 1000 );

  int msec = int( remainder - ( seconds * 1000 ) );

  result.hour = hours;
  result.minute = minutes;
  result.second = seconds;
  result.msec = msec;

  return result;
}


int CTotalTime::hour() const {
  return timeBits().hour;
}

int CTotalTime::minute() const {
  return timeBits().minute;
}

int CTotalTime::second() const {
  return timeBits().second;
}

int CTotalTime::msec() const {
  return timeBits().msec;
}


double CTotalTime::totalHours() const {
  return totalMilliseconds() / 60.0 / 60.0 / 1000.0;
}

double CTotalTime::totalMinutes() const {
  return totalMilliseconds() / 60.0 / 1000.0;
}

double CTotalTime::totalSeconds() const {
  return totalMilliseconds() / 1000.0;
}


qint64 CTotalTime::totalMilliseconds() const {
  if( !_isRunning )
    return _totalMilliseconds;
  else
    return QDateTime::currentDateTime().toMSecsSinceEpoch() - _startTime.toMSecsSinceEpoch() + _totalMilliseconds;
}


// Elapsed time since last start/reset, not including time when the stopwatch was not running
CTotalTime CTotalTime::elapsed() {
  CTotalTime result;
  result.addMSecs( this->totalMilliseconds() );

  return result;
}

// Elapsed time between start/reset and 'time', regardless of whether the stopwatch was running
CTotalTime CTotalTime::elapsedSince( const QDateTime& time ) {
  return CTotalTime( time.toMSecsSinceEpoch() - _startTime.toMSecsSinceEpoch() );
}

// Elapsed time between 'startTime' and 'endTime'
CTotalTime CTotalTime::elapsed( const QDateTime& startTime, const QDateTime& endTime ) {
  return CTotalTime( endTime.toMSecsSinceEpoch() - startTime.toMSecsSinceEpoch() );
}

// Format: hh:mm:ss(.zzz)
QString CTotalTime::toString( const bool includeMSecs ) const {
  STimeBits tb = timeBits();

  if( includeMSecs ) {
    return
      QString( "%1:%2:%3.%4" )
        .arg( tb.hour )
        .arg( paddedInt( tb.minute, 2 ) )
        .arg( paddedInt( tb.second, 2 )
        .arg( rightPaddedStr( paddedInt( tb.msec, 3 ), 3, '0' ) ) )
    ;
  }
  else
    return QString( "%1:%2:%3" ).arg( tb.hour ).arg( paddedInt( tb.minute, 2 ) ).arg( paddedInt( tb.second, 2 ) );
}


void CTotalTime::debug() const {
  qDb() << "isRunning:" << boolToText( _isRunning );
  qDb() << "startTime:" << _startTime.toString( "yyyy-MM-dd hh:mm:ss.zzz" );
  qDb() << "totalMilliseconds:" << _totalMilliseconds;

  qDb() << "hour" << hour();
  qDb() << "minute" << minute();
  qDb() << "second" << second();
  qDb() << "msec" << msec();
}





