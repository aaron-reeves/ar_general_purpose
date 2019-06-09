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

#ifndef CTOTALTIME_H
#define CTOTALTIME_H

#include <QtCore>

struct STimeBits {
  int hour;
  int minute;
  int second;
  int msec;
};

// Similar to QTime, but does not reset to zero after 24 hours
class CTotalTime {
  public:
    CTotalTime();
    CTotalTime( const QDateTime& startTime );
    CTotalTime( const qint64 msecs );
    CTotalTime( const int hours, const int minutes, const int seconds, const int milliseconds );
    CTotalTime( const CTotalTime& other );
    CTotalTime& operator=( const CTotalTime& other );
    ~CTotalTime();
    
    void start();
    void stop();
    void reset();
    
    bool isRunning() const { return _isRunning; }
        
    void addTime( const CTotalTime& other );
    void addHours( const int nHours );
    void addMinutes( const int nMinutes );
    void addSecs( const int nSeconds );
    void addMSecs( const qint64 nMilliseconds );
    
    int hour() const;
    int minute() const;
    int second() const;
    int msec() const;
    
    double totalHours() const;
    double totalMinutes() const;
    double totalSeconds() const;
    qint64 totalMilliseconds() const;

    // Elapsed time since last start/reset, not including time when the stopwatch was not running
    CTotalTime elapsed();

    // Elapsed time between start/reset and 'time', regardless of whether the stopwatch was running
    CTotalTime elapsedSince( const QDateTime& time );

    // Elapsed time between 'startTime' and 'endTime'
    static CTotalTime elapsed( const QDateTime& startTime, const QDateTime& endTime );
    
    // Format: hh:mm:ss(.zzz)
    QString toString( const bool includeMSecs ) const;

    void debug() const;
    
  protected:
    void assign( const CTotalTime& other );
    STimeBits timeBits() const;
    
    bool _isRunning;
    QDateTime _startTime;
    qint64 _totalMilliseconds;
};


#endif // CTOTALTIME_H
