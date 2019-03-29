/*
 * The main header file for SIP.
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


#ifndef SIP_H
#define SIP_H

#include <stdio.h>
#include <sys/types.h>


#ifdef TRUE
#undef TRUE
#endif

#ifdef FALSE
#undef FALSE
#endif

#define	TRUE	1
#define	FALSE	0


#define	DEFAULT_OFILE_EXT	".o"	/* Default object file extension. */

#define	MAX_NR_ARGS		20	/* Max. nr. args. to a function. */


/* For convenience. */

#define	classBaseName(cd)	((cd) -> iff -> name -> text)
#define	classFQName(cd)		((cd) -> iff -> fqname)


/* Handle section flags. */

#define	SECT_IS_PUBLIC		0x0001	/* It is public. */
#define	SECT_IS_PROT		0x0002	/* It is protected. */
#define	SECT_IS_PRIVATE		0x0004	/* It is private. */
#define	SECT_IS_SLOT		0x0008	/* It is a slot. */
#define	SECT_IS_SIGNAL		0x0010	/* It is a signal. */


/* Handle class flags. */

#define	CLASS_HAS_ENUMS		0x00010000	/* It has enums. */
#define	CLASS_HAS_SIGSLOTS	0x00020000	/* It has signals or slots. */
#define	CLASS_CANNOT_CREATE	0x00040000	/* It can't be created by Python. */
#define	CLASS_IS_COMPLEX	0x00080000	/* It is complex. */
#define	CLASS_IS_OPAQUE		0x00100000	/* It is opaque. */
#define	CLASS_CREATES_THREAD	0x00200000	/* It creates a thread. */

#define	hasEnums(cd)		((cd) -> classflags & CLASS_HAS_ENUMS)
#define	setHasEnums(cd)		((cd) -> classflags |= CLASS_HAS_ENUMS)
#define	hasSigSlots(cd)		((cd) -> classflags & CLASS_HAS_SIGSLOTS)
#define	setHasSigSlots(cd)	((cd) -> classflags |= CLASS_HAS_SIGSLOTS)
#define	cannotCreate(cd)	((cd) -> classflags & CLASS_CANNOT_CREATE)
#define	setCannotCreate(cd)	((cd) -> classflags |= CLASS_CANNOT_CREATE)
#define	resetCannotCreate(cd)	((cd) -> classflags &= ~CLASS_CANNOT_CREATE)
#define	isComplex(cd)		((cd) -> classflags & CLASS_IS_COMPLEX)
#define	setIsComplex(cd)	((cd) -> classflags |= CLASS_IS_COMPLEX)
#define	resetIsComplex(cd)	((cd) -> classflags &= ~CLASS_IS_COMPLEX)
#define	isOpaque(cd)		((cd) -> classflags & CLASS_IS_OPAQUE)
#define	setIsOpaque(cd)		((cd) -> classflags |= CLASS_IS_OPAQUE)
#define	createsThread(cd)	((cd) -> classflags & CLASS_CREATES_THREAD)
#define	setCreatesThread(cd)	((cd) -> classflags |= CLASS_CREATES_THREAD)

#define	isPublicDtor(cd)	((cd) -> classflags & SECT_IS_PUBLIC)
#define	setIsPublicDtor(cd)	((cd) -> classflags |= SECT_IS_PUBLIC)
#define	isProtectedDtor(cd)	((cd) -> classflags & SECT_IS_PROT)
#define	isPrivateDtor(cd)	((cd) -> classflags & SECT_IS_PRIVATE)

#define	isDtor(cd)		((cd) -> classflags & (SECT_IS_PUBLIC | SECT_IS_PROT | SECT_IS_PRIVATE))


/* Handle interface file flags. */

#define	IFACEFILE_IS_USED	0x01	/* It is used. */

#define	isUsed(i)		((i) -> ifacefileflags & IFACEFILE_IS_USED)
#define	setIsUsed(i)		((i) -> ifacefileflags |= IFACEFILE_IS_USED)


/* Handle ctor flags. */

#define	CTOR_IS_BLOCKING	0x00010000	/* It is a blocking ctor. */

#define	isPublicCtor(c)		((c) -> ctorflags & SECT_IS_PUBLIC)
#define	setIsPublicCtor(c)	((c) -> ctorflags |= SECT_IS_PUBLIC)
#define	isProtectedCtor(c)	((c) -> ctorflags & SECT_IS_PROT)
#define	setIsProtectedCtor(c)	((c) -> ctorflags |= SECT_IS_PROT)
#define	isPrivateCtor(c)	((c) -> ctorflags & SECT_IS_PRIVATE)
#define	setIsPrivateCtor(c)	((c) -> ctorflags |= SECT_IS_PRIVATE)

#define	isBlockingCtor(c)	((c) -> ctorflags & CTOR_IS_BLOCKING)
#define	setIsBlockingCtor(c)	((c) -> ctorflags |= CTOR_IS_BLOCKING)


/* Handle enum flags. */

#define	isProtectedEnum(e)	((e) -> enumflags & SECT_IS_PROT)


/* Handle hierarchy flags. */

#define	HIER_IS_DUPLICATE	0x0001		/* It is a super class duplicate. */
#define	HIER_HAS_DUPLICATE	0x0002		/* It has a super class duplicate. */

#define	isDuplicateSuper(m)	((m) -> mroflags & HIER_IS_DUPLICATE)
#define	setIsDuplicateSuper(m)	((m) -> mroflags |= HIER_IS_DUPLICATE)
#define	hasDuplicateSuper(m)	((m) -> mroflags & HIER_HAS_DUPLICATE)
#define	setHasDuplicateSuper(m)	((m) -> mroflags |= HIER_HAS_DUPLICATE)


/* Handle overload flags. */

#define	OVER_IS_VIRTUAL		0x00010000	/* It is virtual. */
#define	OVER_IS_ABSTRACT	0x00020000	/* It is abstract. */
#define	OVER_IS_CONST		0x00040000	/* It is a const function. */
#define	OVER_IS_STATIC		0x00080000	/* It is a static function. */
#define	OVER_IS_AUTOGEN		0x00100000	/* It is auto-generated. */
#define	OVER_IS_NEW_THREAD	0x00200000	/* It is in a new thread. */
#define	OVER_IS_FACTORY		0x00400000	/* It is a factory method. */
#define	OVER_XFERRED_BACK	0x00800000	/* Ownership is transferred back. */

#define	isPublic(o)		((o) -> overflags & SECT_IS_PUBLIC)
#define	setIsPublic(o)		((o) -> overflags |= SECT_IS_PUBLIC)
#define	isProtected(o)		((o) -> overflags & SECT_IS_PROT)
#define	setIsProtected(o)	((o) -> overflags |= SECT_IS_PROT)
#define	isPrivate(o)		((o) -> overflags & SECT_IS_PRIVATE)
#define	setIsPrivate(o)		((o) -> overflags |= SECT_IS_PRIVATE)
#define	isSlot(o)		((o) -> overflags & SECT_IS_SLOT)
#define	setIsSlot(o)		((o) -> overflags |= SECT_IS_SLOT)
#define	isSignal(o)		((o) -> overflags & SECT_IS_SIGNAL)
#define	setIsSignal(o)		((o) -> overflags |= SECT_IS_SIGNAL)

#define	isVirtual(o)		((o) -> overflags & OVER_IS_VIRTUAL)
#define	setIsVirtual(o)		((o) -> overflags |= OVER_IS_VIRTUAL)
#define	isAbstract(o)		((o) -> overflags & OVER_IS_ABSTRACT)
#define	setIsAbstract(o)	((o) -> overflags |= OVER_IS_ABSTRACT)
#define	isConst(o)		((o) -> overflags & OVER_IS_CONST)
#define	setIsConst(o)		((o) -> overflags |= OVER_IS_CONST)
#define	isStatic(o)		((o) -> overflags & OVER_IS_STATIC)
#define	setIsStatic(o)		((o) -> overflags |= OVER_IS_STATIC)
#define	isAutoGen(o)		((o) -> overflags & OVER_IS_AUTOGEN)
#define	setIsAutoGen(o)		((o) -> overflags |= OVER_IS_AUTOGEN)
#define	resetIsAutoGen(o)	((o) -> overflags &= ~OVER_IS_AUTOGEN)
#define	isNewThread(o)		((o) -> overflags & OVER_IS_NEW_THREAD)
#define	setIsNewThread(o)	((o) -> overflags |= OVER_IS_NEW_THREAD)
#define	isFactory(o)		((o) -> overflags & OVER_IS_FACTORY)
#define	setIsFactory(o)		((o) -> overflags |= OVER_IS_FACTORY)
#define	isResultTransferredBack(o)	((o) -> overflags & OVER_XFERRED_BACK)
#define	setIsResultTransferredBack(o)	((o) -> overflags |= OVER_XFERRED_BACK)


/* Handle variable flags. */

#define	VAR_IS_STATIC		0x01	/* It is a static variable. */

#define	isStaticVar(v)		((v) -> varflags & VAR_IS_STATIC)
#define	setIsStaticVar(v)	((v) -> varflags |= VAR_IS_STATIC)


/* Handle argument flags. */

#define	ARG_IS_REF		0x0001	/* It is a reference. */
#define	ARG_IS_CONST		0x0002	/* It is a const. */
#define	ARG_XFERRED		0x0004	/* Ownership is transferred. */
#define	ARG_THIS_XFERRED	0x0008	/* Ownership of this is transferred. */
#define	ARG_XFERRED_BACK	0x0010	/* Ownership is transferred back. */
#define	ARG_ARRAY		0x0020	/* Used as an array. */
#define	ARG_ARRAY_SIZE		0x0040	/* Used as an array size. */
#define	ARG_ALLOW_NONE		0x0080	/* Allow None as a value. */
#define	ARG_GET_WRAPPER		0x0100	/* Get the wrapper object. */
#define	ARG_IN			0x0200	/* It passes an argument. */
#define	ARG_OUT			0x0400	/* It returns a result. */

#define	isReference(a)		((a) -> argflags & ARG_IS_REF)
#define	setIsReference(a)	((a) -> argflags |= ARG_IS_REF)
#define	resetIsReference(a)	((a) -> argflags &= ~ARG_IS_REF)
#define	isConstArg(a)		((a) -> argflags & ARG_IS_CONST)
#define	setIsConstArg(a)	((a) -> argflags |= ARG_IS_CONST)
#define	resetIsConstArg(a)	((a) -> argflags &= ~ARG_IS_CONST)
#define	isTransferred(a)	((a) -> argflags & ARG_XFERRED)
#define	setIsTransferred(a)	((a) -> argflags |= ARG_XFERRED)
#define	isThisTransferred(a)	((a) -> argflags & ARG_THIS_XFERRED)
#define	setIsThisTransferred(a)	((a) -> argflags |= ARG_THIS_XFERRED)
#define	isTransferredBack(a)	((a) -> argflags & ARG_XFERRED_BACK)
#define	setIsTransferredBack(a)	((a) -> argflags |= ARG_XFERRED_BACK)
#define	isArray(a)		((a) -> argflags & ARG_ARRAY)
#define	setArray(a)		((a) -> argflags |= ARG_ARRAY)
#define	isArraySize(a)		((a) -> argflags & ARG_ARRAY_SIZE)
#define	setArraySize(a)		((a) -> argflags |= ARG_ARRAY_SIZE)
#define	isAllowNone(a)		((a) -> argflags & ARG_ALLOW_NONE)
#define	setAllowNone(a)		((a) -> argflags |= ARG_ALLOW_NONE)
#define	isGetWrapper(a)		((a) -> argflags & ARG_GET_WRAPPER)
#define	setGetWrapper(a)	((a) -> argflags |= ARG_GET_WRAPPER)
#define	isInArg(a)		((a) -> argflags & ARG_IN)
#define	setIsInArg(a)		((a) -> argflags |= ARG_IN)
#define	isOutArg(a)		((a) -> argflags & ARG_OUT)
#define	setIsOutArg(a)		((a) -> argflags |= ARG_OUT)


/* Slot types. */

typedef enum {
	str_slot,
	unicode_slot,
	int_slot,
	len_slot,
	contains_slot,
	add_slot,
	sub_slot,
	mul_slot,
	div_slot,
	mod_slot,
	and_slot,
	or_slot,
	xor_slot,
	lshift_slot,
	rshift_slot,
	iadd_slot,
	isub_slot,
	imul_slot,
	idiv_slot,
	imod_slot,
	iand_slot,
	ior_slot,
	ixor_slot,
	ilshift_slot,
	irshift_slot,
	invert_slot,
	call_slot,
	getitem_slot,
	setitem_slot,
	delitem_slot,
	lt_slot,
	le_slot,
	eq_slot,
	ne_slot,
	gt_slot,
	ge_slot,
	cmp_slot,
	nonzero_slot,
	neg_slot,
	repr_slot,
	no_slot
} slotType;


/* Argument types. */

typedef enum {
	no_type,
	defined_type,
	class_type,
	struct_type,
	voidptr_type,
	enum_type,
	template_type,
	signal_type,
	slot_type,
	rxcon_type,
	rxdis_type,
	slotcon_type,
	slotdis_type,
	ustring_type,
	string_type,
	short_type,
	ushort_type,
	cint_type,
	int_type,
	uint_type,
	long_type,
	ulong_type,
	float_type,
	cfloat_type,
	double_type,
	cdouble_type,
	bool_type,
	mapped_type,
	pyobject_type,
	pytuple_type,
	pylist_type,
	pydict_type,
	pycallable_type,
	pyslice_type,
	qobject_type
} argType;


/* Value types. */

typedef enum {
	qchar_value,
	string_value,
	numeric_value,
	real_value,
	scoped_value,
	fcall_value
} valueType;


/* Version types. */

typedef enum {
	time_qualifier,
	platform_qualifier,
	feature_qualifier
} qualType;


/* Interface file types. */

typedef enum {
	mappedtype_iface,
	namespace_iface,
	class_iface
} ifaceFileType;


/* A software license. */

typedef struct {
	char			*type;		/* The license type. */
	char			*licensee;	/* The licensee. */
	char			*timestamp;	/* The timestamp. */
	char			*sig;		/* The signature. */
} licenseDef;


/* An exposed function. */

typedef struct _expFuncDef {
	char			*name;		/* The function name. */
	struct _expFuncDef	*next;		/* Next in the list. */
} expFuncDef;


/* A version qualifier. */

typedef struct _qualDef {
	char			*name;		/* The qualifier name. */
	qualType		qtype;		/* The qualifier type. */
	struct _moduleDef	*module;	/* The defining module. */
	int			order;		/* Order if it is a time. */
	struct _qualDef	*next;			/* Next in the list. */
} qualDef;


/* A scoped name. */

typedef struct _scopedNameDef {
	char			*name;		/* The name. */
	struct _scopedNameDef	*next;		/* Next in the scope list. */
} scopedNameDef;


/* A name. */

typedef struct _nameDef {
	char			*text;		/* The text of the name. */
	struct _moduleDef	*module;	/* The defining module. */
	struct _nameDef		*next;		/* Next in the list. */
} nameDef;


/* A module definition. */

typedef struct _moduleDef {
	char			*name;		/* The module name. */
	char			*file;		/* The filename. */
	qualDef			*qualifiers;	/* The list of qualifiers. */
	int			nrTimeSlots;	/* The nr. of time slots. */
	licenseDef		*license;	/* The software license. */
	struct _moduleDef	*next;		/* Next in the list. */
} moduleDef;


/* A literal code block. */

typedef struct _codeBlock {
	char			*frag;		/* The code itself. */
	char			*filename;	/* The original file. */
	int			linenr;		/* The line in the file. */
	struct _codeBlock	*next;		/* Next in the list. */
} codeBlock;


/* The arguments to a throw specifier. */

typedef struct _throwArgs {
	int			nrArgs;		/* The number of arguments. */
	struct _classDef	*args[MAX_NR_ARGS];	/* The arguments. */
} throwArgs;


/* A Makefile template definition. */

typedef struct _mkTemplateDef {
	char			*name;		/* The template name. */
	char			*objext;	/* The object file extension. */
	codeBlock		*templ;		/* The template itself. */
	struct _mkTemplateDef	*next;		/* Next in the list. */
} mkTemplateDef;


/* A function call. */

typedef struct _fcallDef {
	scopedNameDef		*name;		/* The name. */
	int			nrArgs;		/* The number of arguments. */
	struct _valueDef	*args[MAX_NR_ARGS];	/* The arguments. */
} fcallDef;


/* A value. */

typedef struct _valueDef {
	valueType		vtype;		/* The type. */
	char			vunop;		/* Any unary operator. */
	char			vbinop;		/* Any binary operator. */
	union {
		char		vqchar;		/* Quoted character value. */
		long		vnum;		/* Numeric value. */
		double		vreal;		/* Real value. */
		char		*vstr;		/* String value. */
		scopedNameDef	*vscp;		/* Scoped value. */
		fcallDef	*fcd;		/* Function call. */
	} u;
	struct _valueDef	*next;		/* Next in the expression. */
} valueDef;


/* A member function argument (or result). */

typedef struct {
	argType			atype;		/* The type. */
	int			argflags;	/* The argument flags. */
	int			nrderefs;	/* Nr. of dereferences. */
	valueDef		*defval;	/* The default value. */
	union {
		struct _funcArgs	*sa;	/* If it is a slot. */
		struct _templateDef	*td;	/* If it is a template. */
		struct _scopedNameDef	*snd;	/* If it is a defined type. */
		struct _classDef	*cd;	/* If it is a class. */
		struct _enumDef		*ed;	/* If it is an enum. */
		struct _scopedNameDef	*sname;	/* If it is a struct. */
		struct _mappedTypeDef	*mtd;	/* If it is a mapped type. */
	} u;
} argDef;



/* An interface file definition. */

typedef struct _ifaceFileDef {
	nameDef			*name;		/* The name. */
	ifaceFileType		type;		/* Interface file type. */
	scopedNameDef		*fqname;	/* The fully qualified name. */
	moduleDef		*module;	/* The owning module. */
	int			ifacefileflags;	/* The interface file flags. */
	struct _ifaceFileList	*used;		/* Interface files used. */
	struct _ifaceFileDef	*next;		/* Next in the list. */
} ifaceFileDef;


/* An entry in a linked interface file list. */

typedef struct _ifaceFileList {
	ifaceFileDef		*iff;		/* The interface file itself. */
	struct _ifaceFileList	*next;		/* Next in the list. */
} ifaceFileList;


/* A mapped type. */

typedef struct _mappedTypeDef {
	argDef			type;		/* The type being mapped. */
	ifaceFileDef		*iff;		/* The interface file. */
	codeBlock		*hdrcode;	/* Header code. */
	codeBlock		*convfromcode;	/* Convert from C++ code. */
	codeBlock		*convtocode;	/* Convert to C++ code. */
	struct _mappedTypeDef	*next;		/* Next in the list. */
} mappedTypeDef;


/* The arguments to a member function. */

typedef struct _funcArgs {
	int			nrArgs;		/* The number of arguments. */
	argDef			args[MAX_NR_ARGS];	/* The arguments. */
} funcArgs;


/* A function signature. */

typedef struct _signatureDef {
	argDef			result;         /* The result. */
	funcArgs		args;		/* The arguments. */
} signatureDef;


/* A template type. */

typedef struct _templateDef {
	scopedNameDef		*fqname;	/* The name. */
	funcArgs		types;		/* The types. */
} templateDef;


/* A list of function arguments. */

typedef struct _funcArgsList {
	funcArgs		*fa;		/* The arguments. */
	struct _funcArgsList	*next;		/* Next in the list. */
} funcArgsList;


/* A typedef definition. */

typedef struct _typedefDef {
	scopedNameDef		*fqname;	/* The fully qualified name. */
	struct _classDef	*ecd;		/* The enclosing class. */
	argDef			type;		/* The actual type. */
	struct _typedefDef	*next;		/* Next in the list. */
} typedefDef;


/* A variable definition. */

typedef struct _varDef {
	nameDef			*name;		/* The variable name. */
	scopedNameDef		*fqname;	/* The fully qualified name. */
	struct _classDef	*ecd;		/* The enclosing class. */
	moduleDef		*module;	/* The owning module. */
	int			varflags;	/* The variable flags. */
	argDef			type;		/* The actual type. */
	codeBlock		*accessfunc;	/* The access function. */
	struct _varDef		*next;		/* Next in the list. */
} varDef;


/* An overloaded member function definition. */

typedef struct _overDef {
	char			*cppname;	/* The C++ name. */
	int			overflags;	/* The overload flags. */
	struct _memberDef	*common;	/* Common parts. */
	argDef			*result;	/* The result type. */
	funcArgs		args;		/* The arguments. */
	signatureDef		*cppsig;	/* The C++ signature. */
	throwArgs		*exceptions;	/* The exceptions. */
	codeBlock		*methodcode;	/* Method code. */
	codeBlock		*membercode;	/* Member code. */
	codeBlock		*ovirtcode;	/* Old virtual handler code. */
	codeBlock		*virtcode;	/* Virtual catcher code. */
	char			*prehook;	/* The pre-hook name. */
	char			*posthook;	/* The post-hook name. */
	struct _overDef		*next;		/* Next in the list. */
} overDef;


/* An overloaded constructor definition. */

typedef struct _ctorDef {
	int			ctorflags;	/* The ctor flags. */
	funcArgs		args;		/* The arguments. */
	signatureDef		*cppsig;	/* The C++ signature. */
	throwArgs		*exceptions;	/* The exceptions. */
	codeBlock		*methodcode;	/* Method code. */
	codeBlock		*membercode;	/* Member code. */
	char			*prehook;	/* The pre-hook name. */
	char			*posthook;	/* The post-hook name. */
	struct _ctorDef		*next;		/* Next in the list. */
} ctorDef;


/* An enumerated type value definition. */

typedef struct _enumValueDef {
	nameDef			*name;		/* The value name. */
	struct _enumDef		*ed;		/* The enclosing enum. */
	struct _enumValueDef	*next;		/* Next in the list. */
} enumValueDef;


/* An enumerated type definition. */

typedef struct _enumDef {
	int			enumflags;	/* The enum flags. */
	scopedNameDef		*fqname;	/* The name (may be NULL). */
	struct _classDef	*ecd;		/* The enclosing class. */
	moduleDef		*module;	/* The owning module. */
	enumValueDef		*values;	/* The list of values. */
	struct _enumDef		*next;		/* Next in the list. */
} enumDef;


/* An member function definition. */

typedef struct _memberDef {
	nameDef			*pyname;	/* The Python name. */
	slotType		slot;		/* The slot type. */
	moduleDef		*module;	/* The owning module. */
	struct _memberDef	*next;		/* Next in the list. */
} memberDef;


/* A list of visible member functions. */

typedef struct _visibleList {
	memberDef		*m;		/* The member definition. */
	struct _classDef	*cd;		/* The class. */
	struct _visibleList	*next;		/* Next in the list. */
} visibleList;


/* An entry in a linked class list. */

typedef struct _classList {
	struct _classDef	*cd;		/* The class itself. */
	struct _classList	*next;		/* Next in the list. */
} classList;


/* A virtual overload definition. */

typedef struct _virtOverDef {
	overDef			*o;		/* The overload. */
	struct _classDef	*nearc;		/* The nearest class. */
	struct _classDef	*farc;		/* The farthest class. */
	struct _virtOverDef	*next;		/* Next in the list. */
} virtOverDef;


/*
 * A node in the tree of classes used to determine the order and indentation
 * needed by the Python class wrappers.
 */

typedef struct _nodeDef {
	int			ordered;	/* Set if correctly ordered. */
	struct _classDef	*cd;		/* The class. */
	struct _nodeDef		*parent;	/* The parent. */
	struct _nodeDef		*child;		/* The first child. */
	struct _nodeDef		*next;		/* The next sibling. */
} nodeDef;


/*
 * A class that appears in a class's hierarchy.
 */

typedef struct _mroDef {
	struct _classDef	*cd;		/* The class. */
	int			mroflags;	/* The hierarchy flags. */
	struct _mroDef		*next;		/* The next in the list. */
} mroDef;


/* A class definition. */

typedef struct _classDef {
	int			classflags;	/* The class flags. */
	int			classnr;	/* The class number. */
	ifaceFileDef		*iff;		/* The interface file. */
	struct _classDef	*ecd;		/* The enclosing scope. */
	nodeDef			*node;		/* Position in namespace tree. */
	classList		*supers;	/* The parent classes. */
	mroDef			*mro;		/* The super-class hierarchy. */
	ctorDef			*ctors;		/* The constructors. */
	ctorDef			*defctor;	/* The default ctor. */
	codeBlock		*dealloccode;	/* Handwritten dealloc code. */
	codeBlock		*dtorcode;	/* Handwritten dtor code. */
	throwArgs		*dtorexceptions;	/* The dtor exceptions. */
	memberDef		*members;	/* The member functions. */
	overDef			*overs;		/* The overloads. */
	virtOverDef		*vmembers;	/* The virtual members. */
	visibleList		*visible;	/* The visible members. */
	codeBlock		*cppcode;	/* Class C++ code. */
	codeBlock		*hdrcode;	/* Class header code. */
	codeBlock		*convtosubcode;	/* Convert to sub C++ code. */
	struct _classDef	*subbase;	/* Sub-class base class. */
	codeBlock		*convtocode;	/* Convert to C++ code. */
	struct _classDef	*next;		/* Next in the list. */
} classDef;


/* The parse tree corresponding to the specification file. */

typedef struct {
	moduleDef		*moduleList;	/* The module list. */
	moduleDef		*module;	/* This module. */
	char			*cppmname;	/* The C++ module name. */
	nameDef			*namecache;	/* The name cache. */
	ifaceFileDef		*ifacefiles;	/* The list of interface files. */
	classDef		*classes;	/* The list of classes. */
	mappedTypeDef		*mappedtypes;	/* The mapped types. */
	ifaceFileList		*used;		/* Interface files used. */
	int			qobjclass;	/* QObject class, -1 if none. */
	expFuncDef		*exposed;	/* Exposed functions. */
	enumDef			*enums;		/* List of enums. */
	varDef			*vars;		/* List of variables. */
	memberDef		*othfuncs;	/* List of other functions. */
	overDef			*overs;		/* Global overloads. */
	typedefDef		*typedefs;	/* List of typedefs. */
	codeBlock		*copying;	/* Software license. */
	codeBlock		*hdrcode;	/* Header code. */
	codeBlock		*exphdrcode;	/* Exported header code. */
	codeBlock		*cppcode;	/* Global C++ code. */
	codeBlock		*prepycode;	/* Global pre-Python code. */
	codeBlock		*pycode;	/* Global Python code. */
	codeBlock		*docs;		/* Documentation. */
	codeBlock		*postinitcode;	/* Post-initialisation code. */
	mkTemplateDef		*mktemplates;	/* The Makefile templates. */
	funcArgsList		*sigargs;	/* The list of signal arguments. */
	int			sigslots;	/* Set if signals or slots are used. */
} sipSpec;


/* A list of strings. */

typedef struct _stringList {
	char			*s;		/* The string. */
	struct _stringList	*next;		/* The next in the list. */
} stringList;


/* File specific context information for the parser. */

typedef struct _parserContext {
	int			ifdepth;	/* The depth of nested if's. */
	moduleDef		*prevmod;	/* The previous module. */
} parserContext;


extern char *sipVersion;		/* The version of SIP. */
extern stringList *includeDirList;	/* The include directory list for SIP files. */


void parse(sipSpec *,FILE *,char *,char *,stringList *,stringList *);
void parserEOF(char *,parserContext *);
void transform(sipSpec *);
void generateCode(sipSpec *,char *,char *,stringList *,char *,char *,char *,int,int,int,stringList *);
void warning(char *,...);
void fatal(char *,...);
void fatalScopedName(scopedNameDef *);
void setInputFile(FILE *,char *,parserContext *,int);
void *sipMalloc(size_t);
char *sipStrdup(char *);
char *concat(char *,...);
void append(char **,char *);
void addToUsedList(ifaceFileList **,ifaceFileDef *);
int excludedFeature(stringList *,qualDef *);
int sameOverload(overDef *,overDef *,int);
int sameFuncArgs(funcArgs *,funcArgs *,int);
int sameScopedName(scopedNameDef *,scopedNameDef *);
int sameArgType(argDef *,argDef *,int);
int sameBaseType(argDef *,argDef *,int);
char *scopedNameTail(scopedNameDef *);
scopedNameDef *text2scopePart(char *);
scopedNameDef *copyScopedName(scopedNameDef *);
void appendScopedName(scopedNameDef **,scopedNameDef *);
void freeScopedName(scopedNameDef *);
void yywarning(char *);


/* These are only here because bison publically references them. */

/* Represent a set of option flags. */

#define	MAX_NR_FLAGS	5

typedef enum {
	bool_flag,
	string_flag,
	name_flag,
	opt_name_flag,
	integer_flag
} flagType;

typedef struct {
	char		*fname;			/* The flag name. */
	flagType	ftype;			/* The flag type. */
	union {					/* The flag value. */
		char	*sval;			/* A string value. */
		long	ival;			/* An integer value. */
	} fvalue;
} optFlag;

typedef struct {
	int		nrFlags;		/* The number of flags. */
	optFlag		flags[MAX_NR_FLAGS];	/* Each flag. */
} optFlags;

#endif
