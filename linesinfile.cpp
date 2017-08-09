/*
linesinfile.h/cpp
-----------------
Begin: 2017/06/07
Author (for modifications): Aaron Reeves <aaron.reeves@sruc.ac.uk>
------------------------------------------------------------------

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

This code is derived from the GNU core utilities package wc, written by
Paul Rubin, phr@ocf.berkeley.edu and David MacKenzie, djm@gnu.ai.mit.edu.

See https://www.gnu.org/software/coreutils/coreutils.html for the original
sources.
*/

#include "linesinfile.h"

#include <stdio.h>
#include <sys/io.h>
#include <bsd/unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/file.h>
#include <limits.h>
#include <errno.h>

#ifdef EINTR
# define IS_EINTR(x) ((x) == EINTR)
#else
# define IS_EINTR(x) 0
#endif

#ifndef O_BINARY
#define O_BINARY  0
#define O_TEXT    0
#endif

//#define SET_BINARY(_f) do {if (!isatty(_f)) setmode (_f, O_BINARY);} while (0)

//#define SET_BINARY(_f) do {if (!isatty(_f)) setmode (_f);} while (0)

/* Size of atomic reads. */
#define BUFFER_SIZE (16 * 1024)
#define SAFE_READ_ERROR ((size_t) -1)

//typedef int ssize_t;

size_t safe_read (int fd, char *buf, unsigned int count) {
  /* Work around a bug in Tru64 5.1.  Attempting to read more than
     INT_MAX bytes fails with errno == EINVAL.  See
     <http://lists.gnu.org/archive/html/bug-gnu-utils/2002-04/msg00010.html>.
     When decreasing COUNT, keep it block-aligned.  */
  enum { BUGGY_READ_MAXIMUM = INT_MAX & ~8191 };

  for(;;) {
    ssize_t result = read (fd, buf, count);

  if (0 <= result)
    return result;
  else if (IS_EINTR (errno))
    continue;
  else if (errno == EINVAL && BUGGY_READ_MAXIMUM < count)
    count = BUGGY_READ_MAXIMUM;
  else
    return result;
  }
}


unsigned long long linesInFile( const char* filename, bool& ok ) {
  unsigned long long nLines = 0;
  bool test = true;

  int fd = open( filename, O_RDONLY );
  if( -1 == fd ) {
    test = false;
  }
  else {
    char buf[BUFFER_SIZE + 1];
    size_t bytes_read;

    /* We need binary input, since `wc' relies on `lseek' and byte counts.  */
//    SET_BINARY (fd);

   /* Use a separate loop when counting only lines or lines and bytes --
   but not chars or words.  */
    while( 0 < ( bytes_read = safe_read ( fd, buf, BUFFER_SIZE ) ) ) {
      register char *p = buf;

      if (bytes_read == SAFE_READ_ERROR) {
        //error (0, errno, "%s", file);
        test = false;
        break;
      }

      while ((p = (char*)memchr (p, '\n', (buf + bytes_read) - p))) {
        ++p;
        ++nLines;
      }
    }

    if( 0 != close( fd ) ) {
      test = false;
    }
  }

  ok = test;

  return nLines;
}
