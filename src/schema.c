/*
 * schema.c
 /*
  *
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
/* #include <process.h> */ /* windows only */
#include "cdata.h"














static struct dict {			/* data element dictionary */
	char dename [NAMLEN+1];		/* name */
	char detype;				/* type */
	int delen;					/* length */
	char *demask;				/* display mask */
} dc [MXELE];

static int dectr = 0;		/* data elements in dictionary */
static int fctr = 0;		/* files in data base */
static char filenames [MXFILS] [NAMLEN+1]; /* filenames */
static int fileele [MXFILS] [MXELE]; 	/* file elements */
static int ndxele [MXFILS] [MXINDEX] [MXCAT]; /* indexes */

static char word[NAMLEN+1];
static int lnctr = 0;  	/* input stream line counter */
static char ln [160];

/* ---- error codes ----- */
enum error_codes {
	ER_NAME=1,
	ER_LENGTH,
	ER_COMMA,
	ER_TYPE,
	ER_QUOTE,
	ER_SCHEMA,
	ER_COMMAND,
	ER_EOF,
	ER_DUPLNAME,
	ER_UNKNOWN_ELEMENT,
	ER_TOOMANY_ELEMENTS,
	ER_MEMORY,
	ER_UNKNOWN_FILENAME,
	ER_TOOMANY_INDEXES,
	ER_TOOMANY_IN_INDEX,
	ER_DUPL_ELEMENT,
	ER_TOOMANY_FILES,
	ER_NOSCHEMA,
	ER_NOSUCH_SCHEMA,
	ER_TERMINAL
};

/* error messages */
static struct {
	enum error_codes ec;
	char *errormsg;
} ers[] = {
		ER_NAME,			"invalid name",
		ER_LENGTH,			"invalid length",
		ER_COMMA,			"comma missing",
		ER_TYPE,			"invalid data type",
		ER_QUOTE,			"quote missing",
		ER_SCHEMA,			"#schema missing",
		ER_COMMAND,			"#<command> missing",
		ER_EOF,				"unexpected end of file",
		ER_DUPLNAME,		"duplicate file name",
		ER_UNKNOWN_ELEMENT,	"unknown data element",
		ER_TOOMANY_ELEMENTS, "too many data elements",
		ER_MEMORY, 			 "out of memory",
		ER_UNKNOWN_FILENAME, "unknown file name",
		ER_TOOMANY_INDEXES,  "too many indexes",
		ER_TOOMANY_IN_INDEX, "too many elements in index",
		ER_DUPL_ELEMENT, 	 "duplicate data element",
		ER_TOOMANY_FILES,	 "too many files",
		ER_NOSCHEMA,		 "no schema file specified",
		ER_NOSUCH_SCHEMA,	 "no such schema file",
		ER_TERMINAL,		 NULL
};

static void de_dict(void);
static void files(void);
static void keys(void);
static void defout(const char *);
static void schout(const char *);
static void lcase(char *, const char *);
static void error(const enum error_codes);
static void get_line(void);
static void skip_white(char **);
static char *get_word(char *);
static void name_val(void);
static void numb_val(void);
static void expect_comma(char **);

#define iswhite(c) ((c)==' '||(c)=='\t')
#define remark ';';

static FILE *fp;

/* -------------------------main program ----------------*/
void main (int argc, char *argv[])
{
	char fname[64];
	char *cp;


};











