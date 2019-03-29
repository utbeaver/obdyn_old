/*
 * Thread support for the SIP library.  This module provides the hooks for
 * C++ classes that provide a thread interface to interact properly with the
 * Python threading infrastructure.
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


#include "sip.h"
#include "sipint.h"


#ifdef WITH_THREAD

#include <pythread.h>


typedef struct _threadDef {
	long thr_ident;				/* The thread identifier. */
	PyThreadState *thr_state;		/* The thread state. */
	const void *cppPending;
	int cppPendingFlags;
	struct _threadDef *next;		/* Next in the list. */
} threadDef;


static threadDef *threads = NULL;		/* Link list of threads. */


static void startThread(void);
static threadDef *currentThreadDef(void);
static PyThreadState *currentThreadState(void);

#endif


static const void *cppPending = NULL;
static int cppPendingFlags;


/*
 * Get the address of any C/C++ object waiting to be wrapped.
 */
const void *sipGetPending(int *fp)
{
	const void *pend;
	int pendFlags;

#ifdef WITH_THREAD
	threadDef *td;

	if ((td = currentThreadDef()) != NULL)
	{
		pend = td -> cppPending;
		pendFlags = td -> cppPendingFlags;
	}
	else
	{
		pend = cppPending;
		pendFlags = cppPendingFlags;
	}
#else
	pend = cppPending;
	pendFlags = cppPendingFlags;
#endif

	if (pend != NULL)
		*fp = pendFlags;

	return pend;
}


/*
 * Convert a new C/C++ pointer to a Python instance.
 */
PyObject *sipNewCppToSelf(const void *cppPtr,PyObject *pyClass,int initflags)
{
	PyObject *w;

#ifdef WITH_THREAD
	threadDef *td;
#endif

	if (cppPtr == NULL)
	{
		PyErr_SetString(PyExc_RuntimeError,"Attempt to create a Python instance for a NULL pointer");

		return NULL;
	}

#ifdef WITH_THREAD
	if ((td = currentThreadDef()) != NULL)
	{
		td -> cppPending = cppPtr;
		td -> cppPendingFlags = initflags;
	}
	else
	{
		cppPending = cppPtr;
		cppPendingFlags = initflags;
	}
#else
	cppPending = cppPtr;
	cppPendingFlags = initflags;
#endif

	w = PyInstance_New(pyClass,NULL,NULL);

#ifdef WITH_THREAD
	if (td != NULL)
		td -> cppPending = NULL;
	else
		cppPending = NULL;
#else
	cppPending = NULL;
#endif

	return w;
}


#ifdef WITH_THREAD

/*
 * Find the Python member function corresponding to a C/C++ virtual function
 * for execution in the new current thread, if any.  If one was found then the
 * Python lock is acquired.
 */

int sipIsPyMethodThread(sipMethodCache *pymc,sipThisType *sipThis,char *cname,
			char *mname)
{
        int found;

        startThread();

        found = sipFindPyMethod(pymc,sipThis,cname,mname);

        if (!found)
                sipEndThread();

        return found;
}


/*
 * This is called from a newly created thread to integrate it with the Python
 * threading infrastructure.  It acquires the Python lock.
 */

static void startThread(void)
{
	threadDef *td;
	PyThreadState *tstate;

	/* Bootstrap the new thread. */

	tstate = PyThreadState_New(sipMainThreadState -> interp);
	PyEval_AcquireThread(tstate);

	/*
	 * Save the thread ID and the state so we can find the latter when we
	 * need to.  First, find an empty slot in the list.
	 */

	for (td = threads; td != NULL; td = td -> next)
		if (td -> thr_state == NULL)
			break;

	if (td == NULL)
	{
		td = sipMalloc(sizeof (threadDef));
		td -> next = threads;
		threads = td;
	}

	if (td != NULL)
	{
		td -> thr_state = tstate;
		td -> thr_ident = PyThread_get_thread_ident();
		td -> cppPending = NULL;
	}
}


/*
 * Handle the termination of a thread.
 */

void sipEndThread()
{
	threadDef *td;

	if ((td = currentThreadDef()) != NULL)
	{
		PyThreadState *tstate;

		/* Mark the entry as unused. */

		tstate = td -> thr_state;
		td -> thr_state = NULL;

		PyThreadState_Clear(tstate);

#if PY_VERSION_HEX >= 0x02010000
		PyThreadState_DeleteCurrent();
#else
		PyEval_ReleaseThread(tstate);
		PyThreadState_Delete(tstate);
#endif
	}
	else
		PyErr_SetString(PyExc_RuntimeError,"Unknown thread finished");
}


/*
 * This handles the change from C++-land to Python-land.  We acquire the Python
 * lock and restore the correct thread state.
 */

void sipBlockThreads(void)
{
	PyEval_RestoreThread(currentThreadState());
}


/*
 * Return the thread state for the current thread.  If we don't recognise the
 * the thread we assume that it is the main thread.
 */

static PyThreadState *currentThreadState(void)
{
	threadDef *td;

	if ((td = currentThreadDef()) != NULL)
		return td -> thr_state;

	return sipMainThreadState;
}


/*
 * Return the thread data for the current thread or NULL if it wasn't
 * recognised.
 */

static threadDef *currentThreadDef(void)
{
	threadDef *td;
	long ident = PyThread_get_thread_ident();

	for (td = threads; td != NULL; td = td -> next)
		if (td -> thr_state != NULL && td -> thr_ident == ident)
			break;

	return td;
}

#endif
