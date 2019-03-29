/* A Bison parser, made by GNU Bison 1.875d.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     TK_DOC = 258,
     TK_EXPORTEDDOC = 259,
     TK_MAKEFILE = 260,
     TK_ACCESSCODE = 261,
     TK_VARCODE = 262,
     TK_POSTINITCODE = 263,
     TK_CPPCODE = 264,
     TK_MODCODE = 265,
     TK_TYPECODE = 266,
     TK_PREPYCODE = 267,
     TK_PYCODE = 268,
     TK_COPYING = 269,
     TK_MAPPEDTYPE = 270,
     TK_CODEBLOCK = 271,
     TK_IF = 272,
     TK_END = 273,
     TK_NAME = 274,
     TK_PATHNAME = 275,
     TK_STRING = 276,
     TK_VIRTUALCATCHERCODE = 277,
     TK_VIRTUALCODE = 278,
     TK_METHODCODE = 279,
     TK_MEMBERCODE = 280,
     TK_FROMTYPE = 281,
     TK_TOTYPE = 282,
     TK_TOSUBCLASS = 283,
     TK_INCLUDE = 284,
     TK_OPTINCLUDE = 285,
     TK_IMPORT = 286,
     TK_HEADERCODE = 287,
     TK_MODHEADERCODE = 288,
     TK_TYPEHEADERCODE = 289,
     TK_EXPHEADERCODE = 290,
     TK_MODULE = 291,
     TK_CLASS = 292,
     TK_STRUCT = 293,
     TK_PUBLIC = 294,
     TK_PROTECTED = 295,
     TK_PRIVATE = 296,
     TK_SIGNALS = 297,
     TK_SLOTS = 298,
     TK_BOOL = 299,
     TK_SHORT = 300,
     TK_INT = 301,
     TK_LONG = 302,
     TK_FLOAT = 303,
     TK_DOUBLE = 304,
     TK_CHAR = 305,
     TK_VOID = 306,
     TK_PYOBJECT = 307,
     TK_PYTUPLE = 308,
     TK_PYLIST = 309,
     TK_PYDICT = 310,
     TK_PYCALLABLE = 311,
     TK_PYSLICE = 312,
     TK_VIRTUAL = 313,
     TK_ENUM = 314,
     TK_UNSIGNED = 315,
     TK_SCOPE = 316,
     TK_LOGICAL_OR = 317,
     TK_CONST = 318,
     TK_STATIC = 319,
     TK_SIPSIGNAL = 320,
     TK_SIPSLOT = 321,
     TK_SIPRXCON = 322,
     TK_SIPRXDIS = 323,
     TK_SIPSLOTCON = 324,
     TK_SIPSLOTDIS = 325,
     TK_NUMBER = 326,
     TK_REAL = 327,
     TK_TYPEDEF = 328,
     TK_NAMESPACE = 329,
     TK_TIMELINE = 330,
     TK_PLATFORMS = 331,
     TK_FEATURE = 332,
     TK_LICENSE = 333,
     TK_EXPOSE = 334,
     TK_QCHAR = 335,
     TK_TRUE = 336,
     TK_FALSE = 337,
     TK_OPERATOR = 338,
     TK_THROW = 339,
     TK_QOBJECT = 340
   };
#endif
#define TK_DOC 258
#define TK_EXPORTEDDOC 259
#define TK_MAKEFILE 260
#define TK_ACCESSCODE 261
#define TK_VARCODE 262
#define TK_POSTINITCODE 263
#define TK_CPPCODE 264
#define TK_MODCODE 265
#define TK_TYPECODE 266
#define TK_PREPYCODE 267
#define TK_PYCODE 268
#define TK_COPYING 269
#define TK_MAPPEDTYPE 270
#define TK_CODEBLOCK 271
#define TK_IF 272
#define TK_END 273
#define TK_NAME 274
#define TK_PATHNAME 275
#define TK_STRING 276
#define TK_VIRTUALCATCHERCODE 277
#define TK_VIRTUALCODE 278
#define TK_METHODCODE 279
#define TK_MEMBERCODE 280
#define TK_FROMTYPE 281
#define TK_TOTYPE 282
#define TK_TOSUBCLASS 283
#define TK_INCLUDE 284
#define TK_OPTINCLUDE 285
#define TK_IMPORT 286
#define TK_HEADERCODE 287
#define TK_MODHEADERCODE 288
#define TK_TYPEHEADERCODE 289
#define TK_EXPHEADERCODE 290
#define TK_MODULE 291
#define TK_CLASS 292
#define TK_STRUCT 293
#define TK_PUBLIC 294
#define TK_PROTECTED 295
#define TK_PRIVATE 296
#define TK_SIGNALS 297
#define TK_SLOTS 298
#define TK_BOOL 299
#define TK_SHORT 300
#define TK_INT 301
#define TK_LONG 302
#define TK_FLOAT 303
#define TK_DOUBLE 304
#define TK_CHAR 305
#define TK_VOID 306
#define TK_PYOBJECT 307
#define TK_PYTUPLE 308
#define TK_PYLIST 309
#define TK_PYDICT 310
#define TK_PYCALLABLE 311
#define TK_PYSLICE 312
#define TK_VIRTUAL 313
#define TK_ENUM 314
#define TK_UNSIGNED 315
#define TK_SCOPE 316
#define TK_LOGICAL_OR 317
#define TK_CONST 318
#define TK_STATIC 319
#define TK_SIPSIGNAL 320
#define TK_SIPSLOT 321
#define TK_SIPRXCON 322
#define TK_SIPRXDIS 323
#define TK_SIPSLOTCON 324
#define TK_SIPSLOTDIS 325
#define TK_NUMBER 326
#define TK_REAL 327
#define TK_TYPEDEF 328
#define TK_NAMESPACE 329
#define TK_TIMELINE 330
#define TK_PLATFORMS 331
#define TK_FEATURE 332
#define TK_LICENSE 333
#define TK_EXPOSE 334
#define TK_QCHAR 335
#define TK_TRUE 336
#define TK_FALSE 337
#define TK_OPERATOR 338
#define TK_THROW 339
#define TK_QOBJECT 340




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 97 "parser.y"
typedef union YYSTYPE {
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
} YYSTYPE;
/* Line 1285 of yacc.c.  */
#line 226 "parser.h"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;



