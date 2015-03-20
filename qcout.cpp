/*
qcout.h/cpp
-----------
Begin: 2007/03/15
Author: Aaron Reeves <aaron.reeves@naadsm.org>
--------------------------------------------------
Copyright (C) 2007 - 2008 Aaron Reeves

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#include "qcout.h"

#ifdef win32
  #include <windows.h>
#else
  #include <termios.h>
  #include <unistd.h>
#endif

QTextStream cout( stdout, QIODevice::WriteOnly );
QTextStream cin( stdin,  QIODevice::ReadOnly );

// From http://stackoverflow.com/questions/1413445/read-a-password-from-stdcin
void setStdinEcho(bool enable /* = true */){
  #ifdef win32
      HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
      DWORD mode;
      GetConsoleMode(hStdin, &mode);

      if( !enable )
          mode &= ~ENABLE_ECHO_INPUT;
      else
          mode |= ENABLE_ECHO_INPUT;

      SetConsoleMode(hStdin, mode );

  #else
      struct termios tty;
      tcgetattr(STDIN_FILENO, &tty);
      if( !enable )
          tty.c_lflag &= ~ECHO;
      else
          tty.c_lflag |= ECHO;

      (void) tcsetattr(STDIN_FILENO, TCSANOW, &tty);
  #endif
}


void cerr( const QString& msg, const bool silent ) {
  if( !silent )
    cout << msg << endl << flush;
}


void cerr( const char* msg, const bool silent ) {
  if( !silent )
    cout << msg << endl << flush;
}
