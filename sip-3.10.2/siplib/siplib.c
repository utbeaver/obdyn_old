/*
 * SIP library code.
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


#include <Python.h>
#include <stdarg.h>
#include <string.h>

#include "sip.h"
#include "sipint.h"


#define	PARSE_OK	0x00000000	/* Parse is Ok so far. */
#define	PARSE_MANY	0x10000000	/* Too many arguments. */
#define	PARSE_FEW	0x20000000	/* Too few arguments. */
#define	PARSE_TYPE	0x30000000	/* Argument with a bad type. */
#define	PARSE_UNBOUND	0x40000000	/* Unbound method. */
#define	PARSE_FORMAT	0x50000000	/* Bad format character. */
#define	PARSE_RAISED	0x60000000	/* Exception already raised. */
#define	PARSE_STICKY	0x80000000	/* The error sticks. */
#define	PARSE_MASK	0xf0000000

#define	FORMAT_DEREF		0x01	/* The pointer will be dereferenced. */
#define	FORMAT_TRANSFER		0x02	/* Implement /Transfer/. */
#define	FORMAT_TRANSFER_BACK	0x04	/* Implement /TransferBack/. */
#define	FORMAT_GET_WRAPPER	0x08	/* Implement /GetWrapper/. */


typedef PyObject *(*convfromfunc)(void *);	/* From convertor function. */
typedef int (*convfunc)(PyObject *,void *,int *);/* Type convertor function. */
typedef void *(*forceconvfunc)(PyObject *,int *);/* Force convertor function. */

typedef struct _subClassConvertor {
	PyObject *subbase;			/* The sub-class base class. */
	PyObject *(*convertor)(const void *);	/* The convertor. */
	struct _subClassConvertor *next;	/* Next in the list. */
} subClassConvertor;


staticforward PyTypeObject sipVoidPtr_Type;

PyThreadState *sipMainThreadState = NULL;

char *sipWrapperTypeName = "sip.wrapper";

#ifdef SIP_QT_SUPPORT
PyObject *sipQObjectClass = NULL;
#endif


/*
 * Various strings as Python objects created as and when needed.
 */
static PyObject *thisName = NULL;
static PyObject *licenseName = NULL;
static PyObject *licenseeName = NULL;
static PyObject *typeName = NULL;
static PyObject *timestampName = NULL;
static PyObject *signatureName = NULL;

static unsigned traceMask = 0;			/* The current trace mask. */
static int needInit = TRUE;			/* Set if we need to init. */
static PyObject *dtorFunction = NULL;		/* sipDtor as a callable. */
static sipObjectMap cppPyMap;			/* The C/C++ to Python map. */
static getattrofunc savedClassGetAttr;		/* Saved class getattr(). */
static getattrofunc savedInstanceGetAttr;	/* Saved instance getattr(). */
static sipModuleDef *modulesList = NULL;	/* List of registered modules. */
static subClassConvertor *convertorList = NULL;	/* List of sub-class convertors. */
#if PY_VERSION_HEX >= 0x02010000 && PY_VERSION_HEX < 0x02020000
static PyObject *weakRefMeth = NULL;		/* The _weakref.ref method. */
#endif


static int parsePass1(sipThisType **,int *,int *,PyObject *,char *,va_list);
static int parsePass2(sipThisType *,int,int,PyObject *,char *,va_list);
static int getThisFromArgs(PyObject *,PyObject *,int,sipThisType **);
static int addCallableToClass(PyObject *,char *,PyObject *);
static const void *convertToCppAndThis(PyObject *,PyObject *,int *,PyObject **);
static PyMethodDef *searchVarTable(PyMethodDef **,PyObject *);
static PyObject *convertSubClass(PyObject *,const void *);
static PyObject *sipDtor(PyObject *,PyObject *);
static PyObject *classGetAttr(PyObject *,PyObject *);
static PyObject *instanceGetAttr(PyObject *,PyObject *);
static PyObject *handleLazyAttr(PyObject *,PyObject *,PyClassObject *,
				PyObject *);
static PyObject *setTraceMask(PyObject *,PyObject *);
static PyObject *buildObject(PyObject *,PyObject *,char *,va_list);
static void findLazyAttr(PyObject *,char *,PyMethodDef **,
			 sipEnumValueInstanceDef **);
static sipClassDef *findClass(PyObject *);
static PyObject *getThisFromInstance(PyInstanceObject *);
static int initClassObject(sipModuleDef *,int);
static int compareMethodName(const void *,const void *);
static int compareEnumName(const void *,const void *);
static int checkPointer(const void *);
static void setBadNone(int);
static void badArgs(int,char *,char *);
static void finalise(void);
#if PY_VERSION_HEX < 0x02030000
static int getSliceIndex(PyObject *,int *);
#endif


/*
 * The Python module initialisation function.
 */
void initlibsip(void)
{
	static PyMethodDef methods[] = {
		{"settracemask", setTraceMask, METH_VARARGS, NULL},
		{NULL}
	};

	sipVoidPtr_Type.ob_type = &PyType_Type;

	Py_InitModule("libsip",methods);
}


/*
 * Display a printf() style message to stderr according to the current trace
 * mask.
 */
void sipTrace(unsigned mask,const char *fmt,...)
{
	va_list ap;

	va_start(ap,fmt);

	if (mask & traceMask)
		vfprintf(stderr,fmt,ap);

	va_end(ap);
}


/*
 * Set the trace mask.
 */
static PyObject *setTraceMask(PyObject *self,PyObject *args)
{
	unsigned new_mask;

	if (PyArg_ParseTuple(args,"i:settracemask",&new_mask))
	{
		traceMask = new_mask;

		Py_INCREF(Py_None);
		return Py_None;
	}

	return NULL;
}


/*
 * Register a module.
 */
int sipRegisterModule(sipModuleDef *sm,PyObject *args)
{
	PyObject *dictofmods, *mod;

	/* The arguments should consist of just the module name. */

	if (!PyArg_ParseTuple(args,"S",&sm -> md_name))
		return -1;

	Py_INCREF(sm -> md_name);

	if ((dictofmods = PyImport_GetModuleDict()) == NULL)
		return -1;

	/* Initialise the library if it hasn't already been done. */

	if (needInit)
	{
		static PyMethodDef dtorpmd = {"__del__", sipDtor, METH_VARARGS, NULL};

		Py_AtExit(finalise);

		if ((dtorFunction = PyCFunction_New(&dtorpmd,NULL)) == NULL)
			return -1;

		if ((thisName = PyString_FromString("sipThis")) == NULL)
			return -1;

		/*
		 * This truly dreadful hack is needed so that we can resolve
		 * Class.method references for lazy attributes.
		 */

		savedClassGetAttr = PyClass_Type.tp_getattro;
		PyClass_Type.tp_getattro = classGetAttr;

		/* We might as well extend the hack to instances. */

		savedInstanceGetAttr = PyInstance_Type.tp_getattro;
		PyInstance_Type.tp_getattro = instanceGetAttr;

		/* Initialise the object map. */

		sipOMInit(&cppPyMap);

#ifdef SIP_QT_SUPPORT
		sipQObjectClass = NULL;
#endif

		needInit = FALSE;

		/*
		 * Get the state of the current thread.  If the user has
		 * followed the guidelines then this should be the main thread,
		 * but we have no way of checking.
		 */

		sipMainThreadState = PyEval_SaveThread();
		PyEval_RestoreThread(sipMainThreadState);
	}

	/* Get the module dictionary. */

	if ((mod = PyDict_GetItem(dictofmods,sm -> md_name)) == NULL ||
	    (sm -> md_dict = PyModule_GetDict(mod)) == NULL)
	{
		PyErr_Format(PyExc_SystemError,"Unable to find module dictionary for %s",PyString_AsString(sm -> md_name));

		return -1;
	}

	/* Add to the list of modules. */

	sm -> md_next = modulesList;
	modulesList = sm;

	return 0;
}


/*
 * Called by the interpreter to do any final clearing up, just in case the
 * interpreter will re-start.
 */
static void finalise(void)
{
	sipModuleDef *sm;
	subClassConvertor *scc;

	/* Free the list of sub-class convertors. */

	scc = convertorList;

	while (scc != NULL)
	{
		convertorList = scc -> next;

		sipFree(scc);

		scc = convertorList;
	}

	Py_XDECREF(dtorFunction);
	dtorFunction = NULL;

	Py_XDECREF(thisName);
	thisName = NULL;

	Py_XDECREF(licenseName);
	licenseName = NULL;

	Py_XDECREF(licenseeName);
	licenseeName = NULL;

	Py_XDECREF(typeName);
	typeName = NULL;

	Py_XDECREF(timestampName);
	timestampName = NULL;

	Py_XDECREF(signatureName);
	signatureName = NULL;

	/* Release the module names. */

	for (sm = modulesList; sm != NULL; sm = sm -> md_next)
	{
		int c;

		for (c = 0; c < sm -> md_nrclasses; ++c)
		{
			sipClassDef *cd = &sm -> md_classes[c];

			*cd -> cd_classptr = NULL;
		}

		Py_DECREF(sm -> md_name);
	}

	/* Release all memory we've allocated directly. */

	sipOMFinalise(&cppPyMap);

	/* Re-initialise those globals that (might) need it. */

	modulesList = NULL;
	sipMainThreadState = NULL;
#if PY_VERSION_HEX >= 0x02010000 && PY_VERSION_HEX < 0x02020000
	weakRefMeth = NULL;
#endif

	/* Undo the hack. */
	PyClass_Type.tp_getattro = savedClassGetAttr;
	PyInstance_Type.tp_getattro = savedInstanceGetAttr;

	/* We need to re-initialise if we are called again. */

	needInit = TRUE;
}


/*
 * A Python 1.5 style memory allocator that supports Python 1.5 and 1.6.
 */
ANY *sipMalloc(size_t nbytes)
{
#if PY_VERSION_HEX >= 0x01060000
	ANY *mem;

	if ((mem = PyMem_Malloc(nbytes)) == NULL)
		PyErr_NoMemory();

	return mem;
#else
	return Py_Malloc(nbytes);
#endif
}


/*
 * A Python 1.5 style memory de-allocator that supports Python 1.5 and 1.6.
 */
void sipFree(ANY *mem)
{
#if PY_VERSION_HEX >= 0x01060000
	PyMem_Free(mem);
#else
	Py_Free(mem);
#endif
}


/*
 * Call the Python re-implementation of a C++ virtual.
 */
PyObject *sipCallMethod(int *isErr,const sipBoundMethod *bmeth,char *fmt,...)
{
	PyObject *args, *res;
	va_list va;

	va_start(va,fmt);

	if ((args = sipBuildObjectTuple(bmeth -> sipThis -> sipSelf,fmt,va)) != NULL)
	{
		res = sipEvalMethod(bmeth -> method,args);
		Py_DECREF(args);
	}
	else
	{
		res = NULL;

		if (isErr != NULL)
			*isErr = TRUE;
	}

	va_end(va);

	return res;
}


/*
 * Build a result object based on a format string.
 */
PyObject *sipBuildResult(int *isErr,char *fmt,...)
{
	PyObject *res = NULL;
	int badfmt, tupsz;
	va_list va;

	va_start(va,fmt);

	/* Basic validation of the format string. */

	badfmt = FALSE;

	if (*fmt == '(')
	{
		char *ep;

		if ((ep = strchr(fmt,')')) == NULL || ep[1] != '\0')
			badfmt = TRUE;
		else
			tupsz = ep - fmt - 1;
	}
	else if (strlen(fmt) == 1)
		tupsz = -1;
	else
		badfmt = TRUE;

	if (badfmt)
		PyErr_Format(PyExc_SystemError,"sipBuildResult(): invalid format string \"%s\"",fmt);
	else if (tupsz < 0 || (res = PyTuple_New(tupsz)) != NULL)
		res = buildObject(res,NULL,fmt,va);

	va_end(va);

	if (res == NULL && isErr != NULL)
		*isErr = TRUE;

	return res;
}


/*
 * Get the values off the stack and put them into a tuple.
 */
PyObject *sipBuildObjectTuple(PyObject *self,char *fmt,va_list va)
{
	PyObject *args;
	int tupsz;

	tupsz = strlen(fmt);

	if (self != NULL)
		++tupsz;

	if ((args = PyTuple_New(tupsz)) == NULL)
		return NULL;

	return buildObject(args,self,fmt,va);
}


/*
 * Get the values off the stack and put them into an object.
 */
static PyObject *buildObject(PyObject *obj,PyObject *self,char *fmt,va_list va)
{
	char ch, termch;
	int i;

	/*
	 * The format string has already been checked that it is properly
	 * formed if it is enclosed in parenthesis.
	 */
	if (*fmt == '(')
	{
		termch = ')';
		++fmt;
	}
	else
		termch = '\0';

	if (self != NULL)
	{
		Py_INCREF(self);
		PyTuple_SET_ITEM(obj,0,self);

		i = 1;
	}
	else
		i = 0;

	while ((ch = *fmt++) != termch)
	{
		PyObject *el;

		switch (ch)
		{
		case 'a':
			{
				char *s;
				int l;

				s = va_arg(va,char *);
				l = va_arg(va,int);

				if (s != NULL)
					el = PyString_FromStringAndSize(s,l);
				else
				{
					Py_INCREF(Py_None);
					el = Py_None;
				}
			}

			break;

		case 'b':
			el = sipConvertFromBool(va_arg(va,int));
			break;

		case 'c':
			{
				char c = (char)va_arg(va,int);

				el = PyString_FromStringAndSize(&c,1);
			}

			break;

		case 'e':
			el = PyInt_FromLong(va_arg(va,int));
			break;

		case 'd':
		case 'f':
			el = PyFloat_FromDouble(va_arg(va,double));
			break;

		case 'h':
		case 'i':
			el = PyInt_FromLong(va_arg(va,int));
			break;

		case 'l':
			el = PyInt_FromLong(va_arg(va,long));
			break;

		case 's':
			{
				char *s = va_arg(va,char *);

				if (s != NULL)
					el = PyString_FromString(s);
				else
				{
					Py_INCREF(Py_None);
					el = Py_None;
				}
			}

			break;

		case 'M':
			{
				void *sipCpp = va_arg(va,void *);
				PyObject *cls = va_arg(va,PyObject *);

				el = sipMapCppToSelfSubClass(sipCpp,cls);
			}

			break;

		case 'N':
			{
				void *sipCpp = va_arg(va,void *);
				PyObject *cls = va_arg(va,PyObject *);

				el = sipNewCppToSelfSubClass(sipCpp,cls,SIP_PY_OWNED|SIP_SIMPLE);
			}

			break;

		case 'O':
			{
				void *sipCpp = va_arg(va,void *);
				PyObject *cls = va_arg(va,PyObject *);

				el = sipMapCppToSelf(sipCpp,cls);
			}

			break;

		case 'P':
			{
				void *sipCpp = va_arg(va,void *);
				PyObject *cls = va_arg(va,PyObject *);

				el = sipNewCppToSelf(sipCpp,cls,SIP_PY_OWNED|SIP_SIMPLE);
			}

			break;

		case 'R':
			el = va_arg(va,PyObject *);
			break;

		case 'S':
			el = ((sipThisType *)va_arg(va,PyObject *)) -> sipSelf;
			Py_INCREF(el);
			break;

		case 'T':
			{
				void *sipCpp = va_arg(va,void *);
				convfromfunc func = va_arg(va,convfromfunc);

				el = func(sipCpp);
			}

			break;

		case 'V':
			el = sipConvertFromVoidPtr(va_arg(va,void *));
			break;

		default:
			PyErr_Format(PyExc_SystemError,"buildObject(): invalid format character '%c'",ch);
			el = NULL;
		}

		if (el == NULL)
		{
			Py_XDECREF(obj);
			return NULL;
		}

		if (obj == NULL)
			return el;

		PyTuple_SET_ITEM(obj,i,el);
		++i;
	}

	return obj;
}


/*
 * Parse a result object based on a format string.
 */
int sipParseResult(int *isErr,const sipBoundMethod *bmeth,PyObject *res,char *fmt,...)
{
	int badfmt, tupsz, rc = 0;
	va_list va;

	va_start(va,fmt);

	/* Basic validation of the format string. */

	badfmt = FALSE;

	if (*fmt == '(')
	{
		char *ep;

		if ((ep = strchr(fmt,')')) == NULL || ep[1] != '\0')
			badfmt = TRUE;
		else
			tupsz = ep - ++fmt;
	}
	else if (strlen(fmt) == 1)
		tupsz = -1;
	else
		badfmt = TRUE;

	if (badfmt)
	{
		PyErr_Format(PyExc_SystemError,"sipParseResult(): invalid format string \"%s\"",fmt);
		rc = -1;
	}
	else if (tupsz >= 0 && (!PyTuple_Check(res) || PyTuple_GET_SIZE(res) != tupsz))
	{
		sipBadCatcherResult(bmeth);
		rc = -1;
	}
	else
	{
		char ch;
		int i = 0;

		while ((ch = *fmt++) != '\0' && ch != ')' && rc == 0)
		{
			PyObject *arg;
			int invalid = FALSE;

			if (tupsz > 0)
			{
				arg = PyTuple_GET_ITEM(res,i);
				++i;
			}
			else
				arg = res;

			switch (ch)
			{
			case 'a':
				{
					char **p = va_arg(va,char **);
					int *szp = va_arg(va,int *);

					if (arg == Py_None)
					{
						*p = NULL;
						*szp = 0;
					}
					else if (PyString_Check(arg))
					{
						*p = PyString_AS_STRING(arg);
						*szp = PyString_GET_SIZE(arg);
					}
					else
						invalid = TRUE;
				}

				break;

			case 'b':
				{
					long v = PyInt_AsLong(arg);

					if (PyErr_Occurred())
						invalid = TRUE;
					else
						sipSetBool(va_arg(va,void *),(int)v);
				}

				break;

			case 'c':
				{
					if (PyString_Check(arg) && PyString_GET_SIZE(arg) == 1)
						*va_arg(va,char *) = *PyString_AS_STRING(arg);
					else
						invalid = TRUE;
				}

				break;

			case 'd':
				{
					double v = PyFloat_AsDouble(arg);

					if (PyErr_Occurred())
						invalid = TRUE;
					else
						*va_arg(va,double *) = v;
				}

				break;

			case 'e':
				{
					long v = PyInt_AsLong(arg);

					if (PyErr_Occurred())
						invalid = TRUE;
					else
						*va_arg(va,int *) = (int)v;
				}

				break;

			case 'f':
				{
					double v = PyFloat_AsDouble(arg);

					if (PyErr_Occurred())
						invalid = TRUE;
					else
						*va_arg(va,float *) = (float)v;
				}

				break;

			case 'h':
				{
					long v = PyInt_AsLong(arg);

					if (PyErr_Occurred())
						invalid = TRUE;
					else
						*va_arg(va,short *) = (short)v;
				}

				break;

			case 'i':
				{
					long v = PyInt_AsLong(arg);

					if (PyErr_Occurred())
						invalid = TRUE;
					else
						*va_arg(va,int *) = (int)v;
				}

				break;

			case 'l':
				{
					long v = PyInt_AsLong(arg);

					if (PyErr_Occurred())
						invalid = TRUE;
					else
						*va_arg(va,long *) = v;
				}

				break;

			case 's':
				{
					char **p = va_arg(va,char **);

					if (arg == Py_None)
						*p = NULL;
					else if (PyString_Check(arg))
						*p = PyString_AS_STRING(arg);
					else
						invalid = TRUE;
				}

				break;

			case 'J':
				{
					forceconvfunc func = va_arg(va,forceconvfunc);
					void **sipCpp = va_arg(va,void **);
					int iserr = FALSE;

					*sipCpp = func(arg,&iserr);

					if (iserr)
						invalid = TRUE;
				}

				break;

			case 'K':
				{
					forceconvfunc func = va_arg(va,forceconvfunc);
					void **sipCpp = va_arg(va,void **);
					int iserr = FALSE;

					*sipCpp = func(arg,&iserr);

					if (iserr || *sipCpp == NULL)
						invalid = TRUE;
				}

				break;

			case 'L':
				{
					forceconvfunc func = va_arg(va,forceconvfunc);
					void **sipCpp = va_arg(va,void **);
					int iserr = FALSE;

					*sipCpp = func(arg,&iserr);

					if (iserr)
						invalid = TRUE;
				}

				break;

			case 'M':
				{
					forceconvfunc func = va_arg(va,forceconvfunc);
					void **sipCpp = va_arg(va,void **);
					int iserr = FALSE;

					*sipCpp = func(arg,&iserr);

					if (iserr || *sipCpp == NULL)
						invalid = TRUE;
				}

				break;

			case 'N':
				{
					PyTypeObject *type = va_arg(va,PyTypeObject *);
					PyObject **p = va_arg(va,PyObject **);

#if PY_VERSION_HEX >= 0x02020000
					if (arg == Py_None || PyObject_TypeCheck(arg,type))
#else
					if (arg == Py_None || arg -> ob_type == type)
#endif
					{
						Py_INCREF(arg);
						*p = arg;
					}
					else
						invalid = TRUE;
				}

				break;

			case 'O':
				Py_INCREF(arg);
				*va_arg(va,PyObject **) = arg;
				break;

			case 'T':
				{
					PyTypeObject *type = va_arg(va,PyTypeObject *);
					PyObject **p = va_arg(va,PyObject **);

#if PY_VERSION_HEX >= 0x02020000
					if (PyObject_TypeCheck(arg,type))
#else
					if (arg -> ob_type == type)
#endif
					{
						Py_INCREF(arg);
						*p = arg;
					}
					else
						invalid = TRUE;
				}

				break;

			case 'V':
				{
					void *v = sipConvertToVoidPtr(arg);

					if (PyErr_Occurred())
						invalid = TRUE;
					else
						*va_arg(va,void **) = v;
				}

				break;

			case 'Z':
				if (arg != Py_None)
					invalid = TRUE;

				break;

			default:
				PyErr_Format(PyExc_SystemError,"sipParseResult(): invalid format character '%c'",ch);
				rc = -1;
			}

			if (invalid)
			{
				sipBadCatcherResult(bmeth);
				rc = -1;
				break;
			}
		}
	}

	va_end(va);

	if (isErr != NULL && rc < 0)
		*isErr = TRUE;

	return rc;
}


/*
 * Parse the arguments to a C/C++ function without any side effects.
 */
int sipParseArgs(int *argsParsedp,PyObject *sipArgs,char *fmt,...)
{
	int valid, nrargs, selfarg;
	sipThisType *this;
	va_list va;

	/* Previous sticky errors stop subsequent parses. */

	if (*argsParsedp & PARSE_STICKY)
		return 0;

	if ((nrargs = PyTuple_Size(sipArgs)) < 0)
		return 0;

	/*
	 * The first pass checks all the types and does conversions that are
	 * cheap and have no side effects.
	 */

	va_start(va,fmt);
	valid = parsePass1(&this,&selfarg,&nrargs,sipArgs,fmt,va);
	va_end(va);

	if (valid != PARSE_OK)
	{
		/*
		 * Update if we managed to parse more arguments than any
		 * previous attempt.
		 */

		if ((*argsParsedp & PARSE_MASK) == PARSE_OK ||
		    (*argsParsedp & ~PARSE_MASK) < (unsigned)nrargs)
			*argsParsedp = valid | nrargs;

		return 0;
	}

	/*
	 * The second pass does any remaining conversions now that we know we
	 * have the right signature.
	 */

	va_start(va,fmt);
	valid = parsePass2(this,selfarg,nrargs,sipArgs,fmt,va);
	va_end(va);

	if (valid != PARSE_OK)
	{
		*argsParsedp = valid | PARSE_STICKY;

		return 0;
	}

	*argsParsedp = nrargs;

	return 1;
}


/*
 * First pass of the argument parse, converting those that can be done so
 * without any side effects.  Return PARSE_OK if the arguments matched.
 */
static int parsePass1(sipThisType **thisp,int *selfargp,int *argsParsedp,
		      PyObject *sipArgs,char *fmt,va_list va)
{
	int valid, compulsory, nrargs, argnr, nrparsed;

	valid = PARSE_OK;
	nrargs = *argsParsedp;
	nrparsed = 0;
	compulsory = TRUE;
	argnr = 0;

	/*
	 * Handle those format characters that deal with the "self" argument.
	 * They will always be the first one.
	 */

	*thisp = NULL;
	*selfargp = FALSE;

	switch (*fmt)
	{
	case 'n':
		{
			PyObject **selfp = va_arg(va,PyObject **);
			PyObject *classObj = va_arg(va,PyObject *);
			const void **p = va_arg(va,const void **);

			/*
			 * The way that we create slot methods means that
			 * "self" will always be in the argument tuple.
			 */

			if ((valid = getThisFromArgs(classObj,sipArgs,argnr,thisp)) != PARSE_OK)
				break;

			*selfp = (*thisp) -> sipSelf;

			*selfargp = TRUE;
			++nrparsed;
			++argnr;

			++fmt;

			break;
		}

	case 't':
	case 'm':
	case 'p':
		{
			PyObject *selfOrThis = va_arg(va,PyObject *);
			PyObject *classObj = va_arg(va,PyObject *);
			const void **p = va_arg(va,const void **);

			if (selfOrThis == NULL)
			{
				if ((valid = getThisFromArgs(classObj,sipArgs,argnr,thisp)) != PARSE_OK)
					break;

				*selfargp = TRUE;
				++nrparsed;
				++argnr;
			}
			else
				*thisp = (sipThisType *)selfOrThis;

			++fmt;

			break;
		}
	}

	/* Now handle the remaining arguments. */

	while (valid == PARSE_OK)
	{
		char ch;
		PyObject *arg;

		PyErr_Clear();

		/* See if the following arguments are optional. */

		if ((ch = *fmt++) == '|')
		{
			compulsory = FALSE;
			ch = *fmt++;
		}

		/* See if we don't expect anything else. */

		if (ch == '\0')
		{
			/* Invalid if there are still arguments. */

			if (argnr < nrargs)
				valid = PARSE_MANY;

			break;
		}

		/* See if we have run out of arguments. */

		if (argnr == nrargs)
		{
			/* Invalid if we still expect compulsory arguments. */

			if (ch != '\0' && compulsory)
				valid = PARSE_FEW;

			break;
		}

		/* Get the next argument. */

		arg = PyTuple_GET_ITEM(sipArgs,argnr);
		++argnr;

		switch (ch)
		{
		case 's':
			{
				/* String or None. */

				char **p = va_arg(va,char **);

				if (arg == Py_None)
					*p = NULL;
				else if (PyString_Check(arg))
					*p = PyString_AS_STRING(arg);
				else
					valid = PARSE_TYPE;

				break;
			}

		case 'S':
			{
				/* Slot name, return the name. */

				if (PyString_Check(arg))
				{
					char *s = PyString_AS_STRING(arg);

					if (*s == '1' || *s == '2' || *s == '9')
						*va_arg(va,char **) = s;
					else
						valid = PARSE_TYPE;
				}
				else
					valid = PARSE_TYPE;

				break;
			}

		case 'G':
			{
				/* Signal name, return the name. */

				if (PyString_Check(arg))
				{
					char *s = PyString_AS_STRING(arg);

					if (*s == '2' || *s == '9')
						*va_arg(va,char **) = s;
					else
						valid = PARSE_TYPE;
				}
				else
					valid = PARSE_TYPE;

				break;
			}

		case 'J':
			{
				/* Class instance with no convertors. */

				PyObject *classObj = va_arg(va,PyObject *);
				void **ap = va_arg(va,void **);

				if (arg != Py_None && !sipIsSubClassInstance(arg,classObj))
					valid = PARSE_TYPE;

				/* Handle the sub-format. */

				if (*fmt == '\0')
					valid = PARSE_FORMAT;
				else
				{
					int flags = *fmt++ - '0';

					if (flags & FORMAT_GET_WRAPPER)
						va_arg(va,PyObject **);
				}

				break;
			}

		case 'K':
			{
				/*
				 * Class instance with no convertors and return
				 * the original Python class instance as
				 * well - deprecated.
				 */

				PyObject *classObj = va_arg(va,PyObject *);
				void **ap = va_arg(va,void **);
				PyObject **aobjp = va_arg(va,PyObject **);

				if (arg != Py_None && !sipIsSubClassInstance(arg,classObj))
					valid = PARSE_TYPE;

				break;
			}

		case 'M':
			{
				/* Class instance with convertors. */

				convfunc func = va_arg(va,convfunc);
				void *p = va_arg(va,void *);
				int *heap = va_arg(va,int *);

				/*
				 * Passing NULL as the last parameter to the
				 * convertor function tells it to only check
				 * the type and not do any conversion.  We pass
				 * the destination of the final result (the
				 * second parameter) in case the convertor
				 * function wants to use it to save some state
				 * between this call and the next one.
				 */

				if (arg != Py_None && !(*func)(arg,p,NULL))
					valid = PARSE_TYPE;

				/* Handle the sub-format. */

				if (*fmt++ == '\0')
					valid = PARSE_FORMAT;

				break;
			}


		case 'N':
			{
				/* Python object of given type or None. */

				PyTypeObject *type = va_arg(va,PyTypeObject *);
				PyObject **p = va_arg(va,PyObject **);

#if PY_VERSION_HEX >= 0x02020000
				if (arg == Py_None || PyObject_TypeCheck(arg,type))
#else
				if (arg == Py_None || arg -> ob_type == type)
#endif
					*p = arg;
				else
					valid = PARSE_TYPE;

				break;
			}

		case 'O':
			{
				/* Python object of any type. */

				*va_arg(va,PyObject **) = arg;
				break;
			}

		case 'P':
			{
				/*
				 * Python object of any type with a
				 * sub-format.
				 */

				*va_arg(va,PyObject **) = arg;

				/* Skip the sub-format. */

				if (*fmt++ == '\0')
					valid = PARSE_FORMAT;

				break;
			}

		case 'T':
			{
				/* Python object of given type. */

				PyTypeObject *type = va_arg(va,PyTypeObject *);
				PyObject **p = va_arg(va,PyObject **);

#if PY_VERSION_HEX >= 0x02020000
				if (PyObject_TypeCheck(arg,type))
#else
				if (arg -> ob_type == type)
#endif
					*p = arg;
				else
					valid = PARSE_TYPE;

				break;
			}

		case 'R':
			{
#ifdef SIP_QT_SUPPORT
				/* Sub-class of QObject. */

				if (sipIsSubClassInstance(arg,sipQObjectClass))
					*va_arg(va,PyObject **) = arg;
				else
					valid = PARSE_TYPE;
#else
				valid = PARSE_TYPE;
#endif

				break;
			}

		case 'F':
			{
				/* Python callable object. */
 
				if (PyCallable_Check(arg))
					*va_arg(va,PyObject **) = arg;
				else
					valid = PARSE_TYPE;
 
				break;
			}

		case 'q':
			{
#ifdef SIP_QT_SUPPORT
				/* Qt receiver to connect. */

				va_arg(va,void *);
				va_arg(va,char *);
				va_arg(va,const void **);
				va_arg(va,char **);

				if (!sipIsSubClassInstance(arg,sipQObjectClass))
					valid = PARSE_TYPE;
#else
				valid = PARSE_TYPE;
#endif

				break;
			}

		case 'Q':
			{
#ifdef SIP_QT_SUPPORT
				/* Qt receiver to disconnect. */

				va_arg(va,char *);
				va_arg(va,const void **);
				va_arg(va,char **);

				if (!sipIsSubClassInstance(arg,sipQObjectClass))
					valid = PARSE_TYPE;
#else
				valid = PARSE_TYPE;
#endif

				break;
			}

		case 'y':
			{
#ifdef SIP_QT_SUPPORT
				/* Python slot to connect. */

				va_arg(va,void *);
				va_arg(va,char *);
				va_arg(va,const void **);
				va_arg(va,char **);

				if (!PyCallable_Check(arg))
					valid = PARSE_TYPE;
#else
				valid = PARSE_TYPE;
#endif

				break;
			}

		case 'Y':
			{
#ifdef SIP_QT_SUPPORT
				/* Python slot to disconnect. */

				va_arg(va,char *);
				va_arg(va,const void **);
				va_arg(va,char **);

				if (!PyCallable_Check(arg))
					valid = PARSE_TYPE;
#else
				valid = PARSE_TYPE;
#endif

				break;
			}

		case 'a':
			{
				/* Byte array or None. */

				char **p = va_arg(va,char **);
				int *szp = va_arg(va,int *);

				if (arg == Py_None)
				{
					*p = NULL;
					*szp = 0;
				}
				else if (PyString_Check(arg))
				{
					*p = PyString_AS_STRING(arg);
					*szp = PyString_GET_SIZE(arg);
				}
				else
					valid = PARSE_TYPE;

				break;
			}

		case 'c':
			{
				/* Character. */

				if (PyString_Check(arg) && PyString_GET_SIZE(arg) == 1)
					*va_arg(va,char *) = *PyString_AS_STRING(arg);
				else
					valid = PARSE_TYPE;

				break;
			}

		case 'b':
			{
				/* Bool. */

				long v = PyInt_AsLong(arg);

				if (PyErr_Occurred())
					valid = PARSE_TYPE;
				else
					sipSetBool(va_arg(va,void *),(int)v);

				break;
			}

		case 'e':
			{
				/* Enum. */

				long v = PyInt_AsLong(arg);

				if (PyErr_Occurred())
					valid = PARSE_TYPE;
				else
					*va_arg(va,int *) = (int)v;

				break;
			}

		case 'C':
			{
				/* Constrained integer - deprecated. */

				if (PyInt_Check(arg))
					*va_arg(va,int *) = (int)PyInt_AS_LONG(arg);
				else
					valid = PARSE_TYPE;

				break;
			}

		case 'i':
			{
				/* Integer. */

				long v = PyInt_AsLong(arg);

				if (PyErr_Occurred())
					valid = PARSE_TYPE;
				else
					*va_arg(va,int *) = (int)v;

				break;
			}

		case 'h':
			{
				/* Short integer. */

				long v = PyInt_AsLong(arg);

				if (PyErr_Occurred())
					valid = PARSE_TYPE;
				else
					*va_arg(va,short *) = (short)v;

				break;
			}

		case 'l':
			{
				/* Long integer. */

				long v = PyInt_AsLong(arg);

				if (PyErr_Occurred())
					valid = PARSE_TYPE;
				else
					*va_arg(va,long *) = v;

				break;
			}

		case 'L':
			{
				/* List object - deprecated. */

				PyObject **sp = va_arg(va,PyObject **);

				if (PyList_Check(arg))
					*sp = arg;
				else
					valid = PARSE_TYPE;

				break;
			}

		case 'x':
			{
				/* Slice object - deprecated. */

				PyObject **sp = va_arg(va,PyObject **);

				if (PySlice_Check(arg))
					*sp = arg;
				else
					valid = PARSE_TYPE;

				break;
			}

		case 'f':
			{
				/* Float. */

				double v = PyFloat_AsDouble(arg);

				if (PyErr_Occurred())
					valid = PARSE_TYPE;
				else
					*va_arg(va,float *) = (float)v;

				break;
			}

		case 'X':
			{
				/* Constrained (ie. exact) types. */

				switch (*fmt++)
				{
				case 'd':
					{
						/* Double float. */

						if (PyFloat_Check(arg))
							*va_arg(va,double *) = PyFloat_AS_DOUBLE(arg);
						else
							valid = PARSE_TYPE;

						break;
					}

				case 'f':
					{
						/* Float. */

						if (PyFloat_Check(arg))
							*va_arg(va,float *) = (float)PyFloat_AS_DOUBLE(arg);
						else
							valid = PARSE_TYPE;

						break;
					}

				case 'i':
					{
						/* Integer. */

						if (PyInt_Check(arg))
							*va_arg(va,int *) = (int)PyInt_AS_LONG(arg);
						else
							valid = PARSE_TYPE;

						break;
					}

				default:
					valid = PARSE_FORMAT;
				}

				break;
			}

		case 'd':
			{
				/* Double float. */

				double v = PyFloat_AsDouble(arg);

				if (PyErr_Occurred())
					valid = PARSE_TYPE;
				else
					*va_arg(va,double *) = v;

				break;
			}

		case 'v':
			{
				/* Void pointer. */

				void *v = sipConvertToVoidPtr(arg);

				if (PyErr_Occurred())
					valid = PARSE_TYPE;
				else
					*va_arg(va,void **) = v;

				break;
			}

		/*
		 * These are format characters supported by earlier versions.
		 * We make them cause an exception (so that people change their
		 * handwritten code) and to remind us not to re-use them.
		 */
		case '-':
		case 'I':
			/* Drop through. */

		default:
			valid = PARSE_FORMAT;
		}

		if (valid == PARSE_OK)
			++nrparsed;
	}

	*argsParsedp = nrparsed;

	return valid;
}


/*
 * Second pass of the argument parse, converting the remaining ones that might
 * have side effects.  Return PARSE_OK if there was no error.
 */
static int parsePass2(sipThisType *this,int selfarg,int nrargs,
		      PyObject *sipArgs,char *fmt,va_list va)
{
	int a, valid;

	valid = PARSE_OK;

	/* Handle the converions of "self" first. */

	switch (*fmt++)
	{
	case 't':
		{
			/*
			 * The address of the "sipThis" Python object that
			 * wraps the C++ address.
			 */

			PyObject *osot = va_arg(va,PyObject *);
			PyObject *classObj = va_arg(va,PyObject *);
			const void **p = va_arg(va,const void **);

			*(PyObject **)p = (PyObject *)this;

			break;
		}

	case 'm':
	case 'n':
		{
			/*
			 * The address of a C++ instance when calling one of
			 * its public methods.
			 */

			PyObject *osot = va_arg(va,PyObject *);
			PyObject *classObj = va_arg(va,PyObject *);
			const void **p = va_arg(va,const void **);

			if ((*p = sipGetCppPtr(this,classObj)) == NULL)
				valid = PARSE_RAISED;

			break;
		}

	case 'p':
		{
			/*
			 * The address of a C++ instance when calling one of
			 * its protected methods.
			 */

			PyObject *osot = va_arg(va,PyObject *);
			PyObject *classObj = va_arg(va,PyObject *);
			const void **p = va_arg(va,const void **);

			if ((*p = sipGetComplexCppPtr(this)) == NULL)
				valid = PARSE_RAISED;

			break;
		}

	default:
		--fmt;
	}

	for (a = (selfarg ? 1 : 0); a < nrargs && valid == PARSE_OK; ++a)
	{
		char ch;
		PyObject *arg = PyTuple_GET_ITEM(sipArgs,a);

		/* Skip the optional character. */

		if ((ch = *fmt++) == '|')
			ch = *fmt++;

		/*
		 * Do the outstanding conversions.  For most types it has
		 * already been done, so we are just skipping the parameters.
		 */

		switch (ch)
		{
		case 'q':
			{
#ifdef SIP_QT_SUPPORT
				/* Qt receiver to connect. */

				void *func = va_arg(va,void *);
				char *sig = va_arg(va,char *);
				const void **rx = va_arg(va,const void **);
				char **slot = va_arg(va,char **);

				if ((*rx = sipConvertRx(func,this,sig,arg,*slot,slot)) == NULL)
					valid = PARSE_RAISED;
#endif

				break;
			}

		case 'Q':
			{
#ifdef SIP_QT_SUPPORT
				/* Qt receiver to disconnect. */

				char *sig = va_arg(va,char *);
				const void **rx = va_arg(va,const void **);
				char **slot = va_arg(va,char **);

				if ((*rx = sipGetRx(this,sig,arg,*slot,slot)) == NULL)
					valid = PARSE_RAISED;
#endif

				break;
			}

		case 'y':
			{
#ifdef SIP_QT_SUPPORT
				/* Python slot to connect. */

				void *func = va_arg(va,void *);
				char *sig = va_arg(va,char *);
				const void **rx = va_arg(va,const void **);
				char **slot = va_arg(va,char **);

				if ((*rx = sipConvertRx(func,this,sig,arg,NULL,slot)) == NULL)
					valid = PARSE_RAISED;
#endif

				break;
			}

		case 'Y':
			{
#ifdef SIP_QT_SUPPORT
				/* Python slot to disconnect. */

				char *sig = va_arg(va,char *);
				const void **rx = va_arg(va,const void **);
				char **slot = va_arg(va,char **);

				if ((*rx = sipGetRx(this,sig,arg,NULL,slot)) == NULL)
					valid = PARSE_RAISED;
#endif

				break;
			}

		case 'J':
			{
				/* Class instance with no convertors. */

				PyObject *classObj = va_arg(va,PyObject *);
				const void **p = va_arg(va,const void **);
				int flags = *fmt++ - '0';
				int iserr = FALSE;
				PyObject **self;

				if (flags & FORMAT_GET_WRAPPER)
					self = va_arg(va,PyObject **);
				else
					self = NULL;

				*p = convertToCppAndThis(arg,classObj,&iserr,self);

				if (iserr)
					valid = PARSE_RAISED;
				else if (*p == NULL && (flags & FORMAT_DEREF))
				{
					setBadNone(a);
					valid = PARSE_RAISED;
				}
				else if (flags & FORMAT_TRANSFER)
					sipTransferSelf(arg,1);
				else if (flags & FORMAT_TRANSFER_BACK)
					sipTransferSelf(arg,0);

				break;
			}

		case 'K':
			{
				/*
				 * Class instance with no convertors and return
				 * the original Python class instance as
				 * well - deprecated.
				 */

				PyObject *classObj = va_arg(va,PyObject *);
				const void **p = va_arg(va,const void **);
				PyObject **pobj = va_arg(va,PyObject **);
				int iserr = FALSE;

				*p = sipConvertToCpp(arg,classObj,&iserr);

				if (iserr)
					valid = PARSE_RAISED;
				else
					*pobj = arg;

				break;
			}

		case 'M':
			{
				/* Class instance with convertors. */

				convfunc func = va_arg(va,convfunc);
				const void **p = va_arg(va,const void **);
				int *heap = va_arg(va,int *);
				int flags = *fmt++ - '0';
				int iserr = FALSE;

				*heap = (*func)(arg,(void *)p,&iserr);

				if (iserr)
					valid = PARSE_RAISED;
				else if (*p == NULL && (flags & FORMAT_DEREF))
				{
					setBadNone(a);
					valid = PARSE_RAISED;
				}
				else if (flags & FORMAT_TRANSFER)
					sipTransferSelf(arg,1);
				else if (flags & FORMAT_TRANSFER_BACK)
					sipTransferSelf(arg,0);

				break;
			}

		case 'P':
			{
				/*
				 * Python object of any type with a
				 * sub-format.
				 */

				PyObject **p = va_arg(va,PyObject **);
				int flags = *fmt++ - '0';

				if (flags & FORMAT_TRANSFER)
				{
					Py_XINCREF(*p);
				}
				else if (flags & FORMAT_TRANSFER_BACK)
				{
					Py_XDECREF(*p);
				}

				break;
			}

		case 'X':
			{
				/* Constrained (ie. exact) type. */

				++fmt;
				va_arg(va,void *);

				break;
			}

		/*
		 * Every other argument is a pointer and only differ in how
		 * many there are.
		 */

		case 'N':
		case 'T':
		case 'a':
			va_arg(va,void *);

			/* Drop through. */

		default:
			va_arg(va,void *);
		}
	}

	return valid;
}


/*
 * Carry out actions common to all ctors.
 */
void sipCommonCtor(sipMethodCache *cache,int nrmeths)
{
	/* This is thread safe. */

	while (nrmeths-- > 0)
		cache++ -> mcflags = 0;
}


/*
 * Carry out actions common to all dtors.
 */
void sipCommonDtor(sipThisType *sipThis)
{
	if (sipThis != NULL)
	{
		SIP_BLOCK_THREADS

		sipOMRemoveObject(&cppPyMap,sipThis -> u.cppPtr,sipThis);

		/* This no longer points to anything useful. */

		sipThis -> u.cppPtr = NULL;

		/* Delete any extra reference to the Python instance. */

		if (sipIsExtraRef(sipThis))
			Py_DECREF(sipThis -> sipSelf);

		SIP_UNBLOCK_THREADS
	}
}


/*
 * Convert a sequence index.  Return the index or a negative value if there was
 * an error.
 */
int sipConvertFromSequenceIndex(int idx,int len)
{
	/* Negative indices start from the other end. */
	if (idx < 0)
		idx = len + idx;

	if (idx < 0 || idx >= len)
	{
		PyErr_Format(PyExc_IndexError,"list index out of range");
		return -1;
	}

	return idx;
}


#if PY_VERSION_HEX < 0x02030000
/*
 * Get a slice index.  This is a copy of _PyEval_SliceIndex() from Python v2.3
 * with slight changes so that it will work with Python v1.5.2.
 */
static int getSliceIndex(PyObject *v,int *pi)
{
	if (v != NULL) {
		long x;
		if (PyInt_Check(v)) {
			x = PyInt_AsLong(v);
		} else if (PyLong_Check(v)) {
			x = PyLong_AsLong(v);
			if (x==-1 && PyErr_Occurred()) {
				PyObject *long_zero;
				int cmp;

				if (!PyErr_ExceptionMatches(
					PyExc_OverflowError)) {
					/* It's not an overflow error, so just
					   signal an error */
					return 0;
				}

				/* Clear the OverflowError */
				PyErr_Clear();

				/* It's an overflow error, so we need to
				   check the sign of the long integer,
				   set the value to INT_MAX or 0, and clear
				   the error. */

				/* Create a long integer with a value of 0 */
				long_zero = PyLong_FromLong(0L);
				if (long_zero == NULL)
					return 0;

				/* Check sign */
				cmp = PyObject_Compare(v, long_zero);
				Py_DECREF(long_zero);
				if (PyErr_Occurred())
					return 0;
				else if (cmp > 0)
					x = INT_MAX;
				else
					x = 0;
			}
		} else {
			PyErr_SetString(PyExc_TypeError,
					"slice indices must be integers");
			return 0;
		}
		/* Truncate -- very long indices are truncated anyway */
		if (x > INT_MAX)
			x = INT_MAX;
		else if (x < -INT_MAX)
			x = -INT_MAX;
		*pi = x;
	}
	return 1;
}
#endif


/*
 * Convert a slice object.  This is a copy of PySlice_GetIndicesEx() from
 * Python v2.3.
 */
int sipConvertFromSliceObject(PyObject *slice,int length,int *start,int *stop,
			      int *step,int *slicelength)
{
#if PY_VERSION_HEX >= 0x02030000
	return PySlice_GetIndicesEx((PySliceObject *)slice,length,start,stop,step,slicelength);
#else
	/* This is a copy of PySlice_GetIndicesEx() from Python v2.3. */

	PySliceObject *r = (PySliceObject *)slice;
	int defstart, defstop;

	if (r->step == Py_None) {
		*step = 1;
	} 
	else {
		if (!getSliceIndex(r->step, step)) return -1;
		if (*step == 0) {
			PyErr_SetString(PyExc_ValueError,
					"slice step cannot be zero");
			return -1;
		}
	}

	defstart = *step < 0 ? length-1 : 0;
	defstop = *step < 0 ? -1 : length;

	if (r->start == Py_None) {
		*start = defstart;
	}
	else {
		if (!getSliceIndex(r->start, start)) return -1;
		if (*start < 0) *start += length;
		if (*start < 0) *start = (*step < 0) ? -1 : 0;
		if (*start >= length) 
			*start = (*step < 0) ? length - 1 : length;
	}

	if (r->stop == Py_None) {
		*stop = defstop;
	}
	else {
		if (!getSliceIndex(r->stop, stop)) return -1;
		if (*stop < 0) *stop += length;
		if (*stop < 0) *stop = -1;
		if (*stop > length) *stop = length;
	}

	if ((*step < 0 && *stop >= *start) 
	    || (*step > 0 && *start >= *stop)) {
		*slicelength = 0;
	}
	else if (*step < 0) {
		*slicelength = (*stop-*start+1)/(*step)+1;
	}
	else {
		*slicelength = (*stop-*start-1)/(*step)+1;
	}

	return 0;
#endif
}


/*
 * A convenience function to convert a C/C++ boolean to a Python object.
 */
PyObject *sipConvertFromBool(int val)
{
	PyObject *pyobj;

	pyobj = (val ? Py_True : Py_False);

	Py_INCREF(pyobj);

	return pyobj;
}


/*
 * Call the ctor for any of a module's classes.
 */
PyObject *sipCallCtor(sipModuleDef *sm,PyObject *args)
{
	int c;
	PyObject *pySelf, *ctorargs;
	sipClassDef *scd;

	if (!PyArg_ParseTuple(args,"iOO",&c,&pySelf,&ctorargs))
		return NULL;

	scd = &sm -> md_classes[c];

	if (scd -> cd_classptr == NULL)
	{
		PyErr_Format(PyExc_TypeError,"%s has been disabled in this configuration",scd -> cd_name);

		return NULL;
	}

	if (scd -> cd_thisctor == NULL)
	{
		PyErr_Format(PyExc_TypeError,"%s represents a C++ namespace and cannot be created",scd -> cd_name);

		return NULL;
	}

	if (!PyInstance_Check(pySelf))
	{
		PyErr_Format(PyExc_TypeError,"cannot create a class derived from a SIP generated class (%s) and a new-style class - use SIP v4 or later instead",scd -> cd_name);

		return NULL;
	}

	return (*scd -> cd_thisctor)(pySelf,ctorargs);
}


/*
 * Get the "sipThis" object from the instance dictionary.
 */
static PyObject *getThisFromInstance(PyInstanceObject *inst)
{
	PyObject *sipThis;

	if ((sipThis = PyDict_GetItem(inst -> in_dict,thisName)) != NULL)
		return sipThis;

	PyErr_SetObject(PyExc_AttributeError,thisName);

	return NULL;
}


/*
 * Handle the dtor for any class.  This ensures that the wrapped object is
 * deleted after all the other things in the instance.  This more closely
 * mimics what C++ does and can avoid some very, very subtle problems.
 */
static PyObject *sipDtor(PyObject *ignore,PyObject *args)
{
	PyInstanceObject *pySelf;
	PyObject *sipThis;

	if (!PyArg_ParseTuple(args,"O!",&PyInstance_Type,&pySelf))
		return NULL;

	if ((sipThis = PyDict_GetItem(pySelf -> in_dict,thisName)) != NULL)
	{
		/* Keep this alive. */

		Py_INCREF(sipThis);

		/* Kill everything else. */

		PyDict_Clear(pySelf -> in_dict);

		/*
		 * Finally try and kill this. However, it may not succeed if
		 * Python is still keeping a reference to it.  I think the only
		 * way it can do this is if it has a reference to one of the
		 * class's lazy methods which itself will have a reference to
		 * sipThis.  Therefore we NULL the self pointer so that we know
		 * that the Python instance has gone.
		 */

		((sipThisType *)sipThis) -> sipSelf = NULL;

		Py_DECREF(sipThis);
	}

	Py_INCREF(Py_None);
	return Py_None;
}


/*
 * Initialise a single class object.
 */
static int initClassObject(sipModuleDef *sm,int c)
{
	sipClassDef *cd = &sm -> md_classes[c];
	PyObject *dict, *cls;

	/* Check the class is valid for this version. */

	if (cd -> cd_name == NULL || cd -> cd_classptr == NULL)
		return 0;

	/* Check we haven't already done it. */

	if (*cd -> cd_classptr != NULL)
		return 0;

	/*
	 * If there is an enclosing scope, do that first. At the same time, get
	 * the dictionary we need to look in.
	 */

	if (cd -> cd_scope >= 0)
	{
		if (initClassObject(sm,cd -> cd_scope) < 0)
			return -1;

		/* Get the enclosing scope's class dictionary. */

		dict = ((PyClassObject *)(*sm -> md_classes[cd -> cd_scope].cd_classptr)) -> cl_dict;
	}
	else
		dict = sm -> md_dict;

	/* Now get this one. */

	if ((cls = PyDict_GetItemString(dict,cd -> cd_name)) == NULL)
	{
		PyErr_Format(PyExc_SystemError,"Unable to find class object for %s",cd -> cd_name);

		return -1;
	}

	*cd -> cd_classptr = cls;

	/*
	 * Add the (non-lazy) dtor if it's needed, ie. the class has no
	 * super-classes.
	 */

	if (PyTuple_Size(((PyClassObject *)cls) -> cl_bases) == 0)
		if (addCallableToClass(cls,"__del__",dtorFunction) < 0)
			return -1;

	/* Add any non-lazy methods to the class dictionary. */

	if (cd -> cd_attrtab != NULL)
	{
		int i;
		PyMethodDef *pmd;
		PyObject *cls_dict;

		/* Get the class dictionary. */

		cls_dict = ((PyClassObject *)cls) -> cl_dict;

		pmd = cd -> cd_attrtab -> la_nlmethods;

		for (i = 0; i < cd -> cd_attrtab -> la_nrnlmethods; ++i)
		{
			int rc;
			PyObject *func;

			if ((func = PyCFunction_New(pmd,NULL)) == NULL)
				return -1;

			rc = addCallableToClass(cls,pmd -> ml_name,func);

			Py_DECREF(func);

			if (rc < 0)
				return -1;

			++pmd;
		}
	}

	return 0;
}


/*
 * Add a callable to a class as an unbound method.  Return a negative value if
 * there was an error.
 */
static int addCallableToClass(PyObject *cls,char *name,PyObject *func)
{
	int rc;
	PyObject *meth;

	if ((meth = PyMethod_New(func,NULL,cls)) == NULL)
		return -1;

	rc = PyDict_SetItemString(((PyClassObject *)cls) -> cl_dict,name,meth);

	Py_DECREF(meth);

	return rc;
}


/*
 * Register a module's classes.
 */
int sipRegisterClasses(sipModuleDef *sm,int qobjclass)
{
	int c;

	/* Get the class objects. */

	for (c = 0; c < sm -> md_nrclasses; ++c)
		if (initClassObject(sm,c) < 0)
			return -1;

	/* Check we can handle Qt if needed. */

	if (qobjclass >= 0)
	{
#ifdef SIP_QT_SUPPORT
#if PY_VERSION_HEX >= 0x02010000 && PY_VERSION_HEX < 0x02020000
		/*
		 * If this is Python 2.1 and we might be using Qt signals, then
		 * the weak references module will be useful (but not
		 * essential).  With later versions of Python we use the direct
		 * API.
		 */

		PyObject *wrmod;

		if ((wrmod = PyImport_ImportModule("_weakref")) != NULL)
			weakRefMeth = PyObject_GetAttrString(wrmod,"ref");

		/* Just in case. */

		PyErr_Clear();
#endif

		if (sipQObjectClass != NULL)
		{
			PyErr_Format(PyExc_RuntimeError,"SIP - module \"%s\" implements QObject but it has already been implemented",PyString_AsString(sm -> md_name));
			return -1;
		}

		sipQObjectClass = *sm -> md_classes[qobjclass].cd_classptr;
#else
		PyErr_Format(PyExc_RuntimeError,"SIP: module \"%s\" requires Qt support from the SIP module",PyString_AsString(sm -> md_name));
		return -1;
#endif
	}

	return 0;
}


/*
 * Get "this" from the argument tuple for a method called as
 * Class.Method(self,...) rather than self.Method(...).
 */
static int getThisFromArgs(PyObject *classObj,PyObject *args,int argnr,sipThisType **thisp)
{
	PyObject *self;

	/* Get self from the argument tuple. */

	if (argnr >= PyTuple_GET_SIZE(args))
		return PARSE_UNBOUND;

	self = PyTuple_GET_ITEM(args,argnr);

	if (!sipIsSubClassInstance(self,classObj))
		return PARSE_UNBOUND;

	if ((*thisp = sipMapSelfToThis(self)) == NULL)
		return PARSE_RAISED;

	return PARSE_OK;
}


/*
 * This is the replacement class getattro function.  We want to create a
 * class's methods on demand because creating all of them at the start slows
 * the startup time and consumes too much memory, especially for scripts that
 * use a small subset of a large class library.  We used to define a
 * __getattr__ function for each class, but this doesn't work when you want to
 * use the standard technique for calling a base class's method from the same
 * method in a derived class, ie. Base.method(self,...).
 */
static PyObject *classGetAttr(PyObject *op,PyObject *nameobj)
{
	PyObject *attr;

	if ((attr = (*savedClassGetAttr)(op,nameobj)) != NULL)
	{
		char *name;
		int i;
		sipClassDef *cd;
		sipLazyAttrDef *la;
		PyObject *dict;
		sipEnumValueInstanceDef *evi;
		PyMethodDef *pmd;

		/*
		 * See if the dictionary of a SIP'ed class is being returned.
		 */

		if (!PyDict_Check(attr))
			return attr;

		name = PyString_AsString(nameobj);

		if (name == NULL || strcmp(name,"__dict__") != 0)
			return attr;

		if ((cd = findClass(op)) == NULL || (la = cd -> cd_attrtab) == NULL)
			return attr;

		/*
		 * Add the class's lazy enums to the class dictionary.  It
		 * doesn't matter if it has already been cached there.
		 */

		evi = la -> la_enums;

		for (i = 0; i < la -> la_nrenums; ++i)
		{
			int rc;
			PyObject *val;

			if ((val = PyInt_FromLong(evi -> evi_val)) == NULL)
			{
				Py_DECREF(attr);
				return NULL;
			}

			rc = PyDict_SetItemString(attr,evi -> evi_name,val);

			Py_DECREF(val);

			if (rc < 0)
			{
				Py_DECREF(attr);
				return NULL;
			}

			++evi;
		}

		/*
		 * We can't cache lazy methods so if there are any we need to
		 * create a copy of the dictionary and return that.
		 */

		if (la -> la_nrmethods == 0)
			return attr;

#if PY_VERSION_HEX >= 0x01060000
		dict = PyDict_Copy(attr);
#else
		if ((dict = PyDict_New()) != NULL)
		{
			int pos = 0;
			PyObject *key, *val;

			while (PyDict_Next(attr,&pos,&key,&val))
				if (PyDict_SetItem(dict,key,val) < 0)
				{
					Py_DECREF(dict);
					dict = NULL;

					break;
				}
		}
#endif

		Py_DECREF(attr);

		if (dict == NULL)
			return NULL;

		pmd = la -> la_methods;

		for (i = 0; i < la -> la_nrmethods; ++i)
		{
			int rc;
			PyObject *meth;

			if ((meth = PyCFunction_New(pmd,NULL)) == NULL)
			{
				Py_DECREF(dict);
				return NULL;
			}

			rc = PyDict_SetItemString(dict,pmd -> ml_name,meth);

			Py_DECREF(meth);

			if (rc < 0)
			{
				Py_DECREF(dict);
				return NULL;
			}

			++pmd;
		}

		return dict;
	}

	return handleLazyAttr(op,nameobj,(PyClassObject *)op,NULL);
}


/*
 * This is the replacement instance getattro function.  This could be
 * handled using a Python __getattr__ method - but seeing has we need to do
 * it this way for classes, we might as well do it this way for instances.
 */
static PyObject *instanceGetAttr(PyObject *op,PyObject *nameobj)
{
	PyObject *attr;

	if ((attr = (*savedInstanceGetAttr)(op,nameobj)) != NULL)
		return attr;

	return handleLazyAttr(op,nameobj,((PyInstanceObject *)op) -> in_class,op);
}


/*
 * Handle the result of a call to the Python class/instance getattr functions.
 */
static PyObject *handleLazyAttr(PyObject *op,PyObject *nameobj,PyClassObject *clsobj,PyObject *instance)
{
	char *name;
	PyMethodDef *pmd;
	sipEnumValueInstanceDef *enm;
	PyObject *attr, *sipThis;

	/* If it was an error, propagate it. */

	if (!PyErr_ExceptionMatches(PyExc_AttributeError))
		return NULL;

	PyErr_Clear();

	/* See if it was a lazy attribute. */

	if ((name = PyString_AsString(nameobj)) == NULL)
		return NULL;

	pmd = NULL;
	enm = NULL;

	findLazyAttr((PyObject *)clsobj,name,&pmd,&enm);

	if (enm != NULL)
	{
		/*
		 * Convert the value to an object and cache it in the class
		 * dictionary.
		 */

		if ((attr = PyInt_FromLong(enm -> evi_val)) == NULL)
			return NULL;

		if (PyDict_SetItem(clsobj -> cl_dict,nameobj,attr) < 0)
		{
			Py_DECREF(attr);

			return NULL;
		}

		return attr;
	}
	else if (pmd == NULL)
	{
		PyErr_SetObject(PyExc_AttributeError,nameobj);
		return NULL;
	}

	/*
	 * We don't cache methods called using class.method(self,...).  If we
	 * did then, because we use the Python class and instance getattr
	 * functions before we see if an attribute is lazy, then in the case
	 * that we were looking for a method that is being called using
	 * self.method(...) for the first time - and after it has been called
	 * using class.method(self,...) - the Python instance getattr function
	 * would find the unbound method which would cause an exception.
	 * Because the most common calling method is cached, the performance
	 * penalty should be negligable.
	 */

	if (instance == NULL)
		return PyCFunction_New(pmd,NULL);

	/* Get "this". */

	if ((sipThis = getThisFromInstance((PyInstanceObject *)instance)) == NULL)
		return NULL;

	/* Convert it to a function. */

	if ((attr = PyCFunction_New(pmd,sipThis)) == NULL)
		return NULL;

	/* Cache it in the instance dictionary. */

	if (PyDict_SetItem(((PyInstanceObject *)instance) -> in_dict,nameobj,attr) < 0)
	{
		Py_DECREF(attr);
		return NULL;
	}

	return attr;
}


/*
 * Find the data structure for a SIP class.
 */
static sipClassDef *findClass(PyObject *op)
{
	sipModuleDef *sm;

	for (sm = modulesList; sm != NULL; sm = sm -> md_next)
	{
		int c;
		sipClassDef *cd = sm -> md_classes;

		for (c = 0; c < sm -> md_nrclasses; ++c)
		{
			if (cd -> cd_name != NULL && cd -> cd_classptr != NULL && *cd -> cd_classptr == op)
				return cd;

			++cd;
		}
	}

	return NULL;
}


/*
 * Find definition for a lazy class attribute.
 */
static void findLazyAttr(PyObject *op,char *name,PyMethodDef **pmdp,
			 sipEnumValueInstanceDef **enmp)
{
	int b, nrbases;
	sipClassDef *cd;
	sipLazyAttrDef *la;
	PyObject *bases;

	/* See if this is a SIP class. */

	if ((cd = findClass(op)) != NULL && (la = cd -> cd_attrtab) != NULL)
	{
		/* Try the methods. */

		if (la -> la_nrmethods > 0 &&
		    (*pmdp = (PyMethodDef *)bsearch(name,la -> la_methods,la -> la_nrmethods,sizeof (PyMethodDef),compareMethodName)) != NULL)
			return;

		/* Try the enums. */

		if (la -> la_nrenums > 0 &&
		    (*enmp = (sipEnumValueInstanceDef *)bsearch(name,la -> la_enums,la -> la_nrenums,sizeof (sipEnumValueInstanceDef),compareEnumName)) != NULL)
			return;
	}

	/* Check the base classes. */

	bases = ((PyClassObject *)op) -> cl_bases;
	nrbases = PyTuple_Size(bases);

	for (b = 0; b < nrbases; ++b)
	{
		findLazyAttr(PyTuple_GET_ITEM(bases,b),name,pmdp,enmp);

		if (*pmdp != NULL || *enmp != NULL)
			break;
	}
}


/*
 * The bsearch() helper function for searching a sorted method table.
 */
static int compareMethodName(const void *key,const void *el)
{
	return strcmp((const char *)key,((const PyMethodDef *)el) -> ml_name);
}


/*
 * The bsearch() helper function for searching a sorted enum table.
 */
static int compareEnumName(const void *key,const void *el)
{
	return strcmp((const char *)key,((const sipEnumValueInstanceDef *)el) -> evi_name);
}


/*
 * Report a constructor with invalid argument types.
 */
void sipNoCtor(int argsParsed,char *classname)
{
	badArgs(argsParsed,NULL,classname);
}


/*
 * Report a function with invalid argument types.
 */
void sipNoFunction(int argsParsed,char *func)
{
	badArgs(argsParsed,NULL,func);
}


/*
 * Report a method/function/signal with invalid argument types.
 */
void sipNoMethod(int argsParsed,char *classname,char *method)
{
	badArgs(argsParsed,classname,method);
}


/*
 * Handle error reporting for bad arguments to various things.
 */
static void badArgs(int argsParsed,char *classname,char *method)
{
	char *sep;
	int nrparsed = argsParsed & ~PARSE_MASK;

	if (classname != NULL)
		sep = ".";
	else
	{
		classname = "";
		sep = "";
	}

	switch (argsParsed & PARSE_MASK)
	{
	case PARSE_FEW:
		PyErr_Format(PyExc_TypeError,"Insufficient number of arguments to %s%s%s()",classname,sep,method);
		break;

	case PARSE_MANY:
		PyErr_Format(PyExc_TypeError,"Too many arguments to %s%s%s(), %d at most expected",classname,sep,method,nrparsed);
		break;

	case PARSE_TYPE:
		PyErr_Format(PyExc_TypeError,"Argument %d of %s%s%s() has an invalid type",nrparsed + 1,classname,sep,method);
		break;

	case PARSE_FORMAT:
		PyErr_Format(PyExc_TypeError,"Invalid format to sipParseArgs() from %s%s%s()",classname,sep,method);
		break;

	case PARSE_UNBOUND:
		PyErr_Format(PyExc_TypeError,"First argument of unbound method %s%s%s() must be a %s instance",classname,sep,method,classname);
		break;

	case PARSE_RAISED:
		/* It has already been taken care of. */

		break;

	case PARSE_OK:
		PyErr_Format(PyExc_SystemError,"Internal error: %s%s%s",classname,sep,method);
		break;
	}
}


/*
 * Report a sequence length that does not match the length of a slice.
 */
void sipBadLengthForSlice(int seqlen,int slicelen)
{
	PyErr_Format(PyExc_ValueError,"attempt to assign sequence of size %d to slice of size %d",seqlen,slicelen);
}


/*
 * Report a Python object that cannot be converted to a particular class.
 */
void sipBadClass(char *classname)
{
	PyErr_Format(PyExc_TypeError,"Cannot convert Python object to an instance of %s",classname);
}


/*
 * Report an argument that can't be None.
 */
static void setBadNone(int a)
{
	PyErr_Format(PyExc_RuntimeError,"Cannot pass None as argument %d in this call",a);
}


/*
 * Report a Python class variable with an unexpected type.
 */
void sipBadSetType(char *classname,char *var)
{
	PyErr_Format(PyExc_TypeError,"Invalid type for variable %s.%s",classname,var);
}


/*
 * Report a Python member function with an unexpected return type.
 */
void sipBadCatcherResult(const sipBoundMethod *bmeth)
{
	char *cname, *mname;

	cname = PyString_AS_STRING(((PyInstanceObject *)(bmeth -> sipThis -> sipSelf)) -> in_class -> cl_name);
	mname = ((PyCFunctionObject *)(bmeth -> method -> mfunc)) -> m_ml -> ml_name;

	sipBadVirtualResultType(cname,mname);
}


/*
 * Report a Python member function with an unexpected return type - deprecated.
 */
void sipBadVirtualResultType(char *classname,char *method)
{
	PyErr_Format(PyExc_TypeError,"Invalid result type from %s.%s()",classname,method);
}


/*
 * Return the name of the class corresponding to a wrapper object.  This comes
 * with a reference.
 */
PyObject *sipClassName(PyObject *sipThis)
{
	PyObject *nm = ((PyInstanceObject *)(((sipThisType *)sipThis) -> sipSelf)) -> in_class -> cl_name;

	Py_INCREF(nm);

	return nm;
}


/*
 * Transfer ownership of a wrapper object from Python to C/C++, or vice versa.
 */
void sipTransfer(PyObject *sipThis,int toCpp)
{
	PyObject *sipSelf = ((sipThisType *)sipThis) -> sipSelf;

	if (toCpp)
	{
		sipResetPyOwned((sipThisType *)sipThis);

		if (!sipIsExtraRef((sipThisType *)sipThis))
		{
			sipSetIsExtraRef((sipThisType *)sipThis);
			Py_INCREF(sipSelf);
		}
	}
	else
	{
		sipSetPyOwned((sipThisType *)sipThis);

		if (sipIsExtraRef((sipThisType *)sipThis))
		{
			sipResetIsExtraRef((sipThisType *)sipThis);
			Py_DECREF(sipSelf);
		}
	}
}


/*
 * Transfer ownersip of a class instance from Python to C/C++, or vice versa.
 */
void sipTransferSelf(PyObject *sipSelf,int toCpp)
{
	if (sipSelf != NULL && sipSelf != Py_None)
	{
		sipThisType *sipThis;

		/*
		 * There is a legitimate case where we try to transfer a
		 * PyObject that may not be a SIP generated class.  The virtual
		 * handler code calls this function to keep the C/C++ instance
		 * alive when it gets rid of the Python object returned by the
		 * Python method.  A class may have handwritten code that
		 * converts a regular Python type - so we can't assume that
		 * the lack of a "sipThis" is an error.
		 */

		if ((sipThis = sipMapSelfToThis(sipSelf)) == NULL)
			PyErr_Clear();
		else
			sipTransfer((PyObject *)sipThis,toCpp);
	}
}


/*
 * Add a license to a dictionary.
 */
int sipAddLicense(PyObject *dict,sipLicenseDef *lc)
{
	int rc;
	PyObject *ldict, *o;

	/* Convert the strings we use to objects if not already done. */

	if (licenseName == NULL && (licenseName = PyString_FromString("__license__")) == NULL)
		return -1;

	if (licenseeName == NULL && (licenseeName = PyString_FromString("Licensee")) == NULL)
		return -1;

	if (typeName == NULL && (typeName = PyString_FromString("Type")) == NULL)
		return -1;

	if (timestampName == NULL && (timestampName = PyString_FromString("Timestamp")) == NULL)
		return -1;

	if (signatureName == NULL && (signatureName = PyString_FromString("Signature")) == NULL)
		return -1;

	/* We use a dictionary to hold the license information. */

	if ((ldict = PyDict_New()) == NULL)
		return -1;

	/* The license type is compulsory, the rest are optional. */

	if (lc -> lc_type == NULL || (o = PyString_FromString(lc -> lc_type)) == NULL)
		goto deldict;

	rc = PyDict_SetItem(ldict,typeName,o);
	Py_DECREF(o);

	if (rc < 0)
		goto deldict;

	if (lc -> lc_licensee != NULL)
	{
		if ((o = PyString_FromString(lc -> lc_licensee)) == NULL)
			goto deldict;

		rc = PyDict_SetItem(ldict,licenseeName,o);
		Py_DECREF(o);

		if (rc < 0)
			goto deldict;
	}

	if (lc -> lc_timestamp != NULL)
	{
		if ((o = PyString_FromString(lc -> lc_timestamp)) == NULL)
			goto deldict;

		rc = PyDict_SetItem(ldict,timestampName,o);
		Py_DECREF(o);

		if (rc < 0)
			goto deldict;
	}

	if (lc -> lc_signature != NULL)
	{
		if ((o = PyString_FromString(lc -> lc_signature)) == NULL)
			goto deldict;

		rc = PyDict_SetItem(ldict,signatureName,o);
		Py_DECREF(o);

		if (rc < 0)
			goto deldict;
	}

#if PY_VERSION_HEX >= 0x02020000
	/* Create a read-only proxy if they are supported. */

	{
		PyObject *proxy;

		if ((proxy = PyDictProxy_New(ldict)) == NULL)
			goto deldict;

		Py_DECREF(ldict);

		ldict = proxy;
	}
#endif

	rc = PyDict_SetItem(dict,licenseName,ldict);
	Py_DECREF(ldict);

	return rc;

deldict:
	Py_DECREF(ldict);

	return -1;
}


/*
 * Add the global void pointer instances to a dictionary.
 */
int sipAddVoidPtrInstances(PyObject *dict,sipVoidPtrInstanceDef *vi)
{
	while (vi -> vi_name != NULL)
	{
		int rc;
		PyObject *w;

		if ((w = sipConvertFromVoidPtr(vi -> vi_val)) == NULL)
			return -1;

		rc = PyDict_SetItemString(dict,vi -> vi_name,w);
		Py_DECREF(w);

		if (rc < 0)
			return -1;

		++vi;
	}

	return 0;
}


/*
 * Add the global char instances to a dictionary.
 */
int sipAddCharInstances(PyObject *dict,sipCharInstanceDef *ci)
{
	while (ci -> ci_name != NULL)
	{
		int rc;
		PyObject *w;

		if ((w = PyString_FromStringAndSize(&ci -> ci_val,1)) == NULL)
			return -1;

		rc = PyDict_SetItemString(dict,ci -> ci_name,w);
		Py_DECREF(w);

		if (rc < 0)
			return -1;

		++ci;
	}

	return 0;
}


/*
 * Add the global string instances to a dictionary.
 */
int sipAddStringInstances(PyObject *dict,sipStringInstanceDef *si)
{
	while (si -> si_name != NULL)
	{
		int rc;
		PyObject *w;

		if ((w = PyString_FromString(si -> si_val)) == NULL)
			return -1;

		rc = PyDict_SetItemString(dict,si -> si_name,w);
		Py_DECREF(w);

		if (rc < 0)
			return -1;

		++si;
	}

	return 0;
}


/*
 * Add the global long instances to a dictionary.
 */
int sipAddLongInstances(PyObject *dict,sipLongInstanceDef *li)
{
	while (li -> li_name != NULL)
	{
		int rc;
		PyObject *w;

		if ((w = PyInt_FromLong(li -> li_val)) == NULL)
			return -1;

		rc = PyDict_SetItemString(dict,li -> li_name,w);
		Py_DECREF(w);

		if (rc < 0)
			return -1;

		++li;
	}

	return 0;
}


/*
 * Add the global double instances to a dictionary.
 */
int sipAddDoubleInstances(PyObject *dict,sipDoubleInstanceDef *di)
{
	while (di -> di_name != NULL)
	{
		int rc;
		PyObject *w;

		if ((w = PyFloat_FromDouble(di -> di_val)) == NULL)
			return -1;

		rc = PyDict_SetItemString(dict,di -> di_name,w);
		Py_DECREF(w);

		if (rc < 0)
			return -1;

		++di;
	}

	return 0;
}


/*
 * Add enum instances to a dictionary.
 */
int sipAddEnumInstances(PyObject *dict,sipEnumValueInstanceDef *evi)
{
	while (evi -> evi_name != NULL)
	{
		int rc;
		PyObject *w;

		if ((w = PyInt_FromLong(evi -> evi_val)) == NULL)
			return -1;

		rc = PyDict_SetItemString(dict,evi -> evi_name,w);
		Py_DECREF(w);

		if (rc < 0)
			return -1;

		++evi;
	}

	return 0;
}


/*
 * Wrap the global class instances and add them to a dictionary.
 */
int sipAddClassInstances(PyObject *dict,sipClassInstanceDef *ci)
{
	while (ci -> ci_name != NULL)
	{
		int rc;
		PyObject *w;

		if ((w = sipNewCppToSelf(ci -> ci_ptr,ci -> ci_class,ci -> ci_flags)) == NULL)
			return -1;

		rc = PyDict_SetItemString(dict,ci -> ci_name,w);
		Py_DECREF(w);

		if (rc < 0)
			return -1;

		++ci;
	}

	return 0;
}


/*
 * Get the C/C++ pointer for a complex object.
 */
const void *sipGetComplexCppPtr(sipThisType *w)
{
	if (sipIsSimple(w))
	{
		PyErr_SetString(PyExc_RuntimeError,"No access to protected functions or signals for object not created from Python");

		return NULL;
	}

	if (checkPointer(w -> u.cppPtr) < 0)
		return NULL;

	return w -> u.cppPtr;
}


/*
 * Create a number of functions and add them to a dictionary.
 */
int sipAddFunctions(PyObject *dict,PyMethodDef *md)
{
	while (md -> ml_name != NULL)
	{
		PyObject *func;

		if ((func = PyCFunction_New(md,NULL)) == NULL ||
		    PyDict_SetItemString(dict,md -> ml_name,func) < 0)
			return -1;

		++md;
	}

	return 0;
}


/*
 * Search a variable hierachy table for a name.
 */
static PyMethodDef *searchVarTable(PyMethodDef **vhiertab,PyObject *nameobj)
{
	PyMethodDef *vt;

	while ((vt = *vhiertab++) != NULL)
		while (vt -> ml_name != NULL)
		{
			if (strcmp(vt -> ml_name,PyString_AS_STRING(nameobj)) == 0)
				return vt;

			++vt;
		}

	return NULL;
}


/*
 * Handle the getting of class variables.
 */
PyObject *sipGetVar(sipModuleDef *sm,PyObject *args)
{
	int c;
	PyObject *sipSelf, *nameobj;
	PyMethodDef *at;

	if (!PyArg_ParseTuple(args,"iOS",&c,&sipSelf,&nameobj))
		return NULL;

	if ((at = searchVarTable(sm -> md_classes[c].cd_vartab,nameobj)) == NULL)
	{
		PyErr_SetObject(PyExc_AttributeError,nameobj);
		return NULL;
	}

	return (*at -> ml_meth)((PyObject *)sipMapSelfToThis(sipSelf),NULL);
}


/*
 * Handle the setting of class variables.
 */
PyObject *sipSetVar(sipModuleDef *sm,PyObject *args)
{
	int c;
	PyObject *sipSelf, *nameobj, *valobj = NULL;
	PyMethodDef *at;

	if (!PyArg_ParseTuple(args,"iOS|O",&c,&sipSelf,&nameobj,&valobj))
		return NULL;

	if ((at = searchVarTable(sm -> md_classes[c].cd_vartab,nameobj)) == NULL)
	{
		if (sipSetInstanceAttr(sipSelf,nameobj,valobj) < 0)
			return NULL;

		Py_INCREF(Py_None);
		return Py_None;
	}

	if (valobj == NULL)
	{
		PyErr_Format(PyExc_AttributeError,"%s cannot be deleted",at -> ml_name);
		return NULL;
	}

	return (*at -> ml_meth)((PyObject *)sipMapSelfToThis(sipSelf),valobj);
}


/*
 * Get the class of a Python instance.
 */
PyObject *sipGetClass(PyObject *sipSelf)
{
	return (PyObject*)((PyInstanceObject *)sipSelf) -> in_class;
}


/*
 * Find the Python member function corresponding to a C/C++ virtual function,
 * if any.  If one was found then the Python lock is acquired.
 */
int sipIsPyMethod(sipMethodCache *pymc,sipThisType *sipThis,char *cname,
		  char *mname)
{
	int found;

	SIP_BLOCK_THREADS

	found = sipFindPyMethod(pymc,sipThis,cname,mname);

	if (!found)
	{
		SIP_UNBLOCK_THREADS
	}

	return found;
}


/*
 * Find the Python member function corresponding to a C/C++ virtual function,
 * if any.  The Python lock must have been acquired.
 */
int sipFindPyMethod(sipMethodCache *pymc,sipThisType *sipThis,char *cname,
		    char *mname)
{
	/*
	 * It's possible that the Python object has been deleted but the
	 * underlying (complex) C/C++ instance is still working and trying to
	 * handle virtual functions.  Or the Python object hasn't been fully
	 * created yet.  In either case say there is no Python method.
	 */

	if (sipThis == NULL)
		return FALSE;

	/* See if we have already looked for the Python method. */

	if (!sipFoundMethod(pymc))
	{
		PyObject *methobj;

		/*
		 * Using PyMethod_Check() rather than PyCallable_Check() has
		 * the added benefits of ensuring the (common) case of there
		 * being no Python method is handled as a direct call to C/C++
		 * (rather than converted to Python and then back to C/C++) and
		 * makes sure that abstract virtuals are trapped.
		 */

		if ((methobj = PyObject_GetAttrString(sipGetClass(sipThis -> sipSelf),mname)) != NULL)
		{
			if (PyMethod_Check(methobj))
			{
				sipSetIsMethod(pymc);
				sipSaveMethod(&pymc -> pyMethod,methobj);
			}

			Py_DECREF(methobj);
		}

		PyErr_Clear();

		sipSetFoundMethod(pymc);
	}
	else if (sipIsMethod(pymc))
		PyErr_Clear();

	if (sipIsMethod(pymc))
		return TRUE;

	if (cname != NULL)
		PyErr_Format(PyExc_NameError,"%s.%s() is abstract and must be overridden",cname,mname);

	return FALSE;
}


/*
 * Set (or delete) a Python class instance attribute.
 */
int sipSetInstanceAttr(PyObject *sipSelf,PyObject *name,PyObject *value)
{
	if (value != NULL)
		return PyDict_SetItem(((PyInstanceObject *)sipSelf) -> in_dict,name,value);

	return PyDict_DelItem(((PyInstanceObject *)sipSelf) -> in_dict,name);
}


/*
 * Create a new C/C++ object wrapper.
 */
sipThisType *sipCreateThis(PyObject *sipSelf,const void *cppPtr,PyTypeObject *pyType,int initflags,sipExtraType *extraType)
{
	sipThisType *sipThis;
	PyObject *obj;

	/* Check "this" doesn't already exist. */

	if ((obj = PyDict_GetItem(((PyInstanceObject *)sipSelf) -> in_dict,thisName)) != NULL)
	{
		Py_DECREF(obj);

		PyErr_Format(PyExc_TypeError,"Cannot sub-class from more than one wrapped class");

		return NULL;
	}

	if ((sipThis = PyObject_NEW(sipThisType,pyType)) == NULL)
		return NULL;

	/* Save it as an attribute of the instance. */

	if (sipSetInstanceAttr(sipSelf,thisName,(PyObject *)sipThis) < 0)
	{
		Py_DECREF(sipThis);
		return NULL;
	}

	Py_DECREF(sipThis);

	sipThis -> u.cppPtr = cppPtr;
	sipThis -> flags = initflags;
	sipThis -> sipSelf = sipSelf;
	sipThis -> pySigList = NULL;
	sipThis -> xType = extraType;

	/*
	 * If an extra reference has been requested then ownsership is actually
	 * with C++.
	 */

	if (sipIsExtraRef(sipThis))
	{
		sipResetPyOwned(sipThis);

		/* Give the extra reference. */

		Py_INCREF(sipSelf);
	}

	sipOMAddObject(&cppPyMap,cppPtr,sipThis);

	return sipThis;
}


/*
 * Delete a C/C++ object wrapper.
 */
void sipDeleteThis(sipThisType *sipThis)
{
	/* Ignore any errors.  Don't indirect. */

	if (sipThis -> u.cppPtr != NULL)
		sipOMRemoveObject(&cppPyMap,sipThis -> u.cppPtr,sipThis);

	while (sipThis -> pySigList != NULL)
	{
		sipPySig *ps;
		sipPySigRx *psrx;

		/* Take this one out of the list. */

		ps = sipThis -> pySigList;
		sipThis -> pySigList = ps -> next;

		while ((psrx = ps -> rxlist) != NULL)
		{
			ps -> rxlist = psrx -> next;

			if (psrx -> rx.name != NULL)
				sipFree((ANY *)psrx -> rx.name);

			sipFree((ANY *)psrx);
		}

		sipFree((ANY *)ps -> name);
		sipFree((ANY *)ps);
	}

#if PY_VERSION_HEX >= 0x01060000
	PyObject_DEL(sipThis);
#else
	PyMem_DEL(sipThis);
#endif
}


/*
 * Convert a Python class instance to a C/C++ object wrapper.
 */
sipThisType *sipMapSelfToThis(PyObject *sipSelf)
{
	PyObject *sipThis;

	/*
	 * Get the value of "sipThis" from the instance.  The only reason it
	 * might not be found is if the programmer has further sub-classed and
	 * forgot to call the super-class constructor.
	 */

	sipThis = PyObject_GetAttr(sipSelf,thisName);
	Py_XDECREF(sipThis);

	return (sipThisType *)sipThis;
}


/*
 * Do the same as sipNewCppToSelf() but for classes that are influenced by
 * %ConvertToSubClassCode.
 */
PyObject *sipNewCppToSelfSubClass(const void *cppPtr,PyObject *pyClass,
				  int initflags)
{
	return sipNewCppToSelf(cppPtr,convertSubClass(pyClass,cppPtr),initflags);
}


/*
 * Convert a C/C++ pointer to a Python instance.  If the C/C++ pointer is
 * recognised and it is an instance of a sub-class of the expected class then
 * the previously wrapped instance is returned.  Otherwise a new Python
 * instance is created with the expected class.  The instance comes with a
 * reference.
 */
PyObject *sipMapCppToSelf(const void *cppPtr,PyObject *pyClass)
{
	const sipThisType *sipThis;
	PyObject *sipSelf;

	if (cppPtr == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}

	/* See if we have already wrapped it. */

	/* Don't indirect. */

	if ((sipThis = (sipThisType *)sipGetWrapper(cppPtr,pyClass)) != NULL)
	{
		sipSelf = sipThis -> sipSelf;
		Py_INCREF(sipSelf);
	}
	else
		sipSelf = sipNewCppToSelf(cppPtr,pyClass,SIP_SIMPLE);

	return sipSelf;
}


/*
 * Do the same as sipMapCppToSelf() but for classes that are influenced by
 * %ConvertToSubClassCode.
 */
PyObject *sipMapCppToSelfSubClass(const void *cppPtr,PyObject *pyClass)
{
	return sipMapCppToSelf(cppPtr,convertSubClass(pyClass,cppPtr));
}


/*
 * Convert a C/C++ pointer to the object that wraps it.
 */
PyObject *sipGetWrapper(const void *cppPtr,PyObject *pyClass)
{
	return (PyObject *)sipOMFindObject(&cppPyMap,cppPtr,pyClass);
}


/*
 * Get the C/C++ pointer from a wrapper and cast it to the required type.
 */
const void *sipGetCppPtr(sipThisType *w,PyObject *toClass)
{
	const void *ptr;

	if (sipIsAccessFunc(w))
		ptr = (*w -> u.afPtr)();
	else if (sipIsIndirect(w))
		ptr = *((const void **)w -> u.cppPtr);
	else
		ptr = w -> u.cppPtr;

	if (checkPointer(ptr) < 0)
		return NULL;

	return (*w -> xType -> castfunc)(ptr,toClass);
}


/*
 * Check that a pointer is non-NULL.
 */
static int checkPointer(const void *ptr)
{
	if (ptr == NULL)
	{
		PyErr_SetString(PyExc_RuntimeError,"Underlying C/C++ object has been deleted");
		return -1;
	}

	return 0;
}


/*
 * See if a Python object is an instance of a sub-class of a given base class.
 */
int sipIsSubClassInstance(PyObject *inst,PyObject *baseclass)
{
	return (PyInstance_Check(inst) && PyClass_IsSubclass(sipGetClass(inst),baseclass));
}


/*
 * Convert a Python instance of a class to a C/C++ object pointer, checking
 * that the instance's class is derived from a given base class, and also
 * optionally returning the wrapper object.
 */
static const void *convertToCppAndThis(PyObject *sipSelf,PyObject *baseclass,
				       int *iserrp,PyObject **retself)
{
	sipThisType *sipThis;
	const void *ptr;

	if (sipSelf == Py_None)
		return NULL;

	if ((sipThis = sipMapSelfToThis(sipSelf)) == NULL || (ptr = sipGetCppPtr(sipThis,baseclass)) == NULL)
	{
		*iserrp = TRUE;
		return NULL;
	}

	if (retself != NULL)
		*retself = (PyObject *)sipThis;

	return ptr;
}


/*
 * Convert a Python instance of a class to a C/C++ object pointer, checking
 * that the instance's class is derived from a given base class.
 */
const void *sipConvertToCpp(PyObject *sipSelf,PyObject *baseclass,int *iserrp)
{
	return convertToCppAndThis(sipSelf,baseclass,iserrp,NULL);
}


/*
 * Save the components of a Python method.
 */
void sipSaveMethod(sipPyMethod *pm,PyObject *meth)
{
	pm -> mfunc = PyMethod_GET_FUNCTION(meth);
	pm -> mself = PyMethod_GET_SELF(meth);
	pm -> mclass = PyMethod_GET_CLASS(meth);
}


/*
 * Return a weak reference to the given object.
 */
PyObject *sipGetWeakRef(PyObject *obj)
{
#if PY_VERSION_HEX >= 0x02020000
	PyObject *wr;

	if ((wr = PyWeakref_NewRef(obj,NULL)) == NULL)
		PyErr_Clear();

	return wr;
#elif PY_VERSION_HEX >= 0x02010000
	PyObject *args, *wr;

	if (obj == NULL || weakRefMeth == NULL)
		return NULL;

	if ((args = Py_BuildValue("(O)",obj)) == NULL)
		return NULL;

	if ((wr = PyEval_CallObject(weakRefMeth,args)) == NULL)
		PyErr_Clear();

	Py_DECREF(args);

	return wr;
#else
	return NULL;
#endif
}


/*
 * Call a Python method.
 */
PyObject *sipEvalMethod(const sipPyMethod *pm,PyObject *args)
{
	PyObject *meth, *res;

	meth = PyMethod_New(pm -> mfunc,pm -> mself,pm -> mclass);

	if (meth == NULL)
                return NULL;
 
	res = PyEval_CallObject(meth,args);
 
	Py_DECREF(meth); 

	return res;
}


/*
 * Call a hook.
 */
void sipCallHook(char *hookname)
{
	PyObject *dictofmods, *mod, *dict, *hook, *res;
 
	/* Get the dictionary of modules. */
 
	if ((dictofmods = PyImport_GetModuleDict()) == NULL)
		return;
 
	/* Get the __builtin__ module. */
 
	if ((mod = PyDict_GetItemString(dictofmods,"__builtin__")) == NULL)
		return;
 
	/* Get it's dictionary. */
 
	if ((dict = PyModule_GetDict(mod)) == NULL)
		return;
 
	/* Get the function hook. */
 
	if ((hook = PyDict_GetItemString(dict,hookname)) == NULL)
		return;
 
	/* Call the hook and discard any result. */
 
	res = PyObject_CallObject(hook,NULL);
 
	Py_XDECREF(res);
}


/*
 * Call any sub-class convertors for a given class returning a pointer to the
 * sub-class object.
 */
static PyObject *convertSubClass(PyObject *cls,const void *cppPtr)
{
	subClassConvertor *scc;

	if (cppPtr == NULL)
		return NULL;

	for (scc = convertorList; scc != NULL; scc = scc -> next)
		if (PyClass_IsSubclass(cls,scc -> subbase))
		{
			PyObject *subclass;

			if ((subclass = (*scc -> convertor)(cppPtr)) != NULL)
				return subclass;
		}

	/*
	 * We haven't found the exact class, so return the most specific class
	 * that it must be.  This can happen legitimately if the wrapped
	 * library is returning an internal class that is down-cast to a more
	 * generic class.  Also we want this function to be safe if it is
	 * for instances that don't have convertors.
	 */

	return cls;
}


/*
 * Register a sub-class convertor function.
 */
void sipRegisterSubClassConvertor(PyObject *baseclass,PyObject *(*convertor)(const void *))
{
	subClassConvertor *scc;

	if ((scc = sipMalloc(sizeof (subClassConvertor))) != NULL)
	{
		scc -> subbase = baseclass;
		scc -> convertor = convertor;
		scc -> next = convertorList;

		convertorList = scc;
	}
}


/*
 * The bsearch() helper function for searching a sorted string map table.
 */
static int compareStringMapEntry(const void *key,const void *el)
{
	return strcmp((const char *)key,((const sipStringTypeClassMap *)el) -> typeString);
}


/*
 * A convenience function for %ConvertToSubClassCode for types represented as a
 * string.  Returns the Python class object or NULL if the type wasn't
 * recognised.
 */
PyObject *sipMapStringToClass(const char *typeString,
			      const sipStringTypeClassMap *map,int maplen)
{
	sipStringTypeClassMap *me;

	me = (sipStringTypeClassMap *)bsearch((const void *)typeString,
					      (const void *)map,maplen,
					      sizeof (sipStringTypeClassMap),
					      compareStringMapEntry);

        return ((me != NULL) ? *me -> pyClass : NULL);
}


/*
 * The bsearch() helper function for searching a sorted integer map table.
 */
static int compareIntMapEntry(const void *key,const void *el)
{
	if ((int)key > ((const sipIntTypeClassMap *)el) -> typeInt)
		return 1;

	if ((int)key < ((const sipIntTypeClassMap *)el) -> typeInt)
		return -1;

	return 0;
}


/*
 * A convenience function for %ConvertToSubClassCode for types represented as
 * an integer.  Returns the Python class object or NULL if the type wasn't
 * recognised.
 */
PyObject *sipMapIntToClass(int typeInt,const sipIntTypeClassMap *map,
			   int maplen)
{
	sipIntTypeClassMap *me;

	me = (sipIntTypeClassMap *)bsearch((const void *)typeInt,
					   (const void *)map,maplen,
					   sizeof (sipIntTypeClassMap),
					   compareIntMapEntry);

        return ((me != NULL) ? *me -> pyClass : NULL);
}


/*
 * Raise an unknown exception.
 */
void sipRaiseUnknownException(void)
{
	static PyObject *mobj = NULL;

	if (mobj == NULL)
		mobj = PyString_FromString("unknown");

	PyErr_SetObject(PyExc_Exception,mobj);
}


/*
 * Raise an exception implemented as a class.
 */
void sipRaiseClassException(PyObject *cls,const void *ptr)
{
	PyObject *self;

	self = sipNewCppToSelf(ptr,cls,SIP_SIMPLE | SIP_PY_OWNED);

	PyErr_SetObject(cls,self);

	Py_XDECREF(self);
}


/*
 * Raise an exception implemented as a class or sub-class.
 */
void sipRaiseSubClassException(PyObject *cls,const void *ptr)
{
	PyObject *self;

	self = sipNewCppToSelfSubClass(ptr,cls,SIP_SIMPLE | SIP_PY_OWNED);

	PyErr_SetObject(cls,self);

	Py_XDECREF(self);
}


/*
 * The functions, data types and structures to support a Python type to hold a
 * void * that can be converted to an integer.
 */


/* The object data structure. */
typedef struct {
	PyObject_HEAD
	void *voidptr;
} sipVoidPtr;


/*
 * The object destructor.
 */
static void sipVoidPtr_dealloc(sipVoidPtr *self)
{
#if PY_VERSION_HEX >= 0x01060000
	PyObject_DEL(self);
#else
	PyMem_DEL(self);
#endif
}


/*
 * Implement int() for the type.
 */
static PyObject *sipVoidPtr_int(sipVoidPtr *v)
{
	return PyInt_FromLong((long)v -> voidptr);
}


/*
 * Implement asstring() for the type.
 */
static PyObject *sipVoidPtr_asstring(sipVoidPtr *v,PyObject *args)
{
	int nbytes;

	if (!PyArg_ParseTuple(args,"i",&nbytes))
		return NULL;

	return PyString_FromStringAndSize(v -> voidptr,nbytes);
}


/* The methods data structure. */
static PyMethodDef sipVoidPtr_Methods[] = {
	{"asstring", (PyCFunction)sipVoidPtr_asstring, METH_VARARGS, NULL},
	{NULL}
};


/*
 * Implement getattr() for the type.
 */
static PyObject *sipVoidPtr_getattr(PyObject *self,char *name)
{
	return Py_FindMethod(sipVoidPtr_Methods,self,name);
}


/* The number methods data structure. */
PyNumberMethods sipVoidPtr_NumberMethods = {
	0,				/*nb_add*/
	0,				/*nb_subtract*/
	0,				/*nb_multiply*/
	0,				/*nb_divide*/
	0,				/*nb_remainder*/
	0,				/*nb_divmod*/
	0,				/*nb_power*/
	0,				/*nb_negative*/
	0,				/*nb_positive*/
	0,				/*nb_absolute*/
	0,				/*nb_nonzero*/
	0,				/*nb_invert*/
	0,				/*nb_lshift*/
	0,				/*nb_rshift*/
	0,				/*nb_and*/
	0,				/*nb_xor*/
	0,				/*nb_or*/
	0,				/*nb_coerce*/
	(unaryfunc)sipVoidPtr_int,	/*nb_int*/
};


/* The type data structure. */
static PyTypeObject sipVoidPtr_Type = {
	PyObject_HEAD_INIT(NULL)
	0,				/*ob_size*/
	"sip.voidptr",			/*tp_name*/
	sizeof (sipVoidPtr),		/*tp_basicsize*/
	0,				/*tp_itemsize*/
	(destructor)sipVoidPtr_dealloc,	/*tp_dealloc*/
	0,				/*tp_print*/
	sipVoidPtr_getattr,		/*tp_getattr*/
	0,				/*tp_setattr*/
	0,				/*tp_compare*/
	0,				/*tp_repr*/
	&sipVoidPtr_NumberMethods,	/*tp_as_number*/
	0,				/*tp_as_sequence*/
	0,				/*tp_as_mapping*/
	0,				/*tp_hash */
	0,				/*tp_call*/
	0,				/*tp_str*/
	0,				/*tp_getattro*/
	0,				/*tp_setattro*/
	0,				/*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT,		/*tp_flags*/
};


/*
 * A convenience function to convert a C/C++ void pointer from a Python object.
 */
void *sipConvertToVoidPtr(PyObject *obj)
{
	if (obj == NULL)
	{
		PyErr_SetString(PyExc_TypeError,"sip.voidptr is NULL");
		return NULL;
	}

	if (obj == Py_None)
		return NULL;

	if (obj -> ob_type == &sipVoidPtr_Type)
		return ((sipVoidPtr *)obj) -> voidptr;

	return (void *)PyInt_AsLong(obj);
}


/*
 * A convenience function to convert a C/C++ void pointer to a Python object.
 */
PyObject *sipConvertFromVoidPtr(void *val)
{
	sipVoidPtr *self;

	if (val == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}

	self = PyObject_NEW(sipVoidPtr,&sipVoidPtr_Type);

	if (self == NULL)
		return NULL;

	self -> voidptr = val;

	return (PyObject *)self;
}
