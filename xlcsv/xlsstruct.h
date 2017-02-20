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

#ifndef XLSSTRUCT_H
#define XLSSTRUCT_H

#pragma pack(1)

#include <ar_general_purpose/xlcsv/ole.h>

typedef struct BOF
{
    XLSWORD id;
    XLSWORD size;
}
BOF;

typedef struct BIFF
{
    XLSWORD ver;
    XLSWORD type;
    XLSWORD id_make;
    XLSWORD year;
    XLSDWORD flags;
    XLSDWORD min_ver;
    XLSBYTE buf[100];
}
BIFF;

typedef struct WIND1
{
    XLSWORD xWn;
    XLSWORD yWn;
    XLSWORD dxWn;
    XLSWORD dyWn;
    XLSWORD grbit;
    XLSWORD itabCur;
    XLSWORD itabFirst;
    XLSWORD ctabSel;
    XLSWORD wTabRatio;
}
WIND1;

typedef struct BOUNDSHEET
{
    XLSDWORD	filepos;
    XLSBYTE	type;
    XLSBYTE	visible;
    XLSBYTE	name[];
}
BOUNDSHEET;

typedef struct ROW
{
    XLSWORD	index;
    XLSWORD	fcell;
    XLSWORD	lcell;
    XLSWORD	height;
    XLSWORD	notused;
    XLSWORD	notused2; //used only for BIFF3-4
    XLSWORD	flags;
    XLSWORD	xf;
}
ROW;

typedef struct COL
{
    XLSWORD	row;
    XLSWORD	col;
    XLSWORD	xf;
}
COL;


typedef struct FORMULA
{
    XLSWORD	row;
    XLSWORD	col;
    XLSWORD	xf;
    //	ULLONG  res;
    XLSBYTE	resid;
    XLSBYTE	resdata[5];
    XLSWORD	res;
    //	double	res;
    XLSWORD	flags;
    XLSBYTE	chn[4];
    XLSWORD	len;
    XLSBYTE	value[1]; //var
}
FORMULA;

typedef struct RK
{
    XLSWORD	row;
    XLSWORD	col;
    XLSWORD	xf;
    XLSBYTE	value[4];
}
RK;

typedef struct LABELSST
{
    XLSWORD	row;
    XLSWORD	col;
    XLSWORD	xf;
    XLSBYTE	value[4];
}
LABELSST;


typedef struct SST
{
    XLSDWORD	num;
    XLSDWORD	numofstr;
    XLSBYTE	strings;
}
SST;

typedef struct XF5
{
    XLSWORD	font;
    XLSWORD	format;
    XLSWORD	type;
    XLSWORD	align;
    XLSWORD	color;
    XLSWORD	fill;
    XLSWORD	border;
    XLSWORD	linestyle;
}
XF5;

typedef struct XF8
{
    XLSWORD	font;
    XLSWORD	format;
    XLSWORD	type;
    XLSBYTE	align;
    XLSBYTE	rotation;
    XLSBYTE	ident;
    XLSBYTE	usedattr;
    XLSDWORD	linestyle;
    XLSDWORD	linecolor;
    XLSWORD	groundcolor;
}
XF8;

typedef struct BR_NUMBER
{
    XLSWORD	row;
    XLSWORD	col;
    XLSWORD	xf;
    double value;
}
BR_NUMBER;

typedef struct COLINFO
{
    XLSWORD	first;
    XLSWORD	last;
    XLSWORD	width;
    XLSWORD	xf;
    XLSWORD	flags;
    XLSWORD	notused;
}
COLINFO;

typedef struct MERGEDCELLS
{
    XLSWORD	rowf;
    XLSWORD	rowl;
    XLSWORD	colf;
    XLSWORD	coll;
}
MERGEDCELLS;

typedef struct FONT
{
    XLSWORD	height;
    XLSWORD	flag;
    XLSWORD	color;
    XLSWORD	bold;
    XLSWORD	escapement;
    XLSBYTE	underline;
    XLSBYTE	family;
    XLSBYTE	charset;
    XLSBYTE	notused;
    XLSBYTE	name;
}
FONT;

typedef struct FORMAT
{
    XLSWORD	index;
    XLSBYTE	value[1];
}
FORMAT;

//---------------------------------------------------------
typedef	struct st_sheet_data
{
    XLSDWORD filepos;
    XLSBYTE visibility;
    XLSBYTE type;
    char* name;
}
st_sheet_data;

typedef	struct st_sheet
{		//Sheets
    long count;
    st_sheet_data* sheet;
}
st_sheet;

typedef struct st_font_data
{
    XLSWORD	height;
    XLSWORD	flag;
    XLSWORD	color;
    XLSWORD	bold;
    XLSWORD	escapement;
    XLSBYTE	underline;
    XLSBYTE	family;
    XLSBYTE	charset;
    char*	name;
}
st_font_data;

typedef	struct st_font
{
    long count;		//Count of FONT's
    st_font_data* font;
}
st_font;

typedef struct st_format_data
{
     XLSWORD index;
     char *value;
}
st_format_data;

typedef struct st_format
{
    long count;		//Count of FORMAT's
    st_format_data* format;
}
st_format;

typedef struct st_xf_data
{
    XLSWORD	font;
    XLSWORD	format;
    XLSWORD	type;
    XLSBYTE	align;
    XLSBYTE	rotation;
    XLSBYTE	ident;
    XLSBYTE	usedattr;
    XLSDWORD	linestyle;
    XLSDWORD	linecolor;
    XLSWORD	groundcolor;
}
st_xf_data;

typedef	struct st_xf
{
    long count;	//Count of XF
    //	XF** xf;
    st_xf_data* xf;
}
st_xf;


typedef	struct str_sst_string
{
    //	long len;
    char* str;
}
str_sst_string;


typedef	struct st_sst
{
    XLSDWORD count;
    XLSDWORD lastid;
    XLSDWORD continued;
    XLSDWORD lastln;
    XLSDWORD lastrt;
    XLSDWORD lastsz;
    str_sst_string* string;
}
st_sst;


typedef struct st_cell_data {
    XLSWORD	id;
    XLSWORD	row;
    XLSWORD	col;
    XLSWORD	xf;
    double	d;
    long	l;
    char*	str;		//String value;
    XLSBYTE	ishidden;		//Is cell hidden
    XLSWORD	width;		//Width of col
    XLSWORD	colspan;
    XLSWORD	rowspan;
}
st_cell_data;

typedef	struct st_cell
{
    XLSDWORD count;
    st_cell_data* cell;
}
st_cell;


typedef struct st_row_data
{
    XLSWORD index;
    XLSWORD fcell;
    XLSWORD lcell;
    XLSWORD height;
    XLSWORD flags;
    XLSWORD xf;
    XLSBYTE xfflags;
    st_cell cells;
}
st_row_data;


typedef	struct st_row
{
    //	DWORD count;
    XLSWORD lastcol;
    XLSWORD lastrow;
    st_row_data * row;
}
st_row;


typedef struct st_colinfo_data
{
    XLSWORD	first;
    XLSWORD	last;
    XLSWORD	width;
    XLSWORD	xf;
    XLSWORD	flags;
}
st_colinfo_data;

typedef	struct st_colinfo
{
    long count;	//Count of COLINFO
    st_colinfo_data* col;
}
st_colinfo;

typedef struct xlsWorkBook
{
    //FILE*		file;		//
    OLE2Stream*	olestr;
    long		filepos;	//position in file

    //From Header (BIFF)
    XLSBYTE		is5ver;
    XLSWORD		type;

    //Other data
    XLSWORD		codepage;	//Charset codepage
    XLSWORD is1904datesystem;
    char*		charset;
    st_sheet	sheets;
    st_sst		sst;		//SST table
    st_xf		xfs;		//XF table
    st_font		fonts;
    st_format	formats;	//FORMAT table
}
xlsWorkBook;

typedef struct xlsWorkSheet
{
    XLSDWORD		filepos;
    XLSWORD		defcolwidth;
    st_row		rows;
    xlsWorkBook * 	workbook;
    st_colinfo	colinfo;
    XLSWORD		maxcol;
}
xlsWorkSheet;

#pragma pack()

#endif // XLSSTRUCT_H
