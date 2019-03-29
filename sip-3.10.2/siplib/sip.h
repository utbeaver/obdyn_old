/*
 * The SIP library interface without Qt support.
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


#ifndef _SIP_H
#define	_SIP_H

#include <Python.h>


#ifdef __cplusplus
extern "C" {
#endif


/*
 * Define the SIP version number.
 */
#define	SIP_VERSION		0x030a02
#define	SIP_VERSION_STR		"3.10.2"
#define	SIP_BUILD		"205"


/* In case this is Python prior to v1.5.2. */

#if !defined(PyMethod_GET_FUNCTION)
#define	PyMethod_GET_FUNCTION	PyMethod_Function
#endif

#if !defined(PyMethod_GET_SELF)
#define	PyMethod_GET_SELF	PyMethod_Self
#endif

#if !defined(PyMethod_GET_CLASS)
#define	PyMethod_GET_CLASS	PyMethod_Class
#endif

/* In case this is Python v2.3 or later. */

#if !defined(ANY)
#define	ANY	void
#endif


/* If you don't know what this is for, be grateful. */

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#define	SIP_EXPORT	__declspec(dllexport)
#define	SIP_IMPORT	__declspec(dllimport)
#if defined(SIP_MAKE_DLL)
#define	SIP_EXTERN	SIP_EXPORT
#elif !defined(SIP_MAKE_STATIC)
#define	SIP_EXTERN	SIP_IMPORT
#endif
#endif

#if !defined(SIP_EXPORT)
#define	SIP_EXPORT
#endif

#if !defined(SIP_IMPORT)
#define	SIP_IMPORT
#endif

#if !defined(SIP_EXTERN)
#define	SIP_EXTERN
#endif

#if defined(SIP_MAKE_MODULE_DLL)
#define	SIP_MODULE_EXTERN	SIP_EXPORT
#else
#define	SIP_MODULE_EXTERN	SIP_IMPORT
#endif


/*
 * The mask that can be passed to sipTrace().
 */
#define	SIP_TRACE_CATCHERS	0x0001
#define	SIP_TRACE_CTORS		0x0002
#define	SIP_TRACE_DTORS		0x0004
#define	SIP_TRACE_INITS		0x0008
#define	SIP_TRACE_DEALLOCS	0x0010
#define	SIP_TRACE_METHODS	0x0020


/*
 * The information describing an enum value instance to be added to a
 * dictionary.
 */
typedef struct {
	char *evi_name;			/* The enum value name. */
	int evi_val;			/* The enum value value. */
} sipEnumValueInstanceDef;


/*
 * The information describing a class's lazy attributes.
 */
typedef struct {
	int la_nrnlmethods;		/* The number of non-lazy methods. */
	PyMethodDef *la_nlmethods;	/* The table of non-lazy methods. */
	int la_nrmethods;		/* The number of lazy methods. */
	PyMethodDef *la_methods;	/* The table of lazy methods. */
	int la_nrenums;			/* The number of lazy methods. */
	sipEnumValueInstanceDef *la_enums;	/* The table of lazy enums. */
} sipLazyAttrDef;


/*
 * The information describing a class.
 */
typedef struct {
	char *cd_name;			/* The class name. */
	PyCFunction cd_thisctor;	/* The "this" ctor. */
	PyObject **cd_classptr;		/* The address of the Python class. */
	sipLazyAttrDef *cd_attrtab;	/* The lazy attribute table. */
	PyMethodDef **cd_vartab;	/* The variable hierachy table. */
	int cd_scope;			/* The nr. of the scoping class. */
} sipClassDef;


/*
 * The main module structure.
 */
typedef struct _sipModuleDef {
	int md_nrclasses;		/* The number of classes. */
	sipClassDef *md_classes;	/* The table of classes. */
	PyObject *md_name;		/* The module name. */
	PyObject *md_dict;		/* The module dictionary. */
	struct _sipModuleDef *md_next;	/* The next in the list. */
} sipModuleDef;


/*
 * The information describing a license to be added to a dictionary.
 */
typedef struct {
	char *lc_type;			/* The type of license. */
	char *lc_licensee;		/* The licensee. */
	char *lc_timestamp;		/* The timestamp. */
	char *lc_signature;		/* The signature. */
} sipLicenseDef;


/*
 * The information describing a void pointer instance to be added to a
 * dictionary.
 */
typedef struct {
	char *vi_name;			/* The void pointer name. */
	void *vi_val;			/* The void pointer value. */
} sipVoidPtrInstanceDef;


/*
 * The information describing a char instance to be added to a dictionary.
 */
typedef struct {
	char *ci_name;			/* The char name. */
	char ci_val;			/* The char value. */
} sipCharInstanceDef;


/*
 * The information describing a string instance to be added to a dictionary.
 */
typedef struct {
	char *si_name;			/* The string name. */
	char *si_val;			/* The string value. */
} sipStringInstanceDef;


/*
 * The information describing a long instance to be added to a dictionary.
 */
typedef struct {
	char *li_name;			/* The long name. */
	long li_val;			/* The long value. */
} sipLongInstanceDef;


/*
 * The information describing a double instance to be added to a dictionary.
 */
typedef struct {
	char *di_name;			/* The double name. */
	double di_val;			/* The double value. */
} sipDoubleInstanceDef;


/*
 * The information describing a class instance to be added to a dictionary.
 */
typedef struct {
	char *ci_name;			/* The class name. */
	const void *ci_ptr;		/* The actual instance or access func. */
	PyObject *ci_class;		/* The Python class object. */
	int ci_flags;			/* The wrapping flags. */
} sipClassInstanceDef;


/*
 * A Python method's component parts.  This allows us to re-create the method
 * without changing the reference counts of the components.
 */
typedef struct {
	PyObject *mfunc;		/* The function. */
	PyObject *mself;		/* Self if it is a bound method. */
	PyObject *mclass;		/* The class. */
} sipPyMethod;


/*
 * Cache a reference to a Python member function.
 */
typedef struct {
	int mcflags;			/* Method cache flags. */
	sipPyMethod pyMethod;		/* The method. */
} sipMethodCache;


/*
 * Represent a method bound to the actual wrapper.  This helps us keep the
 * public API consistent with SIP v4.
 */
typedef struct {
	const sipPyMethod *method;	/* The method. */
	struct _sipThisType *sipThis;	/* The wrapper it is bound to. */
} sipBoundMethod;


/*
 * Extra type specific information.
 */
typedef struct {
	const void *(*castfunc)(const void *,PyObject *);	/* Cast function. */
	void *proxyfunc;		/* Create proxy function. */
	struct _sipQtSignal *emitTable;	/* Emit table for Qt sigs (complex). */
} sipExtraType;


/*
 * A slot.
 */
typedef struct {
	char *name;			/* Name if a Qt or Python signal. */
	PyObject *pyobj;		/* Signal or Qt slot object. */
	sipPyMethod meth;		/* Python slot method, pyobj is NULL. */
	PyObject *weakSlot;		/* A weak reference to the slot. */
} sipSlot;


/*
 * A receiver of a Python signal.
 */
typedef struct _sipPySigRx {
	sipSlot rx;			/* The receiver. */
	struct _sipPySigRx *next;	/* Next in the list. */
} sipPySigRx;


/*
 * A Python signal.
 */
typedef struct _sipPySig {
	char *name;			/* The name of the signal. */
	sipPySigRx *rxlist;		/* The list of receivers. */
	struct _sipPySig *next;		/* Next in the list. */
} sipPySig;


/*
 * A C/C++ object wrapped as a Python object.
 */
typedef struct _sipThisType {
	PyObject_HEAD
	union {
		const void *cppPtr;	/* C/C++ object pointer. */
		const void *(*afPtr)();	/* Access function. */
	} u;
	int flags;			/* Object flags. */
	PyObject *sipSelf;		/* The Python class instance. */
	sipPySig *pySigList;		/* Python signal list (complex). */
	sipExtraType *xType;		/* Extra type information. */
} sipThisType;


/*
 * Maps the name of a Qt signal to a wrapper function to emit it.
 */
typedef struct _sipQtSignal {
	char *st_name;
	int (*st_emitfunc)(sipThisType *,PyObject *);
} sipQtSignal;


/*
 * Define a mapping between a wrapped type identified by a string and the
 * corresponding Python class.
 */
typedef struct _sipStringTypeClassMap {
	char *typeString;		/* The type as a string. */
	PyObject **pyClass;		/* A pointer to the Python class. */
} sipStringTypeClassMap;


/*
 * Define a mapping between a wrapped type identified by an integer and the
 * corresponding Python class.
 */
typedef struct _sipIntTypeClassMap {
	int typeInt;			/* The type as a string. */
	PyObject **pyClass;		/* A pointer to the Python class. */
} sipIntTypeClassMap;


/*
 * These are public support functions that can be called by handwritten code
 * and will be supported in SIP v4.
 */

extern SIP_EXTERN void sipBadCatcherResult Py_PROTO((const sipBoundMethod *));
extern SIP_EXTERN void sipBadLengthForSlice Py_PROTO((int,int));
extern SIP_EXTERN PyObject *sipBuildResult Py_PROTO((int *,char *,...));
extern SIP_EXTERN PyObject *sipCallMethod Py_PROTO((int *,const sipBoundMethod *,char *,...));
extern SIP_EXTERN PyObject *sipClassName Py_PROTO((PyObject *));
extern SIP_EXTERN int sipConvertFromSequenceIndex Py_PROTO((int,int));
extern SIP_EXTERN int sipConvertFromSliceObject Py_PROTO((PyObject *,int,int *,int *,int *,int *));
extern SIP_EXTERN const void *sipConvertToCpp Py_PROTO((PyObject *,PyObject *,int *));
extern SIP_EXTERN void sipFree Py_PROTO((ANY *));
extern SIP_EXTERN PyObject *sipGetWrapper Py_PROTO((const void *,PyObject *));
extern SIP_EXTERN int sipIsSubClassInstance Py_PROTO((PyObject *,PyObject *));
extern SIP_EXTERN ANY *sipMalloc Py_PROTO((size_t));
extern SIP_EXTERN PyObject *sipMapIntToClass Py_PROTO((int,const sipIntTypeClassMap *,int));
extern SIP_EXTERN PyObject *sipMapStringToClass Py_PROTO((const char *,const sipStringTypeClassMap *,int));
extern SIP_EXTERN int sipParseResult Py_PROTO((int *,const sipBoundMethod *,PyObject *,char *,...));
extern SIP_EXTERN void sipTrace Py_PROTO((unsigned,const char *,...));
extern SIP_EXTERN void sipTransfer Py_PROTO((PyObject *,int));


/*
 * These are private that should only be called by generated code and may not
 * exist in SIP v4.
 */

#define	sipGetSelfFromThis(t)	(((sipThisType *)(t)) -> sipSelf)

extern SIP_EXTERN PyObject *sipMapCppToSelf Py_PROTO((const void *,PyObject *));
extern SIP_EXTERN PyObject *sipMapCppToSelfSubClass Py_PROTO((const void *,PyObject *));
extern SIP_EXTERN void *sipConvertToVoidPtr Py_PROTO((PyObject *));
extern SIP_EXTERN PyObject *sipConvertFromVoidPtr Py_PROTO((void *));
extern SIP_EXTERN PyObject *sipConvertFromBool Py_PROTO((int));
extern SIP_EXTERN PyObject *sipEvalMethod Py_PROTO((const sipPyMethod *,PyObject *));
extern SIP_EXTERN void sipBadVirtualResultType Py_PROTO((char *,char *));
extern SIP_EXTERN void sipBadSetType Py_PROTO((char *,char *));
extern SIP_EXTERN void sipTransferSelf Py_PROTO((PyObject *,int));


#define	SIP_PY_OWNED	0x01		/* Owned by Python. */
#define	SIP_SIMPLE	0x02		/* If the instance is simple. */
#define	SIP_INDIRECT	0x04		/* If there is a level of indirection. */
#define	SIP_ACCFUNC	0x08		/* If there is an access function. */
#define	SIP_XTRA_REF	0x10		/* If C++ has an extra reference. */

#define	sipIsPyOwned(w)		((w) -> flags & SIP_PY_OWNED)
#define	sipSetPyOwned(w)	((w) -> flags |= SIP_PY_OWNED)
#define	sipResetPyOwned(w)	((w) -> flags &= ~SIP_PY_OWNED)
#define	sipIsSimple(w)		((w) -> flags & SIP_SIMPLE)
#define	sipSetSimple(w)		((w) -> flags |= SIP_SIMPLE)
#define	sipResetSimple(w)	((w) -> flags &= ~SIP_SIMPLE)
#define	sipIsIndirect(w)	((w) -> flags & SIP_INDIRECT)
#define	sipSetIndirect(w)	((w) -> flags |= SIP_INDIRECT)
#define	sipResetIndirect(w)	((w) -> flags &= ~SIP_INDIRECT)
#define	sipIsAccessFunc(w)	((w) -> flags & SIP_ACCFUNC)
#define	sipSetIsAccessFunc(w)	((w) -> flags |= SIP_ACCFUNC)
#define	sipResetIsAccessFunc(w)	((w) -> flags &= ~SIP_ACCFUNC)
#define	sipIsExtraRef(w)	((w) -> flags & SIP_XTRA_REF)
#define	sipSetIsExtraRef(w)	((w) -> flags |= SIP_XTRA_REF)
#define	sipResetIsExtraRef(w)	((w) -> flags &= ~SIP_XTRA_REF)

#define	SIP_MC_FOUND	0x01		/* If we have looked for the method. */
#define	SIP_MC_ISMETH	0x02		/* If we looked and there was one. */

#define	sipFoundMethod(m)	((m) -> mcflags & SIP_MC_FOUND)
#define	sipSetFoundMethod(m)	((m) -> mcflags |= SIP_MC_FOUND)
#define	sipIsMethod(m)		((m) -> mcflags & SIP_MC_ISMETH)
#define	sipSetIsMethod(m)	((m) -> mcflags |= SIP_MC_ISMETH)

extern SIP_EXTERN char *sipWrapperTypeName;

extern SIP_EXTERN void initlibsip Py_PROTO((void));
extern SIP_EXTERN int sipRegisterModule Py_PROTO((sipModuleDef *,PyObject *));
extern SIP_EXTERN int sipRegisterClasses Py_PROTO((sipModuleDef *,int));
extern SIP_EXTERN int sipParseArgs Py_PROTO((int *,PyObject *,char *,...));
extern SIP_EXTERN void sipSaveMethod Py_PROTO((sipPyMethod *,PyObject *));
extern SIP_EXTERN void sipCommonCtor Py_PROTO((sipMethodCache *,int));
extern SIP_EXTERN void sipCommonDtor Py_PROTO((sipThisType *));
extern SIP_EXTERN void sipDeleteThis Py_PROTO((sipThisType *));
extern SIP_EXTERN void sipIsAbstract Py_PROTO((char *,char *));
extern SIP_EXTERN void sipNoCtor Py_PROTO((int,char *));
extern SIP_EXTERN void sipNoFunction Py_PROTO((int,char *));
extern SIP_EXTERN void sipNoMethod Py_PROTO((int,char *,char *));
extern SIP_EXTERN int sipIsPyMethod Py_PROTO((sipMethodCache *,sipThisType *,char *,char *));
extern SIP_EXTERN int sipSetInstanceAttr Py_PROTO((PyObject *,PyObject *,PyObject *));
extern SIP_EXTERN int sipAddLicense Py_PROTO((PyObject *,sipLicenseDef *));
extern SIP_EXTERN int sipAddVoidPtrInstances Py_PROTO((PyObject *,sipVoidPtrInstanceDef *));
extern SIP_EXTERN int sipAddCharInstances Py_PROTO((PyObject *,sipCharInstanceDef *));
extern SIP_EXTERN int sipAddStringInstances Py_PROTO((PyObject *,sipStringInstanceDef *));
extern SIP_EXTERN int sipAddLongInstances Py_PROTO((PyObject *,sipLongInstanceDef *));
extern SIP_EXTERN int sipAddDoubleInstances Py_PROTO((PyObject *,sipDoubleInstanceDef *));
extern SIP_EXTERN int sipAddEnumInstances Py_PROTO((PyObject *,sipEnumValueInstanceDef *));
extern SIP_EXTERN int sipAddClassInstances Py_PROTO((PyObject *,sipClassInstanceDef *));
extern SIP_EXTERN int sipAddFunctions Py_PROTO((PyObject *,PyMethodDef *));
extern SIP_EXTERN sipThisType *sipCreateThis Py_PROTO((PyObject *,const void *,PyTypeObject *,int,sipExtraType *));
extern SIP_EXTERN sipThisType *sipMapSelfToThis Py_PROTO((PyObject *));
extern SIP_EXTERN PyObject *sipNewCppToSelf Py_PROTO((const void *,PyObject *,int));
extern SIP_EXTERN PyObject *sipNewCppToSelfSubClass Py_PROTO((const void *,PyObject *,int));
extern SIP_EXTERN PyObject *sipCallCtor Py_PROTO((sipModuleDef *,PyObject *));
extern SIP_EXTERN PyObject *sipGetVar Py_PROTO((sipModuleDef *,PyObject *));
extern SIP_EXTERN PyObject *sipSetVar Py_PROTO((sipModuleDef *,PyObject *));
extern SIP_EXTERN PyObject *sipGetClass Py_PROTO((PyObject *));
extern SIP_EXTERN PyObject *sipGetWeakRef Py_PROTO((PyObject *));
extern SIP_EXTERN void sipRegisterSubClassConvertor Py_PROTO((PyObject *,PyObject *(*)(const void *)));
extern SIP_EXTERN const void *sipGetPending Py_PROTO((int *));
extern SIP_EXTERN void sipBadClass Py_PROTO((char *));
extern SIP_EXTERN const void *sipGetComplexCppPtr Py_PROTO((sipThisType *));
extern SIP_EXTERN const void *sipGetCppPtr Py_PROTO((sipThisType *,PyObject *));
extern SIP_EXTERN void sipRaiseClassException Py_PROTO((PyObject *,const void *));
extern SIP_EXTERN void sipRaiseSubClassException Py_PROTO((PyObject *,const void *));
extern SIP_EXTERN void sipRaiseUnknownException Py_PROTO((void));
extern SIP_EXTERN void sipCallHook Py_PROTO((char *));

#ifdef WITH_THREAD
extern SIP_EXTERN int sipIsPyMethodThread Py_PROTO((sipMethodCache *,sipThisType *,char *,char *));
extern SIP_EXTERN void sipEndThread Py_PROTO((void));
extern SIP_EXTERN void sipBlockThreads Py_PROTO((void));

#define	SIP_BLOCK_THREADS	sipBlockThreads();
#define	SIP_UNBLOCK_THREADS	PyEval_SaveThread();
#else
#define	SIP_BLOCK_THREADS	0;
#define	SIP_UNBLOCK_THREADS	0;
#endif

#ifdef __cplusplus
}
#endif

#endif
