/*
sysutils.h/cpp
--------------
Begin: 2019-02-05
Author: Aaron Reeves <aaron.reeves@sruc.ac.uk>
---------------------------------------------------
Copyright (C) 2019 Scotland's Rural College (SRUC)

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#ifndef SYSUTILS_H
#define SYSUTILS_H

void PrintMemoryInfo();
unsigned long procWorkingSet();
unsigned long procPrivateWorkingSet();

#endif // SYSUTILS_H
