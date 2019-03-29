/*
 * The code generator module for SIP.
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


#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "sip.h"


/* Macro expansions for template code. */

#define	MACRO_ESCAPE	'$'
#define	MACRO_CLASS	'C'		/* Class name. */
#define	MACRO_SOURCES	'S'		/* C++ and header files. */
#define	MACRO_CPP	'B'		/* C++ files. */
#define	MACRO_HEADERS	'H'		/* Header files. */
#define	MACRO_OBJECTS	'O'		/* Object files. */
#define	MACRO_CPPSUFF	'c'		/* C++ file suffix. */
#define	MACRO_OBJSUFF	'o'		/* Object file suffix. */
#define	MACRO_CPPMODULE	'm'		/* C++ module name. */
#define	MACRO_PERCENT	'P'		/* Percent sign. */


/* Control what generateSingleArg() actually generates. */

typedef enum {
	Call,
	Declaration,
	Definition
} funcArgType;


/* An entry in the sorted array of methods. */

typedef struct {
	memberDef	*md;		/* The method. */
} sortedMethTab;


/* An API definition. */

typedef struct _apiDef {
	char		*name;		/* The function name. */
	funcArgs	*args;		/* The function arguments. */
	struct _apiDef	*next;		/* The next in the list. */
} apiDef;


static int currentLineNr;		/* Current output line number. */
static char *currentFileName;		/* Current output file name. */
static int previousLineNr;		/* Previous output line number. */
static char *previousFileName;		/* Previous output file name. */
static int exceptions;			/* Set if exceptions are enabled. */
static int tracing;			/* Set if tracing is enabled. */


static void generateDocumentation(sipSpec *,char *);
static void generateAPI(sipSpec *,char *);
static void generateBuildFile(sipSpec *,char *,char *,int);
static void generateMakefile(sipSpec *,char *,char *,int);
static void generatePython(sipSpec *,char *);
static void generateModuleProxy(sipSpec *,int,FILE *);
static void generateModuleProxyHeader(sipSpec *,char *,int);
static void generatePackageHeader(sipSpec *,char *,stringList *);
static void generateCpp(sipSpec *,char *,char *,int *);
static void generateIfaceCpp(sipSpec *,ifaceFileDef *,char *,char *,FILE *);
static void generateMappedTypeCpp(mappedTypeDef *,sipSpec *,FILE *);
static void generateMappedTypeHeader(mappedTypeDef *,int,char *,sipSpec *,
				     FILE *);
static void generateClassCpp(classDef *,sipSpec *,FILE *);
static void generateClassHeader(classDef *,int,char *,sipSpec *,FILE *);
static void generatePythonClassTree(sipSpec *,nodeDef *,int,FILE *);
static void generatePythonClassWrappers(sipSpec *,FILE *);
static void generatePythonClass(sipSpec *,classDef *,int,FILE *);
static void generateClassFunctions(sipSpec *,classDef *,FILE *);
static void generateComplexCode(sipSpec *,classDef *,FILE *);
static void generateFunction(sipSpec *,memberDef *,overDef *,classDef *,
			     classDef *,FILE *);
static void generateFunctionBody(sipSpec *,overDef *,classDef *,classDef *,
				 FILE *);
static void generatePythonConstructor(sipSpec *,classDef *,FILE *);
static void generateCppCodeBlock(codeBlock *,classDef *,FILE *);
static void generateMacroCode(codeBlock *,classDef *,sipSpec *,char *,char *,
			      int,FILE *);
static void generateMacroFiles(sipSpec *,char,char *,char *,int,FILE *);
static void generateUsedIncludes(ifaceFileList *,FILE *);
static void generateIfaceHeader(sipSpec *,ifaceFileDef *,char *);
static void generateWrapperClassDeclaration(sipSpec *,classDef *,FILE *);
static void generateLimitedWrapperClassDeclaration(sipSpec *,classDef *,FILE *);
static void generateResultType(argDef *,FILE *);
static void generateCallTidyUp(funcArgs *,FILE *);
static void generateArgs(funcArgs *,funcArgType,FILE *);
static void generateVariable(argDef *,int,int,FILE *);
static void generateValueType(argDef *,FILE *);
static void generateSingleArg(argDef *,int,funcArgType,FILE *);
static void generateBaseType(argDef *,FILE *);
static void generateExpression(valueDef *,FILE *);
static void generateTupleBuilder(sipSpec *,funcArgs *,FILE *);
static void generateEmitter(sipSpec *,classDef *,visibleList *,FILE *);
static void generateVirtualHandlerDeclaration(overDef *,FILE *);
static void generateVirtualHandlers(sipSpec *,classDef *,FILE *);
static void generateVirtualHandler(sipSpec *,virtOverDef *,FILE *);
static void generateVirtualCatcher(classDef *,int,virtOverDef *,FILE *);
static void generateUnambiguousClass(classDef *,classDef *,FILE *);
static void generateProtectedEnums(sipSpec *,classDef *,FILE *);
static void generateProtectedDeclarations(sipSpec *,classDef *,FILE *);
static void generateProtectedDefinitions(sipSpec *,classDef *,FILE *);
static void generateArgCallList(funcArgs *,FILE *fp);
static void generateConstructorCall(sipSpec *,classDef *,ctorDef *,int,int,
				    FILE *);
static void generateCallDefaultCtor(ctorDef *,FILE *);
static void generateHandleResult(overDef *,int,FILE *);
static void generateOrdinaryFunction(sipSpec *,classDef *,memberDef *,FILE *);
static void generateSimpleFunctionCall(fcallDef *,FILE *);
static void generateFunctionCall(classDef *,classDef *,overDef *,int,FILE *);
static void generateCppFunctionCall(classDef *cd,classDef *ocd,overDef *od,
				    FILE *fp);
static void generateBinarySlotCall(overDef *,char *,FILE *);
static int isInplaceSlot(memberDef *);
static int isRichCompareSlot(memberDef *);
static void generateVariableHandler(sipSpec *,varDef *,FILE *);
static void generateObjToCppConversion(argDef *,char *,char *,FILE *);
static void generateVarClassConversion(sipSpec *,varDef *,FILE *);
static void generateVoidPointers(sipSpec *,classDef *,FILE *);
static void generateChars(sipSpec *,classDef *,FILE *);
static void generateStrings(sipSpec *,classDef *,FILE *);
static void generateEnums(sipSpec *,FILE *);
static sortedMethTab *createFunctionTable(classDef *,int *);
static sortedMethTab *createMethodTable(classDef *,int,int *);
static int generateMethodTable(classDef *,int,FILE *);
static int generateEnumTable(sipSpec *,classDef *,FILE *);
static void generateLongs(sipSpec *,classDef *,FILE *);
static void generateDoubles(sipSpec *,classDef *,FILE *);
static void generateClasses(sipSpec *,classDef *,FILE *);
static void generateLazyAttributes(classDef *,int,int,int,FILE *);
static void generateAccessFunctions(sipSpec *,FILE *);
static void generateConvertToDeclarations(mappedTypeDef *,classDef *,char *,
					  FILE *);
static void generateConvertToDefinitions(mappedTypeDef *,classDef *,FILE *);
static int generateArgParser(sipSpec *,funcArgs *,classDef *,overDef *,int,
			     FILE *);
static void generateTry(throwArgs *,FILE *);
static void generateCatch(throwArgs *,FILE *);
static void generateThrowSpecifier(throwArgs *,FILE *);
static void addNodeToParent(nodeDef *,classDef *);
static void positionClass(nodeDef *,classDef *);
static void addUniqueAPI(apiDef **,char *,funcArgs *);
static int countVirtuals(classDef *);
static int skipOverload(overDef *,memberDef *,classDef *,classDef *,int);
static int compareMethTab(const void *,const void *);
static int compareEnumTab(const void *,const void *);
static char *getSubFormatChar(int,argDef *);
static char *createIfaceFileName(char *,ifaceFileDef *,char *);
static FILE *createFile(sipSpec *,char *,char *,char *);
static void closeFile(FILE *);
static void prcode(FILE *,char *,...);
static void prScopedName(FILE *,scopedNameDef *,char *);
static void prTypeName(FILE *,argDef *,int);
static void prScopedPyClassName(FILE *,classDef *,classDef *);
static int needString(codeBlock *,const char *);
static int needErrorFlag(codeBlock *);
static int needWrapper(overDef *);
static int needCpp(overDef *);
static int needNewInstance(argDef *);
static void getCppSignature(overDef *,argDef **,funcArgs **);
static char getBuildResultFormat(argDef *);
static char getParseResultFormat(argDef *);
static void generateParseResultExtraArgs(argDef *,FILE *);
static char *makePartName(char *,char *,int,char *);


/*
 * Generate the code from a specification.
 */

void generateCode(sipSpec *pt,char *codeDir,char *buildfile,
		  stringList *makefiles,char *docFile,char *apiFile,
		  char *cppSuffix,int except,int trace,int parts,
		  stringList *xsl)
{
	exceptions = except;
	tracing = trace;

	/* Generate the documentation. */

	if (docFile != NULL)
		generateDocumentation(pt,docFile);

	/* Generate the code. */

	if (codeDir != NULL)
	{
		generatePackageHeader(pt,codeDir,xsl);
		generateCpp(pt,codeDir,cppSuffix,&parts);
		generatePython(pt,codeDir);
	}

	/* Generate the build file. */

	if (buildfile != NULL)
		generateBuildFile(pt,buildfile,cppSuffix,parts);

	/* Generate the Makefiles. */

	while (makefiles != NULL)
	{
		generateMakefile(pt,makefiles -> s,cppSuffix,parts);

		makefiles = makefiles -> next;
	}

	/* Generate the API file. */

	if (apiFile != NULL)
		generateAPI(pt,apiFile);
}


/*
 * Generate the module proxy header file.
 */

static void generateModuleProxyHeader(sipSpec *pt,char *codeDir,int parts)
{
	char *hfile, *mname = pt -> module -> name;
	FILE *fp;
	funcArgsList *fl;
	stringList *sl;

	if (parts)
		hfile = makePartName(codeDir,mname,0,".h");
	else
		hfile = concat(codeDir,"/",mname,"cmodule",".h",NULL);

	fp = createFile(pt,hfile,"//","Module proxy signal/slot handler class.");

	prcode(fp,
"\n"
"#ifndef _%sPROXY_H\n"
"#define	_%sPROXY_H\n"
"\n"
"#include <sipQt.h>\n"
		,mname,mname);

	/*
	 * Generate a list of the unique header code for each of the classes or
	 * namespaces used as a signal argument.
	 */

	sl = NULL;

	for (fl = pt -> sigargs; fl != NULL; fl = fl -> next)
	{
		int a;

		/* Go through each signal's arguments. */

		for (a = 0; a < fl -> fa -> nrArgs; ++a)
		{
			argDef *ad = &fl -> fa -> args[a];
			stringList *s;
			codeBlock *hdrcode;

			/* See if there is header code attached to the type. */

			switch (ad -> atype)
			{
			case class_type:
				hdrcode = ad -> u.cd -> hdrcode;
				break;

			case mapped_type:
				hdrcode = ad -> u.mtd -> hdrcode;
				break;

			case enum_type:
				if (ad -> u.ed -> ecd != NULL)
					hdrcode = ad -> u.ed -> ecd -> hdrcode;
				else
					hdrcode = NULL;

				break;

			default:
				hdrcode = NULL;
			}

			if (hdrcode == NULL)
				continue;

			/* See if we already have this code. */

			for (s = sl; s != NULL; s = s -> next)
				if (strcmp(s -> s,hdrcode -> frag) == 0)
					break;

			if (s == NULL)
			{
				s = sipMalloc(sizeof (stringList));

				s -> s = hdrcode -> frag;

				s -> next = sl;
				sl = s;
			}
		}
	}

	if (sl != NULL)
	{
		prcode(fp,"\n");

		while (sl != NULL)
		{
			stringList *next = sl -> next;

			prcode(fp,"%s",sl -> s);

			free(sl);
			sl = next;
		}
	}

	prcode(fp,
"\n"
"class sipProxy%s: public sipProxy\n"
"{\n"
"	Q_OBJECT\n"
"\n"
"public:\n"
"	char *getProxySlot(char *);\n"
"\n"
"public slots:\n"
		,mname);

	for (fl = pt -> sigargs; fl != NULL; fl = fl -> next)
	{
		prcode(fp,
"	void proxySlot(");

		generateArgs(fl -> fa,Declaration,fp);

		prcode(fp,");\n"
			);
	}

	prcode(fp,
"};\n"
"\n"
"#endif\n"
		);

	closeFile(fp);
	free(hfile);
}


/*
 * Generate the Scintilla API file.
 */

static void generateAPI(sipSpec *pt,char *apiFile)
{
	apiDef *head, *ad;
	overDef *od;
	classDef *cd;
	FILE *fp;

	/* Create the list of unique names/signatures. */
	head = NULL;

	for (od = pt -> overs; od != NULL; od = od -> next)
		addUniqueAPI(&head,od -> cppname,&od -> args);

	for (cd = pt -> classes; cd != NULL; cd = cd -> next)
	{
		ctorDef *ct;

		if (cd -> iff -> module != pt -> module)
			continue;

		for (ct = cd -> ctors; ct != NULL; ct = ct -> next)
		{
			if (isPrivateCtor(ct))
				continue;

			addUniqueAPI(&head,cd -> iff -> name -> text,&ct -> args);
		}

		for (od = cd -> overs; od != NULL; od = od -> next)
		{
			if (isPrivate(od))
				continue;

			addUniqueAPI(&head,od -> cppname,&od -> args);
		}
	}

	/* Generate the file. */
	fp = createFile(pt,apiFile,NULL,NULL);

	for (ad = head; ad != NULL; ad = ad -> next)
	{
		fprintf(fp,"%s(",ad -> name);
		generateArgs(ad -> args,Declaration,fp);
		fprintf(fp,")\n");
	}

	closeFile(fp);
}


/*
 * Add an API function to a list if it isn't already there.
 */

static void addUniqueAPI(apiDef **headp,char *name,funcArgs *args)
{
	apiDef *ad;

	for (ad = *headp; ad != NULL; ad = ad -> next)
	{
		if (strcmp(ad -> name,name) != 0)
			continue;

		if (sameFuncArgs(ad -> args,args,TRUE))
			break;
	}

	if (ad == NULL)
	{
		ad = sipMalloc(sizeof (apiDef));

		ad -> name = name;
		ad -> args = args;
		ad -> next = *headp;
		*headp = ad;
	}
}


/*
 * Generate the documentation.
 */

static void generateDocumentation(sipSpec *pt,char *docFile)
{
	FILE *fp;
	codeBlock *cb;

	fp = createFile(pt,docFile,NULL,NULL);

	for (cb = pt -> docs; cb != NULL; cb = cb -> next)
		fputs(cb -> frag,fp);

	closeFile(fp);
}


/*
 * Generate the build file.
 */

static void generateBuildFile(sipSpec *pt,char *buildFile,char *cppSuffix,
			      int parts)
{
	char *mname = pt -> module -> name;
	ifaceFileDef *iff;
	FILE *fp;

	fp = createFile(pt,buildFile,NULL,NULL);

	prcode(fp,"target = %s\nsources = ",mname);

	if (parts)
	{
		int p;

		for (p = 0; p < parts; ++p)
		{
			if (p > 0)
				prcode(fp," ");

			prcode(fp,"%spart%d%s",mname,p,cppSuffix);
		}
	}
	else
	{
		prcode(fp,"%scmodule%s",mname,cppSuffix);

		for (iff = pt -> ifacefiles; iff != NULL; iff = iff -> next)
		{
			if (iff -> type == namespace_iface)
				continue;

			if (iff -> module != pt -> module)
				continue;

			prcode(fp," sip%s%F%s",mname,iff -> fqname,cppSuffix);
		}
	}

	prcode(fp,"\nheaders = sip%sDecl%s.h",mname,mname);

	for (iff = pt -> ifacefiles; iff != NULL; iff = iff -> next)
	{
		char *imname;

		if (iff -> type == namespace_iface)
			continue;

		if (!isUsed(iff))
			continue;

		imname = (iff -> module == pt -> module ? mname : iff -> module -> name);

		prcode(fp," sip%s%F.h",imname,iff -> fqname);
	}

	prcode(fp,"\n");

	if (pt -> sigargs != NULL)
		prcode(fp,"moc_headers = %s%s.h\n",mname,(parts ? "part0" : "cmodule"));

	closeFile(fp);
}


/*
 * Generate the code Makefile.
 */

static void generateMakefile(sipSpec *pt,char *makeFile,char *cppSuffix,
			     int parts)
{
	char *tn;
	FILE *fp;
	codeBlock *cb;
	mkTemplateDef *mtd;

	/* Find the template. */

	if ((tn = strrchr(makeFile,'/')) != NULL)
		++tn;
	else
		tn = makeFile;

	for (mtd = pt -> mktemplates; mtd != NULL; mtd = mtd -> next)
		if (strcmp(mtd -> name,tn) == 0)
			break;

	if (mtd == NULL)
		fatal("Unknown Makefile template: %s\n",tn);

	fp = createFile(pt,makeFile,NULL,NULL);

	for (cb = mtd -> templ; cb != NULL; cb = cb -> next)
		generateMacroCode(cb,NULL,pt,cppSuffix,mtd -> objext,parts,fp);

	closeFile(fp);
}


/*
 * Generate the Python wrapper code.
 */

static void generatePython(sipSpec *pt,char *codeDir)
{
	char *pyfile, *mname = pt -> module -> name;
	FILE *fp;
	codeBlock *cb;
	classDef *cd;
	classList *cl, *impcdl;

	pyfile = concat(codeDir,"/",mname,".py",NULL);
	fp = createFile(pt,pyfile,"#","Python wrapper code.");

	prcode(fp,
"\n"
"import libsip\n"
		);

	/*
	 * Go through all the classes defined in this module and build up a
	 * list of all the super classes needed from other modules.
	 */

	impcdl = NULL;

	for (cd = pt -> classes; cd != NULL; cd = cd -> next)
	{
		classList *cl;

		if (cd -> iff -> module != pt -> module)
			continue;

		/* Check the super-classes for any not in this module. */

		for (cl = cd -> supers; cl != NULL; cl = cl -> next)
		{
			classList *impcd;
			classDef *need;

			if (cl -> cd -> iff -> module == pt -> module)
				continue;

			/* We import the outermost scope. */

			for (need = cl -> cd; need -> ecd != NULL; need = need -> ecd)
				;

			/* Check it's not already in the list. */

			for (impcd = impcdl; impcd != NULL; impcd = impcd -> next)
				if (impcd -> cd == need)
					break;

			if (impcd == NULL)
			{
				impcd = sipMalloc(sizeof (classList));

				impcd -> cd = need;

				impcd -> next = impcdl;
				impcdl = impcd;
			}
		}
	}

	/* Generate the imports of any classes we need. */

	if (impcdl != NULL)
		prcode(fp,
"\n"
			);

	for (cl = impcdl; cl != NULL; cl = cl -> next)
		prcode(fp,
"from %s import %s\n"
			,cl -> cd -> iff -> module -> name,classBaseName(cl -> cd));

	/* Generate the pre-Python code blocks. */

	for (cb = pt -> prepycode; cb != NULL; cb = cb -> next)
		prcode(fp,"\n%s",cb -> frag);

	/*
	 * Finally import our own C++ module, or the consolidated C++ module.
	 */

	if (pt -> cppmname != mname)
		prcode(fp,
"import lib%sc as lib%sc\n"
			,pt -> cppmname,mname);
	else
		prcode(fp,
"import lib%sc\n"
			,mname);

	prcode(fp,
"\n"
"lib%sc.sipInitModule(__name__)\n"
		,mname);

	prcode(fp,
"\n"
		);

	/* Generate the Python class objects. */

	generatePythonClassWrappers(pt,fp);

	/* Register the classes. */

	prcode(fp,
"# Register the classes with the C++ module.\n"
"\n"
"lib%sc.sipRegisterClasses()\n"
		,mname);

	/* Generate the Python code blocks. */

	for (cb = pt -> pycode; cb != NULL; cb = cb -> next)
		prcode(fp,"\n%s",cb -> frag);

	closeFile(fp);
	free(pyfile);
}


/*
 * Make sure a class is in the namespace tree.
 */

static void addNodeToParent(nodeDef *root,classDef *cd)
{
	nodeDef *nd, *parent;

	/* Skip classes already in the tree. */

	if (cd -> node != NULL)
		return;

	/* Add this child to the parent. */

	nd = sipMalloc(sizeof (nodeDef));

	nd -> ordered = FALSE;
	nd -> cd = cd;
	nd -> child = NULL;

	/* Get the address of the parent node. */

	if (cd -> ecd == NULL)
		parent = root;
	else
	{
		/* Make sure the parent is in the tree. */

		addNodeToParent(root,cd -> ecd);

		parent = cd -> ecd -> node;
	}

	nd -> parent = parent;

	/* Insert this at the head of the parent's children. */

	nd -> next = parent -> child;
	parent -> child = nd;

	/* Remember where we are in the tree. */

	cd -> node = nd;
}


/*
 * Generate a Python class wrapper and all its children.
 */

static void generatePythonClassTree(sipSpec *pt,nodeDef *nd,int indent,FILE *fp)
{
	nodeDef *cnd;

	/* Generate the class if it's not the root. */

	if (nd -> cd != NULL)
		generatePythonClass(pt,nd -> cd,indent,fp);

	/* Generate all it's children. */

	for (cnd = nd -> child; cnd != NULL; cnd = cnd -> next)
		generatePythonClassTree(pt,cnd,indent + 1,fp);
}


/*
 * Generate the Python class objects.
 */

static void generatePythonClassWrappers(sipSpec *pt,FILE *fp)
{
	classDef *cd;
	nodeDef root;

	/* Build up a tree of classes reflecting the namespace structure. */

	root.cd = NULL;
	root.child = NULL;

	for (cd = pt -> classes; cd != NULL; cd = cd -> next)
		if (cd -> iff -> module == pt -> module)
			addNodeToParent(&root,cd);

	/* Order the tree so that a class's super-classes are before it. */

	for (cd = pt -> classes; cd != NULL; cd = cd -> next)
		if (cd -> node != NULL)
			positionClass(&root,cd);

	/* Generate the classes from the ordered tree. */

	generatePythonClassTree(pt,&root,-1,fp);
}


/*
 * Position a single class from this module in the tree.
 */
static void positionClass(nodeDef *root,classDef *cd)
{
	classList *cl;

	/* See if it has already been done. */
	if (cd -> node -> ordered)
		return;

	for (cl = cd -> supers; cl != NULL; cl = cl -> next)
	{
		nodeDef **ndp, *nd1, *nd2;

		/* Ignore super-classes from different modules. */

		if (cl -> cd -> node == NULL)
			continue;

		/* Make sure the super-class is positioned. */

		positionClass(root,cl -> cd);

		/*
		 * Find ancestors of the two that are siblings (ie. they have a
		 * common parent).
		 */

		for (nd1 = cd -> node; nd1 != root; nd1 = nd1 -> parent)
		{
			for (nd2 = cl -> cd -> node; nd2 != root; nd2 = nd2 -> parent)
				if (nd1 -> parent == nd2 -> parent)
					break;

			if (nd2 != root)
				break;
		}

		/*
		 * The first node must appear after the second in the common
		 * parent's list of children.
		 */

		for (ndp = &nd1 -> parent -> child; *ndp != NULL; ndp = &(*ndp) -> next)
		{
			nodeDef *nd = *ndp;

			if (nd == nd2)
				break;

			if (nd == nd1)
			{
				/* Remove this one from the list. */

				*ndp = nd -> next;

				/* Find the super-class ancestor. */

				while (*ndp != nd2)
					ndp = &(*ndp) -> next;

				/*
				 * Put this one back after the super-class
				 * ancestor.
				 */
				nd -> next = (*ndp) -> next;
				(*ndp) -> next = nd;

				break;
			}
		}
	}

	cd -> node -> ordered = TRUE;
}


/*
 * Generate an expression in C++.
 */

static void generateExpression(valueDef *vd,FILE *fp)
{
	while (vd != NULL)
	{
		if (vd -> vunop != '\0')
			prcode(fp,"%c",vd -> vunop);

		switch (vd -> vtype)
		{
		case qchar_value:
			prcode(fp,"'%c'",vd -> u.vqchar);
			break;

		case string_value:
			prcode(fp,"\"%s\"",vd -> u.vstr);
			break;

		case numeric_value:
			prcode(fp,"%l",vd -> u.vnum);
			break;

		case real_value:
			prcode(fp,"%g",vd -> u.vreal);
			break;

		case scoped_value:
			prcode(fp,"%S",vd -> u.vscp);
			break;

		case fcall_value:
			generateSimpleFunctionCall(vd -> u.fcd,fp);
			break;
		}
 
		if (vd -> vbinop != '\0')
			prcode(fp," %c ",vd -> vbinop);
 
		vd = vd -> next;
	}
}


/*
 * Generate the wrapper for a Python class.
 */

static void generatePythonClass(sipSpec *pt,classDef *cd,int indent,FILE *fp)
{
	char *mname = pt -> module -> name;
	int noIntro;
	classList *cl;
	varDef *vd;

	prcode(fp,
"%Iclass %s",indent,classBaseName(cd));

	/* Inherit from all immediate super-classes. */

	noIntro = TRUE;

	for (cl = cd -> supers; cl != NULL; cl = cl -> next)
	{
		if (noIntro)
		{
			prcode(fp,"(");
			noIntro = FALSE;
		}
		else
			prcode(fp,",");

		prScopedPyClassName(fp,cd -> ecd,cl -> cd);
	}

	if (!noIntro)
		prcode(fp,")");

	prcode(fp,":\n");

	/* Generate the standard methods. */

	prcode(fp,
"%I	def __init__(self,*args):\n"
"%I		lib%sc.sipCallCtor(%d,self,args)\n"
		,indent
		,indent,mname,cd -> classnr);

	for (vd = pt -> vars; vd != NULL; vd = vd -> next)
		if (vd -> ecd == cd && !isStaticVar(vd))
		{
			prcode(fp,
"%I	def __getattr__(self,name):\n"
"%I		return lib%sc.sipCallGetVar(%d,self,name)\n"
"%I	def __setattr__(self,name,value):\n"
"%I		return lib%sc.sipCallSetVar(%d,self,name,value)\n"
				,indent
				,indent,mname,cd -> classnr
				,indent
				,indent,mname,cd -> classnr);

			break;
		}

	prcode(fp,
"\n"
		);
}


/*
 * Generate the C++ package specific header file.
 */

static void generatePackageHeader(sipSpec *pt,char *codeDir,stringList *xsl)
{
	char *hfile, *mname = pt -> module -> name;
	FILE *fp;
	classDef *cd;
	nameDef *nd;
	moduleDef *mod;

	hfile = concat(codeDir,"/sip",mname,"Decl",mname,".h",NULL);
	fp = createFile(pt,hfile,"//","C++ module API header file.");

	/* Include files. */

	prcode(fp,
"\n"
"#ifndef _%sEXPORTS_H\n"
"#define	_%sEXPORTS_H\n"
"\n"
"#ifdef HAVE_CONFIG_H\n"
"#include \"config.h\"\n"
"#endif\n"
"\n"
"#include <sip%s.h>\n"
"\n"
		,mname,mname,(pt -> sigslots ? "Qt" : ""));

	/* Defined the enabled features. */

	for (mod = pt -> moduleList; mod != NULL; mod = mod -> next)
	{
		qualDef *qd;

		for (qd = mod -> qualifiers; qd != NULL; qd = qd -> next)
			if (qd -> qtype == feature_qualifier && !excludedFeature(xsl,qd))
				prcode(fp,
"#define	SIP_FEATURE_%s\n"
					,qd -> name);
	}

	generateCppCodeBlock(pt -> exphdrcode,NULL,fp);
	generateCppCodeBlock(pt -> hdrcode,NULL,fp);


	/* The class object declarations (for all modules). */

	for (cd = pt -> classes; cd != NULL; cd = cd -> next)
	{
		if (cd -> iff -> type == namespace_iface)
			continue;

		prcode(fp,
"extern SIP_%s PyObject *sipClass_%C;\n"
			,(cd -> iff -> module == pt -> module) ? "MODULE_EXTERN" : "IMPORT"
			,classFQName(cd));
	}

	/* The names declarations (for all modules). */

	prcode(fp,
"\n"
		);

	for (nd = pt -> namecache; nd != NULL; nd = nd -> next)
		prcode(fp,
"extern SIP_%s char %N[];\n"
			,(nd -> module == pt -> module) ? "MODULE_EXTERN" : "IMPORT"
			,nd);

	if (pt -> sigargs != NULL)
		prcode(fp,
"\n"
"sipProxy *sipNewProxy_%s Py_PROTO(());\n"
			,mname);

	prcode(fp,
"\n"
"#endif\n"
		);

	closeFile(fp);
	free(hfile);
}


/*
 * Return the filename of a source code part on the heap.
 */

static char *makePartName(char *codeDir,char *mname,int part,char *cppSuffix)
{
	char buf[20];

	sprintf(buf,"part%d",part);

	return concat(codeDir,"/",mname,buf,cppSuffix,NULL);
}


/*
 * Generate the C++ code.
 */

static void generateCpp(sipSpec *pt,char *codeDir,char *cppSuffix,int *parts)
{
	char *cppfile, *mname = pt -> module -> name;
	int nrclasses, noIntro, files_in_part, max_per_part, this_part;
	FILE *fp;
	classDef *cd;
	nameDef *nd;
	memberDef *md;
	expFuncDef *ef;
	ifaceFileDef *iff;

	/* Calculate the number of files in each part. */

	if (*parts)
	{
		int nr_files = 1;

		for (iff = pt -> ifacefiles; iff != NULL; iff = iff -> next)
			if (iff -> type != namespace_iface && iff -> module == pt -> module)
				++nr_files;

		max_per_part = (nr_files + *parts - 1) / *parts;
		files_in_part = 1;
		this_part = 0;

		cppfile = makePartName(codeDir,mname,0,cppSuffix);
	}
	else
		cppfile = concat(codeDir,"/",mname,"cmodule",cppSuffix,NULL);

	fp = createFile(pt,cppfile,"//","Module code.");

	prcode(fp,
"\n"
"#include \"sip%sDecl%s.h\"\n"
		,mname,mname);

	generateUsedIncludes(pt -> used,fp);

	/* The names definitions. */

	for (nd = pt -> namecache; nd != NULL; nd = nd -> next)
		if (nd -> module == pt -> module)
			prcode(fp,
"char %N[] = \"%s\";\n"
				,nd,nd -> text);

	/* The class objects for any namespaces. */

	noIntro = TRUE;

	for (cd = pt -> classes; cd != NULL; cd = cd -> next)
	{
		int nr_funcs, nr_enums;

		if (cd -> iff -> module != pt -> module)
			continue;

		if (cd -> iff -> type != namespace_iface)
			continue;

		if (noIntro)
		{
			prcode(fp,
"\n"
"// Class objects for namespaces.\n"
"\n"
				);

			noIntro = FALSE;
		}

		prcode(fp,
"static PyObject *sipClass_%C;\n"
			,classFQName(cd));

		for (md = cd -> members; md != NULL; md = md -> next)
			generateOrdinaryFunction(pt,cd,md,fp);

		nr_funcs = generateMethodTable(cd,FALSE,fp);
		nr_enums = generateEnumTable(pt,cd,fp);

		generateLazyAttributes(cd,0,nr_funcs,nr_enums,fp);
	}

	/* Generate the module proxy. */

	if (pt -> sigargs != NULL)
	{
		generateModuleProxy(pt,*parts,fp);

		prcode(fp,
"\n"
"sipProxy *sipNewProxy_%s()\n"
"{\n"
"	return new sipProxy%s();\n"
"}\n"
			,mname,mname);
	}

	/* Generate the C++ code blocks. */

	generateCppCodeBlock(pt -> cppcode,NULL,fp);

	/* Generate the global functions. */

	for (md = pt -> othfuncs; md != NULL; md = md -> next)
		if (md -> module == pt -> module)
			generateOrdinaryFunction(pt,NULL,md,fp);

	/* Generate the access functions. */

	generateAccessFunctions(pt,fp);

	/* Generate the module data structures. */

	prcode(fp,
"\n"
"static sipClassDef classesTable[] = {\n"
		);

	nrclasses = 0;

	for (cd = pt -> classes; cd != NULL; cd = cd -> next)
	{
		if (cd -> iff -> module != pt -> module)
			continue;

		++nrclasses;

		prcode(fp,
"	{%N, "
			,cd -> iff -> name);

		if (cd -> iff -> type == namespace_iface)
		{
			prcode(fp,"NULL, &sipClass_%C, ",classFQName(cd));

			if (cd -> members != NULL || hasEnums(cd))
				prcode(fp,"&sipClassAttrTab_%C",classFQName(cd));
			else
				prcode(fp,"NULL");

			prcode(fp,", NULL,");
		}
		else
		{
			varDef *vd;

			prcode(fp,"sipNew_%C, &sipClass_%C, &sipClassAttrTab_%C,"
				,classFQName(cd)
				,classFQName(cd)
				,classFQName(cd));

			for (vd = pt -> vars; vd != NULL; vd = vd -> next)
			{
				if (vd -> ecd != cd)
					continue;

				if (!isStaticVar(vd))
					break;
			}

			if (vd != NULL)
				prcode(fp," sipClassVarHierTab_%C,"
					,classFQName(cd));
			else
				prcode(fp," NULL,");
		}

		prcode(fp," %d},\n"
			,cd -> ecd != NULL ? cd -> ecd -> classnr : -1);
	}

	prcode(fp,
"};\n"
"\n"
"static sipModuleDef sipModule = {\n"
"	%d,\n"
"	classesTable\n"
"};\n"
		,nrclasses);


	/* Generate the call class ctor function. */

	prcode(fp,
"\n"
"// The entry point into the bindings for constructors.\n"
"\n"
"static PyObject *callCtor(PyObject *,PyObject *args)\n"
"{\n"
"	return sipCallCtor(&sipModule,args);\n"
"}\n"
		);

	/* Generate the get and set class attribute functions. */

	prcode(fp,
"\n"
"// The entry point into the bindings for getting class variables.\n"
"\n"
"static PyObject *callGetVar(PyObject *,PyObject *args)\n"
"{\n"
"	return sipGetVar(&sipModule,args);\n"
"}\n"
"\n"
"// The entry point into the bindings for setting class variables.\n"
"\n"
"static PyObject *callSetVar(PyObject *,PyObject *args)\n"
"{\n"
"	return sipSetVar(&sipModule,args);\n"
"}\n"
		);

	/* Generate the register Python globals function. */

	prcode(fp,
"\n"
"// Initialise the module.\n"
"\n"
"static PyObject *initModule(PyObject *,PyObject *args)\n"
"{\n"
"	if (sipRegisterModule(&sipModule,args) < 0)\n"
"		return NULL;\n"
		);

	/* Generate the global functions. */

	noIntro = TRUE;

	for (md = pt -> othfuncs; md != NULL; md = md -> next)
		if (md -> module == pt -> module)
		{
			if (noIntro)
			{
				prcode(fp,
"\n"
"	// Add the global functions to the dictionary.\n"
"\n"
"	static PyMethodDef globfuncs[] = {\n"
					);

				noIntro = FALSE;
			}

			prcode(fp,
"		{%N, sipDo_%s, METH_VARARGS, NULL},\n"
				,md -> pyname,md -> pyname -> text);
		}

	if (!noIntro)
		prcode(fp,
"		{NULL}\n"
"	};\n"
"\n"
"	if (sipAddFunctions(sipModule.md_dict,globfuncs) < 0)\n"
"		return NULL;\n"
			);

	/* Generate the global variables. */

	generateVoidPointers(pt,NULL,fp);
	generateChars(pt,NULL,fp);
	generateStrings(pt,NULL,fp);
	generateLongs(pt,NULL,fp);
	generateDoubles(pt,NULL,fp);
	generateEnums(pt,fp);

	/* Generate any license information. */

	if (pt -> module -> license != NULL)
	{
		licenseDef *ld = pt -> module -> license;

		prcode(fp,
"\n"
"	static sipLicenseDef mod_license = {\n"
			);

		prcode(fp,
"		\"%s\",\n"
			,ld -> type);

		if (ld -> licensee != NULL)
			prcode(fp,
"		\"%s\",\n"
				,ld -> licensee);
		else
			prcode(fp,
"		NULL,\n"
				);

		if (ld -> timestamp != NULL)
			prcode(fp,
"		\"%s\",\n"
				,ld -> timestamp);
		else
			prcode(fp,
"		NULL,\n"
				);

		if (ld -> sig != NULL)
			prcode(fp,
"		\"%s\"\n"
				,ld -> sig);
		else
			prcode(fp,
"		NULL\n"
				);

		prcode(fp,
"	};\n"
"\n"
"	if (sipAddLicense(sipModule.md_dict,&mod_license) < 0)\n"
"		return NULL;\n"
			);
	}

	prcode(fp,
"\n"
"	Py_INCREF(Py_None);\n"
"	return Py_None;\n"
"}\n"
		);

	/* Generate the sub-class convertors. */

	for (cd = pt -> classes; cd != NULL; cd = cd -> next)
	{
		if (cd -> iff -> module != pt -> module)
			continue;

		if (cd -> convtosubcode == NULL)
			continue;

		prcode(fp,
"\n"
"// Convert to a sub-class if possible.\n"
"\n"
"static PyObject *sipSubClass_%C(const %S *sipCpp)\n"
"{\n"
"	PyObject *sipClass;\n"
"\n"
			,classFQName(cd),classFQName(cd -> subbase));

		generateCppCodeBlock(cd -> convtosubcode,cd,fp);

		prcode(fp,
"\n"
"	return sipClass;\n"
"}\n"
			);
	}

	/* Generate the register Python classes function. */

	prcode(fp,
"\n"
"// Final stage after the Python classes have been created.\n"
"\n"
"static PyObject *registerClasses(PyObject *,PyObject *)\n"
"{\n"
"	if (sipRegisterClasses(&sipModule,%d) < 0)\n"
"		return NULL;\n"
		,pt -> qobjclass);

	/* Register any sub-class convertors. */

	noIntro = TRUE;

	for (cd = pt -> classes; cd != NULL; cd = cd -> next)
	{
		if (cd -> iff -> module != pt -> module)
			continue;

		if (cd -> convtosubcode == NULL)
			continue;

		if (noIntro)
		{
			prcode(fp,
"\n"
"	/* Register the module's sub-class convertors. */\n"
"\n"
				);

			noIntro = FALSE;
		}

		prcode(fp,
"	sipRegisterSubClassConvertor(sipClass_%C,(PyObject *(*)(const void *))sipSubClass_%C);\n"
			,classFQName(cd -> subbase),classFQName(cd));
	}

	/* Add the global class instances. */

	generateClasses(pt,NULL,fp);

	/* Generate all static class variables. */

	for (cd = pt -> classes; cd != NULL; cd = cd -> next)
	{
		if (cd -> iff -> module != pt -> module)
			continue;

		generateVoidPointers(pt,cd,fp);
		generateChars(pt,cd,fp);
		generateStrings(pt,cd,fp);
		generateLongs(pt,cd,fp);
		generateDoubles(pt,cd,fp);
		generateClasses(pt,cd,fp);
	}

	/* Generate the post-initialisation C++ code. */

	generateCppCodeBlock(pt -> postinitcode,NULL,fp);

	prcode(fp,
"\n"
"	Py_INCREF(Py_None);\n"
"	return Py_None;\n"
"}\n"
		);

	/* Generate the Python module initialisation function. */

	prcode(fp,
"\n"
"// The Python module initialisation function.\n"
"\n"
"extern \"C\" SIP_EXPORT void initlib%sc()\n"
"{\n"
"	static PyMethodDef methods[] = {\n"
"		{\"sipCallCtor\", callCtor, METH_VARARGS, NULL},\n"
"		{\"sipCallGetVar\", callGetVar, METH_VARARGS, NULL},\n"
"		{\"sipCallSetVar\", callSetVar, METH_VARARGS, NULL},\n"
"		{\"sipInitModule\", initModule, METH_VARARGS, NULL},\n"
"		{\"sipRegisterClasses\", registerClasses, METH_VARARGS, NULL},\n"
		,pt -> module -> name);

	for (ef = pt -> exposed; ef != NULL; ef = ef -> next)
		prcode(fp,
"		{\"%s\", %s, METH_VARARGS, NULL},\n"
			,ef -> name,ef -> name);

	prcode(fp,
"		{NULL}\n"
"	};\n"
"\n"
"	// Initialise the module.\n"
"\n"
"	Py_InitModule(\"lib%sc\",methods);\n"
"}\n"
		,pt -> module -> name);

	/* Generate any module proxy header file. */

	if (pt -> sigargs != NULL)
		generateModuleProxyHeader(pt,codeDir,*parts);

	/* Generate the interface source and header files. */

	for (iff = pt -> ifacefiles; iff != NULL; iff = iff -> next)
		if (iff -> type != namespace_iface)
		{
			if (iff -> module == pt -> module)
			{
				if (*parts && files_in_part++ == max_per_part)
				{
					/* Close the old part. */
					closeFile(fp);
					free(cppfile);

					/* Create a new one. */
					files_in_part = 1;
					++this_part;

					cppfile = makePartName(codeDir,mname,this_part,cppSuffix);
					fp = createFile(pt,cppfile,"//","Module code.");
				}

				generateIfaceCpp(pt,iff,codeDir,cppSuffix,(*parts ? fp : NULL));
			}

			if (isUsed(iff))
				generateIfaceHeader(pt,iff,codeDir);
		}

	closeFile(fp);
	free(cppfile);

	/* How many parts we actually generated. */
	if (*parts)
		*parts = this_part + 1;
}


/*
 * Generate an ordinary function (ie. not a class method).
 */

static void generateOrdinaryFunction(sipSpec *pt,classDef *cd,memberDef *md,
				     FILE *fp)
{
	overDef *od;

	prcode(fp,
"\n"
		);

	if (cd != NULL)
	{
		prcode(fp,
"static PyObject *sipDo_%C_%s(PyObject *,PyObject *sipArgs)\n"
			,classFQName(cd),md -> pyname -> text);

		od = cd -> overs;
	}
	else
	{
		prcode(fp,
"static PyObject *sipDo_%s(PyObject *,PyObject *sipArgs)\n"
			,md -> pyname -> text);

		od = pt -> overs;
	}

	prcode(fp,
"{\n"
"	int sipArgsParsed = 0;\n"
		);

	while (od != NULL)
	{
		if (od -> common == md)
			generateFunctionBody(pt,od,cd,cd,fp);

		od = od -> next;
	}

	prcode(fp,
"\n"
"	// Report an error if the arguments couldn't be parsed.\n"
"\n"
"	sipNoFunction(sipArgsParsed,%N);\n"
"\n"
"	return NULL;\n"
"}\n"
		,md -> pyname);
}


/*
 * Generate the table of enum instances.  Return the number of them.
 */

static int generateEnumTable(sipSpec *pt,classDef *cd,FILE *fp)
{
	int i, indent, nr_enums;
	enumDef *ed;
	enumValueDef **etab, **et;

	/* First we count how many. */

	nr_enums = 0;

	for (ed = pt -> enums; ed != NULL; ed = ed -> next)
	{
		enumValueDef *evd;

		if (ed -> ecd != cd || ed -> module != pt -> module)
			continue;

		for (evd = ed -> values; evd != NULL; evd = evd -> next)
			++nr_enums;
	}

	if (nr_enums == 0)
		return 0;

	/* Create a table so they can be sorted. */

	etab = sipMalloc(sizeof (enumValueDef *) * nr_enums);

	et = etab;

	for (ed = pt -> enums; ed != NULL; ed = ed -> next)
	{
		enumValueDef *evd;

		if (ed -> ecd != cd || ed -> module != pt -> module)
			continue;

		for (evd = ed -> values; evd != NULL; evd = evd -> next)
			*et++ = evd;
	}

	qsort(etab,nr_enums,sizeof (enumValueDef *),compareEnumTab);

	/* Now generate the table. */

	if (cd != NULL)
		prcode(fp,
"\n"
"static sipEnumValueInstanceDef lazyEnums_%C[] = {\n"
			,classFQName(cd));
	else
		prcode(fp,
"\n"
"	// Add the enums to the module dictionary.\n"
"\n"
"	static sipEnumValueInstanceDef enumValues[] = {\n"
		);

	indent = (cd == NULL) ? 1 : 0;

	for (i = 0; i < nr_enums; ++i)
	{
		enumValueDef *evd;

		evd = etab[i];

		prcode(fp,
"%I	{%N, ",indent,evd -> name);

		if (cd != NULL)
			prcode(fp,"%s%S::%s}%s\n",(isProtectedEnum(evd -> ed) ? "sip" : ""),classFQName(cd),evd -> name -> text,((i + 1 < nr_enums) ? "," : ""));
		else
			prcode(fp,"%s},\n"
				,evd -> name -> text);
	}

	if (cd == NULL)
		prcode(fp,
"		{NULL}\n"
			);

	prcode(fp,
"%I};\n"
		,indent);

	return nr_enums;
}


/*
 * The qsort helper to compare two enumValueDef structures based on the name
 * name of the enum value.
 */

static int compareEnumTab(const void *m1,const void *m2)
{
	return strcmp((*(enumValueDef **)m1) -> name -> text,
		      (*(enumValueDef **)m2) -> name -> text);
}


/*
 * Generate, if required, the structure pointing to the different lazy
 * attribute tables.
 */

static void generateLazyAttributes(classDef *cd,int nr_slmethods,
				   int nr_methods,int nr_enums,FILE *fp)
{
	prcode(fp,
"\n"
		);

	if (cd -> iff -> type == namespace_iface)
		prcode(fp,"static ");

	prcode(fp,
"sipLazyAttrDef sipClassAttrTab_%C = {\n"
		,classFQName(cd));

	if (nr_slmethods == 0)
		prcode(fp,
"	0, NULL,\n"
			);
	else
		prcode(fp,
"	sizeof (slotMethods_%C) / sizeof (PyMethodDef), slotMethods_%C,\n"
			,classFQName(cd),classFQName(cd));

	if (nr_methods == 0)
		prcode(fp,
"	0, NULL,\n"
			);
	else
		prcode(fp,
"	sizeof (lazyMethods_%C) / sizeof (PyMethodDef), lazyMethods_%C,\n"
			,classFQName(cd),classFQName(cd));

	if (nr_enums == 0)
		prcode(fp,
"	0, NULL\n"
			);
	else
		prcode(fp,
"	sizeof (lazyEnums_%C) / sizeof (sipEnumValueInstanceDef), lazyEnums_%C\n"
			,classFQName(cd),classFQName(cd));

	prcode(fp,
"};\n"
		);
}


/*
 * Generate the access functions for the variables.
 */

static void generateAccessFunctions(sipSpec *pt,FILE *fp)
{
	varDef *vd;

	for (vd = pt -> vars; vd != NULL; vd = vd -> next)
	{
		if (vd -> module != pt -> module || vd -> accessfunc == NULL)
			continue;

		if (vd -> ecd != NULL && !isStaticVar(vd))
			continue;

		prcode(fp,
"\n"
			);

		prcode(fp,
"// Access function.\n"
"\n"
"static const void *sipAccess_%C()\n"
"{\n"
			,vd -> fqname);

		generateCppCodeBlock(vd -> accessfunc,NULL,fp);

		prcode(fp,
"}\n"
			);
	}
}


/*
 * Generate the code to add a set of class instances to a dictionary.
 */

static void generateClasses(sipSpec *pt,classDef *cd,FILE *fp)
{
	int noIntro;
	varDef *vd;

	noIntro = TRUE;

	for (vd = pt -> vars; vd != NULL; vd = vd -> next)
	{
		scopedNameDef *vcname;

		if (vd -> ecd != cd || vd -> module != pt -> module)
			continue;

		if (vd -> type.atype != class_type)
			continue;

		if (cd != NULL && !isStaticVar(vd))
			continue;

		if (noIntro)
		{
			if (cd != NULL)
			{
				prcode(fp,
"\n"
					);

				prcode(fp,
"	// Add the class instances to the class dictionary.\n"
"\n"
"	static sipClassInstanceDef %C_classInstances[] = {\n"
					,classFQName(cd));
			}
			else
				prcode(fp,
"\n"
"	// Add the class instances to the module dictionary.\n"
"\n"
"	static sipClassInstanceDef classInstances[] = {\n"
					);

			noIntro = FALSE;
		}

		vcname = classFQName(vd -> type.u.cd);

		if (vd -> accessfunc != NULL)
		{
			prcode(fp,
"		{%N, sipAccess_%C, sipClass_%C, SIP_SIMPLE | SIP_ACCFUNC},\n"
				,vd -> name,vd -> fqname,vcname);
		}
		else if (vd -> type.nrderefs != 0)
		{
			prcode(fp,
"		{%N, &%S, sipClass_%C, SIP_SIMPLE | SIP_INDIRECT},\n"
				,vd -> name,vd -> fqname,vcname);
		}
		else
		{
			prcode(fp,
"		{%N, &%S, sipClass_%C, SIP_SIMPLE},\n"
				,vd -> name,vd -> fqname,vcname);
		}
	}

	if (!noIntro)
	{
		prcode(fp,
"		{NULL}\n"
"	};\n"
"\n"
"	if (sipAddClassInstances(");

		if (cd != NULL)
			prcode(fp,"((PyClassObject *)sipClass_%C) -> cl_dict,%C_",classFQName(cd),classFQName(cd));
		else
			prcode(fp,"sipModule.md_dict,");

		prcode(fp,"classInstances) < 0)\n"
"		return NULL;\n"
			);
	}
}


/*
 * Generate the code to add a set of void pointers to a dictionary.
 */

static void generateVoidPointers(sipSpec *pt,classDef *cd,FILE *fp)
{
	int noIntro;
	varDef *vd;

	noIntro = TRUE;

	for (vd = pt -> vars; vd != NULL; vd = vd -> next)
	{
		if (vd -> ecd != cd || vd -> module != pt -> module)
			continue;

		if (vd -> type.atype != voidptr_type && vd -> type.atype != struct_type)
			continue;

		if (cd != NULL && !isStaticVar(vd))
			continue;

		if (noIntro)
		{
			if (cd != NULL)
				prcode(fp,
"\n"
"	// Add the void pointers to the class dictionary.\n"
"\n"
"	static sipVoidPtrInstanceDef %C_voidPtrInstances[] = {\n"
					,classFQName(cd));
			else
				prcode(fp,
"\n"
"	// Add the void pointers to the module dictionary.\n"
"\n"
"	static sipVoidPtrInstanceDef voidPtrInstances[] = {\n"
					);

			noIntro = FALSE;
		}

		prcode(fp,
"		{%N, %S},\n"
			,vd -> name,vd -> fqname);
	}

	if (!noIntro)
	{
		prcode(fp,
"		{NULL}\n"
"	};\n"
"\n"
"	if (sipAddVoidPtrInstances(");

		if (cd != NULL)
			prcode(fp,"((PyClassObject *)sipClass_%C) -> cl_dict,%C_",classFQName(cd),classFQName(cd));
		else
			prcode(fp,"sipModule.md_dict,");

		prcode(fp,"voidPtrInstances) < 0)\n"
"		return NULL;\n"
			);
	}
}


/*
 * Generate the code to add a set of characters to a dictionary.
 */

static void generateChars(sipSpec *pt,classDef *cd,FILE *fp)
{
	int noIntro;
	varDef *vd;

	noIntro = TRUE;

	for (vd = pt -> vars; vd != NULL; vd = vd -> next)
	{
		argType vtype = vd -> type.atype;

		if (vd -> ecd != cd || vd -> module != pt -> module)
			continue;

		if (!((vtype == ustring_type || vtype == string_type) && vd -> type.nrderefs == 0))
			continue;

		if (cd != NULL && !isStaticVar(vd))
			continue;

		if (noIntro)
		{
			if (cd != NULL)
				prcode(fp,
"\n"
"	// Add the chars to the class dictionary.\n"
"\n"
"	static sipCharInstanceDef %C_charInstances[] = {\n"
					,classFQName(cd));
			else
				prcode(fp,
"\n"
"	// Add the chars to the module dictionary.\n"
"\n"
"	static sipCharInstanceDef charInstances[] = {\n"
					);

			noIntro = FALSE;
		}

		prcode(fp,
"		{%N, %S},\n"
			,vd -> name,vd -> fqname);
	}

	if (!noIntro)
	{
		prcode(fp,
"		{NULL}\n"
"	};\n"
"\n"
"	if (sipAddCharInstances(");

		if (cd != NULL)
			prcode(fp,"((PyClassObject *)sipClass_%C) -> cl_dict,%C_",classFQName(cd),classFQName(cd));
		else
			prcode(fp,"sipModule.md_dict,");

		prcode(fp,"charInstances) < 0)\n"
"		return NULL;\n"
			);
	}
}


/*
 * Generate the code to add a set of strings to a dictionary.
 */

static void generateStrings(sipSpec *pt,classDef *cd,FILE *fp)
{
	int noIntro;
	varDef *vd;

	noIntro = TRUE;

	for (vd = pt -> vars; vd != NULL; vd = vd -> next)
	{
		argType vtype = vd -> type.atype;

		if (vd -> ecd != cd || vd -> module != pt -> module)
			continue;

		if (!((vtype == ustring_type || vtype == string_type) && vd -> type.nrderefs != 0))
			continue;

		if (cd != NULL && !isStaticVar(vd))
			continue;

		if (noIntro)
		{
			if (cd != NULL)
				prcode(fp,
"\n"
"	// Add the strings to the class dictionary.\n"
"\n"
"	static sipStringInstanceDef %C_stringInstances[] = {\n"
					,classFQName(cd));
			else
				prcode(fp,
"\n"
"	// Add the strings to the module dictionary.\n"
"\n"
"	static sipStringInstanceDef stringInstances[] = {\n"
					);

			noIntro = FALSE;
		}

		prcode(fp,
"		{%N, %S},\n"
			,vd -> name,vd -> fqname);
	}

	if (!noIntro)
	{
		prcode(fp,
"		{NULL}\n"
"	};\n"
"\n"
"	if (sipAddStringInstances(");

		if (cd != NULL)
			prcode(fp,"((PyClassObject *)sipClass_%C) -> cl_dict,%C_",classFQName(cd),classFQName(cd));
		else
			prcode(fp,"sipModule.md_dict,");

		prcode(fp,"stringInstances) < 0)\n"
"		return NULL;\n"
			);
	}
}


/*
 * Generate the code to add a set of enums to the module dictionary.
 */

static void generateEnums(sipSpec *pt,FILE *fp)
{
	if (generateEnumTable(pt,NULL,fp) > 0)
		prcode(fp,
"\n"
"	if (sipAddEnumInstances(sipModule.md_dict,enumValues) < 0)\n"
"		return NULL;\n"
			);
}


/*
 * Generate the code to add a set of longs to a dictionary.
 */

static void generateLongs(sipSpec *pt,classDef *cd,FILE *fp)
{
	int noIntro;
	varDef *vd;

	noIntro = TRUE;

	for (vd = pt -> vars; vd != NULL; vd = vd -> next)
	{
		argType vtype = vd -> type.atype;

		if (vd -> ecd != cd || vd -> module != pt -> module)
			continue;

		if (!(vtype == enum_type || vtype == ushort_type || vtype == short_type || vtype == uint_type || vtype == cint_type || vtype == int_type || vtype == ulong_type || vtype == long_type || vtype == bool_type))
			continue;

		if (cd != NULL && !isStaticVar(vd))
			continue;

		if (noIntro)
		{
			if (cd != NULL)
				prcode(fp,
"\n"
"	// Add the longs to the class dictionary.\n"
"\n"
"	static sipLongInstanceDef %C_longInstances[] = {\n"
					,classFQName(cd));
			else
				prcode(fp,
"\n"
"	// Add the longs to the module dictionary.\n"
"\n"
"	static sipLongInstanceDef longInstances[] = {\n"
					);

			noIntro = FALSE;
		}

		prcode(fp,
"		{%N, %S},\n"
			,vd -> name,vd -> fqname);
	}

	if (!noIntro)
	{
		prcode(fp,
"		{NULL}\n"
"	};\n"
"\n"
"	if (sipAddLongInstances(");

		if (cd != NULL)
			prcode(fp,"((PyClassObject *)sipClass_%C) -> cl_dict,%C_",classFQName(cd),classFQName(cd));
		else
			prcode(fp,"sipModule.md_dict,");

		prcode(fp,"longInstances) < 0)\n"
"		return NULL;\n"
			);
	}
}


/*
 * Generate the code to add a set of doubles to a dictionary.
 */

static void generateDoubles(sipSpec *pt,classDef *cd,FILE *fp)
{
	int noIntro;
	varDef *vd;

	noIntro = TRUE;

	for (vd = pt -> vars; vd != NULL; vd = vd -> next)
	{
		argType vtype = vd -> type.atype;

		if (vd -> ecd != cd || vd -> module != pt -> module)
			continue;

		if (!(vtype == float_type || vtype == cfloat_type || vtype == double_type || vtype == cdouble_type))
			continue;

		if (cd != NULL && !isStaticVar(vd))
			continue;

		if (noIntro)
		{
			if (cd != NULL)
				prcode(fp,
"\n"
"	// Add the doubles to the class dictionary.\n"
"\n"
"	static sipDoubleInstanceDef %C_doubleInstances[] = {\n"
					,classFQName(cd));
			else
				prcode(fp,
"\n"
"	// Add the doubles to the module dictionary.\n"
"\n"
"	static sipDoubleInstanceDef doubleInstances[] = {\n"
					);

			noIntro = FALSE;
		}

		prcode(fp,
"		{%N, %S},\n"
			,vd -> name,vd -> fqname);
	}

	if (!noIntro)
	{
		prcode(fp,
"		{NULL}\n"
"	};\n"
"\n"
"	if (sipAddDoubleInstances(");

		if (cd != NULL)
			prcode(fp,"((PyClassObject *)sipClass_%C) -> cl_dict,%C_",classFQName(cd),classFQName(cd));
		else
			prcode(fp,"sipModule.md_dict,");

		prcode(fp,"doubleInstances) < 0)\n"
"		return NULL;\n"
			);
	}
}


/*
 * Generate the implementation of the module proxy.
 */

static void generateModuleProxy(sipSpec *pt,int parts,FILE *fp)
{
	char *mname = pt -> module -> name;
	funcArgsList *fl;

	prcode(fp,
"\n"
"#include \"%s%s.h\"\n"
"\n"
"char *sipProxy%s::getProxySlot(char *sigargs)\n"
"{\n"
"	static char *tab[] = {\n"
		,mname,(parts ? "part0" : "cmodule")
		,mname);

	for (fl = pt -> sigargs; fl != NULL; fl = fl -> next)
	{
		prcode(fp,
"		SLOT(proxySlot(");

		generateArgs(fl -> fa,Declaration,fp);

		prcode(fp,")),\n"
			);
	}

	prcode(fp,
"		NULL\n"
"	};\n"
"\n"
"	return searchProxySlotTable(tab,sigargs);\n"
"}\n"
		);

	for (fl = pt -> sigargs; fl != NULL; fl = fl -> next)
	{
		prcode(fp,
"\n"
"void sipProxy%s::proxySlot(",mname);

		generateArgs(fl -> fa,Definition,fp);

		prcode(fp,")\n"
"{\n"
"	sipSender = sender();\n"
"\n"
"	SIP_BLOCK_THREADS\n"
"	sipEmitToSlot(&sipRealSlot,");

		generateTupleBuilder(pt,fl -> fa,fp);

		prcode(fp,");\n"
"	SIP_UNBLOCK_THREADS\n"
"}\n"
			);
	}
}


/*
 * Generate the C++ code for an interface.
 */

static void generateIfaceCpp(sipSpec *pt,ifaceFileDef *iff,char *codeDir,
			     char *cppSuffix,FILE *master)
{
	char *cppfile, *cmname = iff -> module -> name;
	classDef *cd;
	mappedTypeDef *mtd;
	FILE *fp;

	if (master == NULL)
	{
		cppfile = createIfaceFileName(codeDir,iff,cppSuffix);
		fp = createFile(pt,cppfile,"//","C++ interface wrapper code.");
	}
	else
		fp = master;

	prcode(fp,
"\n"
"#include \"sip%sDecl%s.h\"\n"
"#include \"sip%s%F.h\"\n"
		,cmname,cmname,cmname,iff -> fqname);

	for (cd = pt -> classes; cd != NULL; cd = cd -> next)
		if (cd -> iff == iff)
			generateClassCpp(cd,pt,fp);

	for (mtd = pt -> mappedtypes; mtd != NULL; mtd = mtd -> next)
		if (mtd -> iff == iff)
			generateMappedTypeCpp(mtd,pt,fp);

	if (master == NULL)
	{
		closeFile(fp);
		free(cppfile);
	}
}


/*
 * Return a filename for an interface C++ or header file on the heap.
 */

static char *createIfaceFileName(char *codeDir,ifaceFileDef *iff,char *suffix)
{
	char *fn;
	scopedNameDef *snd;

	fn = concat(codeDir,"/sip",iff -> module -> name,NULL);

	for (snd = iff -> fqname; snd != NULL; snd = snd -> next)
		append(&fn,snd -> name);

	append(&fn,suffix);

	return fn;
}


/*
 * Generate the C++ code for a mapped type version.
 */

static void generateMappedTypeCpp(mappedTypeDef *mtd,sipSpec *pt,FILE *fp)
{
	prcode(fp,
"\n"
		);

	generateConvertToDefinitions(mtd,NULL,fp);

	/* Generate the from type convertor. */

	prcode(fp,
"\n"
"PyObject *sipConvertFrom_%T(%b *sipCpp)\n"
"{\n"
		,&mtd -> type,&mtd -> type);

	generateCppCodeBlock(mtd -> convfromcode,NULL,fp);

	prcode(fp,
"}\n"
		);
}


/*
 * Generate the C++ code for a class.
 */

static void generateClassCpp(classDef *cd,sipSpec *pt,FILE *fp)
{
	int noIntro, nr_slmethods, nr_methods, nr_enums;
	varDef *vd;

	prcode(fp,
"\n"
"\n"
"PyObject *sipClass_%C;\n"
		,classFQName(cd));

	/* Generate the Python object structures. */

	prcode(fp,
"\n"
"static void sipDealloc_%C(sipThisType *);\n"
		,classFQName(cd));

	prcode(fp,
"\n"
"static PyTypeObject sipType_%C = {\n"
"	PyObject_HEAD_INIT(&PyType_Type)\n"
"	0,\n"
"	sipWrapperTypeName,\n"
"	sizeof (sipThisType),\n"
"	0,\n"
"	(destructor)sipDealloc_%C,\n"
"	0,\n"
"	0,\n"
"	0,\n"
"	0,\n"
"	0,\n"
"	0,\n"
"	0,\n"
"	0,\n"
"	0,\n"
"	0,\n"
"	0,\n"
"	0,\n"
"	0,\n"
"	0,\n"
"	Py_TPFLAGS_DEFAULT,\n"
"};\n"
		,classFQName(cd)
		,classFQName(cd));

	/* Generate any local class code. */

	generateCppCodeBlock(cd -> cppcode,cd,fp);

	generateClassFunctions(pt,cd,fp);

	/* Generate the variable handlers. */

	for (vd = pt -> vars; vd != NULL; vd = vd -> next)
	{
		if (vd -> ecd != cd || isStaticVar(vd))
			continue;

		generateVariableHandler(pt,vd,fp);
	}

	/* Generate the attributes tables. */

	nr_slmethods = generateMethodTable(cd,TRUE,fp);
	nr_methods = generateMethodTable(cd,FALSE,fp);
	nr_enums = generateEnumTable(pt,cd,fp);

	generateLazyAttributes(cd,nr_slmethods,nr_methods,nr_enums,fp);

	/* Generate the variable tables. */

	noIntro = TRUE;

	for (vd = pt -> vars; vd != NULL; vd = vd -> next)
	{
		if (vd -> ecd != cd || isStaticVar(vd))
			continue;

		if (noIntro)
		{
			noIntro = FALSE;

			prcode(fp,
"\n"
"PyMethodDef sipClassVarTab_%C[] = {\n"
				,classFQName(cd));
		}

		prcode(fp,
"	{%N, sipGetSetVar_%C, 0, NULL},\n"
			,vd -> name,vd -> fqname);
	}

	if (!noIntro)
	{
		mroDef *mro;

		prcode(fp,
"	{NULL}\n"
"};\n"
"\n"
"PyMethodDef *sipClassVarHierTab_%C[] = {\n"
			,classFQName(cd));

		for (mro = cd -> mro; mro != NULL; mro = mro -> next)
		{
			if (isDuplicateSuper(mro))
				continue;

			for (vd = pt -> vars; vd != NULL; vd = vd -> next)
			{
				if (vd -> ecd != mro -> cd || isStaticVar(vd))
					continue;

				prcode(fp,
"	sipClassVarTab_%C,\n"
					,classFQName(mro -> cd));

				break;
			}

		}

		prcode(fp,
"	NULL\n"
"};\n"
			);
	}

	generateConvertToDefinitions(NULL,cd,fp);
}


/*
 * Return a sorted array of relevant functions for a namespace.
 */

static sortedMethTab *createFunctionTable(classDef *cd,int *nrp)
{
	int nr;
	sortedMethTab *mtab, *mt;
	memberDef *md;

	/* First we need to count the number of applicable functions. */

	nr = 0;

	for (md = cd -> members; md != NULL; md = md -> next)
		++nr;

	if ((*nrp = nr) == 0)
		return NULL;

	/* Create the table of methods. */

	mtab = sipMalloc(sizeof (sortedMethTab) * nr);

	/* Initialise the table. */

	mt = mtab;

	for (md = cd -> members; md != NULL; md = md -> next)
		mt++ -> md = md;

	/* Finally sort the table. */

	qsort(mtab,nr,sizeof (sortedMethTab),compareMethTab);

	return mtab;
}


/*
 * Return a sorted array of relevant methods (either slots or non-lazy) for a
 * class.
 */

static sortedMethTab *createMethodTable(classDef *cd,int slots,int *nrp)
{
	int nr;
	visibleList *vl;
	sortedMethTab *mtab, *mt;

	/*
	 * First we need to count the number of applicable methods.  Only
	 * provide an entry point if there is at least one overload that is
	 * defined in this class and is a non-abstract function or slot.  We
	 * allow private (even though we don't actually generate code) because
	 * we need to intercept the name before it reaches a more public
	 * version further up the class hierarchy.
	 */

	nr = 0;

	for (vl = cd -> visible; vl != NULL; vl = vl -> next)
	{
		overDef *od;

		for (od = vl -> cd -> overs; od != NULL; od = od -> next)
		{
			if (od -> common -> slot != no_slot && !slots)
				continue;

			if (od -> common -> slot == no_slot && slots)
				continue;

			/*
			 * Skip protected methods if we don't have the means to
			 * access them.
			 */

			if (isProtected(od) && !isComplex(cd))
				continue;

			if (skipOverload(od,vl -> m,cd,vl -> cd,TRUE))
				continue;

			++nr;

			break;
		}
	}

	if ((*nrp = nr) == 0)
		return NULL;

	/* Create the table of methods. */

	mtab = sipMalloc(sizeof (sortedMethTab) * nr);

	/* Initialise the table. */

	mt = mtab;

	for (vl = cd -> visible; vl != NULL; vl = vl -> next)
	{
		overDef *od;

		for (od = vl -> cd -> overs; od != NULL; od = od -> next)
		{
			if (od -> common -> slot != no_slot && !slots)
				continue;

			if (od -> common -> slot == no_slot && slots)
				continue;

			/*
			 * Skip protected methods if we don't have the means to
			 * access them.
			 */

			if (isProtected(od) && !isComplex(cd))
				continue;

			if (skipOverload(od,vl -> m,cd,vl -> cd,TRUE))
				continue;

			mt++ -> md = vl -> m;

			break;
		}
	}

	/* Finally sort the table. */

	qsort(mtab,nr,sizeof (sortedMethTab),compareMethTab);

	return mtab;
}


/*
 * The qsort helper to compare two sortedMethTab structures based on the Python
 * name of the method.
 */

static int compareMethTab(const void *m1,const void *m2)
{
	return strcmp(((sortedMethTab *)m1) -> md -> pyname -> text,
		      ((sortedMethTab *)m2) -> md -> pyname -> text);
}


/*
 * Generate the sorted table of methods and return the number of entries.
 */

static int generateMethodTable(classDef *cd,int slots,FILE *fp)
{
	int nr;
	sortedMethTab *mtab;

	mtab = (cd -> iff -> type == namespace_iface) ?
		createFunctionTable(cd,&nr) :
		createMethodTable(cd,slots,&nr);

	if (mtab != NULL)
	{
		int i;

		prcode(fp,
"\n"
"static PyMethodDef %sMethods_%C[] = {\n"
			,(slots ? "slot" : "lazy"),classFQName(cd));

		for (i = 0; i < nr; ++i)
		{
			memberDef *md = mtab[i].md;

			prcode(fp,
"	{%N, sipDo_%C_%s, METH_VARARGS, NULL}%s\n"
				,md -> pyname,classFQName(cd),md -> pyname -> text,((i + 1) < nr ? "," : ""));
		}

		free(mtab);

		prcode(fp,
"};\n"
			);
	}

	return nr;
}


/*
 * Generate the "to type" convertor definitions.
 */

static void generateConvertToDefinitions(mappedTypeDef *mtd,classDef *cd,
					 FILE *fp)
{
	codeBlock *convtocode;
	ifaceFileDef *iff;
	argDef type;

	if (cd != NULL)
	{
		convtocode = cd -> convtocode;

		iff = cd -> iff;

		type.atype = class_type;
		type.u.cd = cd;
	}
	else
	{
		convtocode = mtd -> convtocode;

		iff = mtd -> iff;

		type.atype = mapped_type;
		type.u.mtd = mtd;
	}

	type.argflags = 0;
	type.nrderefs = 0;
	type.defval = NULL;

	/* Generate the type convertors. */

	if (convtocode != NULL)
	{
		prcode(fp,
"\n"
"int sipConvertTo_%T(PyObject *sipPy,%b **sipCppPtr,int *sipIsErr)\n"
"{\n"
			,&type,&type);

		generateCppCodeBlock(convtocode,cd,fp);

		prcode(fp,
"}\n"
			);
	}

	prcode(fp,
"\n"
"%b *sipForceConvertTo_%T(PyObject *valobj,int *iserrp)\n"
"{\n"
"	if (*iserrp || valobj == NULL)\n"
"		return NULL;\n"
"\n"
		,&type,&type);

	if (convtocode != NULL)
		prcode(fp,
"	if (sipConvertTo_%T(valobj,NULL,NULL))\n"
"	{\n"
"		%b *val;\n"
"\n"
"		sipConvertTo_%T(valobj,&val,iserrp);\n"
"\n"
"		return val;\n"
"	}\n"
			,&type,&type,&type);
	else
		prcode(fp,
"	if (valobj == Py_None || sipIsSubClassInstance(valobj,sipClass_%T))\n"
"		return (%b *)sipConvertToCpp(valobj,sipClass_%T,iserrp);\n"
			,&type,&type,&type);

	prcode(fp,
"\n"
"	sipBadClass(%N);\n"
"\n"
"	*iserrp = 1;\n"
"\n"
"	return NULL;\n"
"}\n"
		,iff -> name);
}


/*
 * Generate a variable handler.
 */

static void generateVariableHandler(sipSpec *pt,varDef *vd,FILE *fp)
{
	char *deref, *ptr;
	argType atype = vd -> type.atype;

	prcode(fp,
"\n"
"static PyObject *sipGetSetVar_%C(PyObject *sipThisObj,PyObject *valobj)\n"
"{\n"
		,vd -> fqname);

	if (atype == class_type || atype == mapped_type)
		prcode(fp,
"	int iserr = 0;\n"
			);

	prcode(fp,
"	");

	generateValueType(&vd -> type,fp);

	if ((atype == string_type || atype == ustring_type) && vd -> type.nrderefs == 0)
	{
		ptr = "ptr";

		prcode(fp,"*");
	}
	else
		ptr = "";

	prcode(fp,"val%s;\n"
"	%S *ptr;\n"
"\n"
"	if ((ptr = (%S *)sipGetCppPtr((sipThisType *)sipThisObj,sipClass_%C)) == NULL)\n"
"		return NULL;\n"
"\n"
"	if (valobj == NULL)\n"
"	{\n"
		,ptr
		,classFQName(vd -> ecd)
		,classFQName(vd -> ecd)
		,classFQName(vd -> ecd));

	/* Generate the get handler part. */

	prcode(fp,
"		");

	if (ptr[0] != '\0')
		generateValueType(&vd -> type,fp);

	prcode(fp,"val = %sptr -> %s;\n"
"\n"
					,(((atype == class_type || atype == mapped_type) && vd -> type.nrderefs == 0) ? "&" : ""),vd -> name -> text);

	switch (atype)
	{
	case mapped_type:
		prcode(fp,
"		valobj = sipConvertFrom_%T(val);\n"
			,&vd -> type);

		break;

	case class_type:
		generateVarClassConversion(pt,vd,fp);
		break;

	case bool_type:
		prcode(fp,
"		valobj = sipConvertFromBool((int)val);\n"
			);

		break;

	case ustring_type:
	case string_type:
		if (vd -> type.nrderefs == 0)
			prcode(fp,
"		valobj = PyString_FromStringAndSize(%s&val,1);\n"
				,(atype == ustring_type) ? "(char *)" : "");
		else
			prcode(fp,
"		valobj = PyString_FromString(%sval);\n"
				,(atype == ustring_type) ? "(char *)" : "");

		break;

	case float_type:
	case cfloat_type:
		prcode(fp,
"		valobj = PyFloat_FromDouble((double)val);\n"
			);
		break;

	case double_type:
	case cdouble_type:
		prcode(fp,
"		valobj = PyFloat_FromDouble(val);\n"
			);
		break;

	case enum_type:
	case ushort_type:
	case short_type:
	case uint_type:
	case cint_type:
	case int_type:
	case ulong_type:
		prcode(fp,
"		valobj = PyInt_FromLong((long)val);\n"
			);
		break;

	case long_type:
		prcode(fp,
"		valobj = PyInt_FromLong(val);\n"
			);
		break;

	case struct_type:
	case voidptr_type:
		prcode(fp,
"		valobj = sipConvertFromVoidPtr(val);\n"
			);
		break;

	case pyobject_type:
		prcode(fp,
"		Py_XINCREF(val);\n"
			);
		break;
	}

	prcode(fp,
"\n"
"		return val%s;\n"
"	}\n"
		,(atype != pyobject_type ? "obj" : ""));

	/* Generate the set handler part. */

	prcode(fp,
"\n"
		);

	generateObjToCppConversion(&vd -> type,"val","\t",fp);

	deref = "";

	if (atype == class_type || atype == mapped_type)
	{
		if (vd -> type.nrderefs == 0)
			deref = "*";

		prcode(fp,
"\n"
"	if (iserr)\n"
			);
	}
	else if (atype == ustring_type || atype == string_type)
	{
		if (vd -> type.nrderefs == 0)
			deref = "*";

		prcode(fp,
"\n"
"	if (val%s == NULL)\n"
			,ptr);
	}
	else
		prcode(fp,
"\n"
"	if (PyErr_Occurred() != NULL)\n"
			);

	prcode(fp,
"	{\n"
"		sipBadSetType(%N,%N);\n"
"		return NULL;\n"
"	}\n"
"\n"
"	ptr -> %s = %sval%s;\n"
"\n"
"	Py_INCREF(Py_None);\n"
"	return Py_None;\n"
"}\n"
		,vd -> ecd -> iff -> name,vd -> name
		,vd -> name -> text,deref,ptr);
}


/*
 * Generate an variable class conversion fragment.
 */

static void generateVarClassConversion(sipSpec *pt,varDef *vd,FILE *fp)
{
	classDef *cd = vd -> type.u.cd;

	prcode(fp,
"		valobj = sipMapCppToSelf%s(val,sipClass_%C);\n"
		,(cd -> subbase != NULL ? "SubClass" : ""),classFQName(cd));
}


/*
 * Generate the declaration of a variable that is initialised from a Python
 * object.
 */

static void generateObjToCppConversion(argDef *ad,char *name,char *indent,FILE *fp)
{
	if (ad -> atype == class_type || ad -> atype == mapped_type)
	{
		prcode(fp,
"%s%s = sipForceConvertTo_%T(%sobj,&iserr);\n"
			,indent,name,ad,name);
	}
	else if (ad -> atype == enum_type)
		prcode(fp,
"%s%s = (%E)PyInt_AsLong(%sobj);\n"
			,indent,name,ad -> u.ed,name);
	else
	{
		char *fmt = NULL;

		switch (ad -> atype)
		{
		case ustring_type:
			if (ad -> nrderefs == 0)
				fmt = "%s%sptr = (unsigned char *)PyString_AsString(%sobj);\n";
			else
				fmt = "%s%s = (unsigned char *)PyString_AsString(%sobj);\n";
			break;

		case string_type:
			if (ad -> nrderefs == 0)
				fmt = "%s%sptr = PyString_AsString(%sobj);\n";
			else
				fmt = "%s%s = PyString_AsString(%sobj);\n";
			break;

		case float_type:
		case cfloat_type:
			fmt = "%s%s = (float)PyFloat_AsDouble(%sobj);\n";
			break;

		case double_type:
		case cdouble_type:
			fmt = "%s%s = PyFloat_AsDouble(%sobj);\n";
			break;

		case bool_type:
			fmt = "%s%s = (bool)PyInt_AsLong(%sobj);\n";
			break;

		case ushort_type:
			fmt = "%s%s = (unsigned short)PyInt_AsLong(%sobj);\n";
			break;

		case short_type:
			fmt = "%s%s = (short)PyInt_AsLong(%sobj);\n";
			break;

		case uint_type:
			fmt = "%s%s = (unsigned)PyInt_AsLong(%sobj);\n";
			break;

		case int_type:
		case cint_type:
			fmt = "%s%s = (int)PyInt_AsLong(%sobj);\n";
			break;

		case ulong_type:
			fmt = "%s%s = (unsigned long)PyInt_AsLong(%sobj);\n";
			break;

		case long_type:
			fmt = "%s%s = PyInt_AsLong(%sobj);\n";
			break;

		case struct_type:
			prcode(fp,
"%s%s = (struct %S *)sipConvertToVoidPtr(%sobj);\n"
				,indent,name,ad -> u.sname,name);
			break;

		case voidptr_type:
			fmt = "%s%s = sipConvertToVoidPtr(%sobj);\n";
			break;

		case pyobject_type:
			fmt = "%s%s = %sobj;\n";
			break;
		}

		if (fmt != NULL)
			prcode(fp,fmt,indent,name,name);
	}
}


/*
 * Generate the member functions for a class.
 */

static void generateClassFunctions(sipSpec *pt,classDef *cd,FILE *fp)
{
	visibleList *vl;
	classList *cl;
	overDef *od;

	/* Any complex code. */

	if (isComplex(cd))
		if (cannotCreate(cd))
			generateVirtualHandlers(pt,cd,fp);
		else
			generateComplexCode(pt,cd,fp);

	/* The member functions. */

	for (vl = cd -> visible; vl != NULL; vl = vl -> next)
		generateFunction(pt,vl -> m,vl -> cd -> overs,cd,vl -> cd,fp);

	/* The cast function. */

	prcode(fp,
"\n"
"// Cast a pointer to a type somewhere in its superclass hierarchy.\n"
"\n"
"const void *sipCast_%C(const void *ptr,PyObject *targetClass)\n"
"{\n"
		,classFQName(cd));

	if (cd -> supers != NULL)
		prcode(fp,
"	const void *res;\n"
"\n"
			);

	prcode(fp,
"	if (targetClass == sipClass_%C)\n"
"		return ptr;\n"
		,classFQName(cd));

	for (cl = cd -> supers; cl != NULL; cl = cl -> next)
	{
		scopedNameDef *sname = cl -> cd -> iff -> fqname;

		prcode(fp,
"\n"
"	if ((res = sipCast_%C((%S *)(%S *)ptr,targetClass)) != NULL)\n"
"		return res;\n"
			,sname,sname,classFQName(cd));
	}

	prcode(fp,
"\n"
"	return NULL;\n"
"}\n"
		);

	/* The dealloc function. */

	prcode(fp,
"\n"
"static void sipDealloc_%C(sipThisType *sipThis)\n"
"{\n"
		,classFQName(cd));

	if (tracing)
		prcode(fp,
"	sipTrace(SIP_TRACE_DEALLOCS,\"sipDealloc_%C()\\n\");\n"
"\n"
			,classFQName(cd));

	if (cd -> dealloccode != NULL)
	{
		generateCppCodeBlock(cd -> dealloccode,cd,fp);

		prcode(fp,
"\n"
			);
	}

	/* Generate the right dtor call - if there is one. */

	if (!cannotCreate(cd) && !isPrivateDtor(cd))
	{
		prcode(fp,
"	if (sipThis -> u.cppPtr != NULL)\n"
"	{\n"
			);

		/* Disable the virtual handlers. */

		if (isComplex(cd))
			prcode(fp,
"		if (!sipIsSimple(sipThis))\n"
"			((sip%C *)sipThis -> u.cppPtr) -> sipPyThis = NULL;\n"
				,classFQName(cd));

		if (isPublicDtor(cd) || (isComplex(cd) && isProtectedDtor(cd)))
		{
			prcode(fp,
"\n"
"		if (sipIsPyOwned(sipThis))\n"
"		{\n"
"			Py_BEGIN_ALLOW_THREADS\n"
"\n"
				);

			if (isComplex(cd))
			{
				prcode(fp,
"			if (!sipIsSimple(sipThis))\n"
"				delete (sip%C *)sipThis -> u.cppPtr;\n"
					,classFQName(cd));

				if (isPublicDtor(cd))
					prcode(fp,
"			else\n"
"				delete (%S *)sipThis -> u.cppPtr;\n"
						,classFQName(cd));
			}
			else
				prcode(fp,
"			delete (%S *)sipThis -> u.cppPtr;\n"
					,classFQName(cd));

			prcode(fp,
"\n"
"			Py_END_ALLOW_THREADS\n"
"		}\n"
				);
		}

		prcode(fp,
"	}\n"
"\n"
			);
	}

	prcode(fp,
"	sipDeleteThis(sipThis);\n"
"}\n"
		);

	/* The Python constructor. */

	generatePythonConstructor(pt,cd,fp);
}


/*
 * Generate the code specific to complex classes.
 */

static void generateComplexCode(sipSpec *pt,classDef *cd,FILE *fp)
{
	int noIntro, nrVirts, virtNr;
	visibleList *vl;
	virtOverDef *vod;
	ctorDef *ct;

	nrVirts = countVirtuals(cd);

	/* Generate the wrapper class constructors. */

	for (ct = cd -> ctors; ct != NULL; ct = ct -> next)
	{
		char *prefix;
		int a;
		funcArgs *args;

		if (isPrivateCtor(ct))
			continue;

		args = (ct -> cppsig != NULL) ? &ct -> cppsig -> args : &ct -> args;

		prcode(fp,
"\n"
"sip%C::sip%C(",classFQName(cd),classFQName(cd));

		generateArgs(args,Definition,fp);

		prcode(fp,")%X : %S(",ct -> exceptions,classFQName(cd));

		prefix = "";

		for (a = 0; a < args -> nrArgs; ++a)
		{
			prcode(fp,"%sa%d",prefix,a);
			prefix = ",";
		}

		prcode(fp,"), sipPyThis(0)\n"
"{\n"
			);

		if (tracing)
		{
			prcode(fp,
"	sipTrace(SIP_TRACE_CTORS,\"sip%C::sip%C(",classFQName(cd),classFQName(cd));
			generateArgs(args,Declaration,fp);
			prcode(fp,")%X (this=0x%%08x)\\n\",this);\n"
"\n"
				,ct -> exceptions);
		}

		prcode(fp,
"	sipCommonCtor(%s,%d);\n"
"}\n"
			,(nrVirts > 0 ? "sipPyMethods" : "NULL"),nrVirts);
	}

	/* The destructor. */

	if (!isPrivateDtor(cd))
	{
		prcode(fp,
"\n"
"sip%C::~sip%C()%X\n"
"{\n"
			,classFQName(cd),classFQName(cd),cd -> dtorexceptions);

		if (tracing)
			prcode(fp,
"       sipTrace(SIP_TRACE_DTORS,\"sip%C::~sip%C()%X (this=0x%%08x)\\n\",this);\n"
"\n"
				,classFQName(cd),classFQName(cd),cd -> dtorexceptions);

		if (cd -> dtorcode != NULL)
			generateCppCodeBlock(cd -> dtorcode,cd,fp);

		prcode(fp,
"	sipCommonDtor(sipPyThis);\n"
"}\n"
			);
	}

	/* Generate the virtual catchers. */
 
	virtNr = 0;
 
	for (vod = cd -> vmembers; vod != NULL; vod = vod -> next)
		if (!isPrivate(vod -> o))
			generateVirtualCatcher(cd,virtNr++,vod,fp);

	/* Generate the virtual handlers. */

	generateVirtualHandlers(pt,cd,fp);

	/* Generate the wrapper around each protected member function. */

	generateProtectedDefinitions(pt,cd,fp);

	/* Generate the emitter functions. */

	for (vl = cd -> visible; vl != NULL; vl = vl -> next)
	{
		overDef *od;

		for (od = vl -> cd -> overs; od != NULL; od = od -> next)
			if (od -> common == vl -> m && isSignal(od))
			{
				generateEmitter(pt,cd,vl,fp);
				break;
			}
	}

	/* Generate the table of signals to support fan-outs. */

	noIntro = TRUE;

	for (vl = cd -> visible; vl != NULL; vl = vl -> next)
	{
		overDef *od;

		for (od = vl -> cd -> overs; od != NULL; od = od -> next)
			if (od -> common == vl -> m && isSignal(od))
			{
				if (noIntro)
				{
					setHasSigSlots(cd);

					prcode(fp,
"\n"
"static sipQtSignal sipSignals_%C[] = {\n"
						,classFQName(cd));

					noIntro = FALSE;
				}

				prcode(fp,
"	{%N, %C_emit_%s},\n"
					,vl -> m -> pyname,classFQName(cd),vl -> m -> pyname -> text);

				break;
			}
	}

	if (!noIntro)
		prcode(fp,
"	{NULL, NULL}\n"
"};\n"
			);
}


/*
 * Generate the protected enums for a class.
 */

static void generateProtectedEnums(sipSpec *pt,classDef *cd,FILE *fp)
{
	enumDef *ed;

	for (ed = pt -> enums; ed != NULL; ed = ed -> next)
	{
		char *eol;
		enumValueDef *evd;

		if (ed -> ecd != cd || !isProtectedEnum(ed))
			continue;

		prcode(fp,
"\n"
"	// Expose this protected enum.\n"
"\n"
"	enum");

		if (ed -> fqname != NULL)
			prcode(fp," sip%s",scopedNameTail(ed -> fqname));

		prcode(fp," {");

		eol = "\n";

		for (evd = ed -> values; evd != NULL; evd = evd -> next)
		{
			prcode(fp,"%s"
"		%s = %S::%s",eol,evd -> name -> text,classFQName(ed -> ecd),evd -> name -> text);

			eol = ",\n";
		}

		prcode(fp,"\n"
"	};\n"
			);
	}
}


/*
 * Generate the catcher for a virtual function.
 */

static void generateVirtualCatcher(classDef *cd,int virtNr,virtOverDef *vod,
				   FILE *fp)
{
	scopedNameDef *fcname = classFQName(vod -> farc);
	char *pname, *mname, *cast, *thread;
	nameDef *pmname;
	overDef *od = vod -> o;
	argDef *res;
	funcArgs *args;

	pname = od -> common -> pyname -> text;
	mname = od -> cppname;
	pmname = od -> common -> pyname;

	prcode(fp,
"\n");

	getCppSignature(od,&res,&args);

	generateResultType(res,fp);

	prcode(fp," sip%C::%s(",classFQName(cd),mname);
	generateArgs(args,Definition,fp);
	prcode(fp,")%s%X\n"
"{\n"
		,(isConst(od) ? " const" : ""),od -> exceptions);

	if (tracing)
	{
		prcode(fp,
"	sipTrace(SIP_TRACE_CATCHERS,\"");

		generateResultType(res,fp);
		prcode(fp," sip%C::%s(",classFQName(cd),mname);
		generateArgs(args,Declaration,fp);
		prcode(fp,")%s%X (this=0x%%08x)\\n\",this);\n"
"\n"
			,(isConst(od) ? " const" : ""),od -> exceptions);
}

	cast = (isConst(od) ? "(sipMethodCache *)" : "");
	thread = (isNewThread(od) ? "Thread" : "");

	if (isAbstract(od))
	{
		prcode(fp,
"	if (sipIsPyMethod%s(%s&sipPyMethods[%d],sipPyThis,%N,%N))\n"
"		%ssip%C::sipVH_%s(&sipPyMethods[%d],sipPyThis",thread,cast,virtNr,cd -> iff -> name,pmname,(od -> result != NULL ? "return " : ""),fcname,mname,virtNr);
 
		if (args -> nrArgs > 0)
			prcode(fp,",");
 
		generateArgCallList(args,fp);
 
		prcode(fp,");\n"
			);
	}
	else if (res == NULL)
	{
		prcode(fp,
"	if (sipIsPyMethod%s(%s&sipPyMethods[%d],sipPyThis,NULL,%N))\n"
"		sip%C::sipVH_%s(&sipPyMethods[%d],sipPyThis",thread,cast,virtNr,pmname,fcname,mname,virtNr);
 
		if (args -> nrArgs > 0)
			prcode(fp,",");
 
		generateArgCallList(args,fp);
 
		prcode(fp,");\n"
"	else\n"
"		");
 
		generateUnambiguousClass(cd,vod -> nearc,fp);

		prcode(fp,"::%s(",mname);
 
		generateArgCallList(args,fp);
 
		prcode(fp,");\n"
			);
	}
	else
	{
		prcode(fp,
"	return sipIsPyMethod%s(%s&sipPyMethods[%d],sipPyThis,NULL,%N) ?\n"
"		sip%C::sipVH_%s(&sipPyMethods[%d],sipPyThis",thread,cast,virtNr,pmname,fcname,mname,virtNr);

		if (args -> nrArgs > 0)
			prcode(fp,",");
 
		generateArgCallList(args,fp);
 
		prcode(fp,") :\n"
"		");
 
		generateUnambiguousClass(cd,vod -> nearc,fp);

		prcode(fp,"::%s(",mname);
 
		generateArgCallList(args,fp);
 
		prcode(fp,");\n"
			);
	}
 
	prcode(fp,
"}\n"
		);
}


/*
 * Generate the scope of the near class of a virtual taking duplicate
 * super-classes into account.
 */

static void generateUnambiguousClass(classDef *cd,classDef *nearc,FILE *fp)
{
	mroDef *mro;

	/* See if the near class has a duplicate. */

	for (mro = cd -> mro; mro != NULL; mro = mro -> next)
		if (mro -> cd == nearc)
		{
			if (hasDuplicateSuper(mro))
			{
				mroDef *guardc;

				/*
				 * Backtrack to find the class that directly
				 * sub-classes the duplicated one.  This will
				 * be the one that disambiguates the duplicated
				 * one.
				 */

				guardc = mro;

				while (guardc != cd -> mro)
				{
					mroDef *sub;
					classList *cl;

					for (sub = cd -> mro; sub -> next != guardc; sub = sub -> next)
						;

					for (cl = sub -> cd -> supers; cl != NULL; cl = cl -> next)
						if (cl -> cd == mro -> cd)
						{
							prcode(fp,"%S",classFQName(sub -> cd));

							return;
						}

					/* Try the previous one. */
					guardc = sub;
				}
			}

			break;
		}

	/* If we got here there is nothing to worry about. */
	prcode(fp,"%S",classFQName(nearc));
}


/*
 * Generate the emitter function for a signal.
 */

static void generateEmitter(sipSpec *pt,classDef *cd,visibleList *vl,FILE *fp)
{
	char *pname = vl -> m -> pyname -> text;
	overDef *od;

	prcode(fp,
"\n"
"int sip%C::sipEmit_%s(PyObject *sipArgs)\n"
"{\n"
"	int sipArgsParsed = 0;\n"
		,classFQName(cd),pname);

	for (od = vl -> cd -> overs; od != NULL; od = od -> next)
	{
		if (od -> common != vl -> m || !isSignal(od))
			continue;

		/*
		 * Generate the code that parses the args and emits the
		 * appropriate overloaded signal.
		 */

		prcode(fp,
"\n"
"	{\n"
			);

		generateArgParser(pt,&od -> args,NULL,NULL,FALSE,fp);

		prcode(fp,
"		{\n"
"			Py_BEGIN_ALLOW_THREADS\n"
"			emit %s("
			,od -> cppname);

		generateArgs(&od -> args,Call,fp);

		prcode(fp,");\n"
"			Py_END_ALLOW_THREADS\n"
"\n"
			);

		generateCallTidyUp(&od -> args,fp);

		prcode(fp,
"			return 0;\n"
"		}\n"
"	}\n"
			);
	}

	prcode(fp,
"\n"
"	sipNoMethod(sipArgsParsed,%N,%N);\n"
"\n"
"	return -1;\n"
"}\n"
"\n"
"static int %C_emit_%s(sipThisType *w,PyObject *sipArgs)\n"
"{\n"
"	sip%C *ptr;\n"
"\n"
"	if ((ptr = (sip%C *)sipGetComplexCppPtr(w)) == NULL)\n"
"		return -1;\n"
"\n"
"	return ptr -> sipEmit_%s(sipArgs);\n"
"}\n"
		,cd -> iff -> name,vl -> m -> pyname
		,classFQName(cd),pname
		,classFQName(cd)
		,classFQName(cd)
		,pname);
}


/*
 * Generate the declarations of the protected wrapper functions for a class.
 */

static void generateProtectedDeclarations(sipSpec *pt,classDef *cd,FILE *fp)
{
	int noIntro;
	visibleList *vl;

	noIntro = TRUE;

	for (vl = cd -> visible; vl != NULL; vl = vl -> next)
	{
		overDef *od;

		for (od = vl -> cd -> overs; od != NULL; od = od -> next)
		{
			argDef *res;
			funcArgs *args;

			if (od -> common != vl -> m || !isProtected(od) ||
			    isAbstract(od))
				continue;

			if (noIntro)
			{
				prcode(fp,
"\n"
"	// There is a public member function for every protected member\n"
"	// function visible from this class.\n"
"\n"
					);

				noIntro = FALSE;
			}

			prcode(fp,
"	");

			getCppSignature(od,&res,&args);

			if (isStatic(od))
				prcode(fp,"static ");

			generateResultType(res,fp);

			prcode(fp," sipProtect_%s(",od -> cppname);
			generateArgs(args,Declaration,fp);
			prcode(fp,");\n"
				);
		}
	}
}


/*
 * Generate the definitions of the protected wrapper functions for a class.
 */

static void generateProtectedDefinitions(sipSpec *pt,classDef *cd,FILE *fp)
{
	visibleList *vl;

	for (vl = cd -> visible; vl != NULL; vl = vl -> next)
	{
		overDef *od;

		for (od = vl -> cd -> overs; od != NULL; od = od -> next)
		{
			char *mname = od -> cppname;
			int a;
			argDef *res;
			funcArgs *args;

			if (od -> common != vl -> m || !isProtected(od) ||
			    isAbstract(od))
				continue;

			prcode(fp,
"\n"
				);

			getCppSignature(od,&res,&args);

			generateResultType(res,fp);

			prcode(fp,
" sip%C::sipProtect_%s(",classFQName(cd),mname);
			generateArgs(args,Definition,fp);
			prcode(fp,")\n"
"{\n"
				);

			if (res != NULL)
			{
				prcode(fp,
"	return ");

				if (res -> atype == enum_type && isProtectedEnum(res -> u.ed))
					prcode(fp,"(%E)",res -> u.ed);
			}
			else
				prcode(fp,
"	");

			prcode(fp,"%S::%s(",classFQName(vl -> cd),mname);

			for (a = 0; a < args -> nrArgs; ++a)
			{
				if (args -> args[a].atype == enum_type && isProtectedEnum(args -> args[a].u.ed))
					prcode(fp,"(%S)",args -> args[a].u.ed -> fqname);

				prcode(fp,"%sa%d",(a == 0 ? "" : ","),a);
			}

			prcode(fp,");\n"
"}\n"
				);
		}
	}
}


/*
 * Generate the argument list of a simple call.
 */

static void generateArgCallList(funcArgs *fa,FILE *fp)
{
	int a;

	for (a = 0; a < fa -> nrArgs; ++a)
		prcode(fp,"%sa%d",(a == 0 ? "" : ","),a);
}


/*
 * Generate the virtual handlers for a class version.
 */

static void generateVirtualHandlers(sipSpec *pt,classDef *cd,FILE *fp)
{
	virtOverDef *vod;

	for (vod = cd -> vmembers; vod != NULL; vod = vod -> next)
		if (vod -> farc == cd)
			generateVirtualHandler(pt,vod,fp);
}


/*
 * Generate the function that does most of the work to handle a particular
 * virtual function.
 */

static void generateVirtualHandler(sipSpec *pt,virtOverDef *vod,FILE *fp)
{
	int error_flag, a, nrvals, copy;
	scopedNameDef *cname = classFQName(vod -> farc);
	argDef *res, res_noconstref;
	funcArgs *args;
	overDef *od = vod -> o;

	getCppSignature(od,&res,&args);

	if (res != NULL)
	{
		res_noconstref = *res;
		resetIsConstArg(&res_noconstref);
		resetIsReference(&res_noconstref);
	}

	prcode(fp,
"\n"
"// The common handler for all classes that inherit this virtual member\n"
"// function.\n"
"\n"
		);

	generateResultType(res,fp);

	prcode(fp," sip%C::sipVH_%s(const sipMethodCache *pymc,sipThisType *sipThis",cname,od -> cppname);

	if (args -> nrArgs > 0)
		prcode(fp,",");

	generateArgs(args,Definition,fp);
	prcode(fp,")\n"
"{\n"
		);

	if (od -> ovirtcode != NULL)
	{
		generateCppCodeBlock(od -> ovirtcode,vod -> farc,fp);
		prcode(fp,
"}\n"
			);

		return;
	}

	copy = FALSE;

	if (res != NULL)
	{
		ctorDef *ct;

		if ((res -> atype == class_type || res -> atype == mapped_type) && res -> nrderefs == 0)
			copy = TRUE;

		prcode(fp,
"	");

		generateResultType(&res_noconstref,fp);

		prcode(fp," sipRes");

		if (res -> atype == class_type && res -> nrderefs == 0 && (ct = res -> u.cd -> defctor) != NULL)
		{
			funcArgs *ctargs;

			ctargs = (ct -> cppsig != NULL) ? &ct -> cppsig -> args : &ct -> args;

			if (isPublicCtor(ct) && ctargs -> nrArgs > 0 && ctargs -> args[0].defval == NULL)
				generateCallDefaultCtor(ct,fp);
		}

		prcode(fp,";\n"
			);
	}

	error_flag = (od -> virtcode != NULL && needErrorFlag(od -> virtcode));

	if (error_flag)
		prcode(fp,
"	int sipIsErr = FALSE;\n"
"\n"
			);

	prcode(fp,
"	sipBoundMethod sipBm;\n"
"	sipBm.method = &pymc -> pyMethod;\n"
"	sipBm.sipThis = sipThis;\n"
		);

	if (od -> virtcode != NULL)
	{
		prcode(fp,
"\n"
"	const sipBoundMethod *sipMethod = &sipBm;\n"
"\n"
			);

		generateCppCodeBlock(od -> virtcode,NULL,fp);

		if (error_flag)
			prcode(fp,
"\n"
"	if (sipIsErr)\n"
"		PyErr_Print();\n"
				);

		prcode(fp,
"\n"
"	%s\n"
			,isNewThread(od) ? "sipEndThread();" : "SIP_UNBLOCK_THREADS");

		if (res != NULL)
			prcode(fp,
"\n"
"	return sipRes;\n"
				);

		prcode(fp,
"}\n"
			);

		return;
	}

	/* See how many values we expect. */

	nrvals = (res != NULL ? 1 : 0);

	for (a = 0; a < args -> nrArgs; ++a)
		if (isOutArg(&args -> args[a]))
			++nrvals;

	if (copy)
	{
		prcode(fp,
"	");

		generateResultType(&res_noconstref,fp);

		prcode(fp," *sipResOrig;\n");
	}

	/* Call the method. */

	prcode(fp,
"\n"
"	PyObject *resobj = sipCallMethod(0,&sipBm,");

	generateTupleBuilder(pt,args,fp);

	prcode(fp,");\n"
"\n"
"	if (!resobj || sipParseResult(0,&sipBm,resobj,\"");

	/* Build the format string. */
	if (nrvals == 0)
		prcode(fp,"Z");
	else
	{
		if (nrvals > 1)
			prcode(fp,"(");

		if (res != NULL)
			prcode(fp,"%c",getParseResultFormat(res));

		for (a = 0; a < args -> nrArgs; ++a)
		{
			argDef *ad = &args -> args[a];

			if (isOutArg(ad))
				prcode(fp,"%c",getParseResultFormat(ad));
		}

		if (nrvals > 1)
			prcode(fp,")");
	}

	prcode(fp,"\"");

	/* Pass the destination pointers. */

	if (res != NULL)
	{
		generateParseResultExtraArgs(res,fp);
		prcode(fp,",&sipRes%s",(copy ? "Orig" : ""));
	}

	for (a = 0; a < args -> nrArgs; ++a)
	{
		argDef *ad = &args -> args[a];

		if (isOutArg(ad))
		{
			generateParseResultExtraArgs(ad,fp);
			prcode(fp,",%sa%d",(isReference(ad) ? "&" : ""),a);
		}
	}

	prcode(fp,") < 0)\n"
"		PyErr_Print();\n"
		);

	/* Make a copy if needed. */

	if (copy)
		prcode(fp,
"	else\n"
"		sipRes = *sipResOrig;\n"
			);

	if (isFactory(od) || isResultTransferredBack(od))
		prcode(fp,
"\n"
"	sipTransferSelf(resobj,1);\n"
			);

	prcode(fp,
"\n"
"	Py_XDECREF(resobj);\n"
"\n"
"	%s\n"
		,isNewThread(od) ? "sipEndThread();" : "SIP_UNBLOCK_THREADS");

	if (res != NULL)
		prcode(fp,
"\n"
"	return sipRes;\n"
			);

	prcode(fp,
"}\n"
		);
}


/*
 * Generate the call to a default ctor.
 */

static void generateCallDefaultCtor(ctorDef *ct,FILE *fp)
{
	int a;
	funcArgs *args;

	args = (ct -> cppsig != NULL) ? &ct -> cppsig -> args : &ct -> args;

	prcode(fp,"(");

	for (a = 0; a < args -> nrArgs; ++a)
	{
		argDef *ad = &args -> args[a];

		if (ad -> defval != NULL)
			break;

		if (a > 0)
			prcode(fp,",");

		if (ad -> atype == enum_type && isProtectedEnum(ad -> u.ed))
			prcode(fp,"(%E)",ad -> u.ed);

		/*
		 * This will generate bad code if an inappropriate default ctor
		 * has been specified.
		 */
		prcode(fp,"0");
	}

	prcode(fp,")");
}


/*
 * Generate the extra arguments needed by sipParseResult() for a particular
 * type.
 */

static void generateParseResultExtraArgs(argDef *ad,FILE *fp)
{
	switch (ad -> atype)
	{
	case mapped_type:
	case class_type:
		prcode(fp,",sipForceConvertTo_%T",ad);
		break;

	case pytuple_type:
		prcode(fp,",&PyTuple_Type");
		break;

	case pylist_type:
		prcode(fp,",&PyList_Type");
		break;

	case pydict_type:
		prcode(fp,",&PyDict_Type");
		break;

	case pyslice_type:
		prcode(fp,",&PySlice_Type");
		break;
	}
}


/*
 * Return the format character used by sipParseResult() for a particular type.
 */
static char getParseResultFormat(argDef *ad)
{
	switch (ad -> atype)
	{
	case mapped_type:
		return (ad -> nrderefs == 0 ? 'M' : 'L');

	case class_type:
		return (ad -> nrderefs == 0 ? 'K' : 'J');

	case bool_type:
		return 'b';

	case ustring_type:
	case string_type:
		return (ad -> nrderefs == 0) ? 'c' : 's';

	case enum_type:
		return 'e';

	case ushort_type:
	case short_type:
		return 'h';

	case uint_type:
	case int_type:
	case cint_type:
		return 'i';

	case ulong_type:
	case long_type:
		return 'l';

	case voidptr_type:
	case struct_type:
		return 'V';

	case float_type:
	case cfloat_type:
		return 'f';

	case double_type:
	case cdouble_type:
		return 'd';

	case pyobject_type:
		return 'O';

	case pytuple_type:
	case pylist_type:
	case pydict_type:
	case pyslice_type:
		return (isAllowNone(ad) ? 'N' : 'T');
	}

	/* We should never get here. */
	return ' ';
}


/*
 * Generate the code to build a tuple of Python arguments.
 */

static void generateTupleBuilder(sipSpec *pt,funcArgs *fa,FILE *fp)
{
	int a, arraylenarg;

	prcode(fp,"\"");

	for (a = 0; a < fa -> nrArgs; ++a)
	{
		char *fmt = "";
		argDef *ad = &fa -> args[a];

		if (!isInArg(ad))
			continue;

		switch (ad -> atype)
		{
		case ustring_type:
		case string_type:
			if (ad -> nrderefs == 0 || (ad -> nrderefs == 1 && isOutArg(ad)))
				fmt = "c";
			else if (isArray(ad))
				fmt = "a";
			else
				fmt = "s";

			break;

		case bool_type:
			fmt = "b";
			break;

		case enum_type:
			fmt = "e";
			break;

		case cint_type:
			fmt = "i";
			break;

		case uint_type:
		case int_type:
			if (isArraySize(ad))
				arraylenarg = a;
			else
				fmt = "i";

			break;

		case ushort_type:
		case short_type:
			if (isArraySize(ad))
				arraylenarg = a;
			else
				fmt = "h";

			break;

		case ulong_type:
		case long_type:
			if (isArraySize(ad))
				arraylenarg = a;
			else
				fmt = "l";

			break;

		case struct_type:
		case voidptr_type:
			fmt = "V";
			break;

		case float_type:
		case cfloat_type:
			fmt = "f";
			break;

		case double_type:
		case cdouble_type:
			fmt = "d";
			break;

		case signal_type:
		case slot_type:
		case slotcon_type:
		case slotdis_type:
			fmt = "s";
			break;

		case mapped_type:
			fmt = "T";
			break;

		case class_type:
			fmt = (ad -> u.cd -> subbase != NULL ? "M" : "O");
			break;

		case rxcon_type:
		case rxdis_type:
		case qobject_type:
			fmt = "O";
			break;

		case pyobject_type:
		case pytuple_type:
		case pylist_type:
		case pydict_type:
		case pycallable_type:
		case pyslice_type:
			fmt = "S";
			break;
		}

		prcode(fp,fmt);
	}

	prcode(fp,"\"");

	for (a = 0; a < fa -> nrArgs; ++a)
	{
		int derefs;
		argDef *ad = &fa -> args[a];

		if (!isInArg(ad))
			continue;

		derefs = ad -> nrderefs;

		switch (ad -> atype)
		{
		case ustring_type:
		case string_type:
			if (!(ad -> nrderefs == 0 || (ad -> nrderefs == 1 && isOutArg(ad))))
				--derefs;

			break;

		case mapped_type:
		case class_type:
			if (ad -> nrderefs > 0)
				--derefs;

			break;

		case struct_type:
		case voidptr_type:
			--derefs;
			break;
		}

		if (ad -> atype == mapped_type || ad -> atype == class_type ||
		    ad -> atype == rxcon_type || ad -> atype == rxdis_type ||
		    ad -> atype == qobject_type)
		{
			prcode(fp,",");

			if (isConstArg(ad))
				prcode(fp,"(%b *)",ad);

			if (ad -> nrderefs == 0)
				prcode(fp,"&");
			else
				while (derefs-- != 0)
					prcode(fp,"*");

			prcode(fp,"a%d",a);

			if (ad -> atype == mapped_type)
				prcode(fp,",sipConvertFrom_%T",ad);
			else if (ad -> atype == class_type)
				prcode(fp,",sipClass_%C",classFQName(ad -> u.cd));
			else
				prcode(fp,",sipClass_QObject");
		}
		else
		{
			if (!isArraySize(ad))
			{
				prcode(fp,",");

				while (derefs-- != 0)
					prcode(fp,"*");

				prcode(fp,"a%d",a);
			}

			if (isArray(ad))
			{
				argType astype = fa -> args[arraylenarg].atype;

				prcode(fp,",%sa%d",(astype == int_type || astype == uint_type ? "" : "(int)"),arraylenarg);
			}
		}
	}
}


/*
 * Generate the class interface #include directives required by either a class
 * or a module.
 */

static void generateUsedIncludes(ifaceFileList *iffl,FILE *fp)
{
	prcode(fp,
"\n"
		);

	while (iffl != NULL)
	{
		prcode(fp,
"#include \"sip%s%F.h\"\n"
			,iffl -> iff -> module -> name,iffl -> iff -> fqname);

		iffl = iffl -> next;
	}

	prcode(fp,
"\n"
		);
}


/*
 * Generate the header file for the C++ interface.
 */

static void generateIfaceHeader(sipSpec *pt,ifaceFileDef *iff,char *codeDir)
{
	char *impexp, *wfile;
	char *cmname = iff -> module -> name;
	classDef *cd;
	mappedTypeDef *mtd;
	int genused;
	FILE *fp;

	/* See if we are importing or exporting. */

	impexp = (iff -> module == pt -> module) ? "MODULE_EXTERN" : "IMPORT";

	/* Create the header file. */

	wfile = createIfaceFileName(codeDir,iff,".h");
	fp = createFile(pt,wfile,"//","C++ interface header file.");

	prcode(fp,
"\n"
"#ifndef _%s%F_h\n"
"#define	_%s%F_h\n"
"\n"
		,cmname,iff -> fqname,cmname,iff -> fqname);

	genused = TRUE;

	for (cd = pt -> classes; cd != NULL; cd = cd -> next)
		if (cd -> iff == iff)
		{
			generateClassHeader(cd,genused,impexp,pt,fp);

			genused = FALSE;
		}

	genused = TRUE;

	for (mtd = pt -> mappedtypes; mtd != NULL; mtd = mtd -> next)
		if (mtd -> iff == iff)
		{
			generateMappedTypeHeader(mtd,genused,impexp,pt,fp);

			genused = FALSE;
		}

	prcode(fp,
"\n"
"#endif\n"
		);

	closeFile(fp);
	free(wfile);
}


/*
 * Generate the C++ header code for a mapped type.
 */

static void generateMappedTypeHeader(mappedTypeDef *mtd,int genused,
				     char *impexp,sipSpec *pt,FILE *fp)
{
	prcode(fp,
"\n"
"\n"
		);

	generateCppCodeBlock(mtd -> hdrcode,NULL,fp);

	if (genused)
		generateUsedIncludes(mtd -> iff -> used,fp);

	generateConvertToDeclarations(mtd,NULL,impexp,fp);

	prcode(fp,
"extern SIP_%s PyObject *sipConvertFrom_%T Py_PROTO((%b *));\n"
		,impexp,&mtd -> type,&mtd -> type);
}


/*
 * Generate the C++ header code for a class.
 */

static void generateClassHeader(classDef *cd,int genused,char *impexp,
				sipSpec *pt,FILE *fp)
{
	varDef *vd;

	generateCppCodeBlock(cd -> hdrcode,cd,fp);

	if (genused)
		generateUsedIncludes(cd -> iff -> used,fp);

	if (isOpaque(cd))
		prcode(fp,
"\n"
"class %S;\n"
			,classFQName(cd));

	prcode(fp,
"\n"
"extern SIP_%s sipLazyAttrDef sipClassAttrTab_%C;\n"
		,impexp,classFQName(cd)
		,impexp,classFQName(cd));

	for (vd = pt -> vars; vd != NULL; vd = vd -> next)
		if (vd -> ecd == cd)
		{
			prcode(fp,
"extern SIP_%s PyMethodDef sipClassVarTab_%C[];\n"
"extern SIP_%s PyMethodDef *sipClassVarHierTab_%C[];\n"
				,impexp,classFQName(cd),impexp,classFQName(cd));

			break;
		}

	/* The cast and constructor functions. */

	prcode(fp,
"\n"
"extern SIP_%s const void *sipCast_%C Py_PROTO((const void *,PyObject *));\n"
"extern SIP_%s PyObject *sipNew_%C Py_PROTO((PyObject *,PyObject *));\n"
			,impexp,classFQName(cd),impexp,classFQName(cd));

	if (isComplex(cd))
		if (cannotCreate(cd))
			generateLimitedWrapperClassDeclaration(pt,cd,fp);
		else
			generateWrapperClassDeclaration(pt,cd,fp);

	generateConvertToDeclarations(NULL,cd,impexp,fp);
}


/*
 * Generate the "convert to" function declarations.
 */

static void generateConvertToDeclarations(mappedTypeDef *mtd,classDef *cd,
					  char *impexp,FILE *fp)
{
	codeBlock *convtocode;
	argDef type;

	if (cd != NULL)
	{
		convtocode = cd -> convtocode;

		type.atype = class_type;
		type.u.cd = cd;
	}
	else
	{
		convtocode = mtd -> convtocode;

		type.atype = mapped_type;
		type.u.mtd = mtd;
	}

	type.argflags = 0;
	type.nrderefs = 0;
	type.defval = NULL;

	prcode(fp,
"\n"
		);

	if (convtocode != NULL)
		prcode(fp,
"extern SIP_%s int sipConvertTo_%T Py_PROTO((PyObject *,%b **,int *));\n"
			,impexp,&type,&type);

	prcode(fp,
"extern SIP_%s %b *sipForceConvertTo_%T Py_PROTO((PyObject *,int *));\n"
		,impexp,&type,&type);
}


/*
 * Generate the declaration of a virtual handler.
 */

static void generateVirtualHandlerDeclaration(overDef *od,FILE *fp)
{
	argDef *res;
	funcArgs *args;

	getCppSignature(od,&res,&args);

	prcode(fp,
"	static ");

	generateResultType(res,fp);

	prcode(fp," sipVH_%s(const sipMethodCache *,sipThisType *",od -> cppname);

	if (args -> nrArgs > 0)
		prcode(fp,",");

	generateArgs(args,Declaration,fp);

	prcode(fp,");\n"
		);
}


/*
 * Generate the limited wrapper class declaration that might be needed by a
 * complex class that cannot be created in Python.
 */

static void generateLimitedWrapperClassDeclaration(sipSpec *pt,classDef *cd,
						   FILE *fp)
{
	int noIntro;
	virtOverDef *vod;

	noIntro = TRUE;

	for (vod = cd -> vmembers; vod != NULL; vod = vod -> next)
		if (vod -> farc == cd)
		{
			if (noIntro)
			{
				noIntro = FALSE;

				prcode(fp,
"\n"
"\n"
"class SIP_%s sip%C\n"
"{\n"
"public:\n"
					,(cd -> iff -> module == pt -> module) ? "MODULE_EXTERN" : "IMPORT"
					,classFQName(cd));
			}

			generateVirtualHandlerDeclaration(vod -> o,fp);
		}

	if (!noIntro)
		prcode(fp,
"};\n"
			);
}


/*
 * Generate the wrapper class declaration.
 */

static void generateWrapperClassDeclaration(sipSpec *pt,classDef *cd,FILE *fp)
{
	int noIntro, nrVirts;
	ctorDef *ct;
	visibleList *vl;
	virtOverDef *vod;

	nrVirts = countVirtuals(cd);

	prcode(fp,
"\n"
"\n"
"class SIP_%s sip%C : public %S\n"
"{\n"
"public:\n"
		,(cd -> iff -> module == pt -> module) ? "MODULE_EXTERN" : "IMPORT"
		,classFQName(cd),classFQName(cd));

	/* The constructor declarations. */

	for (ct = cd -> ctors; ct != NULL; ct = ct -> next)
	{
		if (isPrivateCtor(ct))
			continue;

		prcode(fp,
"	sip%C(",classFQName(cd));

		generateArgs((ct -> cppsig != NULL ? &ct -> cppsig -> args : &ct -> args),Declaration,fp);

		prcode(fp,")%X;\n"
			,ct -> exceptions);
	}

	/* The destructor. */

	if (!isPrivateDtor(cd))
		prcode(fp,
"	~sip%C()%X;\n"
			,classFQName(cd),cd -> dtorexceptions);

	/* The exposure of protected enums. */

	generateProtectedEnums(pt,cd,fp);

	/* The catcher around each virtual function in the hierarchy. */

	for (vod = cd -> vmembers; vod != NULL; vod = vod -> next)
	{
		overDef *od = vod -> o;
		argDef *res;
		funcArgs *args;

		if (vod -> farc == cd)
			generateVirtualHandlerDeclaration(od,fp);

		if (isPrivate(od))
			continue;

		prcode(fp,
"	");
 
		getCppSignature(od,&res,&args);

		generateResultType(res,fp);
 
		prcode(fp," %s(",od -> cppname);
		generateArgs(args,Declaration,fp);
 
		prcode(fp,")%s%X;\n"
			,(isConst(od) ? " const" : ""),od -> exceptions);
	}

	/* The wrapper around each protected member function. */

	generateProtectedDeclarations(pt,cd,fp);

	/* The public wrapper around each signal emitter. */

	noIntro = TRUE;

	for (vl = cd -> visible; vl != NULL; vl = vl -> next)
	{
		overDef *od;

		for (od = vl -> cd -> overs; od != NULL; od = od -> next)
		{
			if (od -> common != vl -> m || !isSignal(od))
				continue;

			if (noIntro)
			{
				prcode(fp,
"\n"
"	// There is a public member function for every Qt signal that can be\n"
"	// emitted by this object.  This function is called by Python to emit\n"
"	// the signal.\n"
"\n"
					);

				noIntro = FALSE;
			}

			prcode(fp,
"	int sipEmit_%s(PyObject *sipArgs);\n"
				,vl -> m -> pyname -> text);

			break;
		}
	}

	/* The private declarations. */

	prcode(fp,
"\n"
"	sipThisType *sipPyThis;\n"
		);

	prcode(fp,
"\n"
"private:\n"
"	sip%C(const sip%C &);\n"
"	sip%C &operator = (const sip%C &);\n"
		,classFQName(cd),classFQName(cd)
		,classFQName(cd),classFQName(cd));

	if (nrVirts > 0)
		prcode(fp,
"\n"
"	sipMethodCache sipPyMethods[%d];\n"
			,nrVirts);

	prcode(fp,
"};\n"
		);
}


/*
 * Generate the return type of a member function.
 */

static void generateResultType(argDef *ad,FILE *fp)
{
	if (ad == NULL)
		prcode(fp,"void");
	else
		generateSingleArg(ad,-1,Declaration,fp);
}


/*
 * Generate typed arguments.
 */

static void generateArgs(funcArgs *args,funcArgType ftype,FILE *fp)
{
	char *prefix = NULL;
	int a;

	for (a = 0; a < args -> nrArgs; ++a)
	{
		if (prefix == NULL)
			prefix = ",";
		else
			prcode(fp,prefix);

		generateSingleArg(&args -> args[a],a,ftype,fp);
	}
}


/*
 * Generate the declaration of a variable (excluding the name) to hold a result
 * from a C++ function call.
 */

static void generateValueType(argDef *ad,FILE *fp)
{
	argDef orig;

	orig = *ad;

	if (ad -> nrderefs == 0 && ad -> atype != class_type && ad -> atype != mapped_type)
		resetIsConstArg(ad);

	resetIsReference(ad);
	generateBaseType(ad,fp);
	*ad = orig;

	if (ad -> nrderefs == 0)
	{
		prcode(fp," ");

		if (ad -> atype == class_type || ad -> atype == mapped_type)
			prcode(fp,"*");
	}
}


/*
 * Generate a single argument.
 */

static void generateSingleArg(argDef *ad,int argnr,funcArgType ftype,FILE *fp)
{
	int genType, genName, derefPtr;

	/* Break the type down to individual modifications. */

	genType = FALSE;
	genName = FALSE;
	derefPtr = FALSE;

	switch (ftype)
	{
	case Call:
		genName = TRUE;
		derefPtr = TRUE;
		break;

	case Declaration:
		genType = TRUE;
		break;

	case Definition:
		genType = TRUE;
		genName = TRUE;
		break;
	}

	if (genType)
		generateBaseType(ad,fp);

	if (genName)
	{
		if (genType)
			prcode(fp," ");

		if (derefPtr)
		{
			char *ind = "";

			switch (ad -> atype)
			{
			case ustring_type:
			case string_type:
				if (ad -> nrderefs == 2)
					ind = "&";

				break;

			case mapped_type:
			case class_type:
				if (ad -> nrderefs == 2)
					ind = "&";
				else if (ad -> nrderefs == 0)
					ind = "*";

				break;

			case struct_type:
			case voidptr_type:
				if (ad -> nrderefs == 2)
					ind = "&";

				break;

			default:
				if (ad -> nrderefs == 1)
					ind = "&";
			}

			prcode(fp,ind);
		}

		prcode(fp,"a%d",argnr);
	}
}


/*
 * Generate a C++ type.
 */

static void generateBaseType(argDef *ad,FILE *fp)
{
	int nr_derefs;

	nr_derefs = ad -> nrderefs;

	if (isConstArg(ad))
		prcode(fp,"const ");

	switch (ad -> atype)
	{
	case ustring_type:
		prcode(fp,"unsigned char");
		break;

	case signal_type:
	case slot_type:
	case slotcon_type:
	case slotdis_type:
		nr_derefs = 1;

		/* Drop through. */

	case string_type:
		prcode(fp,"char");
		break;

	case ushort_type:
		prcode(fp,"unsigned short");
		break;

	case short_type:
		prcode(fp,"short");
		break;

	case uint_type:
		prcode(fp,"unsigned");
		break;

	case int_type:
	case cint_type:
		prcode(fp,"int");
		break;

	case ulong_type:
		prcode(fp,"unsigned long");
		break;

	case long_type:
		prcode(fp,"long");
		break;

	case struct_type:
		prcode(fp,"struct %S",ad -> u.sname);
		break;

	case voidptr_type:
		prcode(fp,"void");
		break;

	case bool_type:
		prcode(fp,"bool");
		break;

	case float_type:
	case cfloat_type:
		prcode(fp,"float");
		break;

	case double_type:
	case cdouble_type:
		prcode(fp,"double");
		break;

	case defined_type:
		/*
		 * The only defined types still remaining are arguments to
		 * templates.
		 */

		prcode(fp,"%S",ad -> u.snd);
		break;

	case enum_type:
		prcode(fp,"%E",ad -> u.ed);
		break;

	case rxcon_type:
	case rxdis_type:
		nr_derefs = 1;
		prcode(fp,"QObject");
		break;

	case mapped_type:
		generateBaseType(&ad -> u.mtd -> type,fp);
		break;

	case class_type:
		prcode(fp,"%S",classFQName(ad -> u.cd));
		break;

	case template_type:
		{
			int a;
			templateDef *td = ad -> u.td;

			prcode(fp,"%S<",td -> fqname);

			for (a = 0; a < td -> types.nrArgs; ++a)
			{
				if (a > 0)
					prcode(fp,",");

				generateBaseType(&td -> types.args[a],fp);
			}

			prcode(fp,">");
			break;
		}

	case pyobject_type:
	case pytuple_type:
	case pylist_type:
	case pydict_type:
	case pycallable_type:
	case pyslice_type:
	case qobject_type:
		prcode(fp,"PyObject *",fp);
		break;
	}

	if (nr_derefs > 0)
	{
		int i;

		prcode(fp," ");

		for (i = 0; i < nr_derefs; ++i)
			prcode(fp,"*");
	}

	if (isReference(ad))
		prcode(fp,"&");
}


/*
 * Generate the definition of a Python argument variable and any supporting
 * variables.
 */
static void generateVariable(argDef *ad,int argnr,int secCall,FILE *fp)
{
	argType atype = ad -> atype;
	argDef orig;

	if (isInArg(ad) && (atype == class_type || atype == mapped_type) &&
	    ad -> defval != NULL && ad -> defval -> vtype == fcall_value &&
	    (ad -> nrderefs == 0 || isReference(ad)))
	{
		/*
		 * Generate something to hold the default value as it cannot be
		 * assigned straight away.
		 */

		prcode(fp,
"		%B a%ddef = ",ad,argnr);
		generateSimpleFunctionCall(ad -> defval -> u.fcd,fp);
		prcode(fp,";\n"
			);
	}

	/* Adjust the type so we have the type that will really handle it. */

	orig = *ad;

	switch (atype)
	{
	case ustring_type:
	case string_type:
		if (!isReference(ad))
			if (ad -> nrderefs == 2)
				ad -> nrderefs = 1;
			else if (ad -> nrderefs == 1 && isOutArg(ad))
				ad -> nrderefs = 0;

		break;

	case mapped_type:
	case class_type:
	case voidptr_type:
	case struct_type:
		ad -> nrderefs = 1;
		break;

	default:
		ad -> nrderefs = 0;
	}

	/* Array sizes are always integers. */

	if (isArraySize(ad))
		ad -> atype = int_type;

	resetIsReference(ad);

	if (ad -> defval == NULL)
		resetIsConstArg(ad);

	prcode(fp,
"		%B a%d",ad,argnr);

	*ad = orig;

	/* Handle any default value. */
	if (isInArg(ad) && ad -> defval != NULL)
	{
		prcode(fp," = ");

		if (atype == class_type || atype == mapped_type)
		{
			if (ad -> defval -> vtype == fcall_value)
			{
				if (ad -> nrderefs == 0 || isReference(ad))
					prcode(fp,"&a%ddef",argnr);
				else
					generateSimpleFunctionCall(ad -> defval -> u.fcd,fp);
			}
			else if (ad -> defval -> vtype == scoped_value)
			{
				/* Cast if needed. */

				if (!isConstArg(ad))
					prcode(fp,"(%C *)",(atype == class_type ? ad -> u.cd -> iff : ad -> u.mtd -> iff) -> fqname);

				prcode(fp,"&%S",ad -> defval -> u.vscp);
			}
			else
				prcode(fp,"NULL");
		}
		else
			generateExpression(ad -> defval,fp);
	}

	prcode(fp,";\n"
		);

	/* Some types have supporting variables. */
	if (isInArg(ad))
		switch (atype)
		{
		case class_type:
			if (ad -> u.cd -> convtocode != NULL)
				prcode(fp,
"		int a%dIsTemp = 0;\n"
					,argnr);
			else if (isGetWrapper(ad))
				prcode(fp,
"		PyObject *a%dWrapper;\n"
					,argnr);

			break;

		case mapped_type:
			prcode(fp,
"		int a%dIsTemp = 0;\n"
				,argnr);
			break;
		}
}


/*
 * Generate a simple function call.
 */

static void generateSimpleFunctionCall(fcallDef *fcd,FILE *fp)
{
	int i;

	prcode(fp,"%S(",fcd -> name);

	for (i = 0; i < fcd -> nrArgs; ++i)
	{
		if (i > 0)
			prcode(fp,",");

		generateExpression(fcd -> args[i],fp);
	}

	prcode(fp,")");
}


/*
 * Generate the Python constructor function for the class.
 */

static void generatePythonConstructor(sipSpec *pt,classDef *cd,FILE *fp)
{
	char *mname = pt -> module -> name;
	ctorDef *ct;

	prcode(fp,
"\n"
"PyObject *sipNew_%C(PyObject *sipSelf,PyObject *sipArgs)\n"
"{\n"
"	static sipExtraType et = {\n"
"		sipCast_%C",classFQName(cd),classFQName(cd));

	if (pt -> sigargs != NULL)
	{
		prcode(fp,",\n"
"		(void *)sipNewProxy_%s,\n"
			,mname);

		if (hasSigSlots(cd))
			prcode(fp,
"		sipSignals_%C"
				,classFQName(cd));
		else
			prcode(fp,
"		NULL");
	}

	prcode(fp,"\n"
"	};\n"
"\n"
"	int sipFlags = SIP_PY_OWNED;\n"
"	int sipArgsParsed = 0;\n"
"	sipThisType *sipThis = NULL;\n"
		);

	if (isComplex(cd) && !cannotCreate(cd))
		prcode(fp,
"	sip%C *sipCpp = NULL;\n"
			,classFQName(cd));
	else
		prcode(fp,
"	%S *sipCpp = NULL;\n"
			,classFQName(cd));

	if (tracing)
		prcode(fp,
"\n"
"	sipTrace(SIP_TRACE_INITS,\"sipNew_%C()\\n\");\n"
			,classFQName(cd));

	prcode(fp,
"\n"
"	// See if there is something pending.\n"
"\n"
		);

	if (isComplex(cd) && !cannotCreate(cd))
		prcode(fp,
"	sipCpp = (sip%C *)sipGetPending(&sipFlags);\n"
			,classFQName(cd));
	else
		prcode(fp,
"	sipCpp = (%S *)sipGetPending(&sipFlags);\n"
			,classFQName(cd));

	/*
	 * Generate the code that parses the Python arguments and calls the
	 * correct constructor.
	 */

	if (!cannotCreate(cd))
		for (ct = cd -> ctors; ct != NULL; ct = ct -> next)
		{
			if (isPrivateCtor(ct))
				continue;

			prcode(fp,
"\n"
"	if (sipCpp == NULL)\n"
"	{\n"
				);

			/* Use any code supplied in the specification. */

			if (ct -> membercode != NULL)
				generateCppCodeBlock(ct -> membercode,cd,fp);
			else
			{
				int needSecCall, error_flag = FALSE;

				if (ct -> methodcode != NULL && needErrorFlag(ct -> methodcode))
				{
					prcode(fp,
"		int sipIsErr = FALSE;\n"
						);

					error_flag = TRUE;
				}

				needSecCall = generateArgParser(pt,&ct -> args,NULL,NULL,FALSE,fp);
				generateConstructorCall(pt,cd,ct,FALSE,error_flag,fp);

				if (needSecCall)
				{
					prcode(fp,
"	}\n"
"\n"
"	if (sipCpp == NULL)\n"
"	{\n"
						);

					if (error_flag)
						prcode(fp,
"		int sipIsErr = FALSE;\n"
							);

					generateArgParser(pt,&ct -> args,NULL,NULL,TRUE,fp);
					generateConstructorCall(pt,cd,ct,TRUE,error_flag,fp);
				}
			}

			prcode(fp,
"	}\n"
				);
		}

	prcode(fp,
"\n"
"	if (sipCpp == NULL)\n"
"	{\n"
"		sipNoCtor(sipArgsParsed,%N);\n"
"		return NULL;\n"
"	}\n"
"\n"
"	// Wrap the object.\n"
"\n"
"	if ((sipThis = sipCreateThis(sipSelf,sipCpp,&sipType_%C,sipFlags,&et)) == NULL)\n"
"	{\n"
		,cd -> iff -> name
		,classFQName(cd));

	if (!cannotCreate(cd) && (isPublicDtor(cd) || (isComplex(cd) && isProtectedDtor(cd))))
	{
		prcode(fp,
"		if (sipFlags & SIP_PY_OWNED)\n"
"		{\n"
"			Py_BEGIN_ALLOW_THREADS\n"
"\n"
			);

		if (isComplex(cd))
		{
			prcode(fp,
"			if (!(sipFlags & SIP_SIMPLE))\n"
"				delete (sip%C *)sipCpp;\n"
				,classFQName(cd));

			if (isPublicDtor(cd))
				prcode(fp,
"			else\n"
"				delete (%S *)sipCpp;\n"
					,classFQName(cd));
		}
		else
			prcode(fp,
"			delete (%S *)sipCpp;\n"
				,classFQName(cd));

		prcode(fp,
"\n"
"			Py_END_ALLOW_THREADS\n"
"		}\n"
"\n"
			);
	}

	prcode(fp,
"		return NULL;\n"
"	}\n"
		);

	if (isComplex(cd) && !cannotCreate(cd))
		prcode(fp,
"\n"
"	if (!(sipFlags & SIP_SIMPLE))\n"
"		((sip%C *)sipCpp) -> sipPyThis = sipThis;\n"
			,classFQName(cd));

	if (createsThread(cd))
		prcode(fp,
"\n"
"	PyEval_InitThreads();\n"
			);

	prcode(fp,
"\n"
"	Py_INCREF(Py_None);\n"
"	return Py_None;\n"
"}\n"
		);
}


/*
 * Count the number of virtual members in a class.
 */
 
static int countVirtuals(classDef *cd)
{
	int nrvirts;
	virtOverDef *vod;
 
	nrvirts = 0;
 
	for (vod = cd -> vmembers; vod != NULL; vod = vod -> next)
		if (!isPrivate(vod -> o))
			++nrvirts;
 
	return nrvirts;
}
 

/*
 * Generate the try block for a call.
 */
static void generateTry(throwArgs *ta,FILE *fp)
{
	/*
	 * Generate the block if there was no throw specifier, or a non-empty
	 * throw specifier.
	 */
	if (exceptions && (ta == NULL || ta -> nrArgs > 0))
		prcode(fp,
"			try\n"
"			{\n"
			);
}


/*
 * Generate the catch block for a call.
 */
static void generateCatch(throwArgs *ta,FILE *fp)
{
	/*
	 * Generate the block if there was no throw specifier, or a non-empty
	 * throw specifier.
	 */
	if (exceptions && (ta == NULL || ta -> nrArgs > 0))
	{
		prcode(fp,
"			}\n"
			);

		if (ta == NULL)
			prcode(fp,
"			catch (...)\n"
"			{\n"
"				Py_BLOCK_THREADS\n"
"\n"
"				sipRaiseUnknownException();\n"
"				return NULL;\n"
"			}\n"
				);
		else
		{
			int a;

			for (a = 0; a < ta -> nrArgs; ++a)
			{
				scopedNameDef *ename = classFQName(ta -> args[a]);

				prcode(fp,
"			catch (%S &e)\n"
"			{\n"
"				// Hope that there is a valid copy ctor.\n"
"				%S *sipCpp = new %S(e);\n"
"\n"
"				Py_BLOCK_THREADS\n"
"\n"
"				sipRaise%sClassException(sipClass_%C,sipCpp);\n"
"				return NULL;\n"
"			}\n"
					,ename
					,ename,ename
					,(ta -> args[a] -> subbase != NULL ? "Sub" : ""),ename);
			}
		}
	}
}


/*
 * Generate a throw specifier.
 */
static void generateThrowSpecifier(throwArgs *ta,FILE *fp)
{
	if (exceptions && ta != NULL)
	{
		int a;

		prcode(fp," throw(");

		for (a = 0; a < ta -> nrArgs; ++a)
		{
			if (a > 0)
				prcode(fp,",");

			prcode(fp,"%P",ta -> args[a]);
		}

		prcode(fp,")");
	}
}


/*
 * Generate a single constructor call.
 */

static void generateConstructorCall(sipSpec *pt,classDef *cd,ctorDef *ct,
				    int secCall,int error_flag,FILE *fp)
{
	prcode(fp,
"		{\n"
		);

	/* Call any pre-hook. */

	if (ct -> prehook != NULL)
		prcode(fp,
"			sipCallHook(\"%s\");\n"
"\n"
			,ct -> prehook);

	if (ct -> methodcode != NULL)
		generateCppCodeBlock(ct -> methodcode,cd,fp);
	else
	{
		prcode(fp,
"			Py_BEGIN_ALLOW_THREADS\n"
			);

		generateTry(ct -> exceptions,fp);

		if (isComplex(cd))
			prcode(fp,
"			sipCpp = new sip%C(",classFQName(cd));
		else
			prcode(fp,
"			sipCpp = new %S(",classFQName(cd));

		generateArgs(&ct -> args,Call,fp);

		prcode(fp,");\n"
			);

		generateCatch(ct -> exceptions,fp);

		prcode(fp,
"			Py_END_ALLOW_THREADS\n"
			);
	}

	generateCallTidyUp(&ct -> args,fp);

	if (error_flag)
		prcode(fp,
"\n"
"			if (sipIsErr)\n"
"				return NULL;\n"
			);

	/* Call any post-hook. */

	if (ct -> posthook != NULL)
		prcode(fp,
"\n"
"			sipCallHook(\"%s\");\n"
			,ct -> posthook);

	prcode(fp,
"		}\n"
		);
}


/*
 * See if a member overload should be skipped.
 */
static int skipOverload(overDef *od,memberDef *md,classDef *cd,classDef *ccd,
			int want_local)
{
	/* Skip if it's not the right name. */

	if (od -> common != md)
		return TRUE;

	/* Skip if it's a signal. */

	if (isSignal(od))
		return TRUE;

	/* Skip if it's a private abstract. */

	if (isAbstract(od) && isPrivate(od))
		return TRUE;

	/*
	 * If we are disallowing them, skip if it's not in the current class
	 * unless it is protected.
	 */
	if (want_local && !isProtected(od) && ccd != cd)
		return TRUE;

	return FALSE;
}


/*
 * Generate a class member function.
 */

static void generateFunction(sipSpec *pt,memberDef *md,overDef *overs,
			     classDef *cd,classDef *ocd,FILE *fp)
{
	overDef *od;

	/*
	 * Check that there is at least one overload that needs to be handled.
	 */
	for (od = overs; od != NULL; od = od -> next)
	{
		/*
		 * Skip protected methods if we don't have the means to access
		 * them.
		 */

		if (isProtected(od) && !isComplex(cd))
			continue;

		if (!skipOverload(od,md,cd,ocd,TRUE))
			break;
	}

	if (od != NULL)
	{
		char *pname = md -> pyname -> text;

		prcode(fp,
"\n"
"static PyObject *sipDo_%C_%s(PyObject *%s,PyObject *sipArgs)\n"
"{\n"
"	int sipArgsParsed = 0;\n"
			,classFQName(cd),pname,(md -> slot == no_slot ? "sipThisObj" : "sipSelf"));

		if (tracing)
			prcode(fp,
"\n"
"	sipTrace(SIP_TRACE_METHODS,\"sipDo_%C_%s()\\n\");\n"
				,classFQName(cd),pname);

		for (od = overs; od != NULL; od = od -> next)
		{
			/*
			 * If we are handling one variant then we must handle
			 * them all.
			 */
			if (skipOverload(od,md,cd,ocd,FALSE))
				continue;

			generateFunctionBody(pt,od,cd,ocd,fp);
		}

		prcode(fp,
"\n"
			);

		if (md -> slot == cmp_slot || isRichCompareSlot(md))
			prcode(fp,
"#if PY_VERSION_HEX >= 0x02010000\n"
"	PyErr_Clear();\n"
"\n"
"	Py_INCREF(Py_NotImplemented);\n"
"	return Py_NotImplemented;\n"
"#else\n"
				);

		prcode(fp,
"	// Report an error if the arguments couldn't be parsed.\n"
"\n"
"	sipNoMethod(sipArgsParsed,%N,%N);\n"
"\n"
"	return NULL;\n"
			,cd -> iff -> name,md -> pyname);

		if (md -> slot == cmp_slot || isRichCompareSlot(md))
			prcode(fp,
"#endif\n"
				);

		prcode(fp,
"}\n"
			);
	}
}


/*
 * Generate the function calls for a particular overload.
 */

static void generateFunctionBody(sipSpec *pt,overDef *od,classDef *cd,
				 classDef *ocd,FILE *fp)
{
	prcode(fp,
"\n"
"	{\n"
		);

	/* Use any code supplied in the specification. */

	if (od -> membercode != NULL)
		generateCppCodeBlock(od -> membercode,cd,fp);
	else if (!isPrivate(od))
	{
		int needSecCall;

		needSecCall = generateArgParser(pt,&od -> args,cd,od,FALSE,fp);
		generateFunctionCall(cd,ocd,od,FALSE,fp);

		if (needSecCall)
		{
			prcode(fp,
"	}\n"
"\n"
"	{\n"
				);

			generateArgParser(pt,&od -> args,cd,od,TRUE,fp);
			generateFunctionCall(cd,ocd,od,TRUE,fp);
		}
	}

	prcode(fp,
"	}\n"
		);
}


/*
 * Return TRUE if an overload is an inplace slot.
 */
static int isInplaceSlot(memberDef *md)
{
	slotType slot = md -> slot;

	return (slot == iadd_slot || slot == isub_slot || slot == imul_slot ||
		slot == idiv_slot || slot == imod_slot ||
		slot == ior_slot || slot == ixor_slot || slot == iand_slot ||
		slot == ilshift_slot || slot == irshift_slot);
}


/*
 * Returns TRUE if the given method is a rich compare slot.
 */
static int isRichCompareSlot(memberDef *md)
{
	slotType st = md -> slot;

	return (st == lt_slot || st == le_slot || st == eq_slot ||
		st == ne_slot || st == gt_slot || st == ge_slot);
}


/*
 * Generate the code to handle the result of a call to a member function.
 */

static void generateHandleResult(overDef *od,int isNew,FILE *fp)
{
	char *vname, vnamebuf[50];
	int a, nrvals, only;
	argDef *ad;

	if (isInplaceSlot(od -> common))
	{
		prcode(fp,
"			Py_INCREF(sipSelf);\n"
"			return sipSelf;\n"
			);

		return;
	}

	/* See if we are returning 0, 1 or more values. */
	nrvals = 0;

	if (od -> result != NULL)
	{
		only = -1;
		++nrvals;
	}

	for (a = 0; a < od -> args.nrArgs; ++a)
		if (isOutArg(&od -> args.args[a]))
		{
			only = a;
			++nrvals;
		}

	/* Handle the trivial case. */
	if (nrvals == 0)
	{
		prcode(fp,
"			Py_INCREF(Py_None);\n"
"			return Py_None;\n"
			);

		return;
	}

	/* Handle results that are classes or mapped types separately. */
	if (od -> result != NULL)
		if (od -> result -> atype == mapped_type)
		{
			if (isConstArg(od -> result))
				prcode(fp,
"			PyObject *sipResObj = sipConvertFrom_%T((%b *)sipRes);\n"
					,od -> result,od -> result);
			else
				prcode(fp,
"			PyObject *sipResObj = sipConvertFrom_%T(sipRes);\n"
					,od -> result);

			if (isNew)
				prcode(fp,
"			delete sipRes;\n"
					);

			/* Shortcut if this is the only value returned. */
			if (nrvals == 1)
			{
				prcode(fp,
"\n"
"			return sipResObj;\n"
					);

				return;
			}
		}
		else if (od -> result -> atype == class_type)
		{
			classDef *cd = od -> result -> u.cd;

			if (isNew || isFactory(od))
			{
				prcode(fp,
"			%s sipNewCppToSelf%s(sipRes,sipClass_%C,%s);\n"
					,(nrvals == 1 ? "return" : "PyObject *sipResObj ="),(cd -> subbase != NULL ? "SubClass" : ""),classFQName(cd),(isFactory(od) ? "sipFlags" : "SIP_SIMPLE|SIP_PY_OWNED"));

				/*
				 * Shortcut if this is the only value returned.
				 */
				if (nrvals == 1)
					return;
			}
			else if (isResultTransferredBack(od))
			{
				prcode(fp,
"			PyObject *sipResObj = sipMapCppToSelf%s(sipRes,sipClass_%C);\n"
"			sipTransferSelf(sipResObj,0);\n"
					,(cd -> subbase != NULL ? "SubClass" : ""),classFQName(cd));

				/*
				 * Shortcut if this is the only value returned.
				 */
				if (nrvals == 1)
				{
					prcode(fp,
"\n"
"			return sipResObj;\n"
						);

					return;
				}
			}
			else
			{
				prcode(fp,
"			%s sipMapCppToSelf%s(sipRes,sipClass_%C);\n"
					,(nrvals == 1 ? "return" : "PyObject *sipResObj ="),(cd -> subbase != NULL ? "SubClass" : ""),classFQName(cd));

				/*
				 * Shortcut if this is the only value returned.
				 */
				if (nrvals == 1)
					return;
			}
		}

        /* If there are multiple values then build a tuple. */
	if (nrvals > 1)
	{
		prcode(fp,
"			return sipBuildResult(0,\"(");

		/* Build the format string. */
		if (od -> result != NULL)
			prcode(fp,"%c",((od -> result -> atype == mapped_type || od -> result -> atype == class_type) ? 'R' : getBuildResultFormat(od -> result)));

		for (a = 0; a < od -> args.nrArgs; ++a)
		{
			argDef *ad = &od -> args.args[a];

			if (isOutArg(ad))
				prcode(fp,"%c",getBuildResultFormat(ad));
		}

		prcode(fp,")\"");

		/* Pass the values for conversion. */
		if (od -> result != NULL)
		{
			prcode(fp,",sipRes");

			if (od -> result -> atype == mapped_type || od -> result -> atype == class_type)
				prcode(fp,"Obj");
		}

		for (a = 0; a < od -> args.nrArgs; ++a)
		{
			argDef *ad = &od -> args.args[a];

			if (isOutArg(ad))
			{
				prcode(fp,",a%d",a);

				if (ad -> atype == mapped_type)
					prcode(fp,",sipConvertFrom_%T",ad);
				else if (ad -> atype == class_type)
					prcode(fp,",sipClass_%C",classFQName(ad -> u.cd));
			}
		}

		prcode(fp,");\n"
			);

		/* All done for multiple values. */
		return;
	}

	/* Deal with the only returned value. */
	if (only < 0)
	{
		ad = od -> result;
		vname = "sipRes";
	}
	else
	{
		ad = &od -> args.args[only];

		sprintf(vnamebuf,"a%d",only);
		vname = vnamebuf;
	}

	switch (ad -> atype)
	{
	case mapped_type:
		if (isConstArg(ad))
			prcode(fp,
"			return sipConvertFrom_%T((%b *)%s);\n"
				,ad,ad,vname);
		else
			prcode(fp,
"			return sipConvertFrom_%T(%s);\n"
				,ad,vname);

		break;

	case class_type:
		{
			classDef *cd = ad -> u.cd;

			if (needNewInstance(ad))
				prcode(fp,
"			return sipNewCppToSelf(%s,sipClass_%C,SIP_PY_OWNED|SIP_SIMPLE);\n"
				,vname,classFQName(cd));
			else
				prcode(fp,
"			return sipMapCppToSelf%s(%s,sipClass_%C);\n"
				,(cd -> subbase != NULL ? "SubClass" : ""),vname,classFQName(cd));
		}

		break;

	case bool_type:
		prcode(fp,
"			return sipConvertFromBool((int)%s);\n"
			,vname);

		break;

	case ustring_type:
	case string_type:
		prcode(fp,
"			return sipBuildResult(0,\"%c\",%s);\n"
			,(ad -> nrderefs == 0 ? 'c' : 's'),vname);

		break;

	case enum_type:
	case ushort_type:
	case short_type:
	case uint_type:
	case int_type:
	case cint_type:
	case ulong_type:
		prcode(fp,
"			return PyInt_FromLong((long)%s);\n"
			,vname);

		break;

	case long_type:
		prcode(fp,
"			return PyInt_FromLong(%s);\n"
			,vname);

		break;

	case voidptr_type:
		prcode(fp,
"			return sipConvertFromVoidPtr(%s);\n"
			,vname);

		break;

	case struct_type:
		prcode(fp,
"			return sipConvertFromVoidPtr(");

		if (isConstArg(ad))
			prcode(fp,"const_cast<%b *>(%s)",ad,vname);
		else
			prcode(fp,"%s",vname);

		prcode(fp,");\n"
			);

		break;

	case float_type:
	case cfloat_type:
		prcode(fp,
"			return PyFloat_FromDouble((double)%s);\n"
			,vname);

		break;

	case double_type:
	case cdouble_type:
		prcode(fp,
"			return PyFloat_FromDouble(%s);\n"
			,vname);

		break;

	case pyobject_type:
	case pytuple_type:
	case pylist_type:
	case pydict_type:
	case pycallable_type:
	case pyslice_type:
		prcode(fp,
"			return %s;\n"
			,vname);

		break;
	}
}


/*
 * Return the format character used by sipBuildResult() for a particular type.
 */

static char getBuildResultFormat(argDef *ad)
{
	switch (ad -> atype)
	{
	case mapped_type:
		return 'T';

	case class_type:
		if (needNewInstance(ad))
			return 'P';

		return (ad -> u.cd -> subbase != NULL) ? 'M' : 'O';

	case bool_type:
		return 'b';

	case ustring_type:
	case string_type:
		return (ad -> nrderefs == 0) ? 'c' : 's';

	case enum_type:
		return 'e';

	case ushort_type:
	case short_type:
		return 'h';

	case uint_type:
	case int_type:
	case cint_type:
		return 'i';

	case ulong_type:
	case long_type:
		return 'l';

	case voidptr_type:
	case struct_type:
		return 'V';

	case float_type:
	case cfloat_type:
		return 'f';

	case double_type:
	case cdouble_type:
		return 'd';

	case pyobject_type:
	case pytuple_type:
	case pylist_type:
	case pydict_type:
	case pycallable_type:
	case pyslice_type:
		return 'R';
	}

	/* We should never get here. */
	return ' ';
}


/*
 * Generate a function call.
 */

static void generateFunctionCall(classDef *cd,classDef *ocd,overDef *od,
				 int secCall,FILE *fp)
{
	int needsNew = FALSE, error_flag = FALSE, newline, is_result = FALSE, a;
	argDef *res = od -> result, orig_res;

	prcode(fp,
"		{\n"
		);

	/*
	 * If we can never create this class from Python, then all protected
	 * methods can never be called.
	 */
	if (isProtected(od) && cannotCreate(cd))
	{
		prcode(fp,
"			/* Never reached. */\n"
"		}\n"
			);

		return;
	}

	if (needWrapper(od) && needCpp(od))
	{
		prcode(fp,
"			if ((sipCpp = (");

		if (isProtected(od) && !cannotCreate(cd))
			prcode(fp,"sip%C",classFQName(cd));
		else
			prcode(fp,"%S",classFQName(cd));

		prcode(fp," *)sipGetCppPtr((sipThisType *)sipSelf,sipClass_%C)) == NULL)\n"
"				return NULL;\n"
"\n"
			,classFQName(cd));
	}

	/* Save the full result type as we may want to fiddle with it. */

	if (res != NULL)
	{
		orig_res = *res;

		/* See if we need to make a copy of the result on the heap. */

		if ((res -> atype == class_type || res -> atype == mapped_type) &&
		    !isReference(res) &&
		    res -> nrderefs == 0)
		{
			needsNew = TRUE;
			resetIsConstArg(res);
		}

		/* See if sipRes is needed. */

		if (!isInplaceSlot(od -> common))
			is_result = TRUE;
	}

	newline = FALSE;

	if (isFactory(od))
	{
		prcode(fp,
"			int sipFlags = SIP_SIMPLE|SIP_PY_OWNED;\n"
			);

		newline = TRUE;
	}

	if (is_result)
	{
		prcode(fp,
"			");
		generateValueType(res,fp);
		prcode(fp,"sipRes;\n"
			);

		newline = TRUE;
	}

	for (a = 0; a < od -> args.nrArgs; ++a)
	{
		argDef *ad = &od -> args.args[a];

		/*
		 * If we are returning a class via an output only reference or
		 * pointer then we need an instance on the heap.
		 */
		if (needNewInstance(ad))
			prcode(fp,
"			a%d = new %b();\n"
				,a,ad);
	}

	if (od -> methodcode != NULL)
	{
		/*
		 * See if the handwritten code seems to be using the error
		 * flag.
		 */

		if (needErrorFlag(od -> methodcode))
		{
			prcode(fp,
"			int sipIsErr = FALSE;\n"
				);

			newline = TRUE;
			error_flag = TRUE;
		}
	}

	if (newline)
		prcode(fp,
"\n"
			);

	/* Call any pre-hook. */

	if (od -> prehook != NULL)
		prcode(fp,
"			sipCallHook(\"%s\");\n"
			,od -> prehook);

	if (od -> methodcode != NULL)
		generateCppCodeBlock(od -> methodcode,cd,fp);
	else
	{
		prcode(fp,
"			Py_BEGIN_ALLOW_THREADS\n"
			);

		generateTry(od -> exceptions,fp);

		prcode(fp,
"			");

		if (od -> common -> slot != cmp_slot && is_result)
		{
			prcode(fp,"sipRes = ");

			/* Construct a copy on the heap if needed. */

			if (needsNew)
				prcode(fp,"new %b(",res);

			/* See if we need the address of the result. */

			if ((res -> atype == class_type || res -> atype == mapped_type) && isReference(res))
				prcode(fp,"&");
		}

		switch(od -> common -> slot)
		{
		case no_slot:
			generateCppFunctionCall(cd,ocd,od,fp);
			break;

		case int_slot:
			prcode(fp,"*sipCpp");
			break;

		case add_slot:
			generateBinarySlotCall(od,"+",fp);
			break;

		case sub_slot:
			generateBinarySlotCall(od,"-",fp);
			break;

		case mul_slot:
			generateBinarySlotCall(od,"*",fp);
			break;

		case div_slot:
			generateBinarySlotCall(od,"/",fp);
			break;

		case mod_slot:
			generateBinarySlotCall(od,"%",fp);
			break;

		case and_slot:
			generateBinarySlotCall(od,"&",fp);
			break;

		case or_slot:
			generateBinarySlotCall(od,"|",fp);
			break;

		case xor_slot:
			generateBinarySlotCall(od,"^",fp);
			break;

		case lshift_slot:
			generateBinarySlotCall(od,"<<",fp);
			break;

		case rshift_slot:
			generateBinarySlotCall(od,">>",fp);
			break;

		case iadd_slot:
			generateBinarySlotCall(od,"+=",fp);
			break;

		case isub_slot:
			generateBinarySlotCall(od,"-=",fp);
			break;

		case imul_slot:
			generateBinarySlotCall(od,"*=",fp);
			break;

		case idiv_slot:
			generateBinarySlotCall(od,"/=",fp);
			break;

		case imod_slot:
			generateBinarySlotCall(od,"%=",fp);
			break;

		case iand_slot:
			generateBinarySlotCall(od,"&=",fp);
			break;

		case ior_slot:
			generateBinarySlotCall(od,"|=",fp);
			break;

		case ixor_slot:
			generateBinarySlotCall(od,"^=",fp);
			break;

		case ilshift_slot:
			generateBinarySlotCall(od,"<<=",fp);
			break;

		case irshift_slot:
			generateBinarySlotCall(od,">>=",fp);
			break;

		case invert_slot:
			prcode(fp,"~(*sipCpp)");
			break;

		case lt_slot:
			generateBinarySlotCall(od,"<",fp);
			break;

		case le_slot:
			generateBinarySlotCall(od,"<=",fp);
			break;

		case eq_slot:
			generateBinarySlotCall(od,"==",fp);
			break;

		case ne_slot:
			generateBinarySlotCall(od,"!=",fp);
			break;

		case gt_slot:
			generateBinarySlotCall(od,">",fp);
			break;

		case ge_slot:
			generateBinarySlotCall(od,">=",fp);
			break;

		case neg_slot:
			prcode(fp,"-(*sipCpp)");
			break;

                case cmp_slot:
			prcode(fp,"if ");
			generateBinarySlotCall(od,"<",fp);
			prcode(fp,"\n"
"				sipRes = -1;\n"
"			else if ");
			generateBinarySlotCall(od,">",fp);
			prcode(fp,"\n"
"				sipRes = 1;\n"
"			else\n"
"				sipRes = 0");

			break;
		}

		if (needsNew)
			prcode(fp,")");

		prcode(fp,";\n"
			);

		generateCatch(od -> exceptions,fp);

		prcode(fp,
"			Py_END_ALLOW_THREADS\n"
			);
	}

        generateCallTidyUp(&od -> args,fp);

	prcode(fp,
"\n"
		);

	/* Handle sipIsErr if it was used. */

	if (error_flag)
		prcode(fp,
"			if (sipIsErr)\n"
"                               return NULL;\n"
"\n"
			);

        /* Call any post-hook. */

        if (od -> posthook != NULL)
		prcode(fp,
"\n"
"			sipCallHook(\"%s\");\n"
			,od -> posthook);

	generateHandleResult(od,needsNew,fp);

	prcode(fp,
"		}\n"
		);

	/* Restore the full type of the result. */

	if (res != NULL)
		*res = orig_res;
}


/*
 * Generate a call to a C++ function.
 */

static void generateCppFunctionCall(classDef *cd,classDef *ocd,overDef *od,
				    FILE *fp)
{
	char *mname = od -> cppname;

	/*
	 * If the function is protected then call the public wrapper.
	 * Otherwise explicitly call the real function and not any version in
	 * the wrapper class in case it is virtual.  This will prevent virtual
	 * loops.  The exception is pure virtual functions as this is the way
	 * we hook into the catcher.  Don't need to worry about indirected
	 * objects for protected functions.
	 */

	if (cd == NULL)
		prcode(fp,"%s(",mname);
	else if (cd -> iff -> type == namespace_iface)
		prcode(fp,"%S::%s(",classFQName(cd),mname);
	else if (isStatic(od))
	{
		if (isProtected(od))
			prcode(fp,"sip%C::sipProtect_%s(",classFQName(cd),mname);
		else
			prcode(fp,"%S::%s(",classFQName(ocd),mname);
	}
	else if (isAbstract(od))
		prcode(fp,"sipCpp -> %s(",mname);
	else if (isProtected(od))
		prcode(fp,"sipCpp -> sipProtect_%s(",mname);
	else
		prcode(fp,"sipCpp -> %S::%s(",classFQName(ocd),mname);

	generateArgs(&od -> args,Call,fp);

	prcode(fp,")");
}


/*
 * Generate the call to a binary Python slot method.
 */

static void generateBinarySlotCall(overDef *od,char *op,FILE *fp)
{
	argDef *ad;
	int deref;

	ad = &od -> args.args[0];
	deref = ((ad -> atype == class_type || ad -> atype == mapped_type) && ad -> nrderefs == 0);

	prcode(fp,"((*sipCpp) %s %sa0)",op,(deref ? "*" : ""));
}


/*
 * Generate the argument variables for a member function/constructor.
 */

static int generateArgParser(sipSpec *pt,funcArgs *args,classDef *cd,
			     overDef *od,int secCall,FILE *fp)
{
	char *mname = pt -> module -> name;
	int a, needSecCall, optargs, arraylenarg, sigarg, need_wrapper, newl;
	int slotconarg, slotdisarg;

	newl = FALSE;

	/* If the class is just a namespace, then ignore it. */

	if (cd != NULL && cd -> iff -> type == namespace_iface)
		cd = NULL;

	/* Assume there isn't a slot. */

	needSecCall = FALSE;

	/* See if we need to provide the wrapper. */

	if (od != NULL)
	{
		need_wrapper = needWrapper(od);

		if (need_wrapper)
		{
			prcode(fp,
"		PyObject *sipSelf;\n"
				);

			newl = TRUE;
		}
	}

	/* Generate the local variables that will hold the parsed arguments. */

	sigarg = -1;

	for (a = 0; a < args -> nrArgs; ++a)
	{
		argDef *ad = &args -> args[a];

		switch (ad -> atype)
		{
		case signal_type:
			sigarg = a;
			break;

		case rxcon_type:
			needSecCall = TRUE;
			break;

		case rxdis_type:
			needSecCall = TRUE;
			break;

		case slotcon_type:
			slotconarg = a;
			break;

		case slotdis_type:
			slotdisarg = a;
			break;
		}

		if (isArraySize(ad))
			arraylenarg = a;

		generateVariable(ad,a,secCall,fp);

		newl = TRUE;
	}

	if (od != NULL && !isStatic(od) && cd != NULL)
		if (!need_wrapper || needCpp(od))
		{
			if (isProtected(od) && !cannotCreate(cd))
				prcode(fp,
"		sip%C *sipCpp;\n"
					,classFQName(cd));
			else
				prcode(fp,
"		%S *sipCpp;\n"
					,classFQName(cd));

			newl = TRUE;
		}

	if (newl)
		prcode(fp,
"\n"
			);

	/* Generate the call to sipParseArgs(). */

	prcode(fp,
"		if (sipParseArgs(&sipArgsParsed,sipArgs,\"");

	/* Generate the format string. */

	optargs = FALSE;

	if (od != NULL && !isStatic(od) && cd != NULL)
	{
		char ch;

		if (od -> common -> slot != no_slot)
			ch = 'n';
		else if (isProtected(od))
			ch = 'p';
		else if (need_wrapper)
			ch = 't';
		else
			ch = 'm';

		prcode(fp,"%c",ch);
	}

	for (a = 0; a < args -> nrArgs; ++a)
	{
		char *fmt = "";
		argDef *ad = &args -> args[a];

		if (!isInArg(ad))
			continue;

		if (ad -> defval != NULL && !optargs)
		{
			prcode(fp,"|");
			optargs = TRUE;
		}

		switch (ad -> atype)
		{
		case ustring_type:
		case string_type:
			if (ad -> nrderefs == 0 || (isOutArg(ad) && ad -> nrderefs == 1))
				fmt = "c";
			else if (isArray(ad))
				fmt = "a";
			else
				fmt = "s";

			break;

		case enum_type:
			fmt = "e";
			break;

		case bool_type:
			fmt = "b";
			break;

		case uint_type:
		case int_type:
			if (!isArraySize(ad))
				fmt = "i";

			break;

		case cint_type:
			fmt = "Xi";
			break;

		case ushort_type:
		case short_type:
			if (!isArraySize(ad))
				fmt = "h";

			break;

		case ulong_type:
		case long_type:
			if (!isArraySize(ad))
				fmt = "l";

			break;

		case struct_type:
		case voidptr_type:
			fmt = "v";
			break;

		case float_type:
			fmt = "f";
			break;

		case cfloat_type:
			fmt = "Xf";
			break;

		case double_type:
			fmt = "d";
			break;

		case cdouble_type:
			fmt = "Xd";
			break;

		case signal_type:
			fmt = "G";
			break;

		case slot_type:
			fmt = "S";
			break;

		case slotcon_type:
		case slotdis_type:
			fmt = (secCall ? "" : "S");
			break;

		case rxcon_type:
			fmt = (secCall ? "y" : "q");
			break;

		case rxdis_type:
			fmt = (secCall ? "Y" : "Q");
			break;

		case mapped_type:
			fmt = getSubFormatChar('M',ad);
			break;

		case class_type:
			fmt = getSubFormatChar((ad -> u.cd -> convtocode != NULL ? 'M' : 'J'),ad);
			break;

		case pyobject_type:
			fmt = getSubFormatChar('P',ad);
			break;

		case pytuple_type:
		case pylist_type:
		case pydict_type:
		case pyslice_type:
			fmt = (isAllowNone(ad) ? "N" : "T");
			break;

		case pycallable_type:
			fmt = "F";
			break;

		case qobject_type:
			fmt = "R";
			break;
		}

		prcode(fp,fmt);
	}

	prcode(fp,"\"");

	/* Generate the parameters corresponding to the format string. */

	if (od != NULL && !isStatic(od) && cd != NULL)
		prcode(fp,",%s,sipClass_%C,&sip%s",(od -> common -> slot == no_slot ? "sipThisObj" : "&sipSelf"),classFQName(cd),(need_wrapper ? "Self" : "Cpp"));

	for (a = 0; a < args -> nrArgs; ++a)
	{
		argDef *ad = &args -> args[a];

		if (!isInArg(ad))
			continue;

		switch (ad -> atype)
		{
		case mapped_type:
			prcode(fp,",sipConvertTo_%T,&a%d,&a%dIsTemp",ad,a,a);
			break;

		case class_type:
			if (ad -> u.cd -> convtocode != NULL)
				prcode(fp,",sipConvertTo_%T,&a%d,&a%dIsTemp",ad,a,a);
			else
			{
				prcode(fp,",sipClass_%T,&a%d",ad,a);

				if (isGetWrapper(ad))
					prcode(fp,",&a%dWrapper",a);
			}

			break;

		case rxcon_type:
			{
				prcode(fp,",sipNewProxy_%s",mname);

				if (sigarg > 0)
					prcode(fp,",a%d",sigarg);
				else
				{
					prcode(fp,",\"(");

					generateArgs(args -> args[slotconarg].u.sa,Declaration,fp);

					prcode(fp,")\"");
				}

				prcode(fp,",&a%d,&a%d",a,slotconarg);

				break;
			}

		case rxdis_type:
			{
				prcode(fp,",\"(");

				generateArgs(args -> args[slotdisarg].u.sa,Declaration,fp);

				prcode(fp,")\",&a%d,&a%d",a,slotdisarg);

				break;
			}

		case slotcon_type:
		case slotdis_type:
			if (!secCall)
				prcode(fp,",&a%d",a);

			break;

		case pytuple_type:
			prcode(fp,",&PyTuple_Type,&a%d",a);
			break;

		case pylist_type:
			prcode(fp,",&PyList_Type,&a%d",a);
			break;

		case pydict_type:
			prcode(fp,",&PyDict_Type,&a%d",a);
			break;

		case pyslice_type:
			prcode(fp,",&PySlice_Type,&a%d",a);
			break;

		default:
			if (!isArraySize(ad))
				prcode(fp,",&a%d",a);

			if (isArray(ad))
				prcode(fp,",&a%d",arraylenarg);
		}
	}

	prcode(fp,"))\n");

	return needSecCall;
}


/*
 * Get the format character string for something that has sub-formats.
 */

static char *getSubFormatChar(int fc,argDef *ad)
{
	static char fmt[3];
	int flags;

	fmt[0] = fc;

	flags = 0;

	if (fc != 'P' && ad -> nrderefs == 0)
		flags |= 0x01;

	if (isTransferred(ad))
		flags |= 0x02;

	if (isTransferredBack(ad))
		flags |= 0x04;

	if (fc == 'J' && isGetWrapper(ad))
		flags |= 0x08;

	fmt[1] = '0' + flags;

	fmt[2] = '\0';

	return fmt;
}


/*
 * Generate the code to tidy up after a call to C++.
 */

static void generateCallTidyUp(funcArgs *args,FILE *fp)
{
	int a;

	for (a = 0; a < args -> nrArgs; ++a)
	{
		argDef *ad = &args -> args[a];
		codeBlock *convtocode;

		if (!isInArg(ad))
			continue;

		if (ad -> atype == class_type)
			convtocode = ad -> u.cd -> convtocode;
		else if (ad -> atype == mapped_type)
			convtocode = ad -> u.mtd -> convtocode;
		else
			continue;

		/* Delete temporary instances created by class convertors. */

		if (convtocode != NULL)
			prcode(fp,
"\n"
"			if (a%dIsTemp)\n"
"				delete a%d;\n"
				,a,a);

		if (isThisTransferred(ad))
			prcode(fp,
"\n"
"			if (a%d)\n"
"				sipFlags |= SIP_XTRA_REF;\n"
				,a);
	}
}


/*
 * Generate a C++ code block.
 */

static void generateCppCodeBlock(codeBlock *code,classDef *cd,FILE *fp)
{
	codeBlock *cb;

	for (cb = code; cb != NULL; cb = cb -> next)
	{
		prcode(fp,
"#line %d \"%s\"\n"
			,cb -> linenr,cb -> filename);

		generateMacroCode(cb,cd,NULL,NULL,NULL,-1,fp);
	}

	if (code != NULL)
	{
		char *bn;

		/* Just use the base name. */

		if ((bn = strrchr(currentFileName,'/')) != NULL)
			++bn;
		else
			bn = currentFileName;

		prcode(fp,
"#line %d \"%s\"\n"
			,currentLineNr + 1,bn);
	}
}


/*
 * Generate a code fragment with macro expansion.
 */

static void generateMacroCode(codeBlock *cb,classDef *cd,sipSpec *pt,
			      char *cppSuffix,char *objSuffix,int parts,
			      FILE *fp)
{
	char *cp;

	for (cp = cb -> frag; *cp != '\0'; ++cp)
	{
		char mc;
		int invalid;

		if (*cp != MACRO_ESCAPE)
		{
			prcode(fp,"%c",*cp);
			continue;
		}

		invalid = FALSE;

		switch (mc = *++cp)
		{
		case MACRO_ESCAPE:
			prcode(fp,"%c",*cp);
			break;

		case MACRO_CLASS:
			if (cd != NULL)
				prcode(fp,"%C",classFQName(cd));
			else
				invalid = TRUE;

			break;

		case MACRO_CPPSUFF:
			if (cppSuffix != NULL)
				prcode(fp,"%s",cppSuffix);
			else
				invalid = TRUE;

			break;

		case MACRO_OBJSUFF:
			if (objSuffix != NULL)
				prcode(fp,"%s",objSuffix);
			else
				invalid = TRUE;

			break;

		case MACRO_OBJECTS:
		case MACRO_SOURCES:
		case MACRO_HEADERS:
		case MACRO_CPP:
			if (pt != NULL)
				generateMacroFiles(pt,mc,cppSuffix,objSuffix,parts,fp);
			else
				invalid = TRUE;

			break;

		case MACRO_CPPMODULE:
			prcode(fp,"%s",pt -> module -> name);
			break;

		case MACRO_PERCENT:
			prcode(fp,"%c",'%');
			break;

		case '\0':
			--cp;
			break;

		default:
			fatal("Invalid macro: %c%c\n",MACRO_ESCAPE,mc);
		}

		if (invalid)
			fatal("%c%c macro invalid in this context\n",MACRO_ESCAPE,mc);
	}
}


/*
 * Generate the macro expansion for the list of files.
 */

static void generateMacroFiles(sipSpec *pt,char ftype,char *cppSuffix,
			       char *objSuffix,int parts,FILE *fp)
{
	char *mname = pt -> module -> name;
	ifaceFileDef *iff;

	prcode(fp,"\\");

	if (parts && ftype != MACRO_HEADERS)
	{
		int i;

		for (i = 0; i < parts; ++i)
		{
			if (i != 0)
				prcode(fp," \\");

			prcode(fp,"\n"
"	%spart%d%s",mname,i,cppSuffix);
		}
	}
	else
	{
		prcode(fp,"\n"
"	");

		if (ftype == MACRO_SOURCES || ftype == MACRO_CPP)
			prcode(fp,"%scmodule%s",mname,cppSuffix);

		if (ftype == MACRO_OBJECTS)
			prcode(fp,"%scmodule%s",mname,objSuffix);

		if (ftype == MACRO_SOURCES || ftype == MACRO_HEADERS)
		{
			if (ftype == MACRO_SOURCES)
				prcode(fp," ");

			prcode(fp,"sip%sDecl%s.h",mname,mname);

			if (pt -> sigargs != NULL)
				prcode(fp," %s%s.h",mname,(parts ? "part0" : "cmodule"));
		}
	}
 
	for (iff = pt -> ifacefiles; iff != NULL; iff = iff -> next)
	{
		if (iff -> type == namespace_iface)
			continue;

		if (iff -> module == pt -> module)
		{
			if (!parts)
				prcode(fp," \\\n"
"	");

			if (!parts && (ftype == MACRO_SOURCES || ftype == MACRO_CPP))
				prcode(fp,"sip%s%F%s",mname,iff -> fqname,cppSuffix);

			if (!parts && ftype == MACRO_OBJECTS)
				prcode(fp,"sip%s%F%s",mname,iff -> fqname,objSuffix);

			if (ftype == MACRO_SOURCES || ftype == MACRO_HEADERS)
			{
				if (ftype == MACRO_SOURCES)
					prcode(fp," ");
				else if (parts)
					prcode(fp," \\\n"
"	");

				prcode(fp,"sip%s%F.h",mname,iff -> fqname);
			}
		}
		else if (isUsed(iff) && (ftype == MACRO_SOURCES || ftype == MACRO_HEADERS))
			prcode(fp," \\\n"
"	sip%s%F.h",iff -> module -> name,iff -> fqname);
	}
}


/*
 * Create a file with an optional standard header.
 */

static FILE *createFile(sipSpec *pt,char *fname,char *commentStr,char *description)
{
	FILE *fp;

	/* Create the file. */

	if ((fp = fopen(fname,"w")) == NULL)
		fatal("Unable to create file \"%s\"\n",fname);

	/* The "stack" doesn't have to be very deep. */
	previousLineNr = currentLineNr;
	currentLineNr = 1;
	previousFileName = currentFileName;
	currentFileName = fname;

	if (commentStr != NULL)
	{
		int needComment;
		codeBlock *cb;
		time_t now;

		/* Write the header. */

		now = time(NULL);

		prcode(fp,"%s %s\n",commentStr,description);
		prcode(fp,"%s\n",commentStr);
		prcode(fp,"%s Generated by SIP %s on %s",commentStr,sipVersion,ctime(&now));

		if (pt -> copying != NULL)
			prcode(fp,"%s\n",commentStr);

		needComment = TRUE;

		for (cb = pt -> copying; cb != NULL; cb = cb -> next)
		{
			char *cp;

			for (cp = cb -> frag; *cp != '\0'; ++cp)
			{
				if (needComment)
				{
					needComment = FALSE;
					prcode(fp,"%s ",commentStr);
				}

				prcode(fp,"%c",*cp);

				if (*cp == '\n')
					needComment = TRUE;
			}
		}
	}

	return fp;
}


/*
 * Close a file and report any errors.
 */

static void closeFile(FILE *fp)
{
	if (ferror(fp))
		fatal("Error writing to \"%s\"\n",currentFileName);

	if (fclose(fp))
		fatal("Error closing \"%s\"\n",currentFileName);

	currentLineNr = previousLineNr;
	currentFileName = previousFileName;
}


/*
 * Print formatted code.
 */

static void prcode(FILE *fp,char *fmt,...)
{
	char ch;
	va_list ap;

	va_start(ap,fmt);

	while ((ch = *fmt++) != '\0')
		if (ch == '%')
		{
			ch = *fmt++;

			switch (ch)
			{
			case 'c':
				{
					char c = va_arg(ap,int);

					if (c == '\n')
						++currentLineNr;

					fputc(c,fp);
					break;
				}

			case 's':
				{
					char *cp = va_arg(ap,char *);

					while (*cp != '\0')
					{
						if (*cp == '\n')
							++currentLineNr;

						fputc(*cp,fp);
						++cp;
					}

					break;
				}

			case 'l':
				fprintf(fp,"%ld",va_arg(ap,long));
				break;

			case 'd':
				fprintf(fp,"%d",va_arg(ap,int));
				break;

			case 'g':
				fprintf(fp,"%g",va_arg(ap,double));
				break;

			case '\0':
				fputc('%',fp);
				--fmt;
				break;

			case '\n':
				fputc('\n',fp);
				++currentLineNr;
				break;

			case 'b':
				{
					argDef *ad, orig;

					ad = va_arg(ap,argDef *);
					orig = *ad;

					resetIsConstArg(ad);
					resetIsReference(ad);
					ad -> nrderefs = 0;

					generateBaseType(ad,fp);

					*ad = orig;

					break;
				}

			case 'B':
				generateBaseType(va_arg(ap,argDef *),fp);
				break;

			case 'T':
				prTypeName(fp,va_arg(ap,argDef *),FALSE);
				break;

			case 'I':
				{
					int indent = va_arg(ap,int);

					while (indent-- > 0)
						fputc('\t',fp);

					break;
				}

			case 'N':
				{
					nameDef *nd = va_arg(ap,nameDef *);

					fprintf(fp,"sipName_%s",nd -> text);
					break;
				}

			case 'E':
				{
					enumDef *ed = va_arg(ap,enumDef *);
					scopedNameDef *snd = ed -> fqname;

					while (snd != NULL)
					{
						if (isProtectedEnum(ed) && (snd == ed -> fqname || snd -> next == NULL))
							fprintf(fp,"sip");

						fprintf(fp,"%s",snd -> name);

						if ((snd = snd -> next) != NULL)
							fprintf(fp,"::");
					}

					break;
				}

			case 'F':
				prScopedName(fp,va_arg(ap,scopedNameDef *),"");
				break;

			case 'C':
				prScopedName(fp,va_arg(ap,scopedNameDef *),"_");
				break;

			case 'S':
				prScopedName(fp,va_arg(ap,scopedNameDef *),"::");
				break;

			case 'X':
				generateThrowSpecifier(va_arg(ap,throwArgs *),fp);
				break;

			default:
				fputc(ch,fp);
			}
		}
		else if (ch == '\n')
		{
			fputc('\n',fp);
			++currentLineNr;
		}
		else
			fputc(ch,fp);

	va_end(ap);
}


/*
 * Generate a scoped name with the given separator string.
 */

static void prScopedName(FILE *fp,scopedNameDef *snd,char *sep)
{
	while (snd != NULL)
	{
		fprintf(fp,"%s",snd -> name);

		if ((snd = snd -> next) != NULL)
			fprintf(fp,"%s",sep);
	}
}


/*
 * Generate a type name to be used as part of an identifier name.
 */

static void prTypeName(FILE *fp,argDef *ad,int intmpl)
{
	if (intmpl)
	{
		char buf[10];
		int flgs;

		/* We use numbers so they don't conflict with names. */

		sprintf(buf,"%02d",ad -> atype);

		flgs = 0;

		if (isConstArg(ad))
			flgs += 1;

		if (isReference(ad))
			flgs += 2;

		prcode(fp,"%s%d%d",buf,flgs,ad -> nrderefs);
	}

	/* Now add a name if there is one associated with the type. */

	switch (ad -> atype)
	{
	case struct_type:
		prcode(fp,"%C",ad -> u.sname);
		break;

	case defined_type:
		prcode(fp,"%C",ad -> u.snd);
		break;

	case enum_type:
		prcode(fp,"%C",ad -> u.ed -> fqname);
		break;

	case mapped_type:
		prTypeName(fp,&ad -> u.mtd -> type,intmpl);
		break;

	case class_type:
		prcode(fp,"%C",classFQName(ad -> u.cd));
		break;

	case template_type:
		{
			int a;
			templateDef *td = ad -> u.td;

			prcode(fp,"%C",td -> fqname);

			for (a = 0; a < td -> types.nrArgs; ++a)
			{
				prcode(fp,"_");
				prTypeName(fp,&td -> types.args[a],TRUE);
			}

			break;
		}
	}
}


/*
 * Generate the fully scoped Python name for a class.
 */

static void prScopedPyClassName(FILE *fp,classDef *scope,classDef *cd)
{
	if (scope != cd -> ecd && cd -> ecd != NULL)
	{
		prScopedPyClassName(fp,scope,cd -> ecd);
		prcode(fp,".");
	}

	prcode(fp,"%s",cd -> iff -> name -> text);
}


/*
 * Return TRUE if handwritten code uses a particular string.
 */

static int needString(codeBlock *cb,const char *s)
{
	return (strstr(cb -> frag,s) != NULL);
}

/*
 * Return TRUE if handwritten code uses the error flag.
 */

static int needErrorFlag(codeBlock *cb)
{
	return needString(cb,"sipIsErr");
}


/*
 * Return TRUE if a method uses the wrapper object.
 */

static int needWrapper(overDef *od)
{
	return (od -> methodcode != NULL && needString(od -> methodcode,"sipSelf"));
}


/*
 * Return TRUE if a method uses the C++ instance.
 */

static int needCpp(overDef *od)
{
	return (od -> methodcode != NULL && needString(od -> methodcode,"sipCpp"));
}


/*
 * Get the C++ signature for a method.
 */

static void getCppSignature(overDef *od,argDef **res,funcArgs **args)
{
	if (od -> cppsig != NULL)
	{
		argDef *ad = &od -> cppsig -> result;

		*res = (ad -> atype == voidptr_type && ad -> nrderefs == 0) ? NULL : ad;
		*args = &od -> cppsig -> args;
	}
	else
	{
		*res = od -> result;
		*args = &od -> args;
	}
}


/*
 * Return TRUE if the argument type means an instance needs to be created on
 * the heap to pass back to Python.
 */
static int needNewInstance(argDef *ad)
{
	return ((ad -> atype == mapped_type || ad -> atype == class_type) &&
		((isReference(ad) && ad -> nrderefs == 0) || (!isReference(ad) && ad -> nrderefs == 1)) &&
		!isInArg(ad) && isOutArg(ad));
}
