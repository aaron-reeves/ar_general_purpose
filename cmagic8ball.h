/*
cmagic8ball.h/cpp
-----------------
Begin: 2017-07-22
Author: Aaron Reeves <aaron.reeves@sruc.ac.uk>
---------------------------------------------------
Copyright (C) 2017 Epidemiology Research Unit, Scotland's Rural College (SRUC)

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#ifndef CMAGIC8BALL_H
#define CMAGIC8BALL_H

#include <QtCore>

class CMagic8Ball {
  public:
    enum Magic8BallType {
      Undefined,
      Number,
      Proportion
    };

    CMagic8Ball( const int number );
    CMagic8Ball( const double proportion );
    CMagic8Ball(const QString& parameters );
    ~CMagic8Ball();

    bool answer( const int number );

    QString errorMsg() const { return _errorMsg; }
    bool error() const { return _error; }

  protected:
    void setNumberType( const int number );
    void setProportionType( const double proportion );

    QRandomGenerator* _rng;

    int _type;
    int _number;
    double _proportion;

    bool _error;
    QString _errorMsg;

  private:
    Q_DISABLE_COPY( CMagic8Ball )
};

#endif // CMAGIC8BALL_H
