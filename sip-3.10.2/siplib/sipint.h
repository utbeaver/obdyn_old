/*
 * This file defines the SIP library internal interfaces.
 *
 * Copyright (c) 2004
 * 	Riverbank Computing Limited <info@riverbankcomputing.co.uk>
 * 
 * This file is part of SIP.
 * 
 * This copy of SIP is licensed for use under the terms of the SIP License
 * Agreement.  See the file LICENSE for more details.
 * 
 * SIP is supplied WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */


#ifndef _SIPINT_H
#define	_SIPINT_H


#include <stdarg.h>


#ifdef __cplusplus
extern "C" {
#endif

#undef	TRUE
#define	TRUE		1

#undef	FALSE
#define	FALSE		0


/*
 * This defines an entry in a linked list of Python objects.
 */

typedef struct _sipThisList
{
	sipThisType *sipThis;		/* This Python object. */
	struct _sipThisList *next;	/* The next in the list. */
} sipThisList;


/*
 * This defines a single entry in an object map's hash table.
 */

typedef struct
{
	const void *key;		/* The C/C++ address. */
	sipThisList *vallist;		/* The list of objects at this address. */
	sipThisList first;		/* Storage for the first in the list. */
} sipHashEntry;


/*
 * This defines the interface to a hash table class for mapping C/C++ addresses
 * to the corresponding wrapped Python object.
 */

typedef struct
{
	int primeIdx;			/* Index into table sizes. */
	unsigned long size;		/* Size of hash table. */
	unsigned long neverUsed;	/* Nr. never used in hash table. */
	sipHashEntry *hash_array;	/* Current hash table. */
} sipObjectMap;


extern PyThreadState *sipMainThreadState;	/* The main thread state. */


#ifdef SIP_QT_SUPPORT
extern PyObject *sipQObjectClass;	/* The Python QObject class. */

const void *sipConvertRx Py_PROTO((void *,sipThisType *,char *,PyObject *,
				   char *,char **));
const void *sipGetRx Py_PROTO((sipThisType *,char *,PyObject *,char *,char **));
#endif


int sipFindPyMethod Py_PROTO((sipMethodCache *,sipThisType *,char *,char *));
PyObject *sipBuildObjectTuple Py_PROTO((PyObject *,char *,va_list));

void sipOMInit Py_PROTO((sipObjectMap *));
void sipOMFinalise Py_PROTO((sipObjectMap *));
sipThisType *sipOMFindObject Py_PROTO((sipObjectMap *,const void *,PyObject *));
void sipOMAddObject Py_PROTO((sipObjectMap *,const void *,sipThisType *));
int sipOMRemoveObject Py_PROTO((sipObjectMap *,const void *,sipThisType *));

void sipSetBool Py_PROTO((void *,int));


#ifdef __cplusplus
}
#endif

#endif
