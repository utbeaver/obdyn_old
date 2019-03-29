/*
 * The SIP lexer.
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "sip.h"
#include "parser.h"


#ifndef FLEX_SCANNER
#error "Only flex is supported at the moment"
#endif


#define	YY_FATAL_ERROR(s)	fatallex(s)

#define	MAX_INCLUDE_DEPTH	10


static struct inputFile {
	int		lineno;		/* The line number. */
	YY_BUFFER_STATE	bs;		/* The flex buffer state handle. */
	char		*name;		/* The file name. */
	char		*cwd;		/* The path part of the file name. */
	parserContext	pc;		/* The parser context. */
} inputFileStack[MAX_INCLUDE_DEPTH];

static int currentFile = -1;		/* Index of the current input file. */

static FILE *openFile(char *);
static void fatallex(char *);
%}

%s code
%x ccomment

%%

^[ \t]*%Include			{return TK_INCLUDE;}
^[ \t]*%OptionalInclude		{return TK_OPTINCLUDE;}
^[ \t]*%Import			{return TK_IMPORT;}
^[ \t]*%Module			{return TK_MODULE;}
^[ \t]*%Timeline		{return TK_TIMELINE;}
^[ \t]*%Platforms		{return TK_PLATFORMS;}
^[ \t]*%Feature			{return TK_FEATURE;}
^[ \t]*%License			{return TK_LICENSE;}
^[ \t]*%ExposeFunction		{return TK_EXPOSE;}
^[ \t]*%MappedType		{return TK_MAPPEDTYPE;}
^[ \t]*%If			{return TK_IF;}
<INITIAL>^[ \t]*%End		{return TK_END;}
class				{return TK_CLASS;}
struct				{return TK_STRUCT;}
public				{return TK_PUBLIC;}
protected			{return TK_PROTECTED;}
private				{return TK_PRIVATE;}
signals				{return TK_SIGNALS;}
slots				{return TK_SLOTS;}
char				{return TK_CHAR;}
bool				{return TK_BOOL;}
short				{return TK_SHORT;}
int				{return TK_INT;}
long				{return TK_LONG;}
float				{return TK_FLOAT;}
double				{return TK_DOUBLE;}
void				{return TK_VOID;}
virtual				{return TK_VIRTUAL;}
enum				{return TK_ENUM;}
unsigned			{return TK_UNSIGNED;}
const				{return TK_CONST;}
static				{return TK_STATIC;}
true				{return TK_TRUE;}
false				{return TK_FALSE;}
typedef				{return TK_TYPEDEF;}
namespace			{return TK_NAMESPACE;}
operator			{return TK_OPERATOR;}
throw				{return TK_THROW;}
::				{return TK_SCOPE;}
\|\|				{return TK_LOGICAL_OR;}
SIP_PYOBJECT			{return TK_PYOBJECT;}
SIP_PYTUPLE			{return TK_PYTUPLE;}
SIP_PYLIST			{return TK_PYLIST;}
SIP_PYDICT			{return TK_PYDICT;}
SIP_PYCALLABLE			{return TK_PYCALLABLE;}
SIP_PYSLICE			{return TK_PYSLICE;}
SIP_SLICE			{return TK_PYSLICE;}	/* Deprecated. */
SIP_SIGNAL			{return TK_SIPSIGNAL;}
SIP_SLOT			{return TK_SIPSLOT;}
SIP_RXOBJ_CON			{return TK_SIPRXCON;}
SIP_RXOBJ_DIS			{return TK_SIPRXDIS;}
SIP_SLOT_CON			{return TK_SIPSLOTCON;}
SIP_SLOT_DIS			{return TK_SIPSLOTDIS;}
SIP_QOBJECT			{return TK_QOBJECT;}


[ \t] {				/* Ignore whitespace. */
	;
}

\n {				/* Maintain the line number. */
	++inputFileStack[currentFile].lineno;
}

\/\/.* {			/* Ignore C++ style comments. */
	;
}


-?[0-9]+ {			/* A signed decimal number. */
	yylval.number = strtol(yytext,NULL,0);
	return TK_NUMBER;
}


-?(([0-9]+)|([0-9]*\.[0-9]+)([eE][-+]?[0-9]+)?) {/* A floating point number. */
	yylval.real = strtod(yytext,NULL);
	return TK_REAL;
}


0x[0-9a-fA-F]+ {		/* An unsigned hexadecimal number. */
	yylval.number = strtol(yytext,NULL,16);
	return TK_NUMBER;
}


[_A-Za-z][_A-Za-z0-9]* {	/* An identifier name. */
	yylval.text = sipStrdup(yytext);
	return TK_NAME;
}


[._A-Za-z][._/A-Za-z0-9\-]*[._A-Za-z0-9] {	/* A relative pathname. */
	yylval.text = sipStrdup(yytext);
	return TK_PATHNAME;
}


\"[^"\n]*["\n] {		/* A double-quoted string. */
	char *dp, *sp;

	/* Copy the string without the quotes. */

	yylval.text = sipMalloc(strlen(yytext) + 1);

	dp = yylval.text;
	sp = yytext;

	while (*sp != '\0')
	{
		if (*sp != '"')
			*dp++ = *sp;

		++sp;
	}

	*dp = '\0';

	return TK_STRING;
}


\'[^'\n]*['\n] {		/* A single-quoted character. */
	if (strlen(yytext) != 3)
		fatallex("Exactly one character expected between single quotes");

	yylval.qchar = yytext[1];

	return TK_QCHAR;
}


\/\* {				/* Ignore C-style comments. */
	BEGIN ccomment;
}
<ccomment>\n {
	++inputFileStack[currentFile].lineno;
}
<ccomment>\*\/ {
	BEGIN INITIAL;
}
<ccomment>. {
	;
}


^%Copying {			/* The software license. */
	BEGIN code;
	return TK_COPYING;
}

^%ConvertFromTypeCode {		/* The start of a from-type code block. */
	BEGIN code;
	return TK_FROMTYPE;
}

^%ConvertToTypeCode {		/* The start of a to-type code block. */
	BEGIN code;
	return TK_TOTYPE;
}

^%ConvertToSubClassCode {	/* The start of a to-sub-class code block. */
	BEGIN code;
	return TK_TOSUBCLASS;
}

^%HeaderCode {			/* The start of a C++ header code block. */
	BEGIN code;
	return TK_HEADERCODE;
}

^%ModuleHeaderCode {		/* The start of a module header code block. */
	BEGIN code;
	return TK_MODHEADERCODE;
}

^%TypeHeaderCode {		/* The start of a type header code block. */
	BEGIN code;
	return TK_TYPEHEADERCODE;
}

^%ExportedHeaderCode {		/* The start of an exported C++ header code block. */
	BEGIN code;
	return TK_EXPHEADERCODE;
}

^%PostInitialisationC\+\+Code {	/* The start of a post-initialisation C++ code block. */
	BEGIN code;
	yywarning("%PostInitialisationC++Code has been renamed %PostInitialisationCode in SIP v4 and later\n");
	return TK_POSTINITCODE;
}

^%PostInitialisationCode {	/* The start of a post-initialisation code block. */
	BEGIN code;
	return TK_POSTINITCODE;
}

^%C\+\+Code {			/* The start of a C++ code block. */
	BEGIN code;
	return TK_CPPCODE;
}

^%ModuleCode {			/* The start of a module code block. */
	BEGIN code;
	return TK_MODCODE;
}

^%TypeCode {			/* The start of a type code block. */
	BEGIN code;
	return TK_TYPECODE;
}

^%MethodCode {			/* The start of a C++ method code block. */
	BEGIN code;
	return TK_METHODCODE;
}

^%MemberCode {			/* The start of a C++ member code block. */
	BEGIN code;
	return TK_MEMBERCODE;
}

^%VirtualCatcherCode {		/* The start of a C++ virtual catcher code block. */
	BEGIN code;
	return TK_VIRTUALCATCHERCODE;
}

^%VirtualCode {			/* The start of a C++ virtual code block. */
	BEGIN code;
	return TK_VIRTUALCODE;
}

^%PrePythonCode {		/* The start of a pre-Python code block. */
	BEGIN code;
	return TK_PREPYCODE;
}

^%PythonCode {			/* The start of a Python code block. */
	BEGIN code;
	return TK_PYCODE;
}

^%Doc {				/* The start of a documentation block. */
	BEGIN code;
	return TK_DOC;
}

^%ExportedDoc {			/* The start of an exported documentation block. */
	BEGIN code;
	return TK_EXPORTEDDOC;
}

^%Makefile {			/* The start of a Makefile code block. */
	BEGIN code;
	return TK_MAKEFILE;
}

^%AccessCode {			/* The start of an access code block. */
	BEGIN code;
	return TK_ACCESSCODE;
}

^%VariableCode {		/* The start of an access code block - deprecated. */
	BEGIN code;
	return TK_VARCODE;
}

<code>^%End {			/* The end of a code block. */
	BEGIN INITIAL;
	return TK_END;
}

<code>^[^%]* {			/* The contents of a code block. */
	char *cp;
	struct inputFile *ifp;

	ifp = &inputFileStack[currentFile];

	yylval.codeb = sipMalloc(sizeof (codeBlock));

	yylval.codeb -> frag = sipStrdup(yytext);
	yylval.codeb -> linenr = ifp -> lineno;
	yylval.codeb -> filename = sipStrdup(ifp -> name);
	yylval.codeb -> next = NULL;

	/* Bump the line count. */

	for (cp = yytext; *cp != '\0'; ++cp)
		if (*cp == '\n')
			ifp -> lineno++;

	return TK_CODEBLOCK;
}

. {				/* Any else is returned as is. */
	return yytext[0];
}

%%

/*
 * Hook into EOF handling.  Return 0 if there is more to process.
 */

int yywrap()
{
	char *cwd;
	struct inputFile *ifp;

	if ((cwd = inputFileStack[currentFile].cwd) != NULL)
		free(cwd);

	ifp = &inputFileStack[currentFile--];

	/* Tell the parser if this is the end of a file. */

	parserEOF(ifp -> name,&ifp -> pc);

	/* Tidy up this file. */

	fclose(yyin);
	free(ifp -> name);

	/* See if this was the original file. */

	if (currentFile < 0)
		return 1;

	yy_delete_buffer(YY_CURRENT_BUFFER);
	yy_switch_to_buffer(ifp -> bs);

	return 0;
}


/*
 * Set up an input file to be read by the lexer, opening it if necessary.
 */

void setInputFile(FILE *fp,char *name,parserContext *pc,int optional)
{
	char *fullname = NULL;

	if (currentFile >= MAX_INCLUDE_DEPTH - 1)
		fatal("Too many nested %%Include, %%OptionalInclude or %%Import statements\n");

	if (fp != NULL || (fp = openFile(name)) != NULL)
		fullname = sipStrdup(name);
	else
	{
		char *cwd;

		/* Try the directory that contains the current file. */
		if (currentFile >= 0 && (cwd = inputFileStack[currentFile].cwd) != NULL)
		{
			fullname = concat(cwd,"/",name,NULL);

			if ((fp = openFile(fullname)) == NULL)
			{
				free(fullname);
				fullname = NULL;
			}
		}
	}

	/* Try the include path if we haven't found anything yet. */
	if (fullname == NULL)
	{
		stringList *sl;

		fullname = NULL;

		for (sl = includeDirList; sl != NULL; sl = sl -> next)
		{
			if (fullname != NULL)
				free(fullname);

			fullname = concat(sl -> s,"/",name,NULL);

			if ((fp = openFile(fullname)) != NULL)
				break;
		}

		if (fp == NULL && !optional)
			fatal("Unable to find file \"%s\"\n",name);
	}

	if (fp != NULL)
	{
		char *cwd;

		yyin = fp;

		++currentFile;

		/*
		 * Remember the directory containing the new file and make it
		 * "current".
		 */
		if ((cwd = strchr(fullname,'/')) != NULL)
		{
			cwd = sipStrdup(fullname);
			*strrchr(cwd,'/') = '\0';
		}

		inputFileStack[currentFile].lineno = 1;
		inputFileStack[currentFile].name = fullname;
		inputFileStack[currentFile].pc = *pc;
		inputFileStack[currentFile].cwd = cwd;

		if (currentFile > 0)
		{
			inputFileStack[currentFile].bs = YY_CURRENT_BUFFER;
			yy_switch_to_buffer(yy_create_buffer(yyin,YY_BUF_SIZE));
		}
	}
}


/*
 * Open a file for reading or return NULL if it doesn't exist.  Any other error
 * is fatal.
 */

static FILE *openFile(char *name)
{
	FILE *fp;

	if ((fp = fopen(name,"r")) == NULL && errno != ENOENT)
		fatal("Error in opening file %s\n",name);

	return fp;
}


/*
 * Handle fatal yacc errors.
 */

void yyerror(char *s)
{
	if (currentFile < 0)
		fatal("Unexpected end of input\n");

	fatal("%s:%d: %s\n",
		inputFileStack[currentFile].name,
		inputFileStack[currentFile].lineno,
		s);
}


/*
 * Handle warnings while parsing.
 */

void yywarning(char *s)
{
	warning("%s:%d: %s\n",
		inputFileStack[currentFile].name,
		inputFileStack[currentFile].lineno,
		s);
}


/*
 * Handle fatal lex errors.
 */

static void fatallex(char *s)
{
	fatal("%s:%d: Lexical analyser error: %s\n",
		inputFileStack[currentFile].name,
		inputFileStack[currentFile].lineno,
		s);
}
