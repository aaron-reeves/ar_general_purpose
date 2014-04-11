/*
qcout.cpp
---------
Begin: 2007/03/15
Last revision: $Date: 2011-10-25 04:57:18 $ $Author: areeves $
Version number: $Revision: 1.4 $
Project: Atriplex Distributed Computing System
Website: 
Author: Aaron Reeves <aaron@aaronreeves.com>
--------------------------------------------------
Copyright (C) 2007 - 2008 Aaron Reeves

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#include "qcout.h"

QTextStream cout( stdout, QIODevice::WriteOnly );
QTextStream cin( stdin,  QIODevice::ReadOnly );
