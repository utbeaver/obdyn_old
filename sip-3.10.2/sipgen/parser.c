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

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



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




/* Copy the first part of user declarations.  */
#line 16 "parser.y"

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


/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

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
/* Line 191 of yacc.c.  */
#line 346 "parser.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 214 of yacc.c.  */
#line 358 "parser.c"

#if ! defined (yyoverflow) || YYERROR_VERBOSE

# ifndef YYFREE
#  define YYFREE free
# endif
# ifndef YYMALLOC
#  define YYMALLOC malloc
# endif

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   define YYSTACK_ALLOC alloca
#  endif
# else
#  if defined (alloca) || defined (_ALLOCA_H)
#   define YYSTACK_ALLOC alloca
#  else
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (defined (YYSTYPE_IS_TRIVIAL) && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short int yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short int) + sizeof (YYSTYPE))			\
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined (__GNUC__) && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short int yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  4
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   697

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  108
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  110
/* YYNRULES -- Number of rules. */
#define YYNRULES  287
/* YYNRULES -- Number of states. */
#define YYNSTATES  453

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   340

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    91,     2,     2,     2,   103,   104,     2,
      89,    90,   101,    95,    93,    92,     2,   102,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    98,    88,
     106,    94,   107,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    99,     2,   100,   105,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    86,    96,    87,    97,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short int yyprhs[] =
{
       0,     0,     3,     5,     8,     9,    12,    14,    16,    18,
      20,    22,    24,    26,    28,    30,    32,    34,    36,    38,
      40,    42,    44,    46,    48,    50,    52,    54,    56,    58,
      60,    62,    64,    66,    68,    70,    72,    74,    75,    83,
      85,    88,    90,    93,    96,    97,   105,   107,   110,   111,
     117,   119,   122,   124,   127,   128,   134,   136,   139,   141,
     146,   148,   151,   155,   160,   162,   166,   168,   171,   174,
     178,   179,   181,   184,   187,   190,   191,   194,   197,   200,
     203,   206,   209,   212,   215,   218,   221,   224,   227,   230,
     233,   236,   239,   242,   247,   250,   251,   259,   260,   262,
     263,   265,   266,   268,   270,   273,   275,   277,   281,   282,
     284,   285,   288,   290,   294,   296,   298,   300,   301,   303,
     305,   307,   309,   312,   314,   318,   320,   322,   327,   329,
     331,   333,   335,   337,   339,   340,   342,   346,   351,   355,
     356,   365,   366,   376,   377,   380,   382,   386,   388,   390,
     393,   395,   397,   399,   401,   403,   405,   407,   409,   411,
     413,   415,   417,   419,   422,   425,   429,   433,   437,   440,
     441,   443,   455,   456,   458,   469,   470,   476,   477,   484,
     500,   514,   516,   518,   520,   522,   524,   526,   528,   530,
     533,   536,   539,   542,   545,   548,   551,   554,   557,   560,
     564,   568,   570,   573,   576,   578,   581,   584,   587,   589,
     592,   593,   595,   596,   599,   600,   604,   606,   610,   612,
     616,   618,   620,   622,   623,   626,   627,   630,   631,   634,
     635,   638,   640,   641,   643,   647,   649,   651,   653,   655,
     660,   665,   667,   670,   671,   675,   677,   679,   681,   682,
     686,   688,   693,   698,   702,   705,   706,   708,   709,   712,
     714,   719,   722,   725,   727,   729,   732,   734,   736,   739,
     741,   743,   745,   748,   750,   752,   754,   756,   758,   760,
     762,   764,   766,   770,   771,   776,   777,   779
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short int yyrhs[] =
{
     109,     0,    -1,   110,    -1,   109,   110,    -1,    -1,   111,
     112,    -1,   136,    -1,   142,    -1,   138,    -1,   139,    -1,
     140,    -1,   126,    -1,   121,    -1,   125,    -1,   134,    -1,
     135,    -1,   143,    -1,   145,    -1,   146,    -1,   148,    -1,
     149,    -1,   150,    -1,   151,    -1,   152,    -1,   153,    -1,
     114,    -1,   113,    -1,   130,    -1,   133,    -1,   118,    -1,
     174,    -1,   176,    -1,   173,    -1,   172,    -1,   155,    -1,
     189,    -1,   209,    -1,    -1,    15,   214,   115,    86,   116,
      87,    88,    -1,   117,    -1,   116,   117,    -1,   144,    -1,
      26,   154,    -1,    27,   154,    -1,    -1,    74,    19,   119,
      86,   120,    87,    88,    -1,   113,    -1,   120,   113,    -1,
      -1,    76,   122,    86,   123,    87,    -1,   124,    -1,   123,
     124,    -1,    19,    -1,    77,    19,    -1,    -1,    75,   127,
      86,   128,    87,    -1,   129,    -1,   128,   129,    -1,    19,
      -1,    17,    89,   132,    90,    -1,    19,    -1,    91,    19,
      -1,   131,    62,    19,    -1,   131,    62,    91,    19,    -1,
     131,    -1,   158,    92,   158,    -1,    18,    -1,    78,   193,
      -1,    79,    19,    -1,    36,    19,   137,    -1,    -1,    71,
      -1,    29,    20,    -1,    30,    20,    -1,    31,    20,    -1,
      -1,     6,   154,    -1,     7,   154,    -1,    14,   154,    -1,
      33,   154,    -1,    32,   154,    -1,    34,   154,    -1,    32,
     154,    -1,    35,   154,    -1,    10,   154,    -1,     9,   154,
      -1,    11,   154,    -1,     9,   154,    -1,     8,   154,    -1,
      12,   154,    -1,    13,   154,    -1,     3,   154,    -1,     4,
     154,    -1,     5,    20,   157,   154,    -1,    16,    18,    -1,
      -1,    59,   158,   156,    86,   159,    87,    88,    -1,    -1,
      20,    -1,    -1,    19,    -1,    -1,   160,    -1,   161,    -1,
     160,   161,    -1,   130,    -1,   133,    -1,    19,   163,   162,
      -1,    -1,    93,    -1,    -1,    94,   164,    -1,   167,    -1,
     164,   165,   167,    -1,    92,    -1,    95,    -1,    96,    -1,
      -1,    91,    -1,    97,    -1,    92,    -1,    95,    -1,   166,
     170,    -1,   169,    -1,   168,    61,   169,    -1,    19,    -1,
     168,    -1,   168,    89,   171,    90,    -1,    72,    -1,    71,
      -1,    81,    -1,    82,    -1,    21,    -1,    80,    -1,    -1,
     167,    -1,   171,    93,   167,    -1,    73,   210,    19,    88,
      -1,    37,   168,    88,    -1,    -1,    38,    19,   175,   193,
      86,   181,    87,    88,    -1,    -1,    37,    19,   177,   178,
     193,    86,   181,    87,    88,    -1,    -1,    98,   179,    -1,
     180,    -1,   179,    93,   180,    -1,   168,    -1,   182,    -1,
     181,   182,    -1,   130,    -1,   133,    -1,   118,    -1,   174,
      -1,   176,    -1,   173,    -1,   172,    -1,   155,    -1,   147,
      -1,   144,    -1,   186,    -1,   184,    -1,   204,    -1,    28,
     154,    -1,    27,   154,    -1,    39,   183,    98,    -1,    40,
     183,    98,    -1,    41,   183,    98,    -1,    42,    98,    -1,
      -1,    43,    -1,   185,    97,    19,    89,    90,   216,    88,
     197,   198,   199,   200,    -1,    -1,    58,    -1,    19,    89,
     201,    90,   216,   193,   187,    88,   197,   198,    -1,    -1,
      99,    89,   201,    90,   100,    -1,    -1,    99,   210,    89,
     201,    90,   100,    -1,   210,    19,    89,   201,    90,   191,
     216,   192,   193,   188,    88,   197,   198,   199,   200,    -1,
     210,    83,   190,    89,   201,    90,   191,   216,   193,   188,
      88,   197,   198,    -1,    95,    -1,    92,    -1,   101,    -1,
     102,    -1,   103,    -1,   104,    -1,    96,    -1,   105,    -1,
     106,   106,    -1,   107,   107,    -1,    95,    94,    -1,    92,
      94,    -1,   101,    94,    -1,   102,    94,    -1,   103,    94,
      -1,   104,    94,    -1,    96,    94,    -1,   105,    94,    -1,
     106,   106,    94,    -1,   107,   107,    94,    -1,    97,    -1,
      89,    90,    -1,    99,   100,    -1,   106,    -1,   106,    94,
      -1,    94,    94,    -1,    91,    94,    -1,   107,    -1,   107,
      94,    -1,    -1,    63,    -1,    -1,    94,    71,    -1,    -1,
     102,   194,   102,    -1,   195,    -1,   194,    93,   195,    -1,
      19,    -1,    19,    94,   196,    -1,    19,    -1,    21,    -1,
      71,    -1,    -1,    24,   154,    -1,    -1,    25,   154,    -1,
      -1,    22,   154,    -1,    -1,    23,   154,    -1,   202,    -1,
      -1,   203,    -1,   202,    93,   203,    -1,    65,    -1,    66,
      -1,    67,    -1,    68,    -1,    69,    89,   201,    90,    -1,
      70,    89,   201,    90,    -1,    85,    -1,   211,   163,    -1,
      -1,    64,   205,   206,    -1,   206,    -1,   207,    -1,   209,
      -1,    -1,    58,   208,   189,    -1,   189,    -1,   210,    19,
      88,   141,    -1,    63,   214,   213,   212,    -1,   214,   213,
     212,    -1,   210,   193,    -1,    -1,   104,    -1,    -1,   213,
     101,    -1,   168,    -1,   168,   106,   215,   107,    -1,    38,
     168,    -1,    60,    45,    -1,    45,    -1,    60,    -1,    60,
      46,    -1,    46,    -1,    47,    -1,    60,    47,    -1,    48,
      -1,    49,    -1,    44,    -1,    60,    50,    -1,    50,    -1,
      51,    -1,    52,    -1,    53,    -1,    54,    -1,    55,    -1,
      56,    -1,    57,    -1,   210,    -1,   215,    93,   210,    -1,
      -1,    84,    89,   217,    90,    -1,    -1,   168,    -1,   217,
      93,   168,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
{
       0,   249,   249,   250,   253,   253,   272,   273,   274,   275,
     276,   277,   278,   279,   280,   281,   282,   286,   287,   291,
     292,   293,   294,   295,   296,   297,   298,   301,   302,   303,
     304,   305,   306,   307,   308,   309,   310,   313,   313,   336,
     337,   340,   344,   353,   364,   364,   385,   386,   389,   389,
     416,   417,   420,   425,   430,   430,   456,   457,   460,   465,
     480,   483,   486,   489,   494,   495,   500,   506,   533,   556,
     569,   572,   575,   580,   585,   590,   593,   596,   602,   608,
     611,   617,   620,   626,   633,   636,   642,   645,   651,   657,
     665,   673,   679,   684,   708,   711,   711,   722,   725,   730,
     733,   738,   739,   742,   743,   746,   747,   748,   775,   776,
     779,   782,   787,   788,   806,   809,   812,   817,   820,   823,
     826,   829,   834,   852,   853,   858,   863,   873,   883,   887,
     891,   895,   899,   903,   909,   914,   920,   938,   944,   958,
     958,   981,   981,  1004,  1005,  1008,  1009,  1012,  1027,  1028,
    1031,  1032,  1033,  1034,  1035,  1036,  1037,  1038,  1039,  1043,
    1047,  1048,  1049,  1050,  1061,  1072,  1076,  1080,  1084,  1090,
    1093,  1098,  1136,  1139,  1144,  1157,  1160,  1167,  1170,  1178,
    1193,  1205,  1206,  1207,  1208,  1209,  1210,  1211,  1212,  1213,
    1214,  1215,  1216,  1217,  1218,  1219,  1220,  1221,  1222,  1223,
    1224,  1225,  1226,  1227,  1228,  1229,  1230,  1231,  1232,  1233,
    1236,  1239,  1244,  1247,  1255,  1258,  1264,  1268,  1280,  1284,
    1290,  1294,  1298,  1304,  1307,  1312,  1315,  1321,  1324,  1329,
    1332,  1338,  1386,  1391,  1397,  1427,  1434,  1441,  1448,  1455,
    1465,  1475,  1480,  1486,  1486,  1487,  1490,  1491,  1494,  1494,
    1495,  1498,  1519,  1524,  1531,  1584,  1587,  1592,  1595,  1600,
    1604,  1614,  1618,  1621,  1624,  1627,  1630,  1633,  1636,  1639,
    1642,  1645,  1648,  1651,  1654,  1657,  1660,  1663,  1666,  1669,
    1672,  1677,  1683,  1701,  1704,  1719,  1725,  1732
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "TK_DOC", "TK_EXPORTEDDOC",
  "TK_MAKEFILE", "TK_ACCESSCODE", "TK_VARCODE", "TK_POSTINITCODE",
  "TK_CPPCODE", "TK_MODCODE", "TK_TYPECODE", "TK_PREPYCODE", "TK_PYCODE",
  "TK_COPYING", "TK_MAPPEDTYPE", "TK_CODEBLOCK", "TK_IF", "TK_END",
  "TK_NAME", "TK_PATHNAME", "TK_STRING", "TK_VIRTUALCATCHERCODE",
  "TK_VIRTUALCODE", "TK_METHODCODE", "TK_MEMBERCODE", "TK_FROMTYPE",
  "TK_TOTYPE", "TK_TOSUBCLASS", "TK_INCLUDE", "TK_OPTINCLUDE", "TK_IMPORT",
  "TK_HEADERCODE", "TK_MODHEADERCODE", "TK_TYPEHEADERCODE",
  "TK_EXPHEADERCODE", "TK_MODULE", "TK_CLASS", "TK_STRUCT", "TK_PUBLIC",
  "TK_PROTECTED", "TK_PRIVATE", "TK_SIGNALS", "TK_SLOTS", "TK_BOOL",
  "TK_SHORT", "TK_INT", "TK_LONG", "TK_FLOAT", "TK_DOUBLE", "TK_CHAR",
  "TK_VOID", "TK_PYOBJECT", "TK_PYTUPLE", "TK_PYLIST", "TK_PYDICT",
  "TK_PYCALLABLE", "TK_PYSLICE", "TK_VIRTUAL", "TK_ENUM", "TK_UNSIGNED",
  "TK_SCOPE", "TK_LOGICAL_OR", "TK_CONST", "TK_STATIC", "TK_SIPSIGNAL",
  "TK_SIPSLOT", "TK_SIPRXCON", "TK_SIPRXDIS", "TK_SIPSLOTCON",
  "TK_SIPSLOTDIS", "TK_NUMBER", "TK_REAL", "TK_TYPEDEF", "TK_NAMESPACE",
  "TK_TIMELINE", "TK_PLATFORMS", "TK_FEATURE", "TK_LICENSE", "TK_EXPOSE",
  "TK_QCHAR", "TK_TRUE", "TK_FALSE", "TK_OPERATOR", "TK_THROW",
  "TK_QOBJECT", "'{'", "'}'", "';'", "'('", "')'", "'!'", "'-'", "','",
  "'='", "'+'", "'|'", "'~'", "':'", "'['", "']'", "'*'", "'/'", "'%'",
  "'&'", "'^'", "'<'", "'>'", "$accept", "specification", "statement",
  "@1", "modstatement", "nsstatement", "mappedtype", "@2", "mtbody",
  "mtline", "namespace", "@3", "nsbody", "platforms", "@4", "platformlist",
  "platform", "feature", "timeline", "@5", "qualifierlist",
  "qualifiername", "ifstart", "oredqualifiers", "qualifiers", "ifend",
  "license", "expose", "module", "optnumber", "include", "optinclude",
  "import", "optaccesscode", "copying", "modhdrcode", "typehdrcode",
  "exphdrcode", "modcode", "typecode", "postinitcode", "prepycode",
  "pycode", "doc", "exporteddoc", "makefile", "codeblock", "enum", "@6",
  "optfilename", "optname", "optenumbody", "enumbody", "enumline",
  "optcomma", "optassign", "expr", "binop", "optunop", "value",
  "scopedname", "scopepart", "simplevalue", "valuelist", "typedef",
  "opaqueclass", "struct", "@7", "class", "@8", "superclasses",
  "superlist", "superclass", "classbody", "classline", "optslot", "dtor",
  "optvirtual", "ctor", "optctorsig", "optsig", "function", "operatorname",
  "optconst", "optabstract", "optflags", "flaglist", "flag", "flagvalue",
  "methodcode", "membercode", "virtualcatchercode", "virtualcode",
  "arglist", "rawarglist", "argvalue", "varmember", "@9", "varmem",
  "member", "@10", "variable", "cpptype", "argtype", "optref", "deref",
  "basetype", "cpptypelist", "optexceptions", "exceptionlist", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short int yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   123,   125,    59,    40,
      41,    33,    45,    44,    61,    43,   124,   126,    58,    91,
      93,    42,    47,    37,    38,    94,    60,    62
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,   108,   109,   109,   111,   110,   112,   112,   112,   112,
     112,   112,   112,   112,   112,   112,   112,   112,   112,   112,
     112,   112,   112,   112,   112,   112,   112,   113,   113,   113,
     113,   113,   113,   113,   113,   113,   113,   115,   114,   116,
     116,   117,   117,   117,   119,   118,   120,   120,   122,   121,
     123,   123,   124,   125,   127,   126,   128,   128,   129,   130,
     131,   131,   131,   131,   132,   132,   133,   134,   135,   136,
     137,   137,   138,   139,   140,   141,   141,   141,   142,   143,
     143,   144,   144,   145,   146,   146,   147,   147,   148,   149,
     150,   151,   152,   153,   154,   156,   155,   157,   157,   158,
     158,   159,   159,   160,   160,   161,   161,   161,   162,   162,
     163,   163,   164,   164,   165,   165,   165,   166,   166,   166,
     166,   166,   167,   168,   168,   169,   170,   170,   170,   170,
     170,   170,   170,   170,   171,   171,   171,   172,   173,   175,
     174,   177,   176,   178,   178,   179,   179,   180,   181,   181,
     182,   182,   182,   182,   182,   182,   182,   182,   182,   182,
     182,   182,   182,   182,   182,   182,   182,   182,   182,   183,
     183,   184,   185,   185,   186,   187,   187,   188,   188,   189,
     189,   190,   190,   190,   190,   190,   190,   190,   190,   190,
     190,   190,   190,   190,   190,   190,   190,   190,   190,   190,
     190,   190,   190,   190,   190,   190,   190,   190,   190,   190,
     191,   191,   192,   192,   193,   193,   194,   194,   195,   195,
     196,   196,   196,   197,   197,   198,   198,   199,   199,   200,
     200,   201,   202,   202,   202,   203,   203,   203,   203,   203,
     203,   203,   203,   205,   204,   204,   206,   206,   208,   207,
     207,   209,   210,   210,   211,   212,   212,   213,   213,   214,
     214,   214,   214,   214,   214,   214,   214,   214,   214,   214,
     214,   214,   214,   214,   214,   214,   214,   214,   214,   214,
     214,   215,   215,   216,   216,   217,   217,   217
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     1,     2,     0,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     0,     7,     1,
       2,     1,     2,     2,     0,     7,     1,     2,     0,     5,
       1,     2,     1,     2,     0,     5,     1,     2,     1,     4,
       1,     2,     3,     4,     1,     3,     1,     2,     2,     3,
       0,     1,     2,     2,     2,     0,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     4,     2,     0,     7,     0,     1,     0,
       1,     0,     1,     1,     2,     1,     1,     3,     0,     1,
       0,     2,     1,     3,     1,     1,     1,     0,     1,     1,
       1,     1,     2,     1,     3,     1,     1,     4,     1,     1,
       1,     1,     1,     1,     0,     1,     3,     4,     3,     0,
       8,     0,     9,     0,     2,     1,     3,     1,     1,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     2,     3,     3,     3,     2,     0,
       1,    11,     0,     1,    10,     0,     5,     0,     6,    15,
      13,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     3,
       3,     1,     2,     2,     1,     2,     2,     2,     1,     2,
       0,     1,     0,     2,     0,     3,     1,     3,     1,     3,
       1,     1,     1,     0,     2,     0,     2,     0,     2,     0,
       2,     1,     0,     1,     3,     1,     1,     1,     1,     4,
       4,     1,     2,     0,     3,     1,     1,     1,     0,     3,
       1,     4,     4,     3,     2,     0,     1,     0,     2,     1,
       4,     2,     2,     1,     1,     2,     1,     1,     2,     1,
       1,     1,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     0,     4,     0,     1,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned short int yydefact[] =
{
       4,     4,     2,     0,     1,     3,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    66,   125,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   271,   263,
     266,   267,   269,   270,   273,   274,   275,   276,   277,   278,
     279,   280,    99,   264,     0,     0,     0,    54,    48,     0,
     214,     0,     5,    26,    25,    29,    12,    13,    11,    27,
      28,    14,    15,     6,     8,     9,    10,     7,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    34,   259,   123,
      33,    32,    30,    31,    35,    36,     0,   257,     0,    91,
      92,    97,    88,    85,    84,    89,    90,    78,     0,    37,
      99,    72,    73,    74,    80,    79,    83,    70,   141,     0,
     125,   261,   100,    95,   262,   265,   268,   272,   257,     0,
      44,     0,     0,    53,     0,    67,    68,     0,     0,     0,
       0,   255,    94,    98,     0,     0,    60,     0,    64,     0,
       0,    71,    69,   143,   138,   214,     0,   255,     0,     0,
       0,     0,   218,     0,   216,   124,   281,     0,    75,   232,
       0,     0,   182,     0,   181,   187,   201,     0,   183,   184,
     185,   186,   188,   204,   208,     0,   258,   256,   253,    93,
       0,    61,     0,    59,    99,     0,   214,     0,   101,   252,
     137,     0,    58,     0,    56,    52,     0,    50,     0,     0,
     215,     0,   260,     0,     0,   251,   235,   236,   237,   238,
       0,     0,   241,     0,   231,   233,   214,   110,   202,   207,
     192,   206,   191,   197,   203,   193,   194,   195,   196,   198,
     205,   189,   209,   190,   232,     0,     0,     0,     0,     0,
      39,    41,    62,     0,    65,   147,   144,   145,     0,   172,
     110,   105,   106,     0,   102,   103,    46,     0,    55,    57,
      49,    51,   220,   221,   222,   219,   217,   282,    76,    77,
     232,   232,   210,     0,   254,   117,   242,   199,   200,     0,
      42,    43,    82,    81,     0,    40,    63,     0,   172,     0,
       0,   125,     0,     0,   169,   169,   169,     0,   248,   243,
     152,   150,   151,   159,   158,   157,   156,   155,   153,   154,
     172,   148,   161,     0,   160,   250,   162,   245,   246,   247,
     108,     0,   104,     0,    47,     0,     0,   211,   283,   234,
     118,   120,   121,   119,   111,     0,   112,   210,    38,   146,
     172,    87,    86,   232,   164,   163,   170,     0,     0,     0,
     168,     0,     0,     0,   149,     0,   109,   107,    96,    45,
     239,   240,     0,   212,   114,   115,   116,   117,   132,   129,
     128,   133,   130,   131,   126,   122,   283,     0,     0,   165,
     166,   167,   249,     0,   248,   244,   140,     0,   285,     0,
     214,   113,   117,   214,   142,   283,     0,     0,   286,     0,
     213,   177,   135,     0,   177,   214,   283,   284,     0,     0,
       0,   127,   117,     0,   175,     0,   287,     0,   223,   136,
     223,     0,     0,   223,   232,     0,   225,   225,   232,   223,
     225,     0,   224,     0,   227,   180,     0,   225,   227,     0,
     226,     0,   229,     0,   174,   229,   178,   228,     0,   179,
     176,   171,   230
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short int yydefgoto[] =
{
      -1,     1,     2,     3,    52,    53,    54,   135,   239,   240,
     300,   149,   257,    56,   122,   196,   197,    57,    58,   121,
     193,   194,   301,   138,   139,   302,    61,    62,    63,   142,
      64,    65,    66,   205,    67,    68,   303,    69,    70,   304,
      71,    72,    73,    74,    75,    76,    89,   305,   146,   134,
     113,   253,   254,   255,   357,   276,   334,   367,   335,   336,
      78,    79,   375,   403,   306,   307,   308,   145,   309,   143,
     186,   246,   247,   310,   311,   347,   312,   313,   314,   422,
     410,   315,   175,   328,   390,   125,   153,   154,   265,   426,
     434,   442,   449,   213,   214,   215,   316,   352,   317,   318,
     351,   319,    86,   217,   178,   131,    87,   157,   363,   399
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -394
static const short int yypact[] =
{
    -394,    41,  -394,   287,  -394,  -394,    65,    65,    25,    65,
      65,    65,    65,    65,    65,   620,    48,  -394,  -394,    98,
     110,   141,    65,    65,    65,    66,    95,   153,  -394,  -394,
    -394,  -394,  -394,  -394,  -394,  -394,  -394,  -394,  -394,  -394,
    -394,  -394,   164,   130,   620,   600,   171,  -394,  -394,   173,
     105,   204,  -394,  -394,  -394,  -394,  -394,  -394,  -394,  -394,
    -394,  -394,  -394,  -394,  -394,  -394,  -394,  -394,  -394,  -394,
    -394,  -394,  -394,  -394,  -394,  -394,  -394,  -394,   -23,  -394,
    -394,  -394,  -394,  -394,  -394,  -394,    11,  -394,   206,  -394,
    -394,   212,  -394,  -394,  -394,  -394,  -394,  -394,   214,  -394,
       0,  -394,  -394,  -394,  -394,  -394,  -394,   165,    -8,    68,
     -47,   181,  -394,  -394,  -394,  -394,  -394,  -394,  -394,   225,
    -394,   160,   161,  -394,   229,  -394,  -394,   214,   600,    36,
     590,   -61,  -394,  -394,    65,   163,   158,   232,   190,   167,
     162,  -394,  -394,   166,  -394,   105,   172,   -61,   174,   175,
     240,   241,   169,    80,  -394,  -394,  -394,   -17,   188,   451,
     176,   187,   193,   194,   195,   199,  -394,   170,   200,   209,
     215,   216,   217,     2,    13,   178,  -394,  -394,  -394,  -394,
     152,  -394,     9,  -394,   164,   214,   105,   198,   203,  -394,
    -394,   561,  -394,    10,  -394,  -394,    29,  -394,    35,   229,
    -394,   600,  -394,    65,    65,  -394,  -394,  -394,  -394,  -394,
     219,   223,  -394,   208,   220,  -394,   105,   233,  -394,  -394,
    -394,  -394,  -394,  -394,  -394,  -394,  -394,  -394,  -394,  -394,
    -394,   234,  -394,   235,   451,    65,    65,    65,    65,    83,
    -394,  -394,  -394,   295,  -394,   181,   237,  -394,   259,   513,
     233,  -394,  -394,   239,   203,  -394,  -394,    14,  -394,  -394,
    -394,  -394,  -394,  -394,  -394,  -394,  -394,  -394,  -394,  -394,
     451,   451,   285,   451,  -394,    96,  -394,  -394,  -394,   261,
    -394,  -394,  -394,  -394,   264,  -394,  -394,   214,   513,    65,
      65,   265,    65,    65,   310,   310,   310,   257,   260,  -394,
    -394,  -394,  -394,  -394,  -394,  -394,  -394,  -394,  -394,  -394,
      94,  -394,  -394,   262,  -394,  -394,  -394,  -394,  -394,  -394,
     263,   270,  -394,   279,  -394,   278,   280,  -394,   288,  -394,
    -394,  -394,  -394,  -394,   123,   145,  -394,   285,  -394,  -394,
     405,  -394,  -394,   451,  -394,  -394,  -394,   271,   277,   281,
    -394,   600,   352,   289,  -394,   357,  -394,  -394,  -394,  -394,
    -394,  -394,   291,   284,  -394,  -394,  -394,    96,  -394,  -394,
    -394,  -394,  -394,  -394,   -11,  -394,   288,   293,   292,  -394,
    -394,  -394,  -394,    16,  -394,  -394,  -394,   294,   214,   313,
     105,  -394,   148,   105,  -394,   288,   296,   297,   181,    -4,
    -394,   290,  -394,    69,   290,   105,   288,  -394,   214,   600,
     298,  -394,    96,   303,   312,   305,   181,   306,   389,  -394,
     389,   328,   331,   389,   451,    65,   395,   395,   451,   389,
     395,   335,  -394,    65,   399,  -394,   337,   395,   399,   329,
    -394,    65,   407,   336,  -394,   407,  -394,  -394,    65,  -394,
    -394,  -394,  -394
};

/* YYPGOTO[NTERM-NUM].  */
static const short int yypgoto[] =
{
    -394,  -394,   427,  -394,  -394,  -164,  -394,  -394,  -394,   192,
       8,  -394,  -394,  -394,  -394,  -394,   242,  -394,  -394,  -394,
    -394,   247,    15,  -394,  -394,    17,  -394,  -394,  -394,  -394,
    -394,  -394,  -394,  -394,  -394,  -394,  -144,  -394,  -394,  -394,
    -394,  -394,  -394,  -394,  -394,  -394,    -7,    18,  -394,  -394,
     -86,  -394,  -394,   213,  -394,   185,  -394,  -394,  -394,  -310,
     -14,   321,  -394,  -394,    19,    20,    21,  -394,    22,  -394,
    -394,  -394,   179,   183,  -268,   -94,  -394,  -394,  -394,  -394,
      70,    -2,  -394,   135,  -394,  -137,  -394,   274,  -394,  -260,
    -393,    37,    31,  -224,  -394,   207,  -394,  -394,   125,  -394,
    -394,    23,   -36,  -394,   334,   364,    60,  -394,  -221,  -394
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -174
static const short int yytable[] =
{
      90,    84,    92,    93,    94,    95,    96,    97,   187,   119,
     279,    55,   109,   111,   140,   104,   105,   106,    59,   136,
      60,    77,    80,    81,    82,    83,    85,   256,   242,   192,
     129,    16,    17,    18,   435,   396,   241,   438,   127,  -139,
     176,     4,   354,   177,   444,    91,   325,   326,   195,   248,
     127,    26,    27,  -125,   262,  -139,   263,   391,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,   354,    42,    43,    99,   201,    44,   392,   274,
    -125,    88,   402,   128,   111,   107,   407,    45,    46,   408,
     202,   137,   156,   324,   130,   241,   230,   258,   244,   130,
     243,   323,   419,   289,   118,   290,   264,   232,   231,   235,
     236,    16,    17,   291,   108,   237,   260,   238,   101,   378,
     233,   292,   293,   216,   158,   159,   237,   179,   238,   127,
     102,    26,    27,   294,   295,   296,   297,   100,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,   298,    42,    43,   393,   144,    44,   299,   411,
     427,   103,   412,   430,    18,   267,   368,    45,    46,   437,
     284,   245,   110,   199,   405,   114,   115,   116,   235,   236,
     117,   353,   200,   112,   237,   415,   238,   330,   331,    84,
     120,   332,   123,   333,   203,   204,   268,   269,   216,    55,
     431,   348,   349,   251,   436,   252,    59,   124,    60,    77,
      80,    81,    82,    83,    85,   364,   369,   370,   365,   366,
      16,    17,   250,   126,   132,   371,   372,   373,   280,   281,
     282,   283,   133,    18,   216,   216,   141,   216,  -134,   330,
     331,  -134,   127,   332,   148,   333,   150,   151,   152,   180,
    -100,   181,   182,   401,   184,    84,   404,   183,   188,   192,
     195,   191,   190,   198,   185,    55,   218,   234,   414,   251,
     224,   252,    59,   245,    60,    77,    80,    81,    82,    83,
      85,   219,   341,   342,   249,   344,   345,   220,   221,   222,
       6,     7,     8,   223,   225,     9,    10,    11,   272,    12,
      13,    14,    15,   226,    16,    17,    18,   216,   270,   227,
     228,   229,   271,   273,   286,   383,    19,    20,    21,    22,
      23,   374,    24,    25,    26,    27,   321,   275,   277,   278,
     287,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,   288,    42,    43,   327,   382,
      44,   337,   338,   346,   343,   350,   356,  -173,   358,   355,
      45,    46,    47,    48,    49,    50,    51,   359,   360,   379,
     361,    18,   362,   417,   398,   380,   387,   386,   389,   381,
     388,   394,   395,   397,   400,   159,   418,   406,   216,   409,
      98,   420,   216,   423,   416,   424,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
     384,   421,    43,   425,   289,    44,   290,   428,   432,   429,
     433,   441,    16,    17,   291,   439,   440,   443,     5,   446,
     448,   285,   292,   293,   447,   320,   450,   237,   261,   238,
     259,   452,    26,    27,   294,   295,   296,   297,   155,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,   298,    42,    43,   339,   322,    44,   299,
      18,   340,   376,   266,   413,   445,   451,   385,    45,    46,
     329,   189,   147,     0,     0,     0,     0,     0,     0,    98,
       0,     0,   377,     0,     0,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,     0,
       0,    43,     0,     0,    44,     0,   206,   207,   208,   209,
     210,   211,   289,     0,   290,     0,     0,     0,     0,     0,
      16,    17,   291,     0,     0,     0,   212,     0,     0,     0,
     292,   293,     0,     0,     0,   237,     0,   238,     0,     0,
      26,    27,   294,   295,   296,   297,     0,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,   298,    42,    43,     0,     0,    44,   299,    16,    17,
      18,     0,     0,     0,     0,     0,    45,    46,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    26,    27,
       0,     0,     0,     0,     0,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    18,
      42,    43,     0,     0,    44,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    45,    46,     0,     0,    98,    18,
       0,     0,     0,     0,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    98,     0,
      43,     0,     0,    44,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,     0,   160,
      43,   161,   162,     0,   163,   164,   165,   166,     0,   167,
       0,   168,   169,   170,   171,   172,   173,   174
};

static const short int yycheck[] =
{
       7,     3,     9,    10,    11,    12,    13,    14,   145,    45,
     234,     3,    26,    27,   100,    22,    23,    24,     3,    19,
       3,     3,     3,     3,     3,     3,     3,   191,    19,    19,
      19,    17,    18,    19,   427,    19,   180,   430,    61,    86,
     101,     0,   310,   104,   437,    20,   270,   271,    19,   186,
      61,    37,    38,    61,    19,   102,    21,   367,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,   340,    59,    60,    15,    93,    63,    89,   216,
      88,    16,   392,   106,    98,    19,    90,    73,    74,    93,
     107,    91,   128,   257,    83,   239,    94,    87,   184,    83,
      91,    87,   412,     9,    44,    11,    71,    94,   106,    26,
      27,    17,    18,    19,    19,    32,    87,    34,    20,   343,
     107,    27,    28,   159,    88,    89,    32,   134,    34,    61,
      20,    37,    38,    39,    40,    41,    42,    89,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,   376,    88,    63,    64,    90,
     420,    20,    93,   423,    19,   201,    21,    73,    74,   429,
      87,   185,    19,    93,   395,    45,    46,    47,    26,    27,
      50,    87,   102,    19,    32,   406,    34,    91,    92,   191,
      19,    95,    19,    97,     6,     7,   203,   204,   234,   191,
     424,   295,   296,   188,   428,   188,   191,   102,   191,   191,
     191,   191,   191,   191,   191,    92,    71,    72,    95,    96,
      17,    18,    19,    19,    18,    80,    81,    82,   235,   236,
     237,   238,    20,    19,   270,   271,    71,   273,    90,    91,
      92,    93,    61,    95,    19,    97,    86,    86,    19,    86,
      92,    19,    62,   390,    92,   257,   393,    90,    86,    19,
      19,    86,    88,    94,    98,   257,    90,    89,   405,   254,
     100,   254,   257,   287,   257,   257,   257,   257,   257,   257,
     257,    94,   289,   290,    86,   292,   293,    94,    94,    94,
       3,     4,     5,    94,    94,     8,     9,    10,    90,    12,
      13,    14,    15,    94,    17,    18,    19,   343,    89,    94,
      94,    94,    89,    93,    19,   351,    29,    30,    31,    32,
      33,   335,    35,    36,    37,    38,    87,    94,    94,    94,
      93,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    86,    59,    60,    63,   351,
      63,    90,    88,    43,    89,    98,    93,    97,    88,    97,
      73,    74,    75,    76,    77,    78,    79,    88,    90,    98,
      90,    19,    84,   409,   388,    98,    19,    88,    94,    98,
      89,    88,    90,    89,    71,    89,    88,    90,   424,    99,
      38,    88,   428,    88,   408,    89,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    99,    60,    24,     9,    63,    11,    89,   425,    88,
      25,    22,    17,    18,    19,    90,   433,    90,     1,   100,
      23,   239,    27,    28,   441,   250,   100,    32,   196,    34,
     193,   448,    37,    38,    39,    40,    41,    42,   127,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,   287,   254,    63,    64,
      19,   288,   337,   199,   404,   438,   445,   352,    73,    74,
     273,   147,   118,    -1,    -1,    -1,    -1,    -1,    -1,    38,
      -1,    -1,    87,    -1,    -1,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    -1,
      -1,    60,    -1,    -1,    63,    -1,    65,    66,    67,    68,
      69,    70,     9,    -1,    11,    -1,    -1,    -1,    -1,    -1,
      17,    18,    19,    -1,    -1,    -1,    85,    -1,    -1,    -1,
      27,    28,    -1,    -1,    -1,    32,    -1,    34,    -1,    -1,
      37,    38,    39,    40,    41,    42,    -1,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    -1,    -1,    63,    64,    17,    18,
      19,    -1,    -1,    -1,    -1,    -1,    73,    74,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    37,    38,
      -1,    -1,    -1,    -1,    -1,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    19,
      59,    60,    -1,    -1,    63,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    73,    74,    -1,    -1,    38,    19,
      -1,    -1,    -1,    -1,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    38,    -1,
      60,    -1,    -1,    63,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    -1,    89,
      60,    91,    92,    -1,    94,    95,    96,    97,    -1,    99,
      -1,   101,   102,   103,   104,   105,   106,   107
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,   109,   110,   111,     0,   110,     3,     4,     5,     8,
       9,    10,    12,    13,    14,    15,    17,    18,    19,    29,
      30,    31,    32,    33,    35,    36,    37,    38,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    59,    60,    63,    73,    74,    75,    76,    77,
      78,    79,   112,   113,   114,   118,   121,   125,   126,   130,
     133,   134,   135,   136,   138,   139,   140,   142,   143,   145,
     146,   148,   149,   150,   151,   152,   153,   155,   168,   169,
     172,   173,   174,   176,   189,   209,   210,   214,    16,   154,
     154,    20,   154,   154,   154,   154,   154,   154,    38,   214,
      89,    20,    20,    20,   154,   154,   154,    19,    19,   168,
      19,   168,    19,   158,    45,    46,    47,    50,   214,   210,
      19,   127,   122,    19,   102,   193,    19,    61,   106,    19,
      83,   213,    18,    20,   157,   115,    19,    91,   131,   132,
     158,    71,   137,   177,    88,   175,   156,   213,    19,   119,
      86,    86,    19,   194,   195,   169,   210,   215,    88,    89,
      89,    91,    92,    94,    95,    96,    97,    99,   101,   102,
     103,   104,   105,   106,   107,   190,   101,   104,   212,   154,
      86,    19,    62,    90,    92,    98,   178,   193,    86,   212,
      88,    86,    19,   128,   129,    19,   123,   124,    94,    93,
     102,    93,   107,     6,     7,   141,    65,    66,    67,    68,
      69,    70,    85,   201,   202,   203,   210,   211,    90,    94,
      94,    94,    94,    94,   100,    94,    94,    94,    94,    94,
      94,   106,    94,   107,    89,    26,    27,    32,    34,   116,
     117,   144,    19,    91,   158,   168,   179,   180,   193,    86,
      19,   130,   133,   159,   160,   161,   113,   120,    87,   129,
      87,   124,    19,    21,    71,   196,   195,   210,   154,   154,
      89,    89,    90,    93,   193,    94,   163,    94,    94,   201,
     154,   154,   154,   154,    87,   117,    19,    93,    86,     9,
      11,    19,    27,    28,    39,    40,    41,    42,    58,    64,
     118,   130,   133,   144,   147,   155,   172,   173,   174,   176,
     181,   182,   184,   185,   186,   189,   204,   206,   207,   209,
     163,    87,   161,    87,   113,   201,   201,    63,   191,   203,
      91,    92,    95,    97,   164,   166,   167,    90,    88,   180,
     181,   154,   154,    89,   154,   154,    43,   183,   183,   183,
      98,   208,   205,    87,   182,    97,    93,   162,    88,    88,
      90,    90,    84,   216,    92,    95,    96,   165,    21,    71,
      72,    80,    81,    82,   168,   170,   191,    87,   201,    98,
      98,    98,   189,   210,    58,   206,    88,    19,    89,    94,
     192,   167,    89,   216,    88,    90,    19,    89,   168,   217,
      71,   193,   167,   171,   193,   216,    90,    90,    93,    99,
     188,    90,    93,   188,   193,   216,   168,   210,    88,   167,
      88,    99,   187,    88,    89,    24,   197,   197,    89,    88,
     197,   201,   154,    25,   198,   198,   201,   197,   198,    90,
     154,    22,   199,    90,   198,   199,   100,   154,    23,   200,
     100,   200,   154
};

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)		\
   ((Current).first_line   = (Rhs)[1].first_line,	\
    (Current).first_column = (Rhs)[1].first_column,	\
    (Current).last_line    = (Rhs)[N].last_line,	\
    (Current).last_column  = (Rhs)[N].last_column)
#endif

/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)

# define YYDSYMPRINT(Args)			\
do {						\
  if (yydebug)					\
    yysymprint Args;				\
} while (0)

# define YYDSYMPRINTF(Title, Token, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr, 					\
                  Token, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short int *bottom, short int *top)
#else
static void
yy_stack_print (bottom, top)
    short int *bottom;
    short int *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %u), ",
             yyrule - 1, yylno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname [yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname [yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YYDSYMPRINT(Args)
# define YYDSYMPRINTF(Title, Token, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if defined (YYMAXDEPTH) && YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

#endif /* !YYERROR_VERBOSE */



#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (yytype < YYNTOKENS)
    {
      YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
# ifdef YYPRINT
      YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
    }
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yytype, yyvaluep)
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  switch (yytype)
    {

      default:
        break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short int yyssa[YYINITDEPTH];
  short int *yyss = yyssa;
  register short int *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;


  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short int *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	short int *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YYDSYMPRINTF ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %s, ", yytname[yytoken]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 4:
#line 253 "parser.y"
    {
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
	}
    break;

  case 16:
#line 282 "parser.y"
    {
			if (notSkipping() && isMainModule(currentModule))
				appendCodeBlock(&currentSpec -> hdrcode,yyvsp[0].codeb);
		}
    break;

  case 18:
#line 287 "parser.y"
    {
			if (notSkipping() && isMainModule(currentModule))
				appendCodeBlock(&currentSpec -> cppcode,yyvsp[0].codeb);
		}
    break;

  case 37:
#line 313 "parser.y"
    {
			if (notSkipping())
			{
				yyvsp[0].memArg.nrderefs = 0;
				yyvsp[0].memArg.argflags = 0;

				currentMappedType = newMappedType(currentSpec,&yyvsp[0].memArg);
				usedInMainModule(currentSpec,currentMappedType -> iff);
			}
		}
    break;

  case 38:
#line 322 "parser.y"
    {
			if (notSkipping())
			{
				if (currentMappedType -> convfromcode == NULL)
					yyerror("%MappedType must have a %ConvertFromTypeCode directive");

				if (currentMappedType -> convtocode == NULL)
					yyerror("%MappedType must have a %ConvertToTypeCode directive");

				currentMappedType = NULL;
			}
		}
    break;

  case 41:
#line 340 "parser.y"
    {
			if (notSkipping())
				appendCodeBlock(&currentMappedType -> hdrcode,yyvsp[0].codeb);
		}
    break;

  case 42:
#line 344 "parser.y"
    {
			if (notSkipping())
			{
				if (currentMappedType -> convfromcode != NULL)
					yyerror("%MappedType has more than one %ConvertFromTypeCode directive");

				currentMappedType -> convfromcode = yyvsp[0].codeb;
			}
		}
    break;

  case 43:
#line 353 "parser.y"
    {
			if (notSkipping())
			{
				if (currentMappedType -> convtocode != NULL)
					yyerror("%MappedType has more than one %ConvertToTypeCode directive");

				currentMappedType -> convtocode = yyvsp[0].codeb;
			}
		}
    break;

  case 44:
#line 364 "parser.y"
    {
			if (notSkipping())
			{
				classDef *ns;

				ns = newClass(currentSpec,namespace_iface,text2scopedName(yyvsp[0].text));
			
				setCannotCreate(ns);

				pushScope(ns);

				sectionFlags = 0;
			}
		}
    break;

  case 45:
#line 377 "parser.y"
    {
			/* Remove the namespace name. */

			if (notSkipping())
				popScope();
		}
    break;

  case 48:
#line 389 "parser.y"
    {
			qualDef *qd;

			for (qd = currentModule -> qualifiers; qd != NULL; qd = qd -> next)
				if (qd -> qtype == platform_qualifier)
					yyerror("%Platforms has already been defined for this module");
		}
    break;

  case 49:
#line 396 "parser.y"
    {
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
    break;

  case 52:
#line 420 "parser.y"
    {
			newQualifier(currentSpec,currentModule,-1,yyvsp[0].text,platform_qualifier);
		}
    break;

  case 53:
#line 425 "parser.y"
    {
			newQualifier(currentSpec,currentModule,-1,yyvsp[0].text,feature_qualifier);
		}
    break;

  case 54:
#line 430 "parser.y"
    {
			if (currentModule -> nrTimeSlots >= 0)
				yyerror("%Timeline has already been defined for this module");

			currentModule -> nrTimeSlots = 0;
		}
    break;

  case 55:
#line 436 "parser.y"
    {
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
    break;

  case 58:
#line 460 "parser.y"
    {
			newQualifier(currentSpec,currentModule,currentModule -> nrTimeSlots++,yyvsp[0].text,time_qualifier);
		}
    break;

  case 59:
#line 465 "parser.y"
    {
			int skipNext;

			if (skipStackPtr >= MAX_NESTED_IF)
				yyerror("Internal error: increase the value of MAX_NESTED_IF");

			/* Nested %Ifs are implicit logical ands. */

			if (skipStackPtr > 0)
				yyvsp[-1].boolean = (yyvsp[-1].boolean && skipStack[skipStackPtr - 1]);

			skipStack[skipStackPtr++] = yyvsp[-1].boolean;
		}
    break;

  case 60:
#line 480 "parser.y"
    {
			yyval.boolean = platOrFeature(currentSpec,yyvsp[0].text,FALSE);
		}
    break;

  case 61:
#line 483 "parser.y"
    {
			yyval.boolean = platOrFeature(currentSpec,yyvsp[0].text,TRUE);
		}
    break;

  case 62:
#line 486 "parser.y"
    {
			yyval.boolean = (platOrFeature(currentSpec,yyvsp[0].text,FALSE) || yyvsp[-2].boolean);
		}
    break;

  case 63:
#line 489 "parser.y"
    {
			yyval.boolean = (platOrFeature(currentSpec,yyvsp[0].text,TRUE) || yyvsp[-3].boolean);
		}
    break;

  case 65:
#line 495 "parser.y"
    {
			yyval.boolean = timePeriod(currentSpec,yyvsp[-2].text,yyvsp[0].text);
		}
    break;

  case 66:
#line 500 "parser.y"
    {
			if (skipStackPtr-- <= 0)
				yyerror("Too many %End directives");
		}
    break;

  case 67:
#line 506 "parser.y"
    {
			optFlag *of;

			if (yyvsp[0].optflags.nrFlags == 0)
				yyerror("%License details not specified");

			if ((of = findOptFlag(&yyvsp[0].optflags,"Type",string_flag)) == NULL)
				yyerror("%License type not specified");

			currentModule -> license = sipMalloc(sizeof (licenseDef));

			currentModule -> license -> type = of -> fvalue.sval;

			currentModule -> license -> licensee = 
				((of = findOptFlag(&yyvsp[0].optflags,"Licensee",string_flag)) != NULL)
					? of -> fvalue.sval : NULL;

			currentModule -> license -> timestamp = 
				((of = findOptFlag(&yyvsp[0].optflags,"Timestamp",string_flag)) != NULL)
					? of -> fvalue.sval : NULL;

			currentModule -> license -> sig = 
				((of = findOptFlag(&yyvsp[0].optflags,"Signature",string_flag)) != NULL)
					? of -> fvalue.sval : NULL;
		}
    break;

  case 68:
#line 533 "parser.y"
    {
			/* Only bother if this is the main module. */

			if (isMainModule(currentModule))
			{
				expFuncDef *ef;

				/* Check it isn't already there. */

				for (ef = currentSpec -> exposed; ef != NULL; ef = ef -> next)
					if (strcmp(ef -> name,yyvsp[0].text) == 0)
						yyerror("Function has already been exposed in this module");

				ef = sipMalloc(sizeof (expFuncDef));

				ef -> name = yyvsp[0].text;
				ef -> next = currentSpec -> exposed;

				currentSpec -> exposed = ef;
			}
		}
    break;

  case 69:
#line 556 "parser.y"
    {
			/* Check the module hasn't already been defined. */

			moduleDef *mod;

			for (mod = currentSpec -> moduleList; mod != NULL; mod = mod -> next)
				if (mod -> name != NULL && strcmp(mod -> name,yyvsp[-1].text) == 0)
					yyerror("Module is already defined");

			currentModule -> name = yyvsp[-1].text;
		}
    break;

  case 70:
#line 569 "parser.y"
    {
			yyval.number = -1;
		}
    break;

  case 72:
#line 575 "parser.y"
    {
			parseFile(NULL,yyvsp[0].text,NULL,FALSE);
		}
    break;

  case 73:
#line 580 "parser.y"
    {
			parseFile(NULL,yyvsp[0].text,NULL,TRUE);
		}
    break;

  case 74:
#line 585 "parser.y"
    {
			newImport(currentSpec,yyvsp[0].text);
		}
    break;

  case 75:
#line 590 "parser.y"
    {
			yyval.codeb = NULL;
		}
    break;

  case 76:
#line 593 "parser.y"
    {
			yyval.codeb = yyvsp[0].codeb;
		}
    break;

  case 77:
#line 596 "parser.y"
    {
			yywarning("%VariableCode is renamed to %AccessCode in SIP v4 and later");
			yyval.codeb = yyvsp[0].codeb;
		}
    break;

  case 78:
#line 602 "parser.y"
    {
			if (isMainModule(currentModule))
				appendCodeBlock(&currentSpec -> copying,yyvsp[0].codeb);
		}
    break;

  case 79:
#line 608 "parser.y"
    {
			yyval.codeb = yyvsp[0].codeb;
		}
    break;

  case 80:
#line 611 "parser.y"
    {
			yywarning("%HeaderCode is renamed to %ModuleHeaderCode in this context in SIP v4 and later");
			yyval.codeb = yyvsp[0].codeb;
		}
    break;

  case 81:
#line 617 "parser.y"
    {
			yyval.codeb = yyvsp[0].codeb;
		}
    break;

  case 82:
#line 620 "parser.y"
    {
			yywarning("%HeaderCode is renamed to %TypeHeaderCode in this context in SIP v4 and later");
			yyval.codeb = yyvsp[0].codeb;
		}
    break;

  case 83:
#line 626 "parser.y"
    {
			yywarning("%ExportedHeaderCode is deprecated");

			appendCodeBlock(&currentSpec -> exphdrcode,yyvsp[0].codeb);
		}
    break;

  case 84:
#line 633 "parser.y"
    {
			yyval.codeb = yyvsp[0].codeb;
		}
    break;

  case 85:
#line 636 "parser.y"
    {
			yywarning("%C++Code is renamed to %ModuleCode in this context in SIP v4 and later");
			yyval.codeb = yyvsp[0].codeb;
		}
    break;

  case 86:
#line 642 "parser.y"
    {
			yyval.codeb = yyvsp[0].codeb;
		}
    break;

  case 87:
#line 645 "parser.y"
    {
			yywarning("%C++Code is renamed to %TypeCode in this context in SIP v4 and later");
			yyval.codeb = yyvsp[0].codeb;
		}
    break;

  case 88:
#line 651 "parser.y"
    {
			if (isMainModule(currentModule))
				appendCodeBlock(&currentSpec -> postinitcode,yyvsp[0].codeb);
		}
    break;

  case 89:
#line 657 "parser.y"
    {
			yywarning("%PrePythonCode is deprecated");

			if (isMainModule(currentModule))
				appendCodeBlock(&currentSpec -> prepycode,yyvsp[0].codeb);
		}
    break;

  case 90:
#line 665 "parser.y"
    {
			yywarning("%PythonCode is deprecated");

			if (isMainModule(currentModule))
				appendCodeBlock(&currentSpec -> pycode,yyvsp[0].codeb);
		}
    break;

  case 91:
#line 673 "parser.y"
    {
			if (isMainModule(currentModule))
				appendCodeBlock(&currentSpec -> docs,yyvsp[0].codeb);
		}
    break;

  case 92:
#line 679 "parser.y"
    {
			appendCodeBlock(&currentSpec -> docs,yyvsp[0].codeb);
		}
    break;

  case 93:
#line 684 "parser.y"
    {
			if (isMainModule(currentModule))
			{
				mkTemplateDef *mtd;

				yywarning("%Makefile is deprecated, please use the -b flag instead");

				mtd = sipMalloc(sizeof (mkTemplateDef));

				mtd -> name = yyvsp[-2].text;

				if ((mtd -> objext = yyvsp[-1].text) == NULL)
					mtd -> objext = DEFAULT_OFILE_EXT;

				mtd -> templ = NULL;

				appendCodeBlock(&mtd -> templ,yyvsp[0].codeb);

				mtd -> next = currentSpec -> mktemplates;
				currentSpec -> mktemplates = mtd;
			}
		}
    break;

  case 95:
#line 711 "parser.y"
    {
			if (notSkipping())
			{
				if (sectionFlags != 0 && (sectionFlags & ~(SECT_IS_PUBLIC | SECT_IS_PROT)) != 0)
					yyerror("Class enums must be in the public or protected sections");

				currentEnum = newEnum(currentSpec,currentModule,yyvsp[0].text,sectionFlags);
			}
		}
    break;

  case 97:
#line 722 "parser.y"
    {
			yyval.text = NULL;
		}
    break;

  case 98:
#line 725 "parser.y"
    {
			yyval.text = yyvsp[0].text;
		}
    break;

  case 99:
#line 730 "parser.y"
    {
			yyval.text = NULL;
		}
    break;

  case 100:
#line 733 "parser.y"
    {
			yyval.text = yyvsp[0].text;
		}
    break;

  case 107:
#line 748 "parser.y"
    {
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

				evd -> name = cacheName(currentSpec,currentModule,yyvsp[-2].text);
				evd -> ed = currentEnum;

				checkAttributes(currentSpec,evd -> ed -> ecd,evd -> name,enum_attr);

				evd -> next = currentEnum -> values;
				currentEnum -> values = evd;

			}
		}
    break;

  case 110:
#line 779 "parser.y"
    {
			yyval.valp = NULL;
		}
    break;

  case 111:
#line 782 "parser.y"
    {
			yyval.valp = yyvsp[0].valp;
		}
    break;

  case 113:
#line 788 "parser.y"
    {
			valueDef *vd;
 
			if (yyvsp[-2].valp -> vtype == string_value || yyvsp[0].valp -> vtype == string_value)
				yyerror("Invalid binary operator for string");
 
			/* Find the last value in the existing expression. */
 
			for (vd = yyvsp[-2].valp; vd -> next != NULL; vd = vd -> next)
				;
 
			vd -> vbinop = yyvsp[-1].qchar;
			vd -> next = yyvsp[0].valp;

			yyval.valp = yyvsp[-2].valp;
		}
    break;

  case 114:
#line 806 "parser.y"
    {
			yyval.qchar = '-';
		}
    break;

  case 115:
#line 809 "parser.y"
    {
			yyval.qchar = '+';
		}
    break;

  case 116:
#line 812 "parser.y"
    {
			yyval.qchar = '|';
		}
    break;

  case 117:
#line 817 "parser.y"
    {
			yyval.qchar = '\0';
		}
    break;

  case 118:
#line 820 "parser.y"
    {
			yyval.qchar = '!';
		}
    break;

  case 119:
#line 823 "parser.y"
    {
			yyval.qchar = '~';
		}
    break;

  case 120:
#line 826 "parser.y"
    {
			yyval.qchar = '-';
		}
    break;

  case 121:
#line 829 "parser.y"
    {
			yyval.qchar = '+';
		}
    break;

  case 122:
#line 834 "parser.y"
    {
			if (yyvsp[-1].qchar != '\0' && yyvsp[0].value.vtype == string_value)
				yyerror("Invalid unary operator for string");
 
			/*
			 * Convert the value to a simple expression on the
			 * heap.
			 */
 
			yyval.valp = sipMalloc(sizeof (valueDef));
 
			*yyval.valp = yyvsp[0].value;
			yyval.valp -> vunop = yyvsp[-1].qchar;
			yyval.valp -> vbinop = '\0';
			yyval.valp -> next = NULL;
		}
    break;

  case 124:
#line 853 "parser.y"
    {
			appendScopedName(&yyvsp[-2].scpvalp,yyvsp[0].scpvalp);
		}
    break;

  case 125:
#line 858 "parser.y"
    {
			yyval.scpvalp = text2scopePart(yyvsp[0].text);
		}
    break;

  case 126:
#line 863 "parser.y"
    {
			/*
			 * We let the C++ compiler decide if the value is a
			 * valid one - no point in building a full C++ parser
			 * here.
			 */

			yyval.value.vtype = scoped_value;
			yyval.value.u.vscp = yyvsp[0].scpvalp;
		}
    break;

  case 127:
#line 873 "parser.y"
    {
			fcallDef *fcd;

			fcd = sipMalloc(sizeof (fcallDef));
			*fcd = yyvsp[-1].fcall;
			fcd -> name = yyvsp[-3].scpvalp;

			yyval.value.vtype = fcall_value;
			yyval.value.u.fcd = fcd;
		}
    break;

  case 128:
#line 883 "parser.y"
    {
			yyval.value.vtype = real_value;
			yyval.value.u.vreal = yyvsp[0].real;
		}
    break;

  case 129:
#line 887 "parser.y"
    {
			yyval.value.vtype = numeric_value;
			yyval.value.u.vnum = yyvsp[0].number;
		}
    break;

  case 130:
#line 891 "parser.y"
    {
			yyval.value.vtype = numeric_value;
			yyval.value.u.vnum = 1;
		}
    break;

  case 131:
#line 895 "parser.y"
    {
			yyval.value.vtype = numeric_value;
			yyval.value.u.vnum = 0;
		}
    break;

  case 132:
#line 899 "parser.y"
    {
			yyval.value.vtype = string_value;
			yyval.value.u.vstr = yyvsp[0].text;
		}
    break;

  case 133:
#line 903 "parser.y"
    {
			yyval.value.vtype = qchar_value;
			yyval.value.u.vqchar = yyvsp[0].qchar;
		}
    break;

  case 134:
#line 909 "parser.y"
    {
			/* No values. */

			yyval.fcall.nrArgs = 0;
		}
    break;

  case 135:
#line 914 "parser.y"
    {
			/* The single or first value. */

			yyval.fcall.args[0] = yyvsp[0].valp;
			yyval.fcall.nrArgs = 1;
		}
    break;

  case 136:
#line 920 "parser.y"
    {
			/* Check that it wasn't ...(,value...). */

			if (yyval.fcall.nrArgs == 0)
				yyerror("First argument to function call is missing");

			/* Check there is room. */

			if (yyvsp[-2].fcall.nrArgs == MAX_NR_ARGS)
				yyerror("Too many arguments to function call");

			yyval.fcall = yyvsp[-2].fcall;

			yyval.fcall.args[yyval.fcall.nrArgs] = yyvsp[0].valp;
			yyval.fcall.nrArgs++;
		}
    break;

  case 137:
#line 938 "parser.y"
    {
			if (notSkipping())
				newTypedef(currentSpec,yyvsp[-1].text,&yyvsp[-2].memArg);
		}
    break;

  case 138:
#line 944 "parser.y"
    {
			if (notSkipping())
			{
				classDef *cd;

				cd = newClass(currentSpec,class_iface,yyvsp[-1].scpvalp);

				usedInMainModule(currentSpec,cd -> iff);
				setCannotCreate(cd);
				setIsOpaque(cd);
			}
		}
    break;

  case 139:
#line 958 "parser.y"
    {
			if (notSkipping())
			{
				classDef *cd;

				cd = newClass(currentSpec,class_iface,text2scopedName(yyvsp[0].text));

				usedInMainModule(currentSpec,cd -> iff);
				setCannotCreate(cd);

				pushScope(cd);

				sectionFlags = SECT_IS_PUBLIC;
			}
		}
    break;

  case 140:
#line 972 "parser.y"
    {
			if (notSkipping())
			{
				finishClass(currentScope(),&yyvsp[-4].optflags);
				popScope();
			}
		}
    break;

  case 141:
#line 981 "parser.y"
    {
			if (notSkipping())
			{
				classDef *cd;

				cd = newClass(currentSpec,class_iface,text2scopedName(yyvsp[0].text));

				usedInMainModule(currentSpec,cd -> iff);
				setCannotCreate(cd);

				pushScope(cd);

				sectionFlags = SECT_IS_PRIVATE;
			}
		}
    break;

  case 142:
#line 995 "parser.y"
    {
			if (notSkipping())
			{
				finishClass(currentScope(),&yyvsp[-4].optflags);
				popScope();
			}
		}
    break;

  case 147:
#line 1012 "parser.y"
    {
			if (notSkipping())
			{
				classDef *cd, *super;

				cd = currentScope();

				super = findClass(currentSpec,class_iface,yyvsp[0].scpvalp);

				appendToClassList(&cd -> supers,super);
				addToUsedList(&cd -> iff -> used,super -> iff);
			}
		}
    break;

  case 158:
#line 1039 "parser.y"
    {
			if (notSkipping())
				appendCodeBlock(&currentScope() -> cppcode,yyvsp[0].codeb);
		}
    break;

  case 159:
#line 1043 "parser.y"
    {
			if (notSkipping())
				appendCodeBlock(&currentScope() -> hdrcode,yyvsp[0].codeb);
		}
    break;

  case 163:
#line 1050 "parser.y"
    {
			if (notSkipping())
			{
				classDef *cd = currentScope();

				if (cd -> convtosubcode != NULL)
					yyerror("Class has more than one %ConvertToSubClassCode directive");

				cd -> convtosubcode = yyvsp[0].codeb;
			}
		}
    break;

  case 164:
#line 1061 "parser.y"
    {
			if (notSkipping())
			{
				classDef *cd = currentScope();

				if (cd -> convtocode != NULL)
					yyerror("Class has more than one %ConvertToTypeCode directive");

				cd -> convtocode = yyvsp[0].codeb;
			}
		}
    break;

  case 165:
#line 1072 "parser.y"
    {
			if (notSkipping())
				sectionFlags = SECT_IS_PUBLIC | yyvsp[-1].number;
		}
    break;

  case 166:
#line 1076 "parser.y"
    {
			if (notSkipping())
				sectionFlags = SECT_IS_PROT | yyvsp[-1].number;
		}
    break;

  case 167:
#line 1080 "parser.y"
    {
			if (notSkipping())
				sectionFlags = SECT_IS_PRIVATE | yyvsp[-1].number;
		}
    break;

  case 168:
#line 1084 "parser.y"
    {
			if (notSkipping())
				sectionFlags = SECT_IS_SIGNAL;
		}
    break;

  case 169:
#line 1090 "parser.y"
    {
			yyval.number = 0;
		}
    break;

  case 170:
#line 1093 "parser.y"
    {
			yyval.number = SECT_IS_SLOT;
		}
    break;

  case 171:
#line 1098 "parser.y"
    {
			if (notSkipping())
			{
				classDef *cd = currentScope();

				if (strcmp(classBaseName(cd),yyvsp[-8].text) != 0)
					yyerror("Destructor doesn't have the same name as its class");

				if (isDtor(cd))
					yyerror("Destructor has already been defined");

				if (yyvsp[-3].codeb != NULL && yyvsp[-2].codeb != NULL)
					yyerror("Cannot provide %MethodCode and %MemberCode");

				if (yyvsp[-3].codeb == NULL)
					yyvsp[-3].codeb = yyvsp[-2].codeb;

				if (yyvsp[-1].codeb != NULL && yyvsp[0].codeb != NULL)
					yyerror("Cannot provide %VirtualCatcherCode and %VirtualCode");

				if (yyvsp[-1].codeb == NULL)
					yyvsp[-1].codeb = yyvsp[0].codeb;

				cd -> dealloccode = yyvsp[-3].codeb;
				cd -> dtorcode = yyvsp[-1].codeb;
				cd -> dtorexceptions = yyvsp[-5].throwlist;
				cd -> classflags |= sectionFlags;

				/*
				 * Make the class complex if we have a virtual
				 * dtor or some dtor code.
				 */
				if (yyvsp[-10].number || yyvsp[-1].codeb != NULL)
					setIsComplex(cd);
			}
		}
    break;

  case 172:
#line 1136 "parser.y"
    {
			yyval.number = FALSE;
		}
    break;

  case 173:
#line 1139 "parser.y"
    {
			yyval.number = TRUE;
		}
    break;

  case 174:
#line 1144 "parser.y"
    {
			if (notSkipping())
			{
				if ((sectionFlags & (SECT_IS_PUBLIC | SECT_IS_PROT | SECT_IS_PRIVATE)) == 0)
					yyerror("Constructor must be in the public, private or protected sections");

				newCtor(yyvsp[-9].text,sectionFlags,&yyvsp[-7].arglist,&yyvsp[-4].optflags,yyvsp[-1].codeb,yyvsp[0].codeb,yyvsp[-5].throwlist,yyvsp[-3].optsignature);
			}

			free(yyvsp[-9].text);
		}
    break;

  case 175:
#line 1157 "parser.y"
    {
			yyval.optsignature = NULL;
		}
    break;

  case 176:
#line 1160 "parser.y"
    {
			yyval.optsignature = sipMalloc(sizeof (signatureDef));

			yyval.optsignature -> args = yyvsp[-2].arglist;
		}
    break;

  case 177:
#line 1167 "parser.y"
    {
			yyval.optsignature = NULL;
		}
    break;

  case 178:
#line 1170 "parser.y"
    {
			yyval.optsignature = sipMalloc(sizeof (signatureDef));

			yyval.optsignature -> result = yyvsp[-4].memArg;
			yyval.optsignature -> args = yyvsp[-2].arglist;
		}
    break;

  case 179:
#line 1178 "parser.y"
    {
			if (notSkipping())
			{
				if (sectionFlags != 0 && (sectionFlags & (SECT_IS_PUBLIC | SECT_IS_PROT | SECT_IS_PRIVATE | SECT_IS_SLOT | SECT_IS_SIGNAL)) == 0)
					yyerror("Class function must be in the public, private, protected, slot or signal sections");

				newFunction(currentSpec,currentModule,
					    sectionFlags,currentIsStatic,
					    currentOverIsVirt,
					    &yyvsp[-14].memArg,yyvsp[-13].text,&yyvsp[-11].arglist,yyvsp[-9].number,yyvsp[-7].number,&yyvsp[-6].optflags,yyvsp[-3].codeb,yyvsp[-2].codeb,yyvsp[-1].codeb,yyvsp[0].codeb,yyvsp[-8].throwlist,yyvsp[-5].optsignature);
			}

			currentIsStatic = FALSE;
			currentOverIsVirt = FALSE;
		}
    break;

  case 180:
#line 1193 "parser.y"
    {
			if (notSkipping())
				newFunction(currentSpec,currentModule,
					    sectionFlags,currentIsStatic,
					    currentOverIsVirt,
					    &yyvsp[-12].memArg,yyvsp[-10].text,&yyvsp[-8].arglist,yyvsp[-6].number,FALSE,&yyvsp[-4].optflags,yyvsp[-1].codeb,yyvsp[0].codeb,NULL,NULL,yyvsp[-5].throwlist,yyvsp[-3].optsignature);

			currentIsStatic = FALSE;
			currentOverIsVirt = FALSE;
		}
    break;

  case 181:
#line 1205 "parser.y"
    {yyval.text = "__add__";}
    break;

  case 182:
#line 1206 "parser.y"
    {yyval.text = "__sub__";}
    break;

  case 183:
#line 1207 "parser.y"
    {yyval.text = "__mul__";}
    break;

  case 184:
#line 1208 "parser.y"
    {yyval.text = "__div__";}
    break;

  case 185:
#line 1209 "parser.y"
    {yyval.text = "__mod__";}
    break;

  case 186:
#line 1210 "parser.y"
    {yyval.text = "__and__";}
    break;

  case 187:
#line 1211 "parser.y"
    {yyval.text = "__or__";}
    break;

  case 188:
#line 1212 "parser.y"
    {yyval.text = "__xor__";}
    break;

  case 189:
#line 1213 "parser.y"
    {yyval.text = "__lshift__";}
    break;

  case 190:
#line 1214 "parser.y"
    {yyval.text = "__rshift__";}
    break;

  case 191:
#line 1215 "parser.y"
    {yyval.text = "__iadd__";}
    break;

  case 192:
#line 1216 "parser.y"
    {yyval.text = "__isub__";}
    break;

  case 193:
#line 1217 "parser.y"
    {yyval.text = "__imul__";}
    break;

  case 194:
#line 1218 "parser.y"
    {yyval.text = "__idiv__";}
    break;

  case 195:
#line 1219 "parser.y"
    {yyval.text = "__imod__";}
    break;

  case 196:
#line 1220 "parser.y"
    {yyval.text = "__iand__";}
    break;

  case 197:
#line 1221 "parser.y"
    {yyval.text = "__ior__";}
    break;

  case 198:
#line 1222 "parser.y"
    {yyval.text = "__ixor__";}
    break;

  case 199:
#line 1223 "parser.y"
    {yyval.text = "__ilshift__";}
    break;

  case 200:
#line 1224 "parser.y"
    {yyval.text = "__irshift__";}
    break;

  case 201:
#line 1225 "parser.y"
    {yyval.text = "__invert__";}
    break;

  case 202:
#line 1226 "parser.y"
    {yyval.text = "__call__";}
    break;

  case 203:
#line 1227 "parser.y"
    {yyval.text = "__getitem__";}
    break;

  case 204:
#line 1228 "parser.y"
    {yyval.text = "__lt__";}
    break;

  case 205:
#line 1229 "parser.y"
    {yyval.text = "__le__";}
    break;

  case 206:
#line 1230 "parser.y"
    {yyval.text = "__eq__";}
    break;

  case 207:
#line 1231 "parser.y"
    {yyval.text = "__ne__";}
    break;

  case 208:
#line 1232 "parser.y"
    {yyval.text = "__gt__";}
    break;

  case 209:
#line 1233 "parser.y"
    {yyval.text = "__ge__";}
    break;

  case 210:
#line 1236 "parser.y"
    {
			yyval.number = FALSE;
		}
    break;

  case 211:
#line 1239 "parser.y"
    {
			yyval.number = TRUE;
		}
    break;

  case 212:
#line 1244 "parser.y"
    {
			yyval.number = 0;
		}
    break;

  case 213:
#line 1247 "parser.y"
    {
			if (yyvsp[0].number != 0)
				yyerror("Abstract virtual function '= 0' expected");

			yyval.number = TRUE;
		}
    break;

  case 214:
#line 1255 "parser.y"
    {
			yyval.optflags.nrFlags = 0;
		}
    break;

  case 215:
#line 1258 "parser.y"
    {
			yyval.optflags = yyvsp[-1].optflags;
		}
    break;

  case 216:
#line 1264 "parser.y"
    {
			yyval.optflags.flags[0] = yyvsp[0].flag;
			yyval.optflags.nrFlags = 1;
		}
    break;

  case 217:
#line 1268 "parser.y"
    {
			/* Check there is room. */

			if (yyvsp[-2].optflags.nrFlags == MAX_NR_FLAGS)
				yyerror("Too many optional flags");

			yyval.optflags = yyvsp[-2].optflags;

			yyval.optflags.flags[yyval.optflags.nrFlags++] = yyvsp[0].flag;
		}
    break;

  case 218:
#line 1280 "parser.y"
    {
			yyval.flag.ftype = bool_flag;
			yyval.flag.fname = yyvsp[0].text;
		}
    break;

  case 219:
#line 1284 "parser.y"
    {
			yyval.flag = yyvsp[0].flag;
			yyval.flag.fname = yyvsp[-2].text;
		}
    break;

  case 220:
#line 1290 "parser.y"
    {
			yyval.flag.ftype = name_flag;
			yyval.flag.fvalue.sval = yyvsp[0].text;
		}
    break;

  case 221:
#line 1294 "parser.y"
    {
			yyval.flag.ftype = string_flag;
			yyval.flag.fvalue.sval = yyvsp[0].text;
		}
    break;

  case 222:
#line 1298 "parser.y"
    {
			yyval.flag.ftype = integer_flag;
			yyval.flag.fvalue.ival = yyvsp[0].number;
		}
    break;

  case 223:
#line 1304 "parser.y"
    {
			yyval.codeb = NULL;
		}
    break;

  case 224:
#line 1307 "parser.y"
    {
			yyval.codeb = yyvsp[0].codeb;
		}
    break;

  case 225:
#line 1312 "parser.y"
    {
			yyval.codeb = NULL;
		}
    break;

  case 226:
#line 1315 "parser.y"
    {
			yywarning("%MemberCode is deprecated, please use %MethodCode instead");
			yyval.codeb = yyvsp[0].codeb;
		}
    break;

  case 227:
#line 1321 "parser.y"
    {
			yyval.codeb = NULL;
		}
    break;

  case 228:
#line 1324 "parser.y"
    {
			yyval.codeb = yyvsp[0].codeb;
		}
    break;

  case 229:
#line 1329 "parser.y"
    {
			yyval.codeb = NULL;
		}
    break;

  case 230:
#line 1332 "parser.y"
    {
			yywarning("%VirtualCode is deprecated, please use %VirtualCatcherCode instead");
			yyval.codeb = yyvsp[0].codeb;
		}
    break;

  case 231:
#line 1338 "parser.y"
    {
			int a, nrrxcon, nrrxdis, nrslotcon, nrslotdis, nrarray, nrarraysize;

			nrrxcon = nrrxdis = nrslotcon = nrslotdis = nrarray = nrarraysize = 0;

			for (a = 0; a < yyvsp[0].arglist.nrArgs; ++a)
			{
				argDef *ad = &yyvsp[0].arglist.args[a];

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

			yyval.arglist = yyvsp[0].arglist;
		}
    break;

  case 232:
#line 1386 "parser.y"
    {
			/* No arguments. */

			yyval.arglist.nrArgs = 0;
		}
    break;

  case 233:
#line 1391 "parser.y"
    {
			/* The single or first argument. */

			yyval.arglist.args[0] = yyvsp[0].memArg;
			yyval.arglist.nrArgs = 1;
		}
    break;

  case 234:
#line 1397 "parser.y"
    {
			/* Check that it wasn't ...(,arg...). */

			if (yyvsp[-2].arglist.nrArgs == 0)
				yyerror("First argument of list is missing");

			/*
			 * If this argument has no default value, then all
			 * previous ones mustn't either.
			 */

			if (yyvsp[0].memArg.defval == NULL)
			{
				int a;

				for (a = 0; a < yyvsp[-2].arglist.nrArgs; ++a)
					if (yyvsp[-2].arglist.args[a].defval != NULL)
						yyerror("Compulsory argument given after optional argument");
			}

			/* Check there is room. */

			if (yyvsp[-2].arglist.nrArgs == MAX_NR_ARGS)
				yyerror("Internal error - increase the value of MAX_NR_ARGS");

			yyval.arglist = yyvsp[-2].arglist;
			yyval.arglist.args[yyval.arglist.nrArgs++] = yyvsp[0].memArg;
		}
    break;

  case 235:
#line 1427 "parser.y"
    {
			yyval.memArg.atype = signal_type;
			yyval.memArg.argflags = 0;
			yyval.memArg.nrderefs = 0;

			currentSpec -> sigslots = TRUE;
		}
    break;

  case 236:
#line 1434 "parser.y"
    {
			yyval.memArg.atype = slot_type;
			yyval.memArg.argflags = 0;
			yyval.memArg.nrderefs = 0;

			currentSpec -> sigslots = TRUE;
		}
    break;

  case 237:
#line 1441 "parser.y"
    {
			yyval.memArg.atype = rxcon_type;
			yyval.memArg.argflags = 0;
			yyval.memArg.nrderefs = 0;

			currentSpec -> sigslots = TRUE;
		}
    break;

  case 238:
#line 1448 "parser.y"
    {
			yyval.memArg.atype = rxdis_type;
			yyval.memArg.argflags = 0;
			yyval.memArg.nrderefs = 0;

			currentSpec -> sigslots = TRUE;
		}
    break;

  case 239:
#line 1455 "parser.y"
    {
			yyval.memArg.atype = slotcon_type;
			yyval.memArg.argflags = 0;
			yyval.memArg.nrderefs = 0;

			yyval.memArg.u.sa = sipMalloc(sizeof (funcArgs));
			*yyval.memArg.u.sa = yyvsp[-1].arglist;

			currentSpec -> sigslots = TRUE;
		}
    break;

  case 240:
#line 1465 "parser.y"
    {
			yyval.memArg.atype = slotdis_type;
			yyval.memArg.argflags = 0;
			yyval.memArg.nrderefs = 0;

			yyval.memArg.u.sa = sipMalloc(sizeof (funcArgs));
			*yyval.memArg.u.sa = yyvsp[-1].arglist;

			currentSpec -> sigslots = TRUE;
		}
    break;

  case 241:
#line 1475 "parser.y"
    {
			yyval.memArg.atype = qobject_type;
			yyval.memArg.argflags = 0;
			yyval.memArg.nrderefs = 0;
		}
    break;

  case 242:
#line 1480 "parser.y"
    {
			yyval.memArg = yyvsp[-1].memArg;
			yyval.memArg.defval = yyvsp[0].valp;
		}
    break;

  case 243:
#line 1486 "parser.y"
    {currentIsStatic = TRUE;}
    break;

  case 248:
#line 1494 "parser.y"
    {currentOverIsVirt = TRUE;}
    break;

  case 251:
#line 1498 "parser.y"
    {
			if (notSkipping())
			{
				/* Check the section. */

				if (sectionFlags != 0)
				{
					if ((sectionFlags & SECT_IS_PUBLIC) == 0)
						yyerror("Class variables must be in the public section");

					if (!currentIsStatic && yyvsp[0].codeb != NULL)
						yyerror("%AccessCode cannot be specified for non-static class variables");
				}

				newVar(currentSpec,currentModule,yyvsp[-2].text,currentIsStatic,&yyvsp[-3].memArg,yyvsp[0].codeb);
			}

			currentIsStatic = FALSE;
		}
    break;

  case 252:
#line 1519 "parser.y"
    {
			yyval.memArg = yyvsp[-2].memArg;
			yyval.memArg.nrderefs = yyvsp[-1].number;
			yyval.memArg.argflags = ARG_IS_CONST | yyvsp[0].number;
		}
    break;

  case 253:
#line 1524 "parser.y"
    {
			yyval.memArg = yyvsp[-2].memArg;
			yyval.memArg.nrderefs = yyvsp[-1].number;
			yyval.memArg.argflags = yyvsp[0].number;
		}
    break;

  case 254:
#line 1531 "parser.y"
    {
			yyval.memArg = yyvsp[-1].memArg;

			if (findOptFlag(&yyvsp[0].optflags,"AllowNone",bool_flag) != NULL)
				yyval.memArg.argflags |= ARG_ALLOW_NONE;

			if (findOptFlag(&yyvsp[0].optflags,"GetWrapper",bool_flag) != NULL)
				yyval.memArg.argflags |= ARG_GET_WRAPPER;

			if (findOptFlag(&yyvsp[0].optflags,"Array",bool_flag) != NULL)
				yyval.memArg.argflags |= ARG_ARRAY;

			if (findOptFlag(&yyvsp[0].optflags,"ArraySize",bool_flag) != NULL)
				yyval.memArg.argflags |= ARG_ARRAY_SIZE;

			if (findOptFlag(&yyvsp[0].optflags,"Transfer",bool_flag) != NULL)
				yyval.memArg.argflags |= ARG_XFERRED;

			if (findOptFlag(&yyvsp[0].optflags,"TransferThis",bool_flag) != NULL)
				yyval.memArg.argflags |= ARG_THIS_XFERRED;

			if (findOptFlag(&yyvsp[0].optflags,"TransferBack",bool_flag) != NULL)
				yyval.memArg.argflags |= ARG_XFERRED_BACK;

			if (findOptFlag(&yyvsp[0].optflags,"In",bool_flag) != NULL)
				yyval.memArg.argflags |= ARG_IN;

			if (findOptFlag(&yyvsp[0].optflags,"Out",bool_flag) != NULL)
				yyval.memArg.argflags |= ARG_OUT;

			if (findOptFlag(&yyvsp[0].optflags,"Constrained",bool_flag) != NULL)
			{
				switch (yyval.memArg.atype)
				{
				case int_type:
					yyval.memArg.atype = cint_type;
					break;

				case float_type:
					yyval.memArg.atype = cfloat_type;
					break;

				case double_type:
					yyval.memArg.atype = cdouble_type;
					break;

				default:
					yyerror("/Constrained/ may not be specified for this type");
				}
			}
		}
    break;

  case 255:
#line 1584 "parser.y"
    {
			yyval.number = 0;
		}
    break;

  case 256:
#line 1587 "parser.y"
    {
			yyval.number = ARG_IS_REF;
		}
    break;

  case 257:
#line 1592 "parser.y"
    {
			yyval.number = 0;
		}
    break;

  case 258:
#line 1595 "parser.y"
    {
			yyval.number = yyvsp[-1].number + 1;
		}
    break;

  case 259:
#line 1600 "parser.y"
    {
			yyval.memArg.atype = defined_type;
			yyval.memArg.u.snd = yyvsp[0].scpvalp;
		}
    break;

  case 260:
#line 1604 "parser.y"
    {
			templateDef *td;

			td = sipMalloc(sizeof(templateDef));
			td -> fqname = yyvsp[-3].scpvalp;
			td -> types = yyvsp[-1].arglist;

			yyval.memArg.atype = template_type;
			yyval.memArg.u.td = td;
		}
    break;

  case 261:
#line 1614 "parser.y"
    {
			yyval.memArg.atype = struct_type;
			yyval.memArg.u.sname = yyvsp[0].scpvalp;
		}
    break;

  case 262:
#line 1618 "parser.y"
    {
			yyval.memArg.atype = ushort_type;
		}
    break;

  case 263:
#line 1621 "parser.y"
    {
			yyval.memArg.atype = short_type;
		}
    break;

  case 264:
#line 1624 "parser.y"
    {
			yyval.memArg.atype = uint_type;
		}
    break;

  case 265:
#line 1627 "parser.y"
    {
			yyval.memArg.atype = uint_type;
		}
    break;

  case 266:
#line 1630 "parser.y"
    {
			yyval.memArg.atype = int_type;
		}
    break;

  case 267:
#line 1633 "parser.y"
    {
			yyval.memArg.atype = long_type;
		}
    break;

  case 268:
#line 1636 "parser.y"
    {
			yyval.memArg.atype = ulong_type;
		}
    break;

  case 269:
#line 1639 "parser.y"
    {
			yyval.memArg.atype = float_type;
		}
    break;

  case 270:
#line 1642 "parser.y"
    {
			yyval.memArg.atype = double_type;
		}
    break;

  case 271:
#line 1645 "parser.y"
    {
			yyval.memArg.atype = bool_type;
		}
    break;

  case 272:
#line 1648 "parser.y"
    {
			yyval.memArg.atype = ustring_type;
		}
    break;

  case 273:
#line 1651 "parser.y"
    {
			yyval.memArg.atype = string_type;
		}
    break;

  case 274:
#line 1654 "parser.y"
    {
			yyval.memArg.atype = voidptr_type;
		}
    break;

  case 275:
#line 1657 "parser.y"
    {
			yyval.memArg.atype = pyobject_type;
		}
    break;

  case 276:
#line 1660 "parser.y"
    {
			yyval.memArg.atype = pytuple_type;
		}
    break;

  case 277:
#line 1663 "parser.y"
    {
			yyval.memArg.atype = pylist_type;
		}
    break;

  case 278:
#line 1666 "parser.y"
    {
			yyval.memArg.atype = pydict_type;
		}
    break;

  case 279:
#line 1669 "parser.y"
    {
			yyval.memArg.atype = pycallable_type;
		}
    break;

  case 280:
#line 1672 "parser.y"
    {
			yyval.memArg.atype = pyslice_type;
		}
    break;

  case 281:
#line 1677 "parser.y"
    {
			/* The single or first type. */

			yyval.arglist.args[0] = yyvsp[0].memArg;
			yyval.arglist.nrArgs = 1;
		}
    break;

  case 282:
#line 1683 "parser.y"
    {
			/* Check that it wasn't ,type... */

			if (yyval.arglist.nrArgs == 0)
				yyerror("First type of list is missing");

			/* Check there is room. */

			if (yyvsp[-2].arglist.nrArgs == MAX_NR_ARGS)
				yyerror("Internal error - increase the value of MAX_NR_ARGS");

			yyval.arglist = yyvsp[-2].arglist;

			yyval.arglist.args[yyval.arglist.nrArgs] = yyvsp[0].memArg;
			yyval.arglist.nrArgs++;
		}
    break;

  case 283:
#line 1701 "parser.y"
    {
			yyval.throwlist = NULL;
		}
    break;

  case 284:
#line 1704 "parser.y"
    {
			int e;

			/*
			 * Make sure the exceptions' header files are
			 * included.
			 */

			for (e = 0; e < yyvsp[-1].throwlist -> nrArgs; ++e)
				addToUsedList(&currentScope() -> iff -> used,yyvsp[-1].throwlist -> args[e] -> iff);

			yyval.throwlist = yyvsp[-1].throwlist;
		}
    break;

  case 285:
#line 1719 "parser.y"
    {
			/* Empty list so use a blank. */

			yyval.throwlist = sipMalloc(sizeof (throwArgs));
			yyval.throwlist -> nrArgs = 0;
		}
    break;

  case 286:
#line 1725 "parser.y"
    {
			/* The only or first exception. */

			yyval.throwlist = sipMalloc(sizeof (throwArgs));
			yyval.throwlist -> nrArgs = 1;
			yyval.throwlist -> args[0] = findClass(currentSpec,class_iface,yyvsp[0].scpvalp);
		}
    break;

  case 287:
#line 1732 "parser.y"
    {
			/* Check that it wasn't ...(,arg...). */

			if (yyvsp[-2].throwlist -> nrArgs == 0)
				yyerror("First exception of throw specifier is missing");

			/* Check there is room. */

			if (yyvsp[-2].throwlist -> nrArgs == MAX_NR_ARGS)
				yyerror("Internal error - increase the value of MAX_NR_ARGS");

			yyval.throwlist = yyvsp[-2].throwlist;
			yyval.throwlist -> args[yyval.throwlist -> nrArgs++] = findClass(currentSpec,class_iface,yyvsp[0].scpvalp);
		}
    break;


    }

/* Line 1010 of yacc.c.  */
#line 3834 "parser.c"

  yyvsp -= yylen;
  yyssp -= yylen;


  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  int yytype = YYTRANSLATE (yychar);
	  const char* yyprefix;
	  char *yymsg;
	  int yyx;

	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  int yyxbegin = yyn < 0 ? -yyn : 0;

	  /* Stay within bounds of both yycheck and yytname.  */
	  int yychecklim = YYLAST - yyn;
	  int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
	  int yycount = 0;

	  yyprefix = ", expecting ";
	  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      {
		yysize += yystrlen (yyprefix) + yystrlen (yytname [yyx]);
		yycount += 1;
		if (yycount == 5)
		  {
		    yysize = 0;
		    break;
		  }
	      }
	  yysize += (sizeof ("syntax error, unexpected ")
		     + yystrlen (yytname[yytype]));
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "syntax error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[yytype]);

	      if (yycount < 5)
		{
		  yyprefix = ", expecting ";
		  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
		      {
			yyp = yystpcpy (yyp, yyprefix);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yyprefix = " or ";
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("syntax error; also virtual memory exhausted");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror ("syntax error");
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* If at end of input, pop the error token,
	     then the rest of the stack, then return failure.  */
	  if (yychar == YYEOF)
	     for (;;)
	       {
		 YYPOPSTACK;
		 if (yyssp == yyss)
		   YYABORT;
		 YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
		 yydestruct (yystos[*yyssp], yyvsp);
	       }
        }
      else
	{
	  YYDSYMPRINTF ("Error: discarding", yytoken, &yylval, &yylloc);
	  yydestruct (yytoken, &yylval);
	  yychar = YYEMPTY;

	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

#ifdef __GNUC__
  /* Pacify GCC when the user code never invokes YYERROR and the label
     yyerrorlab therefore never appears in user code.  */
  if (0)
     goto yyerrorlab;
#endif

  yyvsp -= yylen;
  yyssp -= yylen;
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
      yydestruct (yystos[yystate], yyvsp);
      YYPOPSTACK;
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;


  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*----------------------------------------------.
| yyoverflowlab -- parser overflow comes here.  |
`----------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}


#line 1748 "parser.y"



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

