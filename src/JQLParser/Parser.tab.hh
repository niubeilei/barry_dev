// A Bison parser, made by GNU Bison 3.0.2.

// Skeleton interface for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2013 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.

/**
 ** \file Parser.tab.hh
 ** Define the yy::parser class.
 */

// C++ LALR(1) parser skeleton written by Akim Demaille.

#ifndef YY_YY_PARSER_TAB_HH_INCLUDED
# define YY_YY_PARSER_TAB_HH_INCLUDED


# include <vector>
# include <iostream>
# include <stdexcept>
# include <string>
# include "stack.hh"
# include "location.hh"


#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif


namespace yy {
#line 113 "Parser.tab.hh" // lalr1.cc:372





  /// A Bison parser.
  class Parser
  {
  public:
#ifndef YYSTYPE
    /// Symbol semantic values.
    union semantic_type
    {
    #line 45 "Parser.yy" // lalr1.cc:372

	int64_t 	ll_value;
	u64 	    u64_value;
	u32 	    u32_value;
	double 		double_val;
	char *  	strval;
	int 		subtok;
	bool 		bool_val;

	typedef AosJqlStmtDataConnector::AosDirList DirList;  
	typedef vector<DirList*> AosVecDirList; 
	typedef vector<OmnString> AosStrList; 
	typedef vector<AosExprList*>	*AosExprListVec;
	typedef vector<AosExprNameValuePtr>	AosExprNameValues;
	typedef vector<AosJqlRecordPickerPtr>	AosJqlRecordPicks;
	typedef vector<AosGenericValueObjPtr>	AosJqlGenericobjValues;
	typedef vector<AosJqlStatementPtr> AosJqlStatementList;
	
	AosJQLDataFieldTypeInfo			*AosJQLDataFieldTypeInfoPtr;

	AosJqlStatement					*AosJqlStatementValue;

	// new format
	AosJqlStmtGenericobj			*AosJqlStmtGenericobjValue;
	AosGenericValueObj				*AosJqlGenericobjValueValues;
	AosJqlGenericobjValues			*AosJqlGenericobjValuesValues;

	AosJoinStmtPhrase				*AosJoinStmtPhraseValue;
    AosJimoid						*AosPJimoidValue;
    AosJoinType						 AosJoinTypeValue;
    AosRecordProcItem				*AosPRecordProcItemValue;
    JQLTypes						*AosPStmtListValue;
	AosJqlStmtExpr					*AosPJqlStmtExprValue;
	AosExprObj						*AosExprValue;
	AosDataSetItem                  *AosPDataSetItemValue;
	AosDataSetItemList              *AosPDataSetItemListValue;
	AosVecDirList					*AosVecDirListValue;
	AosStrList						*AosStrListValue;
	AosExprList                     *AosExprListValue;
	vector<AosExprList*>			*AosExprListVecValue;
	AosExprNameValues				*AosExprNameValuesValue;

	// select statment
	AosJqlStmtQuery					*AosJqlStmtQueryValue;
	AosJQLStmtSelectOptionList  	*AosJQLStmtSelectOptionListValue;
	AosJqlSelectFieldList			*AosJqlSelectFieldListValue;
	vector<AosJqlSelectFieldList*>	*AosJqlSelectFieldListVecValue;
	AosJqlSelectField				*AosJqlSelectFieldValue;
	AosTableReferences				*AosTableReferencesValue;
	AosJqlTableReference			*AosJqlTableReferenceValue;
	AosJqlTableFactor				*AosJqlTableFactorValue;
	AosJqlSubQueryTable				*AosJqlSubQueryTableValue;
	AosJqlJoinTable					*AosJqlJoinTableValue;
	AosJqlLimit						*AosJqlLimitValue;
	AosJqlOrderBy					*AosJqlOrderByValue;
	AosJqlOrderByField				*AosJqlOrderByFieldValue;
	AosJqlOrderByFieldList			*AosJqlOrderByFieldListValue;
	AosJqlGroupBy					*AosJqlGroupByValue;
	AosJqlWhere						*AosJqlWhereValue;
	AosJqlHaving					*AosJqlHavingValue;
	AosExprSearchCase				*AosExprSearchCaseValue;
	AosExprSimpleCase				*AosExprSimpleCaseValue;


	AosJqlStmtInsertItem			*AosJqlStmtInsertItemValue;
	AosJqlStmtUpdateItem 			*AosJqlStmtUpdateItemValue;
	AosJqlStmtDeleteItem 			*AosJqlStmtDeleteItemValue;

	AosJqlStmtJoinSyncher			*AosJqlStmtJoinSyncherValue;
	
	AosJqlStmtDictionary			*AosJqlStmtDictValues;

	// map
	AosJqlStmtMap					*AosJqlStmtMapValues;
	// distributionmap
	AosJqlStmtDistributionMap		*AosJqlStmtDistributionMapValues;

	// create table statements
	AosJqlStmtTable					*AosJqlStmtTableValue;
	AosJqlColumn					*AosJqlColumnValue;
	AosJqlTableOption				*AosJqlTableOptionValue;
	AosJqlCreateDefineGroup			*AosJqlCreateDefineGroupValue;		
	AosDataType::E                  AosDataTypeVal;
	vector<AosJqlTableVirtulFieldDefPtr> *AosJqlTableVirtualFieldDefListValue;


	// index statements
	AosJqlStmtIndex          		*AosJqlStmtIndexValue;
	AosJqlStmtIndex::AosFieldIdxCols *AosMoreFieldIndexColumnValue;
	vector<AosJqlStmtIndex::AosFieldIdxCols*> *AosMoreFieldIndexColumnValues;
	// dataset statements

	AosJqlStmtDataset        		*AosJqlStmtDatasetValue;
	AosStringListVec					*AosStringListValue;

	// schema statements
	// create schema statements
	AosJqlStmtSchema          		*AosJqlStmtSchemaValue;
	AosJqlRecordPicks				*AosJqlRecordPicksValue;

	// Schedule Statements
	AosJqlStmtSchedule				*AosJqlStmtScheduleValue;

	// job 
	AosJqlStmtJob 					*AosJqlStmtJobValue;

	AosJqlStmtDoc 					*AosJqlStmtDocValue;
	// task
	AosJqlStmtTask 					*AosJqlStmtTaskValue;
	AosJqlStmtDatascanner			*AosJqlStmtDatascannerValue;

	// data record statements
	AosJqlStmtDataRecord			*AosJqlStmtDataRecordValue;

	// data field statements
	AosJqlStmtDataField				*AosJqlStmtDataFieldValue;
	AosJqlStmtVirtualField			*AosJqlStmtVirtualFieldValue;
	AosJqlStmtFieldExpr				*AosJqlStmtFieldExprValue;
	AosJqlStmtJimoLogic				*AosJqlStmtJimoLogicValue;
	AosJqlStmtNickField				*AosJqlStmtNickFieldValue;

	AosJqlSelect					*AosJqlSelectValue;
	AosJqlStmtSelectInto			*AosJqlSelectIntoValue;

	// data proc statements
	AosJqlStmtDataProc				*AosJqlStmtDataProcValue;

	AosJqlStmtDataConnector			*AosJqlStmtDataConnectorValue;

	// load data
	AosJqlStmtLoadData				*AosJqlStmtLoadDataValue;

	//service statement
	AosJqlStmtService 				*AosJqlStmtServiceValue;

	// User Mgr
	AosJqlStmtUserMgr				*AosJqlStmtUserMgrValue;

	// debug statements
	AosJqlStmtDebug					*AosJqlStmtDebugValue;

	AosJqlStmtSequence				*AosJqlStmtSequenceValue;
	AosJQLSequenceInfo				*AosJQLSequenceInfoValue;

	// data record statements
	AosJqlStmtDatabase			    *AosJqlStmtDatabaseValue;

	AosJqlStmtStatistics			*AosJqlStmtStatisticsValue;
	AosJqlStmtStat					*AosJqlStmtStatValue;
	AosJqlStmtActor					*AosJqlStmtActorValue;
	AosJqlStmtStatistics::AosMeasure						*AosMeasureValue;
	vector<AosJqlStmtStatistics::AosMeasure*>						*AosMeasuresValue;

	AosJqlStmtRunScriptFile 		*AosJqlStmtRunScriptFileValue;

	AosJqlStmtSyncer				*AosJqlStmtSyncerValue;

	AosJqlStmtCompleteIIL			*AosJqlStmtCompleteIILValue;

	AosJqlStmtHBase					*AosJqlStmtHBaseValue;

	// union
	AosJqlStmtQueryUnion			*AosJqlStmtQueryUnionValue;

	// IF
	AosJqlStmtIF					*AosJqlStmtIFValue;
	AosJqlStatementList				*AosJqlStatementListValue;
	AosJqlStmtAssignment			*AosJqlStmtAssignmentValue;

#line 297 "Parser.tab.hh" // lalr1.cc:372
    };
#else
    typedef YYSTYPE semantic_type;
#endif
    /// Symbol locations.
    typedef location location_type;

    /// Syntax errors thrown from user actions.
    struct syntax_error : std::runtime_error
    {
      syntax_error (const location_type& l, const std::string& m);
      location_type location;
    };

    /// Tokens.
    struct token
    {
      enum yytokentype
      {
        ASSIGN = 258,
        ASSIGNMENT = 259,
        LOGIC = 260,
        XOR = 261,
        IN = 262,
        IS = 263,
        LIKE = 264,
        REGEXP = 265,
        BETWEEN = 266,
        COMPARISON = 267,
        NOT = 268,
        SHIFT = 269,
        MOD = 270,
        UMINUS = 271,
        INTNUM = 272,
        NAME = 273,
        U8VAR = 274,
        STRING = 275,
        UNSIGNED_LONG = 276,
        UNSIGNED_INT = 277,
        BOOL = 278,
        DOUBLE_NUM = 279,
        CREATE = 280,
        DELETE = 281,
        DOCBEGIN = 282,
        DOCEND = 283,
        XMLDOC = 284,
        DOC = 285,
        EXISTS = 286,
        LOGIN = 287,
        LOGOUT = 288,
        RETRIEVE = 289,
        START = 290,
        STOP = 291,
        CHECK = 292,
        RESTART = 293,
        OVERRIDE = 294,
        BY = 295,
        ANDOP = 296,
        IGNORE = 297,
        LOW_PRIORITY = 298,
        QUICK = 299,
        SET = 300,
        UPDATE = 301,
        WHERE = 302,
        GROUP = 303,
        KEY = 304,
        USERVAR = 305,
        WAIT = 306,
        DOES = 307,
        SELECT = 308,
        DELAYED = 309,
        INSERT = 310,
        INTO = 311,
        VALUES = 312,
        INTO_OUTFILE = 313,
        COMPLETE = 314,
        LEFTSHIFT = 315,
        ROLLUP = 316,
        RIGHTSHIFT = 317,
        RECORD = 318,
        RECORDS = 319,
        PICKER = 320,
        PICKERS = 321,
        TYPE = 322,
        JIMOID = 323,
        AS = 324,
        END = 325,
        PROC = 326,
        PROCS = 327,
        DEBUG = 328,
        DATA = 329,
        SCANNER = 330,
        SCANNERS = 331,
        DATASET = 332,
        DATASETS = 333,
        DATABASE = 334,
        DATABASES = 335,
        DATE = 336,
        SCHEMA = 337,
        SCHEMAS = 338,
        WITH = 339,
        ORDER = 340,
        CONNECTOR = 341,
        CONNECTORS = 342,
        CUBE = 343,
        USE = 344,
        USING = 345,
        FILTER = 346,
        FILTERED = 347,
        ACCEPTED = 348,
        MATCHED = 349,
        MISMATCHED = 350,
        DROP = 351,
        DROPPED = 352,
        MULTIPLE = 353,
        DEFAULT = 354,
        VALUE = 355,
        SELECTOR = 356,
        SELECTORS = 357,
        INNER = 358,
        FULL = 359,
        OUTER = 360,
        LEFT = 361,
        RIGHT = 362,
        EXCLUSIVE = 363,
        XO = 364,
        JOIN = 365,
        OR = 366,
        AND = 367,
        ALL = 368,
        ANY = 369,
        MODE = 370,
        ACCUMULATE = 371,
        BINARY = 372,
        BOTH = 373,
        CASE = 374,
        CURRENT_DATE = 375,
        CURRENT_TIME = 376,
        CURRENT_TIMESTAMP = 377,
        DAY_HOUR = 378,
        DAY_MICROSECOND = 379,
        DAY_MINUTE = 380,
        DAY_SECOND = 381,
        ELSE = 382,
        FCOUNT = 383,
        FDATE_ADD = 384,
        FDATE_SUB = 385,
        FOR = 386,
        FROM = 387,
        FSUBSTRING = 388,
        FTRIM = 389,
        HOUR_MICROSECOND = 390,
        HOUR_MINUTE = 391,
        HOUR_SECOND = 392,
        INTERVAL = 393,
        NULLX = 394,
        NUMBER = 395,
        SOME = 396,
        THEN = 397,
        TRIM = 398,
        TRAILING = 399,
        WHEN = 400,
        YEAR = 401,
        YEAR_MONTH = 402,
        LEADING = 403,
        LIMIT = 404,
        HAVING = 405,
        TABLE = 406,
        TABLES = 407,
        SHOW = 408,
        INDEX = 409,
        INDEXES = 410,
        AUTO_INCREMENT = 411,
        CHECKSUM = 412,
        ASC = 413,
        DESC = 414,
        COMMENTS = 415,
        CONFIG = 416,
        BLOCKSIZE = 417,
        UNSIGNED = 418,
        BIT = 419,
        CHAR = 420,
        INT = 421,
        TINYINT = 422,
        SMALLINT = 423,
        MEDIUMINT = 424,
        VARCHAR = 425,
        VARCHAR2 = 426,
        STR = 427,
        BIGINT = 428,
        DOUBLE = 429,
        DATETIME = 430,
        DECIMAL = 431,
        FLOAT = 432,
        TEXT = 433,
        ON = 434,
        OFF = 435,
        SUPPRESS = 436,
        DIR = 437,
        DESCRIBE = 438,
        VARIABLE = 439,
        FIELD = 440,
        FIELDS = 441,
        DELIMITER = 442,
        QUALIFIER = 443,
        TASKS = 444,
        JOB = 445,
        JOBS = 446,
        STATUS = 447,
        TIMES = 448,
        TIME = 449,
        PRIORITY = 450,
        RUN = 451,
        AT = 452,
        ACTOR = 453,
        JAVA = 454,
        ACTION = 455,
        ENGING = 456,
        INPUT = 457,
        MAPPING = 458,
        MAPTASK = 459,
        OUTPUT = 460,
        REDUCETASK = 461,
        SINGLE = 462,
        TASK = 463,
        SCHEDULE = 464,
        OFFSET = 465,
        EXPRESSION = 466,
        FORM = 467,
        FORMATTER = 468,
        FORMAT = 469,
        LENGTH = 470,
        MAX = 471,
        POS = 472,
        TO = 473,
        PRINT = 474,
        CODING = 475,
        COMBINE = 476,
        COMBINATIONS = 477,
        FILENAME = 478,
        ID = 479,
        SERVER = 480,
        SERVICE = 481,
        SERVICES = 482,
        SYNCER = 483,
        SIZE = 484,
        SPLIT = 485,
        IILNAME = 486,
        IIL = 487,
        OPERATOR = 488,
        REDUCE = 489,
        STINCT = 490,
        DISTINCT = 491,
        DISTINCTROW = 492,
        HIGH_PRIORITY = 493,
        SQL_BIG_RESULT = 494,
        SQL_CALC_FOUND_ROWS = 495,
        SQL_SMALL_RESULT = 496,
        STRAIGHT_JOIN = 497,
        UNIFORM = 498,
        PATH = 499,
        ENCODING = 500,
        STATIC = 501,
        DYNAMIC = 502,
        ROW = 503,
        PICKED = 504,
        BYTE = 505,
        BYTES = 506,
        IDFAMILY = 507,
        LOAD = 508,
        LOG = 509,
        IDENTIFIED = 510,
        USER = 511,
        USERS = 512,
        SOURCE = 513,
        DUAL = 514,
        JIMODB = 515,
        DIRECTORY = 516,
        STATISTICS = 517,
        STAT = 518,
        UNIT = 519,
        UNION = 520,
        OUTFILE = 521,
        CSV = 522,
        FIXED = 523,
        DIMENSIONS = 524,
        HBASE = 525,
        RAWKEY = 526,
        ADDRESS = 527,
        PORT = 528,
        THREAD = 529,
        MEASURES = 530,
        MODEL = 531,
        PARSER = 532,
        MAP = 533,
        DICTIONARY = 534,
        DISTRIBUTIONMAP = 535,
        DISTRIBUTIONMAPS = 536,
        MAPS = 537,
        VIRTUAL = 538,
        NICK = 539,
        LIST = 540,
        CONTAIN = 541,
        JIMOLOGIC = 542,
        CHECKPOINT = 543,
        ENDPOINT = 544,
        MATRIX = 545,
        RESULTS = 546,
        EXPORT = 547,
        XPATH = 548,
        TERMINATED = 549,
        ENCLOSED = 550,
        ESCAPED = 551,
        LINES = 552,
        STARTING = 553,
        SKIP = 554,
        INVALID = 555,
        SUB = 556,
        IF = 557,
        SYSTEM_ARGS = 558,
        LEFT_BRACKET = 559,
        RIGHT_BRACKET = 560,
        CACHE = 561,
        CYCLE = 562,
        INCREMENT = 563,
        MAXVALUE = 564,
        MINVALUE = 565,
        NOCACHE = 566,
        NOCYCLE = 567,
        NOMAXVALUE = 568,
        NOMINVALUE = 569,
        NOORDER = 570,
        SEQUENCE = 571,
        CONVERT = 572,
        INFOFIELD = 573,
        ALTER = 574,
        SHUFFLE = 575
      };
    };

    /// (External) token type, as returned by yylex.
    typedef token::yytokentype token_type;

    /// Internal symbol number.
    typedef int symbol_number_type;

    /// Internal symbol number for tokens (subsumed by symbol_number_type).
    typedef unsigned short int token_number_type;

    /// A complete symbol.
    ///
    /// Expects its Base type to provide access to the symbol type
    /// via type_get().
    ///
    /// Provide access to semantic value and location.
    template <typename Base>
    struct basic_symbol : Base
    {
      /// Alias to Base.
      typedef Base super_type;

      /// Default constructor.
      basic_symbol ();

      /// Copy constructor.
      basic_symbol (const basic_symbol& other);

      /// Constructor for valueless symbols.
      basic_symbol (typename Base::kind_type t,
                    const location_type& l);

      /// Constructor for symbols with semantic value.
      basic_symbol (typename Base::kind_type t,
                    const semantic_type& v,
                    const location_type& l);

      ~basic_symbol ();

      /// Destructive move, \a s is emptied into this.
      void move (basic_symbol& s);

      /// The semantic value.
      semantic_type value;

      /// The location.
      location_type location;

    private:
      /// Assignment operator.
      basic_symbol& operator= (const basic_symbol& other);
    };

    /// Type access provider for token (enum) based symbols.
    struct by_type
    {
      /// Default constructor.
      by_type ();

      /// Copy constructor.
      by_type (const by_type& other);

      /// The symbol type as needed by the constructor.
      typedef token_type kind_type;

      /// Constructor from (external) token numbers.
      by_type (kind_type t);

      /// Steal the symbol type from \a that.
      void move (by_type& that);

      /// The (internal) type number (corresponding to \a type).
      /// -1 when this symbol is empty.
      symbol_number_type type_get () const;

      /// The token.
      token_type token () const;

      enum { empty = 0 };

      /// The symbol type.
      /// -1 when this symbol is empty.
      token_number_type type;
    };

    /// "External" symbols: returned by the scanner.
    typedef basic_symbol<by_type> symbol_type;


    /// Build a parser object.
    Parser ();
    virtual ~Parser ();

    /// Parse.
    /// \returns  0 iff parsing succeeded.
    virtual int parse ();

#if YYDEBUG
    /// The current debugging stream.
    std::ostream& debug_stream () const YY_ATTRIBUTE_PURE;
    /// Set the current debugging stream.
    void set_debug_stream (std::ostream &);

    /// Type for debugging levels.
    typedef int debug_level_type;
    /// The current debugging level.
    debug_level_type debug_level () const YY_ATTRIBUTE_PURE;
    /// Set the current debugging level.
    void set_debug_level (debug_level_type l);
#endif

    /// Report a syntax error.
    /// \param loc    where the syntax error is found.
    /// \param msg    a description of the syntax error.
    virtual void error (const location_type& loc, const std::string& msg);

    /// Report a syntax error.
    void error (const syntax_error& err);

  private:
    /// This class is not copyable.
    Parser (const Parser&);
    Parser& operator= (const Parser&);

    /// State numbers.
    typedef int state_type;

    /// Generate an error message.
    /// \param yystate   the state where the error occurred.
    /// \param yytoken   the lookahead token type, or yyempty_.
    virtual std::string yysyntax_error_ (state_type yystate,
                                         symbol_number_type yytoken) const;

    /// Compute post-reduction state.
    /// \param yystate   the current state
    /// \param yysym     the nonterminal to push on the stack
    state_type yy_lr_goto_state_ (state_type yystate, int yysym);

    /// Whether the given \c yypact_ value indicates a defaulted state.
    /// \param yyvalue   the value to check
    static bool yy_pact_value_is_default_ (int yyvalue);

    /// Whether the given \c yytable_ value indicates a syntax error.
    /// \param yyvalue   the value to check
    static bool yy_table_value_is_error_ (int yyvalue);

    static const short int yypact_ninf_;
    static const short int yytable_ninf_;

    /// Convert a scanner token number \a t to a symbol number.
    static token_number_type yytranslate_ (int t);

    // Tables.
  // YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
  // STATE-NUM.
  static const short int yypact_[];

  // YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
  // Performed when YYTABLE does not specify something else to do.  Zero
  // means the default is an error.
  static const unsigned short int yydefact_[];

  // YYPGOTO[NTERM-NUM].
  static const short int yypgoto_[];

  // YYDEFGOTO[NTERM-NUM].
  static const short int yydefgoto_[];

  // YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
  // positive, shift that token.  If negative, reduce the rule whose
  // number is the opposite.  If YYTABLE_NINF, syntax error.
  static const short int yytable_[];

  static const short int yycheck_[];

  // YYSTOS[STATE-NUM] -- The (internal number of the) accessing
  // symbol of state STATE-NUM.
  static const unsigned short int yystos_[];

  // YYR1[YYN] -- Symbol number of symbol that rule YYN derives.
  static const unsigned short int yyr1_[];

  // YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.
  static const unsigned char yyr2_[];


#if YYDEBUG
    /// For a symbol, its name in clear.
    static const char* const yytname_[];

  // YYRLINE[YYN] -- Source line where rule number YYN was defined.
  static const unsigned short int yyrline_[];
    /// Report on the debug stream that the rule \a r is going to be reduced.
    virtual void yy_reduce_print_ (int r);
    /// Print the state stack on the debug stream.
    virtual void yystack_print_ ();

    // Debugging.
    int yydebug_;
    std::ostream* yycdebug_;

    /// \brief Display a symbol type, value and location.
    /// \param yyo    The output stream.
    /// \param yysym  The symbol.
    template <typename Base>
    void yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const;
#endif

    /// \brief Reclaim the memory associated to a symbol.
    /// \param yymsg     Why this token is reclaimed.
    ///                  If null, print nothing.
    /// \param yysym     The symbol.
    template <typename Base>
    void yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const;

  private:
    /// Type access provider for state based symbols.
    struct by_state
    {
      /// Default constructor.
      by_state ();

      /// The symbol type as needed by the constructor.
      typedef state_type kind_type;

      /// Constructor.
      by_state (kind_type s);

      /// Copy constructor.
      by_state (const by_state& other);

      /// Steal the symbol type from \a that.
      void move (by_state& that);

      /// The (internal) type number (corresponding to \a state).
      /// "empty" when empty.
      symbol_number_type type_get () const;

      enum { empty = 0 };

      /// The state.
      state_type state;
    };

    /// "Internal" symbol: element of the stack.
    struct stack_symbol_type : basic_symbol<by_state>
    {
      /// Superclass.
      typedef basic_symbol<by_state> super_type;
      /// Construct an empty symbol.
      stack_symbol_type ();
      /// Steal the contents from \a sym to build this.
      stack_symbol_type (state_type s, symbol_type& sym);
      /// Assignment, needed by push_back.
      stack_symbol_type& operator= (const stack_symbol_type& that);
    };

    /// Stack type.
    typedef stack<stack_symbol_type> stack_type;

    /// The stack.
    stack_type yystack_;

    /// Push a new state on the stack.
    /// \param m    a debug message to display
    ///             if null, no trace is output.
    /// \param s    the symbol
    /// \warning the contents of \a s.value is stolen.
    void yypush_ (const char* m, stack_symbol_type& s);

    /// Push a new look ahead token on the state on the stack.
    /// \param m    a debug message to display
    ///             if null, no trace is output.
    /// \param s    the state
    /// \param sym  the symbol (for its value and location).
    /// \warning the contents of \a s.value is stolen.
    void yypush_ (const char* m, state_type s, symbol_type& sym);

    /// Pop \a n symbols the three stacks.
    void yypop_ (unsigned int n = 1);

    // Constants.
    enum
    {
      yyeof_ = 0,
      yylast_ = 17259,     ///< Last index in yytable_.
      yynnts_ = 230,  ///< Number of nonterminal symbols.
      yyempty_ = -2,
      yyfinal_ = 566, ///< Termination state number.
      yyterror_ = 1,
      yyerrcode_ = 256,
      yyntokens_ = 342  ///< Number of tokens.
    };


  };



} // yy
#line 937 "Parser.tab.hh" // lalr1.cc:372




#endif // !YY_YY_PARSER_TAB_HH_INCLUDED
