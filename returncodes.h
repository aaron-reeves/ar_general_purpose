/*
returncodes.h/cpp
-----------------
Begin: 2016/09/16
Authors: Aaron Reeves <aaron.reeves@sruc.ac.uk>
         Julie Stirling <julie.stirling@sruc.ac.uk>
---------------------------------------------------
Copyright (C) 2016 Epidemiology Research Unity, Scotland's Rural College (SRUC)

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#ifndef RETURNCODES_H
#define RETURNCODES_H

#include <QString>

namespace ReturnCode {
  enum Result {
    SUCCESS = 0x0000,                  //    0
    DATA_VALIDATION_PROBLEM = 0x0001,  //    1
    DUPLICATE_RECORD = 0x0002,         //    2
    BAD_COMMAND = 0x0004,              //    4
    INPUT_FILE_PROBLEM = 0x0008,       //    8
    OUTPUT_FILE_PROBLEM = 0x0010,      //   16
    ERROR_LOG_PROBLEM = 0x0020,        //   32
    ERROR_VARIANT_CONVERSION = 0x0040, //   64
    PROCESSING_INTERRUPTED = 0x0080,   //  128
    FAILED_DB_QUERY = 0x0100,          //  256
    BAD_CONFIG = 0x0200,               //  512
    FILE_SYSTEM_PROBLEM = 0x0400,      // 1024
    REQUIRED_FIELDS_MISSING = 0x0800,  // 2048
    BAD_DATABASE = 0x1000,             // 4096
    FATAL_ERROR = 0x2000,              // 8192
    APPLICATION_ERROR = 0x4000,       // 16384
    UNRECOGNIZED_FIELD = 0x8000       // 32768
    //NEXT_PROBLEM = ???
    //...
  };

  QString codeList();

  QString codeDescr( const int val );
}

#endif // RETURNCODES_H

