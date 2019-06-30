/*
returncodes.h/cpp
-----------------
Begin: 2016/09/16
Authors: Aaron Reeves <aaron.reeves@sruc.ac.uk>
         Julie Stirling <julie.stirling@sruc.ac.uk>
---------------------------------------------------
Copyright (C) 2016 - 2019 Epidemiology Research Unit, Scotland's Rural College (SRUC)

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
    SUCCESS                  = 0x000000, //       0
    EMPTY_INPUT_FILE         = 0x000001, //       1
    DUPLICATE_RECORD         = 0x000002, //       2
    KNOWN_VALIDATION_ISSUE   = 0x000004, //       4
    DATA_VALIDATION_PROBLEM  = 0x000008, //       8
    BAD_COMMAND              = 0x000010, //      16
    INPUT_FILE_PROBLEM       = 0x000020, //      32
    OUTPUT_FILE_PROBLEM      = 0x000040, //      64
    ERROR_LOG_PROBLEM        = 0x000080, //     128
    ERROR_VARIANT_CONVERSION = 0x000100, //     256
    PROCESSING_INTERRUPTED   = 0x000200, //     512
    FAILED_DB_QUERY          = 0x000400, //    1024
    BAD_CONFIG               = 0x000800, //    2048
    FILE_SYSTEM_PROBLEM      = 0x001000, //    4096
    REQUIRED_FIELDS_MISSING  = 0x002000, //    8192
    BAD_DATABASE             = 0x004000, //   16384
    APPLICATION_ERROR        = 0x008000, //   32768
    UNRECOGNIZED_FIELD       = 0x010000, //   65536
    CHECKSUM_PROBLEM         = 0x020000, //  131072
    UNKNOWN_RESULT           = 0x040000, //  262144
    NETWORK_PROBLEM          = 0x080000, //  524288
    //NEXT_PROBLEM        // = 0x100000, // 1048576
                          // = 0x200000, // 2097152
                          // = 0x400000, // 4194304
    FATAL_ERROR              = 0x800000  // 8388608
    //...
  };

  QString codeList();

  QString codeDescr( const int val );
}

#endif // RETURNCODES_H

