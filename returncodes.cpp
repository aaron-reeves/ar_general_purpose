/*
returncodes.h/cpp
-----------------
Begin: 2016/09/22
Authors: Aaron Reeves <aaron.reeves@sruc.ac.uk>
         Julie Stirling <julie.stirling@sruc.ac.uk>
---------------------------------------------------
Copyright (C) 2016 Epidemiology Research Unity, Scotland's Rural College (SRUC)

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#include "returncodes.h"

QString ReturnCode::codeList() {
 return QString(
   "Exit codes (used in combination with one another):\n"
   "  SUCCESS                 :     0\n"
   "  EMPTY_INPUT_FILE        :     1\n"
   "  DUPLICATE_RECORD        :     2\n"
   "  DATA_VALIDATION_PROBLEM :     4\n"
   "  BAD_COMMAND             :     8\n"
   "  INPUT_FILE_PROBLEM      :    16\n"
   "  OUTPUT_FILE_PROBLEM     :    32\n"
   "  ERROR_LOG_PROBLEM       :    64\n"
   "  ERROR_VARIANT_CONVERSION:   128\n"
   "  PROCESSING_INTERRUPTED  :   256\n"
   "  FAILED_DB_QUERY         :   512\n"
   "  BAD_CONFIG              :  1024\n"
   "  FILE_SYSTEM_PROBLEM     :  2048\n"
   "  REQUIRED_FIELDS_MISSING :  4096\n"
   "  BAD_DATABASE            :  8192\n"
   "  FATAL_ERROR             : 16384\n"
   "  APPLICATION_ERROR       : 32768\n"
   "  UNRECOGNIZED_FIELD      : 65536\n"
 );
}

QString ReturnCode::codeDescr( const int val ) {
  if( SUCCESS == val )
    return "SUCCESS: No errors were encountered.";
  else {
     QString result;

     if( val & EMPTY_INPUT_FILE )
       result.append( "EMPTY_INPUT_FILE: The specified input file contains no data.\n" );
     if( val & DUPLICATE_RECORD )
       result.append( "DUPLICATE_RECORD: Duplicate records were encountered.\n" );
    if( val & DATA_VALIDATION_PROBLEM )
      result.append( "DATA_VALIDATION_PROBLEM: Some records contain invalid fields.\n" );
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
    if( val & FATAL_ERROR )
      result.append( "FATAL_ERROR: A fatal error occurred.  Please check with the developers: this is our bad.\n" );
    if( val & APPLICATION_ERROR )
      result.append( "APPLICATION_ERROR: An application error occurred.  Please check with the developers.\n" );
    if( val & UNRECOGNIZED_FIELD )
      result.append( "UNRECOGNIZED_FIELD: A database field specified in the applicatoin does not exist.  Please check with the developers.\n" );

    return result.trimmed();
  }
}
