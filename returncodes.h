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

// These are listed in approximate order of severity.
namespace ReturnCode {
  enum Result {
    SUCCESS                  = 0x00000, //      0
    EMPTY_INPUT_FILE         = 0x00001, //      1
    DUPLICATE_RECORD         = 0x00002, //      2
    KNOWN_VALIDATION_ISSUE = 0x00004, //      4
    DATA_VALIDATION_PROBLEM  = 0x00008, //      8
    BAD_COMMAND              = 0x00010, //     16
    INPUT_FILE_PROBLEM       = 0x00020, //     32
    OUTPUT_FILE_PROBLEM      = 0x00040, //     64
    ERROR_LOG_PROBLEM        = 0x00080, //    128
    ERROR_VARIANT_CONVERSION = 0x00100, //    256
    PROCESSING_INTERRUPTED   = 0x00200, //    512
    FAILED_DB_QUERY          = 0x00400, //   1024
    BAD_CONFIG               = 0x00800, //   2048
    FILE_SYSTEM_PROBLEM      = 0x01000, //   4096
    REQUIRED_FIELDS_MISSING  = 0x02000, //   8192
    BAD_DATABASE             = 0x04000, //  16384
    FATAL_ERROR              = 0x08000, //  32768
    APPLICATION_ERROR        = 0x10000, //  65536
    UNRECOGNIZED_FIELD       = 0x20000  // 131072
    //NEXT_PROBLEM = ???
    //...
  };

  QString codeList();

  QString codeDescr( const int val );
}

#endif // RETURNCODES_H

