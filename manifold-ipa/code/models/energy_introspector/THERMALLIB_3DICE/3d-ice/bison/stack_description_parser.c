
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0

/* Substitute the variable and function names.  */
#define yyparse         stack_description_parse
#define yylex           stack_description_lex
#define yyerror         stack_description_error
#define yylval          stack_description_lval
#define yychar          stack_description_char
#define yydebug         stack_description_debug
#define yynerrs         stack_description_nerrs


/* Copy the first part of user declarations.  */


/* Line 189 of yacc.c  */
#line 81 "stack_description_parser.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

/* "%code requires" blocks.  */

/* Line 209 of yacc.c  */
#line 40 "stack_description_parser.y"

    #include "types.h"

    #include "material.h"
    #include "die.h"
    #include "stack_element.h"
    #include "inspection_point.h"
    #include "stack_description.h"

    typedef void* yyscan_t;



/* Line 209 of yacc.c  */
#line 119 "stack_description_parser.c"

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     _2RM = 258,
     _4RM = 259,
     AMBIENT = 260,
     AVERAGE = 261,
     BOTTOM = 262,
     CAPACITY = 263,
     CELL = 264,
     CHANNEL = 265,
     CHIP = 266,
     COEFFICIENT = 267,
     CONDUCTIVITY = 268,
     CONVENTIONAL = 269,
     COOLANT = 270,
     DARCY = 271,
     DIAMETER = 272,
     DIE = 273,
     DIMENSIONS = 274,
     DISTRIBUTION = 275,
     FINAL = 276,
     FIRST = 277,
     FLOORPLAN = 278,
     FLOW = 279,
     HEAT = 280,
     HEIGHT = 281,
     INCOMING = 282,
     INITIAL_ = 283,
     INLINE = 284,
     LAST = 285,
     LAYER = 286,
     LENGTH = 287,
     MATERIAL = 288,
     MAXIMUM = 289,
     MICROCHANNEL = 290,
     MINIMUM = 291,
     OUTPUT = 292,
     PIN = 293,
     PINFIN = 294,
     PITCH = 295,
     RATE = 296,
     SIDE = 297,
     SINK = 298,
     SLOT = 299,
     SOLVER = 300,
     SOURCE = 301,
     STACK = 302,
     STAGGERED = 303,
     STATE = 304,
     STEADY = 305,
     STEP = 306,
     TCELL = 307,
     TCOOLANT = 308,
     TEMPERATURE = 309,
     TFLP = 310,
     TFLPEL = 311,
     THERMAL = 312,
     TMAP = 313,
     TOP = 314,
     TRANSFER = 315,
     TRANSIENT = 316,
     VELOCITY = 317,
     VOLUMETRIC = 318,
     WALL = 319,
     WIDTH = 320,
     DVALUE = 321,
     IDENTIFIER = 322,
     PATH = 323
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 55 "stack_description_parser.y"

    double                double_v ;
    String_t              string ;
    Material             *material_p ;
    Coolant_t             coolant_v ;
    ChannelModel_t        channel_model_v ;
    Die                  *die_p ;
    Layer                *layer_p ;
    StackElement         *stack_element_p ;
    InspectionPoint      *inspection_point_p ;
    OutputInstant_t       output_instant_v ;
    OutputQuantity_t      output_quantity_v ;



/* Line 214 of yacc.c  */
#line 220 "stack_description_parser.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 232 "stack_description_parser.c"
/* Unqualified %code blocks.  */

/* Line 265 of yacc.c  */
#line 70 "stack_description_parser.y"

    #include "analysis.h"
    #include "channel.h"
    #include "conventional_heat_sink.h"
    #include "dimensions.h"
    #include "floorplan_element.h"
    #include "floorplan.h"
    #include "layer.h"
    #include "macros.h"
    #include "stack_description.h"

    #include "../flex/stack_description_scanner.h"

    void stack_description_error

        (StackDescription *stack, Analysis *analysis,
         yyscan_t scanner, String_t message) ;

    static char error_message [100] ;

    static CellDimension_t first_wall_length ;
    static CellDimension_t last_wall_length ;
    static CellDimension_t wall_length ;
    static CellDimension_t channel_length ;
    static Quantity_t      num_channels ;
    static Quantity_t      num_dies ;



/* Line 265 of yacc.c  */
#line 267 "stack_description_parser.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  6
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   310

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  75
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  28
/* YYNRULES -- Number of rules.  */
#define YYNRULES  55
/* YYNRULES -- Number of states.  */
#define YYNSTATES  315

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   323

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      72,    73,     2,     2,    71,     2,    74,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    69,    70,
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
      65,    66,    67,    68
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,    13,    15,    18,    31,    32,    46,    47,
      85,   121,   158,   165,   179,   186,   197,   198,   204,   205,
     211,   213,   215,   216,   219,   222,   225,   229,   231,   234,
     241,   258,   262,   264,   267,   273,   277,   284,   289,   299,
     304,   305,   309,   311,   314,   327,   338,   351,   360,   371,
     373,   375,   377,   378,   381,   384
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      76,     0,    -1,    77,    79,    80,    90,    92,    93,    96,
      97,    98,    -1,    78,    -1,    77,    78,    -1,    33,    67,
      69,    57,    13,    66,    70,    63,    25,     8,    66,    70,
      -1,    -1,    14,    25,    43,    69,    25,    60,    12,    66,
      70,     5,    54,    66,    70,    -1,    -1,    35,     4,    69,
      26,    66,    70,    10,    32,    66,    70,    64,    32,    66,
      70,    83,    84,    64,    33,    67,    70,    15,    24,    41,
      66,    70,    81,    15,    63,    25,     8,    66,    70,    15,
      27,    54,    66,    70,    -1,    35,     3,    69,    26,    66,
      70,    10,    32,    66,    70,    64,    32,    66,    70,    64,
      33,    67,    70,    15,    24,    41,    66,    70,    82,    15,
      63,    25,     8,    66,    70,    15,    27,    54,    66,    70,
      -1,    39,    69,    26,    66,    70,    38,    17,    66,    70,
      38,    40,    66,    70,    38,    20,    85,    70,    38,    33,
      67,    70,    16,    62,    66,    70,    15,    63,    25,     8,
      66,    70,    15,    27,    54,    66,    70,    -1,    15,    25,
      60,    12,    66,    70,    -1,    15,    25,    60,    12,    42,
      66,    71,    59,    66,    71,     7,    66,    70,    -1,    15,
      25,    60,    12,    66,    70,    -1,    15,    25,    60,    12,
      59,    66,    71,     7,    66,    70,    -1,    -1,    22,    64,
      32,    66,    70,    -1,    -1,    30,    64,    32,    66,    70,
      -1,    29,    -1,    48,    -1,    -1,    86,    87,    -1,    31,
      89,    -1,    46,    89,    -1,    66,    67,    70,    -1,    91,
      -1,    90,    91,    -1,    18,    67,    69,    86,    88,    86,
      -1,    19,    69,    11,    32,    66,    71,    65,    66,    70,
       9,    32,    66,    71,    65,    66,    70,    -1,    47,    69,
      94,    -1,    95,    -1,    94,    95,    -1,    31,    67,    66,
      67,    70,    -1,    10,    67,    70,    -1,    18,    67,    67,
      23,    68,    70,    -1,    45,    69,    50,    70,    -1,    45,
      69,    61,    51,    66,    71,    44,    66,    70,    -1,    28,
      54,    66,    70,    -1,    -1,    37,    69,    99,    -1,   100,
      -1,    99,   100,    -1,    52,    72,    67,    71,    66,    71,
      66,    71,    68,   102,    73,    70,    -1,    55,    72,    67,
      71,    68,    71,   101,   102,    73,    70,    -1,    56,    72,
      67,    74,    67,    71,    68,    71,   101,   102,    73,    70,
      -1,    58,    72,    67,    71,    68,   102,    73,    70,    -1,
      53,    72,    67,    71,    68,    71,   101,   102,    73,    70,
      -1,    34,    -1,    36,    -1,     6,    -1,    -1,    71,    51,
      -1,    71,    44,    -1,    71,    21,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   220,   220,   237,   243,   265,   290,   294,   316,   320,
     376,   428,   482,   488,   500,   506,   516,   517,   521,   522,
     526,   527,   537,   540,   549,   551,   555,   596,   601,   623,
     721,   802,   896,   909,   940,   996,  1024,  1097,  1108,  1151,
    1161,  1165,  1171,  1176,  1186,  1247,  1320,  1417,  1460,  1535,
    1536,  1537,  1543,  1544,  1545,  1546
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "\"keyword 2rm\"", "\"keyword 4rm\"",
  "\"keyword ambient\"", "\"keyword average\"", "\"keyword bottom\"",
  "\"keyword capacity\"", "\"keyword cell\"", "\"keyword channel\"",
  "\"keyword chip\"", "\"keyword coefficient\"",
  "\"keyword conductivity\"", "\"keyword conventional\"",
  "\"keyword coolant\"", "\"keyword darcy\"", "\"keyword diameter\"",
  "\"keyword die\"", "\"keyword dimensions\"", "\"keyword distribution\"",
  "\"keyword final\"", "\"keyword first\"", "\"keyword floorplan\"",
  "\"keyword flow\"", "\"keyword heat\"", "\"keyword height\"",
  "\"keyword incoming\"", "\"keyword initial\"", "\"keyword inline\"",
  "\"keyword last\"", "\"keyword layer\"", "\"keyword length\"",
  "\"keyword material\"", "\"keyword maximum\"",
  "\"keyword microchannel\"", "\"keyword minimum\"", "\"keyword output\"",
  "\"keyword pin\"", "\"keyword pinfin\"", "\"keyword pitch\"",
  "\"keyword rate\"", "\"keyword side\"", "\"keywork sink\"",
  "\"keyword slot\"", "\"keyword solver\"", "\"keyword source\"",
  "\"keyword stack\"", "\"keyword staggered\"", "\"keyword state\"",
  "\"keyword steady\"", "\"keyword step\"", "\"keyword T\"",
  "\"keyword Tcoolant\"", "\"keyword temperature\"", "\"keyword Tflp\"",
  "\"keyword Tflpel\"", "\"keyword thermal\"", "\"keyword Tmap\"",
  "\"keyword top\"", "\"keyword transfer\"", "\"keyword transient\"",
  "\"keyword velocity\"", "\"keywork volumetric\"", "\"keyword wall\"",
  "\"keyword width\"", "\"double value\"", "\"identifier\"",
  "\"path to file\"", "':'", "';'", "','", "'('", "')'", "'.'", "$accept",
  "stack_description_file", "materials_list", "material",
  "conventional_heat_sink", "microchannel",
  "coolant_heat_transfer_coefficients_4rm",
  "coolant_heat_transfer_coefficients_2rm", "first_wall_length",
  "last_wall_length", "distribution", "layers_list", "layer",
  "source_layer", "layer_content", "dies_list", "die", "dimensions",
  "stack", "stack_elements", "stack_element", "solver",
  "initial_temperature", "inspection_points", "inspection_points_list",
  "inspection_point", "maxminavg", "when", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,    58,
      59,    44,    40,    41,    46
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    75,    76,    77,    77,    78,    79,    79,    80,    80,
      80,    80,    81,    81,    82,    82,    83,    83,    84,    84,
      85,    85,    86,    86,    87,    88,    89,    90,    90,    91,
      92,    93,    94,    94,    95,    95,    95,    96,    96,    97,
      98,    98,    99,    99,   100,   100,   100,   100,   100,   101,
     101,   101,   102,   102,   102,   102
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     9,     1,     2,    12,     0,    13,     0,    37,
      35,    36,     6,    13,     6,    10,     0,     5,     0,     5,
       1,     1,     0,     2,     2,     2,     3,     1,     2,     6,
      16,     3,     1,     2,     5,     3,     6,     4,     9,     4,
       0,     3,     1,     2,    12,    10,    12,     8,    10,     1,
       1,     1,     0,     2,     2,     2
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     6,     3,     0,     1,     0,     4,     8,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    27,     0,     0,     0,     0,     0,     0,     0,
      28,     0,     0,     0,     0,     0,     0,    22,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    23,    22,
       0,     0,     0,     0,    31,    32,     0,     0,    40,     0,
       0,     0,     0,     0,     0,    24,    25,    29,     0,     0,
       0,     0,    33,     0,     0,     0,     0,     2,     0,     0,
       0,     0,     0,     0,     0,    35,     0,     0,    37,     0,
       0,     0,     0,     0,     0,     0,     0,    26,     0,     0,
       0,     0,    39,     0,     0,     0,     0,     0,    41,    42,
       5,     0,     0,     0,     0,     0,     0,    34,     0,     0,
       0,     0,     0,     0,    43,     0,     0,     0,     0,     0,
      36,     0,     0,     0,     0,     0,     0,     7,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    38,     0,     0,     0,     0,    52,     0,    16,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      18,     0,     0,     0,    51,    49,    50,    52,    52,     0,
      55,    54,    53,     0,     0,     0,     0,     0,    20,    21,
       0,     0,     0,     0,     0,     0,    47,     0,     0,     0,
       0,     0,     0,    52,     0,     0,    52,     0,     0,     0,
       0,     0,    30,     0,    48,    45,     0,     0,    17,     0,
       0,     0,     0,     0,     0,    19,     0,     0,    44,    46,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    14,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    12,     0,
       0,     0,     0,     0,     0,     0,    15,     0,     0,     0,
       0,     0,     0,     0,     0,    10,     0,     0,     0,     0,
       0,    11,     0,     9,    13
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     2,     3,     4,     9,    14,   260,   250,   180,   197,
     200,    46,    58,    59,    75,    21,    22,    31,    40,    64,
      65,    50,    68,    87,   118,   119,   187,   177
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -180
static const yytype_int16 yypact[] =
{
     -13,   -40,    29,   -10,  -180,   -29,  -180,    17,  -180,   -23,
     -14,     1,    10,   -22,    27,    33,   -21,   -20,   -19,    25,
     -15,     0,  -180,   -12,    28,    30,    31,   -11,    -9,    -8,
    -180,    11,    -7,    -1,    -2,     2,    -5,  -180,    51,     3,
      21,     4,    57,     5,     6,    32,   -25,    39,    -3,     8,
      45,    49,    12,    69,    70,    64,    16,    16,  -180,  -180,
      18,    19,    20,    22,    -3,  -180,   -39,    34,    46,    77,
      23,    58,    59,    26,    35,  -180,  -180,    63,    24,    36,
      37,    41,  -180,    38,    47,    43,    42,  -180,    44,    91,
      48,    50,    52,    53,    40,  -180,    74,    54,  -180,    60,
      55,   -17,    61,    65,    62,    66,    75,  -180,    67,    56,
      68,    71,  -180,    72,    73,    76,    78,    79,   -17,  -180,
    -180,    80,    83,    85,    87,    82,    84,  -180,    86,    88,
      89,    90,    92,    93,  -180,    94,    96,    97,    95,   103,
    -180,    99,    98,   100,   101,   102,   104,  -180,   107,   108,
     109,   105,   110,   111,   113,   114,   116,   117,   118,   119,
     115,   112,  -180,   120,   121,   122,   123,   124,   106,    81,
     138,   125,   131,    -4,    -4,   130,   -18,   126,   129,   136,
     133,   -24,   137,   132,  -180,  -180,  -180,   124,   124,   134,
    -180,  -180,  -180,   139,   140,   135,   142,   144,  -180,  -180,
     141,   146,   145,   128,   143,    -4,  -180,   147,   148,   152,
     153,   149,   150,   124,   151,   154,   124,   175,   155,   156,
     159,   171,  -180,   157,  -180,  -180,   158,   186,  -180,   162,
     163,   160,   164,   165,   127,  -180,   200,   166,  -180,  -180,
     172,   194,   203,   167,   182,   177,   213,   174,   176,   204,
     226,   173,   178,   184,   183,   230,   232,   237,   225,   227,
     236,   190,   -49,   246,   195,   193,   233,   191,   189,   196,
     248,   238,   253,   197,  -180,   199,   -41,   256,   201,   258,
     251,   205,   202,   207,   206,   208,   243,   209,  -180,   211,
     260,   212,   223,   219,   264,   257,  -180,   217,   220,   261,
     231,   221,   216,   235,   224,  -180,   285,   228,   229,   234,
     239,  -180,   240,  -180,  -180
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -180,  -180,  -180,   290,  -180,  -180,  -180,  -180,  -180,  -180,
    -180,   242,  -180,  -180,   241,  -180,   274,  -180,  -180,  -180,
     244,  -180,  -180,  -180,  -180,   179,  -174,  -179
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint16 yytable[] =
{
     188,   281,   184,   190,     7,   198,    56,    61,   203,   204,
     267,    83,    12,    17,    18,    62,    13,   268,    20,    29,
       1,    57,    84,     1,   199,   282,   191,     5,    63,     6,
     185,   216,   186,   192,   223,   113,   114,   226,   115,   116,
      10,   117,    11,    15,    16,    20,    23,    19,    24,    25,
      26,    27,    28,    33,    32,    36,    34,    35,    39,    42,
      37,    38,    47,    41,    43,    45,    49,    51,    44,    52,
      55,    60,    48,    67,    69,    53,    54,    66,    70,    71,
      72,    73,    74,    86,    78,    88,    79,    80,    85,    81,
      90,    91,    92,    89,    56,    94,   103,   109,    99,     0,
       0,     0,    93,   179,    96,   108,    95,    97,    98,   100,
     102,   101,   151,   124,   104,     0,   105,     0,     0,   121,
       0,   110,   106,   107,   126,   112,   111,   138,   148,   149,
     141,   120,   122,   125,     0,     0,   123,   161,   127,     0,
       0,     0,   128,     0,   129,   130,   135,   136,   131,   137,
     132,   133,   139,   170,   140,   142,   143,   144,   181,   145,
     146,   150,   194,   196,   147,   152,     0,   208,   240,   153,
     178,   154,   155,   158,   159,   157,   156,   163,   171,   160,
     162,   164,   165,   166,   219,   167,   220,   221,   168,   169,
     227,   172,   173,   174,   175,   176,   182,   183,   189,   193,
     195,   214,   201,   202,   231,   205,   209,   207,   210,   206,
     234,   211,   212,   213,   218,   241,   215,   217,   244,   245,
     222,   224,   229,   247,   225,   228,   230,   237,   249,   253,
     232,   233,   235,   236,   238,   239,   242,   246,   243,   248,
     251,   254,   252,   255,   257,   259,   258,   261,   256,   262,
     263,   265,   264,   266,   269,   270,   271,   273,   272,   274,
     276,   278,   275,   277,   283,   285,   286,   284,   279,   280,
     292,   287,   288,   289,   291,   295,   290,   297,   298,   299,
     293,   294,   296,   301,   300,   304,   302,   306,   303,   307,
     308,   305,   309,     8,   310,    30,     0,   134,    76,   311,
     312,    77,     0,     0,     0,     0,     0,     0,    82,   313,
     314
};

static const yytype_int16 yycheck[] =
{
     174,    42,     6,    21,    14,    29,    31,    10,   187,   188,
      59,    50,    35,     3,     4,    18,    39,    66,    18,    19,
      33,    46,    61,    33,    48,    66,    44,    67,    31,     0,
      34,   205,    36,    51,   213,    52,    53,   216,    55,    56,
      69,    58,    25,    57,    43,    18,    13,    69,    69,    69,
      69,    26,    67,    25,    66,    66,    26,    26,    47,    60,
      69,    69,    11,    70,    66,    70,    45,    63,    66,    12,
      38,    32,    69,    28,    25,    70,    70,    69,    66,    10,
      10,    17,    66,    37,    66,     8,    67,    67,    54,    67,
      32,    32,    66,    70,    31,    71,     5,    23,    51,    -1,
      -1,    -1,    67,    22,    67,    65,    70,    66,    70,    66,
      66,    69,     9,    38,    66,    -1,    66,    -1,    -1,    54,
      -1,    67,    70,    70,    68,    70,    66,    40,    32,    32,
      44,    70,    70,    66,    -1,    -1,    70,    32,    70,    -1,
      -1,    -1,    71,    -1,    72,    72,    66,    64,    72,    64,
      72,    72,    70,    38,    70,    67,    67,    67,    20,    67,
      67,    66,    33,    30,    70,    66,    -1,    32,    41,    71,
      64,    71,    71,    66,    66,    71,    74,    66,    66,    70,
      70,    68,    68,    67,    32,    68,    33,    38,    70,    70,
      15,    71,    71,    71,    71,    71,    71,    66,    68,    73,
      64,    73,    65,    71,    33,    71,    64,    67,    64,    70,
      24,    70,    66,    68,    66,    15,    73,    70,    24,    16,
      70,    70,    66,    41,    70,    70,    67,    67,    15,    25,
      73,    73,    70,    70,    70,    70,    70,    70,    66,    62,
      66,    15,    66,    70,    60,    15,    63,    15,    70,    12,
      25,    15,    25,    63,     8,    60,    63,    66,    25,    70,
      12,     8,    66,    25,     8,     7,    15,    66,    71,    70,
      27,    66,    70,    66,    66,    15,    70,    54,    59,    15,
      71,    70,    70,    66,    27,    54,    66,    71,    27,    54,
      66,    70,     7,     3,    66,    21,    -1,   118,    57,    70,
      66,    59,    -1,    -1,    -1,    -1,    -1,    -1,    64,    70,
      70
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    33,    76,    77,    78,    67,     0,    14,    78,    79,
      69,    25,    35,    39,    80,    57,    43,     3,     4,    69,
      18,    90,    91,    13,    69,    69,    69,    26,    67,    19,
      91,    92,    66,    25,    26,    26,    66,    69,    69,    47,
      93,    70,    60,    66,    66,    70,    86,    11,    69,    45,
      96,    63,    12,    70,    70,    38,    31,    46,    87,    88,
      32,    10,    18,    31,    94,    95,    69,    28,    97,    25,
      66,    10,    10,    17,    66,    89,    89,    86,    66,    67,
      67,    67,    95,    50,    61,    54,    37,    98,     8,    70,
      32,    32,    66,    67,    71,    70,    67,    66,    70,    51,
      66,    69,    66,     5,    66,    66,    70,    70,    65,    23,
      67,    66,    70,    52,    53,    55,    56,    58,    99,   100,
      70,    54,    70,    70,    38,    66,    68,    70,    71,    72,
      72,    72,    72,    72,   100,    66,    64,    64,    40,    70,
      70,    44,    67,    67,    67,    67,    67,    70,    32,    32,
      66,     9,    66,    71,    71,    71,    74,    71,    66,    66,
      70,    32,    70,    66,    68,    68,    67,    68,    70,    70,
      38,    66,    71,    71,    71,    71,    71,   102,    64,    22,
      83,    20,    71,    66,     6,    34,    36,   101,   101,    68,
      21,    44,    51,    73,    33,    64,    30,    84,    29,    48,
      85,    65,    71,   102,   102,    71,    70,    67,    32,    64,
      64,    70,    66,    68,    73,    73,   101,    70,    66,    32,
      33,    38,    70,   102,    70,    70,   102,    15,    70,    66,
      67,    33,    73,    73,    24,    70,    70,    67,    70,    70,
      41,    15,    70,    66,    24,    16,    70,    41,    62,    15,
      82,    66,    66,    25,    15,    70,    70,    60,    63,    15,
      81,    15,    12,    25,    25,    15,    63,    59,    66,     8,
      60,    63,    25,    66,    70,    66,    12,    25,     8,    71,
      70,    42,    66,     8,    66,     7,    15,    66,    70,    66,
      70,    66,    27,    71,    70,    15,    70,    54,    59,    15,
      27,    66,    66,    27,    54,    70,    71,    54,    66,     7,
      66,    70,    66,    70,    70
};

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
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (stkd, analysis, scanner, YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (&yylval, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval, scanner)
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
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value, stkd, analysis, scanner); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, StackDescription *stkd, Analysis         *analysis, yyscan_t          scanner)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, stkd, analysis, scanner)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    StackDescription *stkd;
    Analysis         *analysis;
    yyscan_t          scanner;
#endif
{
  if (!yyvaluep)
    return;
  YYUSE (stkd);
  YYUSE (analysis);
  YYUSE (scanner);
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, StackDescription *stkd, Analysis         *analysis, yyscan_t          scanner)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, stkd, analysis, scanner)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    StackDescription *stkd;
    Analysis         *analysis;
    yyscan_t          scanner;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep, stkd, analysis, scanner);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule, StackDescription *stkd, Analysis         *analysis, yyscan_t          scanner)
#else
static void
yy_reduce_print (yyvsp, yyrule, stkd, analysis, scanner)
    YYSTYPE *yyvsp;
    int yyrule;
    StackDescription *stkd;
    Analysis         *analysis;
    yyscan_t          scanner;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       , stkd, analysis, scanner);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule, stkd, analysis, scanner); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
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
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, StackDescription *stkd, Analysis         *analysis, yyscan_t          scanner)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, stkd, analysis, scanner)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    StackDescription *stkd;
    Analysis         *analysis;
    yyscan_t          scanner;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (stkd);
  YYUSE (analysis);
  YYUSE (scanner);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {
      case 67: /* "\"identifier\"" */

/* Line 1000 of yacc.c  */
#line 185 "stack_description_parser.y"
	{ FREE_POINTER (free,                     (yyvaluep->string)) ; };

/* Line 1000 of yacc.c  */
#line 1428 "stack_description_parser.c"
	break;
      case 68: /* "\"path to file\"" */

/* Line 1000 of yacc.c  */
#line 185 "stack_description_parser.y"
	{ FREE_POINTER (free,                     (yyvaluep->string)) ; };

/* Line 1000 of yacc.c  */
#line 1437 "stack_description_parser.c"
	break;
      case 86: /* "layers_list" */

/* Line 1000 of yacc.c  */
#line 186 "stack_description_parser.y"
	{ FREE_POINTER (free_layers_list,         (yyvaluep->layer_p)) ; };

/* Line 1000 of yacc.c  */
#line 1446 "stack_description_parser.c"
	break;
      case 87: /* "layer" */

/* Line 1000 of yacc.c  */
#line 186 "stack_description_parser.y"
	{ FREE_POINTER (free_layers_list,         (yyvaluep->layer_p)) ; };

/* Line 1000 of yacc.c  */
#line 1455 "stack_description_parser.c"
	break;
      case 88: /* "source_layer" */

/* Line 1000 of yacc.c  */
#line 186 "stack_description_parser.y"
	{ FREE_POINTER (free_layers_list,         (yyvaluep->layer_p)) ; };

/* Line 1000 of yacc.c  */
#line 1464 "stack_description_parser.c"
	break;
      case 89: /* "layer_content" */

/* Line 1000 of yacc.c  */
#line 186 "stack_description_parser.y"
	{ FREE_POINTER (free_layers_list,         (yyvaluep->layer_p)) ; };

/* Line 1000 of yacc.c  */
#line 1473 "stack_description_parser.c"
	break;

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (StackDescription *stkd, Analysis         *analysis, yyscan_t          scanner);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */





/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (StackDescription *stkd, Analysis         *analysis, yyscan_t          scanner)
#else
int
yyparse (stkd, analysis, scanner)
    StackDescription *stkd;
    Analysis         *analysis;
    yyscan_t          scanner;
#endif
#endif
{
/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

/* User initialization code.  */

/* Line 1242 of yacc.c  */
#line 201 "stack_description_parser.y"
{
    first_wall_length = 0.0 ;
    last_wall_length  = 0.0 ;
    wall_length       = 0.0 ;
    channel_length    = 0.0 ;
    num_channels      = 0u ;
    num_dies          = 0u ;
}

/* Line 1242 of yacc.c  */
#line 1614 "stack_description_parser.c"

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
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

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

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
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
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

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

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
        case 3:

/* Line 1455 of yacc.c  */
#line 238 "stack_description_parser.y"
    {
        stkd->MaterialsList = (yyvsp[(1) - (1)].material_p) ;

        (yyval.material_p) = (yyvsp[(1) - (1)].material_p) ;           // $1 will be the new last element in the list
    ;}
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 245 "stack_description_parser.y"
    {

        if (find_material_in_list (stkd->MaterialsList, (yyvsp[(2) - (2)].material_p)->Id) != NULL)
        {
            sprintf (error_message, "Material %s already declared", (yyvsp[(2) - (2)].material_p)->Id) ;

            FREE_POINTER (free_material, (yyvsp[(2) - (2)].material_p)) ;

            stack_description_error (stkd, analysis, scanner, error_message) ;

            YYABORT ;
        }

        (yyvsp[(1) - (2)].material_p)->Next = (yyvsp[(2) - (2)].material_p) ;     // insert $2 at the end of the list
        (yyval.material_p) = (yyvsp[(2) - (2)].material_p) ;           // $2 will be the new last element in the list
    ;}
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 268 "stack_description_parser.y"
    {
        Material *material = (yyval.material_p) = alloc_and_init_material () ;

        if (material == NULL)
        {
            FREE_POINTER (free, (yyvsp[(2) - (12)].string)) ;

            stack_description_error (stkd, analysis, scanner, "Malloc material failed") ;

            YYABORT ;
        }

        material->Id                     = (yyvsp[(2) - (12)].string) ;
        material->ThermalConductivity    = (SolidTC_t) (yyvsp[(6) - (12)].double_v) ;
        material->VolumetricHeatCapacity = (SolidVHC_t) (yyvsp[(11) - (12)].double_v) ;
    ;}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 297 "stack_description_parser.y"
    {
        stkd->ConventionalHeatSink = alloc_and_init_conventional_heat_sink () ;

        if (stkd->ConventionalHeatSink == NULL)
        {
            stack_description_error (stkd, analysis, scanner, "Malloc conventional heat sink failed") ;

            YYABORT ;
        }

        stkd->ConventionalHeatSink->AmbientHTC         = (AmbientHTC_t) (yyvsp[(8) - (13)].double_v) ;
        stkd->ConventionalHeatSink->AmbientTemperature = (yyvsp[(12) - (13)].double_v) ;
    ;}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 331 "stack_description_parser.y"
    {
        stkd->Channel = alloc_and_init_channel () ;

        if (stkd->Channel == NULL)
        {
            FREE_POINTER (free, (yyvsp[(19) - (37)].string)) ;

            stack_description_error (stkd, analysis, scanner, "Malloc channel failed") ;

            YYABORT ;
        }

        channel_length    = (yyvsp[(9) - (37)].double_v) ;
        wall_length       = (yyvsp[(13) - (37)].double_v) ;
        first_wall_length = ((yyvsp[(15) - (37)].double_v) != 0.0) ? (yyvsp[(15) - (37)].double_v) : (yyvsp[(13) - (37)].double_v) ;
        last_wall_length  = ((yyvsp[(16) - (37)].double_v) != 0.0) ? (yyvsp[(16) - (37)].double_v) : (yyvsp[(13) - (37)].double_v) ;

        stkd->Channel->ChannelModel      = TDICE_CHANNEL_MODEL_MC_4RM ;
        stkd->Channel->NLayers           = NUM_LAYERS_CHANNEL_4RM ;
        stkd->Channel->SourceLayerOffset = SOURCE_OFFSET_CHANNEL_4RM ;
        stkd->Channel->Height            = (yyvsp[(5) - (37)].double_v) ;
        stkd->Channel->Coolant.FlowRate  = FLOW_RATE_FROM_MLMIN_TO_UM3SEC ((yyvsp[(24) - (37)].double_v)) ;
        stkd->Channel->Coolant.HTCSide   = (yyvsp[(26) - (37)].coolant_v).HTCSide ;
        stkd->Channel->Coolant.HTCTop    = (yyvsp[(26) - (37)].coolant_v).HTCTop ;
        stkd->Channel->Coolant.HTCBottom = (yyvsp[(26) - (37)].coolant_v).HTCBottom ;
        stkd->Channel->Coolant.VHC       = (yyvsp[(31) - (37)].double_v) ;
        stkd->Channel->Coolant.TIn       = (yyvsp[(36) - (37)].double_v) ;
        stkd->Channel->WallMaterial      = find_material_in_list (stkd->MaterialsList, (yyvsp[(19) - (37)].string)) ;

        if (stkd->Channel->WallMaterial == NULL)
        {
            sprintf (error_message, "Unknown material %s", (yyvsp[(19) - (37)].string)) ;

            FREE_POINTER (free, (yyvsp[(19) - (37)].string)) ;

            stack_description_error (stkd, analysis, scanner, error_message) ;

            YYABORT ;
        }

        stkd->Channel->WallMaterial->Used++ ;

        FREE_POINTER (free, (yyvsp[(19) - (37)].string)) ;
    ;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 385 "stack_description_parser.y"
    {
        stkd->Channel = alloc_and_init_channel () ;

        if (stkd->Channel == NULL)
        {
            FREE_POINTER (free, (yyvsp[(17) - (35)].string)) ;

            stack_description_error (stkd, analysis, scanner, "Malloc channel failed") ;

            YYABORT ;
        }

        stkd->Channel->ChannelModel      = TDICE_CHANNEL_MODEL_MC_2RM ;
        stkd->Channel->NLayers           = NUM_LAYERS_CHANNEL_2RM ;
        stkd->Channel->SourceLayerOffset = SOURCE_OFFSET_CHANNEL_2RM ;
        stkd->Channel->Height            = (yyvsp[(5) - (35)].double_v) ;
        stkd->Channel->Length            = (yyvsp[(9) - (35)].double_v) ;
        stkd->Channel->Pitch             = (yyvsp[(13) - (35)].double_v) + (yyvsp[(9) - (35)].double_v) ;
        stkd->Channel->Porosity          = stkd->Channel->Length / stkd->Channel->Pitch ;
        stkd->Channel->Coolant.FlowRate  = FLOW_RATE_FROM_MLMIN_TO_UM3SEC ((yyvsp[(22) - (35)].double_v)) ;
        stkd->Channel->Coolant.HTCSide   = (yyvsp[(24) - (35)].coolant_v).HTCSide ;
        stkd->Channel->Coolant.HTCTop    = (yyvsp[(24) - (35)].coolant_v).HTCTop ;
        stkd->Channel->Coolant.HTCBottom = (yyvsp[(24) - (35)].coolant_v).HTCBottom ;
        stkd->Channel->Coolant.VHC       = (yyvsp[(29) - (35)].double_v) ;
        stkd->Channel->Coolant.TIn       = (yyvsp[(34) - (35)].double_v) ;
        stkd->Channel->WallMaterial      = find_material_in_list (stkd->MaterialsList, (yyvsp[(17) - (35)].string)) ;

        if (stkd->Channel->WallMaterial == NULL)
        {
            sprintf (error_message, "Unknown material %s", (yyvsp[(17) - (35)].string)) ;

            FREE_POINTER (free, (yyvsp[(17) - (35)].string)) ;

            stack_description_error (stkd, analysis, scanner, error_message) ;

            YYABORT ;
        }

        stkd->Channel->WallMaterial->Used++ ;

        FREE_POINTER (free, (yyvsp[(17) - (35)].string)) ;
    ;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 437 "stack_description_parser.y"
    {
        stkd->Channel = alloc_and_init_channel () ;

        if (stkd->Channel == NULL)
        {
            FREE_POINTER (free, (yyvsp[(20) - (36)].string)) ;

            stack_description_error (stkd, analysis, scanner, "Malloc channel failed") ;

            YYABORT ;
        }

        stkd->Channel->Height                = (yyvsp[(4) - (36)].double_v) ;
        stkd->Channel->Porosity              = POROSITY ((yyvsp[(8) - (36)].double_v), (yyvsp[(12) - (36)].double_v)) ;
        stkd->Channel->Pitch                 = (yyvsp[(12) - (36)].double_v) ;
        stkd->Channel->ChannelModel          = (yyvsp[(16) - (36)].channel_model_v) ;
        stkd->Channel->NLayers               = NUM_LAYERS_CHANNEL_2RM ;
        stkd->Channel->SourceLayerOffset     = SOURCE_OFFSET_CHANNEL_2RM ;
        stkd->Channel->Coolant.DarcyVelocity = (yyvsp[(24) - (36)].double_v) ;
        stkd->Channel->Coolant.HTCSide       = 0.0 ;
        stkd->Channel->Coolant.HTCTop        = 0.0 ;
        stkd->Channel->Coolant.HTCBottom     = 0.0 ;
        stkd->Channel->Coolant.VHC           = (yyvsp[(30) - (36)].double_v) ;
        stkd->Channel->Coolant.TIn           = (yyvsp[(35) - (36)].double_v) ;
        stkd->Channel->WallMaterial          = find_material_in_list (stkd->MaterialsList, (yyvsp[(20) - (36)].string)) ;

        if (stkd->Channel->WallMaterial == NULL)
        {
            sprintf (error_message, "Unknown material %s", (yyvsp[(20) - (36)].string)) ;

            FREE_POINTER (free, (yyvsp[(20) - (36)].string)) ;

            stack_description_error (stkd, analysis, scanner, error_message) ;

            YYABORT ;
        }

        stkd->Channel->WallMaterial->Used++ ;

        FREE_POINTER (free, (yyvsp[(20) - (36)].string)) ;
    ;}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 483 "stack_description_parser.y"
    {
        (yyval.coolant_v).HTCSide   = (yyvsp[(5) - (6)].double_v) ;
        (yyval.coolant_v).HTCTop    = (yyvsp[(5) - (6)].double_v) ;
        (yyval.coolant_v).HTCBottom = (yyvsp[(5) - (6)].double_v) ;
    ;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 491 "stack_description_parser.y"
    {
        (yyval.coolant_v).HTCSide   = (yyvsp[(6) - (13)].double_v) ;
        (yyval.coolant_v).HTCTop    = (yyvsp[(9) - (13)].double_v) ;
        (yyval.coolant_v).HTCBottom = (yyvsp[(12) - (13)].double_v) ;
    ;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 501 "stack_description_parser.y"
    {
        (yyval.coolant_v).HTCSide   = 0.0 ;
        (yyval.coolant_v).HTCTop    = (yyvsp[(5) - (6)].double_v) ;
        (yyval.coolant_v).HTCBottom = (yyvsp[(5) - (6)].double_v) ;
    ;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 508 "stack_description_parser.y"
    {
        (yyval.coolant_v).HTCSide   = 0.0 ;
        (yyval.coolant_v).HTCTop    = (yyvsp[(6) - (10)].double_v) ;
        (yyval.coolant_v).HTCBottom = (yyvsp[(9) - (10)].double_v) ;
    ;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 516 "stack_description_parser.y"
    { (yyval.double_v) = 0.0 ; ;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 517 "stack_description_parser.y"
    { (yyval.double_v) = (yyvsp[(4) - (5)].double_v) ;  ;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 521 "stack_description_parser.y"
    { (yyval.double_v) = 0.0 ; ;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 522 "stack_description_parser.y"
    { (yyval.double_v) = (yyvsp[(4) - (5)].double_v) ;  ;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 526 "stack_description_parser.y"
    { (yyval.channel_model_v) = TDICE_CHANNEL_MODEL_PF_INLINE ;    ;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 527 "stack_description_parser.y"
    { (yyval.channel_model_v) = TDICE_CHANNEL_MODEL_PF_STAGGERED ; ;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 537 "stack_description_parser.y"
    {
        (yyval.layer_p) = NULL ;    // The first layer in the list will be null
    ;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 542 "stack_description_parser.y"
    {
        if ((yyvsp[(1) - (2)].layer_p) != NULL)
            JOIN_ELEMENTS ((yyvsp[(2) - (2)].layer_p), (yyvsp[(1) - (2)].layer_p)) ; // this reverse the order !
        (yyval.layer_p) = (yyvsp[(2) - (2)].layer_p) ;                    // $2 will be the new reference to the list
    ;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 549 "stack_description_parser.y"
    { (yyval.layer_p) = (yyvsp[(2) - (2)].layer_p) ; ;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 551 "stack_description_parser.y"
    { (yyval.layer_p) = (yyvsp[(2) - (2)].layer_p) ; ;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 557 "stack_description_parser.y"
    {
        Layer *layer = (yyval.layer_p) = alloc_and_init_layer () ;

        if (layer == NULL)
        {
            FREE_POINTER (free, (yyvsp[(2) - (3)].string)) ;

            stack_description_error (stkd, analysis, scanner, "Malloc layer failed") ;

            YYABORT ;
        }

        layer->Height   = (yyvsp[(1) - (3)].double_v) ;
        layer->Material = find_material_in_list (stkd->MaterialsList, (yyvsp[(2) - (3)].string)) ;

        if (layer->Material == NULL)
        {
            sprintf (error_message, "Unknown material %s", (yyvsp[(2) - (3)].string)) ;

            FREE_POINTER (free,       (yyvsp[(2) - (3)].string)) ;
            FREE_POINTER (free_layer, layer) ;

            stack_description_error (stkd, analysis, scanner, error_message) ;

            YYABORT ;
        }

        layer->Material->Used++ ;

        FREE_POINTER (free, (yyvsp[(2) - (3)].string)) ;
    ;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 597 "stack_description_parser.y"
    {
        stkd->DiesList = (yyvsp[(1) - (1)].die_p) ;
        (yyval.die_p) = (yyvsp[(1) - (1)].die_p) ;         // $1 will be the new die in the list
    ;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 603 "stack_description_parser.y"
    {

        if (find_die_in_list (stkd->DiesList, (yyvsp[(2) - (2)].die_p)->Id) != NULL)
        {
            sprintf (error_message, "Die %s already declared", (yyvsp[(2) - (2)].die_p)->Id) ;

            FREE_POINTER (free_die, (yyvsp[(2) - (2)].die_p)) ;

            stack_description_error (stkd, analysis, scanner, error_message) ;

            YYABORT ;
        }

        (yyvsp[(1) - (2)].die_p)->Next = (yyvsp[(2) - (2)].die_p) ;   // insert $2 at the end of the list
        (yyval.die_p) = (yyvsp[(2) - (2)].die_p) ;         // $2 will be the new last element in the list
    ;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 627 "stack_description_parser.y"
    {
        Die *die = (yyval.die_p) = alloc_and_init_die () ;

        if (die == NULL)
        {
            FREE_POINTER (free,             (yyvsp[(2) - (6)].string)) ;
            FREE_POINTER (free_layers_list, (yyvsp[(4) - (6)].layer_p)) ;
            FREE_POINTER (free_layer,       (yyvsp[(5) - (6)].layer_p)) ;
            FREE_POINTER (free_layers_list, (yyvsp[(6) - (6)].layer_p));

            stack_description_error (stkd, analysis, scanner, "Malloc die failed") ;

            YYABORT ;
        }

        die->Id = (yyvsp[(2) - (6)].string) ;
        die->SourceLayer = (yyvsp[(5) - (6)].layer_p) ;

        // The layers within a die are declared in the stack file from top
        // to bottom but here we revert the order: the first layer in the list
        // LayersList will be the bottom-most layer in the die

        if ((yyvsp[(6) - (6)].layer_p) != NULL)
        {
            // if there are layers below the source,
            // then the list of layers begins with $6

            die->BottomLayer = (yyvsp[(6) - (6)].layer_p) ;

            die->NLayers++ ;
            die->SourceLayerOffset++ ;

            // $6 moved until the end ..

            while ((yyvsp[(6) - (6)].layer_p)->Next != NULL)
            {
                (yyvsp[(6) - (6)].layer_p) = (yyvsp[(6) - (6)].layer_p)->Next ;

                die->NLayers++ ;
                die->SourceLayerOffset++ ;
            }

            // the list $6 continues with the source layer $5

            JOIN_ELEMENTS ((yyvsp[(6) - (6)].layer_p), (yyvsp[(5) - (6)].layer_p)) ;
        }
        else
        {
            // if there aren't layers below the source, the list of layers
            // begins directly with the source layer $5

            die->BottomLayer = (yyvsp[(5) - (6)].layer_p) ;
        }

        die->NLayers++ ;

        if ((yyvsp[(4) - (6)].layer_p) != NULL)
        {
            // if there are layers above the source
            // $5 is connected to the list $4

            JOIN_ELEMENTS ((yyvsp[(5) - (6)].layer_p), (yyvsp[(4) - (6)].layer_p)) ;

            die->NLayers++ ;

            // $4 moved until the end ..

            while ((yyvsp[(4) - (6)].layer_p)->Next != NULL)
            {
                (yyvsp[(4) - (6)].layer_p) = (yyvsp[(4) - (6)].layer_p)->Next ;

                die->NLayers++ ;
            }

            // the list finishes with the last layer in $4

            die->TopLayer = (yyvsp[(4) - (6)].layer_p) ;
        }
        else
        {
            // if there aren't layers below the source,
            // The list finishes with the source layer $5

            die->TopLayer = (yyvsp[(5) - (6)].layer_p) ;
        }
    ;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 724 "stack_description_parser.y"
    {
        stkd->Dimensions = alloc_and_init_dimensions () ;

        if (stkd->Dimensions == NULL)
        {
            stack_description_error (stkd, analysis, scanner, "Malloc dimensions failed") ;
            YYABORT ;
        }

        stkd->Dimensions->Chip.Length = (yyvsp[(5) - (16)].double_v) ;
        stkd->Dimensions->Chip.Width  = (yyvsp[(8) - (16)].double_v) ;

        stkd->Dimensions->Cell.ChannelLength   = (yyvsp[(12) - (16)].double_v) ;
        stkd->Dimensions->Cell.FirstWallLength = (yyvsp[(12) - (16)].double_v) ;
        stkd->Dimensions->Cell.LastWallLength  = (yyvsp[(12) - (16)].double_v) ;
        stkd->Dimensions->Cell.WallLength      = (yyvsp[(12) - (16)].double_v) ;

        stkd->Dimensions->Cell.Width  = (yyvsp[(15) - (16)].double_v) ;

        stkd->Dimensions->Grid.NRows    = ((yyvsp[(8) - (16)].double_v) / (yyvsp[(15) - (16)].double_v)) ;
        stkd->Dimensions->Grid.NColumns = ((yyvsp[(5) - (16)].double_v) / (yyvsp[(12) - (16)].double_v)) ;


        if (stkd->Channel != NULL)
        {
            if (stkd->Channel->ChannelModel == TDICE_CHANNEL_MODEL_MC_4RM)
            {
                stkd->Dimensions->Cell.ChannelLength   = channel_length ;
                stkd->Dimensions->Cell.FirstWallLength = first_wall_length ;
                stkd->Dimensions->Cell.LastWallLength  = last_wall_length ;
                stkd->Dimensions->Cell.WallLength      = wall_length ;

                CellDimension_t ratio
                    = ((yyvsp[(5) - (16)].double_v) - first_wall_length - last_wall_length -channel_length)
                    /
                    (channel_length + wall_length) ;

                if ( ratio - (int) ratio != 0)
                {
                    stack_description_error (stkd, analysis, scanner, "Error: cell dimensions does not fit the chip length correctly") ;

                    YYABORT ;
                }

                stkd->Dimensions->Grid.NColumns = 2 * ratio + 3 ;

                if ((stkd->Dimensions->Grid.NColumns & 1) == 0)
                {
                    stack_description_error (stkd, analysis, scanner, "Error: colum number must be odd when channel is declared") ;

                    YYABORT ;
                }

                // Check the number of columns

                if (stkd->Dimensions->Grid.NColumns < 3)
                {
                    stack_description_error (stkd, analysis, scanner, "Error: not enough columns") ;

                    YYABORT ;
                }

                stkd->Channel->NChannels = ((stkd->Dimensions->Grid.NColumns - 1 )  / 2) ;
            }
            else if (stkd->Channel->ChannelModel == TDICE_CHANNEL_MODEL_MC_2RM)
            {
                stkd->Channel->NChannels = (((yyvsp[(5) - (16)].double_v) / stkd->Channel->Pitch) + 0.5) ; // round function
            }
        }
    ;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 804 "stack_description_parser.y"
    {
        if (num_dies == 0u)
        {
            stack_description_error (stkd, analysis, scanner, "Error: stack must contain at least one die") ;

            YYABORT ;
        }

        if (stkd->BottomStackElement->Type == TDICE_STACK_ELEMENT_CHANNEL)
        {
            stack_description_error (stkd, analysis, scanner, "Error: cannot declare a channel as bottom-most stack element") ;

            YYABORT ;
        }

        if (stkd->ConventionalHeatSink == NULL && stkd->Channel == NULL)

            fprintf (stderr, "Warning: neither heat sink nor channel has been declared\n") ;


        FOR_EVERY_ELEMENT_IN_LIST_NEXT (Material, material, stkd->MaterialsList)

            if (material->Used == 0)

                fprintf (stderr, "Warning: material %s declared but not used\n", material->Id) ;


        FOR_EVERY_ELEMENT_IN_LIST_NEXT (Die, die, stkd->DiesList)

            if (die->Used == 0)

                fprintf (stderr, "Warning: die %s declared but not used\n", die->Id) ;

        if (stkd->ConventionalHeatSink != NULL)
        {
            if (stkd->TopStackElement->Type == TDICE_STACK_ELEMENT_LAYER)
            {
                stkd->ConventionalHeatSink->TopLayer = stkd->TopStackElement->Pointer.Layer ;
            }
            else
            {
                stkd->ConventionalHeatSink->TopLayer = stkd->TopStackElement->Pointer.Die->TopLayer ;
            }
        }

        // Counts the number of layers and fix the layer offset starting from
        // the bottom most element in the stack. This operation can be done only
        // here since the parser processes elements in the stack from the top most.

        CellIndex_t layer_index = 0u ;

        FOR_EVERY_ELEMENT_IN_LIST_NEXT (StackElement, stk_el, stkd->BottomStackElement)
        {
            stk_el->Offset = layer_index ;
            layer_index   += stk_el->NLayers ;
        }

        stkd->Dimensions->Grid.NLayers = layer_index ;

        // Evaluate the number of cells and nonzero elements

        stkd->Dimensions->Grid.NCells
            =   get_number_of_layers (stkd->Dimensions)
                * get_number_of_rows (stkd->Dimensions)
                * get_number_of_columns (stkd->Dimensions) ;

        if (stkd->Dimensions->Grid.NCells >  INT32_MAX)
        {
            sprintf (error_message, "%d are too many cells ... (SuperLU uses 'int')", stkd->Dimensions->Grid.NCells) ;

            stack_description_error (stkd, analysis, scanner, error_message) ;

            YYABORT ;
        }

        if (stkd->Channel == NULL)

            compute_number_of_connections

                (stkd->Dimensions, num_channels, TDICE_CHANNEL_MODEL_NONE) ;

        else

            compute_number_of_connections

                (stkd->Dimensions, num_channels, stkd->Channel->ChannelModel) ;

    ;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 897 "stack_description_parser.y"
    {
        if (   stkd->TopStackElement == NULL && (yyvsp[(1) - (1)].stack_element_p)->Type == TDICE_STACK_ELEMENT_CHANNEL)
        {
            stack_description_error (stkd, analysis, scanner, "Error: cannot declare a channel as top-most stack element") ;

            YYABORT ;
        }

        stkd->TopStackElement    = (yyvsp[(1) - (1)].stack_element_p) ;
        stkd->BottomStackElement = (yyvsp[(1) - (1)].stack_element_p) ;
        (yyval.stack_element_p) = (yyvsp[(1) - (1)].stack_element_p) ;
    ;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 911 "stack_description_parser.y"
    {
        if (find_stack_element_in_list ((yyvsp[(1) - (2)].stack_element_p), (yyvsp[(2) - (2)].stack_element_p)->Id) != NULL)
        {
            sprintf (error_message, "Stack element %s already declared", (yyvsp[(2) - (2)].stack_element_p)->Id) ;

            FREE_POINTER (free_stack_element, (yyvsp[(2) - (2)].stack_element_p)) ;

            stack_description_error (stkd, analysis, scanner, error_message) ;

            YYABORT ;
        }

        if (   (yyvsp[(1) - (2)].stack_element_p)->Type == TDICE_STACK_ELEMENT_CHANNEL
            && (yyvsp[(2) - (2)].stack_element_p)->Type == TDICE_STACK_ELEMENT_CHANNEL)
        {
            stack_description_error (stkd, analysis, scanner, "Error: cannot declare two consecutive channels") ;

            YYABORT ;
        }

        JOIN_ELEMENTS ((yyvsp[(2) - (2)].stack_element_p), (yyvsp[(1) - (2)].stack_element_p)) ;

        stkd->BottomStackElement = (yyvsp[(2) - (2)].stack_element_p) ;
        (yyval.stack_element_p) = (yyvsp[(2) - (2)].stack_element_p) ;                 // $2 will be the last stack element in the list
    ;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 943 "stack_description_parser.y"
    {
        StackElement *stack_element = (yyval.stack_element_p) = alloc_and_init_stack_element () ;

        if (stack_element == NULL)
        {
            FREE_POINTER (free, (yyvsp[(2) - (5)].string)) ;
            FREE_POINTER (free, (yyvsp[(4) - (5)].string)) ;

            stack_description_error (stkd, analysis, scanner, "Malloc stack element failed") ;

            YYABORT ;
        }

        Layer *layer = alloc_and_init_layer () ;

        if (layer == NULL)
        {
            FREE_POINTER (free,               (yyvsp[(2) - (5)].string)) ;
            FREE_POINTER (free,               (yyvsp[(4) - (5)].string)) ;
            FREE_POINTER (free_stack_element, stack_element) ;

            stack_description_error (stkd, analysis, scanner, "Malloc layer failed") ;

            YYABORT ;
        }

        layer->Height   = (yyvsp[(3) - (5)].double_v) ;
        layer->Material = find_material_in_list (stkd->MaterialsList, (yyvsp[(4) - (5)].string)) ;

        if (layer->Material == NULL)
        {
            sprintf (error_message, "Unknown material %s", (yyvsp[(4) - (5)].string)) ;

            FREE_POINTER (free,               (yyvsp[(2) - (5)].string)) ;
            FREE_POINTER (free,               (yyvsp[(4) - (5)].string)) ;
            FREE_POINTER (free_stack_element, stack_element) ;
            FREE_POINTER (free_layer,         layer) ;

            stack_description_error (stkd, analysis, scanner, error_message) ;

            YYABORT ;
        }

        layer->Material->Used++ ;

        FREE_POINTER (free, (yyvsp[(4) - (5)].string)) ;

        stack_element->Type          = TDICE_STACK_ELEMENT_LAYER ;
        stack_element->Pointer.Layer = layer ;
        stack_element->Id            = (yyvsp[(2) - (5)].string) ;
        stack_element->NLayers       = 1 ;
    ;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 997 "stack_description_parser.y"
    {
        num_channels++ ;

        if (stkd->Channel == NULL)
        {
            stack_description_error (stkd, analysis, scanner, "Error: channel used in stack but not declared") ;

            YYABORT ;
        }

        StackElement *stack_element = (yyval.stack_element_p) = alloc_and_init_stack_element () ;

        if (stack_element == NULL)
        {
            FREE_POINTER (free, (yyvsp[(2) - (3)].string)) ;

            stack_description_error (stkd, analysis, scanner, "Malloc stack element failed") ;

            YYABORT ;
        }

        stack_element->Type            = TDICE_STACK_ELEMENT_CHANNEL ;
        stack_element->Pointer.Channel = stkd->Channel ; // This might be NULL !!!
        stack_element->Id              = (yyvsp[(2) - (3)].string) ;
        stack_element->NLayers         = stkd->Channel->NLayers ;
    ;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 1027 "stack_description_parser.y"
    {
        num_dies++ ;

        StackElement *stack_element = (yyval.stack_element_p) = alloc_and_init_stack_element () ;

        if (stack_element == NULL)
        {
            FREE_POINTER (free, (yyvsp[(2) - (6)].string)) ;
            FREE_POINTER (free, (yyvsp[(3) - (6)].string)) ;
            FREE_POINTER (free, (yyvsp[(5) - (6)].string)) ;

            stack_description_error (stkd, analysis, scanner, "Malloc stack element failed") ;

            YYABORT ;
        }

        stack_element->Type        = TDICE_STACK_ELEMENT_DIE ;
        stack_element->Id          = (yyvsp[(2) - (6)].string) ;
        stack_element->Pointer.Die = find_die_in_list (stkd->DiesList, (yyvsp[(3) - (6)].string)) ;

        if (stack_element->Pointer.Die == NULL)
        {
            sprintf (error_message, "Unknown die %s", (yyvsp[(3) - (6)].string)) ;

            FREE_POINTER (free,               (yyvsp[(3) - (6)].string)) ;
            FREE_POINTER (free,               (yyvsp[(5) - (6)].string)) ;
            FREE_POINTER (free_stack_element, stack_element) ;

            stack_description_error (stkd, analysis, scanner, error_message) ;

            YYABORT ;
        }

        stack_element->Pointer.Die->Used++ ;

        stack_element->NLayers = stack_element->Pointer.Die->NLayers ;
        stack_element->Floorplan = alloc_and_init_floorplan () ;

        if (stack_element->Floorplan == NULL)
        {
            FREE_POINTER (free,               (yyvsp[(3) - (6)].string)) ;
            FREE_POINTER (free,               (yyvsp[(5) - (6)].string)) ;
            FREE_POINTER (free_stack_element, stack_element) ;

            stack_description_error (stkd, analysis, scanner, "Malloc floorplan failed") ;

            YYABORT ;
        }

        if (   fill_floorplan (stack_element->Floorplan, stkd->Dimensions, (yyvsp[(5) - (6)].string))
            == TDICE_FAILURE)
        {
            FREE_POINTER (free,                   (yyvsp[(3) - (6)].string)) ;
            FREE_POINTER (free,                   (yyvsp[(5) - (6)].string)) ;
            FREE_POINTER (free_stack_description, stkd) ;

            YYABORT ; // CHECKME error messages printed in this case ....
        }

        FREE_POINTER (free, (yyvsp[(5) - (6)].string)) ;  // FIXME check memory leak
        FREE_POINTER (free, (yyvsp[(3) - (6)].string)) ;
    ;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 1098 "stack_description_parser.y"
    {
        // StepTime is set to 1 to avoid division by zero when computing
        // the capacitance of a thermal cell

        analysis->AnalysisType = TDICE_ANALYSIS_TYPE_STEADY ;
        analysis->StepTime     = (Time_t) 1.0 ;
        analysis->SlotTime     = (Time_t) 0.0 ;
        analysis->SlotLength   = 1u ; // CHECKME
    ;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 1110 "stack_description_parser.y"
    {
        if ((yyvsp[(8) - (9)].double_v) < (yyvsp[(5) - (9)].double_v))
        {
            stack_description_error

                (stkd, analysis, scanner, "Slot time must be higher than StepTime") ;

            YYABORT ;
        }

        if ((yyvsp[(5) - (9)].double_v) <= 0.0)
        {
            stack_description_error

                (stkd, analysis, scanner, "StepTime must be a positive value") ;

            YYABORT ;
        }

        if ((yyvsp[(8) - (9)].double_v) <= 0.0)
        {
            stack_description_error

                (stkd, analysis, scanner, "SlotTime must be a positive value") ;

            YYABORT ;
        }

        analysis->AnalysisType = TDICE_ANALYSIS_TYPE_TRANSIENT ;
        analysis->StepTime     = (Time_t) (yyvsp[(5) - (9)].double_v) ;
        analysis->SlotTime     = (Time_t) (yyvsp[(8) - (9)].double_v) ;
        analysis->SlotLength   = (Quantity_t) ( (yyvsp[(8) - (9)].double_v) / (yyvsp[(5) - (9)].double_v) ) ;
    ;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 1152 "stack_description_parser.y"
    {
        analysis->InitialTemperature = (Temperature_t) (yyvsp[(3) - (4)].double_v);
    ;}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 1172 "stack_description_parser.y"
    {
        add_inspection_point_to_analysis (analysis, (yyvsp[(1) - (1)].inspection_point_p)) ;
     ;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 1179 "stack_description_parser.y"
    {
        add_inspection_point_to_analysis (analysis, (yyvsp[(2) - (2)].inspection_point_p)) ;
     ;}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 1193 "stack_description_parser.y"
    {
        StackElement *stack_element =

            find_stack_element_in_list (stkd->BottomStackElement, (yyvsp[(3) - (12)].string)) ;

        if (stack_element == NULL)
        {
            sprintf (error_message, "Unknown stack element %s", (yyvsp[(3) - (12)].string)) ;

            stack_description_error (stkd, analysis, scanner, error_message) ;

            FREE_POINTER (free, (yyvsp[(3) - (12)].string)) ;
            FREE_POINTER (free, (yyvsp[(9) - (12)].string)) ;

            YYABORT ;
        }

        Tcell *tcell = alloc_and_init_tcell () ;

        if (tcell == NULL)
        {
            FREE_POINTER (free, (yyvsp[(3) - (12)].string)) ;
            FREE_POINTER (free, (yyvsp[(9) - (12)].string)) ;

            stack_description_error (stkd, analysis, scanner, "Malloc tcell failed") ;

            YYABORT ;
        }

        align_tcell (tcell, (yyvsp[(5) - (12)].double_v), (yyvsp[(7) - (12)].double_v), stkd->Dimensions) ;

        InspectionPoint *inspection_point = (yyval.inspection_point_p) = alloc_and_init_inspection_point () ;

        if (inspection_point == NULL)
        {
            FREE_POINTER (free, (yyvsp[(3) - (12)].string)) ;
            FREE_POINTER (free, (yyvsp[(9) - (12)].string)) ;

            FREE_POINTER (free_tcell, tcell) ;

            stack_description_error (stkd, analysis, scanner, "Malloc inspection point command failed") ;

            YYABORT ;
        }

        inspection_point->Type          = TDICE_OUTPUT_TYPE_TCELL ;
        inspection_point->Instant       = (yyvsp[(10) - (12)].output_instant_v) ;
        inspection_point->FileName      = (yyvsp[(9) - (12)].string) ;
        inspection_point->Pointer.Tcell = tcell ;
        inspection_point->StackElement  = stack_element ;

        FREE_POINTER (free, (yyvsp[(3) - (12)].string)) ;
     ;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 1254 "stack_description_parser.y"
    {
        StackElement *stack_element =

            find_stack_element_in_list (stkd->BottomStackElement, (yyvsp[(3) - (10)].string)) ;

        if (stack_element == NULL)
        {
            sprintf (error_message, "Unknown stack element %s", (yyvsp[(3) - (10)].string)) ;

            stack_description_error (stkd, analysis, scanner, error_message) ;

            FREE_POINTER (free, (yyvsp[(3) - (10)].string)) ;
            FREE_POINTER (free, (yyvsp[(5) - (10)].string)) ;

            YYABORT ;
        }

        if (stack_element->Type != TDICE_STACK_ELEMENT_DIE)
        {
            sprintf (error_message, "The stack element %s must be a die", (yyvsp[(3) - (10)].string)) ;

            stack_description_error (stkd, analysis, scanner, error_message) ;

            FREE_POINTER (free, (yyvsp[(3) - (10)].string)) ;
            FREE_POINTER (free, (yyvsp[(5) - (10)].string)) ;

            YYABORT ;
        }

        Tflp *tflp = alloc_and_init_tflp () ;

        if (tflp == NULL)
        {
            FREE_POINTER (free, (yyvsp[(3) - (10)].string)) ;
            FREE_POINTER (free, (yyvsp[(5) - (10)].string)) ;

            stack_description_error (stkd, analysis, scanner, "Malloc tflp failed") ;

            YYABORT ;
        }

        tflp->Quantity = (yyvsp[(7) - (10)].output_quantity_v) ;

        InspectionPoint *inspection_point = (yyval.inspection_point_p) = alloc_and_init_inspection_point () ;

        if (inspection_point == NULL)
        {
            FREE_POINTER (free, (yyvsp[(3) - (10)].string)) ;
            FREE_POINTER (free, (yyvsp[(5) - (10)].string)) ;

            FREE_POINTER (free_tflp, tflp) ;

            stack_description_error (stkd, analysis, scanner, "Malloc inspection point command failed") ;

            YYABORT ;
        }

        inspection_point->Type         = TDICE_OUTPUT_TYPE_TFLP ;
        inspection_point->Instant      = (yyvsp[(8) - (10)].output_instant_v) ;
        inspection_point->FileName     = (yyvsp[(5) - (10)].string) ;
        inspection_point->Pointer.Tflp = tflp ;
        inspection_point->StackElement = stack_element ;

        FREE_POINTER (free, (yyvsp[(3) - (10)].string)) ;
     ;}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 1328 "stack_description_parser.y"
    {
        StackElement *stack_element =

            find_stack_element_in_list (stkd->BottomStackElement, (yyvsp[(3) - (12)].string)) ;

        if (stack_element == NULL)
        {
            sprintf (error_message, "Unknown stack element %s", (yyvsp[(3) - (12)].string)) ;

            stack_description_error (stkd, analysis, scanner, error_message) ;

            FREE_POINTER (free, (yyvsp[(3) - (12)].string)) ;
            FREE_POINTER (free, (yyvsp[(5) - (12)].string)) ;
            FREE_POINTER (free, (yyvsp[(7) - (12)].string)) ;

            YYABORT ;
        }

        if (stack_element->Type != TDICE_STACK_ELEMENT_DIE)
        {
            sprintf (error_message, "The stack element %s must be a die", (yyvsp[(3) - (12)].string)) ;

            stack_description_error (stkd, analysis, scanner, error_message) ;

            FREE_POINTER (free, (yyvsp[(3) - (12)].string)) ;
            FREE_POINTER (free, (yyvsp[(5) - (12)].string)) ;
            FREE_POINTER (free, (yyvsp[(7) - (12)].string)) ;

            YYABORT ;
        }

        FloorplanElement *floorplan_element = find_floorplan_element_in_list

            (stack_element->Floorplan->ElementsList, (yyvsp[(5) - (12)].string)) ;

        if (floorplan_element == NULL)
        {
            sprintf (error_message, "Unknown floorplan element %s", (yyvsp[(5) - (12)].string)) ;

            stack_description_error (stkd, analysis, scanner, error_message) ;

            FREE_POINTER (free, (yyvsp[(3) - (12)].string)) ;
            FREE_POINTER (free, (yyvsp[(5) - (12)].string)) ;
            FREE_POINTER (free, (yyvsp[(7) - (12)].string)) ;

            YYABORT ;
        }

        Tflpel *tflpel = alloc_and_init_tflpel () ;

        if (tflpel == NULL)
        {
            FREE_POINTER (free, (yyvsp[(3) - (12)].string)) ;
            FREE_POINTER (free, (yyvsp[(5) - (12)].string)) ;
            FREE_POINTER (free, (yyvsp[(7) - (12)].string)) ;

            stack_description_error (stkd, analysis, scanner, "Malloc Tflpel failed") ;

            YYABORT ;
        }

        tflpel->FloorplanElement = floorplan_element ;
        tflpel->Quantity         = (yyvsp[(9) - (12)].output_quantity_v) ;

        InspectionPoint *inspection_point = (yyval.inspection_point_p) = alloc_and_init_inspection_point () ;

        if (inspection_point == NULL)
        {
            FREE_POINTER (free, (yyvsp[(3) - (12)].string)) ;
            FREE_POINTER (free, (yyvsp[(5) - (12)].string)) ;
            FREE_POINTER (free, (yyvsp[(7) - (12)].string)) ;

            FREE_POINTER (free_tflpel, tflpel) ;

            stack_description_error (stkd, analysis, scanner, "Malloc inspection point command failed") ;

            YYABORT ;
        }

        inspection_point->Type           = TDICE_OUTPUT_TYPE_TFLPEL ;
        inspection_point->Instant        = (yyvsp[(10) - (12)].output_instant_v) ;
        inspection_point->FileName       = (yyvsp[(7) - (12)].string) ;
        inspection_point->Pointer.Tflpel = tflpel ;
        inspection_point->StackElement   = stack_element ;

        FREE_POINTER (free, (yyvsp[(3) - (12)].string)) ;
        FREE_POINTER (free, (yyvsp[(5) - (12)].string)) ;
     ;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 1423 "stack_description_parser.y"
    {
        StackElement *stack_element =

            find_stack_element_in_list (stkd->BottomStackElement, (yyvsp[(3) - (8)].string)) ;

        if (stack_element == NULL)
        {
            sprintf (error_message, "Unknown stack element %s", (yyvsp[(3) - (8)].string)) ;

            stack_description_error (stkd, analysis, scanner, error_message) ;

            FREE_POINTER (free, (yyvsp[(3) - (8)].string)) ;
            FREE_POINTER (free, (yyvsp[(5) - (8)].string)) ;

            YYABORT ;
        }

        InspectionPoint *inspection_point = (yyval.inspection_point_p) = alloc_and_init_inspection_point () ;

        if (inspection_point == NULL)
        {
            FREE_POINTER (free, (yyvsp[(3) - (8)].string)) ;
            FREE_POINTER (free, (yyvsp[(5) - (8)].string)) ;

            stack_description_error (stkd, analysis, scanner, "Malloc inspection point command failed") ;

            YYABORT ;
        }

        inspection_point->Type         = TDICE_OUTPUT_TYPE_TMAP ;
        inspection_point->Instant      = (yyvsp[(6) - (8)].output_instant_v) ;
        inspection_point->FileName     = (yyvsp[(5) - (8)].string) ;
        inspection_point->StackElement = stack_element ;

        FREE_POINTER (free, (yyvsp[(3) - (8)].string)) ;
     ;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 1467 "stack_description_parser.y"
    {
        StackElement *stack_element =

            find_stack_element_in_list (stkd->BottomStackElement, (yyvsp[(3) - (10)].string)) ;

        if (stack_element == NULL)
        {
            sprintf (error_message, "Unknown stack element %s", (yyvsp[(3) - (10)].string)) ;

            stack_description_error (stkd, analysis, scanner, error_message) ;

            FREE_POINTER (free, (yyvsp[(3) - (10)].string)) ;
            FREE_POINTER (free, (yyvsp[(5) - (10)].string)) ;

            YYABORT ;
        }

        if (stack_element->Type != TDICE_STACK_ELEMENT_CHANNEL)
        {
            sprintf (error_message, "The stack element %s must be a channel", (yyvsp[(3) - (10)].string)) ;

            stack_description_error (stkd, analysis, scanner, error_message) ;

            FREE_POINTER (free, (yyvsp[(3) - (10)].string)) ;
            FREE_POINTER (free, (yyvsp[(5) - (10)].string)) ;

            YYABORT ;
        }

        InspectionPoint *inspection_point = (yyval.inspection_point_p) = alloc_and_init_inspection_point () ;

        if (inspection_point == NULL)
        {
            FREE_POINTER (free, (yyvsp[(3) - (10)].string)) ;
            FREE_POINTER (free, (yyvsp[(5) - (10)].string)) ;

            stack_description_error (stkd, analysis, scanner, "Malloc inspection point command failed") ;

            YYABORT ;
        }

        Tcoolant *tcoolant = alloc_and_init_tcoolant () ;

        if (tcoolant == NULL)
        {
            FREE_POINTER (free, (yyvsp[(3) - (10)].string)) ;
            FREE_POINTER (free, (yyvsp[(5) - (10)].string)) ;

            stack_description_error (stkd, analysis, scanner, "Malloc tcoolant failed") ;

            YYABORT ;
        }

        tcoolant->Quantity = (yyvsp[(7) - (10)].output_quantity_v) ;

        inspection_point->Type             = TDICE_OUTPUT_TYPE_TCOOLANT ;
        inspection_point->Instant          = (yyvsp[(8) - (10)].output_instant_v) ;
        inspection_point->FileName         = (yyvsp[(5) - (10)].string) ;
        inspection_point->Pointer.Tcoolant = tcoolant ;
        inspection_point->StackElement     = stack_element ;

        FREE_POINTER (free, (yyvsp[(3) - (10)].string)) ;
     ;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 1535 "stack_description_parser.y"
    { (yyval.output_quantity_v) =  TDICE_OUTPUT_QUANTITY_MAXIMUM ; ;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 1536 "stack_description_parser.y"
    { (yyval.output_quantity_v) =  TDICE_OUTPUT_QUANTITY_MINIMUM ; ;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 1537 "stack_description_parser.y"
    { (yyval.output_quantity_v) =  TDICE_OUTPUT_QUANTITY_AVERAGE ; ;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 1543 "stack_description_parser.y"
    { (yyval.output_instant_v) =  TDICE_OUTPUT_INSTANT_FINAL ; ;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 1544 "stack_description_parser.y"
    { (yyval.output_instant_v) =  TDICE_OUTPUT_INSTANT_STEP ;  ;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 1545 "stack_description_parser.y"
    { (yyval.output_instant_v) =  TDICE_OUTPUT_INSTANT_SLOT ;  ;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 1546 "stack_description_parser.y"
    { (yyval.output_instant_v) =  TDICE_OUTPUT_INSTANT_FINAL ; ;}
    break;



/* Line 1455 of yacc.c  */
#line 3137 "stack_description_parser.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
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
#if ! YYERROR_VERBOSE
      yyerror (stkd, analysis, scanner, YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (stkd, analysis, scanner, yymsg);
	  }
	else
	  {
	    yyerror (stkd, analysis, scanner, YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval, stkd, analysis, scanner);
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

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
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


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp, stkd, analysis, scanner);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

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

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (stkd, analysis, scanner, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval, stkd, analysis, scanner);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, stkd, analysis, scanner);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 1675 of yacc.c  */
#line 1549 "stack_description_parser.y"


void stack_description_error
(
    StackDescription *stkd,
    Analysis          __attribute__ ((unused)) *analysis,
    yyscan_t          scanner,
    String_t          message
)
{
    fprintf (stack_description_get_out (scanner),
             "%s:%d: %s\n",
            stkd->FileName, stack_description_get_lineno (scanner), message) ;

    FREE_POINTER (free_stack_description, stkd) ;
}

