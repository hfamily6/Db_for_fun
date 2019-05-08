/* cdata.h */

#include <stdio.h>

#ifndef CDATA_H
#define CDATA_H

#define MXFILS		11   /* Max # of files in a database */
#define MXELE		100  /* Max data elements in a file */
#define MXINDEX		5	/* max indexes */
#define MXKEYLEN	80 	/* max key length */
#define MXCAT 		3  /* Max elements per index */
#define NAMLEN		31  /* data elements name length */

/* init for database errors */
extern void (*database_message)(void);

#define ERROR -1
#define OK 0

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

typedef long RPTR; 		/* B-tree node and file address */

#ifndef APPLICATION_H
typedef int DBFILE;
typedef int ELEMENT;
#endif

/* schema as build for application */
extern const char *dbfiles[];		/* file names */
extern const char *denames[];		/* data element names */
extern const char *elmask[];		/* data element masks */
extern const char *eltype[];		/* data element types */
extern const int ellen[]; 			/* data element lengths */
extern const ELEMENT *file_ele[];	/* file data elements */
extern const ELEMENT **index_ele[]; /* index data elements */

/* database properties */

/* Cdata API functions */
void db_open(const char *, const DBFILE *);
int add_rcd(DBFILE, void *);
int find_rcd(DBFILE, int, char *, void *);
int verify_rcd(DBFILE, int, char *);
int first_rcd(DBFILE, int, void *);
int last_rcd(DBFILE, int, void *);
int next_rcd(DBFILE, int, void *);
int prev_rcd(DBFILE, int, void *);
int rtn_rcd(DBFILE, void *);
int del_rcd(DBFILE);
int curr_rcd(DBFILE, int, void *);
int seqrcd(DBFILE, void *);
void db_cls(void);
void dberror(void);
int rlen(DBFILE);
void init_rcd(DBFILE, void *);
void clrrcd(void *, const ELEMENT *);
int epos(ELEMENT, const ELEMENT *);
void rcd_fill(const void *, void *, const ELEMENT *, const ELEMENT *);


/* functions used by cdata utility programs */
void build_index(char *, DBFILE);
int add_indexes(DBFILE, void *, RPTR);
DBFILE filename(char *);
void name_cvt(char *, char *);
int ellist(int, char **, ELEMENT *);
void clist(FILE *, const ELEMENT *, const ELEMENT *, void *, const char *);
void test_eop(FILE *, const char *, const ELEMENT *);
void dblist(FILE *, DBFILE, int, const ELEMENT *);


/* dbms error codes for errno return */
enum dberrors {
	D_NF=1, 	/* record not found */
	D_PRIOR,	/* no prior record for this request */
	D_EOF,		/* end of file */
	D_BOF,		/* Begining of file */
	D_DUPL,		/* primary key already exists */
	D_OM,		/* out of memory */
	D_INDXC,	/* index corrupted */
	D_IOERR		/* i/o error */
};

#endif
