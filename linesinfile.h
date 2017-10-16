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

See https://www.gnu.org/software/coreutils/coreutils.html
and http://gnuwin32.sourceforge.net/packages/coreutils.htm
for the original sources.
*/

#ifndef LINESINFILE_H
#define LINESINFILE_H

/*
 * Returns the number of lines in the indicated file.
 * Code is based on the GNU utility wc (with option -l), so
 * number of lines is calculated independently of any Qt code.
 *
 * If Qt encounters a null character (0x00)
 * in a (badly formatted!) plain-text file, Qt treats this as
 * the end of the file.  This provides one way to double-check
 * that a file was completely processed.
 *
 * Parameters:
 *   filename: name of an existing file
 *   ok: if false, an error occurred
 *
 * Return: number of lines in the indicated file
 */
unsigned long long linesInFile( const char* filename, bool& ok );

#endif // LINESINFILE_H
