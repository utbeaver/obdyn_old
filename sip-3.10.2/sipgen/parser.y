/*
 * The SIP parser.
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

%{
#include <stdlib.h>
#include <string.h>

#include "sip.h"


#define	MAX_NESTED_IF		10
#define	MAX_NESTED_SCOPE	10

#define	isMainModule(m)		(currentSpec -> module == (m))


/* The different attribute name types. */

typedef enum {
	func_attr,
	var_attr,
	enum_attr,
	class_attr
} attrType;


static sipSpec *currentSpec;		/* The current spec being parsed. */
static stringList *neededQualifiers;	/* The list of required qualifiers. */
static stringList *excludedQualifiers;	/* The list of excluded qualifiers. */
static moduleDef *currentModule;	/* The current module being parsed. */
static mappedTypeDef *currentMappedType;	/* The current mapped type. */
static enumDef *currentEnum;		/* The current enum being parsed. */
static int sectionFlags;		/* The current section flags. */
static int currentOverIsVirt;		/* Set if the overload is virtual. */
static int currentIsStatic;		/* Set if the current is static. */
static char *previousFile;		/* The file just parsed. */
static parserContext newContext;	/* The new pending context. */
static int skipStackPtr;		/* The skip stack pointer. */
static int skipStack[MAX_NESTED_IF];	/* Stack of skip flags. */
static classDef *scopeStack[MAX_NESTED_SCOPE];	/* The scope stack. */
static int sectFlagsStack[MAX_NESTED_SCOPE];	/* The section flags stack. */
static int currentScopeIdx;		/* The scope stack index. */


static nameDef *cacheName(sipSpec *,moduleDef *,char *);
static ifaceFileDef *findIfaceFile(sipSpec *,scopedNameDef *,ifaceFileType,
				   argDef *);
static classDef *findClass(sipSpec *,ifaceFileType,scopedNameDef *);
static classDef *newClass(sipSpec *,ifaceFileType,scopedNameDef *);
static void finishClass(classDef *,optFlags *);
static mappedTypeDef *newMappedType(sipSpec *,argDef *);
static enumDef *newEnum(sipSpec *,moduleDef *,char *,int);
static void newTypedef(sipSpec *,char *,argDef *);
static void newVar(sipSpec *,moduleDef *,char *,int,argDef *,codeBlock *);
static void newCtor(char *,int,funcArgs *,optFlags *,codeBlock *,codeBlock *,
		    throwArgs *,signatureDef *);
static void newFunction(sipSpec *,moduleDef *,int,int,int,argDef *,char *,
			funcArgs *,int,int,optFlags *,codeBlock *,codeBlock *,
			codeBlock *,codeBlock *,throwArgs *,signatureDef *);
static optFlag *findOptFlag(optFlags *,char *,flagType);
static memberDef *findFunction(sipSpec *,moduleDef *,classDef *,char *,
			       slotType);
static void checkAttributes(sipSpec *,classDef *,nameDef *,attrType);
static void newModule(FILE *,char *);
static void appendCodeBlock(codeBlock **,codeBlock *);
static void parseFile(FILE *,char *,moduleDef *,int);
static void handleEOF(void);
static void handleEOM(void);
static qualDef *findQualifier(sipSpec *,char *);
static scopedNameDef *text2scopedName(char *);
static void pushScope(classDef *);
static void popScope(void);
static classDef *currentScope(void);
static void newQualifier(sipSpec *,moduleDef *,int,char *,qualType);
static void newImport(sipSpec *,char *);
static void usedInMainModule(sipSpec *,ifaceFileDef *);
static int timePeriod(sipSpec *,char *,char *);
static int platOrFeature(sipSpec *,char *,int);
static int isNeeded(qualDef *);
static int notSkipping(void);
static void appendToClassList(classList **,classDef *);
static void getHooks(optFlags *,char **,char **);
%}

%union {
	char		qchar;
	char		*text;
	long		number;
	double		real;
	argDef		memArg;
	funcArgs	arglist;
	signatureDef	*optsignature;
	throwArgs	*throwlist;
	codeBlock	*codeb;
	valueDef	value;
	valueDef	*valp;
	optFlags	optflags;
	optFlag		flag;
	scopedNameDef	*scpvalp;
	fcallDef	fcall;
	int		boolean;
}

%token			TK_DOC
%token			TK_EXPORTEDDOC
%token			TK_MAKEFILE
%token			TK_ACCESSCODE
%token			TK_VARCODE
%token			TK_POSTINITCODE
%token			TK_CPPCODE
%token			TK_MODCODE
%token			TK_TYPECODE
%token			TK_PREPYCODE
%token			TK_PYCODE
%token 			TK_COPYING
%token			TK_MAPPEDTYPE
%token <codeb>		TK_CODEBLOCK
%token			TK_IF
%token			TK_END
%token <text>		TK_NAME
%token <text>		TK_PATHNAME
%token <text>		TK_STRING
%token			TK_VIRTUALCATCHERCODE
%token			TK_VIRTUALCODE
%token			TK_METHODCODE
%token			TK_MEMBERCODE
%token			TK_FROMTYPE
%token			TK_TOTYPE
%token			TK_TOSUBCLASS
%token			TK_INCLUDE
%token			TK_OPTINCLUDE
%token			TK_IMPORT
%token			TK_HEADERCODE
%token			TK_MODHEADERCODE
%token			TK_TYPEHEADERCODE
%token			TK_EXPHEADERCODE
%token			TK_MODULE
%token			TK_CLASS
%token			TK_STRUCT
%token			TK_PUBLIC
%token			TK_PROTECTED
%token			TK_PRIVATE
%token			TK_SIGNALS
%token			TK_SLOTS
%token			TK_BOOL
%token			TK_SHORT
%token			TK_INT
%token			TK_LONG
%token			TK_FLOAT
%token			TK_DOUBLE
%token			TK_CHAR
%token			TK_VOID
%token			TK_PYOBJECT
%token			TK_PYTUPLE
%token			TK_PYLIST
%token			TK_PYDICT
%token			TK_PYCALLABLE
%token			TK_PYSLICE
%token			TK_VIRTUAL
%token			TK_ENUM
%token			TK_UNSIGNED
%token			TK_SCOPE
%token			TK_LOGICAL_OR
%token			TK_CONST
%token			TK_STATIC
%token			TK_SIPSIGNAL
%token			TK_SIPSLOT
%token			TK_SIPRXCON
%token			TK_SIPRXDIS
%token			TK_SIPSLOTCON
%token			TK_SIPSLOTDIS
%token <number>		TK_NUMBER
%token <real>		TK_REAL
%token			TK_TYPEDEF
%token			TK_NAMESPACE
%token			TK_TIMELINE
%token			TK_PLATFORMS
%token			TK_FEATURE
%token			TK_LICENSE
%token			TK_EXPOSE
%token <qchar>		TK_QCHAR
%token			TK_TRUE
%token			TK_FALSE
%token			TK_OPERATOR
%token			TK_THROW
%token			TK_QOBJECT

%type <memArg>		argvalue
%type <memArg>		argtype
%type <memArg>		cpptype
%type <memArg>		basetype
%type <arglist>		arglist
%type <arglist>		rawarglist
%type <arglist>		cpptypelist
%type <optsignature>	optsig
%type <optsignature>	optctorsig
%type <throwlist>	optexceptions
%type <throwlist>	exceptionlist
%type <number>		optslot
%type <number>		optref
%type <number>		optconst
%type <number>		optvirtual
%type <number>		optabstract
%type <number>		deref
%type <number>		optnumber
%type <value>		simplevalue
%type <valp>		value
%type <valp>		expr
%type <valp>		optassign
%type <codeb>		optaccesscode
%type <codeb>		modhdrcode
%type <codeb>		typehdrcode
%type <codeb>		modcode
%type <codeb>		typecode
%type <codeb>		codeblock
%type <codeb>		virtualcode
%type <codeb>		virtualcatchercode
%type <codeb>		methodcode
%type <codeb>		membercode
%type <text>		operatorname
%type <text>		optfilename
%type <text>		optname
%type <optflags>	optflags
%type <optflags>	flaglist
%type <flag>		flag
%type <flag>		flagvalue
%type <qchar>		optunop
%type <qchar>		binop
%type <scpvalp>		scopepart
%type <scpvalp>		scopedname
%type <fcall>		valuelist
%type <boolean>		qualifiers
%type <boolean>		oredqualifiers

%%

specification: 	statement
	|	specification statement
	;

statement:	{
			/*
			 * We don't do these in parserEOF() because the parser
			 * is reading ahead and that would be too early.
			 */

			if (previousFile != NULL)
			{
				handleEOF();

				if (newContext.prevmod != NULL)
					handleEOM();

				free(previousFile);
				previousFile = NULL;
			}
	} modstatement
	;

modstatement:	module
	|	copying
	|	include
	|	optinclude
	|	import
	|	timeline
	|	platforms
	|	feature
	|	license
	|	expose
	|	modhdrcode {
			if (notSkipping() && isMainModule(currentModule))
				appendCodeBlock(&currentSpec -> hdrcode,$1);
		}
	|	exphdrcode
	|	modcode {
			if (notSkipping() && isMainModule(currentModule))
				appendCodeBlock(&currentSpec -> cppcode,$1);
		}
	|	postinitcode
	|	prepycode
	|	pycode
	|	doc
	|	exporteddoc
	|	makefile
	|	mappedtype
	|	nsstatement
	;

nsstatement:	ifstart
	|	ifend
	|	namespace
	|	struct
	|	class
	|	opaqueclass
	|	typedef
	|	enum
	|	function
	|	variable
	;

mappedtype:	TK_MAPPEDTYPE basetype {
			if (notSkipping())
			{
				$2.nrderefs = 0;
				$2.argflags = 0;

				currentMappedType = newMappedType(currentSpec,&$2);
				usedInMainModule(currentSpec,currentMappedType -> iff);
			}
		} '{' mtbody '}' ';' {
			if (notSkipping())
			{
				if (currentMappedType -> convfromcode == NULL)
					yyerror("%MappedType must have a %ConvertFromTypeCode directive");

				if (currentMappedType -> convtocode == NULL)
					yyerror("%MappedType must have a %ConvertToTypeCode directive");

				currentMappedType = NULL;
			}
		}
	;

mtbody:		mtline
	|	mtbody mtline
	;

mtline: 	typehdrcode {
			if (notSkipping())
				appendCodeBlock(&currentMappedType -> hdrcode,$1);
		}
	|	TK_FROMTYPE codeblock {
			if (notSkipping())
			{
				if (currentMappedType -> convfromcode != NULL)
					yyerror("%MappedType has more than one %ConvertFromTypeCode directive");

				currentMappedType -> convfromcode = $2;
			}
		}
	|	TK_TOTYPE codeblock {
			if (notSkipping())
			{
				if (currentMappedType -> convtocode != NULL)
					yyerror("%MappedType has more than one %ConvertToTypeCode directive");

				currentMappedType -> convtocode = $2;
			}
		}
	;

namespace:	TK_NAMESPACE TK_NAME {
			if (notSkipping())
			{
				classDef *ns;

				ns = newClass(currentSpec,namespace_iface,text2scopedName($2));
			
				setCannotCreate(ns);

				pushScope(ns);

				sectionFlags = 0;
			}
		} '{' nsbody '}' ';' {
			/* Remove the namespace name. */

			if (notSkipping())
				popScope();
		}
	;

nsbody:		nsstatement
	|	nsbody nsstatement
	;

platforms:	TK_PLATFORMS {
			qualDef *qd;

			for (qd = currentModule -> qualifiers; qd != NULL; qd = qd -> next)
				if (qd -> qtype == platform_qualifier)
					yyerror("%Platforms has already been defined for this module");
		}
		'{' platformlist '}' {
			qualDef *qd;
			int nrneeded;

			/*
			 * Check that exactly one platform in the set was
			 * requested.
			 */

			nrneeded = 0;

			for (qd = currentModule -> qualifiers; qd != NULL; qd = qd -> next)
				if (qd -> qtype == platform_qualifier && isNeeded(qd))
					++nrneeded;

			if (nrneeded != 1)
				yyerror("Exactly one of these %Platforms must be specified with the -t flag");
		}
	;

platformlist:	platform
	|	platformlist platform
	;

platform:	TK_NAME {
			newQualifier(currentSpec,currentModule,-1,$1,platform_qualifier);
		}
	;

feature:	TK_FEATURE TK_NAME {
			newQualifier(currentSpec,currentModule,-1,$2,feature_qualifier);
		}
	;

timeline:	TK_TIMELINE {
			if (currentModule -> nrTimeSlots >= 0)
				yyerror("%Timeline has already been defined for this module");

			currentModule -> nrTimeSlots = 0;
		}
		'{' qualifierlist '}' {
			qualDef *qd;
			int nrneeded;

			/*
			 * Check that exactly one time slot in the set was
			 * requested.
			 */

			nrneeded = 0;

			for (qd = currentModule -> qualifiers; qd != NULL; qd = qd -> next)
				if (qd -> qtype == time_qualifier && isNeeded(qd))
					++nrneeded;

			if (nrneeded != 1)
				yyerror("Exactly one of this %Timeline must be specified with the -t flag");
		}
	;

qualifierlist:	qualifiername
	|	qualifierlist qualifiername
	;

qualifiername:	TK_NAME {
			newQualifier(currentSpec,currentModule,currentModule -> nrTimeSlots++,$1,time_qualifier);
		}
	;

ifstart:	TK_IF '(' qualifiers ')' {
			int skipNext;

			if (skipStackPtr >= MAX_NESTED_IF)
				yyerror("Internal error: increase the value of MAX_NESTED_IF");

			/* Nested %Ifs are implicit logical ands. */

			if (skipStackPtr > 0)
				$3 = ($3 && skipStack[skipStackPtr - 1]);

			skipStack[skipStackPtr++] = $3;
		}
	;

oredqualifiers:	TK_NAME {
			$$ = platOrFeature(currentSpec,$1,FALSE);
		}
	|	'!' TK_NAME {
			$$ = platOrFeature(currentSpec,$2,TRUE);
		}
	|	oredqualifiers TK_LOGICAL_OR TK_NAME {
			$$ = (platOrFeature(currentSpec,$3,FALSE) || $1);
		}
	|	oredqualifiers TK_LOGICAL_OR '!' TK_NAME {
			$$ = (platOrFeature(currentSpec,$4,TRUE) || $1);
		}
	;

qualifiers:	oredqualifiers
	|	optname '-' optname {
			$$ = timePeriod(currentSpec,$1,$3);
		}
	;

ifend:		TK_END {
			if (skipStackPtr-- <= 0)
				yyerror("Too many %End directives");
		}
	;

license:	TK_LICENSE optflags {
			optFlag *of;

			if ($2.nrFlags == 0)
				yyerror("%License details not specified");

			if ((of = findOptFlag(&$2,"Type",string_flag)) == NULL)
				yyerror("%License type not specified");

			currentModule -> license = sipMalloc(sizeof (licenseDef));

			currentModule -> license -> type = of -> fvalue.sval;

			currentModule -> license -> licensee = 
				((of = findOptFlag(&$2,"Licensee",string_flag)) != NULL)
					? of -> fvalue.sval : NULL;

			currentModule -> license -> timestamp = 
				((of = findOptFlag(&$2,"Timestamp",string_flag)) != NULL)
					? of -> fvalue.sval : NULL;

			currentModule -> license -> sig = 
				((of = findOptFlag(&$2,"Signature",string_flag)) != NULL)
					? of -> fvalue.sval : NULL;
		}
	;

expose:		TK_EXPOSE TK_NAME {
			/* Only bother if this is the main module. */

			if (isMainModule(currentModule))
			{
				expFuncDef *ef;

				/* Check it isn't already there. */

				for (ef = currentSpec -> exposed; ef != NULL; ef = ef -> next)
					if (strcmp(ef -> name,$2) == 0)
						yyerror("Function has already been exposed in this module");

				ef = sipMalloc(sizeof (expFuncDef));

				ef -> name = $2;
				ef -> next = currentSpec -> exposed;

				currentSpec -> exposed = ef;
			}
		}
	;

module:		TK_MODULE TK_NAME optnumber {
			/* Check the module hasn't already been defined. */

			moduleDef *mod;

			for (mod = currentSpec -> moduleList; mod != NULL; mod = mod -> next)
				if (mod -> name != NULL && strcmp(mod -> name,$2) == 0)
					yyerror("Module is already defined");

			currentModule -> name = $2;
		}
	;

optnumber:	{
			$$ = -1;
		}
	|	TK_NUMBER
	;

include:	TK_INCLUDE TK_PATHNAME {
			parseFile(NULL,$2,NULL,FALSE);
		}
	;

optinclude:	TK_OPTINCLUDE TK_PATHNAME {
			parseFile(NULL,$2,NULL,TRUE);
		}
	;

import:		TK_IMPORT TK_PATHNAME {
			newImport(currentSpec,$2);
		}
	;

optaccesscode:	{
			$$ = NULL;
		}
	|	TK_ACCESSCODE codeblock {
			$$ = $2;
		}
	|	TK_VARCODE codeblock {
			yywarning("%VariableCode is renamed to %AccessCode in SIP v4 and later");
			$$ = $2;
		}
	;

copying:	TK_COPYING codeblock {
			if (isMainModule(currentModule))
				appendCodeBlock(&currentSpec -> copying,$2);
		}
	;

modhdrcode:	TK_MODHEADERCODE codeblock {
			$$ = $2;
		}
	|	TK_HEADERCODE codeblock {
			yywarning("%HeaderCode is renamed to %ModuleHeaderCode in this context in SIP v4 and later");
			$$ = $2;
		}
	;

typehdrcode:	TK_TYPEHEADERCODE codeblock {
			$$ = $2;
		}
	|	TK_HEADERCODE codeblock {
			yywarning("%HeaderCode is renamed to %TypeHeaderCode in this context in SIP v4 and later");
			$$ = $2;
		}
	;

exphdrcode:	TK_EXPHEADERCODE codeblock {
			yywarning("%ExportedHeaderCode is deprecated");

			appendCodeBlock(&currentSpec -> exphdrcode,$2);
		}
	;

modcode:	TK_MODCODE codeblock {
			$$ = $2;
		}
	|	TK_CPPCODE codeblock {
			yywarning("%C++Code is renamed to %ModuleCode in this context in SIP v4 and later");
			$$ = $2;
		}
	;

typecode:	TK_TYPECODE codeblock {
			$$ = $2;
		}
	|	TK_CPPCODE codeblock {
			yywarning("%C++Code is renamed to %TypeCode in this context in SIP v4 and later");
			$$ = $2;
		}
	;

postinitcode:	TK_POSTINITCODE codeblock {
			if (isMainModule(currentModule))
				appendCodeBlock(&currentSpec -> postinitcode,$2);
		}
	;

prepycode:	TK_PREPYCODE codeblock {
			yywarning("%PrePythonCode is deprecated");

			if (isMainModule(currentModule))
				appendCodeBlock(&currentSpec -> prepycode,$2);
		}
	;

pycode:		TK_PYCODE codeblock {
			yywarning("%PythonCode is deprecated");

			if (isMainModule(currentModule))
				appendCodeBlock(&currentSpec -> pycode,$2);
		}
	;

doc:		TK_DOC codeblock {
			if (isMainModule(currentModule))
				appendCodeBlock(&currentSpec -> docs,$2);
		}
	;

exporteddoc:	TK_EXPORTEDDOC codeblock {
			appendCodeBlock(&currentSpec -> docs,$2);
		}
	;

makefile:	TK_MAKEFILE TK_PATHNAME optfilename codeblock {
			if (isMainModule(currentModule))
			{
				mkTemplateDef *mtd;

				yywarning("%Makefile is deprecated, please use the -b flag instead");

				mtd = sipMalloc(sizeof (mkTemplateDef));

				mtd -> name = $2;

				if ((mtd -> objext = $3) == NULL)
					mtd -> objext = DEFAULT_OFILE_EXT;

				mtd -> templ = NULL;

				appendCodeBlock(&mtd -> templ,$4);

				mtd -> next = currentSpec -> mktemplates;
				currentSpec -> mktemplates = mtd;
			}
		}
	;

codeblock:	TK_CODEBLOCK TK_END
	;

enum:		TK_ENUM optname {
			if (notSkipping())
			{
				if (sectionFlags != 0 && (sectionFlags & ~(SECT_IS_PUBLIC | SECT_IS_PROT)) != 0)
					yyerror("Class enums must be in the public or protected sections");

				currentEnum = newEnum(currentSpec,currentModule,$2,sectionFlags);
			}
		} '{' optenumbody '}' ';'
	;

optfilename:	{
			$$ = NULL;
		}
	|	TK_PATHNAME {
			$$ = $1;
		}
	;

optname:	{
			$$ = NULL;
		}
	|	TK_NAME {
			$$ = $1;
		}
	;

optenumbody:
	|	enumbody
	;

enumbody:	enumline
	|	enumbody enumline
	;

enumline:	ifstart
	|	ifend
	|	TK_NAME optassign optcomma {
			if (notSkipping())
			{
				/*
				 * Note that we don't use the assigned value.
				 * This is a hangover from when enums where
				 * generated in Python.  We can remove it when
				 * we have got around to updating all the .sip
				 * files.
				 */

				enumValueDef *evd;

				evd = sipMalloc(sizeof (enumValueDef));

				evd -> name = cacheName(currentSpec,currentModule,$1);
				evd -> ed = currentEnum;

				checkAttributes(currentSpec,evd -> ed -> ecd,evd -> name,enum_attr);

				evd -> next = currentEnum -> values;
				currentEnum -> values = evd;

			}
		}
	;

optcomma:
	|	','
	;

optassign:	{
			$$ = NULL;
		}
	|	'=' expr {
			$$ = $2;
		}
	;

expr:		value
	|	expr binop value {
			valueDef *vd;
 
			if ($1 -> vtype == string_value || $3 -> vtype == string_value)
				yyerror("Invalid binary operator for string");
 
			/* Find the last value in the existing expression. */
 
			for (vd = $1; vd -> next != NULL; vd = vd -> next)
				;
 
			vd -> vbinop = $2;
			vd -> next = $3;

			$$ = $1;
		}
	;

binop:		'-' {
			$$ = '-';
		}
	|	'+' {
			$$ = '+';
		}
	|	'|' {
			$$ = '|';
		}
	;

optunop:	{
			$$ = '\0';
		}
	|	'!' {
			$$ = '!';
		}
	|	'~' {
			$$ = '~';
		}
	|	'-' {
			$$ = '-';
		}
	|	'+' {
			$$ = '+';
		}
	;

value:		optunop simplevalue {
			if ($1 != '\0' && $2.vtype == string_value)
				yyerror("Invalid unary operator for string");
 
			/*
			 * Convert the value to a simple expression on the
			 * heap.
			 */
 
			$$ = sipMalloc(sizeof (valueDef));
 
			*$$ = $2;
			$$ -> vunop = $1;
			$$ -> vbinop = '\0';
			$$ -> next = NULL;
		}
	;

scopedname:	scopepart
	|	scopedname TK_SCOPE scopepart {
			appendScopedName(&$1,$3);
		}
	;

scopepart: 	TK_NAME {
			$$ = text2scopePart($1);
		}
	;

simplevalue:	scopedname {
			/*
			 * We let the C++ compiler decide if the value is a
			 * valid one - no point in building a full C++ parser
			 * here.
			 */

			$$.vtype = scoped_value;
			$$.u.vscp = $1;
		}
	|	scopedname '(' valuelist ')' {
			fcallDef *fcd;

			fcd = sipMalloc(sizeof (fcallDef));
			*fcd = $3;
			fcd -> name = $1;

			$$.vtype = fcall_value;
			$$.u.fcd = fcd;
		}
	|	TK_REAL {
			$$.vtype = real_value;
			$$.u.vreal = $1;
		}
	|	TK_NUMBER {
			$$.vtype = numeric_value;
			$$.u.vnum = $1;
		}
	|	TK_TRUE {
			$$.vtype = numeric_value;
			$$.u.vnum = 1;
		}
	|	TK_FALSE {
			$$.vtype = numeric_value;
			$$.u.vnum = 0;
		}
	|	TK_STRING {
			$$.vtype = string_value;
			$$.u.vstr = $1;
		}
	|	TK_QCHAR {
			$$.vtype = qchar_value;
			$$.u.vqchar = $1;
		}
	;

valuelist:	{
			/* No values. */

			$$.nrArgs = 0;
		}
	|	value {
			/* The single or first value. */

			$$.args[0] = $1;
			$$.nrArgs = 1;
		}
	|	valuelist ',' value {
			/* Check that it wasn't ...(,value...). */

			if ($$.nrArgs == 0)
				yyerror("First argument to function call is missing");

			/* Check there is room. */

			if ($1.nrArgs == MAX_NR_ARGS)
				yyerror("Too many arguments to function call");

			$$ = $1;

			$$.args[$$.nrArgs] = $3;
			$$.nrArgs++;
		}
	;

typedef:	TK_TYPEDEF cpptype TK_NAME ';' {
			if (notSkipping())
				newTypedef(currentSpec,$3,&$2);
		}
	;

opaqueclass:	TK_CLASS scopedname ';' {
			if (notSkipping())
			{
				classDef *cd;

				cd = newClass(currentSpec,class_iface,$2);

				usedInMainModule(currentSpec,cd -> iff);
				setCannotCreate(cd);
				setIsOpaque(cd);
			}
		}
	;

struct:		TK_STRUCT TK_NAME {
			if (notSkipping())
			{
				classDef *cd;

				cd = newClass(currentSpec,class_iface,text2scopedName($2));

				usedInMainModule(currentSpec,cd -> iff);
				setCannotCreate(cd);

				pushScope(cd);

				sectionFlags = SECT_IS_PUBLIC;
			}
		} optflags '{' classbody '}' ';' {
			if (notSkipping())
			{
				finishClass(currentScope(),&$4);
				popScope();
			}
		}
	;

class:		TK_CLASS TK_NAME {
			if (notSkipping())
			{
				classDef *cd;

				cd = newClass(currentSpec,class_iface,text2scopedName($2));

				usedInMainModule(currentSpec,cd -> iff);
				setCannotCreate(cd);

				pushScope(cd);

				sectionFlags = SECT_IS_PRIVATE;
			}
		} superclasses optflags '{' classbody '}' ';' {
			if (notSkipping())
			{
				finishClass(currentScope(),&$5);
				popScope();
			}
		}
	;

superclasses:
	|	':' superlist
	;

superlist:	superclass
	|	superlist ',' superclass
	;

superclass:	scopedname {
			if (notSkipping())
			{
				classDef *cd, *super;

				cd = currentScope();

				super = findClass(currentSpec,class_iface,$1);

				appendToClassList(&cd -> supers,super);
				addToUsedList(&cd -> iff -> used,super -> iff);
			}
		}
	;

classbody:	classline
	|	classbody classline
	;

classline:	ifstart
	|	ifend
	|	namespace
	|	struct
	|	class
	|	opaqueclass
	|	typedef
	|	enum
	|	typecode {
			if (notSkipping())
				appendCodeBlock(&currentScope() -> cppcode,$1);
		}
	|	typehdrcode {
			if (notSkipping())
				appendCodeBlock(&currentScope() -> hdrcode,$1);
		}
	|	ctor
	|	dtor
	|	varmember
	|	TK_TOSUBCLASS codeblock {
			if (notSkipping())
			{
				classDef *cd = currentScope();

				if (cd -> convtosubcode != NULL)
					yyerror("Class has more than one %ConvertToSubClassCode directive");

				cd -> convtosubcode = $2;
			}
		}
	|	TK_TOTYPE codeblock {
			if (notSkipping())
			{
				classDef *cd = currentScope();

				if (cd -> convtocode != NULL)
					yyerror("Class has more than one %ConvertToTypeCode directive");

				cd -> convtocode = $2;
			}
		}
	|	TK_PUBLIC optslot ':' {
			if (notSkipping())
				sectionFlags = SECT_IS_PUBLIC | $2;
		}
	|	TK_PROTECTED optslot ':' {
			if (notSkipping())
				sectionFlags = SECT_IS_PROT | $2;
		}
	|	TK_PRIVATE optslot ':' {
			if (notSkipping())
				sectionFlags = SECT_IS_PRIVATE | $2;
		}
	|	TK_SIGNALS ':' {
			if (notSkipping())
				sectionFlags = SECT_IS_SIGNAL;
		}
	;

optslot:	{
			$$ = 0;
		}
	|	TK_SLOTS {
			$$ = SECT_IS_SLOT;
		}
	;

dtor:		optvirtual '~' TK_NAME '(' ')' optexceptions ';' methodcode membercode virtualcatchercode virtualcode {
			if (notSkipping())
			{
				classDef *cd = currentScope();

				if (strcmp(classBaseName(cd),$3) != 0)
					yyerror("Destructor doesn't have the same name as its class");

				if (isDtor(cd))
					yyerror("Destructor has already been defined");

				if ($8 != NULL && $9 != NULL)
					yyerror("Cannot provide %MethodCode and %MemberCode");

				if ($8 == NULL)
					$8 = $9;

				if ($10 != NULL && $11 != NULL)
					yyerror("Cannot provide %VirtualCatcherCode and %VirtualCode");

				if ($10 == NULL)
					$10 = $11;

				cd -> dealloccode = $8;
				cd -> dtorcode = $10;
				cd -> dtorexceptions = $6;
				cd -> classflags |= sectionFlags;

				/*
				 * Make the class complex if we have a virtual
				 * dtor or some dtor code.
				 */
				if ($1 || $10 != NULL)
					setIsComplex(cd);
			}
		}
	;

optvirtual:	{
			$$ = FALSE;
		}
	|	TK_VIRTUAL {
			$$ = TRUE;
		}
	;

ctor:		TK_NAME '(' arglist ')' optexceptions optflags optctorsig ';' methodcode membercode {
			if (notSkipping())
			{
				if ((sectionFlags & (SECT_IS_PUBLIC | SECT_IS_PROT | SECT_IS_PRIVATE)) == 0)
					yyerror("Constructor must be in the public, private or protected sections");

				newCtor($1,sectionFlags,&$3,&$6,$9,$10,$5,$7);
			}

			free($1);
		}
	;

optctorsig:	{
			$$ = NULL;
		}
	|	'[' '(' arglist ')' ']' {
			$$ = sipMalloc(sizeof (signatureDef));

			$$ -> args = $3;
		}
	;

optsig:	{
			$$ = NULL;
		}
	|	'[' cpptype '(' arglist ')' ']' {
			$$ = sipMalloc(sizeof (signatureDef));

			$$ -> result = $2;
			$$ -> args = $4;
		}
	;

function:	cpptype TK_NAME '(' arglist ')' optconst optexceptions optabstract optflags optsig ';' methodcode membercode virtualcatchercode virtualcode {
			if (notSkipping())
			{
				if (sectionFlags != 0 && (sectionFlags & (SECT_IS_PUBLIC | SECT_IS_PROT | SECT_IS_PRIVATE | SECT_IS_SLOT | SECT_IS_SIGNAL)) == 0)
					yyerror("Class function must be in the public, private, protected, slot or signal sections");

				newFunction(currentSpec,currentModule,
					    sectionFlags,currentIsStatic,
					    currentOverIsVirt,
					    &$1,$2,&$4,$6,$8,&$9,$12,$13,$14,$15,$7,$10);
			}

			currentIsStatic = FALSE;
			currentOverIsVirt = FALSE;
		}
	|	cpptype TK_OPERATOR operatorname '(' arglist ')' optconst optexceptions optflags optsig ';' methodcode membercode {
			if (notSkipping())
				newFunction(currentSpec,currentModule,
					    sectionFlags,currentIsStatic,
					    currentOverIsVirt,
					    &$1,$3,&$5,$7,FALSE,&$9,$12,$13,NULL,NULL,$8,$10);

			currentIsStatic = FALSE;
			currentOverIsVirt = FALSE;
		}
	;

operatorname:	'+'		{$$ = "__add__";}
	|	'-'		{$$ = "__sub__";}
	|	'*'		{$$ = "__mul__";}
	|	'/'		{$$ = "__div__";}
	|	'%'		{$$ = "__mod__";}
	|	'&'		{$$ = "__and__";}
	|	'|'		{$$ = "__or__";}
	|	'^'		{$$ = "__xor__";}
	|	'<' '<'		{$$ = "__lshift__";}
	|	'>' '>'		{$$ = "__rshift__";}
	|	'+' '='		{$$ = "__iadd__";}
	|	'-' '='		{$$ = "__isub__";}
	|	'*' '='		{$$ = "__imul__";}
	|	'/' '='		{$$ = "__idiv__";}
	|	'%' '='		{$$ = "__imod__";}
	|	'&' '='		{$$ = "__iand__";}
	|	'|' '='		{$$ = "__ior__";}
	|	'^' '='		{$$ = "__ixor__";}
	|	'<' '<' '='	{$$ = "__ilshift__";}
	|	'>' '>' '='	{$$ = "__irshift__";}
	|	'~'		{$$ = "__invert__";}
	|	'(' ')'		{$$ = "__call__";}
	|	'[' ']'		{$$ = "__getitem__";}
	|	'<'		{$$ = "__lt__";}
	|	'<' '='		{$$ = "__le__";}
	|	'=' '='		{$$ = "__eq__";}
	|	'!' '='		{$$ = "__ne__";}
	|	'>'		{$$ = "__gt__";}
	|	'>' '='		{$$ = "__ge__";}
	;

optconst:	{
			$$ = FALSE;
		}
	|	TK_CONST {
			$$ = TRUE;
		}
	;

optabstract:	{
			$$ = 0;
		}
	|	'=' TK_NUMBER {
			if ($2 != 0)
				yyerror("Abstract virtual function '= 0' expected");

			$$ = TRUE;
		}
	;

optflags:	{
			$$.nrFlags = 0;
		}
	|	'/' flaglist '/' {
			$$ = $2;
		}
	;


flaglist:	flag {
			$$.flags[0] = $1;
			$$.nrFlags = 1;
		}
	|	flaglist ',' flag {
			/* Check there is room. */

			if ($1.nrFlags == MAX_NR_FLAGS)
				yyerror("Too many optional flags");

			$$ = $1;

			$$.flags[$$.nrFlags++] = $3;
		}
	;

flag:		TK_NAME {
			$$.ftype = bool_flag;
			$$.fname = $1;
		}
	|	TK_NAME '=' flagvalue {
			$$ = $3;
			$$.fname = $1;
		}
	;

flagvalue:	TK_NAME {
			$$.ftype = name_flag;
			$$.fvalue.sval = $1;
		}
	|	TK_STRING {
			$$.ftype = string_flag;
			$$.fvalue.sval = $1;
		}
	|	TK_NUMBER {
			$$.ftype = integer_flag;
			$$.fvalue.ival = $1;
		}
	;

methodcode:	{
			$$ = NULL;
		}
	|	TK_METHODCODE codeblock {
			$$ = $2;
		}
	;

membercode:	{
			$$ = NULL;
		}
	|	TK_MEMBERCODE codeblock {
			yywarning("%MemberCode is deprecated, please use %MethodCode instead");
			$$ = $2;
		}
	;

virtualcatchercode:	{
			$$ = NULL;
		}
	|	TK_VIRTUALCATCHERCODE codeblock {
			$$ = $2;
		}
	;

virtualcode:	{
			$$ = NULL;
		}
	|	TK_VIRTUALCODE codeblock {
			yywarning("%VirtualCode is deprecated, please use %VirtualCatcherCode instead");
			$$ = $2;
		}
	;

arglist:	rawarglist {
			int a, nrrxcon, nrrxdis, nrslotcon, nrslotdis, nrarray, nrarraysize;

			nrrxcon = nrrxdis = nrslotcon = nrslotdis = nrarray = nrarraysize = 0;

			for (a = 0; a < $1.nrArgs; ++a)
			{
				argDef *ad = &$1.args[a];

				switch (ad -> atype)
				{
				case rxcon_type:
					++nrrxcon;
					break;

				case rxdis_type:
					++nrrxdis;
					break;

				case slotcon_type:
					++nrslotcon;
					break;

				case slotdis_type:
					++nrslotdis;
					break;
				}

				if (isArray(ad))
					++nrarray;

				if (isArraySize(ad))
					++nrarraysize;
			}

			if (nrrxcon != nrslotcon || nrrxcon > 1)
				yyerror("SIP_RXOBJ_CON and SIP_SLOT_CON must both be given and at most once");

			if (nrrxdis != nrslotdis || nrrxdis > 1)
				yyerror("SIP_RXOBJ_DIS and SIP_SLOT_DIS must both be given and at most once");

			if (nrarray != nrarraysize || nrarray > 1)
				yyerror("/Array/ and /ArraySize/ must both be given and at most once");

			$$ = $1;
		}
	;

rawarglist:	{
			/* No arguments. */

			$$.nrArgs = 0;
		}
	|	argvalue {
			/* The single or first argument. */

			$$.args[0] = $1;
			$$.nrArgs = 1;
		}
	|	rawarglist ',' argvalue {
			/* Check that it wasn't ...(,arg...). */

			if ($1.nrArgs == 0)
				yyerror("First argument of list is missing");

			/*
			 * If this argument has no default value, then all
			 * previous ones mustn't either.
			 */

			if ($3.defval == NULL)
			{
				int a;

				for (a = 0; a < $1.nrArgs; ++a)
					if ($1.args[a].defval != NULL)
						yyerror("Compulsory argument given after optional argument");
			}

			/* Check there is room. */

			if ($1.nrArgs == MAX_NR_ARGS)
				yyerror("Internal error - increase the value of MAX_NR_ARGS");

			$$ = $1;
			$$.args[$$.nrArgs++] = $3;
		}
	;

argvalue:	TK_SIPSIGNAL {
			$$.atype = signal_type;
			$$.argflags = 0;
			$$.nrderefs = 0;

			currentSpec -> sigslots = TRUE;
		}
	|	TK_SIPSLOT {
			$$.atype = slot_type;
			$$.argflags = 0;
			$$.nrderefs = 0;

			currentSpec -> sigslots = TRUE;
		}
	|	TK_SIPRXCON {
			$$.atype = rxcon_type;
			$$.argflags = 0;
			$$.nrderefs = 0;

			currentSpec -> sigslots = TRUE;
		}
	|	TK_SIPRXDIS {
			$$.atype = rxdis_type;
			$$.argflags = 0;
			$$.nrderefs = 0;

			currentSpec -> sigslots = TRUE;
		}
	|	TK_SIPSLOTCON '(' arglist ')' {
			$$.atype = slotcon_type;
			$$.argflags = 0;
			$$.nrderefs = 0;

			$$.u.sa = sipMalloc(sizeof (funcArgs));
			*$$.u.sa = $3;

			currentSpec -> sigslots = TRUE;
		}
	|	TK_SIPSLOTDIS '(' arglist ')' {
			$$.atype = slotdis_type;
			$$.argflags = 0;
			$$.nrderefs = 0;

			$$.u.sa = sipMalloc(sizeof (funcArgs));
			*$$.u.sa = $3;

			currentSpec -> sigslots = TRUE;
		}
	|	TK_QOBJECT {
			$$.atype = qobject_type;
			$$.argflags = 0;
			$$.nrderefs = 0;
		}
	|	argtype optassign {
			$$ = $1;
			$$.defval = $2;
		}
	;

varmember:	TK_STATIC {currentIsStatic = TRUE;} varmem
	|	varmem
	;

varmem:		member
	|	variable
	;

member:		TK_VIRTUAL {currentOverIsVirt = TRUE;} function
	|	function
	;

variable:	cpptype TK_NAME ';' optaccesscode {
			if (notSkipping())
			{
				/* Check the section. */

				if (sectionFlags != 0)
				{
					if ((sectionFlags & SECT_IS_PUBLIC) == 0)
						yyerror("Class variables must be in the public section");

					if (!currentIsStatic && $4 != NULL)
						yyerror("%AccessCode cannot be specified for non-static class variables");
				}

				newVar(currentSpec,currentModule,$2,currentIsStatic,&$1,$4);
			}

			currentIsStatic = FALSE;
		}
	;

cpptype:	TK_CONST basetype deref optref {
			$$ = $2;
			$$.nrderefs = $3;
			$$.argflags = ARG_IS_CONST | $4;
		}
	|	basetype deref optref {
			$$ = $1;
			$$.nrderefs = $2;
			$$.argflags = $3;
		}
	;

argtype:	cpptype optflags {
			$$ = $1;

			if (findOptFlag(&$2,"AllowNone",bool_flag) != NULL)
				$$.argflags |= ARG_ALLOW_NONE;

			if (findOptFlag(&$2,"GetWrapper",bool_flag) != NULL)
				$$.argflags |= ARG_GET_WRAPPER;

			if (findOptFlag(&$2,"Array",bool_flag) != NULL)
				$$.argflags |= ARG_ARRAY;

			if (findOptFlag(&$2,"ArraySize",bool_flag) != NULL)
				$$.argflags |= ARG_ARRAY_SIZE;

			if (findOptFlag(&$2,"Transfer",bool_flag) != NULL)
				$$.argflags |= ARG_XFERRED;

			if (findOptFlag(&$2,"TransferThis",bool_flag) != NULL)
				$$.argflags |= ARG_THIS_XFERRED;

			if (findOptFlag(&$2,"TransferBack",bool_flag) != NULL)
				$$.argflags |= ARG_XFERRED_BACK;

			if (findOptFlag(&$2,"In",bool_flag) != NULL)
				$$.argflags |= ARG_IN;

			if (findOptFlag(&$2,"Out",bool_flag) != NULL)
				$$.argflags |= ARG_OUT;

			if (findOptFlag(&$2,"Constrained",bool_flag) != NULL)
			{
				switch ($$.atype)
				{
				case int_type:
					$$.atype = cint_type;
					break;

				case float_type:
					$$.atype = cfloat_type;
					break;

				case double_type:
					$$.atype = cdouble_type;
					break;

				default:
					yyerror("/Constrained/ may not be specified for this type");
				}
			}
		}
	;

optref:		{
			$$ = 0;
		}
	|	'&' {
			$$ = ARG_IS_REF;
		}
	;

deref:		{
			$$ = 0;
		}
	|	deref '*' {
			$$ = $1 + 1;
		}
	;

basetype:	scopedname {
			$$.atype = defined_type;
			$$.u.snd = $1;
		}
	|	scopedname '<' cpptypelist '>' {
			templateDef *td;

			td = sipMalloc(sizeof(templateDef));
			td -> fqname = $1;
			td -> types = $3;

			$$.atype = template_type;
			$$.u.td = td;
		}
	|	TK_STRUCT scopedname {
			$$.atype = struct_type;
			$$.u.sname = $2;
		}
	|	TK_UNSIGNED TK_SHORT {
			$$.atype = ushort_type;
		}
	|	TK_SHORT {
			$$.atype = short_type;
		}
	|	TK_UNSIGNED {
			$$.atype = uint_type;
		}
	|	TK_UNSIGNED TK_INT {
			$$.atype = uint_type;
		}
	|	TK_INT {
			$$.atype = int_type;
		}
	|	TK_LONG {
			$$.atype = long_type;
		}
	|	TK_UNSIGNED TK_LONG {
			$$.atype = ulong_type;
		}
	|	TK_FLOAT {
			$$.atype = float_type;
		}
	|	TK_DOUBLE {
			$$.atype = double_type;
		}
	|	TK_BOOL {
			$$.atype = bool_type;
		}
	|	TK_UNSIGNED TK_CHAR {
			$$.atype = ustring_type;
		}
	|	TK_CHAR {
			$$.atype = string_type;
		}
	|	TK_VOID {
			$$.atype = voidptr_type;
		}
	|	TK_PYOBJECT {
			$$.atype = pyobject_type;
		}
	|	TK_PYTUPLE {
			$$.atype = pytuple_type;
		}
	|	TK_PYLIST {
			$$.atype = pylist_type;
		}
	|	TK_PYDICT {
			$$.atype = pydict_type;
		}
	|	TK_PYCALLABLE {
			$$.atype = pycallable_type;
		}
	|	TK_PYSLICE {
			$$.atype = pyslice_type;
		}
	;

cpptypelist:	cpptype {
			/* The single or first type. */

			$$.args[0] = $1;
			$$.nrArgs = 1;
		}
	|	cpptypelist ',' cpptype {
			/* Check that it wasn't ,type... */

			if ($$.nrArgs == 0)
				yyerror("First type of list is missing");

			/* Check there is room. */

			if ($1.nrArgs == MAX_NR_ARGS)
				yyerror("Internal error - increase the value of MAX_NR_ARGS");

			$$ = $1;

			$$.args[$$.nrArgs] = $3;
			$$.nrArgs++;
		}
	;

optexceptions:	{
			$$ = NULL;
		}
	|	TK_THROW '(' exceptionlist ')' {
			int e;

			/*
			 * Make sure the exceptions' header files are
			 * included.
			 */

			for (e = 0; e < $3 -> nrArgs; ++e)
				addToUsedList(&currentScope() -> iff -> used,$3 -> args[e] -> iff);

			$$ = $3;
		}
	;

exceptionlist:	{
			/* Empty list so use a blank. */

			$$ = sipMalloc(sizeof (throwArgs));
			$$ -> nrArgs = 0;
		}
	|	scopedname {
			/* The only or first exception. */

			$$ = sipMalloc(sizeof (throwArgs));
			$$ -> nrArgs = 1;
			$$ -> args[0] = findClass(currentSpec,class_iface,$1);
		}
	|	exceptionlist ',' scopedname {
			/* Check that it wasn't ...(,arg...). */

			if ($1 -> nrArgs == 0)
				yyerror("First exception of throw specifier is missing");

			/* Check there is room. */

			if ($1 -> nrArgs == MAX_NR_ARGS)
				yyerror("Internal error - increase the value of MAX_NR_ARGS");

			$$ = $1;
			$$ -> args[$$ -> nrArgs++] = findClass(currentSpec,class_iface,$3);
		}
	;

%%


/*
 * Parse the specification.
 */

void parse(sipSpec *spec,FILE *fp,char *filename,char *cppMName,
	   stringList *tsl,stringList *xfl)
{
        /* Initialise the spec. */
 
	spec -> cppmname = cppMName;
	spec -> namecache = NULL;
	spec -> moduleList = NULL;
	spec -> ifacefiles = NULL;
	spec -> classes = NULL;
	spec -> mappedtypes = NULL;
	spec -> used = NULL;
	spec -> qobjclass = -1;
	spec -> exposed = NULL;
	spec -> enums = NULL;
	spec -> vars = NULL;
	spec -> othfuncs = NULL;
	spec -> overs = NULL;
	spec -> typedefs = NULL;
	spec -> copying = NULL;
	spec -> hdrcode = NULL;
	spec -> exphdrcode = NULL;
	spec -> cppcode = NULL;
	spec -> prepycode = NULL;
	spec -> pycode = NULL;
	spec -> docs = NULL;
	spec -> postinitcode = NULL;
	spec -> mktemplates = NULL;
	spec -> sigargs = NULL;
	spec -> sigslots = FALSE;

	currentSpec = spec;
	neededQualifiers = tsl;
	excludedQualifiers = xfl;
	currentModule = NULL;
	currentMappedType = NULL;
	currentOverIsVirt = FALSE;
	currentIsStatic = FALSE;
	previousFile = NULL;
	skipStackPtr = 0;
	currentScopeIdx = 0;
	sectionFlags = 0;

	newModule(fp,filename);
	spec -> module = currentModule;

	yyparse();

	handleEOF();
	handleEOM();
}


/*
 * Tell the parser that a complete file has now been read.
 */

void parserEOF(char *name,parserContext *pc)
{
	previousFile = sipStrdup(name);
	newContext = *pc;
}


/*
 * Append a class definition to a class list if it doesn't already exist.
 * Append is needed specifically for the list of super-classes because the
 * order is important to Python.
 */

static void appendToClassList(classList **clp,classDef *cd)
{
	classList *new;

	/* Find the end of the list. */

	while (*clp != NULL)
	{
		if ((*clp) -> cd == cd)
			return;

		clp = &(*clp) -> next;
	}

	new = sipMalloc(sizeof (classList));

	new -> cd = cd;
	new -> next = NULL;

	*clp = new;
}


/*
 * Create a new module for the current specification and make it current.
 */

static void newModule(FILE *fp,char *filename)
{
	parseFile(fp,filename,currentModule,FALSE);

	currentModule = sipMalloc(sizeof (moduleDef));

	currentModule -> name = NULL;
	currentModule -> file = filename;
	currentModule -> qualifiers = NULL;
	currentModule -> nrTimeSlots = -1;
	currentModule -> license = NULL;
	currentModule -> next = currentSpec -> moduleList;

	currentSpec -> moduleList = currentModule;
}


/*
 * Switch to parsing a new file.
 */

static void parseFile(FILE *fp,char *name,moduleDef *prevmod,int optional)
{
	parserContext pc;

	pc.ifdepth = skipStackPtr;
	pc.prevmod = prevmod;

	setInputFile(fp,name,&pc,optional);
}


/*
 * Find an interface file, or create a new one.
 */

static ifaceFileDef *findIfaceFile(sipSpec *pt,scopedNameDef *fqname,
				   ifaceFileType iftype,argDef *ad)
{
	ifaceFileDef *iff;

	/* See if the name is already used. */

	for (iff = pt -> ifacefiles; iff != NULL; iff = iff -> next)
	{
		if (!sameScopedName(iff -> fqname,fqname))
			continue;

		/* They must be the same type. */

		if (iff -> type != iftype)
			yyerror("A class, namespace or mapped type has already been defined with the same name");

		/*
		 * If this is a mapped type with the same name defined in a
		 * different module, then check that this type isn't the same
		 * as any of the mapped types defined in that module.
		 */

		if (iftype == mappedtype_iface && iff -> module != currentModule)
		{
			mappedTypeDef *mtd;

			for (mtd = pt -> mappedtypes; mtd != NULL; mtd = mtd -> next)
			{
				if (mtd -> iff != iff)
					continue;

				if (ad -> atype != template_type ||
				    mtd -> type.atype != template_type ||
				    sameBaseType(ad,&mtd -> type,TRUE))
					yyerror("Mapped type has already been defined in another module");
			}

			/*
			 * If we got here then we have a mapped type based on
			 * an existing template, but with unique parameters.
			 * We don't want to use interface files from other
			 * modules, so skip this one.
			 */

			continue;
		}

		return iff;
	}

	iff = sipMalloc(sizeof (ifaceFileDef));

	iff -> name = cacheName(pt,currentModule,scopedNameTail(fqname));
	iff -> type = iftype;
	iff -> fqname = fqname;
	iff -> module = NULL;
	iff -> ifacefileflags = 0;
	iff -> used = NULL;
	iff -> next = pt -> ifacefiles;

	pt -> ifacefiles = iff;

	return iff;
}


/*
 * Find a class definition in a parse tree.
 */

static classDef *findClass(sipSpec *pt,ifaceFileType iftype,
			   scopedNameDef *fqname)
{
	classDef *cd;
	ifaceFileDef *iff;

	/* See if the name is already used. */

	iff = findIfaceFile(pt,fqname,iftype,NULL);

	for (cd = pt -> classes; cd != NULL; cd = cd -> next)
		if (cd -> iff == iff)
			return cd;

	/* Create a new one. */

	cd = sipMalloc(sizeof (classDef));

	cd -> dtorexceptions = NULL;
	cd -> iff = iff;
	cd -> next = pt -> classes;

	pt -> classes = cd;

	return cd;
}


/*
 * Add an interface file to an interface file list if it isn't already there.
 */

void addToUsedList(ifaceFileList **ifflp,ifaceFileDef *iff)
{
	ifaceFileList *iffl;

	while ((iffl = *ifflp) != NULL)
	{
		/* Don't bother if it is already there. */

		if (iffl -> iff == iff)
			return;

		ifflp = &iffl -> next;
	}

	setIsUsed(iff);

	iffl = sipMalloc(sizeof (ifaceFileList));

	iffl -> iff = iff;
	iffl -> next = NULL;

	*ifflp = iffl;
}


/*
 * Find an undefined (or create a new) class definition in a parse tree.
 */

static classDef *newClass(sipSpec *pt,ifaceFileType iftype,scopedNameDef *fqname)
{
	classDef *cd;

	cd = findClass(pt,iftype,fqname);

	/* Check it hasn't already been defined. */

	if (cd -> iff -> module != NULL)
	{
		/*
		 * Namespaces can be defined any number of times in the same
		 * module.
		 */

		if (iftype == namespace_iface && cd -> iff -> module == currentModule)
			return cd;

		yyerror(iftype == class_iface ?
			"The class has already been defined" :
			"The namespace has already been defined in another module");
	}

	cd -> iff -> module = currentModule;

	/* Complete the initialisation. */

	cd -> classflags = 0;
	cd -> classnr = -1;
	cd -> ecd = currentScope();
	cd -> node = NULL;
	cd -> supers = NULL;
	cd -> mro = NULL;
	cd -> ctors = NULL;
	cd -> defctor = NULL;
	cd -> dealloccode = NULL;
	cd -> dtorcode = NULL;
	cd -> members = NULL;
	cd -> overs = NULL;
	cd -> vmembers = NULL;
	cd -> visible = NULL;
	cd -> cppcode = NULL;
	cd -> hdrcode = (cd -> ecd != NULL ? cd -> ecd -> hdrcode : NULL);
	cd -> convtosubcode = NULL;
	cd -> subbase = NULL;
	cd -> convtocode = NULL;

	return cd;
}


/*
 * Tidy up after finishing a class definition.
 */

static void finishClass(classDef *cd,optFlags *optflgs)
{
	overDef *od;

	/* Handle any optional flags. */

	if (findOptFlag(optflgs,"CreatesThread",bool_flag) != NULL)
		setCreatesThread(cd);

	if (cd -> ctors == NULL)
	{
		/* Provide a default ctor. */

		cd -> ctors = sipMalloc(sizeof (ctorDef));
 
		cd -> ctors -> ctorflags = SECT_IS_PUBLIC;
		cd -> ctors -> args.nrArgs = 0;
		cd -> ctors -> cppsig = NULL;
		cd -> ctors -> exceptions = NULL;
		cd -> ctors -> methodcode = NULL;
		cd -> ctors -> membercode = NULL;
		cd -> ctors -> prehook = NULL;
		cd -> ctors -> posthook = NULL;
		cd -> ctors -> next = NULL;

		cd -> defctor = cd -> ctors;

		resetCannotCreate(cd);
	}
	else if (cd -> defctor == NULL)
	{
		ctorDef *ct;

		for (ct = cd -> ctors; ct != NULL; ct = ct -> next)
			if (ct -> args.nrArgs == 0 || ct -> args.args[0].defval != NULL)
			{
				cd -> defctor = ct;
				break;
			}
	}

	/* We can't create this class if it has private abstract functions. */

	for (od = cd -> overs; od != NULL; od = od -> next)
		if (isAbstract(od) && isPrivate(od))
		{
			setCannotCreate(cd);
			break;
		}

	/* We assume a public dtor if nothing specific was provided. */

	if (!isDtor(cd))
		setIsPublicDtor(cd);
}


/*
 * Create a new mapped type.
 */

static mappedTypeDef *newMappedType(sipSpec *pt,argDef *ad)
{
	mappedTypeDef *mtd;
	scopedNameDef *snd;
	ifaceFileDef *iff;

	/* Check that the type is one we want to map. */

	switch (ad -> atype)
	{
	case defined_type:
		snd = ad -> u.snd;
		break;

	case template_type:
		snd = ad -> u.td -> fqname;
		break;

	case struct_type:
		snd = ad -> u.sname;
		break;

	default:
		yyerror("Invalid type for %MappedType");
	}

	iff = findIfaceFile(pt,snd,mappedtype_iface,ad);

	/* Check it hasn't already been defined. */

	for (mtd = pt -> mappedtypes; mtd != NULL; mtd = mtd -> next)
		if (mtd -> iff == iff)
		{
			/*
			 * We allow types based on the same template but with
			 * different arguments.
			 */

			if (ad -> atype != template_type ||
			    sameBaseType(ad,&mtd -> type,TRUE))
				yyerror("Mapped type has already been defined in this module");
		}

	/* The module may not have been set yet. */

	iff -> module = currentModule;

	/* Create a new mapped type. */

	mtd = sipMalloc(sizeof (mappedTypeDef));

	mtd -> type = *ad;
	mtd -> iff = iff;
	mtd -> hdrcode = NULL;
	mtd -> convfromcode = NULL;
	mtd -> convtocode = NULL;
	mtd -> next = pt -> mappedtypes;

	pt -> mappedtypes = mtd;

	return mtd;
}


/*
 * Create a new enum.
 */

static enumDef *newEnum(sipSpec *pt,moduleDef *mod,char *name,int flags)
{
	enumDef *ed;
	classDef *escope = currentScope();

	ed = sipMalloc(sizeof (enumDef));

	ed -> enumflags = flags;
	ed -> fqname = (name != NULL ? text2scopedName(name) : NULL);
	ed -> ecd = escope;
	ed -> module = mod;
	ed -> values = NULL;
	ed -> next = pt -> enums;

	pt -> enums = ed;

	if (escope != NULL)
		setHasEnums(escope);

	return ed;
}


/*
 * Create a new typedef.
 */

static void newTypedef(sipSpec *pt,char *name,argDef *type)
{
	typedefDef *td;
	scopedNameDef *fqname = text2scopedName(name);

	/* Check it doesn't already exist. */

	for (td = pt -> typedefs; td != NULL; td = td -> next)
		if (sameScopedName(td -> fqname,fqname))
		{
			fatalScopedName(fqname);
			fatal(" already defined\n");
		}

	td = sipMalloc(sizeof (typedefDef));

	td -> fqname = fqname;
	td -> ecd = currentScope();
	td -> type = *type;
	td -> next = pt -> typedefs;

	pt -> typedefs = td;
}


/*
 * Create a new variable.
 */

static void newVar(sipSpec *pt,moduleDef *mod,char *name,int isstatic,
		   argDef *type,codeBlock *code)
{
	varDef *var;
	classDef *escope = currentScope();
	nameDef *nd = cacheName(pt,mod,name);

	checkAttributes(pt,escope,nd,var_attr);

	var = sipMalloc(sizeof (varDef));

	var -> name = nd;
	var -> fqname = text2scopedName(name);
	var -> ecd = escope;
	var -> module = mod;
	var -> varflags = 0;
	var -> type = *type;
	var -> accessfunc = code;
	var -> next = pt -> vars;

	if (isstatic)
		setIsStaticVar(var);

	pt -> vars = var;
}


/*
 * Create a new ctor.
 */

static void newCtor(char *name,int sectFlags,funcArgs *args,optFlags *optflgs,
		    codeBlock *methodcode,codeBlock *membercode,
		    throwArgs *exceptions,signatureDef *cppsig)
{
	ctorDef *ct, **ctp;
	classDef *cd = currentScope();

	if (methodcode != NULL && membercode != NULL)
		yyerror("Cannot provide %MethodCode and %MemberCode");

	/* Check the name of the constructor. */

	if (strcmp(classBaseName(cd),name) != 0)
		yyerror("Constructor doesn't have the same name as its class");

	/*
	 * Find the end of the list and, at the same time, check for any
	 * existing one with the same signature.
	 */

	for (ctp = &cd -> ctors; *ctp != NULL; ctp = &(*ctp) -> next)
		if (sameFuncArgs(&(*ctp) -> args,args,FALSE))
			yyerror("A constructor with the same Python signature has already been defined");

	/* Add to the list of constructors. */

	ct = sipMalloc(sizeof (ctorDef));

	ct -> ctorflags = sectFlags;
	ct -> args = *args;
	ct -> cppsig = cppsig;
	ct -> exceptions = exceptions;
	ct -> methodcode = methodcode;
	ct -> membercode = membercode;
	ct -> next = NULL;

	if (isProtectedCtor(ct))
		setIsComplex(cd);

	if (!isPrivateCtor(ct))
		resetCannotCreate(cd);

	getHooks(optflgs,&ct -> prehook,&ct -> posthook);

	if (findOptFlag(optflgs,"Default",bool_flag) != NULL)
	{
		if (cd -> defctor != NULL)
			yyerror("A constructor with the /Default/ flag has already been defined");

		cd -> defctor = ct;
	}

	*ctp = ct;
}


/*
 * Create a new function.
 */

static void newFunction(sipSpec *pt,moduleDef *mod,int sflags,int isstatic,
			int isvirt,argDef *type,char *name,funcArgs *args,
			int isconst,int isabstract,optFlags *optflgs,
			codeBlock *methodcode,codeBlock *membercode,
			codeBlock *vcode,codeBlock *ovcode,
			throwArgs *exceptions,signatureDef *cppsig)
{
	static struct slot_map {
		char *name;		/* The slot name. */
		slotType type;		/* The corresponding type. */
		int hwcode;		/* If handwritten code is required. */
		int nrargs;		/* Nr. of arguments. */
	} slot_table[] = {
		{"__str__", str_slot, TRUE, 0},
		{"__unicode__", unicode_slot, TRUE, 0},
		{"__int__", int_slot, FALSE, 0},
		{"__len__", len_slot, TRUE, 0},
		{"__contains__", contains_slot, TRUE, 1},
		{"__add__", add_slot, FALSE, 1},
		{"__sub__", sub_slot, FALSE, 1},
		{"__mul__", mul_slot, FALSE, 1},
		{"__div__", div_slot, FALSE, 1},
		{"__mod__", mod_slot, FALSE, 1},
		{"__and__", and_slot, FALSE, 1},
		{"__or__", or_slot, FALSE, 1},
		{"__xor__", xor_slot, FALSE, 1},
		{"__lshift__", lshift_slot, FALSE, 1},
		{"__rshift__", rshift_slot, FALSE, 1},
		{"__iadd__", iadd_slot, FALSE, 1},
		{"__isub__", isub_slot, FALSE, 1},
		{"__imul__", imul_slot, FALSE, 1},
		{"__idiv__", idiv_slot, FALSE, 1},
		{"__imod__", imod_slot, FALSE, 1},
		{"__iand__", iand_slot, FALSE, 1},
		{"__ior__", ior_slot, FALSE, 1},
		{"__ixor__", ixor_slot, FALSE, 1},
		{"__ilshift__", ilshift_slot, FALSE, 1},
		{"__irshift__", irshift_slot, FALSE, 1},
		{"__invert__", invert_slot, FALSE, 0},
		{"__call__", call_slot, TRUE, -1},
		{"__getitem__", getitem_slot, TRUE, -1},
		{"__setitem__", setitem_slot, TRUE, -1},
		{"__delitem__", delitem_slot, TRUE, -1},
		{"__lt__", lt_slot, FALSE, 1},
		{"__le__", le_slot, FALSE, 1},
		{"__eq__", eq_slot, FALSE, 1},
		{"__ne__", ne_slot, FALSE, 1},
		{"__gt__", gt_slot, FALSE, 1},
		{"__ge__", ge_slot, FALSE, 1},
		{"__cmp__", cmp_slot, FALSE, 1},
		{"__nonzero__", nonzero_slot, TRUE, 0},
		{"__neg__", neg_slot, FALSE, 0},
		{"__repr__", repr_slot, TRUE, 0},
		{NULL}
	};

	classDef *cd = currentScope();
	char *pname;
	int factory, xferback;
	overDef *od, **odp, **headp;
	optFlag *of;
	struct slot_map *sm;
	slotType st;

	if (methodcode != NULL && membercode != NULL)
		yyerror("Cannot provide %MethodCode and %MemberCode");

	if (vcode != NULL && ovcode != NULL)
		yyerror("Cannot provide %VirtualCatcherCode and %VirtualCode");

        /* Get the slot type. */

	st = no_slot;

	for (sm = slot_table; sm -> name != NULL; ++sm)
		if (strcmp(sm -> name,name) == 0)
		{
			/* Apply the restrictions on operators. */

			if (cd == NULL || (sflags & SECT_IS_PUBLIC) == 0)
				yyerror("Operators must be defined in the public section of a class");

			if (sm -> hwcode && methodcode == NULL && membercode == NULL)
				yyerror("This operator requires %MethodCode or %MemberCode");

			if (sm -> nrargs < 0)
			{
				int exact_nr;

				/*
				 * These require an exact number, for SIP v3.x
				 * but not for SIP v4.x.
				 */
				switch (sm -> type)
				{
				case getitem_slot:
				case delitem_slot:
					exact_nr = 1;
					break;

				case setitem_slot:
					exact_nr = 2;
					break;

				default:
					exact_nr = 0;
				}

				if (exact_nr > 0)
					if (args -> nrArgs < exact_nr)
						yyerror("Incorrect number of arguments to operator");
					else if (args -> nrArgs > exact_nr)
						yyerror("Too many arguments to operator - use SIP v4.0 or later");
			}
			else if (sm -> nrargs != args -> nrArgs)
				yyerror("Incorrect number of arguments to operator");

			if (isvirt)
				yyerror("Operators cannot be virtual");

			if (isabstract)
				yyerror("Operators cannot be abstract");

			st = sm -> type;

			break;
		}

	headp = (cd != NULL ?  &cd -> overs : &pt -> overs);

	/* See if it is a factory method. */
	if (findOptFlag(optflgs,"Factory",bool_flag) != NULL)
		factory = TRUE;
	else
	{
		int a;

		factory = FALSE;

		/* Check /TransferThis/ wasn't specified. */
		for (a = 0; a < args -> nrArgs; ++a)
			if (isThisTransferred(&args -> args[a]))
				yyerror("/TransferThis/ may only be specified in constructors and /Factory/ methods");
	}

	/* See if the result is to be returned to Python ownership. */
	xferback = (findOptFlag(optflgs,"TransferBack",bool_flag) != NULL);

	if (factory && xferback)
		yyerror("/TransferBack/ and /Factory/ cannot both be specified");

	/* Use the C++ name if a Python name wasn't given. */

	if ((of = findOptFlag(optflgs,"PyName",name_flag)) != NULL)
		pname = of -> fvalue.sval;
	else
		pname = name;

	/* Create a new overload definition. */

	od = sipMalloc(sizeof (overDef));

	/* Set the overload flags. */

	od -> overflags = sflags;

	if (factory)
		setIsFactory(od);

	if (xferback)
		setIsResultTransferredBack(od);

	if (isProtected(od))
		setIsComplex(cd);

	if ((isSlot(od) || isSignal(od)) && !isPrivate(od))
	{
		if (isSignal(od))
			setIsComplex(cd);

		pt -> sigslots = TRUE;
	}

	if (isSignal(od) && (methodcode != NULL || membercode != NULL || vcode != NULL))
		yyerror("Cannot provide code for signals");

	if (isstatic)
	{
		if (isSlot(od) || isSignal(od))
			yyerror("Static functions cannot be slots or signals");

		if (isvirt)
			yyerror("Static functions cannot be virtual");

		setIsStatic(od);
	}

	if (isconst)
		setIsConst(od);

	if (isabstract)
	{
		if (sflags == 0)
			yyerror("Non-class function specified as abstract");

		setIsAbstract(od);
	}

	if ((of = findOptFlag(optflgs,"AutoGen",opt_name_flag)) != NULL)
	{
		setIsAutoGen(od);

		if (of -> fvalue.sval != NULL)
		{
			qualDef *qd;

			if ((qd = findQualifier(pt,of -> fvalue.sval)) == NULL || qd -> qtype != feature_qualifier)
				yyerror("No such feature");

			if (excludedFeature(excludedQualifiers,qd))
				resetIsAutoGen(od);
		}
	}

	if (isvirt)
	{
		if (isSignal(od))
			yyerror("Virtual signals aren't supported");

		setIsVirtual(od);
		setIsComplex(cd);
	}
	else if (vcode != NULL)
		yyerror("%VirtualCode provided for non-virtual function");

	od -> cppname = name;
	od -> args = *args;
	od -> cppsig = cppsig;
	od -> exceptions = exceptions;
	od -> methodcode = methodcode;
	od -> membercode = membercode;
	od -> virtcode = vcode;
	od -> ovirtcode = ovcode;
	od -> common = findFunction(pt,mod,cd,pname,st);

	/* For Python slots, %MethodCode and %MemberCode are synonymous. */

	if (od -> common -> slot != no_slot && membercode != NULL)
	{
		od -> methodcode = membercode;
		od -> membercode = NULL;
	}

	/* Methods that run in new threads must be virtual. */

	if (findOptFlag(optflgs,"NewThread",bool_flag) != NULL)
	{
		if (!isvirt)
			yyerror("/NewThread/ may only be specified for virtual methods");

		setIsNewThread(od);
	}

	getHooks(optflgs,&od -> prehook,&od -> posthook);

	if (type -> atype == voidptr_type && type -> nrderefs == 0)
		od -> result = NULL;
	else
	{
		od -> result = sipMalloc(sizeof (argDef));
		*od -> result = *type;
	}

	od -> next = NULL;

	/*
	 * Find the end of the list and, at the same time, check for any
	 * existing one with the same signature.
	 */

	for (odp = headp; *odp != NULL; odp = &(*odp) -> next)
	{
		if ((*odp) -> common != od -> common)
			continue;

		if (!sameOverload(*odp,od,FALSE))
			continue;

		yyerror("A function with the same Python signature has already been defined");
	}

	*odp = od;
}


/*
 * Cache a name in a module.
 */

static nameDef *cacheName(sipSpec *pt,moduleDef *mod,char *name)
{
	nameDef *nd;

	/* See if it already exists. */

	for (nd = pt -> namecache; nd != NULL; nd = nd -> next)
		if (strcmp(nd -> text,name) == 0)
			return nd;

	/* Create a new one. */

	nd = sipMalloc(sizeof (nameDef));

	nd -> text = name;
	nd -> module = mod;
	nd -> next = pt -> namecache;

	pt -> namecache = nd;

	return nd;
}


/*
 * Find (or create) an overloaded function name.
 */

static memberDef *findFunction(sipSpec *pt,moduleDef *mod,classDef *cd,
			       char *pname,slotType st)
{
	memberDef *md, **flist;
	nameDef *pnd;

	/* Cache the name. */

	pnd = cacheName(pt,mod,pname);

	/* Check there is no name clash. */

	checkAttributes(pt,cd,pnd,func_attr);

	/* See if it already exists. */

	flist = (cd != NULL ? &cd -> members : &pt -> othfuncs);

	for (md = *flist; md != NULL; md = md -> next)
		if (md -> pyname == pnd)
			return md;

	/* Create a new one. */

	md = sipMalloc(sizeof (memberDef));

	md -> pyname = pnd;
	md -> slot = st;
	md -> module = mod;
	md -> next = *flist;

	*flist = md;

	return md;
}


/*
 * Search a set of flags for a particular one and check its type.
 */

static optFlag *findOptFlag(optFlags *flgs,char *name,flagType ft)
{
	int f;

	for (f = 0; f < flgs -> nrFlags; ++f)
	{
		optFlag *of = &flgs -> flags[f];

		if (strcmp(of -> fname,name) == 0)
		{
			/*
			 * An optional name can look like a boolean or a name.
			 */

			if (ft == opt_name_flag)
			{
				if (of -> ftype == bool_flag)
				{
					of -> ftype = opt_name_flag;
					of -> fvalue.sval = NULL;
				}
				else if (of -> ftype == name_flag)
					of -> ftype = opt_name_flag;
			}

			if (ft != of -> ftype)
				yyerror("Optional flag has a value of the wrong type");

			return of;
		}
	}

	return NULL;
}


/*
 * A name is going to be used as a Python attribute name within a Python scope
 * (ie. a Python dictionary), so check against what we already know is going in
 * the same scope in case there is a clash.
 */

static void checkAttributes(sipSpec *pt,classDef *pyscope,nameDef *attr,
			    attrType at)
{
	enumDef *ed;
	varDef *vd;
	classDef *cd;

	/* Check the enum values. */

	for (ed = pt -> enums; ed != NULL; ed = ed -> next)
	{
		enumValueDef *evd;

		if (ed -> ecd != pyscope)
			continue;

		for (evd = ed -> values; evd != NULL; evd = evd -> next)
			if (evd -> name == attr)
				yyerror("There is already an enum value in scope with the same Python name");
	}

	/* Check the variables. */

	for (vd = pt -> vars; vd != NULL; vd = vd -> next)
	{
		if (vd -> ecd != pyscope)
			continue;

		if (vd -> name != attr)
			continue;

		yyerror("There is already a variable in scope with the same Python name");
	}

	/*
	 * Only check the members if this attribute isn't a member because we
	 * can handle members with the same name in the same scope.
	 */

	if (at != func_attr)
	{
		memberDef *md, *membs;

		membs = (pyscope != NULL ? pyscope -> members : pt -> othfuncs);

		for (md = membs; md != NULL; md = md -> next)
		{
			overDef *od, *overs;

			if (md -> pyname != attr)
				continue;

			/* Check for a conflict with all overloads. */

			overs = (pyscope != NULL ? pyscope -> overs : pt -> overs);

			for (od = overs; od != NULL; od = od -> next)
			{
				if (od -> common != md)
					continue;

				yyerror("There is already a function in scope with the same Python name");
			}
		}
	}

	/* Check the classes. */

	for (cd = pt -> classes; cd != NULL; cd = cd -> next)
	{
		if (cd -> ecd != pyscope)
			continue;

		if (cd -> iff -> name != attr)
			continue;

		yyerror("There is already a class or namespace in scope with the same Python name");
	}
}


/*
 * Append a code block to a list of them.  Append is needed to give the
 * specifier easy control over the order of the documentation.
 */

static void appendCodeBlock(codeBlock **headp,codeBlock *new)
{
	while (*headp != NULL)
		headp = &(*headp) -> next;

	*headp = new;
}


/*
 * Handle the end of a fully parsed a file.
 */

static void handleEOF()
{
	/*
	 * Check that the number of nested if's is the same as when we started
	 * the file.
	 */

	if (skipStackPtr > newContext.ifdepth)
		fatal("Too many %%If statements in %s\n",previousFile);

	if (skipStackPtr < newContext.ifdepth)
		fatal("Too many %%End statements in %s\n",previousFile);
}


/*
 * Handle the end of a fully parsed a module.
 */

static void handleEOM()
{
	/* Check it has been named. */

	if (currentModule -> name == NULL)
		fatal("No %%Module has been specified for module defined in %s\n",previousFile);

	/* The previous module is now current. */

	currentModule = newContext.prevmod;
}


/*
 * Find an existing qualifier.
 */

static qualDef *findQualifier(sipSpec *pt,char *name)
{
	moduleDef *mod;

	for (mod = pt -> moduleList; mod != NULL; mod = mod -> next)
	{
		qualDef *qd;

		for (qd = mod -> qualifiers; qd != NULL; qd = qd -> next)
			if (strcmp(qd -> name,name) == 0)
				return qd;
	}

	return NULL;
}


/*
 * Return a copy of a scoped name.
 */

scopedNameDef *copyScopedName(scopedNameDef *snd)
{
	scopedNameDef *head;

	head = NULL;

	while (snd != NULL)
	{
		appendScopedName(&head,text2scopePart(snd -> name));
		snd = snd -> next;
	}

	return head;
}


/*
 * Append a name to a list of scopes.
 */

void appendScopedName(scopedNameDef **headp,scopedNameDef *newsnd)
{
	while (*headp != NULL)
		headp = &(*headp) -> next;

	*headp = newsnd;
}


/*
 * Free a scoped name - but not the text itself.
 */

void freeScopedName(scopedNameDef *snd)
{
	while (snd != NULL)
	{
		scopedNameDef *next = snd -> next;

		free(snd);

		snd = next;
	}
}


/*
 * Convert a text string to a scope part structure.
 */

scopedNameDef *text2scopePart(char *text)
{
	scopedNameDef *snd;

	snd = sipMalloc(sizeof (scopedNameDef));

	snd -> name = text;
	snd -> next = NULL;

	return snd;
}


/*
 * Convert a text string to a fully scoped name.
 */

static scopedNameDef *text2scopedName(char *text)
{
	classDef *cd = currentScope();
	scopedNameDef *snd;

	snd = (cd != NULL ? copyScopedName(cd -> iff -> fqname) : NULL);

	appendScopedName(&snd,text2scopePart(text));

	return snd;
}


/*
 * Return a pointer to the tail part of a scoped name.
 */

char *scopedNameTail(scopedNameDef *snd)
{
	if (snd == NULL)
		return NULL;

	while (snd -> next != NULL)
		snd = snd -> next;

	return snd -> name;
}


/*
 * Push the given scope onto the scope stack.
 */

static void pushScope(classDef *scope)
{
	if (currentScopeIdx >= MAX_NESTED_SCOPE)
		fatal("Internal error: increase the value of MAX_NESTED_SCOPE\n");

	scopeStack[currentScopeIdx] = scope;
	sectFlagsStack[currentScopeIdx] = sectionFlags;

	++currentScopeIdx;
}


/*
 * Pop the scope stack.
 */

static void popScope(void)
{
	if (currentScopeIdx > 0)
		sectionFlags = sectFlagsStack[currentScopeIdx--];
}


/*
 * Return non-zero if the current input should be parsed rather than be
 * skipped.
 */

static int notSkipping()
{
	return (skipStackPtr == 0 ? TRUE : skipStack[skipStackPtr - 1]);
}


/*
 * Return the value of an expression involving a time period.
 */

static int timePeriod(sipSpec *pt,char *lname,char *uname)
{
	int this;
	qualDef *qd, *lower, *upper;
	moduleDef *mod;

	if (lname == NULL)
		lower = NULL;
	else if ((lower = findQualifier(pt,lname)) == NULL || lower -> qtype != time_qualifier)
		yyerror("Lower bound is not a time version");

	if (uname == NULL)
		upper = NULL;
	else if ((upper = findQualifier(pt,uname)) == NULL || upper -> qtype != time_qualifier)
		yyerror("Upper bound is not a time version");

	/* Sanity checks on the bounds. */

	if (lower == NULL && upper == NULL)
		yyerror("Lower and upper bounds cannot both be omitted");

	if (lower != NULL && upper != NULL)
	{
		if (lower -> module != upper -> module)
			yyerror("Lower and upper bounds are from different timelines");

		if (lower == upper)
			yyerror("Lower and upper bounds must be different");

		if (lower -> order > upper -> order)
			yyerror("Later version specified as lower bound");
	}

	/* Go through each slot in the relevant timeline. */

	if (lower != NULL)
		mod = lower -> module;
	else
		mod = upper -> module;

	this = FALSE;

	for (qd = mod -> qualifiers; qd != NULL; qd = qd -> next)
	{
		if (qd -> qtype != time_qualifier)
			continue;

		if (lower != NULL && qd -> order < lower -> order)
			continue;

		if (upper != NULL && qd -> order >= upper -> order)
			continue;

		/*
		 * This is within the required range so if it is also needed
		 * then the expression is true.
		 */

		if (isNeeded(qd))
		{
			this = TRUE;
			break;
		}
	}

	return this;
}


/*
 * Return the value of an expression involving a single platform or feature.
 */

static int platOrFeature(sipSpec *pt,char *name,int optnot)
{
	int this;
	qualDef *qd;

	if ((qd = findQualifier(pt,name)) == NULL || qd -> qtype == time_qualifier)
		yyerror("No such platform or feature");

	/* Assume this sub-expression is false. */

	this = FALSE;

	if (qd -> qtype == feature_qualifier)
	{
		if (!excludedFeature(excludedQualifiers,qd))
			this = TRUE;
	}
	else if (isNeeded(qd))
		this = TRUE;

	if (optnot)
		this = !this;

	return this;
}


/*
 * Return TRUE if the given feature qualifier is excluded.
 */

int excludedFeature(stringList *xsl,qualDef *qd)
{
	while (xsl != NULL)
	{
		if (strcmp(qd -> name,xsl -> s) == 0)
			return TRUE;

		xsl = xsl -> next;
	}

	return FALSE;
}


/*
 * Return TRUE if the given qualifier is needed.
 */

static int isNeeded(qualDef *qd)
{
	stringList *sl;

	for (sl = neededQualifiers; sl != NULL; sl = sl -> next)
		if (strcmp(qd -> name,sl -> s) == 0)
			return TRUE;

	return FALSE;
}


/*
 * Return the current scope.  currentScope() is only valid if notSkipping()
 * returns non-zero.
 */

static classDef *currentScope(void)
{
	return (currentScopeIdx > 0 ? scopeStack[currentScopeIdx - 1] : NULL);
}


/*
 * Create a new qualifier.
 */

static void newQualifier(sipSpec *pt,moduleDef *mod,int order,char *name,
			 qualType qt)
{
	qualDef *qd;

	/* Check it doesn't already exist. */

	if (findQualifier(pt,name) != NULL)
		yyerror("Version is already defined");

	qd = sipMalloc(sizeof (qualDef));

	qd -> name = name;
	qd -> qtype = qt;
	qd -> module = mod;
	qd -> order = order;
	qd -> next = mod -> qualifiers;

	mod -> qualifiers = qd;
}


/*
 * Create a new imported module.
 */

static void newImport(sipSpec *pt,char *name)
{
	/* Ignore if the file has already been imported. */

	moduleDef *mod;

	for (mod = pt -> moduleList; mod != NULL; mod = mod -> next)
		if (strcmp(mod -> file,name) == 0)
			return;

	newModule(NULL,name);
}


/*
 * If the use of an interface is by the main module, add it to the used list.
 */

static void usedInMainModule(sipSpec *pt,ifaceFileDef *iff)
{
	if (isMainModule(iff -> module))
		addToUsedList(&pt -> used,iff);
}


/*
 * Set up pointers to hook names.
 */

static void getHooks(optFlags *optflgs,char **pre,char **post)
{
	optFlag *of;

	if ((of = findOptFlag(optflgs,"PreHook",name_flag)) != NULL)
		*pre = of -> fvalue.sval;
	else
		*pre = NULL;

	if ((of = findOptFlag(optflgs,"PostHook",name_flag)) != NULL)
		*post = of -> fvalue.sval;
	else
		*post = NULL;
}
