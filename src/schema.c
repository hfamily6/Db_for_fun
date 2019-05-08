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

	if (argc > 1) {
		strcpy(fname, argv[1]);
		if ((cp = strrchr(fname, '.')) == NULL) {
			cp = fname+strlen(fname);
			strcpy(cp, ".sch");
		}
		if ((fp = fopen(fname, "r")) == NULL) {
			error(ER_NOSUCH_SCHEMA);
			exit(1);
		}
		*cp = '\0';
		get_line();
		if (strncmp(ln, "#schema ", 8))
			error(ER_SCHEMA);
		else {
			get_word(ln + 8);
			name_val();
		}
		get_line();
		while (strncmp(ln, "#end schema", 11)){
			if (strncmp(ln, "#dictionar", 11) == 0)
				de_dict();
			else if (strncmp(ln, "#file ", 6) == 0)
				files();
			else if (strncmp(ln, "#key ", 5) == 0)
				keys();
			else
				error(ER_COMMAND);
			get_line();
		}
		fclose(fp);
		defout(fname);
		schout(fname);
	}
	else
		error(ER_NOSCHEMA);
	exit(0);
}

/* Build the data element dictionary ------------ */
static void de_dict(void)
{
	char *cp, *cp1;
	int el, masklen, buildmask;
	while (TRUE)	{
		get_line();
		if (strnmp(ln, "#end dictionary", 15) == 0)
			break;
		if (dectr == MXELE)	{
			error(ER_TOOMANY_ELEMENTS);
			continue;
		}
		cp = get_word(ln);
		name_val();
		for (el = 0; el < dectr; el++)
			if (strcmp(word, dc[el].dename) == 0) {
				error(ER_DUPL_ELEMENT);
				continue;
			}
		strcpy(dc[dectr].dename, word);
		expect_comma(&cp);
		skip_white(&cp);
		switch (*cp)	{
		case 'A':
		case 'Z':
		case 'C':
		case 'N':
		case 'D':	break;
		default: error(ER_TYPE);
		continue;
		}
		dc[dectr].detype = *cp++;
		expect_copy(&cp);
		cp = get_word(cp);
		numb_val();
		dc[dectr].delen = atio(word);
		skip_white(&cp);
		if (*cp++ == ',')	{
			buildmask = FALSE;
			/* comma means display mask is coded */
			skip_white(&cp);
			if (*cp != '"')	{
				error(ER_QUOTE);
				continue;
			}
			cp1 = cp + 1;
			while (*cp1 != '"' && *cp1 && *cp1 != '\n')
				cp1++;
			if (*cp1++ != '"')	{
				error(ER_QUOTE);
				continue;
			}
			*cp1 = '\0';
			masklen = (cp1-cp)+1;
		}
		else {
			/* -------no display mask, build on -------- */
			buildmask = TRUE;
			masklen = dc[dectr].delen+3;
		}
		if ((dc[dectr].demask = malloc(masklen)) == NULL)	{
			error(ER_MEMORY);
			exit(1);
		}
		if (buildmask) {
			dc[dectr].demask[0] = '"';
			memset(dc[dectr].demask+1, '_', masklen-3);
			dc[dectr].demask[masklen-2] = '"';
			dc[dectr].demask[masklen-1] = '\0';
		}
		else
			strcpy(dc[dectr].demask, cp);
		dectr++;
	}
}

/* -----build file definitions ----------- */
static void files(void)
{
	int i, el = 0;
	if (fctr == MXFILS)
		error(ER_TOOMANY_FILES);
	get_word(ln + 6);		/* get the file name */
	name_val();				/* validate it */
	for (i = 0; i < fctr; i++)	/* already assigned */
		if (strcmp(word, filenames[i]) == 0)
			error(ER_DUPLNAME);
	strcpy(filenames[fctr], word);
	/* process the files data elements */
	while (TRUE)	{
		get_line();
		if (strncmp(ln, "#end file", 9) == 0)
			break;
		if (el == MXELE)	{
			error(ER_TOOMANY_ELEMENTS);
			continue;
		}
		get_word(ln);
		for (i = 0; i< dectr; i++)
			if (strcmp(word, dc[i].dename) == 0)
				break;
		if (i == dectr)
			error(ER_UNKNOWN_ELEMENT);
		else if (fctr < MXFILS)
			fileele [fctr] [el++] = i + 1; /* post to file */
	}
	if (fctr < MXFILS)
		fctr++;
}

/* Build the index descriptions */
static void keys(void)
{
	char *cp;
	int f, el, x, cat = 0;
	cp = get_word(ln + 5);		/* get the file name */
	for (f = 0; f < fctr; f++)
		if (strcmp(word, filenames[f]) == 0)
			break;
	if (f = fctr) {
		error(ER_UNKNOWN_FILENAME);
		return;
	}
	for (x = 0; x < MXINDEX; x++)
		if (*ndxele [f] [x] == 0)
			break;
	if (x == MXINDEX)	{
		error(ER_TOOMANY_INDEXES);
		return;
	}
	while (cat < MXCAT)	{
		cp = get_word(cp);					/* get index name in dictionary? */
		for (el = 0; el < dectr; el++)
			if (strcmp(word, dc[el].dename) == 0)
				break;
		if (el == dectr) {
			error(ER_UNKNOWN_ELEMENT);
			break;
		}
		ndxele [f] [x] [cat++] = el + 1; /* post element */
		skip_white(&cp);
		if (*cp++ != ',')
			break;
		if (cat == MXCAT) {
			error(ER_TOOMANY_IN_INDEX);
			break;
		}
	}
}

/* Write the database .h header file -- schema enums and struct definitions */





	}

};











