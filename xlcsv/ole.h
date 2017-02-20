/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * This file is part of libxls -- A multiplatform, C library
 * for parsing Excel(TM) files.
 *
 * libxls is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libxls is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libxls.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * Copyright 2004 Komarov Valery
 * Copyright 2006 Christophe Leitienne
 * Copyright 2008 David Hoerl
 */

#ifndef OLE_INCLUDE
#define OLE_INCLUDE

#pragma pack(1)

#include <stdio.h>			// FILE *
#include <ar_general_purpose/xlcsv/xlstypes.h>

typedef struct TIME_T
{
    XLSDWORD	LowDate;
    XLSDWORD	HighDate;
}
TIME_T;

typedef struct OLE2Header
{
    XLSDWORD		id[2];		//D0CF11E0 A1B11AE1
    XLSDWORD		clid[4];
    XLSWORD		verminor;	//0x3e
    XLSWORD		verdll;		//0x03
    XLSWORD		byteorder;
    XLSWORD		lsectorB;
    XLSWORD		lssectorB;

    XLSWORD		reserved1;
    XLSDWORD		reserved2;
    XLSDWORD		reserved3;

    XLSDWORD		cfat;			// count full sectors
    XLSDWORD		dirstart;

    XLSDWORD		reserved4;

    XLSDWORD		sectorcutoff;	// min size of a standard stream ; if less than this then it uses short-streams
    XLSDWORD		sfatstart;		// first short-sector or EOC
    XLSDWORD		csfat;			// count short sectors
    XLSDWORD		difstart;		// first sector master sector table or EOC
    XLSDWORD		cdif;			// total count
    XLSDWORD		MSAT[109];		// First 109 MSAT
}
OLE2Header;


//-----------------------------------------------------------------------------------
typedef struct st_olefiles_data
{
    char*	name;
    XLSDWORD	start;
    XLSDWORD	size;
}
st_olefiles_data;

typedef	struct st_olefiles
{
    long count;
    st_olefiles_data* file;
}
st_olefiles;

typedef struct OLE2
{
    FILE*		file;
    XLSWORD		lsector;
    XLSWORD		lssector;
    XLSDWORD		cfat;
    XLSDWORD		dirstart;

    XLSDWORD		sectorcutoff;
    XLSDWORD		sfatstart;
    XLSDWORD		csfat;
    XLSDWORD		difstart;
    XLSDWORD		cdif;
    XLSDWORD*		SecID;	// regular sector data
	XLSDWORD*		SSecID;	// short sector data
	XLSBYTE*		SSAT;	// directory of short sectors
    st_olefiles	files;
}
OLE2;

typedef struct OLE2Stream
{
    OLE2*	ole;
    XLSDWORD	start;
    XLSDWORD	pos;
    int		cfat;
    int		size;
    XLSDWORD	fatpos;
    XLSBYTE*	buf;
    XLSDWORD	bufsize;
    XLSBYTE	eof;
	XLSBYTE	sfat;	// short
}
OLE2Stream;

typedef struct PSS
{
    XLSBYTE	name[64];
    XLSWORD	bsize;
    XLSBYTE	type;		//STGTY
#define PS_EMPTY		00
#define PS_USER_STORAGE	01
#define PS_USER_STREAM	02
#define PS_USER_ROOT	05
    XLSBYTE	flag;		//COLOR
#define BLACK	1
    XLSDWORD	left;
    XLSDWORD	right;
    XLSDWORD	child;
    XLSWORD	guid[8];
    XLSDWORD	userflags;
    TIME_T	time[2];
    XLSDWORD	sstart;
    XLSDWORD	size;
    XLSDWORD	proptype;
}
PSS;

extern int ole2_read(void* buf,long size,long count,OLE2Stream* olest);
extern OLE2Stream* ole2_sopen(OLE2* ole,XLSDWORD start, int size);
extern void ole2_seek(OLE2Stream* olest,XLSDWORD ofs);
extern OLE2Stream*  ole2_fopen(OLE2* ole,const char* file);
extern void ole2_fclose(OLE2Stream* ole2st);
extern OLE2* ole2_open(const char *file, const char *charset);
extern void ole2_close(OLE2* ole2);
extern void ole2_bufread(OLE2Stream* olest);

#pragma pack()

#endif
