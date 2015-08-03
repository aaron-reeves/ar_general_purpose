#ifndef FILEMAGIC_H
#define FILEMAGIC_H

#include <inttypes.h>

#include <magic.h>

#include <qstring.h>

/*
 * High-level, user-friendly functions
 * -----------------------------------
 */
bool magicIsAsciiTextFile( QString fileName, bool* error = NULL, QString* returnTypeInfo = NULL );


/*
 * More versatile, lower-level functions
 * -------------------------------------
 */
struct magic_set;
magic_set* magicLoadMagic( QString magicFile, int flags );
bool magicProcess( struct magic_set* ms, QString fileName, QString& fileTypeInfo, QString& errMsg );
void magicCloseMagic( struct magic_set* ms );



/*
 * Bits and pieces of the interface to the magic library.
 * These can be considered internal, and mostly ignored.
 * ------------------------------------------------------
 */
#define MAXstring 32		/* max leng of "string" types */

union VALUETYPE {
  uint8_t b;
  uint16_t h;
  uint32_t l;
  uint64_t q;
  uint8_t hs[2];	/* 2 bytes of a fixed-endian "short" */
  uint8_t hl[4];	/* 4 bytes of a fixed-endian "long" */
  uint8_t hq[8];	/* 8 bytes of a fixed-endian "quad" */
  char s[MAXstring];	/* the search string or regex pattern */
  unsigned char us[MAXstring];
  float f;
  double d;
};


/* list of magic entries */
struct mlist {
  struct magic *magic;		/* array of magic entries */
  uint32_t nmagic;			/* number of entries in array */
  int mapped;  /* allocation type: 0 => apprentice_file
          *                  1 => apprentice_map + malloc
          *                  2 => apprentice_map + mmap */
  struct mlist *next, *prev;
};


struct level_info {
  int32_t off;
  int got_match;
  int last_match;
  int last_cond;	/* used for error checking by parse() */
};


struct magic_set {
  struct mlist *mlist;
  struct cont {
    size_t len;
    struct level_info *li;
  } c;
  struct out {
    char *buf;		/* Accumulation buffer */
    char *pbuf;		/* Printable buffer */
  } o;
  uint32_t offset;
  int error;
  int flags;			/* Control magic tests. */
  int event_flags;		/* Note things that happened. */
#define 		EVENT_HAD_ERR		0x01
  const char *file;
  size_t line;			/* current magic line number */

  /* data for searches */
  struct {
    const char *s;		/* start of search in original source */
    size_t s_len;		/* length of search region */
    size_t offset;		/* starting offset in source: XXX - should this be off_t? */
    size_t rm_len;		/* match length */
  } search;

  /* FIXME: Make the string dynamically allocated so that e.g.
     strings matched in files can be longer than MAXstring */
  union VALUETYPE ms_value;	/* either number or string */
};


#endif // #FILEMAGIC_H
