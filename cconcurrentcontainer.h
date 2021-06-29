/*
cconcurrentcontainer.h/cpp
--------------------------
Begin: 2020-04-24
Author: Aaron Reeves <aaron.reeves@sruc.ac.uk>
---------------------------------------------------------
Copyright (C) 2020 - 2021 Scotland's Rural College (SRUC)

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#ifndef CCONCURRENTCONTAINER_H
#define CCONCURRENTCONTAINER_H

#include <QtCore>

/*
This is one of the base classes for the various concurrent containers in concurrentcontainers.h/tpp
*/
class CConcurrentContainer {
  public:
    //CConcurrentContainer() { /* Nothing to do here */ }
    //CConcurrentContainer( const CConcurrentContainer& other ) { /* Nothing to do here */ }
    //CConcurrentContainer& operator=( const CConcurrentContainer& other ) { /* Nothing to do here */ }
    //~CConcurrentContainer() { /* Nothing to do here */ }

    virtual QHash<QString, int> resultsTemplate() const;

    virtual QHash<QString, int> mergeResults( const QHash<QString, int>& results1, const QHash<QString, int>& results2 ) const;
};

#endif // CCONCURRENTCONTAINER_H
