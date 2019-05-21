/* ---- cdata */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "cdata.h"
/* #include "datafile.h"
#include "btree.h"
#include "keys.h" */

void (*database_message)(void);
static void init_index(const char *, const DBFILE);
static void cls_index(DBFILE);
static void del_indexes(DBFILE, RPTR);
static int relate_rcd(DBFILE, void *);
static int data_in(char *);
static int getrc(DBFILE, RPTR, void *);
static int rel_rcd(DBFILE, RPTR, void *);

static int db_opened = FALSE;  /* database opened indicator */
static int curr_fd [MXFILS];  /* Current file descriptor */
static char *bfs [MXFILS];  /* File i/o buffers */
static int bfd [MXFILS] [MXINDEX];
static char dbpath [64];

RPTR curr_a [MXFILS];
/* Macro to compute tree number from file and key number - */
#define treen(f,k) (bfd[f][(k)-1])

/* ---- open the database ---- */
void db_open(const char *path, const DBFILE *fl)
{
	char fnm[64];
	int i;

	if (!db_opened)	{
		for (i = 0; i < MXFILS; i++)
			curr_fd [i] = -1;
		db_opened = TRUE;
	}
	strcpy(dbpath, path);
	while (*fl != -1)	{
		sprintf(fnm, "%s%.8s.dat", path, dbfiles [*fl]);
		curr_fd [*fl] = file_open(fnm);
		init_index(path, *fl);
		if ((bfs [*fl] = malloc(rlen(*fl))) == NULL)		{
				errno = D_OM;
				dberror();
		}
		fl++;
	}
}

/* ---- Add a record to a file */
int add_rcd(DBFILE f, void *bf)
{
	RPTR ad;
	int rtn;

	if ((rtn = relate_rcd(f, bf)) != ERROR) 	{
		ad = new_record(curr_fd [f], bf);
		if ((rtn = add_indexes(f, bf, ad)) == ERROR)	{
			errno = D_DUPL;
			delete_record(curr_fd [f], ad);
		}
	}
	return rtn;
}

/* ---find a record */
int find_rcd(DBFILE f, int k, char *key, void *bf)
{
	RPTR ad;

	if ((ad = locate(treeno(f,k), key)) == 0)	{
		errno = D_NF;
		return ERROR;
	}
	return getrcd(f, ad, bf);
}

/* --- verify record is in a file */
int verify_rcd(DBFILE f, int k, char *key)
{
	if (locate(treeno(f,k), key) == 0)	{
		errno = D_NF;
		return ERROR;
	}
	return OK;
}

/* -- find the first indexed record in a file */
int first_rcd(DBFILE f, int k, void *bf)
{
	RPTR ad;

	if ((ad = firstkey(treeno(f,k))) == 0) {
		errno = D_EOF;
		return ERROR;
	}
	return getrcd(f, ad, bf);
}

/* find the last indexed record in a file */
int last_rcd(DBFILE f, int k, void *bf)
{
	RPTR ad;

	if ((ad = lastkey(treeno(f,k))) == 0)	{
		errno = D_BOF;
		return ERROR;
	}
	return getrcd(f, ad, bf);
}

/* find the next record in a file */
int next_rcd(DBFILE f, int k, void *bf)
{
	RPTR ad;

	if ((ad = nextkey(treeno(f,k))) == 0)	{
		errno = D_EOF;
		return ERROR;
	}
	return getrcd(f, ad, bf);
}




