// The SIP library code that implements the main Qt support.
//
// Copyright (c) 2004
// 	Riverbank Computing Limited <info@riverbankcomputing.co.uk>
// 
// This file is part of SIP.
// 
// This copy of SIP is licensed for use under the terms of the SIP License
// Agreement.  See the file LICENSE for more details.
// 
// SIP is supplied WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.


#ifdef SIP_QT_SUPPORT

#include <string.h>
#include <stdarg.h>

#include "sipQt.h"
#include "sipint.h"


// This is how Qt "types" signals and slots.

#define	isQtSlot(s)	(*(s) == '1')
#define	isQtSignal(s)	(*(s) == '2')
#define	isQtSigSlot(s)	(isQtSlot(s) || isQtSignal(s))


const QObject *sipProxy::sipSender = 0;		// The last QObject::sender().


static sipProxy *proxyList = NULL;		// List of proxies.


static int isSameSlot(sipSlot *,PyObject *,char *);
static int emitQtSig(sipThisType *,char *,PyObject *);
static int emitToSlot(sipSlot *,PyObject *);
static int emitToSlotList(sipPySigRx *,PyObject *);
static int addSlotToPySigList(sipThisType *,char *,PyObject *,char *);
static void removeSlotFromPySigList(sipThisType *,char *,PyObject *,char *);
static QObject *findProxy(sipThisType *,char *,PyObject *,char *,char **);
static const void *convertPythonRx(void *,sipThisType *,char *,PyObject *,char *,char **);
static PyObject *connectToPythonSlot(sipThisType *,char *,PyObject *);
static PyObject *disconnectFromPythonSlot(sipThisType *,char *,PyObject *);
static PyObject *doDisconnect(sipThisType *,char *,QObject *,char *);
static sipPySig *findPySignal(sipThisType *,char *);
static char *sipStrdup(char *);
static int setSlot(sipSlot *,PyObject *,char *);
static int sameSigSlotName(char *,char *);


// sipProxy constructor.

sipProxy::sipProxy()
{
	sipRealSlot.name = NULL;
	sipRealSlot.weakSlot = NULL;
	sipTxThis = NULL;
	sipTxSig = NULL;

	// Add it to the head of the linked list of proxies.

	if ((sipNext = proxyList) != NULL)
		sipNext -> sipPrev = this;

	sipPrev = NULL;
	proxyList = this;
}


// sipProxy destructor.

sipProxy::~sipProxy()
{
	if (sipRealSlot.name != NULL)
		sipFree((ANY *)sipRealSlot.name);

	Py_XDECREF(sipRealSlot.weakSlot);

	if (sipTxSig != NULL)
		sipFree((ANY *)sipTxSig);

	// Remove it from the linked list of proxies.

	if (sipNext != NULL)
		sipNext -> sipPrev = sipPrev;

	if (sipPrev != NULL)
		sipPrev -> sipNext = sipNext;
	else
		proxyList = sipNext;
}


// Convert receiver and slot objects to the base QObject derived receiver.

char *sipProxy::searchProxySlotTable(char **tab,char *sigargs)
{
	char *args;

	// Find the start of the signal arguments.

	if ((args = strchr(sigargs,'(')) != NULL)
		while ((sipRxSlot = *tab++) != NULL)
			if (sameSigSlotName(strchr(sipRxSlot,'('),args))
			{
				// Save the signal and slot names so that they
				// can be used later to disconnect.

				if ((sipTxSig = sipStrdup(sigargs)) == NULL)
					return NULL;

				return sipRxSlot;
			}

	PyErr_Format(PyExc_RuntimeError,"Signal has wrong argument types for slot");

	return NULL;
}


// Return the last signal sender.

const void *sipGetSender()
{
	return sipProxy::sipSender;
}


// Compare two signal/slot names and return non-zero if they match.

static int sameSigSlotName(char *s1,char *s2)
{
	// moc formats signal names, so we should first convert the supplied
	// string to the same format before comparing them.  Instead we just
	// compare them as they are, but ignoring all spaces - this will have
	// the same result.

	do
	{
		// Skip any spaces.

		while (*s1 == ' ')
			++s1;

		while (*s2 == ' ')
			++s2;

		if (*s1++ != *s2)
			return 0;
	}
	while (*s2++ != '\0');

	return 1;
}


// Search for the proxy connected to a particular Qt signal.

static QObject *findProxy(sipThisType *txThis,char *sig,PyObject *rxobj,char *slot,char **memberp)
{
	sipProxy *sp;

	for (sp = proxyList; sp != NULL; sp = sp -> sipNext)
		if (sp -> sipTxThis == txThis && sameSigSlotName(sp -> sipTxSig,sig) && isSameSlot(&sp -> sipRealSlot,rxobj,slot))
		{
			*memberp = sp -> sipRxSlot;
			return sp;
		}

	return NULL;
}


// Emit a Python or Qt signal.

int sipEmitSignal(PyObject *w,char *sig,PyObject *sigargs)
{
	sipPySig *ps;
	QObject *tx;

	// Don't do anything if signals are blocked.  Qt signals would be
	// blocked anyway, but this blocks Python signals as well.

	if ((tx = (QObject *)sipGetCppPtr((sipThisType *)w,sipQObjectClass)) == NULL || tx -> signalsBlocked())
		return 0;

	if (isQtSigSlot(sig))
		return emitQtSig((sipThisType *)w,sig,sigargs);

	if ((ps = findPySignal((sipThisType *)w,sig)) != NULL)
		return emitToSlotList(ps -> rxlist,sigargs);

	return 0;
}


// Search the Python signal list for a signal.

static sipPySig *findPySignal(sipThisType *w,char *sig)
{
	sipPySig *ps;

	for (ps = w -> pySigList; ps != NULL; ps = ps -> next)
		if (sameSigSlotName(ps -> name,sig))
			return ps;

	return NULL;
}


// Search a signal table for a signal.  If found, call the emitter function
// with the signal arguments.  Return 0 if the signal was emitted or <0 if
// there was an error.

static int emitQtSig(sipThisType *w,char *sig,PyObject *sigargs)
{
	sipQtSignal *tab;

	// Search the table.

	for (tab = w -> xType -> emitTable; tab -> st_name != NULL; ++tab)
	{
		char *sp, *tp;
		int found;

		// Compare only the base name.

		sp = &sig[1];
		tp = tab -> st_name;

		found = TRUE;

		while (*sp != '\0' && *sp != '(' && *tp != '\0')
			if (*sp++ != *tp++)
			{
				found = FALSE;
				break;
			}

		if (found)
			return (*tab -> st_emitfunc)(w,sigargs);
	}

	// It wasn't found if we got this far.

	PyErr_Format(PyExc_NameError,"Invalid signal %s",&sig[1]);

	return -1;
}


// Send a signal to a single slot (Qt or Python) with arguments from the stack.

void sipEmitToSlot(sipSlot *slot,char *fmt,...)
{
	PyObject *args;
	va_list va;

	va_start(va,fmt);

	if ((args = sipBuildObjectTuple(NULL,fmt,va)) == NULL || emitToSlot(slot,args) < 0)
		PyErr_Print();

	Py_XDECREF(args);

	va_end(va);
}


// Send a signal to a single slot (Qt or Python).

static int emitToSlot(sipSlot *slot,PyObject *sigargs)
{
	PyObject *sa, *oxtype, *oxvalue, *oxtb, *sfunc, *newmeth, *sref;

	// Keep some compilers quiet.

	oxtype = oxvalue = oxtb = NULL;

	// If the slot has a name then it is a Qt signal so fan out.

	if (slot -> name != NULL)
		return sipEmitSignal(slot -> pyobj,slot -> name,sigargs);

	// Get the object to call, resolving any weak references.

	if (slot -> weakSlot == NULL)
		sref = NULL;
#if PY_VERSION_HEX >= 0x02020000
	else if ((sref = PyWeakref_GetObject(slot -> weakSlot)) == NULL)
		return -1;
	else
		Py_INCREF(sref);
#else
	else if ((sref = PyEval_CallObject(slot -> weakSlot,NULL)) == NULL)
		return -1;
#endif

	if (sref == Py_None)
	{
		/*
		 * If the real object has gone then we pretend everything is
		 * Ok.  This mimics the Qt behaviour of not caring if a
		 * receiving object has been deleted.  (However, programmers
		 * may prefer an exception because it usually means a bug where
		 * they have forgotten to keep the receiving object alive.)
		 */

		Py_DECREF(sref);
		return 0;
	}

	if (slot -> pyobj != NULL)
	{
		sfunc = slot -> pyobj;
		newmeth = NULL;
	}
	else
	{
		if ((sfunc = PyMethod_New(slot -> meth.mfunc,(sref != NULL ? sref : slot -> meth.mself),slot -> meth.mclass)) == NULL)
			return -1;

		// Make sure we garbage collect the new method.

		newmeth = sfunc;
	}

	// We make repeated attempts to call a slot.  If we work out that it
	// failed because of an immediate type error we try again with one less
	// argument.  We keep going until we run out of arguments to drop.
	// This emulates the Qt ability of the slot to accept fewer arguments
	// than a signal provides.

	sa = sigargs;
        Py_INCREF(sa);

	for (;;)
	{
		PyObject *nsa, *xtype, *xvalue, *xtb, *resobj;

		if ((resobj = PyEval_CallObject(sfunc,sa)) != NULL)
		{
			Py_DECREF(resobj);

			Py_XDECREF(newmeth);
			Py_XDECREF(sref);

			// Remove any previous exception. */

			if (sa != sigargs)
			{
				Py_XDECREF(oxtype);
				Py_XDECREF(oxvalue);
				Py_XDECREF(oxtb);
				PyErr_Clear();
			}

			Py_DECREF(sa);

			return 0;
		}

		// Get the exception.

		PyErr_Fetch(&xtype,&xvalue,&xtb);

		// See if it is unacceptable.  An acceptable failure is a type
		// error with no traceback - so long as we can still reduce the
		// number of arguments and try again.

		if (!PyErr_GivenExceptionMatches(xtype,PyExc_TypeError) ||
		    xtb != NULL ||
		    PyTuple_GET_SIZE(sa) == 0)
		{
			// If there is a traceback then we must have called the
                        // slot and the exception was later on - so report the
                        // exception as is.
			// exception.

                        if (xtb != NULL)
                        {
                                if (sa != sigargs)
                                {
				        Py_XDECREF(oxtype);
				        Py_XDECREF(oxvalue);
				        Py_XDECREF(oxtb);
                                }

                                PyErr_Restore(xtype,xvalue,xtb);
                        }
			else if (sa == sigargs)
				PyErr_Restore(xtype,xvalue,xtb);
			else
			{
				// Discard the latest exception and restore the
				// original one.

				Py_XDECREF(xtype);
				Py_XDECREF(xvalue);
				Py_XDECREF(xtb);

				PyErr_Restore(oxtype,oxvalue,oxtb);
			}

			break;
		}

		// If this is the first attempt, save the exception.

		if (sa == sigargs)
		{
			oxtype = xtype;
			oxvalue = xvalue;
			oxtb = xtb;
		}
		else
		{
			Py_XDECREF(xtype);
			Py_XDECREF(xvalue);
			Py_XDECREF(xtb);
		}

		// Create the new argument tuple.

		if ((nsa = PyTuple_GetSlice(sa,0,PyTuple_GET_SIZE(sa) - 1)) == NULL)
		{
			// Tidy up.

			Py_XDECREF(oxtype);
			Py_XDECREF(oxvalue);
			Py_XDECREF(oxtb);

			break;
		}

	        Py_DECREF(sa);
		sa = nsa;
	}

	Py_XDECREF(newmeth);
	Py_XDECREF(sref);

        Py_DECREF(sa);

	return -1;
}


// Send a signal to the slots (Qt or Python) in a Python list.

static int emitToSlotList(sipPySigRx *rxlist,PyObject *sigargs)
{
	int rc;

	// Apply the arguments to each slot method.

	rc = 0;

	while (rxlist != NULL && rc >= 0)
	{
		sipPySigRx *next;

		// We get the next in the list before calling the slot in case
		// the list gets changed by the slot - usually because the slot
		// disconnects itself.

		next = rxlist -> next;
		rc = emitToSlot(&rxlist -> rx,sigargs);
		rxlist = next;
	}

	return rc;
}


// Add a slot to a transmitter's Python signal list.  The signal is a Python
// signal, the slot may be either a Qt signal, a Qt slot, a Python signal or a
// Python slot.

static int addSlotToPySigList(sipThisType *txThis,char *sig,PyObject *rxobj,char *slot)
{
	sipPySig *ps;
	sipPySigRx *psrx;

	// Create a new one if necessary.

	if ((ps = findPySignal(txThis,sig)) == NULL)
	{
		if ((ps = (sipPySig *)sipMalloc(sizeof (sipPySig))) == NULL)
			return -1;

		if ((ps -> name = sipStrdup(sig)) == NULL)
		{
			sipFree((ANY *)ps);
			return -1;
		}

		ps -> rxlist = NULL;
		ps -> next = txThis -> pySigList;

		txThis -> pySigList = ps;
	}

	// Create the new receiver.

	if ((psrx = (sipPySigRx *)sipMalloc(sizeof (sipPySigRx))) == NULL)
		return -1;

	if (setSlot(&psrx -> rx,rxobj,slot) < 0)
	{
		sipFree((ANY *)psrx);
		return -1;
	}

	psrx -> next = ps -> rxlist;
	ps -> rxlist = psrx;

	return 0;
}


// Compare two slots to see if they are the same.

static int isSameSlot(sipSlot *slot1,PyObject *rxobj2,char *slot2)
{
	// See if they are signals or Qt slots, ie. they have a name.

	if (slot1 -> name != NULL)
		return (slot2 != NULL &&
			sameSigSlotName(slot1 -> name,slot2) &&
			slot1 -> pyobj == rxobj2);

	// Both must be Python slots.

	if (slot2 != NULL)
		return 0;

	// See if they are Python methods.

	if (slot1 -> pyobj == NULL)
		return (PyMethod_Check(rxobj2) &&
			slot1 -> meth.mfunc == PyMethod_GET_FUNCTION(rxobj2) &&
			slot1 -> meth.mself == PyMethod_GET_SELF(rxobj2) &&
			slot1 -> meth.mclass == PyMethod_GET_CLASS(rxobj2));

	if (PyMethod_Check(rxobj2))
		return 0;

	// The objects must be the same.

	return (slot1 -> pyobj == rxobj2);
}


// Remove a slot from a transmitter's Python signal list.

static void removeSlotFromPySigList(sipThisType *txThis,char *sig,PyObject *rxobj,char *slot)
{
	sipPySig *ps;

	if ((ps = findPySignal(txThis,sig)) != NULL)
	{
		sipPySigRx **psrxp;

		for (psrxp = &ps -> rxlist; *psrxp != NULL; psrxp = &(*psrxp) -> next)
		{
			sipPySigRx *psrx = *psrxp;

			if (isSameSlot(&psrx -> rx,rxobj,slot))
			{
				*psrxp = psrx -> next;

				if (psrx -> rx.name != NULL)
					sipFree((ANY *)psrx -> rx.name);

				/* Remove any weak reference. */

				Py_XDECREF(psrx -> rx.weakSlot);

				sipFree((ANY *)psrx);

				break;
			}
		}
	}
}


// A Qt signal is being connected to either a Python signal or a Python slot,
// so create a proxy Qt slot to receive the Qt signal.  The reason for the use
// of void * is because we are being called via the C part of the SIP library
// which doesn't know anything about QObject.

static const void *convertPythonRx(void *func,sipThisType *txThis,char *sigargs,PyObject *rxobj,char *slot,char **member)
{
	QObject *qobj;
	sipProxy *pxy;
	sipProxy *(*proxyfunc)() = (sipProxy *(*)())func;

	qobj = pxy = (*proxyfunc)();

	if (setSlot(&pxy -> sipRealSlot,rxobj,slot) < 0)
		goto delproxy;

	pxy -> sipTxThis = txThis;

	if ((*member = pxy -> getProxySlot(sigargs)) != NULL)
		return (const void *)qobj;

delproxy:
	delete pxy;

	return NULL;
}


// Convert a valid Python signal or slot to an existing proxy Qt slot.

const void *sipGetRx(sipThisType *txThis,char *sigargs,PyObject *rxobj,char *slot,char **memberp)
{
	QObject *rx;

	if (slot != NULL && isQtSigSlot(slot))
	{
		sipThisType *rxThis;

		if ((rxThis = sipMapSelfToThis(rxobj)) == NULL)
			return NULL;

		*memberp = slot;

		return sipGetCppPtr(rxThis,sipQObjectClass);
	}

	if ((rx = findProxy(txThis,sigargs,rxobj,slot,memberp)) != NULL)
		return (const void *)rx;

	PyErr_Format(PyExc_RuntimeError,"Slot hasn't been connected");

	return NULL;
}


// Convert a Python receiver (either a Python signal or slot or a Qt signal
// or slot) to a Qt receiver.  It is only ever called when the signal is a
// Qt signal.  Return NULL is there was an error.

const void *sipConvertRx(void *proxyfunc,sipThisType *txThis,char *sigargs,PyObject *rxobj,char *slot,char **memberp)
{
	sipThisType *rxThis;

	if (slot == NULL)
		return convertPythonRx(proxyfunc,txThis,sigargs,rxobj,NULL,memberp);

	if ((rxThis = sipMapSelfToThis(rxobj)) == NULL)
		return NULL;

	if (isQtSigSlot(slot))
	{
		*memberp = slot;

		return sipGetCppPtr(rxThis,sipQObjectClass);
	}

	// We need a proxy to relay the signal.

	return convertPythonRx(proxyfunc,txThis,sigargs,(PyObject *)rxThis,slot,memberp);
}


// Connect a Qt signal or a Python signal to a Qt slot, a Qt signal, a Python
// slot or a Python signal.  This is all possible combinations.

PyObject *sipConnectRx(PyObject *txobj,char *sig,PyObject *rxobj,char *slot)
{
	sipThisType *txThis, *rxThis;

	if ((txThis = sipMapSelfToThis(txobj)) == NULL)
		return NULL;

	// See if the receiver is a Python slot.

	if (slot == NULL)
		return connectToPythonSlot(txThis,sig,rxobj);

	// Handle Qt signals.

	if (isQtSignal(sig))
	{
		const void *txPtr, *rxQObj;
		char *member;
		bool res;

		if ((txPtr = sipGetCppPtr(txThis,sipQObjectClass)) == NULL)
			return NULL;

		if ((rxQObj = sipConvertRx(txThis -> xType -> proxyfunc,txThis,sig,rxobj,slot,&member)) == NULL)
			return NULL;

		Py_BEGIN_ALLOW_THREADS
		res = QObject::connect((QObject *)txPtr,sig,(QObject *)rxQObj,member);
		Py_END_ALLOW_THREADS

		return sipConvertFromBool(res);
	}

	// Handle Python signals.  The receiver could be the wrapped object or
	// a class instance.

	if (!PyInstance_Check(rxobj))
		rxThis = (sipThisType *)rxobj;
	else if ((rxThis = sipMapSelfToThis(rxobj)) == NULL)
		return NULL;

	if (addSlotToPySigList(txThis,sig,(PyObject *)rxThis,slot) < 0)
		return NULL;

	Py_INCREF(Py_True);
	return Py_True;
}


// Connect either a Qt signal or a Python signal to a Python slot.  This will
// not be called for any other combination.

static PyObject *connectToPythonSlot(sipThisType *txThis,char *sig,PyObject *rxobj)
{
	// Handle Qt signals.

	if (isQtSignal(sig))
	{
		const void *txPtr, *rxQObj;
		char *member;
		bool res;

		if ((txPtr = sipGetCppPtr(txThis,sipQObjectClass)) == NULL)
			return NULL;

		if ((rxQObj = convertPythonRx(txThis -> xType -> proxyfunc,txThis,sig,rxobj,NULL,&member)) == NULL)
			return NULL;

		Py_BEGIN_ALLOW_THREADS
		res = QObject::connect((QObject *)txPtr,sig,(QObject *)rxQObj,member);
		Py_END_ALLOW_THREADS

		return sipConvertFromBool(res);
	}

	// Handle Python signals.

	if (addSlotToPySigList(txThis,sig,rxobj,NULL) < 0)
		return NULL;

	Py_INCREF(Py_True);
	return Py_True;
}


// Disconnect a signal to a signal or a Qt slot.

PyObject *sipDisconnectRx(PyObject *txobj,char *sig,PyObject *rxobj,char *slot)
{
	sipThisType *txThis, *rxThis;

	if ((txThis = sipMapSelfToThis(txobj)) == NULL)
		return NULL;

	if (slot == NULL)
		return disconnectFromPythonSlot(txThis,sig,rxobj);

	if ((rxThis = sipMapSelfToThis(rxobj)) == NULL)
		return NULL;

	// Handle Qt signals.

	if (isQtSignal(sig))
	{
		char *member;
		QObject *rxQObj;

		if ((rxQObj = (QObject *)sipGetRx(txThis,sig,rxobj,slot,&member)) == NULL)
			return NULL;

		return doDisconnect(txThis,sig,rxQObj,member);
	}

	// Handle Python signals.

	removeSlotFromPySigList(txThis,sig,(PyObject *)rxThis,slot);

	Py_INCREF(Py_True);
	return Py_True;
}


// Disconnect a signal from a Python slot.

static PyObject *disconnectFromPythonSlot(sipThisType *txThis,char *sig,PyObject *rxobj)
{
	// Handle Qt signals.

	if (isQtSignal(sig))
	{
		char *member;
		QObject *rxQObj;

		if ((rxQObj = (QObject *)sipGetRx(txThis,sig,rxobj,NULL,&member)) == NULL)
			return NULL;

		return doDisconnect(txThis,sig,rxQObj,member);
	}

	// Handle Python signals.

	removeSlotFromPySigList(txThis,sig,rxobj,NULL);

	Py_INCREF(Py_True);
	return Py_True;
}


// Actually do a QObject disconnect.

static PyObject *doDisconnect(sipThisType *txThis,char *sig,QObject *rxQObj,char *slot)
{
	const void *txPtr;
	PyObject *res;

	if ((txPtr = sipGetCppPtr(txThis,sipQObjectClass)) == NULL)
		res = NULL;
	else
		res = sipConvertFromBool(QObject::disconnect((QObject *)txPtr,sig,rxQObj,slot));

	// Delete any proxy as this will be it's only connection.

	if (rxQObj -> inherits("sipProxy"))
		delete rxQObj;

	return res;
}


// Implement strdup() using sipMalloc().

static char *sipStrdup(char *s)
{
	char *d;

	if ((d = (char *)sipMalloc(strlen(s) + 1)) != NULL)
		strcpy(d,s);

	return d;
}


// Initialise a slot, returning 0 if there was no error.  If the signal was a
// Qt signal, then the slot may be a Python signal or a Python slot.  If the
// signal was a Python signal, then the slot may be anything.
static int setSlot(sipSlot *sp,PyObject *rxobj,char *slot)
{
	sp -> weakSlot = NULL;

	if (slot == NULL)
	{
		sp -> name = NULL;

		if (PyMethod_Check(rxobj))
		{
			// Python creates methods on the fly.  We could
			// increment the reference count to keep it alive, but
			// that would keep "self" alive as well and would
			// probably be a circular reference.  Instead we
			// remember the component parts and hope they are still
			// valid when we re-create the method when we need it.
			sipSaveMethod(&sp -> meth,rxobj);

			// Notice if the class instance disappears.
			sp -> weakSlot = sipGetWeakRef(sp -> meth.mself);

			// This acts a flag to say that the slot is a method.
			sp -> pyobj = NULL;
		}
		else
		{
			// We know that it is another type of callable, ie. a
			// function/builtin.  This includes the possibility of
			// it being a wrapped C++ class method, in which case
			// we want to get a weak reference to the class
			// instance.  First save the callable.
			sp -> pyobj = rxobj;

			// See if it is a wrapped C++ class method.
			if (PyCFunction_Check(rxobj) && PyCFunction_GET_SELF(rxobj) && PyCFunction_GET_SELF(rxobj) -> ob_type -> tp_name == sipWrapperTypeName)
				sp -> weakSlot = sipGetWeakRef(sipGetSelfFromThis(PyCFunction_GET_SELF(rxobj)));
			else
			{
				// It's unlikely that we will succeed in
				// getting a weak reference to the slot, but
				// there is no harm in trying (and future
				// versions of Python may support references to
				// more object types).
				sp -> weakSlot = sipGetWeakRef(rxobj);
			}
		}
	}
	else if ((sp -> name = sipStrdup(slot)) == NULL)
		return -1;
	else if (isQtSlot(slot))
	{
		// The user has decided to connect a Python signal to a Qt slot
		// and specified the slot as "obj,SLOT('meth()')" rather than
		// "obj.meth".  Therefore we do the conversion here.

		char *mname, *tail;

		// Isolate the part that is the method name.  It's on the heap
		// so it doesn't matter that we mess with it.
		if (*(mname = sp -> name) != '\0')
			++mname;

		if ((tail = strchr(mname,'(')) != NULL)
			*tail = '\0';

		// The attribute should be a C function as it is a slot
		// implemented in the library being wrapped.
		if ((sp -> pyobj = PyObject_GetAttrString(((sipThisType *)rxobj) -> sipSelf,mname)) == NULL || !PyCFunction_Check(sp -> pyobj))
		{
			PyErr_Format(PyExc_NameError,"Invalid slot %s",mname);

			Py_XDECREF(sp -> pyobj);
			sipFree((ANY *)sp -> name);

			return -1;
		}

		// Notice if the class instance disappears.
		sp -> weakSlot = sipGetWeakRef(((sipThisType *)rxobj) -> sipSelf);

		// We discard the reference.  It will now be cached in the
		// instance's dictionary.
		Py_DECREF(sp -> pyobj);

		sipFree((ANY *)sp -> name);
		sp -> name = NULL;
	}
	else
		sp -> pyobj = rxobj;

	return 0;
}

#endif


// Set a C++ bool for the main C implementation of the module.

extern "C" void sipSetBool(void *ptr,int val)
{
	*(bool *)ptr = val;
}
