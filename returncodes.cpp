/*
returncodes.h/cpp
-----------------
Begin: 2016/09/22
Authors: Aaron Reeves <aaron.reeves@sruc.ac.uk>
         Julie Stirling <julie.stirling@sruc.ac.uk>
---------------------------------------------------
Copyright (C) 2016 - 2019 Epidemiology Research Unit, Scotland's Rural College (SRUC)

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#include "returncodes.h"

const QString ReturnCode::codeList() {
 return QString(
   "Exit codes (used in combination with one another):\n"
   "  SUCCESS                  :       0\n"
   "  WARNING_EMPTY_INPUT_FILE :       1\n"
   "  WARNING_DUPLICATE_RECORD :       2\n"
   "  WARNING_KNOWN_ISSUE      :       4\n"
   "  DATA_VALIDATION_PROBLEM  :       8\n"
   "  BAD_COMMAND              :      16\n"
   "  INPUT_FILE_PROBLEM       :      32\n"
   "  OUTPUT_FILE_PROBLEM      :      64\n"
   "  ERROR_LOG_PROBLEM        :     128\n"
   "  ERROR_VARIANT_CONVERSION :     256\n"
   "  PROCESSING_INTERRUPTED   :     512\n"
   "  FAILED_DB_QUERY          :    1024\n"
   "  BAD_CONFIG               :    2048\n"
   "  FILE_SYSTEM_PROBLEM      :    4096\n"
   "  REQUIRED_FIELDS_MISSING  :    8192\n"
   "  BAD_DATABASE             :   16384\n"
   "  APPLICATION_ERROR        :   32768\n"
   "  UNRECOGNIZED_FIELD       :   65536\n"
   "  CHECKSUM_PROBLEM         :  131072\n"
   "  UNKNOWN_RESULT           :  262144\n"
   "  NETWORK_PROBLEM          :  524288\n"
   "  FATAL_ERROR              : 8388608\n"
 );
}

const QString ReturnCode::codeDescr( const int val ) {
  if( SUCCESS == val )
    return QStringLiteral( "SUCCESS: No errors were encountered." );
  else {
     QString result;

     if( val & WARNING_EMPTY_INPUT_FILE )
       result.append( "WARNING_EMPTY_INPUT_FILE: The specified input file contains no data.\n" );
     if( val & WARNING_DUPLICATE_RECORD )
       result.append( "WARNING_DUPLICATE_RECORD: Duplicate records were encountered.\n" );
     if( val & WARNING_KNOWN_ISSUE )
       result.append( "WARNING_KNOWN_ISSUE: Some records contain recognized but invalid field values.\n" );
    if( val & DATA_VALIDATION_PROBLEM )
      result.append( "DATA_VALIDATION_PROBLEM: Some records contain invalid field values.\n" );
    if( val & BAD_COMMAND )
      result.append( "BAD_COMMAND: Command line arguments are invalid.  RTFM and try again.\n" );
    if( val & INPUT_FILE_PROBLEM )
      result.append( "INPUT_FILE_PROBLEM: An input file does not exist (did you specify a valid path?), could not be opened, could not be read, or is not the expected type.\n" );
    if( val & OUTPUT_FILE_PROBLEM )
      result.append( "OUTPUT_FILE_PROBLEM: An output file could not be created or written.  Did you specify a valid path?\n" );
    if( val & ERROR_LOG_PROBLEM )
      result.append( "ERROR_LOG_PROBLEM: Application log could not be generated.  Did you specify a valid path?\n" );
    if( val & ERROR_VARIANT_CONVERSION )
      result.append( "ERROR_VARIANT_CONVERSION: Invalid data conversion requested.  Check your data file format, and then check with developers.\n" );
    if( val & PROCESSING_INTERRUPTED )
      result.append( "PROCESSING_INTERRUPTED: Operation interrupted by the user.  What did you do?\n" );
    if( val & FAILED_DB_QUERY )
      result.append( "FAILED_DB_QUERY: An SQL statement or query could not be executed. Check any SQL code that you may have written, and then check with the developers.\n" );
    if( val & BAD_CONFIG )
      result.append( "BAD_CONFIG: Specified configuration file could not be opened or properly parsed.\n" );
    if( val & FILE_SYSTEM_PROBLEM )
      result.append( "FILE_SYSTEM_PROBLEM: A file could not be properly accessed.  Do you have the right permissions?\n" );
    if( val & REQUIRED_FIELDS_MISSING )
      result.append( "REQUIRED_FIELDS_MISSING: Check the data file.\n" );
    if( val & BAD_DATABASE )
      result.append( "BAD_DATABASE: Specified database could not be accessed.  Check your network and the specified database parameters.\n" );
    if( val & APPLICATION_ERROR )
      result.append( "APPLICATION_ERROR: An application error occurred.  Please check with the developers.\n" );
    if( val & UNRECOGNIZED_FIELD )
      result.append( "UNRECOGNIZED_FIELD: A database field specified in the applicatoin does not exist.  Please check with the developers.\n" );
    if( val & CHECKSUM_PROBLEM )
      result.append( "CHECKSUM_PROBLEM: There is a mismatch between actual and expected file contents.  Please check with the developers.\n" );
    if( val & UNKNOWN_RESULT )
      result.append( "UNKNOWN_RESULT: The outcome is not yet known.  Has the process run?\n" );
    if( val & NETWORK_PROBLEM )
      result.append( "NETWORK_PROBLEM: A required network seems to be unreachable, or the required network service cannot be run.\n" );

    if( val & FATAL_ERROR )
      result.append( "FATAL_ERROR: A fatal error occurred.  Please check with the developers: this is our bad.\n" );
    return result.trimmed();
  }
}
