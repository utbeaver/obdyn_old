/*
 * The parse tree transformation module for SIP.
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


#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "sip.h"


#define	pyToBasicInt(t)	((t) == short_type || (t) == ushort_type || \
			 (t) == enum_type || (t) == bool_type || \
	    		 (t) == int_type || (t) == uint_type || \
	    		 (t) == long_type || (t) == ulong_type)

#define	pyToInt(t)	(pyToBasicInt(t) || (t) == cint_type)

#define	pyToDouble(t)	(pyToBasicInt(t) || \
			 (t) == float_type || (t) == cfloat_type || \
			 (t) == double_type || (t) == cdouble_type)


static int supportedType(classDef *,overDef *,argDef *,int);
static void ensureInput(classDef *,overDef *,argDef *);
static void defaultInput(argDef *);
static void defaultOutput(classDef *cd,overDef *od,argDef *);
static void addAutoOverload(sipSpec *,classDef *,overDef *);
static void ifaceFileIsUsed(sipSpec *,moduleDef *,ifaceFileDef *,argDef *,int);
static void ifaceFilesAreUsed(sipSpec *,ifaceFileDef *,overDef *);
static void scopeDefaultValue(sipSpec *,classDef *,argDef *);
static void setHierarchy(sipSpec *,classDef *,classDef *);
static void transformCtors(sipSpec *,classDef *);
static void addDefaultCopyCtor(classDef *);
static void transformOverloads(sipSpec *,classDef *,overDef *);
static void transformVariableList(sipSpec *);
static void transformMappedTypes(sipSpec *);
static void getVisibleMembers(sipSpec *,classDef *);
static void getVirtuals(virtOverDef **,mroDef *);
static void addSignalSignatures(sipSpec *,classDef *);
static void addSlotArguments(sipSpec *,overDef *);
static void addSignalArgs(sipSpec *,funcArgs *);
static void resolveMappedTypeTypes(sipSpec *,mappedTypeDef *);
static void resolveCtorTypes(sipSpec *,classDef *,ctorDef *,argDef *);
static void resolveFuncTypes(sipSpec *,classDef *,overDef *);
static void resolvePySigTypes(sipSpec *,classDef *,overDef *,funcArgs *,int);
static void resolveVariableType(sipSpec *,varDef *);
static void fatalNoDefinedType(sipSpec *,scopedNameDef *);
static void getBaseType(sipSpec *,classDef *,argDef *);
static void searchScope(sipSpec *,classDef *,scopedNameDef *,argDef *);
static void searchMappedTypes(sipSpec *,scopedNameDef *,argDef *);
static void searchTypedefs(sipSpec *,scopedNameDef *,argDef *);
static void searchEnums(sipSpec *,scopedNameDef *,argDef *);
static void searchClasses(sipSpec *,scopedNameDef *,argDef *);
static void appendToMRO(mroDef *,mroDef ***,classDef *);


/*
 * Transform the parse tree.
 */

void transform(sipSpec *pt)
{
	int classnr;
	classDef *cd;
	overDef *od;

	if (pt -> module -> name == NULL)
		fatal("No %%Module has been specified for the module\n");

	if (pt -> cppmname == NULL)
		pt -> cppmname = pt -> module -> name;

	/* The simple class checks. */

	classnr = 0;

	for (cd = pt -> classes; cd != NULL; cd = cd -> next)
	{
		if (cd -> iff -> module == NULL)
		{
			fatalScopedName(cd -> iff -> fqname);
			fatal(" has not been defined\n");
		}

		/* Allocate a class number if it is in the main module. */

		if (cd -> iff -> module == pt -> module)
		{
			cd -> classnr = classnr++;

			/*
			 * If we find a class defined in this module called
			 * QObject, assume it's Qt.
			 */

			if (strcmp(cd -> iff -> name -> text,"QObject") == 0)
				pt -> qobjclass = cd -> classnr;
		}
	}

	/* Set the super-class hierarchy for each class. */

	for (cd = pt -> classes; cd != NULL; cd = cd -> next)
		setHierarchy(pt,cd,cd);

	/* Add any automatically generated methods. */

	for (cd = pt -> classes; cd != NULL; cd = cd -> next)
		for (od = cd -> overs; od != NULL; od = od -> next)
			if (isAutoGen(od))
				addAutoOverload(pt,cd,od);

	/* Transform variables and global functions. */
 
	transformVariableList(pt);
	transformOverloads(pt,NULL,pt -> overs);
 
	/* Transform class variables and functions. */
 
	for (cd = pt -> classes; cd != NULL; cd = cd -> next)
	{
		transformCtors(pt,cd);
		transformOverloads(pt,cd,cd -> overs);
	}

	/* Transform mapped types based on templates. */

	transformMappedTypes(pt);

	/* Add default ctors now that the argument types are resolved. */

	for (cd = pt -> classes; cd != NULL; cd = cd -> next)
		if (!isOpaque(cd))
			addDefaultCopyCtor(cd);

	/* Generate the different class views. */

	for (cd = pt -> classes; cd != NULL; cd = cd -> next)
		if (cd -> iff -> type == class_iface)
		{
			/* Get the list of visible member functions. */

			getVisibleMembers(pt,cd);

			if (cd -> iff -> module == pt -> module)
				addSignalSignatures(pt,cd);

			/* Get the virtual members. */

			if (isComplex(cd))
				getVirtuals(&cd -> vmembers,cd -> mro);
		}
		else if (cd -> iff -> type == namespace_iface)
			for (od = cd -> overs; od != NULL; od = od -> next)
			{
				if (cd -> iff -> module == pt -> module)
					addSlotArguments(pt,od);

				ifaceFilesAreUsed(pt,NULL,od);
			}

	/* In case there are any global functions with slot arguments. */

	for (od = pt -> overs; od != NULL; od = od -> next)
		addSlotArguments(pt,od);
}


/*
 * Add an overload that is automatically generated (typically by Qt's moc).
 */

static void addAutoOverload(sipSpec *pt,classDef *autocd,overDef *autood)
{
	classDef *cd;

	/* Find every class that has this one in its hierarchy. */

	for (cd = pt -> classes; cd != NULL; cd = cd -> next)
	{
		mroDef *mro;

		if (cd == autocd)
			continue;

		for (mro = cd -> mro; mro != NULL; mro = mro -> next)
			if (mro -> cd == autocd)
			{
				memberDef *md;
				overDef *od;

				/* Another overload may already exist. */

				for (md = cd -> members; md != NULL; md = md -> next)
					if (md -> pyname == autood -> common -> pyname)
						break;

				if (md == NULL)
				{
					md = sipMalloc(sizeof (memberDef));

					md -> pyname = autood -> common -> pyname;
					md -> slot = autood -> common -> slot;
					md -> module = cd -> iff -> module;
					md -> next = cd -> members;
					cd -> members = md;
				}

				od = sipMalloc(sizeof (overDef));

				*od = *autood;
				od -> common = md;
				od -> next = cd -> overs;
				cd -> overs = od;

				resetIsAutoGen(od);

				break;
			}
	}
}


/*
 * Set the complete hierarchy for a class.
 */

static void setHierarchy(sipSpec *pt,classDef *base,classDef *cd)
{
	classList *cl;
	mroDef **tailp = &cd -> mro;

	/* See if it is a namespace or has already been done. */

	if (cd -> iff -> type == namespace_iface || cd -> mro != NULL)
		return;

	/*
	 * If the base class is in the main module then we need the interface
	 * files for all classes in its hierachy.
	 */

	if (base -> iff -> module == pt -> module)
		setIsUsed(cd -> iff);

	/* The first thing is itself. */

	appendToMRO(cd -> mro,&tailp,cd);

	if (cd -> convtosubcode != NULL)
		cd -> subbase = cd;

	/* Now do it's superclasses. */

	for (cl = cd -> supers; cl != NULL; cl = cl -> next)
	{
		mroDef *mro;

		/* Make sure the super-class's hierarchy has been done. */

		setHierarchy(pt,base,cl -> cd);

		/* Append the super-classes hierarchy. */

		for (mro = cl -> cd -> mro; mro != NULL; mro = mro -> next)
		{
			appendToMRO(cd -> mro,&tailp,mro -> cd);

			/* Ripple through the complex flag. */

			if (isComplex(mro -> cd))
				setIsComplex(cd);

			/*
			 * Ensure that the sub-class base class is the furthest
			 * up the hierarchy.
			 */

			if (mro -> cd -> subbase != NULL)
				cd -> subbase = mro -> cd -> subbase;
		}
	}

	/* We can't derive from a class with a private dtor. */

	if (isPrivateDtor(cd))
		resetIsComplex(cd);
}


/*
 * Append a class definition to an mro list
 */

static void appendToMRO(mroDef *head,mroDef ***tailp,classDef *cd)
{
	mroDef *mro, *new;

	new = sipMalloc(sizeof (mroDef));

	new -> cd = cd;
	new -> mroflags = 0;
	new -> next = NULL;

	/* See if it is a duplicate. */

	for (mro = head; mro != NULL; mro = mro -> next)
		if (mro -> cd == cd)
		{
			setIsDuplicateSuper(new);

			if (!isDuplicateSuper(mro))
				setHasDuplicateSuper(mro);

			break;
		}

	/* Append to the list and update the tail pointer. */
	**tailp = new;
	*tailp = &new -> next;
}


/*
 * Transform the data types for mapped types based on a template.
 */

static void transformMappedTypes(sipSpec *pt)
{
	mappedTypeDef *mt;

	for (mt = pt -> mappedtypes; mt != NULL; mt = mt -> next)
	{
		/* Nothing to do if this isn't template based. */

		if (mt -> type.atype == template_type)
			resolveMappedTypeTypes(pt,mt);
	}
}


/*
 * Transform the data types for a list of ctors.
 */

static void transformCtors(sipSpec *pt,classDef *cd)
{
	ctorDef *ct;

	for (ct = cd -> ctors; ct != NULL; ct = ct -> next)
		resolveCtorTypes(pt,cd,ct,ct -> args.args);
}


/*
 * Add a default copy ctor is required.
 */

static void addDefaultCopyCtor(classDef *cd)
{
	ctorDef *copyct;
	mroDef *mro;

	/* See if there is a private copy ctor in the hierarchy. */
 
	copyct = NULL;
 
	for (mro = cd -> mro; mro != NULL; mro = mro -> next)
	{
		ctorDef *ct;

		for (ct = mro -> cd -> ctors; ct != NULL; ct = ct -> next)
		{
			argDef *ad = &ct -> args.args[0];
 
			/* See if is a copy ctor. */
 
			if (ct -> args.nrArgs != 1 || ad -> nrderefs != 0 ||
			    !isReference(ad) || ad -> atype != class_type ||
			    ad -> u.cd != mro -> cd)
				continue;

			/* Stop now if the copy ctor is private. */

			if (isPrivateCtor(ct))
				return;
 
			/*
			 * Remember if it's in the class we are dealing with.
			 */
 
			if (mro == cd -> mro)
				copyct = ct;
 
			break;
		}
	}
 
	if (copyct == NULL)
	{
		ctorDef **tailp;
 
		/* Create a default public copy ctor. */
 
		copyct = sipMalloc(sizeof (ctorDef));
 
		copyct -> ctorflags = SECT_IS_PUBLIC;
		copyct -> args.nrArgs = 1;
		copyct -> args.args[0].atype = class_type;
		copyct -> args.args[0].u.cd = cd;
		copyct -> args.args[0].argflags = (ARG_IS_REF | ARG_IS_CONST | ARG_IN);
		copyct -> args.args[0].nrderefs = 0;
		copyct -> args.args[0].defval = NULL;
 
		copyct -> cppsig = NULL;
		copyct -> exceptions = NULL;
		copyct -> methodcode = NULL;
		copyct -> membercode = NULL;
		copyct -> prehook = NULL;
		copyct -> posthook = NULL;
		copyct -> next = NULL;
 
		/* Append it to the list. */
 
		for (tailp = &cd -> ctors; *tailp != NULL; tailp = &(*tailp) -> next)
			;
 
		*tailp = copyct;
	}
}


/*
 * Transform the data types for a list of overloads.
 */

static void transformOverloads(sipSpec *pt,classDef *scope,overDef *overs)
{
	overDef *od;

	for (od = overs; od != NULL; od = od -> next)
		resolveFuncTypes(pt,scope,od);
}


/*
 * Transform the data types for the variables.
 */

static void transformVariableList(sipSpec *pt)
{
	varDef *vd;

	for (vd = pt -> vars; vd != NULL; vd = vd -> next)
		resolveVariableType(pt,vd);
}


/*
 * Set the list of visible member functions for a class.
 */

static void getVisibleMembers(sipSpec *pt,classDef *cd)
{
	mroDef *mro;

	cd -> visible = NULL;

	for (mro = cd -> mro; mro != NULL; mro = mro -> next)
	{
		memberDef *md;

		if (isDuplicateSuper(mro))
			continue;

		for (md = mro -> cd -> members; md != NULL; md = md -> next)
		{
			visibleList *vl;

			/*
			 * See if it is already in the list.  This has the
			 * desired side effect of eliminating any functions
			 * that have an implementation closer to this class in
			 * the hierarchy.  This is the only reason to define
			 * private functions.
			 */

			for (vl = cd -> visible; vl != NULL; vl = vl -> next)
				if (vl -> m -> pyname == md -> pyname)
					break;

			/* See if it is a new member function. */

			if (vl == NULL)
			{
				overDef *od;

				vl = sipMalloc(sizeof (visibleList));

				vl -> m = md;
				vl -> cd = mro -> cd;
				vl -> next = cd -> visible;

				cd -> visible = vl;


				for (od = mro -> cd -> overs; od != NULL; od = od -> next)
					if (od -> common == md)
						ifaceFilesAreUsed(pt,cd -> iff,od);
			}
		}
	}
}


/*
 * Get the list of visible virtual functions for a class.
 */
 
static void getVirtuals(virtOverDef **headp,mroDef *mro)
{
	classDef *cd = mro -> cd;
	overDef *od;

	/*
	 * Go through the next in the super-class hierarchy.  Note that we
	 * don't skip duplicate classes because we want to make sure that the
	 * far pointer points to something that implements a virtual handler.
	 */

	for (od = cd -> overs; od != NULL; od = od -> next)
	{
		virtOverDef *vod;

		if (!isVirtual(od))
			continue;

		/*
		 * See if a virtual of this name and signature is already in
		 * the list.
		 */
 
		for (vod = *headp; vod != NULL; vod = vod -> next)
			if (strcmp(vod -> o -> cppname,od -> cppname) == 0 && sameOverload(vod -> o,od,TRUE))
				break;
 
		if (vod == NULL)
		{
			vod = sipMalloc(sizeof (virtOverDef));
 
			vod -> o = od;
			vod -> next = *headp;
 
			*headp = vod;
		}

		/*
		 * Set the far pointer, possibly overwriting one that wasn't so
		 * far.
		 */

		vod -> farc = cd;
	}
 
	/* Now go through the next in the hierarchy. */
 
	if (mro -> next != NULL)
		getVirtuals(headp,mro -> next);

	/* If this is a duplicate class, then that's all we need to do. */

	if (isDuplicateSuper(mro))
		return;
 
	/*
	 * Look for any members in this class that match any in the virtual
	 * list and set the "near" class.
	 */
 
	for (od = cd -> overs; od != NULL; od = od -> next)
	{
		virtOverDef *vod;

		for (vod = *headp; vod != NULL; vod = vod -> next)
			if (strcmp(vod -> o -> cppname,od -> cppname) == 0 && sameOverload(vod -> o,od,TRUE))
				break;
 
		if (vod != NULL)
		{
			/*
			 * Use this implementation of the overload to pick up
			 * its protected/private flags.  However, keep the
			 * real C++ signature if we have one.
			 */

			if (od -> cppsig == NULL)
				od -> cppsig = vod -> o -> cppsig;

			vod -> o = od;
			vod -> nearc = cd;
		}
	}
}


/*
 * Resolve the types of a mapped type based on a template.
 */

static void resolveMappedTypeTypes(sipSpec *pt,mappedTypeDef *mt)
{
	int a;
	templateDef *td = mt -> type.u.td;

	for (a = 0; a < td -> types.nrArgs; ++a)
	{
		getBaseType(pt,NULL,&td -> types.args[a]);

		ifaceFileIsUsed(pt,NULL,mt -> iff,&td -> types.args[a],FALSE);
	}
}


/*
 * Resolve the types of a ctor.
 */

static void resolveCtorTypes(sipSpec *pt,classDef *scope,ctorDef *ct,
			     argDef *args)
{
	int a;

	/* Handle any C++ signature. */

	if (ct -> cppsig != NULL)
		for (a = 0; a < ct -> cppsig -> args.nrArgs; ++a)
			getBaseType(pt,scope,&ct -> cppsig -> args.args[a]);

	/* Handle the Python signature. */

	for (a = 0; a < ct -> args.nrArgs; ++a)
	{
		argDef *ad = &args[a];

		getBaseType(pt,scope,ad);

		if (!supportedType(scope,NULL,ad,FALSE) && (ct -> cppsig == NULL || ct -> methodcode == NULL) && ct -> membercode == NULL)
		{
			fatalScopedName(scope -> iff -> fqname);
			fatal(" unsupported ctor argument type - provide %%MethodCode and a C++ signature\n");
		}

		ifaceFileIsUsed(pt,NULL,scope -> iff,ad,FALSE);
		scopeDefaultValue(pt,scope,ad);
	}
}


/*
 * Resolve the types of a function.
 */

static void resolveFuncTypes(sipSpec *pt,classDef *scope,overDef *od)
{
	argDef *res = od -> result;

	/* Handle any C++ signature. */

	if (od -> cppsig != NULL)
	{
		int a;
 
		getBaseType(pt,scope,&od -> cppsig -> result);

		for (a = 0; a < od -> cppsig -> args.nrArgs; ++a)
			getBaseType(pt,scope,&od -> cppsig -> args.args[a]);
        }

	/* Handle the Python signature. */

	if (res != NULL)
	{
		int bad;

		if (isSignal(od))
		{
			if (scope != NULL)
			{
				fatalScopedName(scope -> iff -> fqname);
				fatal("::");
			}

			fatal("%s() signals must return void\n",od -> cppname);
		}

		getBaseType(pt,scope,res);

		/* Results must be simple. */

		bad = TRUE;

		if (!(!supportedType(scope,od,res,FALSE) && (od -> cppsig == NULL || od -> methodcode == NULL) && od -> membercode == NULL))
		{
			switch (od -> common -> slot)
			{
			case iadd_slot:
			case isub_slot:
			case imul_slot:
			case idiv_slot:
			case imod_slot:
			case iand_slot:
			case ior_slot:
			case ixor_slot:
			case ilshift_slot:
			case irshift_slot:
				if (res -> atype == class_type &&
				    res -> u.cd == scope)
					bad = FALSE;

				break;

			case int_slot:
			case len_slot:
			case contains_slot:
				if (res -> atype == int_type &&
				    !isReference(res) && !isConstArg(res) &&
				    res -> nrderefs == 0)
					bad = FALSE;

				break;

			default:
				bad = FALSE;
			}
		}

		if (bad)
		{
			if (scope != NULL)
			{
				fatalScopedName(scope -> iff -> fqname);
				fatal("::");
			}

			fatal("%s() unsupported function return type - provide %%MethodCode and a C++ signature\n",od -> cppname);
		}
	}

	resolvePySigTypes(pt,scope,od,&od -> args,isSignal(od));

	/*
	 * SIP v4 is more strict about the return types of operators, so warn
	 * about them here.
	 */

	if (od -> common -> slot == len_slot ||
	    od -> common -> slot == cmp_slot ||
	    od -> common -> slot == contains_slot ||
	    od -> common -> slot == nonzero_slot)
		if (res == NULL || res -> atype != int_type ||
		    res -> nrderefs != 0 || isReference(res) ||
		    isConstArg(res))
			warning("SIP v4 requires that %s slots return int\n",od -> common -> pyname -> text);

	if (od -> common -> slot == setitem_slot ||
	    od -> common -> slot == delitem_slot)
		if (res != NULL)
			warning("SIP v4 requires that %s slots return void\n",od -> common -> pyname -> text);
}


/*
 * Resolve the types of a Python signature.
 */

static void resolvePySigTypes(sipSpec *pt,classDef *scope,overDef *od,
			      funcArgs *pysig,int issignal)
{
	int a;

	for (a = 0; a < pysig -> nrArgs; ++a)
	{
		argDef *ad = &pysig -> args[a];

		getBaseType(pt,scope,ad);

		if (ad -> atype == slotcon_type)
			resolvePySigTypes(pt,scope,od,ad -> u.sa,TRUE);

		/*
		 * Note signal arguments are restricted in their types because
		 * we don't (yet) support handwritten code for them.
		 */
		if (issignal)
		{
			if (!supportedType(scope,od,ad,FALSE))
			{
				if (scope != NULL)
				{
					fatalScopedName(scope -> iff -> fqname);
					fatal("::");
				}

				fatal("%s() unsupported signal argument type\n");
			}
		}
		else if (isVirtual(od))
		{
			if (!supportedType(scope,od,ad,TRUE))
			{
				if (od -> methodcode == NULL && od -> membercode == NULL)
				{
					if (scope != NULL)
					{
						fatalScopedName(scope -> iff -> fqname);
						fatal("::");
					}

					fatal("%s() unsupported argument type for virtual functions - provide %%MethodCode or %%MemberCode\n",od -> cppname);
				}

				if (od -> virtcode == NULL)
				{
					if (scope != NULL)
					{
						fatalScopedName(scope -> iff -> fqname);
						fatal("::");
					}

					fatal("%s() unsupported function argument type - provide %%VirtualCode\n",od -> cppname);
				}
			}
		}
		else if (!supportedType(scope,od,ad,TRUE) && (od -> cppsig == NULL || od -> methodcode == NULL) && od -> membercode == NULL)
		{
			if (scope != NULL)
			{
				fatalScopedName(scope -> iff -> fqname);
				fatal("::");
			}

			fatal("%s() unsupported function argument type - provide %%MemberCode\n",od -> cppname);
		}

		if (scope != NULL)
			scopeDefaultValue(pt,scope,ad);
	}
}


/*
 * Resolve the type of a variable.
 */

static void resolveVariableType(sipSpec *pt,varDef *vd)
{
	int bad = TRUE;
	argDef *vtype = &vd -> type;

	getBaseType(pt,vd -> ecd,vtype);

	switch (vtype -> atype)
	{
	case mapped_type:
	case class_type:
		/* Class, Class & and Class * are supported. */

		if (vtype -> nrderefs <= 1)
			bad = FALSE;
		break;

	case ustring_type:
	case string_type:
		/* (unsigned) char, (unsigned) char * are supported. */

		if (!isReference(vtype) && vtype -> nrderefs <= 1)
			bad = FALSE;
		break;

	case float_type:
	case double_type:
	case enum_type:
	case bool_type:
	case ushort_type:
	case short_type:
	case uint_type:
	case cint_type:
	case int_type:
	case ulong_type:
	case long_type:
	case pyobject_type:
		/* These are supported without pointers or references. */

		if (!isReference(vtype) && vtype -> nrderefs == 0)
			bad = FALSE;
		break;

	case struct_type:
	case voidptr_type:
		/* A simple pointer is supported. */

		if (!isReference(vtype) && vtype -> nrderefs == 1)
			bad = FALSE;
		break;
	}

	if (bad)
	{
		fatalScopedName(vd -> fqname);
		fatal(" has an unsupported type\n");
	}
 
	if (vtype -> atype != class_type && vd -> accessfunc != NULL)
	{
		fatalScopedName(vd -> fqname);
		fatal(" has %%VariableCode but isn't a class instance\n");
	}

	ifaceFileIsUsed(pt,vd -> module,(vd -> ecd != NULL ? vd -> ecd -> iff : NULL),vtype,FALSE);
}



/*
 * Add any signal signatures defined by a class to the list that the module's
 * proxy needs to deal with.
 */

static void addSignalSignatures(sipSpec *pt,classDef *cd)
{
	visibleList *vl;

	for (vl = cd -> visible; vl != NULL; vl = vl -> next)
	{
		overDef *od;

		for (od = vl -> cd -> overs; od != NULL; od = od -> next)
			if (od -> common == vl -> m)
			{
				if (isSignal(od))
					addSignalArgs(pt,&od -> args);

				addSlotArguments(pt,od);
			}
	}
}


/*
 * Add any argument slots to the list of module signals.
 */

static void addSlotArguments(sipSpec *pt,overDef *od)
{
	int a;

	for (a = 0; a < od -> args.nrArgs; ++a)
		if (od -> args.args[a].atype == slotcon_type)
			addSignalArgs(pt,od -> args.args[a].u.sa);
}


/*
 * Add a set of arguments to the list of signals that might be emitted by this
 * module.
 */

static void addSignalArgs(sipSpec *pt,funcArgs *fa)
{
	funcArgsList *fl;

	for (fl = pt -> sigargs; fl != NULL; fl = fl -> next)
		if (sameFuncArgs(fa,fl -> fa,TRUE))
			break;
 
	/* Add if it is a new signature. */

	if (fl == NULL)
	{
		fl = sipMalloc(sizeof (funcArgsList));
		fl -> fa = fa;
		fl -> next = pt -> sigargs;
 
		pt -> sigargs = fl;
	}
}


/*
 * See if a type is supported by the generated code.
 */

static int supportedType(classDef *cd,overDef *od,argDef *ad,int outputs)
{
	switch (ad -> atype)
	{
	case signal_type:
	case slot_type:
	case rxcon_type:
	case rxdis_type:
	case slotcon_type:
	case slotdis_type:
	case qobject_type:
		/* These can only appear in argument lists without * or &. */

		ensureInput(cd,od,ad);
		return TRUE;

	case ustring_type:
	case string_type:
		if (isReference(ad))
		{
			if (outputs && ad -> nrderefs <= 1)
			{
				defaultOutput(cd,od,ad);
				return TRUE;
			}
		}
		else if (ad -> nrderefs == 0)
		{
			ensureInput(cd,od,ad);
			return TRUE;
		}
		else if (ad -> nrderefs == 1)
		{
			if (outputs)
				defaultInput(ad);
			else
				ensureInput(cd,od,ad);

			return TRUE;
		}
		else if (ad -> nrderefs == 2 && outputs)
		{
			defaultOutput(cd,od,ad);
			return TRUE;
		}

		break;

	case float_type:
	case double_type:
	case enum_type:
	case bool_type:
	case ushort_type:
	case short_type:
	case uint_type:
	case cint_type:
	case int_type:
	case ulong_type:
	case long_type:
	case pyobject_type:
	case pytuple_type:
	case pylist_type:
	case pydict_type:
	case pycallable_type:
	case pyslice_type:
		if (isReference(ad))
		{
			if (ad -> nrderefs == 0 && outputs)
			{
				defaultOutput(cd,od,ad);
				return TRUE;
			}
		}
		else if (ad -> nrderefs == 0)
		{
			ensureInput(cd,od,ad);
			return TRUE;
		}
		else if (ad -> nrderefs == 1 && outputs)
		{
			defaultOutput(cd,od,ad);
			return TRUE;
		}

		break;

	case mapped_type:
	case class_type:
		if (isReference(ad))
		{
			if (ad -> nrderefs == 0)
			{
				defaultInput(ad);
				return TRUE;
			}
			else if (ad -> nrderefs == 1 && outputs)
			{
				defaultOutput(cd,od,ad);
				return TRUE;
			}
		}
		else if (ad -> nrderefs == 0)
		{
			ensureInput(cd,od,ad);
			return TRUE;
		}
		else if (ad -> nrderefs == 1)
		{
			if (outputs)
				defaultInput(ad);
			else
				ensureInput(cd,od,ad);

			return TRUE;
		}
		else if (ad -> nrderefs == 2 && outputs)
		{
			defaultOutput(cd,od,ad);
			return TRUE;
		}

		break;

	case struct_type:
	case voidptr_type:
		if (isReference(ad))
		{
			if (ad -> nrderefs == 1 && outputs)
			{
				defaultOutput(cd,od,ad);
				return TRUE;
			}
		}
		else if (ad -> nrderefs == 1)
		{
			ensureInput(cd,od,ad);
			return TRUE;
		}
		else if (ad -> nrderefs == 2 && outputs)
		{
			defaultOutput(cd,od,ad);
			return TRUE;
		}

		break;
	}

	/* Unsupported if we got this far. */

	return FALSE;
}


/*
 * Ensure the direction of an argument is an input.
 */

static void ensureInput(classDef *cd,overDef *od,argDef *ad)
{
	if (isOutArg(ad))
	{
		if (cd != NULL)
		{
			fatalScopedName(cd -> iff -> fqname);
			fatal("::");
		}

		if (od != NULL)
			fatal("%s",od -> cppname);

		fatal("() invalid argument type for /Out/\n");
	}

	setIsInArg(ad);
}


/*
 * Default the direction of an argument to an input.
 */

static void defaultInput(argDef *ad)
{
	if (!isInArg(ad) && !isOutArg(ad))
		setIsInArg(ad);
}


/*
 * Default the direction of an argument to an output.
 */

static void defaultOutput(classDef *cd,overDef *od,argDef *ad)
{
	if (isOutArg(ad))
	{
		if (isConstArg(ad))
		{
			if (cd != NULL)
			{
				fatalScopedName(cd -> iff -> fqname);
				fatal("::");
			}

			if (od != NULL)
				fatal("%s",od -> cppname);

			fatal("() const argument cannot have /Out/ specified\n");
		}
	}
	else if (!isInArg(ad))
		if (isConstArg(ad))
			setIsInArg(ad);
		else
			setIsOutArg(ad);
}


/*
 * Put a scoped name to stderr.
 */

void fatalScopedName(scopedNameDef *snd)
{
	while (snd != NULL)
	{
		fatal("%s",snd -> name);

		snd = snd -> next;

		if (snd != NULL)
			fatal("::");
	}
}


/*
 * Compare two overloads and return TRUE if they are the same as far as C++ is
 * concerned.
 */

int sameOverload(overDef *od1,overDef *od2,int strict)
{
	/* They must both be const, or both not. */

	if (isConst(od1) != isConst(od2))
		return FALSE;

	/* The return type must be the same. */

	if (od1 -> result == NULL && od2 -> result != NULL)
		return FALSE;

	if (od1 -> result != NULL && od2 -> result == NULL)
		return FALSE;

	if (od1 -> result != NULL && od2 -> result != NULL && !sameArgType(od1 -> result,od2 -> result,strict))
		return FALSE;

	return sameFuncArgs(&od1 -> args,&od2 -> args,strict);
}


/*
 * Compare two sets of function arguments are return TRUE if they are the same.
 */

int sameFuncArgs(funcArgs *fa1,funcArgs *fa2,int strict)
{
	int a;

	if (strict)
	{
		/* The number of arguments must be the same. */

		if (fa1 -> nrArgs != fa2 -> nrArgs)
			return FALSE;
	}
	else
	{
		int na1, na2;

		/* We only count the compulsory arguments. */

		na1 = 0;

		for (a = 0; a < fa1 -> nrArgs; ++a)
		{
			if (fa1 -> args[a].defval != NULL)
				break;

			++na1;
		}

		na2 = 0;

		for (a = 0; a < fa2 -> nrArgs; ++a)
		{
			if (fa2 -> args[a].defval != NULL)
				break;

			++na2;
		}

		if (na1 != na2)
			return FALSE;
	}

	/* The arguments must be the same. */

	for (a = 0; a < fa1 -> nrArgs; ++a)
	{
		if (!strict && fa1 -> args[a].defval != NULL)
			break;

		if (!sameArgType(&fa1 -> args[a],&fa2 -> args[a],strict))
			return FALSE;
	}

	/* Must be the same if we've got this far. */

	return TRUE;
}


/*
 * Compare two argument types and return TRUE if they are the same.
 */

int sameArgType(argDef *a1,argDef *a2,int strict)
{
	/* The indirection and the references must be the same. */

	if (isReference(a1) != isReference(a2) || a1 -> nrderefs != a2 -> nrderefs)
		return FALSE;

	/* The const should be the same. */

	if (isConstArg(a1) != isConstArg(a2))
		return FALSE;

	/*
	 * In non-strict, just check if they are subject to Python conversions.
	 */

	if (!strict)
		if ((pyToInt(a1 -> atype) && pyToInt(a2 -> atype)) ||
		    (pyToDouble(a1 -> atype) && pyToDouble(a2 -> atype)))
		return TRUE;

	return sameBaseType(a1,a2,strict);
}


/*
 * Compare two basic types and return TRUE if they are the same.
 */

int sameBaseType(argDef *a1,argDef *a2,int strict)
{
	/* The types must be the same. */

	if (a1 -> atype != a2 ->atype)
		return FALSE;

	switch (a1 -> atype)
	{
	case class_type:
		if (a1 -> u.cd != a2 -> u.cd)
			return FALSE;

		break;

	case enum_type:
		if (strict && a1 -> u.ed != a2 -> u.ed)
			return FALSE;

		break;

	case slotcon_type:
	case slotdis_type:
		if (!sameFuncArgs(a1 -> u.sa,a2 -> u.sa,strict))
			return FALSE;

		break;

	case template_type:
		{
			int a;
			templateDef *td1, *td2;

			td1 = a1 -> u.td;
			td2 = a2 -> u.td;

			if (!sameScopedName(td1 -> fqname,td2 -> fqname) != 0 ||
			    td1 -> types.nrArgs != td2 -> types.nrArgs)
				return FALSE;

			for (a = 0; a < td1 -> types.nrArgs; ++a)
				if (!sameBaseType(&td1 -> types.args[a],&td2 -> types.args[a],strict))
					return FALSE;

			break;
		}

	case struct_type:
		if (!sameScopedName(a1 -> u.sname,a2 -> u.sname) != 0)
			return FALSE;

		break;

	case defined_type:
		if (!sameScopedName(a1 -> u.snd,a2 -> u.snd))
			return FALSE;

		break;

	case mapped_type:
		if (a1 -> u.mtd != a2 -> u.mtd)
			return FALSE;

		break;
	}

	/* Must be the same if we've got this far. */

	return TRUE;
}


/*
 * Return TRUE if two scoped names are the same.
 */

int sameScopedName(scopedNameDef *snd1,scopedNameDef *snd2)
{
	while (snd1 != NULL && snd2 != NULL && strcmp(snd1 -> name,snd2 -> name) == 0)
	{
		snd1 = snd1 -> next;
		snd2 = snd2 -> next;
	}

	return (snd1 == NULL && snd2 == NULL);
}


/*
 * Add an explicit scope to the default value of an argument if possible.
 */

static void scopeDefaultValue(sipSpec *pt,classDef *cd,argDef *ad)
{
	valueDef *vd, **tailp, *newvd;

	/*
	 * We do a quick check to see if we need to do anything.  This means
	 * we can limit the times we need to copy the default value.  It needs
	 * to be copied because it will be shared by class versions that have
	 * been created on the fly and it may need to be scoped differently for
	 * each of those versions.
	 */

	for (vd = ad -> defval; vd != NULL; vd = vd -> next)
		if (vd -> vtype == scoped_value && vd -> u.vscp -> next == NULL)
			break;

	if (vd == NULL)
		return;

	/*
	 * It's not certain that we will do anything, but we assume we will and
	 * start copying.
	 */

	newvd = NULL;
	tailp = &newvd;

	for (vd = ad -> defval; vd != NULL; vd = vd -> next)
	{
		mroDef *mro;
		scopedNameDef *origname;
		valueDef *new;

		/* Make the copy. */

		new = sipMalloc(sizeof (valueDef));

		*new = *vd;
		*tailp = new;
		tailp = &new -> next;

		/*
		 * Skip this part of the expression if it isn't a named value
		 * or it already has a scope.
		 */

		if (vd -> vtype != scoped_value || vd -> u.vscp -> next != NULL)
			continue;

		/*
		 * Search the class hierarchy for an enum value with the same
		 * name.  If we don't find one, leave it as it is (the compiler
		 * will find out if this is a problem).
		 */

		origname = vd -> u.vscp;

		for (mro = cd -> mro; mro != NULL; mro = mro -> next)
		{
			enumDef *ed;

			if (isDuplicateSuper(mro))
				continue;

			for (ed = pt -> enums; ed != NULL; ed = ed -> next)
			{
				enumValueDef *evd;

				if (ed -> ecd != mro -> cd)
					continue;

				for (evd = ed -> values; evd != NULL; evd = evd -> next)
					if (strcmp(evd -> name -> text,origname -> name) == 0)
					{
						scopedNameDef *snd;

						/*
						 * Take the scope from the
						 * class that the enum was
						 * defined in.
						 */

						snd = copyScopedName(mro -> cd -> iff -> fqname);
						appendScopedName(&snd,origname);

						new -> u.vscp = snd;

						/* Nothing more to do. */

						break;
					}

				if (evd != NULL)
					break;
			}

			if (ed != NULL)
				break;
		}
	}

	ad -> defval = newvd;
}


/*
 * Make sure a type is a base type.
 */

static void getBaseType(sipSpec *pt,classDef *defscope,argDef *type)
{
	/* Loop until we've got to a base type. */

	while (type -> atype == defined_type)
	{
		scopedNameDef *snd = type -> u.snd;

		type -> atype = no_type;

		if (defscope != NULL)
			searchScope(pt,defscope,snd,type);

		if (type -> atype == no_type)
			searchMappedTypes(pt,snd,type);

		if (type -> atype == no_type)
			searchTypedefs(pt,snd,type);

		if (type -> atype == no_type)
			searchEnums(pt,snd,type);

		if (type -> atype == no_type)
			searchClasses(pt,snd,type);

		if (type -> atype == no_type)
			fatalNoDefinedType(pt,snd);
	}

	/* Get the base of type of any slot arguments. */

	if (type -> atype == slotcon_type || type -> atype == slotdis_type)
	{
		int sa;

		for (sa = 0; sa < type -> u.sa -> nrArgs; ++sa)
			getBaseType(pt,defscope,&type -> u.sa -> args[sa]);
	}

	/* Replace the base type if it has been mapped. */

	if (type -> atype == struct_type || type -> atype == template_type)
		searchMappedTypes(pt,NULL,type);
}


/*
 * Search for a name in a scope and return the corresponding type.
 */

static void searchScope(sipSpec *pt,classDef *scope,scopedNameDef *snd,
			argDef *ad)
{
	scopedNameDef *tmpsnd = NULL;
	mroDef *mro;

	for (mro = scope -> mro; mro != NULL; mro = mro -> next)
	{
		if (isDuplicateSuper(mro))
			continue;

		/* Append the name to the scope and see if it exists. */

		tmpsnd = copyScopedName(classFQName(mro -> cd));
		appendScopedName(&tmpsnd,copyScopedName(snd));

		searchMappedTypes(pt,tmpsnd,ad);

		if (ad -> atype != no_type)
			break;

		searchTypedefs(pt,tmpsnd,ad);

		if (ad -> atype != no_type)
			break;

		searchEnums(pt,tmpsnd,ad);

		if (ad -> atype != no_type)
			break;

		searchClasses(pt,tmpsnd,ad);

		if (ad -> atype != no_type)
			break;

		freeScopedName(tmpsnd);
		tmpsnd = NULL;
	}

	if (tmpsnd != NULL)
		freeScopedName(tmpsnd);
}


/*
 * Search the mapped types for a name and return the type.
 */

static void searchMappedTypes(sipSpec *pt,scopedNameDef *snd,argDef *ad)
{
	mappedTypeDef *mtd;
	scopedNameDef *oname;

	/* Patch back to defined types so we can use sameBaseType(). */

	if (snd != NULL)
	{
		oname = ad -> u.snd;
		ad -> u.snd = snd;
		ad -> atype = defined_type;
	}

	for (mtd = pt -> mappedtypes; mtd != NULL; mtd = mtd -> next)
		if (sameBaseType(ad,&mtd -> type,TRUE))
		{
			/* Copy the type. */

			ad -> atype = mapped_type;
			ad -> u.mtd = mtd;

			return;
		}

	/* Restore because we didn't find anything. */

	if (snd != NULL)
	{
		ad -> u.snd = oname;
		ad -> atype = no_type;
	}
}


/*
 * Search the typedefs for a name and return the type.
 */

static void searchTypedefs(sipSpec *pt,scopedNameDef *snd,argDef *ad)
{
	typedefDef *td;

	for (td = pt -> typedefs; td != NULL; td = td -> next)
		if (sameScopedName(td -> fqname,snd))
		{
			/* Copy the type. */

			ad -> atype = td -> type.atype;
			ad -> argflags |= td -> type.argflags;
			ad -> nrderefs += td -> type.nrderefs;
			ad -> u = td -> type.u;

			break;
		}
}


/*
 * Search the enums for a name and return the type.
 */

static void searchEnums(sipSpec *pt,scopedNameDef *snd,argDef *ad)
{
	enumDef *ed;

	for (ed = pt -> enums; ed != NULL; ed = ed -> next)
	{
		if (ed -> fqname == NULL)
			continue;

		if (sameScopedName(ed -> fqname,snd))
		{
			ad -> atype = enum_type;
			ad -> u.ed = ed;

			break;
		}
	}
}


/*
 * Search the classes for a name and return the type.
 */

static void searchClasses(sipSpec *pt,scopedNameDef *snd,argDef *ad)
{
	classDef *cd;

	for (cd = pt -> classes; cd != NULL; cd = cd -> next)
		if (sameScopedName(classFQName(cd),snd))
		{
			ad -> atype = class_type;
			ad -> u.cd = cd;

			break;
		}
}


/*
 * Print an error message describing an undefined type to stderr and terminate.
 */

static void fatalNoDefinedType(sipSpec *pt,scopedNameDef *snd)
{
	fatalScopedName(snd);
	fatal(" is undefined\n");
}


/*
 * Make sure all interface files for a signature are used.
 */

static void ifaceFilesAreUsed(sipSpec *pt,ifaceFileDef *iff,overDef *od)
{
	int a;

	if (od -> result != NULL)
		ifaceFileIsUsed(pt,od -> common -> module,iff,od -> result,FALSE);

	for (a = 0; a < od -> args.nrArgs; ++a)
		ifaceFileIsUsed(pt,od -> common -> module,iff,&od -> args.args[a],isSignal(od));
}


/*
 * If a type has an interface file then add it to the appropriate list of used
 * interface files so that the header file is #included in the generated code.
 */

static void ifaceFileIsUsed(sipSpec *pt,moduleDef *mod,ifaceFileDef *iff,argDef *ad,int issig)
{
	ifaceFileDef *usediff;

	switch (ad -> atype)
	{
	case class_type:
		usediff = ad -> u.cd -> iff;
		break;

	case mapped_type:
		usediff = ad -> u.mtd -> iff;
		break;

	default:
		return;
	}

	if (iff != NULL && iff != usediff)
		addToUsedList(&iff -> used,usediff);

	/*
	 * It's used by the main module if it is in a namespace, outside a
	 * class/mapped type or it is a signal.
	 */

	if (mod == pt -> module && (iff == NULL || issig))
		addToUsedList(&pt -> used,usediff);
}
