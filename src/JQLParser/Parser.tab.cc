// A Bison parser, made by GNU Bison 3.0.2.

// Skeleton implementation for Bison LALR(1) parsers in C++

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


// First part of user declarations.
#line 12 "Parser.yy" // lalr1.cc:399

#ifndef YYDEBUG
#define YYDEBUG 1
#endif


#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <iostream>


using namespace std;

//include your head files or define vars
#include "JQLParser/ParserInclude.h"
#include "Parser.tab.hh"
 
void yyerror(const char *s, ...);
void emit(const char *s, ...);
extern int AosJQLParser_ReadInput(char* buffer,int *numBytesRead,int maxBytesToRead);
extern char * yytext;


#line 62 "Parser.tab.cc" // lalr1.cc:399

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

#include "Parser.tab.hh"

// User implementation prologue.
#line 681 "Parser.yy" // lalr1.cc:407

	AosJQLParser			gAosJQLParser;
	//AosJoinStatement		gJoinStatement;
  extern int yylex(yy::Parser::semantic_type *yylval, 
                   yy::Parser::location_type *yylloc);

#line 82 "Parser.tab.cc" // lalr1.cc:407


#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K].location)
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

# ifndef YYLLOC_DEFAULT
#  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).begin  = YYRHSLOC (Rhs, 1).begin;                   \
          (Current).end    = YYRHSLOC (Rhs, N).end;                     \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;      \
        }                                                               \
    while (/*CONSTCOND*/ false)
# endif


// Suppress unused-variable warnings by "using" E.
#define YYUSE(E) ((void) (E))

// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << std::endl;                  \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yystack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YYUSE(Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void>(0)
# define YY_STACK_PRINT()                static_cast<void>(0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyempty = true)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)


namespace yy {
#line 168 "Parser.tab.cc" // lalr1.cc:474

  /// Build a parser object.
  Parser::Parser ()
#if YYDEBUG
     :yydebug_ (false),
      yycdebug_ (&std::cerr)
#endif
  {}

  Parser::~Parser ()
  {}


  /*---------------.
  | Symbol types.  |
  `---------------*/

  inline
  Parser::syntax_error::syntax_error (const location_type& l, const std::string& m)
    : std::runtime_error (m)
    , location (l)
  {}

  // basic_symbol.
  template <typename Base>
  inline
  Parser::basic_symbol<Base>::basic_symbol ()
    : value ()
  {}

  template <typename Base>
  inline
  Parser::basic_symbol<Base>::basic_symbol (const basic_symbol& other)
    : Base (other)
    , value ()
    , location (other.location)
  {
    value = other.value;
  }


  template <typename Base>
  inline
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const semantic_type& v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}


  /// Constructor for valueless symbols.
  template <typename Base>
  inline
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const location_type& l)
    : Base (t)
    , value ()
    , location (l)
  {}

  template <typename Base>
  inline
  Parser::basic_symbol<Base>::~basic_symbol ()
  {
  }

  template <typename Base>
  inline
  void
  Parser::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move(s);
    value = s.value;
    location = s.location;
  }

  // by_type.
  inline
  Parser::by_type::by_type ()
     : type (empty)
  {}

  inline
  Parser::by_type::by_type (const by_type& other)
    : type (other.type)
  {}

  inline
  Parser::by_type::by_type (token_type t)
    : type (yytranslate_ (t))
  {}

  inline
  void
  Parser::by_type::move (by_type& that)
  {
    type = that.type;
    that.type = empty;
  }

  inline
  int
  Parser::by_type::type_get () const
  {
    return type;
  }


  // by_state.
  inline
  Parser::by_state::by_state ()
    : state (empty)
  {}

  inline
  Parser::by_state::by_state (const by_state& other)
    : state (other.state)
  {}

  inline
  void
  Parser::by_state::move (by_state& that)
  {
    state = that.state;
    that.state = empty;
  }

  inline
  Parser::by_state::by_state (state_type s)
    : state (s)
  {}

  inline
  Parser::symbol_number_type
  Parser::by_state::type_get () const
  {
    return state == empty ? 0 : yystos_[state];
  }

  inline
  Parser::stack_symbol_type::stack_symbol_type ()
  {}


  inline
  Parser::stack_symbol_type::stack_symbol_type (state_type s, symbol_type& that)
    : super_type (s, that.location)
  {
    value = that.value;
    // that is emptied.
    that.type = empty;
  }

  inline
  Parser::stack_symbol_type&
  Parser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    value = that.value;
    location = that.location;
    return *this;
  }


  template <typename Base>
  inline
  void
  Parser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);

    // User destructor.
    YYUSE (yysym.type_get ());
  }

#if YYDEBUG
  template <typename Base>
  void
  Parser::yy_print_ (std::ostream& yyo,
                                     const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YYUSE (yyoutput);
    symbol_number_type yytype = yysym.type_get ();
    yyo << (yytype < yyntokens_ ? "token" : "nterm")
        << ' ' << yytname_[yytype] << " ("
        << yysym.location << ": ";
    YYUSE (yytype);
    yyo << ')';
  }
#endif

  inline
  void
  Parser::yypush_ (const char* m, state_type s, symbol_type& sym)
  {
    stack_symbol_type t (s, sym);
    yypush_ (m, t);
  }

  inline
  void
  Parser::yypush_ (const char* m, stack_symbol_type& s)
  {
    if (m)
      YY_SYMBOL_PRINT (m, s);
    yystack_.push (s);
  }

  inline
  void
  Parser::yypop_ (unsigned int n)
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  Parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  Parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  Parser::debug_level_type
  Parser::debug_level () const
  {
    return yydebug_;
  }

  void
  Parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  inline Parser::state_type
  Parser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - yyntokens_] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - yyntokens_];
  }

  inline bool
  Parser::yy_pact_value_is_default_ (int yyvalue)
  {
    return yyvalue == yypact_ninf_;
  }

  inline bool
  Parser::yy_table_value_is_error_ (int yyvalue)
  {
    return yyvalue == yytable_ninf_;
  }

  int
  Parser::parse ()
  {
    /// Whether yyla contains a lookahead.
    bool yyempty = true;

    // State.
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The locations where the error started and ended.
    stack_symbol_type yyerror_range[3];

    /// The return value of parse ().
    int yyresult;

    // FIXME: This shoud be completely indented.  It is not yet to
    // avoid gratuitous conflicts when merging into the master branch.
    try
      {
    YYCDEBUG << "Starting parse" << std::endl;


    // User initialization code.
    #line 38 "Parser.yy" // lalr1.cc:725
{
    yyla.location.begin.filename = yyla.location.end.filename = new std::string("stdin");
}

#line 471 "Parser.tab.cc" // lalr1.cc:725

    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, yyla);

    // A new symbol was pushed on the stack.
  yynewstate:
    YYCDEBUG << "Entering state " << yystack_[0].state << std::endl;

    // Accept?
    if (yystack_[0].state == yyfinal_)
      goto yyacceptlab;

    goto yybackup;

    // Backup.
  yybackup:

    // Try to take a decision without lookahead.
    yyn = yypact_[yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyempty)
      {
        YYCDEBUG << "Reading a token: ";
        try
          {
            yyla.type = yytranslate_ (yylex (&yyla.value, &yyla.location));
          }
        catch (const syntax_error& yyexc)
          {
            error (yyexc);
            goto yyerrlab1;
          }
        yyempty = false;
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.type_get ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.type_get ())
      goto yydefault;

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Discard the token being shifted.
    yyempty = true;

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", yyn, yyla);
    goto yynewstate;

  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;

  /*-----------------------------.
  | yyreduce -- Do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_(yystack_[yylen].state, yyr1_[yyn]);
      /* If YYLEN is nonzero, implement the default value of the
         action: '$$ = $1'.  Otherwise, use the top of the stack.

         Otherwise, the following line sets YYLHS.VALUE to garbage.
         This behavior is undocumented and Bison users should not rely
         upon it.  */
      if (yylen)
        yylhs.value = yystack_[yylen - 1].value;
      else
        yylhs.value = yystack_[0].value;

      // Compute the default @$.
      {
        slice<stack_symbol_type, stack_type> slice (yystack_, yylen);
        YYLLOC_DEFAULT (yylhs.location, slice, yylen);
      }

      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
      try
        {
          switch (yyn)
            {
  case 4:
#line 700 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtQueryValue);}
#line 585 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 5:
#line 701 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtInsertItemValue);}
#line 591 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 6:
#line 702 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtUpdateItemValue);}
#line 597 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 7:
#line 703 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtDeleteItemValue);}
#line 603 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 8:
#line 704 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtTableValue);}
#line 609 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 9:
#line 705 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtTableValue);}
#line 615 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 10:
#line 706 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtSyncerValue);}
#line 621 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 11:
#line 707 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtIndexValue);}
#line 627 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 12:
#line 708 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtTableValue);}
#line 633 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 13:
#line 709 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtDatasetValue);}
#line 639 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 14:
#line 710 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtDatascannerValue);}
#line 645 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 15:
#line 711 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtDatascannerValue);}
#line 651 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 16:
#line 712 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtDatascannerValue);}
#line 657 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 17:
#line 713 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtDatascannerValue);}
#line 663 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 18:
#line 714 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtSchemaValue);}
#line 669 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 19:
#line 715 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtSchemaValue);}
#line 675 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 20:
#line 716 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtDatasetValue);}
#line 681 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 21:
#line 717 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtIndexValue);}
#line 687 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 22:
#line 718 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtTableValue);}
#line 693 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 23:
#line 719 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtSyncerValue);}
#line 699 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 24:
#line 720 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtDebugValue);}
#line 705 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 25:
#line 721 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtSchemaValue);}
#line 711 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 26:
#line 722 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtDatasetValue);}
#line 717 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 27:
#line 723 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtSchemaValue);}
#line 723 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 28:
#line 724 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtDatasetValue);}
#line 729 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 29:
#line 725 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtIndexValue);}
#line 735 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 30:
#line 726 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtIndexValue);}
#line 741 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 31:
#line 727 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtScheduleValue);}
#line 747 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 32:
#line 728 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtScheduleValue);}
#line 753 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 33:
#line 729 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtScheduleValue);}
#line 759 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 34:
#line 730 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtScheduleValue);}
#line 765 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 35:
#line 731 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtJobValue);}
#line 771 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 36:
#line 732 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtJobValue);}
#line 777 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 37:
#line 733 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtJobValue);}
#line 783 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 38:
#line 734 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtJobValue);}
#line 789 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 39:
#line 735 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtJobValue);}
#line 795 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 40:
#line 736 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtJobValue);}
#line 801 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 41:
#line 737 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtJobValue);}
#line 807 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 42:
#line 738 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtJobValue);}
#line 813 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 43:
#line 739 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtJobValue);}
#line 819 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 44:
#line 740 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtJobValue);}
#line 825 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 45:
#line 741 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtTaskValue);}
#line 831 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 46:
#line 742 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtTaskValue);}
#line 837 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 47:
#line 743 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtTaskValue);}
#line 843 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 48:
#line 744 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtTaskValue);}
#line 849 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 49:
#line 745 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtDataRecordValue);}
#line 855 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 50:
#line 746 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtDataRecordValue);}
#line 861 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 51:
#line 747 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtDataRecordValue);}
#line 867 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 52:
#line 748 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtDataRecordValue);}
#line 873 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 53:
#line 749 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtDataFieldValue);}
#line 879 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 54:
#line 750 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtDataFieldValue);}
#line 885 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 55:
#line 751 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtDataFieldValue);}
#line 891 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 56:
#line 752 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtDataFieldValue);}
#line 897 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 57:
#line 753 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtDataProcValue);}
#line 903 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 58:
#line 754 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtDataProcValue);}
#line 909 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 59:
#line 755 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtDataProcValue);}
#line 915 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 60:
#line 756 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtDataProcValue);}
#line 921 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 61:
#line 757 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtDataConnectorValue);}
#line 927 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 62:
#line 758 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtDataConnectorValue);}
#line 933 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 63:
#line 759 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtDataConnectorValue);}
#line 939 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 64:
#line 760 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtDataConnectorValue);}
#line 945 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 65:
#line 761 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtIndexValue);}
#line 951 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 66:
#line 762 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtSequenceValue);}
#line 957 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 67:
#line 763 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtDatabaseValue);}
#line 963 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 68:
#line 764 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtDatabaseValue);}
#line 969 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 69:
#line 765 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtDatabaseValue);}
#line 975 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 70:
#line 766 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtDatabaseValue);}
#line 981 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 71:
#line 767 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtLoadDataValue);}
#line 987 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 72:
#line 768 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtUserMgrValue);}
#line 993 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 73:
#line 769 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtUserMgrValue);}
#line 999 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 74:
#line 770 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtUserMgrValue);}
#line 1005 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 75:
#line 771 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtUserMgrValue);}
#line 1011 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 76:
#line 772 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtUserMgrValue);}
#line 1017 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 77:
#line 773 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtRunScriptFileValue);}
#line 1023 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 78:
#line 774 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtStatisticsValue);}
#line 1029 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 79:
#line 775 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtStatisticsValue);}
#line 1035 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 80:
#line 776 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtStatisticsValue);}
#line 1041 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 81:
#line 777 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtCompleteIILValue);}
#line 1047 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 82:
#line 778 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtStatValue);}
#line 1053 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 83:
#line 779 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtDocValue);}
#line 1059 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 84:
#line 780 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtDocValue);}
#line 1065 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 85:
#line 781 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtJoinSyncherValue);}
#line 1071 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 86:
#line 782 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtDictValues);}
#line 1077 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 87:
#line 783 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtDictValues);}
#line 1083 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 88:
#line 784 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtDictValues);}
#line 1089 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 89:
#line 785 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtDictValues);}
#line 1095 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 90:
#line 786 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtMapValues);}
#line 1101 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 91:
#line 787 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtMapValues);}
#line 1107 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 92:
#line 788 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtMapValues);}
#line 1113 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 93:
#line 789 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtMapValues);}
#line 1119 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 94:
#line 790 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtMapValues);}
#line 1125 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 95:
#line 791 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtDistributionMapValues);}
#line 1131 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 96:
#line 792 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtDistributionMapValues);}
#line 1137 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 97:
#line 793 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtDistributionMapValues);}
#line 1143 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 98:
#line 794 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtDistributionMapValues);}
#line 1149 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 99:
#line 795 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtHBaseValue);}
#line 1155 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 100:
#line 796 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtServiceValue);}
#line 1161 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 101:
#line 797 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtServiceValue);}
#line 1167 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 102:
#line 798 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtServiceValue);}
#line 1173 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 103:
#line 799 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtServiceValue);}
#line 1179 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 104:
#line 800 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtServiceValue);}
#line 1185 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 105:
#line 801 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtServiceValue);}
#line 1191 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 106:
#line 802 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtNickFieldValue);}
#line 1197 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 107:
#line 803 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtFieldExprValue);}
#line 1203 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 108:
#line 804 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtVirtualFieldValue);}
#line 1209 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 109:
#line 805 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtVirtualFieldValue);}
#line 1215 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 110:
#line 806 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtVirtualFieldValue);}
#line 1221 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 111:
#line 807 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtVirtualFieldValue);}
#line 1227 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 112:
#line 808 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtJimoLogicValue);}
#line 1233 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 113:
#line 809 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtJimoLogicValue);}
#line 1239 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 114:
#line 810 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtJimoLogicValue);}
#line 1245 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 115:
#line 811 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtJimoLogicValue);}
#line 1251 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 116:
#line 812 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtQueryUnionValue);}
#line 1257 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 117:
#line 813 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosPJqlStmtExprValue);}
#line 1263 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 118:
#line 814 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtActorValue);}
#line 1269 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 119:
#line 815 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtActorValue);}
#line 1275 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 120:
#line 816 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtGenericobjValue);}
#line 1281 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 121:
#line 817 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlSelectIntoValue);}
#line 1287 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 122:
#line 818 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtAssignmentValue);}
#line 1293 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 123:
#line 819 "Parser.yy" // lalr1.cc:847
    {(yylhs.value.AosJqlStatementValue) = (yystack_[0].value.AosJqlStmtIFValue);}
#line 1299 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 124:
#line 825 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtActor *actor= new AosJqlStmtActor;
	           actor->setName((yystack_[5].value.strval));
	           actor->setClassName((yystack_[4].value.strval));
	           actor->setPath((yystack_[3].value.strval));
	           actor->setType_language((yystack_[2].value.strval));
	           actor->setDescription((yystack_[1].value.strval));
			   gAosJQLParser.appendStatement(actor);
		       (yylhs.value.AosJqlStmtActorValue) = actor;
			   (yylhs.value.AosJqlStmtActorValue)->setOp(JQLTypes::eCreate);
    }
#line 1315 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 125:
#line 840 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtActor *actor = new AosJqlStmtActor;

		actor->setName((yystack_[4].value.strval));
		actor->setKeys((yystack_[3].value.AosExprListValue));
		actor->setValues((yystack_[1].value.AosExprListValue));
		gAosJQLParser.appendStatement(actor);

		(yylhs.value.AosJqlStmtActorValue) = actor;
		(yylhs.value.AosJqlStmtActorValue)->setOp(JQLTypes::eRun);
	}
#line 1331 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 126:
#line 855 "Parser.yy" // lalr1.cc:847
    {
			(yylhs.value.AosExprListValue) = (yystack_[0].value.AosExprListValue);
		}
#line 1339 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 127:
#line 862 "Parser.yy" // lalr1.cc:847
    {
			(yylhs.value.AosExprListValue) = (yystack_[0].value.AosExprListValue);
		}
#line 1347 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 128:
#line 868 "Parser.yy" // lalr1.cc:847
    {
		OmnString name = (yystack_[2].value.strval);
		AosJqlStmtAssignment *stmt = new AosJqlStmtAssignment();
		stmt->setName(name);
		stmt->setType((yystack_[1].value.AosJQLDataFieldTypeInfoPtr));
		(yylhs.value.AosJqlStmtAssignmentValue) = stmt;
		(yylhs.value.AosJqlStmtAssignmentValue)->setOp(JQLTypes::eCreate);
		gAosJQLParser.appendStatement(stmt);
	}
#line 1361 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 129:
#line 879 "Parser.yy" // lalr1.cc:847
    {
		OmnString name = (yystack_[4].value.strval);
		AosJqlStmtAssignment *stmt = new AosJqlStmtAssignment();
		stmt->setName(name);
		stmt->setType((yystack_[3].value.AosJQLDataFieldTypeInfoPtr));
		stmt->setValue((yystack_[1].value.AosExprValue));
		(yylhs.value.AosJqlStmtAssignmentValue) = stmt;
		(yylhs.value.AosJqlStmtAssignmentValue)->setOp(JQLTypes::eCreate);
		gAosJQLParser.appendStatement(stmt);
	}
#line 1376 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 130:
#line 891 "Parser.yy" // lalr1.cc:847
    {
		OmnString name = (yystack_[3].value.strval);
		AosJqlStmtAssignment *stmt = new AosJqlStmtAssignment();
		stmt->setName(name);
		stmt->setValue((yystack_[1].value.AosExprValue));
		(yylhs.value.AosJqlStmtAssignmentValue) = stmt;
		(yylhs.value.AosJqlStmtAssignmentValue)->setOp(JQLTypes::eCreate);
		gAosJQLParser.appendStatement(stmt);
	}
#line 1390 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 131:
#line 902 "Parser.yy" // lalr1.cc:847
    {
		OmnString name = (yystack_[6].value.strval);
		AosJqlStmtAssignment *stmt = new AosJqlStmtAssignment();
		stmt->setName(name);
		stmt->setIsParms(true);
		stmt->setParms((yystack_[2].value.ll_value));
		(yylhs.value.AosJqlStmtAssignmentValue) = stmt;
		(yylhs.value.AosJqlStmtAssignmentValue)->setOp(JQLTypes::eCreate);
		gAosJQLParser.appendStatement(stmt);
	}
#line 1405 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 132:
#line 914 "Parser.yy" // lalr1.cc:847
    {
		OmnString name = (yystack_[7].value.strval);
		AosJqlStmtAssignment *stmt = new AosJqlStmtAssignment();
		stmt->setName(name);
		stmt->setType((yystack_[6].value.AosJQLDataFieldTypeInfoPtr));
		stmt->setIsParms(true);
		stmt->setParms((yystack_[2].value.ll_value));
		(yylhs.value.AosJqlStmtAssignmentValue) = stmt;
		(yylhs.value.AosJqlStmtAssignmentValue)->setOp(JQLTypes::eCreate);
		gAosJQLParser.appendStatement(stmt);
	}
#line 1421 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 133:
#line 937 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDataConnector *data_connector = new AosJqlStmtDataConnector;
		data_connector->setType((yystack_[13].value.strval));
		data_connector->setName((yystack_[10].value.strval));
		data_connector->setSvrId((yystack_[7].value.ll_value));
		data_connector->setFileName((yystack_[5].value.strval));
		data_connector->setCoding((yystack_[2].value.strval));
		data_connector->setSplitSize((yystack_[1].value.strval));
		gAosJQLParser.appendStatement(data_connector);
		(yylhs.value.AosJqlStmtDataConnectorValue) = data_connector;
		(yylhs.value.AosJqlStmtDataConnectorValue)->setOp(JQLTypes::eCreate);
	}
#line 1438 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 134:
#line 954 "Parser.yy" // lalr1.cc:847
    {
		//	[FILENAME MATCH PATTERN pattern]
		AosJqlStmtDataConnector *data_connector = new AosJqlStmtDataConnector;
		data_connector->setType("DIRECTORY");
		data_connector->setName((yystack_[8].value.strval));
		data_connector->setDirList((yystack_[6].value.AosVecDirListValue));
		data_connector->setCoding((yystack_[2].value.strval));
		data_connector->setSplitSize((yystack_[1].value.strval));
		gAosJQLParser.appendStatement(data_connector);
		(yylhs.value.AosJqlStmtDataConnectorValue) = data_connector;
		(yylhs.value.AosJqlStmtDataConnectorValue)->setOp(JQLTypes::eCreate);
	}
#line 1455 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 135:
#line 968 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDataConnector *stmt = new AosJqlStmtDataConnector;
		stmt->setName((yystack_[4].value.strval));
		stmt->setConfParms((yystack_[2].value.AosExprNameValuesValue));
		gAosJQLParser.appendStatement(stmt);
		(yylhs.value.AosJqlStmtDataConnectorValue) = stmt;
		(yylhs.value.AosJqlStmtDataConnectorValue)->setOp(JQLTypes::eCreate);
	}
#line 1468 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 136:
#line 978 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = NULL;
	}
#line 1476 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 137:
#line 983 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = (yystack_[0].value.strval);	
	}
#line 1484 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 138:
#line 990 "Parser.yy" // lalr1.cc:847
    {
	 	typedef AosJqlStmtDataConnector::AosDirList DirList; 
		vector<DirList*> *dls = new vector<DirList*>;
	 	DirList *dl = new DirList();  
		dl->mFileName = (yystack_[3].value.strval);
		dl->mSvrId = (yystack_[0].value.ll_value);
		(yylhs.value.AosVecDirListValue) = dls;
		(yylhs.value.AosVecDirListValue)->push_back(dl);
	 }
#line 1498 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 139:
#line 1001 "Parser.yy" // lalr1.cc:847
    {
	 	typedef AosJqlStmtDataConnector::AosDirList DirList; 
	 	DirList *dl = new DirList();  
		dl->mFileName = (yystack_[3].value.strval);
		dl->mSvrId = (yystack_[0].value.ll_value);
		(yylhs.value.AosVecDirListValue)->push_back(dl);
	 }
#line 1510 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 140:
#line 1013 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDataConnector *data_connector = new AosJqlStmtDataConnector;
		data_connector->setName((yystack_[1].value.strval));
		gAosJQLParser.appendStatement(data_connector);
		(yylhs.value.AosJqlStmtDataConnectorValue) = data_connector;
		(yylhs.value.AosJqlStmtDataConnectorValue)->setOp(JQLTypes::eDrop);
	}
#line 1522 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 141:
#line 1024 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDataConnector *data_connector = new AosJqlStmtDataConnector;
		gAosJQLParser.appendStatement(data_connector);
		(yylhs.value.AosJqlStmtDataConnectorValue) = data_connector;
		(yylhs.value.AosJqlStmtDataConnectorValue)->setOp(JQLTypes::eShow);
	}
#line 1533 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 142:
#line 1034 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDataConnector *data_connector = new AosJqlStmtDataConnector;
		data_connector->setName((yystack_[1].value.strval));
		gAosJQLParser.appendStatement(data_connector);
		(yylhs.value.AosJqlStmtDataConnectorValue) = data_connector;
		(yylhs.value.AosJqlStmtDataConnectorValue)->setOp(JQLTypes::eDescribe);
	}
#line 1545 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 143:
#line 1049 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDataField *data_field = new AosJqlStmtDataField;
		data_field->setName((yystack_[7].value.strval));
		data_field->setType((yystack_[5].value.strval));
		data_field->setIdfamily((yystack_[4].value.strval));
		data_field->setMaxLen((yystack_[3].value.ll_value));
		data_field->setOffset((yystack_[2].value.ll_value));
		data_field->setDefValue((yystack_[1].value.strval));
		gAosJQLParser.appendStatement(data_field);
		(yylhs.value.AosJqlStmtDataFieldValue) = data_field;
		(yylhs.value.AosJqlStmtDataFieldValue)->setOp(JQLTypes::eCreate);
	}
#line 1562 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 144:
#line 1067 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDataField *data_field = new AosJqlStmtDataField;
		OmnString mode = (yystack_[3].value.strval);

		data_field->setName((yystack_[12].value.strval));
		data_field->setType((yystack_[10].value.strval));
		data_field->setFromFieldName((yystack_[7].value.strval));
		data_field->setIILName((yystack_[4].value.strval));
		if (mode == "combine")
			data_field->setNeedSplit(true);
		else
			data_field->setNeedSplit(false);

		if (mode == "single")
			data_field->setUseKeyAsValue(true);
		else
			data_field->setUseKeyAsValue(false);

		data_field->setNeedSwap(false);

		gAosJQLParser.appendStatement(data_field);
		(yylhs.value.AosJqlStmtDataFieldValue) = data_field;
		(yylhs.value.AosJqlStmtDataFieldValue)->setOp(JQLTypes::eCreate);
	}
#line 1591 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 145:
#line 1095 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDataField *data_field = new AosJqlStmtDataField;
		data_field->setName((yystack_[11].value.strval));
		data_field->setType((yystack_[9].value.strval));
		data_field->setFromFieldName((yystack_[5].value.strval));
		data_field->setFromName((yystack_[3].value.strval));
		data_field->setToName((yystack_[1].value.strval));
		gAosJQLParser.appendStatement(data_field);
		(yylhs.value.AosJqlStmtDataFieldValue) = data_field;
		(yylhs.value.AosJqlStmtDataFieldValue)->setOp(JQLTypes::eCreate);
	}
#line 1607 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 146:
#line 1108 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDataField* stmt = new AosJqlStmtDataField;
		stmt->setName((yystack_[4].value.strval));
		stmt->setConfParms((yystack_[2].value.AosExprNameValuesValue));
		gAosJQLParser.appendStatement(stmt);
		(yylhs.value.AosJqlStmtDataFieldValue) = stmt;
		(yylhs.value.AosJqlStmtDataFieldValue)->setOp(JQLTypes::eCreate);
	}
#line 1620 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 147:
#line 1119 "Parser.yy" // lalr1.cc:847
    {	
		(yylhs.value.ll_value) = 0;
	}
#line 1628 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 148:
#line 1124 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.ll_value) = (yystack_[0].value.ll_value);
	}
#line 1636 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 149:
#line 1130 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = NULL;
	}
#line 1644 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 150:
#line 1135 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = (yystack_[0].value.strval); 
	}
#line 1652 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 151:
#line 1141 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = NULL;
	}
#line 1660 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 152:
#line 1146 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = (yystack_[0].value.strval);
	}
#line 1668 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 153:
#line 1150 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.ll_value) = -1;
	}
#line 1676 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 154:
#line 1155 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.ll_value) = (yystack_[0].value.ll_value);
	}
#line 1684 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 155:
#line 1162 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDataField *data_field = new AosJqlStmtDataField;
		data_field->setName((yystack_[1].value.strval));
		gAosJQLParser.appendStatement(data_field);
		(yylhs.value.AosJqlStmtDataFieldValue) = data_field;
		(yylhs.value.AosJqlStmtDataFieldValue)->setOp(JQLTypes::eDrop);
	}
#line 1696 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 156:
#line 1173 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDataField *data_field = new AosJqlStmtDataField;
		gAosJQLParser.appendStatement(data_field);
		(yylhs.value.AosJqlStmtDataFieldValue) = data_field;
		(yylhs.value.AosJqlStmtDataFieldValue)->setOp(JQLTypes::eShow);
	}
#line 1707 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 157:
#line 1183 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDataField *data_field = new AosJqlStmtDataField;
		data_field->setName((yystack_[1].value.strval));
		gAosJQLParser.appendStatement(data_field);
		(yylhs.value.AosJqlStmtDataFieldValue) = data_field;
		(yylhs.value.AosJqlStmtDataFieldValue)->setOp(JQLTypes::eDescribe);
	}
#line 1719 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 158:
#line 1194 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = OmnString("combine").getBuffer();
	}
#line 1727 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 159:
#line 1199 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = OmnString("single").getBuffer();
	}
#line 1735 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 160:
#line 1208 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDataProc *data_proc = new AosJqlStmtDataProc;
		data_proc->setName((yystack_[7].value.strval));
		data_proc->setType("compose");
		data_proc->setInputs((yystack_[4].value.AosExprListValue));
		data_proc->setOutput((yystack_[1].value.strval));

		gAosJQLParser.appendStatement(data_proc);
		(yylhs.value.AosJqlStmtDataProcValue) = data_proc;
		(yylhs.value.AosJqlStmtDataProcValue)->setOp(JQLTypes::eCreate);
	}
#line 1751 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 161:
#line 1225 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDataProc *data_proc = new AosJqlStmtDataProc;
		data_proc->setName((yystack_[10].value.strval));
		data_proc->setType("index");
		data_proc->setDatasetName((yystack_[8].value.strval));
		data_proc->setDataRecordName((yystack_[5].value.strval));
		data_proc->setOperator((yystack_[3].value.strval));
		data_proc->setIILName((yystack_[1].value.strval));
		gAosJQLParser.appendStatement(data_proc);
		(yylhs.value.AosJqlStmtDataProcValue) = data_proc;
		(yylhs.value.AosJqlStmtDataProcValue)->setOp(JQLTypes::eCreate);
	}
#line 1768 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 162:
#line 1242 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDataProc *data_proc = new AosJqlStmtDataProc;
		data_proc->setName((yystack_[9].value.strval));
		data_proc->setType("doc");
		data_proc->setDatasetName((yystack_[7].value.strval));
		data_proc->setDataRecordName((yystack_[4].value.strval));
		data_proc->setDocType((yystack_[1].value.strval));
		gAosJQLParser.appendStatement(data_proc);
		(yylhs.value.AosJqlStmtDataProcValue) = data_proc;
		(yylhs.value.AosJqlStmtDataProcValue)->setOp(JQLTypes::eCreate);
	}
#line 1784 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 163:
#line 1255 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDataProc *stmt = new AosJqlStmtDataProc;
		stmt->setName((yystack_[4].value.strval));
		stmt->setConfParms((yystack_[2].value.AosExprNameValuesValue));
		gAosJQLParser.appendStatement(stmt);
		(yylhs.value.AosJqlStmtDataProcValue) = stmt;
		(yylhs.value.AosJqlStmtDataProcValue)->setOp(JQLTypes::eCreate);
	}
#line 1797 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 164:
#line 1267 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDataProc *data_proc = new AosJqlStmtDataProc;
		data_proc->setName((yystack_[1].value.strval));
		gAosJQLParser.appendStatement(data_proc);
		(yylhs.value.AosJqlStmtDataProcValue) = data_proc;
		(yylhs.value.AosJqlStmtDataProcValue)->setOp(JQLTypes::eDrop);
	}
#line 1809 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 165:
#line 1278 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDataProc *data_proc = new AosJqlStmtDataProc;
		gAosJQLParser.appendStatement(data_proc);
		(yylhs.value.AosJqlStmtDataProcValue) = data_proc;
		(yylhs.value.AosJqlStmtDataProcValue)->setOp(JQLTypes::eShow);
	}
#line 1820 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 166:
#line 1288 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDataProc *data_proc = new AosJqlStmtDataProc;
		data_proc->setName((yystack_[1].value.strval));
		gAosJQLParser.appendStatement(data_proc);
		(yylhs.value.AosJqlStmtDataProcValue) = data_proc;
		(yylhs.value.AosJqlStmtDataProcValue)->setOp(JQLTypes::eDescribe);
	}
#line 1832 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 167:
#line 1299 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDataRecord *stmt = new AosJqlStmtDataRecord;
		stmt->setName((yystack_[4].value.strval));
		stmt->setConfParms((yystack_[2].value.AosExprNameValuesValue));
		gAosJQLParser.appendStatement(stmt);
		(yylhs.value.AosJqlStmtDataRecordValue) = stmt;
		(yylhs.value.AosJqlStmtDataRecordValue)->setOp(JQLTypes::eCreate);
	}
#line 1845 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 168:
#line 1314 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDataRecord *data_record = new AosJqlStmtDataRecord;
		OmnString name = (yystack_[9].value.strval);
		data_record->setName(name);
		data_record->setType((yystack_[12].value.strval));
		data_record->setRecordLength((yystack_[8].value.ll_value));
		data_record->setFieldNames((yystack_[5].value.AosExprListValue));
		data_record->setRecordDelimiter((yystack_[3].value.strval));
		data_record->setFieldDelimiter((yystack_[2].value.strval));
		data_record->setTextQualidier((yystack_[1].value.strval));
		gAosJQLParser.appendStatement(data_record);
		(yylhs.value.AosJqlStmtDataRecordValue) = data_record;
		(yylhs.value.AosJqlStmtDataRecordValue)->setOp(JQLTypes::eCreate);
	}
#line 1864 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 169:
#line 1338 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDataRecord *data_record = new AosJqlStmtDataRecord;
		OmnString type = "schema";
		OmnString name = (yystack_[14].value.strval);
		data_record->setName(name);
		data_record->setRecordType(type);
		data_record->setType((yystack_[11].value.strval));
		data_record->setRecordLength((yystack_[10].value.ll_value));
		data_record->setTrimCondition((yystack_[9].value.strval));
		data_record->setScheamPicker((yystack_[8].value.AosExprValue));
		data_record->setFieldNames((yystack_[5].value.AosExprListValue));
		data_record->setRecordDelimiter((yystack_[3].value.strval));
		data_record->setFieldDelimiter((yystack_[2].value.strval));
		data_record->setTextQualidier((yystack_[1].value.strval));
		gAosJQLParser.appendStatement(data_record);
		(yylhs.value.AosJqlStmtDataRecordValue) = data_record;
		(yylhs.value.AosJqlStmtDataRecordValue)->setOp(JQLTypes::eCreate);
	}
#line 1887 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 170:
#line 1359 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = OmnString("csv").getBuffer();
	}
#line 1895 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 171:
#line 1364 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = OmnString("fixbin").getBuffer();
	}
#line 1903 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 172:
#line 1369 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = OmnString("multi").getBuffer();
	}
#line 1911 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 173:
#line 1374 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = NULL;
	}
#line 1919 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 174:
#line 1379 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = (yystack_[0].value.strval);	
	}
#line 1927 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 175:
#line 1385 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosExprListValue) = new AosExprList; 
		(yylhs.value.AosExprListValue)->push_back((yystack_[0].value.AosExprValue));
	}
#line 1936 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 176:
#line 1391 "Parser.yy" // lalr1.cc:847
    {
		 (yylhs.value.AosExprListValue)->push_back((yystack_[0].value.AosExprValue));
	}
#line 1944 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 177:
#line 1397 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosExprValue) = (yystack_[0].value.AosExprValue);
	}
#line 1952 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 178:
#line 1408 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDataFieldPtr data_field = OmnNew AosJqlStmtDataField; 
		//data_field->setName($1->getValue(0).toLower());
		data_field->setName((yystack_[9].value.AosExprValue)->getValue(0));
		data_field->setIsInfoField((yystack_[8].value.bool_val));
		data_field->setType((yystack_[6].value.AosJQLDataFieldTypeInfoPtr));
		data_field->setFormater((yystack_[5].value.strval));
		data_field->setIdfamily((yystack_[4].value.strval));
		data_field->setMaxLen((yystack_[3].value.ll_value));
		data_field->setOffset((yystack_[2].value.ll_value)); 
		data_field->setDefValue((yystack_[1].value.strval));
		data_field->setMapping((yystack_[0].value.AosExprListValue));
		gAosJQLParser.appendStatement(data_field); 
		data_field->setOp(JQLTypes::eCreate); 
		//$$ = new AosExprString(($1->getValue(0).toLower()).data());
		(yylhs.value.AosExprValue) = new AosExprString(((yystack_[9].value.AosExprValue)->getValue(0)).data());
	}
#line 1974 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 179:
#line 1434 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDataField *data_field = new AosJqlStmtDataField; 
		//data_field->setName($1->getValue(0).toLower());
		data_field->setName((yystack_[11].value.AosExprValue)->getValue(0));
		data_field->setIsInfoField((yystack_[10].value.bool_val));
		data_field->setType((yystack_[8].value.AosJQLDataFieldTypeInfoPtr));                                      
        data_field->setDataType((yystack_[7].value.AosJQLDataFieldTypeInfoPtr));
		data_field->setFormater((yystack_[6].value.strval));
		if ((yystack_[4].value.AosExprValue))
		{
			data_field->setValue((yystack_[4].value.AosExprValue)->dump());
		}
		data_field->setIdfamily((yystack_[3].value.strval));
		data_field->setMaxLen((yystack_[2].value.ll_value));
		data_field->setOffset((yystack_[1].value.ll_value)); 
		data_field->setDefValue((yystack_[0].value.strval));
		gAosJQLParser.appendStatement(data_field); 
		data_field->setOp(JQLTypes::eCreate); 
		//$$ = new AosExprString(($1->getValue(0).toLower()).data());
		(yylhs.value.AosExprValue) = new AosExprString(((yystack_[11].value.AosExprValue)->getValue(0)).data());
	}
#line 2000 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 180:
#line 1463 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDataField *data_field = new AosJqlStmtDataField; 
		//data_field->setName($1->getValue(0).toLower());
		data_field->setName((yystack_[10].value.AosExprValue)->getValue(0));
		data_field->setIsInfoField((yystack_[9].value.bool_val));
		data_field->setType((yystack_[7].value.AosJQLDataFieldTypeInfoPtr)); 
		data_field->setFormater((yystack_[6].value.strval));
		data_field->setIdfamily((yystack_[5].value.strval));
		data_field->setMaxLen((yystack_[4].value.ll_value));
		data_field->setOffset((yystack_[3].value.ll_value)); 
		data_field->setDefValue((yystack_[2].value.strval));
		data_field->setNOTNULL("true");
		gAosJQLParser.appendStatement(data_field); 
		data_field->setOp(JQLTypes::eCreate); 
		//$$ = new AosExprString(($1->getValue(0).toLower()).data());
		(yylhs.value.AosExprValue) = new AosExprString(((yystack_[10].value.AosExprValue)->getValue(0)).data());
	}
#line 2022 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 181:
#line 1482 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = "";
	}
#line 2030 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 182:
#line 1487 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = (yystack_[0].value.strval);
	}
#line 2038 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 183:
#line 1493 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJQLDataFieldTypeInfoPtr) = new AosJQLDataFieldTypeInfo();
		(yylhs.value.AosJQLDataFieldTypeInfoPtr)->setType((yystack_[0].value.strval));
	}
#line 2047 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 184:
#line 1499 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJQLDataFieldTypeInfoPtr) = new AosJQLDataFieldTypeInfo();
		(yylhs.value.AosJQLDataFieldTypeInfoPtr)->setType("double");
	}
#line 2056 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 185:
#line 1505 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJQLDataFieldTypeInfoPtr) = new AosJQLDataFieldTypeInfo();
		(yylhs.value.AosJQLDataFieldTypeInfoPtr)->setType("datetime");
	}
#line 2065 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 186:
#line 1511 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJQLDataFieldTypeInfoPtr) = new AosJQLDataFieldTypeInfo();
		(yylhs.value.AosJQLDataFieldTypeInfoPtr)->setType("varchar");
		(yylhs.value.AosJQLDataFieldTypeInfoPtr)->setValue((yystack_[1].value.ll_value), 0);
	}
#line 2075 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 187:
#line 1518 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJQLDataFieldTypeInfoPtr) = new AosJQLDataFieldTypeInfo();
		(yylhs.value.AosJQLDataFieldTypeInfoPtr)->setType("varchar");
		(yylhs.value.AosJQLDataFieldTypeInfoPtr)->setValue((yystack_[1].value.ll_value), 0);
	}
#line 2085 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 188:
#line 1525 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJQLDataFieldTypeInfoPtr) = new AosJQLDataFieldTypeInfo();
		(yylhs.value.AosJQLDataFieldTypeInfoPtr)->setType("varchar");
		(yylhs.value.AosJQLDataFieldTypeInfoPtr)->setValue((yystack_[1].value.ll_value), 0);
	}
#line 2095 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 189:
#line 1532 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJQLDataFieldTypeInfoPtr) = new AosJQLDataFieldTypeInfo();
		(yylhs.value.AosJQLDataFieldTypeInfoPtr)->setType("text");
	}
#line 2104 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 190:
#line 1538 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJQLDataFieldTypeInfoPtr) = new AosJQLDataFieldTypeInfo();
		(yylhs.value.AosJQLDataFieldTypeInfoPtr)->setType("number");
		(yylhs.value.AosJQLDataFieldTypeInfoPtr)->setValue((yystack_[3].value.ll_value), (yystack_[1].value.ll_value));
	}
#line 2114 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 191:
#line 1545 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJQLDataFieldTypeInfoPtr) = new AosJQLDataFieldTypeInfo();
		(yylhs.value.AosJQLDataFieldTypeInfoPtr)->setType("number");
		(yylhs.value.AosJQLDataFieldTypeInfoPtr)->setValue((yystack_[1].value.ll_value), 0);
	}
#line 2124 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 192:
#line 1552 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJQLDataFieldTypeInfoPtr) = new AosJQLDataFieldTypeInfo();
		(yylhs.value.AosJQLDataFieldTypeInfoPtr)->setType("decimal");
		(yylhs.value.AosJQLDataFieldTypeInfoPtr)->setValue((yystack_[3].value.ll_value), (yystack_[1].value.ll_value));
	}
#line 2134 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 193:
#line 1559 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJQLDataFieldTypeInfoPtr) = new AosJQLDataFieldTypeInfo();
		(yylhs.value.AosJQLDataFieldTypeInfoPtr)->setType("bigint");
		(yylhs.value.AosJQLDataFieldTypeInfoPtr)->setValue((yystack_[1].value.ll_value), 0);
	}
#line 2144 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 194:
#line 1566 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.ll_value) = 0;
	}
#line 2152 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 195:
#line 1571 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.ll_value) = (yystack_[0].value.ll_value);
	}
#line 2160 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 196:
#line 1576 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.ll_value) = (yystack_[0].value.ll_value);
	}
#line 2168 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 197:
#line 1581 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosExprValue) = 0;
	}
#line 2176 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 198:
#line 1586 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosExprValue) = (yystack_[0].value.AosExprValue);
	}
#line 2184 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 199:
#line 1591 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = NULL;
	}
#line 2192 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 200:
#line 1596 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = (yystack_[0].value.strval);
	}
#line 2200 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 201:
#line 1601 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = (yystack_[0].value.strval);
	}
#line 2208 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 202:
#line 1606 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosExprListValue) = NULL;
	}
#line 2216 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 203:
#line 1611 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosExprListValue) = (yystack_[1].value.AosExprListValue);	
	}
#line 2224 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 204:
#line 1616 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = NULL;
	}
#line 2232 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 205:
#line 1621 "Parser.yy" // lalr1.cc:847
    {
		/*$$ = $3->getValue(0).getBuffer();*/
		(yylhs.value.strval) = (yystack_[0].value.strval);
	}
#line 2241 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 206:
#line 1627 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = NULL;
	}
#line 2249 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 207:
#line 1636 "Parser.yy" // lalr1.cc:847
    {
		/*$$ = $3->getValue(0).getBuffer();*/
		(yylhs.value.strval) = (yystack_[0].value.strval);
	}
#line 2258 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 208:
#line 1643 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDataRecord *data_record = new AosJqlStmtDataRecord;
		data_record->setName((yystack_[1].value.strval));
		gAosJQLParser.appendStatement(data_record);
		(yylhs.value.AosJqlStmtDataRecordValue) = data_record;
		(yylhs.value.AosJqlStmtDataRecordValue)->setOp(JQLTypes::eDrop);
	}
#line 2270 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 209:
#line 1652 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDataRecord *data_record = new AosJqlStmtDataRecord;
		gAosJQLParser.appendStatement(data_record);
		OmnString name = (yystack_[1].value.strval);
		data_record->setName(name);
		(yylhs.value.AosJqlStmtDataRecordValue) = data_record;
		(yylhs.value.AosJqlStmtDataRecordValue)->setOp(JQLTypes::eDrop);
	}
#line 2283 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 210:
#line 1663 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDataRecord *data_record = new AosJqlStmtDataRecord;
		gAosJQLParser.appendStatement(data_record);
		(yylhs.value.AosJqlStmtDataRecordValue) = data_record;
		(yylhs.value.AosJqlStmtDataRecordValue)->setOp(JQLTypes::eShow);
	}
#line 2294 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 211:
#line 1671 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDataRecord *stmt = new AosJqlStmtDataRecord;
	 	gAosJQLParser.appendStatement(stmt);
		(yylhs.value.AosJqlStmtDataRecordValue) = stmt;
		(yylhs.value.AosJqlStmtDataRecordValue)->setOp(JQLTypes::eShow);
	}
#line 2305 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 212:
#line 1680 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDataRecord *data_record = new AosJqlStmtDataRecord;
		data_record->setName((yystack_[1].value.strval));
		gAosJQLParser.appendStatement(data_record);
		(yylhs.value.AosJqlStmtDataRecordValue) = data_record;
		(yylhs.value.AosJqlStmtDataRecordValue)->setOp(JQLTypes::eDescribe);
	}
#line 2317 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 213:
#line 1689 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDatabase* statement = new AosJqlStmtDatabase;
		
		statement->setName((yystack_[1].value.strval));

		gAosJQLParser.appendStatement(statement);

		(yylhs.value.AosJqlStmtDatabaseValue) = statement;
		(yylhs.value.AosJqlStmtDatabaseValue)->setOp(JQLTypes::eCreate);
		//cout << "Create database:" << " name " << $3 << endl;
	}
#line 2333 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 214:
#line 1703 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDatabase* statement = new AosJqlStmtDatabase;
		statement->setName((yystack_[1].value.strval));
		gAosJQLParser.appendStatement(statement);
		(yylhs.value.AosJqlStmtDatabaseValue) = statement;
		(yylhs.value.AosJqlStmtDatabaseValue)->setOp(JQLTypes::eDrop); 
	}
#line 2345 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 215:
#line 1714 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDatabase* statement = new AosJqlStmtDatabase;

		gAosJQLParser.appendStatement(statement);
		(yylhs.value.AosJqlStmtDatabaseValue) = statement;
		(yylhs.value.AosJqlStmtDatabaseValue)->setOp(JQLTypes::eShow); 
	}
#line 2357 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 216:
#line 1725 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDatabase* statement = new AosJqlStmtDatabase;

		gAosJQLParser.appendStatement(statement);
		statement->setName((yystack_[1].value.strval));
		(yylhs.value.AosJqlStmtDatabaseValue) = statement;
		(yylhs.value.AosJqlStmtDatabaseValue)->setOp(JQLTypes::eUse); 
	}
#line 2370 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 217:
#line 1736 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDatascanner* stmt = new AosJqlStmtDatascanner;
		stmt->setName((yystack_[4].value.strval));
		stmt->setConfParms((yystack_[2].value.AosExprNameValuesValue));
		gAosJQLParser.appendStatement(stmt);
		(yylhs.value.AosJqlStmtDatascannerValue) = stmt;
		(yylhs.value.AosJqlStmtDatascannerValue)->setOp(JQLTypes::eCreate);
	}
#line 2383 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 218:
#line 1746 "Parser.yy" // lalr1.cc:847
    {
	 	AosJqlStmtDatascanner* statement = new AosJqlStmtDatascanner;
		statement->setName((yystack_[5].value.strval));
		//OmnString conn_name = "";
		//conn_name << $4 << "_conn";
		//statement->setConnectorName(conn_name);
		statement->setConnectorName((yystack_[2].value.strval));

		if ((yystack_[1].value.bool_val)) statement->setOrder();
		gAosJQLParser.appendStatement(statement);

		(yylhs.value.AosJqlStmtDatascannerValue) = statement;
  		(yylhs.value.AosJqlStmtDatascannerValue)->setOp(JQLTypes::eCreate);
	}
#line 2402 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 219:
#line 1767 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDatascanner* statement = new AosJqlStmtDatascanner;
		statement->setName((yystack_[14].value.strval));
		OmnString conn_name = "";
		conn_name << (yystack_[14].value.strval) << "_conn";
		statement->setConnectorName(conn_name);
		statement->setConnectorType((yystack_[10].value.strval));
		statement->setSvrId((yystack_[7].value.ll_value));	
		statement->setFileName((yystack_[5].value.strval));
		statement->setEncoding((yystack_[2].value.strval));
		statement->setSplitSize((yystack_[1].value.strval));

		statement->setOrder();
		gAosJQLParser.appendStatement(statement);

		(yylhs.value.AosJqlStmtDatascannerValue) = statement;
		(yylhs.value.AosJqlStmtDatascannerValue)->setOp(JQLTypes::eCreate);
		//cout << "Create Datascanner: " << $4 << endl;

	}
#line 2427 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 220:
#line 1793 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDatascanner* statement = new AosJqlStmtDatascanner;
		statement->setName((yystack_[12].value.strval));
		OmnString conn_name = "";
		conn_name << (yystack_[12].value.strval) << "_conn";
		statement->setConnectorName(conn_name);
		statement->setConnectorType("DIRECTORY");
		statement->setDirList((yystack_[6].value.AosVecDirListValue));
		statement->setEncoding((yystack_[2].value.strval));
		statement->setSplitSize((yystack_[1].value.strval));

		//statement->setOrder();
		gAosJQLParser.appendStatement(statement);

		(yylhs.value.AosJqlStmtDatascannerValue) = statement;
		(yylhs.value.AosJqlStmtDatascannerValue)->setOp(JQLTypes::eCreate);
	}
#line 2449 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 221:
#line 1814 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.bool_val) = false;
	}
#line 2457 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 222:
#line 1819 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.bool_val) = true;	
	}
#line 2465 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 223:
#line 1824 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDatascanner *data_datascanner = new AosJqlStmtDatascanner;
		data_datascanner->setName((yystack_[1].value.strval));
		gAosJQLParser.appendStatement(data_datascanner);
		(yylhs.value.AosJqlStmtDatascannerValue) = data_datascanner;
		(yylhs.value.AosJqlStmtDatascannerValue)->setOp(JQLTypes::eDrop);
	}
#line 2477 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 224:
#line 1835 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDatascanner *data_datascanner = new AosJqlStmtDatascanner;
		gAosJQLParser.appendStatement(data_datascanner);
		(yylhs.value.AosJqlStmtDatascannerValue) = data_datascanner;
		(yylhs.value.AosJqlStmtDatascannerValue)->setOp(JQLTypes::eShow);
	}
#line 2488 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 225:
#line 1845 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDatascanner *data_datascanner = new AosJqlStmtDatascanner;
		data_datascanner->setName((yystack_[1].value.strval));
		gAosJQLParser.appendStatement(data_datascanner);
		(yylhs.value.AosJqlStmtDatascannerValue) = data_datascanner;
		(yylhs.value.AosJqlStmtDatascannerValue)->setOp(JQLTypes::eDescribe);
	}
#line 2500 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 226:
#line 1856 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDataset* stmt = new AosJqlStmtDataset;
		stmt->setName((yystack_[4].value.strval));
		stmt->setConfParms((yystack_[2].value.AosExprNameValuesValue));
		gAosJQLParser.appendStatement(stmt);
		(yylhs.value.AosJqlStmtDatasetValue) = stmt;
		(yylhs.value.AosJqlStmtDatasetValue)->setOp(JQLTypes::eCreate);
	}
#line 2513 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 227:
#line 1866 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDataset* statement = new AosJqlStmtDataset;

		statement->setName((yystack_[6].value.strval));
		statement->setScanner((yystack_[3].value.strval));
		statement->setSchema((yystack_[1].value.strval));
		gAosJQLParser.appendStatement(statement);

		(yylhs.value.AosJqlStmtDatasetValue) = statement;
		(yylhs.value.AosJqlStmtDatasetValue)->setOp(JQLTypes::eCreate);
		//cout << "Create dataset: " << $3 << endl;
	}
#line 2530 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 228:
#line 1880 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDataset* statement = new AosJqlStmtDataset;

		statement->setName((yystack_[7].value.strval));
		statement->setScanner((yystack_[4].value.strval));
		statement->setSchema((yystack_[1].value.strval));
		
		gAosJQLParser.appendStatement(statement);

		(yylhs.value.AosJqlStmtDatasetValue) = statement;
		(yylhs.value.AosJqlStmtDatasetValue)->setOp(JQLTypes::eCreate);
		//cout << "Create dataset: " << $3 << endl;
	}
#line 2548 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 229:
#line 1895 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDataset* statement = new AosJqlStmtDataset;
		statement->setName((yystack_[1].value.strval)); 
		gAosJQLParser.appendStatement(statement);  
		                                           
		(yylhs.value.AosJqlStmtDatasetValue) = statement;                            
		(yylhs.value.AosJqlStmtDatasetValue)->setOp(JQLTypes::eCreate);              
	}
#line 2561 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 230:
#line 1906 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDataset* statement = new AosJqlStmtDataset;

		gAosJQLParser.appendStatement(statement);

		(yylhs.value.AosJqlStmtDatasetValue) = statement;
		(yylhs.value.AosJqlStmtDatasetValue)->setOp(JQLTypes::eShow);
		//cout << "List all the Datasets" << endl;
	}
#line 2575 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 231:
#line 1918 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDataset* statement = new AosJqlStmtDataset;

		gAosJQLParser.appendStatement(statement);

		(yylhs.value.AosJqlStmtDatasetValue) = statement;
		(yylhs.value.AosJqlStmtDatasetValue)->setName((yystack_[1].value.strval));
		(yylhs.value.AosJqlStmtDatasetValue)->setOp(JQLTypes::eDescribe);
		//cout << "Display the Dataset: " << $3 << endl;
	}
#line 2590 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 232:
#line 1931 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDataset* statement = new AosJqlStmtDataset;
		statement->setName((yystack_[1].value.strval));

		gAosJQLParser.appendStatement(statement);
		(yylhs.value.AosJqlStmtDatasetValue) = statement;
		(yylhs.value.AosJqlStmtDatasetValue)->setOp(JQLTypes::eDrop);
	}
#line 2603 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 233:
#line 1943 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDebug* statement = new AosJqlStmtDebug;
		statement->setSwitch("on");
		gAosJQLParser.appendStatement(statement);
		(yylhs.value.AosJqlStmtDebugValue) = statement;
	}
#line 2614 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 234:
#line 1951 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDebug* statement = new AosJqlStmtDebug;
		statement->setSwitch("off");
		gAosJQLParser.appendStatement(statement);
		(yylhs.value.AosJqlStmtDebugValue) = statement;
	}
#line 2625 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 235:
#line 1963 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDelete *d = new AosJqlStmtDelete;
		d->table= (yystack_[2].value.AosExprValue);
		if ((yystack_[1].value.AosJqlWhereValue)) d->opt_where = (yystack_[1].value.AosJqlWhereValue)->getWhereExpr().getPtr();

		AosJqlStmtDeleteItem *del = new AosJqlStmtDeleteItem(d);	
		gAosJQLParser.appendStatement(del);
		(yylhs.value.AosJqlStmtDeleteItemValue) = del;
	}
#line 2639 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 236:
#line 1976 "Parser.yy" // lalr1.cc:847
    {
		//cout << "============ " << endl;
	}
#line 2647 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 237:
#line 1981 "Parser.yy" // lalr1.cc:847
    {
		//cout << "============  delete_opts LOW_PRIORITY  " << endl;
	}
#line 2655 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 238:
#line 1986 "Parser.yy" // lalr1.cc:847
    {
		//cout << "============  delete_opts QUICK " << endl;
	}
#line 2663 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 239:
#line 1991 "Parser.yy" // lalr1.cc:847
    {
		//cout << "============ delete_opts IGNORE " << endl;
	}
#line 2671 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 240:
#line 2021 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDictionary *stmt = new AosJqlStmtDictionary;
		stmt->setDictName((yystack_[9].value.strval));
		stmt->setTableName((yystack_[6].value.strval));
		stmt->setKeys((yystack_[3].value.AosExprListValue));
		if ((yystack_[1].value.AosJqlWhereValue)) stmt->setWhereCond((yystack_[1].value.AosJqlWhereValue)->getWhereExpr().getPtr());
		stmt->setOp(JQLTypes::eCreate);               
		gAosJQLParser.appendStatement(stmt);   
		(yylhs.value.AosJqlStmtDictValues) = stmt;                             
	}
#line 2686 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 241:
#line 2034 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDictionary *Dict = new AosJqlStmtDictionary;
		Dict->setDictName((yystack_[1].value.strval));
		gAosJQLParser.appendStatement(Dict);   
		(yylhs.value.AosJqlStmtDictValues) = Dict;                             
		(yylhs.value.AosJqlStmtDictValues)->setOp(JQLTypes::eDrop);               
	}
#line 2698 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 242:
#line 2044 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDictionary *Dict = new AosJqlStmtDictionary;
		gAosJQLParser.appendStatement(Dict);   
		(yylhs.value.AosJqlStmtDictValues) = Dict;                             
		(yylhs.value.AosJqlStmtDictValues)->setOp(JQLTypes::eShow);               
	}
#line 2709 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 243:
#line 2053 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDictionary *Dict = new AosJqlStmtDictionary;
		Dict->setDictName((yystack_[1].value.strval));
		gAosJQLParser.appendStatement(Dict);   
		(yylhs.value.AosJqlStmtDictValues) = Dict;                             
		(yylhs.value.AosJqlStmtDictValues)->setOp(JQLTypes::eDescribe);               
	}
#line 2721 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 244:
#line 2068 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDistributionMap *map = new AosJqlStmtDistributionMap;
		map->setMapName((yystack_[15].value.strval));
		map->setTableName((yystack_[13].value.strval));
		map->setKeys((yystack_[10].value.AosExprListValue));
		map->setMaxNums((yystack_[7].value.ll_value));
		map->setSpliteNum((yystack_[5].value.ll_value));
		map->setDataType((yystack_[3].value.strval));
		map->setMaxLen((yystack_[2].value.ll_value));
		if ((yystack_[1].value.AosJqlWhereValue))
			map->setWhereCond((yystack_[1].value.AosJqlWhereValue)->getWhereExpr().getPtr());
		map->setOp(JQLTypes::eCreate);               
		gAosJQLParser.appendStatement(map);   
		(yylhs.value.AosJqlStmtDistributionMapValues) = map;                             
	}
#line 2741 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 245:
#line 2086 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDistributionMap *map = new AosJqlStmtDistributionMap;
		map->setMapName((yystack_[1].value.strval));
		gAosJQLParser.appendStatement(map);   
		(yylhs.value.AosJqlStmtDistributionMapValues) = map;                             
		(yylhs.value.AosJqlStmtDistributionMapValues)->setOp(JQLTypes::eDrop);               
	}
#line 2753 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 246:
#line 2097 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDistributionMap *map = new AosJqlStmtDistributionMap;
		gAosJQLParser.appendStatement(map);   
		(yylhs.value.AosJqlStmtDistributionMapValues) = map;                             
		(yylhs.value.AosJqlStmtDistributionMapValues)->setOp(JQLTypes::eShow);               
	}
#line 2764 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 247:
#line 2107 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDistributionMap *map = new AosJqlStmtDistributionMap;
		map->setMapName((yystack_[1].value.strval));
		gAosJQLParser.appendStatement(map);   
		(yylhs.value.AosJqlStmtDistributionMapValues) = map;                             
		(yylhs.value.AosJqlStmtDistributionMapValues)->setOp(JQLTypes::eDescribe);               
	}
#line 2776 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 248:
#line 2119 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDoc *doc = new AosJqlStmtDoc;
		doc->setOp(JQLTypes::eCreate);               
		doc->setDocConf((yystack_[1].value.strval));
		gAosJQLParser.appendStatement(doc);   
		(yylhs.value.AosJqlStmtDocValue) = doc;                             
	}
#line 2788 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 249:
#line 2129 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDoc *doc = new AosJqlStmtDoc;
		doc->setOp(JQLTypes::eShow);               
		doc->setDocDocid((yystack_[1].value.ll_value));
		gAosJQLParser.appendStatement(doc);   
		(yylhs.value.AosJqlStmtDocValue) = doc;                             
	}
#line 2800 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 250:
#line 2138 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDoc *doc = new AosJqlStmtDoc;
		doc->setOp(JQLTypes::eShow);               
		doc->setDocObjid((yystack_[1].value.strval));
		gAosJQLParser.appendStatement(doc);   
		(yylhs.value.AosJqlStmtDocValue) = doc;                             
	}
#line 2812 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 251:
#line 2148 "Parser.yy" // lalr1.cc:847
    {	
		AosExprObjPtr expr = (yystack_[1].value.AosExprValue);
		gAosJQLParser.setExpr(expr);
		(yylhs.value.AosPJqlStmtExprValue) = new AosJqlStmtExpr(expr);
		gAosJQLParser.appendStatement((yylhs.value.AosPJqlStmtExprValue));
	}
#line 2823 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 252:
#line 2158 "Parser.yy" // lalr1.cc:847
    {
	    //cout << "this is name: " << $1 << endl;
		(yylhs.value.AosExprValue) = new AosExprFieldName((yystack_[0].value.strval));
		if ((yystack_[0].value.strval)) free((yystack_[0].value.strval)); 
	}
#line 2833 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 253:
#line 2165 "Parser.yy" // lalr1.cc:847
    {
	    //cout << "this is name: " << $1 << endl;
		(yylhs.value.AosExprValue) = new AosExprFieldName((yystack_[2].value.strval), (yystack_[0].value.strval));
		if ((yystack_[2].value.strval)) free((yystack_[2].value.strval)); 
		if ((yystack_[0].value.strval)) free((yystack_[0].value.strval)); 
	}
#line 2844 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 254:
#line 2173 "Parser.yy" // lalr1.cc:847
    {
		//cout << "this is U8VAR: " << $1 << endl;
		(yylhs.value.AosExprValue) = new AosExprString((yystack_[0].value.strval));
		if ((yystack_[0].value.strval)) free((yystack_[0].value.strval));
	}
#line 2854 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 255:
#line 2180 "Parser.yy" // lalr1.cc:847
    {
		OmnString name = (yystack_[0].value.strval);
		AosExprUserVar *expr = new AosExprUserVar();
		expr->setName(name);
		(yylhs.value.AosExprValue) = expr;
		if ((yystack_[0].value.strval)) free((yystack_[0].value.strval));
	}
#line 2866 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 256:
#line 2189 "Parser.yy" // lalr1.cc:847
    { 
		(yylhs.value.AosExprValue) = new AosExprMemberOpt("", (yystack_[2].value.strval), (yystack_[0].value.strval));
		if ((yystack_[2].value.strval)) free((yystack_[2].value.strval));
		if ((yystack_[0].value.strval)) free((yystack_[0].value.strval));
	}
#line 2876 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 257:
#line 2196 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosExprValue) = new AosExprMemberOpt((yystack_[4].value.strval), (yystack_[2].value.strval), (yystack_[0].value.strval));
		if ((yystack_[4].value.strval)) free((yystack_[4].value.strval));
		if ((yystack_[2].value.strval)) free((yystack_[2].value.strval));
		if ((yystack_[0].value.strval)) free((yystack_[0].value.strval));
	}
#line 2887 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 258:
#line 2204 "Parser.yy" // lalr1.cc:847
    { 
		(yylhs.value.AosExprValue) = new AosExprMemberOpt("", (yystack_[2].value.strval), (yystack_[0].value.strval));
		if ((yystack_[2].value.strval)) free((yystack_[2].value.strval));
		if ((yystack_[0].value.strval)) free((yystack_[0].value.strval));
	}
#line 2897 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 259:
#line 2211 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosExprValue) = new AosExprMemberOpt((yystack_[4].value.strval), (yystack_[2].value.strval), (yystack_[0].value.strval));
		if ((yystack_[4].value.strval)) free((yystack_[4].value.strval));
		if ((yystack_[2].value.strval)) free((yystack_[2].value.strval));
		if ((yystack_[0].value.strval)) free((yystack_[0].value.strval));
	}
#line 2908 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 260:
#line 2220 "Parser.yy" // lalr1.cc:847
    { 
	    //cout << "this is string" << endl;
		u32 len = strlen((yystack_[0].value.strval));
		if (len > 0)
		{
			char *newch = new char[len+1]; 
			strncpy(newch, (yystack_[0].value.strval) , len);
			newch[len] = '\0';
			(yylhs.value.AosExprValue) = new AosExprString(newch);
			delete [] newch;
		}
		else
		{
			(yylhs.value.AosExprValue) = new AosExprString("");
			//cout << "Parser string error!!: " << $1 << endl;
		}
		free((yystack_[0].value.strval)); 
	}
#line 2931 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 261:
#line 2240 "Parser.yy" // lalr1.cc:847
    { 
		int64_t e1 = (yystack_[0].value.ll_value);
		//cout << "Found number: " << $1 << endl;
		(yylhs.value.AosExprValue) = new AosExprNumber(e1);
	}
#line 2941 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 262:
#line 2247 "Parser.yy" // lalr1.cc:847
    { 
		(yylhs.value.AosExprValue) = new AosExprDouble((yystack_[0].value.double_val));
	}
#line 2949 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 263:
#line 2252 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosExprValue) = new AosExprNull();
	}
#line 2957 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 264:
#line 2267 "Parser.yy" // lalr1.cc:847
    { 
		AosExprObj* e1 = (yystack_[2].value.AosExprValue);
		AosExprObj* e2 = (yystack_[0].value.AosExprValue);
		// AosValuePtr value = OmnNew AosValueInt64(0);

		//cout << "Found expression + " << endl;
        (yylhs.value.AosExprValue) = new AosExprArith(e1, AosExprArith::eAdd, e2);
		// if (!$$->getValue(0, 0, (AosValuePtr &)value)) 
		// {
		// 	//cout << "failed to get add result: " << $1 << " + " << $3 << endl;
		// } else 
		// {
		// 	//cout << value->toString() << endl;
		// }
	}
#line 2977 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 265:
#line 2284 "Parser.yy" // lalr1.cc:847
    { 
		AosExprObj* e1 = (yystack_[2].value.AosExprValue);
		AosExprObj* e2 = (yystack_[0].value.AosExprValue);
		// AosValuePtr value = OmnNew AosValueInt64(0);

		//cout << "Found expression -"  << endl;
        (yylhs.value.AosExprValue) = new AosExprArith(e1, AosExprArith::eMinus, e2);
		// if (!$$->getValue(0, 0, (AosValuePtr &)value)) 
		// {
		// 	//cout << "failed to get minus result: " << $1 << " + " << $3 << endl;
		// } else 
		// {
		// 	//cout << value->toString() << endl;
		// }
	}
#line 2997 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 266:
#line 2301 "Parser.yy" // lalr1.cc:847
    { 
		AosExprObj* e1 = (yystack_[2].value.AosExprValue);
		AosExprObj* e2 = (yystack_[0].value.AosExprValue);
		// AosValuePtr value = OmnNew AosValueInt64(0);

		//cout << "Found expression *" << endl;
        (yylhs.value.AosExprValue) = new AosExprArith(e1, AosExprArith::eMultiply, e2);
		// if (!$$->getValue(0, 0, (AosValuePtr &)value)) 
		// {
		// 	//cout << "failed to get multiply result: " << $1 << " + " << $3 << endl;
		// } else 
		// {
		// 	//cout << value->toString() << endl;
		// }
	}
#line 3017 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 267:
#line 2318 "Parser.yy" // lalr1.cc:847
    { 
		AosExprObj* e1 = (yystack_[2].value.AosExprValue);
		AosExprObj* e2 = (yystack_[0].value.AosExprValue);
		// AosValuePtr value = OmnNew AosValueInt64(0);

		//cout << "Found expression /" << endl;
        (yylhs.value.AosExprValue) = new AosExprArith(e1, AosExprArith::eDivide, e2);
		// if (!$$->getValue(0, 0, (AosValuePtr &)value)) 
		// {
		// 	//cout << "failed to get divide result: " << $1 << " + " << $3 << endl;
		// } else 
		// {
		// 	//cout << value->toString() << endl;
		// }
	}
#line 3037 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 268:
#line 2335 "Parser.yy" // lalr1.cc:847
    { 
		AosExprObj* e1 = (yystack_[2].value.AosExprValue);
		AosExprObj* e2 = (yystack_[0].value.AosExprValue);
		// AosValuePtr value = OmnNew AosValueU64(0);

		//cout << "Found expression %" << endl;
        (yylhs.value.AosExprValue) = new AosExprArith(e1, AosExprArith::eMod, e2);
		// if (!$$->getValue(0, 0, (AosValuePtr &)value)) 
		// {
		// 	//cout << "failed to get mod result: " << $1 << " % " << $3 << endl;
		// } else 
		// {
		// 	//cout << value->toString() << endl;
		// }
	}
#line 3057 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 269:
#line 2352 "Parser.yy" // lalr1.cc:847
    { 
		//$$ = new AosExprUnaryMinus($2);
	}
#line 3065 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 270:
#line 2357 "Parser.yy" // lalr1.cc:847
    { 
		AosExprObj *e1 = (yystack_[2].value.AosExprValue);
		AosExprObj *e2 = (yystack_[0].value.AosExprValue);
		//$$ = new AosExprLogic(e1, (AosExprLogic::Operator)$2, e2);
       // $$ = new AosExprArith(e1, AosExprArith::eConcat, e2);

		// Young, 2014/11/05
		AosExpr* logicExpr = new AosExprLogic(e1, (AosExprLogic::Operator)(yystack_[1].value.subtok), e2);
		if (e1->getType() == AosExprType::eComparison || e2->getType() == AosExprType::eComparison)
		{
			logicExpr->setExpr(e1, 1);
			logicExpr->setExpr(e2, 2);
		}
		(yylhs.value.AosExprValue) = logicExpr;
	}
#line 3085 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 271:
#line 2374 "Parser.yy" // lalr1.cc:847
    { 
		AosExprObj *e1 = (yystack_[2].value.AosExprValue);
		AosExprObj *e2 = (yystack_[0].value.AosExprValue);
		(yylhs.value.AosExprValue) = new AosExprLike(e1, e2, false);
	}
#line 3095 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 272:
#line 2381 "Parser.yy" // lalr1.cc:847
    { 
		AosExprObj *e1 = (yystack_[3].value.AosExprValue);
		AosExprObj *e2 = (yystack_[0].value.AosExprValue);
		(yylhs.value.AosExprValue) = new AosExprLike(e1, e2, true);
	}
#line 3105 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 273:
#line 2388 "Parser.yy" // lalr1.cc:847
    { 
        //$$ = new AosExprXor($1, $3);
	}
#line 3113 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 274:
#line 2393 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosExprValue) = new AosExprIn((yystack_[4].value.AosExprValue), (yystack_[1].value.AosExprListValue));
	}
#line 3121 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 275:
#line 2398 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosExprValue) = new AosExprNotIn((yystack_[5].value.AosExprValue), (yystack_[1].value.AosExprListValue));
	}
#line 3129 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 276:
#line 2403 "Parser.yy" // lalr1.cc:847
    { 
		AosExprObj *e1 = (yystack_[2].value.AosExprValue);
		AosExprObj *e2 = (yystack_[0].value.AosExprValue);
		AosExprType::E expr_type = e1->getType();
		if (expr_type == AosExprType::eComparison)
		{
			AosExprBinary* expr = dynamic_cast<AosExprBinary*>(e1);
			AosExprObjPtr e3 = expr->getRHS();

			AosExpr *rhs= new AosExprComparison(e3, (AosExprComparison::Operator)(yystack_[1].value.subtok), e2);     
			(yylhs.value.AosExprValue) = new AosExprLogic(e1, AosExprLogic::eAnd, rhs);
		}
		else
		{
			AosExpr *expr= new AosExprComparison(e1, (AosExprComparison::Operator)(yystack_[1].value.subtok), e2);
			expr->setExpr(e1, 1);
			expr->setExpr(e2, 2);
			(yylhs.value.AosExprValue) = expr;
		}
	}
#line 3154 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 277:
#line 2425 "Parser.yy" // lalr1.cc:847
    {
		AosExpr *rhs= new AosExprComparison((yystack_[4].value.AosExprValue), AosExprComparison::eLargerEqual, (yystack_[2].value.AosExprValue));
		AosExpr *lhs= new AosExprComparison((yystack_[4].value.AosExprValue), AosExprComparison::eLessEqual, (yystack_[0].value.AosExprValue));     
		(yylhs.value.AosExprValue) = new AosExprLogic(rhs, AosExprLogic::eAnd, lhs);
	}
#line 3164 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 278:
#line 2432 "Parser.yy" // lalr1.cc:847
    { 
		AosExprObj* e1 = (yystack_[2].value.AosExprValue);
		AosExprObj* e2 = (yystack_[0].value.AosExprValue);
		// AosValuePtr value = OmnNew AosValueU32(0);

		//cout << "Found expression |" << endl;
        (yylhs.value.AosExprValue) = new AosExprBitArith(e1, AosExprBitArith::eBitOr, e2);
		// if (!$$->getValue(0, 0, (AosValuePtr &)value)) 
		// {
		// 	//cout << "failed to get bitor result: " << $1 << " | " << $3 << endl;
		// } else 
		// {
		// 	//cout << value->toString() << endl;
		// }
	}
#line 3184 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 279:
#line 2449 "Parser.yy" // lalr1.cc:847
    { 
		AosExprObj* e1 = (yystack_[2].value.AosExprValue);
		AosExprObj* e2 = (yystack_[0].value.AosExprValue);
		// AosValuePtr value = OmnNew AosValueU32(0);

		//cout << "Found expression &" << endl;
        (yylhs.value.AosExprValue) = new AosExprBitArith(e1, AosExprBitArith::eBitAnd, e2);
		// if (!$$->getValue(0, 0, (AosValuePtr &)value)) 
		// {
		// 	//cout << "failed to get bitand result: " << $1 << " & " << $3 << endl;
		// } else 
		// {
		// 	//cout << value->toString() << endl;
		// }
	}
#line 3204 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 280:
#line 2466 "Parser.yy" // lalr1.cc:847
    { 
		AosExprObj* e1 = (yystack_[2].value.AosExprValue);
		AosExprObj* e2 = (yystack_[0].value.AosExprValue);
		// AosValuePtr value = OmnNew AosValueU32(0);

		//cout << "Found expression ^" << endl;
        (yylhs.value.AosExprValue) = new AosExprBitArith(e1, AosExprBitArith::eBitXor, e2);
		// if (!$$->getValue(0, 0, (AosValuePtr &)value)) 
		// {
		// 	//cout << "failed to get bitand result: " << $1 << " ^ " << $3 << endl;
		// } else 
		// {
		// 	//cout << value->toString() << endl;
		// }
	}
#line 3224 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 281:
#line 2483 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosExprValue) = new AosExprContain((yystack_[2].value.AosExprValue), (yystack_[0].value.strval));
	}
#line 3232 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 282:
#line 2488 "Parser.yy" // lalr1.cc:847
    { 
        // $$ = new AosSQLExpr;
		// $$->setOperType( $2==1 ? eOper_LeftShift : eOper_RightShift);
		// $$->setExpr($1, 1);
		// $$->setExpr($3, 2);
	}
#line 3243 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 283:
#line 2496 "Parser.yy" // lalr1.cc:847
    { 
        // $$ = new AosSQLExpr;
		// $$->setOperType(eOper_Not);
		// $$->setExpr($2, 1);
	}
#line 3253 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 284:
#line 2503 "Parser.yy" // lalr1.cc:847
    { 
        // $$ = new AosSQLExpr;
		// $$->setOperType(eOper_Not);
		// $$->setExpr($2, 1);
	}
#line 3263 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 285:
#line 2510 "Parser.yy" // lalr1.cc:847
    { 
        // $$ = new AosSQLExpr;
		// $$->setOperType(eOper_Assign);
		// $$->setValue($1);
		// $$->setExpr($3, 1);
	}
#line 3274 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 286:
#line 2518 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosExprValue) = new AosExprNameValue((yystack_[2].value.strval), (yystack_[0].value.AosExprValue));
	}
#line 3282 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 287:
#line 2523 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosExprValue) = new AosExprGenFunc("left", (yystack_[1].value.AosExprListValue));
	}
#line 3290 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 288:
#line 2528 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosExprValue) = new AosExprGenFunc("right", (yystack_[1].value.AosExprListValue));
	}
#line 3298 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 289:
#line 2533 "Parser.yy" // lalr1.cc:847
    {
		AosExprList* expr_list = new AosExprList;
		(yylhs.value.AosExprValue) = new AosExprGenFunc((yystack_[2].value.strval), expr_list);
	}
#line 3307 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 290:
#line 2539 "Parser.yy" // lalr1.cc:847
    {
		AosExprList* expr_list = new AosExprList;
		expr_list->push_back(new AosExprFieldName("*"));
		(yylhs.value.AosExprValue) = new AosExprGenFunc((yystack_[3].value.strval), expr_list);
	}
#line 3317 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 291:
#line 2546 "Parser.yy" // lalr1.cc:847
    {
		OmnString fname = (yystack_[3].value.strval);
		if (fname.toLower() == "avg" && (yystack_[1].value.AosExprListValue)->size() == 1)
		{
			AosExprObjPtr lhs = OmnNew AosExprGenFunc("sum", (*(yystack_[1].value.AosExprListValue))[0]);
			AosExprObjPtr lhs1 = OmnNew AosExprGenFunc("to_double", lhs);
			AosExprObjPtr rhs = OmnNew AosExprGenFunc("count", (*(yystack_[1].value.AosExprListValue))[0]);
			(yylhs.value.AosExprValue) = new AosExprArith(lhs1, AosExprArith::eDivide, rhs);
		}
		else
		{
			(yylhs.value.AosExprValue) = new AosExprGenFunc((yystack_[3].value.strval), (yystack_[1].value.AosExprListValue));
		}
	}
#line 3336 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 292:
#line 2562 "Parser.yy" // lalr1.cc:847
    {
		OmnString fname = (yystack_[4].value.strval);
		AosValueRslt v;

		if (fname.toLower() == "count")
		{
			if ((yystack_[1].value.AosExprListValue)->size() == 1)
			{
				AosExprObjPtr expr = (*(yystack_[1].value.AosExprListValue))[0];
				bool rslt = expr->getValue(0, 0, v);
				aos_assert_r(rslt, false);
				OmnString rname = v.getStr();
				if (rname == "*") 
				{
					(*(yystack_[1].value.AosExprListValue))[0] = new AosExprFieldName("_rec_count");
				}
			}
			
			fname = "dist_count";
		}
		(yylhs.value.AosExprValue) = new AosExprGenFunc(fname, (yystack_[1].value.AosExprListValue));
	}
#line 3363 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 293:
#line 2586 "Parser.yy" // lalr1.cc:847
    {
		OmnString fname = (yystack_[4].value.strval);
		OmnString newFname = "accu_";
		AosValueRslt v;

		fname = fname.toLower();
		newFname << fname;
		aos_assert_r(fname == "count" ||
					 fname == "sum" ||
					 fname == "max" ||
					 fname == "min",  false);

		aos_assert_r((yystack_[1].value.AosExprListValue)->size() == 1, false);
		(yylhs.value.AosExprValue) = new AosExprGenFunc(newFname, (yystack_[1].value.AosExprListValue));
	}
#line 3383 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 294:
#line 2603 "Parser.yy" // lalr1.cc:847
    {
		OmnString fname = (yystack_[4].value.strval);
		OmnString newFname = "accu_";
		AosValueRslt v;

		fname = fname.toLower();
		newFname << fname;
		aos_assert_r(fname == "count",  false);

		AosExprList* exprList = new AosExprList;
		AosExprFieldName* expr = new AosExprFieldName("_rec_count");
		exprList->push_back(expr);

		(yylhs.value.AosExprValue) = new AosExprGenFunc(newFname, exprList);
	}
#line 3403 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 295:
#line 2620 "Parser.yy" // lalr1.cc:847
    {
		//max is an exception since there is 
		//a token "MAX", therefore we need to
		//add one explicit line for it
		(yylhs.value.AosExprValue) = new AosExprGenFunc("max", (yystack_[1].value.AosExprListValue));
	}
#line 3414 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 296:
#line 2628 "Parser.yy" // lalr1.cc:847
    {
		//cout << "Found expression" << endl;
		(yylhs.value.AosExprValue) = new AosExprBrackets((yystack_[1].value.AosExprValue));
	}
#line 3423 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 297:
#line 2634 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosExprValue) = new AosExprBrackets(*((yystack_[1].value.AosExprListValue)));
	}
#line 3431 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 298:
#line 2639 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosExprValue) = new AosExprBrackets(*((yystack_[1].value.AosExprListValue)));
	}
#line 3439 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 299:
#line 2644 "Parser.yy" // lalr1.cc:847
    {
		//cout << "Found case1............" << endl;
		(yylhs.value.AosExprValue) = new AosExprCase((yystack_[2].value.AosExprSearchCaseValue), 0, (yystack_[1].value.AosExprValue));
	}
#line 3448 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 300:
#line 2650 "Parser.yy" // lalr1.cc:847
    {
		//cout << "Found case2............" << endl;
		(yylhs.value.AosExprValue) = new AosExprCase(0, (yystack_[2].value.AosExprSimpleCaseValue), (yystack_[1].value.AosExprValue));
	}
#line 3457 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 301:
#line 2656 "Parser.yy" // lalr1.cc:847
    {
		//cout << "Found is not null ..." << endl;
		(yylhs.value.AosExprValue) = new AosExprIsNotNull((yystack_[3].value.AosExprValue));
	}
#line 3466 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 302:
#line 2662 "Parser.yy" // lalr1.cc:847
    {
		//cout << "Found is not null ..." << endl;
		(yylhs.value.AosExprValue) = new AosExprIsNull((yystack_[2].value.AosExprValue));
	}
#line 3475 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 303:
#line 2670 "Parser.yy" // lalr1.cc:847
    {
		AosExprSimpleCase::AosJqlWhenCaseExpr *aa = new AosExprSimpleCase::AosJqlWhenCaseExpr((yystack_[3].value.AosExprValue), (yystack_[1].value.AosExprValue));
		(yystack_[0].value.AosExprSimpleCaseValue)->appendWhenCase(aa);
	 	//$6->appendPair($3, $5);
		(yystack_[0].value.AosExprSimpleCaseValue)->setCaseExpr((yystack_[5].value.AosExprValue));
		(yylhs.value.AosExprSimpleCaseValue) = (yystack_[0].value.AosExprSimpleCaseValue);
	 }
#line 3487 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 304:
#line 2679 "Parser.yy" // lalr1.cc:847
    {
	 	(yylhs.value.AosExprSimpleCaseValue) = new AosExprSimpleCase();
	 }
#line 3495 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 305:
#line 2684 "Parser.yy" // lalr1.cc:847
    {
	 	//$1->appendPair($3, $5);
		AosExprSimpleCase::AosJqlWhenCaseExpr *aa = new AosExprSimpleCase::AosJqlWhenCaseExpr((yystack_[2].value.AosExprValue), (yystack_[0].value.AosExprValue));
		(yystack_[4].value.AosExprSimpleCaseValue)->appendWhenCase(aa);
	 }
#line 3505 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 306:
#line 2692 "Parser.yy" // lalr1.cc:847
    {
		 (yylhs.value.AosExprSearchCaseValue) = new AosExprSearchCase((yystack_[0].value.AosExprSimpleCaseValue));
	 }
#line 3513 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 307:
#line 2698 "Parser.yy" // lalr1.cc:847
    {
		 (yylhs.value.AosExprValue) = 0;
	 }
#line 3521 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 308:
#line 2703 "Parser.yy" // lalr1.cc:847
    {
		 (yylhs.value.AosExprValue) = (yystack_[0].value.AosExprValue);
	 }
#line 3529 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 309:
#line 2709 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosExprListValue) = new AosExprList;
		(yylhs.value.AosExprListValue)->push_back((yystack_[0].value.AosExprValue));
	}
#line 3538 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 310:
#line 2715 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosExprListValue) = (yystack_[2].value.AosExprListValue);
		(yylhs.value.AosExprListValue)->push_back((yystack_[0].value.AosExprValue));
	}
#line 3547 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 311:
#line 2722 "Parser.yy" // lalr1.cc:847
    {
		typedef vector<AosExprNameValuePtr> AosExprNameValues;
		(yylhs.value.AosExprNameValuesValue) = new AosExprNameValues;
	}
#line 3556 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 312:
#line 2728 "Parser.yy" // lalr1.cc:847
    {
		typedef vector<AosExprNameValuePtr> AosExprNameValues;
		AosExprNameValue *expr = new AosExprNameValue((yystack_[2].value.strval), (yystack_[0].value.AosExprValue));
		(yylhs.value.AosExprNameValuesValue) = new AosExprNameValues;
		(yylhs.value.AosExprNameValuesValue)->push_back(expr);
	}
#line 3567 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 313:
#line 2736 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosExprNameValuesValue) = (yystack_[4].value.AosExprNameValuesValue);
		AosExprNameValue *expr = new AosExprNameValue((yystack_[2].value.strval), (yystack_[0].value.AosExprValue));
		(yylhs.value.AosExprNameValuesValue)->push_back(expr);
	}
#line 3577 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 314:
#line 2745 "Parser.yy" // lalr1.cc:847
    { 
		(yylhs.value.AosExprValue) = new AosExprFieldName((yystack_[0].value.strval));  
		if ((yystack_[0].value.strval)) free((yystack_[0].value.strval));            
  	}
#line 3586 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 315:
#line 2751 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosExprValue) = new AosExprString((yystack_[0].value.strval));  
		if ((yystack_[0].value.strval)) free((yystack_[0].value.strval));            
	}
#line 3595 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 316:
#line 2757 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosExprValue) = new AosExprString((yystack_[0].value.strval));  
		if ((yystack_[0].value.strval)) free((yystack_[0].value.strval));            
	}
#line 3604 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 317:
#line 2763 "Parser.yy" // lalr1.cc:847
    { 
		(yylhs.value.AosExprValue) = new AosExprFieldName((yystack_[0].value.strval));  
		if ((yystack_[0].value.strval)) free((yystack_[0].value.strval));            
  	}
#line 3613 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 318:
#line 2769 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosExprValue) = NULL;
  	}
#line 3621 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 319:
#line 2776 "Parser.yy" // lalr1.cc:847
    {              
        (yylhs.value.AosJqlWhereValue) = 0;    
    }
#line 3629 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 320:
#line 2781 "Parser.yy" // lalr1.cc:847
    {              
		(yylhs.value.AosJqlWhereValue) = new AosJqlWhere();
		(yylhs.value.AosJqlWhereValue)->setWhereExpr((yystack_[0].value.AosExprValue));
    }
#line 3638 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 321:
#line 2789 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtGenericobj * statement = new AosJqlStmtGenericobj;
		statement->setType((yystack_[5].value.AosExprValue));
		statement->setName((yystack_[4].value.AosExprValue));
//		statement->setGenericobjValues($5);
		gAosJQLParser.appendStatement(statement);
		(yylhs.value.AosJqlStmtGenericobjValue) = statement;
		(yylhs.value.AosJqlStmtGenericobjValue)->setOp(JQLTypes::eCreate);
	}
#line 3652 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 322:
#line 2801 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlGenericobjValuesValues) = NULL;
	}
#line 3660 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 323:
#line 2806 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlGenericobjValuesValues) = new vector<AosGenericValueObjPtr>();
		(yylhs.value.AosJqlGenericobjValuesValues)->push_back((yystack_[0].value.AosJqlGenericobjValueValues));
	}
#line 3669 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 324:
#line 2812 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlGenericobjValuesValues) = (yystack_[2].value.AosJqlGenericobjValuesValues);
		(yylhs.value.AosJqlGenericobjValuesValues)->push_back((yystack_[0].value.AosJqlGenericobjValueValues));
	}
#line 3678 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 325:
#line 2819 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlGenericobjValueValues) = NULL;
	}
#line 3686 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 326:
#line 2824 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlGenericobjValueValues) = new AosGenericValueObj();
		(yylhs.value.AosJqlGenericobjValueValues)->mValue = (yystack_[0].value.AosExprValue);
	}
#line 3695 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 327:
#line 2830 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlGenericobjValueValues) = new AosGenericValueObj();
		(yylhs.value.AosJqlGenericobjValueValues)->mName = (yystack_[4].value.AosExprValue);
		(yylhs.value.AosJqlGenericobjValueValues)->mArrayValues = *(yystack_[1].value.AosExprListValue);
	}
#line 3705 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 328:
#line 2843 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtIF *stmt = new AosJqlStmtIF;
		stmt->setNot((yystack_[6].value.bool_val));
		stmt->setCond((yystack_[5].value.AosExprValue));
		stmt->setThen((yystack_[4].value.AosJqlStatementListValue));
		stmt->setElse((yystack_[3].value.AosJqlStatementListValue));
		gAosJQLParser.appendStatement(stmt);
		(yylhs.value.AosJqlStmtIFValue) = stmt;
		(yylhs.value.AosJqlStmtIFValue)->setOp(JQLTypes::eRun);
	}
#line 3720 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 329:
#line 2856 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.bool_val) = false;
	}
#line 3728 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 330:
#line 2861 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.bool_val) = true;
	}
#line 3736 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 331:
#line 2867 "Parser.yy" // lalr1.cc:847
    {
		typedef vector<AosJqlStatementPtr> AosJqlStatementList;
		(yylhs.value.AosJqlStatementListValue) = new AosJqlStatementList; 
		(yylhs.value.AosJqlStatementListValue)->push_back((yystack_[0].value.AosJqlStatementValue));
		gAosJQLParser.finishParse();
	}
#line 3747 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 332:
#line 2875 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlStatementListValue) = (yystack_[1].value.AosJqlStatementListValue);
		(yylhs.value.AosJqlStatementListValue)->push_back((yystack_[0].value.AosJqlStatementValue));
		gAosJQLParser.finishParse();
	}
#line 3757 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 333:
#line 2883 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlStatementListValue) = NULL;
	}
#line 3765 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 334:
#line 2888 "Parser.yy" // lalr1.cc:847
    {
		typedef vector<AosJqlStatementPtr> AosJqlStatementList;
		(yylhs.value.AosJqlStatementListValue) = new AosJqlStatementList; 
		(yylhs.value.AosJqlStatementListValue)->push_back((yystack_[0].value.AosJqlStatementValue));
		gAosJQLParser.finishParse();
	}
#line 3776 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 335:
#line 2896 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlStatementListValue) = (yystack_[1].value.AosJqlStatementListValue);
		(yylhs.value.AosJqlStatementListValue)->push_back((yystack_[0].value.AosJqlStatementValue));
		gAosJQLParser.finishParse();
	}
#line 3786 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 336:
#line 2907 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtIndex* statement = new AosJqlStmtIndex;
		statement->setIndexName((yystack_[8].value.strval));
		statement->setTableName((yystack_[5].value.strval));
		//statement->setFieldIndexColumns($8);
		statement->setWhereConds((yystack_[1].value.AosJqlWhereValue));
		// statement->setFieldIndexColumns($9);
		gAosJQLParser.appendStatement(statement);

		AosJqlStmtIndex::AosFieldIdxCols *field_list = new AosJqlStmtIndex::AosFieldIdxCols; 
		field_list->mFieldList = (yystack_[3].value.AosExprListValue);
		field_list->mIndexName= "";
		field_list->mType = "";

		statement->setFieldList(field_list);

		(yylhs.value.AosJqlStmtIndexValue) = statement;
		(yylhs.value.AosJqlStmtIndexValue)->setOp(JQLTypes::eCreate);
	}
#line 3810 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 337:
#line 2930 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtIndex* statement = new AosJqlStmtIndex;
		statement->setIndexName((yystack_[10].value.strval));
		statement->setTableName((yystack_[7].value.strval));
		statement->setIndexRecords((yystack_[2].value.AosExprListVecValue));
		gAosJQLParser.appendStatement(statement);

		(yylhs.value.AosJqlStmtIndexValue) = statement;
		(yylhs.value.AosJqlStmtIndexValue)->setOp(JQLTypes::eCreate);
	}
#line 3825 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 338:
#line 2942 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtIndex* stmt = new AosJqlStmtIndex;
		stmt->setIndexName((yystack_[4].value.strval));
		stmt->setTableName((yystack_[2].value.strval));
		stmt->setWhereConds((yystack_[1].value.AosJqlWhereValue));
		gAosJQLParser.appendStatement(stmt);
		(yylhs.value.AosJqlStmtIndexValue) = stmt;
		(yylhs.value.AosJqlStmtIndexValue)->setOp(JQLTypes::eCreate);
	}
#line 3839 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 339:
#line 2954 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtIndex* statement = new AosJqlStmtIndex;
		gAosJQLParser.appendStatement(statement);
		(yylhs.value.AosJqlStmtIndexValue) = statement;
		(yylhs.value.AosJqlStmtIndexValue)->setIndexName((yystack_[1].value.strval));
		(yylhs.value.AosJqlStmtIndexValue)->setOp(JQLTypes::eDescribe);

		cout << "Describe index: " << (yystack_[1].value.strval) << endl;
	}
#line 3853 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 340:
#line 2967 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtIndex* statement = new AosJqlStmtIndex;
		gAosJQLParser.appendStatement(statement);
		(yylhs.value.AosJqlStmtIndexValue) = statement;
		(yylhs.value.AosJqlStmtIndexValue)->setIndexName((yystack_[4].value.AosExprValue)->getValue(0));
		(yylhs.value.AosJqlStmtIndexValue)->setKeysExpr((yystack_[3].value.AosExprListValue));
		(yylhs.value.AosJqlStmtIndexValue)->setWhereConds((yystack_[2].value.AosJqlWhereValue));
		(yylhs.value.AosJqlStmtIndexValue)->setLimit((yystack_[1].value.AosJqlLimitValue));
		(yylhs.value.AosJqlStmtIndexValue)->setOp(JQLTypes::eList);
	}
#line 3868 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 341:
#line 2980 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosExprListValue) = NULL;
	}
#line 3876 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 342:
#line 2985 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosExprListValue) = (yystack_[1].value.AosExprListValue);
	}
#line 3884 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 343:
#line 2992 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtIndex *show_iil = new AosJqlStmtIndex();	
		gAosJQLParser.appendStatement(show_iil);
		(yylhs.value.AosJqlStmtIndexValue) = show_iil;
		(yylhs.value.AosJqlStmtIndexValue)->setOp(JQLTypes::eShow); 
    }
#line 3895 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 344:
#line 3002 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtIndex *drop_iil = new AosJqlStmtIndex();
		drop_iil->setIndexName((yystack_[1].value.strval));
		gAosJQLParser.appendStatement(drop_iil);
		(yylhs.value.AosJqlStmtIndexValue) = drop_iil;
		(yylhs.value.AosJqlStmtIndexValue)->setOp(JQLTypes::eDrop); 
		free((yystack_[1].value.strval));
	}
#line 3908 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 345:
#line 3014 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosExprListVecValue) = new vector<AosExprList *>;
		(yylhs.value.AosExprListVecValue)->push_back((yystack_[1].value.AosExprListValue));
	}
#line 3917 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 346:
#line 3020 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosExprListVecValue) = (yystack_[4].value.AosExprListVecValue);
		(yylhs.value.AosExprListVecValue)->push_back((yystack_[1].value.AosExprListValue));
	}
#line 3926 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 347:
#line 3028 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtInsert* insert = new AosJqlStmtInsert;
		insert->table_name = (yystack_[4].value.strval);
		insert->opt_col_names = (yystack_[3].value.AosExprListValue);
		insert->insert_vals_list = (yystack_[1].value.AosExprListValue);
		(yylhs.value.AosJqlStmtInsertItemValue) = new  AosJqlStmtInsertItem(insert);
		gAosJQLParser.appendStatement((yylhs.value.AosJqlStmtInsertItemValue));
	}
#line 3939 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 348:
#line 3040 "Parser.yy" // lalr1.cc:847
    {  
	}
#line 3946 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 349:
#line 3044 "Parser.yy" // lalr1.cc:847
    {
	}
#line 3953 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 350:
#line 3048 "Parser.yy" // lalr1.cc:847
    { 
	}
#line 3960 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 351:
#line 3052 "Parser.yy" // lalr1.cc:847
    { 
	}
#line 3967 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 352:
#line 3056 "Parser.yy" // lalr1.cc:847
    { 
	}
#line 3974 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 353:
#line 3062 "Parser.yy" // lalr1.cc:847
    {
	}
#line 3981 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 354:
#line 3066 "Parser.yy" // lalr1.cc:847
    {
	}
#line 3988 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 355:
#line 3072 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosExprListValue) = new AosExprList;
	}
#line 3996 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 356:
#line 3077 "Parser.yy" // lalr1.cc:847
    { 
		(yylhs.value.AosExprListValue) = (yystack_[1].value.AosExprListValue); 
	}
#line 4004 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 357:
#line 3084 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosExprListValue) = new AosExprList;
		(yylhs.value.AosExprListValue)->push_back((yystack_[0].value.AosExprValue));
	}
#line 4013 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 358:
#line 3090 "Parser.yy" // lalr1.cc:847
    { 
		(yylhs.value.AosExprListValue)->push_back((yystack_[0].value.AosExprValue));
	}
#line 4021 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 359:
#line 3097 "Parser.yy" // lalr1.cc:847
    { 
		(yylhs.value.AosExprListValue) = (yystack_[1].value.AosExprListValue);
	}
#line 4029 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 360:
#line 3104 "Parser.yy" // lalr1.cc:847
    { 
		(yylhs.value.AosExprListValue) = new AosExprList;
		(yylhs.value.AosExprListValue)->push_back((yystack_[0].value.AosExprValue)); 
	}
#line 4038 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 361:
#line 3110 "Parser.yy" // lalr1.cc:847
    { 
	}
#line 4045 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 362:
#line 3114 "Parser.yy" // lalr1.cc:847
    { 
		(yylhs.value.AosExprListValue)->push_back((yystack_[0].value.AosExprValue)); 
	}
#line 4053 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 363:
#line 3119 "Parser.yy" // lalr1.cc:847
    { 
	}
#line 4060 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 364:
#line 3129 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtJimoLogic *logic = new AosJqlStmtJimoLogic;
		logic->setName((yystack_[14].value.strval));
		logic->setTableName((yystack_[12].value.strval));
		logic->setEndPoint((yystack_[10].value.strval));
		logic->setTime((yystack_[8].value.strval));
		logic->setCheckPoint((yystack_[6].value.strval));
		logic->setMatrixName((yystack_[3].value.strval));
		logic->setResultsName((yystack_[1].value.strval));
		gAosJQLParser.appendStatement(logic);
		(yylhs.value.AosJqlStmtJimoLogicValue) = logic;
		(yylhs.value.AosJqlStmtJimoLogicValue)->setOp(JQLTypes::eCreate);
	}
#line 4078 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 365:
#line 3150 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtJimoLogic *logic = new AosJqlStmtJimoLogic;
		logic->setName((yystack_[17].value.strval));
		logic->setTableName((yystack_[15].value.strval));
		logic->setEndPoint((yystack_[13].value.strval));
		logic->setTime((yystack_[11].value.strval));
		logic->setCheckPoint((yystack_[9].value.strval));
		logic->setMatrixName((yystack_[6].value.strval));
		logic->setSecondName((yystack_[3].value.strval));
		logic->setResultsName((yystack_[1].value.strval));
		gAosJQLParser.appendStatement(logic);
		(yylhs.value.AosJqlStmtJimoLogicValue) = logic;
		(yylhs.value.AosJqlStmtJimoLogicValue)->setOp(JQLTypes::eCreate);
	}
#line 4097 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 366:
#line 3168 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtJimoLogic *logic = new AosJqlStmtJimoLogic;
		logic->setName((yystack_[1].value.strval));
		gAosJQLParser.appendStatement(logic);
		(yylhs.value.AosJqlStmtJimoLogicValue) = logic;
		(yylhs.value.AosJqlStmtJimoLogicValue)->setOp(JQLTypes::eDrop);
	}
#line 4109 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 367:
#line 3179 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtJimoLogic *logic = new AosJqlStmtJimoLogic;
		gAosJQLParser.appendStatement(logic);
		(yylhs.value.AosJqlStmtJimoLogicValue) = logic;
		(yylhs.value.AosJqlStmtJimoLogicValue)->setOp(JQLTypes::eShow);
	}
#line 4120 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 368:
#line 3189 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtJimoLogic *logic = new AosJqlStmtJimoLogic;
		logic->setName((yystack_[1].value.strval));
		gAosJQLParser.appendStatement(logic);
		(yylhs.value.AosJqlStmtJimoLogicValue) = logic;
		(yylhs.value.AosJqlStmtJimoLogicValue)->setOp(JQLTypes::eDescribe);
	}
#line 4132 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 369:
#line 3199 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtJob *stmt = new AosJqlStmtJob;
		stmt->setJobName((yystack_[4].value.strval));
		stmt->setConfParms((yystack_[2].value.AosExprNameValuesValue));
		gAosJQLParser.appendStatement(stmt);
		(yylhs.value.AosJqlStmtJobValue) = stmt;
		(yylhs.value.AosJqlStmtJobValue)->setOp(JQLTypes::eCreate);
	}
#line 4145 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 370:
#line 3210 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtJob *job = new AosJqlStmtJob;
		job->setJobName((yystack_[6].value.strval));
		job->setTaskNames((yystack_[2].value.AosExprListValue));
		gAosJQLParser.appendStatement(job);   
		(yylhs.value.AosJqlStmtJobValue) = job;                             
		(yylhs.value.AosJqlStmtJobValue)->setOp(JQLTypes::eCreate);               
	}
#line 4158 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 371:
#line 3233 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtJob *job = new AosJqlStmtJob;
		job->setJobName((yystack_[3].value.strval));
		job->setJobFileName((yystack_[1].value.strval));
		gAosJQLParser.appendStatement(job);   
		(yylhs.value.AosJqlStmtJobValue) = job;                             
		(yylhs.value.AosJqlStmtJobValue)->setOp(JQLTypes::eRun);               
	}
#line 4171 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 372:
#line 3245 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtJob *job = new AosJqlStmtJob;
		job->setJobName((yystack_[1].value.strval));
		gAosJQLParser.appendStatement(job);   
		(yylhs.value.AosJqlStmtJobValue) = job;                             
		(yylhs.value.AosJqlStmtJobValue)->setOp(JQLTypes::eStop);               
	}
#line 4183 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 373:
#line 3256 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtJob *job = new AosJqlStmtJob;
		job->setJobName((yystack_[1].value.strval));
		gAosJQLParser.appendStatement(job);   
		(yylhs.value.AosJqlStmtJobValue) = job;                             
		(yylhs.value.AosJqlStmtJobValue)->setOp(JQLTypes::eDrop);               
	}
#line 4195 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 374:
#line 3267 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtJob *job = new AosJqlStmtJob;
		gAosJQLParser.appendStatement(job);   
		(yylhs.value.AosJqlStmtJobValue) = job;                             
		(yylhs.value.AosJqlStmtJobValue)->setOp(JQLTypes::eShow);               
	}
#line 4206 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 375:
#line 3277 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtJob *job = new AosJqlStmtJob;
		job->setJobName((yystack_[1].value.strval));
		gAosJQLParser.appendStatement(job);   
		(yylhs.value.AosJqlStmtJobValue) = job;                             
		(yylhs.value.AosJqlStmtJobValue)->setOp(JQLTypes::eDescribe);               
	}
#line 4218 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 376:
#line 3288 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtJob *job = new AosJqlStmtJob;
		/*job->setJobName($3);*/
		job->setLimit((yystack_[1].value.AosJqlLimitValue));
		job->setOp(JQLTypes::eShowStatus);               
		gAosJQLParser.appendStatement(job);   
		(yylhs.value.AosJqlStmtJobValue) = job; 
	}
#line 4231 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 377:
#line 3299 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtJob *job = new AosJqlStmtJob;
		job->setLimit((yystack_[1].value.AosJqlLimitValue));
		job->setIsLog(true);
		job->setJobName((yystack_[2].value.strval));
		job->setOp(JQLTypes::eShow);               
		gAosJQLParser.appendStatement(job);   
		(yylhs.value.AosJqlStmtJobValue) = job; 
	}
#line 4245 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 378:
#line 3312 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtJob *job = new AosJqlStmtJob;
		job->setJobName((yystack_[1].value.strval));
		job->setOp(JQLTypes::eWait);               
		gAosJQLParser.appendStatement(job);   
		(yylhs.value.AosJqlStmtJobValue) = job; 
	}
#line 4257 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 379:
#line 3322 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtJob *job = new AosJqlStmtJob;
		job->setJobName((yystack_[1].value.strval));
		gAosJQLParser.appendStatement(job);   
		(yylhs.value.AosJqlStmtJobValue) = job;                             
		(yylhs.value.AosJqlStmtJobValue)->setOp(JQLTypes::eRestart);               
	}
#line 4269 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 380:
#line 3336 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtJoinSyncher *stmt = new AosJqlStmtJoinSyncher;
		stmt->setJoinType("inner_join");
		stmt->setJoinTables((yystack_[10].value.AosExprListValue));
		stmt->setCondition((yystack_[6].value.AosExprValue));
		stmt->setJoinIndexs((yystack_[2].value.AosExprListValue));
		stmt->setOp(JQLTypes::eCreate);
		gAosJQLParser.appendStatement(stmt);
		(yylhs.value.AosJqlStmtJoinSyncherValue) = stmt;
	}
#line 4284 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 381:
#line 3348 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtLoadData *stmt = new AosJqlStmtLoadData();
		stmt->setLoadDataName((yystack_[4].value.strval));
		stmt->setConfParms((yystack_[2].value.AosExprNameValuesValue));
		gAosJQLParser.appendStatement(stmt);
		(yylhs.value.AosJqlStmtLoadDataValue) = stmt;
		(yylhs.value.AosJqlStmtLoadDataValue)->setOp(JQLTypes::eCreate);
	}
#line 4297 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 382:
#line 3358 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtLoadData *load_data = new AosJqlStmtLoadData();
		load_data->setVersionNum(2);
		load_data->setLoadDataName((yystack_[9].value.strval));
		load_data->setFromDataSet((yystack_[6].value.strval));
		load_data->setToDataSet((yystack_[4].value.strval));
		load_data->setMaxThread((yystack_[3].value.u32_value));
		load_data->setTaskNum((yystack_[2].value.u32_value));
		load_data->setOptionFields((yystack_[1].value.AosExprListValue));
		gAosJQLParser.appendStatement(load_data); 
		(yylhs.value.AosJqlStmtLoadDataValue) = load_data;                           
		(yylhs.value.AosJqlStmtLoadDataValue)->setOp(JQLTypes::eCreate);          
    }
#line 4315 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 383:
#line 3374 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtLoadData *load_data = new AosJqlStmtLoadData();
		load_data->setVersionNum(2);
		load_data->setLoadDataName((yystack_[16].value.strval));
		load_data->setFromDataSet((yystack_[13].value.strval));
		load_data->setToDataSet((yystack_[11].value.strval));
		load_data->setIsHbase(true);
		load_data->setRawkeyList((yystack_[5].value.AosExprListValue));
		load_data->setMaxThread((yystack_[3].value.u32_value));
		load_data->setTaskNum((yystack_[2].value.u32_value));
		load_data->setOptionFields((yystack_[1].value.AosExprListValue));
		gAosJQLParser.appendStatement(load_data); 
		(yylhs.value.AosJqlStmtLoadDataValue) = load_data;                           
		(yylhs.value.AosJqlStmtLoadDataValue)->setOp(JQLTypes::eCreate);
    }
#line 4335 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 384:
#line 3391 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtLoadData *stmt = new AosJqlStmtLoadData();
		stmt->setLoadDataName((yystack_[4].value.strval));
		stmt->setConfParms((yystack_[2].value.AosExprNameValuesValue));
		gAosJQLParser.appendStatement(stmt);
		(yylhs.value.AosJqlStmtLoadDataValue) = stmt;
		(yylhs.value.AosJqlStmtLoadDataValue)->setOp(JQLTypes::eDrop);
	}
#line 4348 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 385:
#line 3401 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtLoadData *load_data = new AosJqlStmtLoadData();
		load_data->setLoadDataName((yystack_[9].value.strval));
		load_data->setFromDataSet((yystack_[6].value.strval));
		load_data->setToDataSet((yystack_[4].value.strval));
		load_data->setMaxThread((yystack_[3].value.u32_value));
		load_data->setOptionFields((yystack_[1].value.AosExprListValue));
		load_data->setTaskNum((yystack_[2].value.u32_value));
		gAosJQLParser.appendStatement(load_data); 
		(yylhs.value.AosJqlStmtLoadDataValue) = load_data;                           
		(yylhs.value.AosJqlStmtLoadDataValue)->setOp(JQLTypes::eDrop);
    }
#line 4365 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 386:
#line 3416 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtLoadData *load_data = new AosJqlStmtLoadData();
		load_data->setLoadDataName((yystack_[16].value.strval));
		load_data->setFromDataSet((yystack_[13].value.strval));
		load_data->setToDataSet((yystack_[11].value.strval));
		load_data->setIsHbase(true);
		load_data->setRawkeyList((yystack_[5].value.AosExprListValue));
		load_data->setMaxThread((yystack_[3].value.u32_value));
		load_data->setOptionFields((yystack_[1].value.AosExprListValue));
		load_data->setTaskNum((yystack_[2].value.u32_value));
		gAosJQLParser.appendStatement(load_data); 
		(yylhs.value.AosJqlStmtLoadDataValue) = load_data;                           
		(yylhs.value.AosJqlStmtLoadDataValue)->setOp(JQLTypes::eDrop);
    }
#line 4384 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 387:
#line 3435 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtHBase *hbase = new AosJqlStmtHBase();
		hbase->setConfig((yystack_[1].value.strval));
		(yylhs.value.AosJqlStmtHBaseValue) = hbase;                           
		gAosJQLParser.appendStatement(hbase); 
		(yylhs.value.AosJqlStmtHBaseValue)->setOp(JQLTypes::eRun);          
	}
#line 4396 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 388:
#line 3446 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.u32_value) = 1;
	}
#line 4404 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 389:
#line 3451 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.u32_value) = (yystack_[0].value.ll_value);
	}
#line 4412 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 390:
#line 3457 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.u32_value) = 1;
	}
#line 4420 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 391:
#line 3462 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.u32_value) = (yystack_[0].value.ll_value);
	}
#line 4428 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 392:
#line 3468 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosExprListValue) = new AosExprList; 
	}
#line 4436 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 393:
#line 3473 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosExprListValue) = (yystack_[1].value.AosExprListValue);
	}
#line 4444 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 394:
#line 3484 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtMap *map = new AosJqlStmtMap;
		map->setMapName((yystack_[16].value.strval));
		map->setTableName((yystack_[13].value.strval));
		map->setKeys((yystack_[10].value.AosExprListValue));
		map->setValue((yystack_[6].value.AosExprValue));
		map->setDataType((yystack_[3].value.AosJQLDataFieldTypeInfoPtr));
		map->setMaxLen((yystack_[2].value.ll_value));
		if ((yystack_[1].value.AosJqlWhereValue))
			map->setWhereCond((yystack_[1].value.AosJqlWhereValue)->getWhereExpr().getPtr());
		map->setOp(JQLTypes::eCreate);               
		gAosJQLParser.appendStatement(map);   
		(yylhs.value.AosJqlStmtMapValues) = map;                             
	}
#line 4463 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 395:
#line 3501 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtMap *map = new AosJqlStmtMap;
		map->setMapName((yystack_[1].value.strval));
		gAosJQLParser.appendStatement(map);   
		(yylhs.value.AosJqlStmtMapValues) = map;                             
		(yylhs.value.AosJqlStmtMapValues)->setOp(JQLTypes::eDrop);               
	}
#line 4475 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 396:
#line 3512 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtMap *map = new AosJqlStmtMap;
		gAosJQLParser.appendStatement(map);   
		(yylhs.value.AosJqlStmtMapValues) = map;                             
		(yylhs.value.AosJqlStmtMapValues)->setOp(JQLTypes::eShow);               
	}
#line 4486 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 397:
#line 3522 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtMap *map = new AosJqlStmtMap;
		map->setMapName((yystack_[1].value.strval));
		gAosJQLParser.appendStatement(map);   
		(yylhs.value.AosJqlStmtMapValues) = map;                             
		(yylhs.value.AosJqlStmtMapValues)->setOp(JQLTypes::eDescribe);               
	}
#line 4498 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 398:
#line 3534 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtMap *map = new AosJqlStmtMap;
		map->setMapName((yystack_[1].value.strval));
		gAosJQLParser.appendStatement(map);   
		(yylhs.value.AosJqlStmtMapValues) = map;                             
		(yylhs.value.AosJqlStmtMapValues)->setOp(JQLTypes::eList);               
	}
#line 4510 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 399:
#line 3545 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtNickField *stmt = new AosJqlStmtNickField;
		stmt->setName((yystack_[4].value.strval));
		stmt->setConfParms((yystack_[2].value.AosExprNameValuesValue));
		gAosJQLParser.appendStatement(stmt);
		(yylhs.value.AosJqlStmtNickFieldValue) = stmt;
		(yylhs.value.AosJqlStmtNickFieldValue)->setOp(JQLTypes::eCreate);
	}
#line 4523 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 400:
#line 3557 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtNickField *field = new AosJqlStmtNickField;
		field->setName((yystack_[7].value.strval));
		field->setTable((yystack_[5].value.strval));
		field->setOrigFieldName((yystack_[3].value.strval));
		field->setMaxLen((yystack_[1].value.ll_value));
		gAosJQLParser.appendStatement(field);
		(yylhs.value.AosJqlStmtNickFieldValue) = field;
		(yylhs.value.AosJqlStmtNickFieldValue)->setOp(JQLTypes::eCreate);
	}
#line 4538 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 401:
#line 3571 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtRunScriptFile *stmt = new AosJqlStmtRunScriptFile;
		stmt->setFileName((yystack_[3].value.strval));
		stmt->setParameters((yystack_[2].value.AosExprListValue));
		stmt->setSuppress((yystack_[1].value.bool_val));
		gAosJQLParser.appendStatement(stmt);   
		(yylhs.value.AosJqlStmtRunScriptFileValue) = stmt;                             
	}
#line 4551 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 402:
#line 3582 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosExprListValue) = NULL;
	}
#line 4559 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 403:
#line 3587 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosExprListValue) = (yystack_[0].value.AosExprListValue);
	}
#line 4567 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 404:
#line 3593 "Parser.yy" // lalr1.cc:847
    {
		/* nil  */
		(yylhs.value.bool_val) = false;
	}
#line 4576 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 405:
#line 3599 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.bool_val) = true;
	}
#line 4584 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 406:
#line 3604 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.bool_val) = false;
	}
#line 4592 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 407:
#line 3611 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtSchedule *schedule = new AosJqlStmtSchedule;
		schedule->setScheduleName((yystack_[4].value.strval));
		schedule->setJobName((yystack_[1].value.strval));
		gAosJQLParser.appendStatement(schedule);   
		(yylhs.value.AosJqlStmtScheduleValue) = schedule;                             
		(yylhs.value.AosJqlStmtScheduleValue)->setOp(JQLTypes::eCreate);               
	}
#line 4605 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 408:
#line 3625 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtSchedule *schedule = new AosJqlStmtSchedule;
		schedule->setScheduleName((yystack_[16].value.strval));
		schedule->setJobName((yystack_[13].value.strval));
		schedule->setDateType((yystack_[11].value.strval));
		schedule->setDate((yystack_[8].value.strval));
		schedule->setRunTime((yystack_[7].value.strval));
		schedule->setStopTime((yystack_[3].value.strval));
		schedule->setPriority((yystack_[1].value.ll_value));
		gAosJQLParser.appendStatement(schedule);   
		(yylhs.value.AosJqlStmtScheduleValue) = schedule;                             
		(yylhs.value.AosJqlStmtScheduleValue)->setOp(JQLTypes::eCreate);               
	}
#line 4623 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 409:
#line 3644 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtSchedule *schedule = new AosJqlStmtSchedule;
		schedule->setScheduleName((yystack_[14].value.strval));
		schedule->setJobName((yystack_[11].value.strval));
		schedule->setDateType((yystack_[9].value.strval));
		schedule->setDate((yystack_[5].value.strval));
		schedule->setStopTime((yystack_[2].value.strval));
		schedule->setPriority((yystack_[1].value.ll_value));
		gAosJQLParser.appendStatement(schedule);   
		(yylhs.value.AosJqlStmtScheduleValue) = schedule;                             
		(yylhs.value.AosJqlStmtScheduleValue)->setOp(JQLTypes::eCreate);               

	}
#line 4641 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 410:
#line 3662 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtSchedule *schedule = new AosJqlStmtSchedule;
		schedule->setScheduleName((yystack_[10].value.strval));
		schedule->setJobName((yystack_[7].value.strval));
		schedule->setDateType((yystack_[5].value.strval));
		schedule->setDate((yystack_[2].value.strval));
		schedule->setPriority((yystack_[1].value.ll_value));
		gAosJQLParser.appendStatement(schedule);   
		(yylhs.value.AosJqlStmtScheduleValue) = schedule;                             
		(yylhs.value.AosJqlStmtScheduleValue)->setOp(JQLTypes::eCreate);               
	}
#line 4657 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 411:
#line 3676 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.ll_value) = 0;
	}
#line 4665 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 412:
#line 3681 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.ll_value) = (yystack_[0].value.ll_value);
	}
#line 4673 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 413:
#line 3688 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtSchedule *schedule = new AosJqlStmtSchedule;
		schedule->setScheduleName((yystack_[1].value.strval));
		gAosJQLParser.appendStatement(schedule);   
		(yylhs.value.AosJqlStmtScheduleValue) = schedule;                             
		(yylhs.value.AosJqlStmtScheduleValue)->setOp(JQLTypes::eRun);               
	}
#line 4685 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 414:
#line 3699 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtSchedule *schedule = new AosJqlStmtSchedule;
		schedule->setScheduleName((yystack_[1].value.strval));
		gAosJQLParser.appendStatement(schedule);   
		(yylhs.value.AosJqlStmtScheduleValue) = schedule;                             
		(yylhs.value.AosJqlStmtScheduleValue)->setOp(JQLTypes::eStop);               
	}
#line 4697 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 415:
#line 3710 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtSchedule *schedule = new AosJqlStmtSchedule;
		schedule->setScheduleName((yystack_[1].value.strval));
		gAosJQLParser.appendStatement(schedule);   
		(yylhs.value.AosJqlStmtScheduleValue) = schedule;                             
		(yylhs.value.AosJqlStmtScheduleValue)->setOp(JQLTypes::eDrop);               
	}
#line 4709 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 416:
#line 3723 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtSchema* statement = new AosJqlStmtSchema;
		statement->setName((yystack_[5].value.strval));
		statement->setType((yystack_[3].value.strval));
		statement->setRecord((yystack_[1].value.strval));
		statement->setSchemaType("static");
		gAosJQLParser.appendStatement(statement);
		(yylhs.value.AosJqlStmtSchemaValue) = statement;
		(yylhs.value.AosJqlStmtSchemaValue)->setOp(JQLTypes::eCreate);
	}
#line 4724 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 417:
#line 3737 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtSchema* statement = new AosJqlStmtSchema;
		OmnString type = "parser";
		statement->setRecordType(type);
		statement->setName((yystack_[5].value.strval));
		statement->setType((yystack_[3].value.strval));
		statement->setRecord((yystack_[1].value.strval));
		statement->setSchemaType("static");
		gAosJQLParser.appendStatement(statement);
		(yylhs.value.AosJqlStmtSchemaValue) = statement;
		(yylhs.value.AosJqlStmtSchemaValue)->setOp(JQLTypes::eCreate);
	}
#line 4741 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 418:
#line 3755 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtSchema* statement = new AosJqlStmtSchema;
		statement->setName((yystack_[14].value.strval));
		statement->setType("fixedlength");
		statement->setRecordLen(2000);
		statement->setRecordDelimiter((yystack_[11].value.strval));
		statement->setSchemaPos((yystack_[6].value.ll_value));
		statement->setSchemaLen((yystack_[4].value.ll_value));
		statement->setPickers((yystack_[2].value.AosJqlRecordPicksValue));
		statement->setSchemaType("static");
		gAosJQLParser.appendStatement(statement);
		(yylhs.value.AosJqlStmtSchemaValue) = statement;
		(yylhs.value.AosJqlStmtSchemaValue)->setOp(JQLTypes::eCreate);
	}
#line 4760 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 419:
#line 3774 "Parser.yy" // lalr1.cc:847
    {
	 	(yylhs.value.AosJqlRecordPicksValue) = new vector<AosJqlRecordPickerPtr>;
		AosJqlRecordPickerPtr record_pick = OmnNew AosJqlRecordPicker;
		record_pick->record = (yystack_[4].value.strval);
		record_pick->matchStr = (yystack_[2].value.strval);
		record_pick->offset = (yystack_[0].value.ll_value);
		free((yystack_[4].value.strval));
		free((yystack_[2].value.strval));
		(yylhs.value.AosJqlRecordPicksValue)->push_back(record_pick);
	}
#line 4775 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 420:
#line 3786 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlRecordPicksValue) = (yystack_[7].value.AosJqlRecordPicksValue);
		AosJqlRecordPickerPtr record_pick = OmnNew AosJqlRecordPicker;
		record_pick->record = (yystack_[4].value.strval);
		record_pick->matchStr = (yystack_[2].value.strval);
		record_pick->offset = (yystack_[0].value.ll_value);
		free((yystack_[4].value.strval));
		free((yystack_[2].value.strval));
		(yylhs.value.AosJqlRecordPicksValue)->push_back(record_pick);
	}
#line 4790 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 421:
#line 3800 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtSchema* statement = new AosJqlStmtSchema;

		gAosJQLParser.appendStatement(statement);

		(yylhs.value.AosJqlStmtSchemaValue) = statement;
		(yylhs.value.AosJqlStmtSchemaValue)->setOp(JQLTypes::eShow);
		//cout << "List all the Schema " << endl;
	}
#line 4804 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 422:
#line 3812 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtSchema* statement = new AosJqlStmtSchema;
		gAosJQLParser.appendStatement(statement);

		(yylhs.value.AosJqlStmtSchemaValue) = statement;
		(yylhs.value.AosJqlStmtSchemaValue)->setName((yystack_[1].value.strval));
		(yylhs.value.AosJqlStmtSchemaValue)->setOp(JQLTypes::eDescribe);
	}
#line 4817 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 423:
#line 3823 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtSchema* statement = new AosJqlStmtSchema;
		statement->setName((yystack_[1].value.strval));

		gAosJQLParser.appendStatement(statement);
		(yylhs.value.AosJqlStmtSchemaValue) = statement;
		(yylhs.value.AosJqlStmtSchemaValue)->setOp(JQLTypes::eDrop);
	}
#line 4830 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 424:
#line 3834 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtQuery * select = new AosJqlStmtQuery();
		gAosJQLParser.appendStatement(select);             
		select->setExprs((yystack_[1].value.AosJqlSelectFieldListValue));
		(yylhs.value.AosJqlStmtQueryValue) = select;   
	}
#line 4841 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 425:
#line 3842 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtQuery * select = new AosJqlStmtQuery((yystack_[1].value.AosJqlSelectValue));
		gAosJQLParser.appendStatement(select);
		(yylhs.value.AosJqlStmtQueryValue) = select;
	//	$$->mQueryStr =gAosJQLParser.getCurrJQL();
	}
#line 4852 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 426:
#line 3856 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlSelectValue) =new AosJqlSelect;
		//s1->mType = $2;
		(yylhs.value.AosJqlSelectValue)->mFieldListPtr = (yystack_[10].value.AosJqlSelectFieldListValue);
		(yylhs.value.AosJqlSelectValue)->mTableReferencesPtr = (yystack_[8].value.AosTableReferencesValue);
		(yylhs.value.AosJqlSelectValue)->mWherePtr = (yystack_[7].value.AosJqlWhereValue);
		(yylhs.value.AosJqlSelectValue)->mGroupByPtr = (yystack_[6].value.AosJqlGroupByValue);
		(yylhs.value.AosJqlSelectValue)->mHavingPtr = (yystack_[5].value.AosJqlHavingValue);
		(yylhs.value.AosJqlSelectValue)->mOrderByPtr = (yystack_[4].value.AosJqlOrderByValue);
		(yylhs.value.AosJqlSelectValue)->mLimitPtr = (yystack_[3].value.AosJqlLimitValue);
		(yylhs.value.AosJqlSelectValue)->mHackConvertTo = (yystack_[2].value.AosExprValue);
		(yylhs.value.AosJqlSelectValue)->mHackFormat = (yystack_[1].value.AosExprValue);
		(yylhs.value.AosJqlSelectValue)->mHackIntoFile = (yystack_[0].value.AosExprValue);
    }
#line 4871 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 427:
#line 3875 "Parser.yy" // lalr1.cc:847
    { 
		(yylhs.value.AosJQLStmtSelectOptionListValue) = new AosJQLStmtSelectOptionList; 
	}
#line 4879 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 428:
#line 3880 "Parser.yy" // lalr1.cc:847
    { 
		(yylhs.value.AosJQLStmtSelectOptionListValue)->push_back(eSelectOpt_SelectAll); 
	}
#line 4887 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 429:
#line 3885 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJQLStmtSelectOptionListValue)->push_back(eSelectOpt_Distinct);
	}
#line 4895 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 430:
#line 3890 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJQLStmtSelectOptionListValue)->push_back(eSelectOpt_DistinctRow);
	}
#line 4903 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 431:
#line 3895 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJQLStmtSelectOptionListValue)->push_back(eSelectOpt_HighPriority);
	}
#line 4911 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 432:
#line 3900 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJQLStmtSelectOptionListValue)->push_back(eSelectOpt_StraightJoin);
	}
#line 4919 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 433:
#line 3905 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJQLStmtSelectOptionListValue)->push_back(eSelectOpt_SmallResult);
	}
#line 4927 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 434:
#line 3910 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJQLStmtSelectOptionListValue)->push_back(eSelectOpt_BigResult);
	}
#line 4935 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 435:
#line 3915 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJQLStmtSelectOptionListValue)->push_back(eSelectOpt_CallFoundRows);
	}
#line 4943 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 436:
#line 3922 "Parser.yy" // lalr1.cc:847
    { 
		(yylhs.value.AosJqlSelectFieldListValue) = new AosJqlSelectFieldList;
		(yylhs.value.AosJqlSelectFieldListValue)->push_back((yystack_[0].value.AosJqlSelectFieldValue));
	}
#line 4952 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 437:
#line 3928 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlSelectFieldListValue)->push_back((yystack_[0].value.AosJqlSelectFieldValue)); 
	}
#line 4960 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 438:
#line 3935 "Parser.yy" // lalr1.cc:847
    {
		AosJqlSelectField *field = new AosJqlSelectField; 
		if ((yystack_[1].value.AosExprValue)->getType() == AosExprType::eGenFunc)
		{
			AosExprGenFuncPtr expr_func;
			expr_func = dynamic_cast<AosExprGenFunc*>((yystack_[1].value.AosExprValue));
			AosExprList *parmlist = expr_func->getParmList();
			OmnString func_name = expr_func->getFuctName();
			if (func_name.toLower() == "avg" && parmlist->size() == 1)
			{
				AosExprObjPtr lhs = OmnNew AosExprGenFunc("sum", (*parmlist)[0]);
				AosExprObjPtr rhs = OmnNew AosExprGenFunc("count", (*parmlist)[0]);
				AosExprObjPtr expr = new AosExprArith(lhs, AosExprArith::eDivide, rhs);
				field->setField(expr);
				if ((yystack_[0].value.AosExprValue))
					field->setAliasName((yystack_[0].value.AosExprValue));
				else
					field->setAliasName(OmnNew AosExprString(expr_func->getStatFieldName().getBuffer()));
			}
			else
			{
				field->setField((yystack_[1].value.AosExprValue));
				field->setAliasName((yystack_[0].value.AosExprValue));
			}
		}
		else
		{
			field->setField((yystack_[1].value.AosExprValue));
			field->setAliasName((yystack_[0].value.AosExprValue));
		}
		(yylhs.value.AosJqlSelectFieldValue) = field;
	}
#line 4997 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 439:
#line 3969 "Parser.yy" // lalr1.cc:847
    {
		 AosJqlSelectField *field = new AosJqlSelectField;
		 AosExprObjPtr expr = OmnNew AosExprFieldName("*");
		 field->setField(expr);
		 (yylhs.value.AosJqlSelectFieldValue) = field;
	}
#line 5008 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 440:
#line 3979 "Parser.yy" // lalr1.cc:847
    { 
		(yylhs.value.AosTableReferencesValue) = new AosTableReferences;
		(yylhs.value.AosTableReferencesValue)->push_back((yystack_[0].value.AosJqlTableReferenceValue));
	}
#line 5017 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 441:
#line 3985 "Parser.yy" // lalr1.cc:847
    { 
		(yylhs.value.AosTableReferencesValue)->push_back((yystack_[0].value.AosJqlTableReferenceValue)); 
	}
#line 5025 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 442:
#line 3992 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlTableReferenceValue) = new AosJqlTableReference;
		(yylhs.value.AosJqlTableReferenceValue)->setTableFactor((yystack_[0].value.AosJqlTableFactorValue));
	}
#line 5034 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 443:
#line 3998 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlTableReferenceValue) = new AosJqlTableReference;
		(yylhs.value.AosJqlTableReferenceValue)->setJqlJoinTable((yystack_[0].value.AosJqlJoinTableValue));
	}
#line 5043 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 444:
#line 4006 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlJoinTableValue) = new AosJqlJoinTable;
	}
#line 5051 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 445:
#line 4015 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlTableFactorValue) = new AosJqlTableFactor;
		(yylhs.value.AosJqlTableFactorValue)->setType(eTRef_TableFactorByTable);
		(yylhs.value.AosJqlTableFactorValue)->setAliasName((yystack_[1].value.AosExprValue));
		(yylhs.value.AosJqlTableFactorValue)->setTable((yystack_[2].value.AosExprValue));
		(yylhs.value.AosJqlTableFactorValue)->setIndexHint((yystack_[0].value.AosExprListValue));
	}
#line 5063 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 446:
#line 4024 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlTableFactorValue) = new AosJqlTableFactor;
		/*$$->setVirtualTable($1);*/
		(yylhs.value.AosJqlTableFactorValue)->setQuery((yystack_[2].value.AosJqlStmtQueryValue));
		(yylhs.value.AosJqlTableFactorValue)->setTable(new AosExprFieldName((yystack_[0].value.strval)));
		(yylhs.value.AosJqlTableFactorValue)->setAliasName(new AosExprFieldName((yystack_[0].value.strval)));

	}
#line 5076 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 447:
#line 4034 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlTableFactorValue) = new AosJqlTableFactor;
		(yylhs.value.AosJqlTableFactorValue)->setTable(new AosExprMemberOpt("", (yystack_[4].value.strval), (yystack_[2].value.strval)));
		(yylhs.value.AosJqlTableFactorValue)->setAliasName((yystack_[1].value.AosExprValue));
		(yylhs.value.AosJqlTableFactorValue)->setIndexHint((yystack_[0].value.AosExprListValue));
	}
#line 5087 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 448:
#line 4044 "Parser.yy" // lalr1.cc:847
    {
		/*
		$$ = new AosJqlSubQueryTable;
		$$->setVirtualTable($2);
		$$->setName($5);
		*/
		AosJqlStmtQuery *select = new AosJqlStmtQuery((yystack_[1].value.AosJqlSelectValue));
		(yylhs.value.AosJqlStmtQueryValue) = select;
	}
#line 5101 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 449:
#line 4056 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosExprListValue) = NULL;
	}
#line 5109 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 450:
#line 4061 "Parser.yy" // lalr1.cc:847
    {
		/* index hint */			
		(yylhs.value.AosExprListValue) = (yystack_[1].value.AosExprListValue);
	}
#line 5118 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 451:
#line 4070 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlLimitValue) = 0;
	//  $$ = new AosExprList;
	}
#line 5127 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 452:
#line 4076 "Parser.yy" // lalr1.cc:847
    {
	  (yylhs.value.AosJqlLimitValue) = new AosJqlLimit;
	  (yylhs.value.AosJqlLimitValue)->setOffset(0);
	  (yylhs.value.AosJqlLimitValue)->setRowCount((yystack_[0].value.ll_value));
	}
#line 5137 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 453:
#line 4083 "Parser.yy" // lalr1.cc:847
    {
	  (yylhs.value.AosJqlLimitValue) = new AosJqlLimit;
	  if ((yystack_[2].value.ll_value) < 0) (yystack_[2].value.ll_value) = 0;
	  (yylhs.value.AosJqlLimitValue)->setOffset((yystack_[2].value.ll_value));
	  (yylhs.value.AosJqlLimitValue)->setRowCount((yystack_[0].value.ll_value));
	}
#line 5148 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 454:
#line 4093 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlGroupByValue) = 0;
	}
#line 5156 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 455:
#line 4098 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlGroupByValue) = new AosJqlGroupBy;
		(yylhs.value.AosJqlGroupByValue)->setGroupFieldList((yystack_[0].value.AosJqlSelectFieldListValue));
	}
#line 5165 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 456:
#line 4104 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlGroupByValue) = new AosJqlGroupBy;
		(yylhs.value.AosJqlGroupByValue)->setRollupLists((yystack_[1].value.AosJqlSelectFieldListVecValue));
	}
#line 5174 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 457:
#line 4110 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlGroupByValue) = new AosJqlGroupBy;
		(yylhs.value.AosJqlGroupByValue)->setCubeLists((yystack_[1].value.AosJqlSelectFieldListValue));
	}
#line 5183 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 458:
#line 4116 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlGroupByValue) = new AosJqlGroupBy;
		(yylhs.value.AosJqlGroupByValue)->setGroupFieldList((yystack_[5].value.AosJqlSelectFieldListValue));
		(yylhs.value.AosJqlGroupByValue)->setCubeLists((yystack_[1].value.AosJqlSelectFieldListValue));
	}
#line 5193 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 459:
#line 4125 "Parser.yy" // lalr1.cc:847
    {
		vector<AosJqlSelectFieldList*> *rollupLists;

		rollupLists = new vector<AosJqlSelectFieldList*>;
		rollupLists->push_back((yystack_[1].value.AosJqlSelectFieldListValue));
		(yylhs.value.AosJqlSelectFieldListVecValue) = rollupLists;
	}
#line 5205 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 460:
#line 4134 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlSelectFieldListVecValue) = (yystack_[4].value.AosJqlSelectFieldListVecValue);
		(yylhs.value.AosJqlSelectFieldListVecValue)->push_back((yystack_[1].value.AosJqlSelectFieldListValue));
	}
#line 5214 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 461:
#line 4141 "Parser.yy" // lalr1.cc:847
    {
		/*nil, init a expression list*/
		AosJqlSelectFieldList *rollupList = new AosJqlSelectFieldList;
		(yylhs.value.AosJqlSelectFieldListValue) = rollupList;
	}
#line 5224 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 462:
#line 4148 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlSelectFieldListValue) = (yystack_[0].value.AosJqlSelectFieldListValue);
	}
#line 5232 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 463:
#line 4154 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlOrderByValue) = 0;
	}
#line 5240 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 464:
#line 4159 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlOrderByValue) = new AosJqlOrderBy;	
		(yylhs.value.AosJqlOrderByValue)->setOrderFieldList((yystack_[0].value.AosJqlOrderByFieldListValue));
	}
#line 5249 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 465:
#line 4167 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlOrderByFieldValue) =  new AosJqlOrderByField;
		AosJqlSelectField *field = new AosJqlSelectField;
		field->setField((yystack_[1].value.AosExprValue));
		(yylhs.value.AosJqlOrderByFieldValue)->mField = field;
		(yylhs.value.AosJqlOrderByFieldValue)->mIsAsc = (yystack_[0].value.bool_val);
	}
#line 5261 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 466:
#line 4178 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlOrderByFieldListValue) = new AosJqlOrderByFieldList;
		(yylhs.value.AosJqlOrderByFieldListValue)->push_back((yystack_[0].value.AosJqlOrderByFieldValue));
	}
#line 5270 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 467:
#line 4184 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlOrderByFieldListValue)->push_back((yystack_[0].value.AosJqlOrderByFieldValue));
	}
#line 5278 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 468:
#line 4190 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.bool_val) = true;
	}
#line 5286 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 469:
#line 4195 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.bool_val) = false;
	}
#line 5294 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 470:
#line 4200 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.bool_val) = true;
	}
#line 5302 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 471:
#line 4206 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlHavingValue) = 0;
	}
#line 5310 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 472:
#line 4211 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlHavingValue) = new AosJqlHaving();
		(yylhs.value.AosJqlHavingValue)->setHavingExpr((yystack_[0].value.AosExprValue));
	}
#line 5319 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 473:
#line 4218 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosExprValue) = NULL;
	}
#line 5327 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 474:
#line 4223 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosExprValue) = (yystack_[0].value.AosExprValue);
	}
#line 5335 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 475:
#line 4228 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosExprValue) = NULL;
	}
#line 5343 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 476:
#line 4233 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosExprValue) = (yystack_[0].value.AosExprValue);
	}
#line 5351 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 477:
#line 4238 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosExprValue) = NULL;
	}
#line 5359 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 478:
#line 4243 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosExprValue) = (yystack_[0].value.AosExprValue);
	}
#line 5367 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 479:
#line 4249 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtSelectInto * selectinto = new AosJqlStmtSelectInto();
		selectinto->setSelect((yystack_[6].value.AosJqlSelectValue));
		selectinto->setFilePath((yystack_[4].value.strval));
		selectinto->setJobName((yystack_[3].value.strval));
		selectinto->setFormat((yystack_[2].value.AosStrListValue));
		selectinto->setMaxThreads((yystack_[1].value.u32_value));
		gAosJQLParser.appendStatement(selectinto);             
		(yylhs.value.AosJqlSelectIntoValue) = selectinto;   
	}
#line 5382 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 480:
#line 4263 "Parser.yy" // lalr1.cc:847
    {	
		(yylhs.value.AosStrListValue) = 0;
	}
#line 5390 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 481:
#line 4268 "Parser.yy" // lalr1.cc:847
    {
		vector<OmnString> *v = new vector<OmnString>; 
		v->push_back((yystack_[13].value.strval));
		v->push_back((yystack_[10].value.strval));
		v->push_back((yystack_[7].value.strval));
		v->push_back((yystack_[3].value.strval));
		v->push_back((yystack_[0].value.strval));
		(yylhs.value.AosStrListValue) = v;
	}
#line 5404 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 482:
#line 4280 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = 0;
	}
#line 5412 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 483:
#line 4285 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = (yystack_[0].value.strval);
	}
#line 5420 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 484:
#line 4291 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtSequence* stmt = new AosJqlStmtSequence;
		stmt->setName((yystack_[2].value.strval));
		stmt->setSequenceInfo((yystack_[1].value.AosJQLSequenceInfoValue));
		gAosJQLParser.appendStatement(stmt);
		(yylhs.value.AosJqlStmtSequenceValue) = stmt;
		(yylhs.value.AosJqlStmtSequenceValue)->setOp(JQLTypes::eCreate);
	}
#line 5433 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 485:
#line 4308 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJQLSequenceInfoValue) = new AosJQLSequenceInfo;
		(yylhs.value.AosJQLSequenceInfoValue)->setIncrementBy((yystack_[6].value.ll_value));
		(yylhs.value.AosJQLSequenceInfoValue)->setStartWith((yystack_[5].value.ll_value));
		(yylhs.value.AosJQLSequenceInfoValue)->setMaxValue((yystack_[4].value.ll_value));
		(yylhs.value.AosJQLSequenceInfoValue)->setMinValue((yystack_[3].value.ll_value));
		(yylhs.value.AosJQLSequenceInfoValue)->setIsCycle((yystack_[2].value.bool_val));
		(yylhs.value.AosJQLSequenceInfoValue)->setCacheNum((yystack_[1].value.ll_value));
		(yylhs.value.AosJQLSequenceInfoValue)->setIsOrder((yystack_[0].value.bool_val));
	}
#line 5448 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 486:
#line 4320 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.ll_value) = 1;
	}
#line 5456 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 487:
#line 4325 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.ll_value) = (yystack_[0].value.ll_value); 
	}
#line 5464 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 488:
#line 4330 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.ll_value) = 1;
	}
#line 5472 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 489:
#line 4335 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.ll_value) = (yystack_[0].value.ll_value);	
	}
#line 5480 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 490:
#line 4340 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.ll_value) = ~(1 << 31);
	}
#line 5488 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 491:
#line 4345 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.ll_value) = ~(1 << 31);
	}
#line 5496 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 492:
#line 4350 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.ll_value) = (yystack_[0].value.ll_value);
	}
#line 5504 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 493:
#line 4355 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.ll_value) = 1 << 31;
	}
#line 5512 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 494:
#line 4360 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.ll_value) = 1 << 31;
	}
#line 5520 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 495:
#line 4365 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.ll_value) = (yystack_[0].value.ll_value);
	}
#line 5528 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 496:
#line 4370 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.bool_val) = false;
	}
#line 5536 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 497:
#line 4375 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.bool_val) = true;
	}
#line 5544 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 498:
#line 4380 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.bool_val) = false;
	}
#line 5552 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 499:
#line 4385 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.ll_value) = 20;
	}
#line 5560 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 500:
#line 4390 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.ll_value) = (yystack_[0].value.ll_value);
	}
#line 5568 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 501:
#line 4395 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.ll_value) = 20;
	}
#line 5576 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 502:
#line 4400 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.bool_val) = false;
	}
#line 5584 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 503:
#line 4405 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.bool_val) = true;
	}
#line 5592 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 504:
#line 4410 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.bool_val) = false;
	}
#line 5600 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 505:
#line 4415 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtService* statement = new AosJqlStmtService;

		statement->setServiceName((yystack_[4].value.strval));
		statement->setJobName((yystack_[1].value.strval));
		gAosJQLParser.appendStatement(statement);

		(yylhs.value.AosJqlStmtServiceValue) = statement;
		(yylhs.value.AosJqlStmtServiceValue)->setOp(JQLTypes::eCreate);
	}
#line 5615 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 506:
#line 4429 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtService* statement = new AosJqlStmtService();	
		gAosJQLParser.appendStatement(statement);

		statement->setServiceName((yystack_[1].value.strval));
		(yylhs.value.AosJqlStmtServiceValue) = statement;
		(yylhs.value.AosJqlStmtServiceValue)->setOp(JQLTypes::eStart); 
    }
#line 5628 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 507:
#line 4439 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtService *stmt = new AosJqlStmtService;
		stmt->setServiceName((yystack_[4].value.strval));
		stmt->setConfParms((yystack_[2].value.AosExprNameValuesValue));
		gAosJQLParser.appendStatement(stmt);
		(yylhs.value.AosJqlStmtServiceValue) = stmt;
		(yylhs.value.AosJqlStmtServiceValue)->setOp(JQLTypes::eStart);
	}
#line 5641 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 508:
#line 4451 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtService* statement = new AosJqlStmtService();	
		gAosJQLParser.appendStatement(statement);

		statement->setServiceName((yystack_[1].value.strval));
		(yylhs.value.AosJqlStmtServiceValue) = statement;
		(yylhs.value.AosJqlStmtServiceValue)->setOp(JQLTypes::eStop); 
    }
#line 5654 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 509:
#line 4463 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtService* statement = new AosJqlStmtService();	
		gAosJQLParser.appendStatement(statement);

		(yylhs.value.AosJqlStmtServiceValue) = statement;
		(yylhs.value.AosJqlStmtServiceValue)->setOp(JQLTypes::eShow); 
    }
#line 5666 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 510:
#line 4474 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtService* statement = new AosJqlStmtService();	
		gAosJQLParser.appendStatement(statement);

		statement->setServiceName((yystack_[1].value.strval));
		(yylhs.value.AosJqlStmtServiceValue) = statement;
		(yylhs.value.AosJqlStmtServiceValue)->setOp(JQLTypes::eDescribe); 
    }
#line 5679 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 511:
#line 4485 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtService* statement = new AosJqlStmtService();	
		gAosJQLParser.appendStatement(statement);

		statement->setServiceName((yystack_[1].value.strval));
		(yylhs.value.AosJqlStmtServiceValue) = statement;
		(yylhs.value.AosJqlStmtServiceValue)->setOp(JQLTypes::eDrop); 
    }
#line 5692 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 512:
#line 4496 "Parser.yy" // lalr1.cc:847
    {
		/*
		Kttttt
		AosJqlStmtStat *stat = new AosJqlStmtStat;
		stat->setDataSetName($3);
		stat->setStatDocName($4);
		stat->setMaxThread($5);
		gAosJQLParser.appendStatement(stat);   
		$$ = stat;                             
		$$->setOp(JQLTypes::eRun);              
		*/
	}
#line 5709 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 513:
#line 4511 "Parser.yy" // lalr1.cc:847
    {
		/*
		Kttttt
		AosJqlStmtStat *stat = new AosJqlStmtStat;
		stat->setDataSetName($3);
		stat->setStatDocName($4);
		stat->setKeyFields($8);
		stat->setMaxThread($10);
		gAosJQLParser.appendStatement(stat);   
		$$ = stat;                             
		$$->setOp(JQLTypes::eRun);               
		*/
	}
#line 5727 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 514:
#line 4527 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtStatistics* statement = new AosJqlStmtStatistics;
		statement->setStatName((yystack_[4].value.strval));
		statement->setTableName((yystack_[1].value.strval));
		gAosJQLParser.appendStatement(statement);
		(yylhs.value.AosJqlStmtStatisticsValue) = statement;
		(yylhs.value.AosJqlStmtStatisticsValue)->setOp(JQLTypes::eCreate);
	}
#line 5740 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 515:
#line 4540 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtStatistics* statement = new AosJqlStmtStatistics;
		statement->setStatName((yystack_[15].value.strval));
		statement->setTableNames((yystack_[12].value.AosExprListValue));
		if ((yystack_[11].value.AosJqlWhereValue) != 0)
			statement->setWhereCond((yystack_[11].value.AosJqlWhereValue)->getWhereExpr().getPtr());
		statement->setGroupByFields((yystack_[7].value.AosExprListVecValue));
		statement->setMeasures((yystack_[3].value.AosMeasuresValue));
		statement->setShuffleField((yystack_[1].value.AosExprListValue));
		gAosJQLParser.appendStatement(statement);
		(yylhs.value.AosJqlStmtStatisticsValue) = statement;
		(yylhs.value.AosJqlStmtStatisticsValue)->setOp(JQLTypes::eCreate);
	}
#line 5758 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 516:
#line 4557 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtStatistics* statement = new AosJqlStmtStatistics;
		statement->setStatName((yystack_[10].value.strval));
		statement->setTableNames((yystack_[7].value.AosExprListValue));
		if ((yystack_[6].value.AosJqlWhereValue) != 0)
			statement->setWhereCond((yystack_[6].value.AosJqlWhereValue)->getWhereExpr().getPtr());
		statement->setMeasures((yystack_[3].value.AosMeasuresValue));
		statement->setShuffleField((yystack_[1].value.AosExprListValue));
		
		gAosJQLParser.appendStatement(statement);
		(yylhs.value.AosJqlStmtStatisticsValue) = statement;
		(yylhs.value.AosJqlStmtStatisticsValue)->setOp(JQLTypes::eCreate);
	}
#line 5776 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 517:
#line 4575 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtStatistics* statement = new AosJqlStmtStatistics;
		statement->setStatName((yystack_[15].value.strval));
		statement->setTableNames((yystack_[12].value.AosExprListValue));
		if ((yystack_[11].value.AosJqlWhereValue) != 0)
			statement->setWhereCond((yystack_[11].value.AosJqlWhereValue)->getWhereExpr().getPtr());
		statement->setMeasures((yystack_[8].value.AosMeasuresValue));
		statement->setTimeFieldName((yystack_[4].value.AosExprValue));
		statement->setTimeUnit((yystack_[2].value.strval));
		statement->setShuffleField((yystack_[1].value.AosExprListValue));

		gAosJQLParser.appendStatement(statement);
		(yylhs.value.AosJqlStmtStatisticsValue) = statement;
		(yylhs.value.AosJqlStmtStatisticsValue)->setOp(JQLTypes::eCreate);
	}
#line 5796 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 518:
#line 4595 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtStatistics* statement = new AosJqlStmtStatistics;
		statement->setStatName((yystack_[17].value.strval));
		statement->setTableNames((yystack_[14].value.AosExprListValue));
		if ((yystack_[13].value.AosJqlWhereValue) != 0)
			statement->setWhereCond((yystack_[13].value.AosJqlWhereValue)->getWhereExpr().getPtr());
		statement->setMeasures((yystack_[10].value.AosMeasuresValue));
		statement->setTimeFieldName((yystack_[6].value.AosExprValue));
		statement->setTimeOriginalFormat((yystack_[4].value.strval));
		statement->setTimeUnit((yystack_[2].value.strval));
		statement->setShuffleField((yystack_[1].value.AosExprListValue));

		gAosJQLParser.appendStatement(statement);
		(yylhs.value.AosJqlStmtStatisticsValue) = statement;
		(yylhs.value.AosJqlStmtStatisticsValue)->setOp(JQLTypes::eCreate);
	}
#line 5817 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 519:
#line 4617 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtStatistics* statement = new AosJqlStmtStatistics;
		statement->setStatName((yystack_[20].value.strval));
		statement->setTableNames((yystack_[17].value.AosExprListValue));
		if ((yystack_[16].value.AosJqlWhereValue) != 0)
			statement->setWhereCond((yystack_[16].value.AosJqlWhereValue)->getWhereExpr().getPtr());
		statement->setGroupByFields((yystack_[12].value.AosExprListVecValue));
		statement->setMeasures((yystack_[8].value.AosMeasuresValue));
		statement->setTimeFieldName((yystack_[4].value.AosExprValue));
		statement->setTimeUnit((yystack_[2].value.strval));
		statement->setShuffleField((yystack_[1].value.AosExprListValue));

		gAosJQLParser.appendStatement(statement);
		(yylhs.value.AosJqlStmtStatisticsValue) = statement;
		(yylhs.value.AosJqlStmtStatisticsValue)->setOp(JQLTypes::eCreate);
	}
#line 5838 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 520:
#line 4639 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtStatistics* statement = new AosJqlStmtStatistics;
		statement->setStatName((yystack_[22].value.strval));
		statement->setTableNames((yystack_[19].value.AosExprListValue));
		if ((yystack_[18].value.AosJqlWhereValue) != 0)
			statement->setWhereCond((yystack_[18].value.AosJqlWhereValue)->getWhereExpr().getPtr());
		statement->setGroupByFields((yystack_[14].value.AosExprListVecValue));
		statement->setMeasures((yystack_[10].value.AosMeasuresValue));
		statement->setTimeFieldName((yystack_[6].value.AosExprValue));
		statement->setTimeOriginalFormat((yystack_[4].value.strval));
		statement->setTimeUnit((yystack_[2].value.strval));
		statement->setShuffleField((yystack_[1].value.AosExprListValue));

		gAosJQLParser.appendStatement(statement);
		(yylhs.value.AosJqlStmtStatisticsValue) = statement;
		(yylhs.value.AosJqlStmtStatisticsValue)->setOp(JQLTypes::eCreate);
	}
#line 5860 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 521:
#line 4659 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtStatistics* statement = new AosJqlStmtStatistics;
		statement->setStatName((yystack_[9].value.strval));
		statement->setTableName((yystack_[6].value.strval));
		statement->setDimensions((yystack_[2].value.AosExprListVecValue));

		gAosJQLParser.appendStatement(statement);
		(yylhs.value.AosJqlStmtStatisticsValue) = statement;
		(yylhs.value.AosJqlStmtStatisticsValue)->setOp(JQLTypes::eUpdate);
	}
#line 5875 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 522:
#line 4671 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtStatistics* statement = new AosJqlStmtStatistics;
		statement->setStatName((yystack_[1].value.strval));
		gAosJQLParser.appendStatement(statement);
		(yylhs.value.AosJqlStmtStatisticsValue) = statement;
		(yylhs.value.AosJqlStmtStatisticsValue)->setOp(JQLTypes::eDrop);
	}
#line 5887 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 523:
#line 4681 "Parser.yy" // lalr1.cc:847
    {
	 	(yylhs.value.AosExprListValue) = NULL;
	 }
#line 5895 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 524:
#line 4686 "Parser.yy" // lalr1.cc:847
    {
	 	(yylhs.value.AosExprListValue) = new AosExprList;
		(yylhs.value.AosExprListValue) = (yystack_[1].value.AosExprListValue);
	 }
#line 5904 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 525:
#line 4693 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosExprListVecValue) = new vector<AosExprList*>;
		(yylhs.value.AosExprListVecValue)->push_back((yystack_[0].value.AosExprListValue));
	}
#line 5913 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 526:
#line 4699 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosExprListVecValue)->push_back((yystack_[0].value.AosExprListValue));
	}
#line 5921 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 527:
#line 4706 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosExprListValue) = (yystack_[0].value.AosExprListValue);
	}
#line 5929 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 528:
#line 4711 "Parser.yy" // lalr1.cc:847
    {
		OmnString distName = "distinct:";
		AosExprObj *expr;
		
		(yylhs.value.AosExprListValue) = (yystack_[0].value.AosExprListValue);
		distName << (yystack_[2].value.strval);
		expr = new AosExprFieldName(distName.getBuffer());
		(yylhs.value.AosExprListValue)->push_back(expr);
	}
#line 5943 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 529:
#line 4724 "Parser.yy" // lalr1.cc:847
    {
			(yylhs.value.AosMeasuresValue) = new vector<AosJqlStmtStatistics::AosMeasure*>;
			(yylhs.value.AosMeasuresValue)->push_back((yystack_[0].value.AosMeasureValue));
		}
#line 5952 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 530:
#line 4730 "Parser.yy" // lalr1.cc:847
    {
			(yylhs.value.AosMeasuresValue)->push_back((yystack_[0].value.AosMeasureValue));
		}
#line 5960 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 531:
#line 4737 "Parser.yy" // lalr1.cc:847
    {
			(yylhs.value.AosMeasureValue) = new AosJqlStmtStatistics::AosMeasure;
			(yylhs.value.AosMeasureValue)->mExpr = (yystack_[0].value.AosExprValue);
		}
#line 5969 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 532:
#line 4743 "Parser.yy" // lalr1.cc:847
    {
			(yylhs.value.AosMeasureValue) = new AosJqlStmtStatistics::AosMeasure;
			(yylhs.value.AosMeasureValue)->mExpr = (yystack_[2].value.AosExprValue);
			(yylhs.value.AosMeasureValue)->mExprList = (yystack_[0].value.AosExprListValue);
		}
#line 5979 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 533:
#line 4753 "Parser.yy" // lalr1.cc:847
    {
	    AosJqlStmtStatistics *statistics = new AosJqlStmtStatistics;
	  	gAosJQLParser.appendStatement(statistics);   
	    (yylhs.value.AosJqlStmtStatisticsValue) = statistics;                             
		(yylhs.value.AosJqlStmtStatisticsValue)->setOp(JQLTypes::eShow);
	}
#line 5990 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 534:
#line 4764 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtStatistics *statistics = new AosJqlStmtStatistics;
		statistics->setStatName((yystack_[1].value.strval));
		gAosJQLParser.appendStatement(statistics);   
		(yylhs.value.AosJqlStmtStatisticsValue) = statistics;                             
		(yylhs.value.AosJqlStmtStatisticsValue)->setOp(JQLTypes::eDescribe);        
	}
#line 6002 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 535:
#line 4774 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtCompleteIIL *statement = new AosJqlStmtCompleteIIL;
		OmnString iilname = (yystack_[1].value.strval);
		statement->setIILName(iilname);
		gAosJQLParser.appendStatement(statement);

		(yylhs.value.AosJqlStmtCompleteIILValue) = statement;
	}
#line 6015 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 536:
#line 4785 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtSyncer * statement = new AosJqlStmtSyncer;
		statement->setName((yystack_[5].value.AosExprValue));
		statement->setParms((yystack_[2].value.AosExprListValue));
		gAosJQLParser.appendStatement(statement);
		(yylhs.value.AosJqlStmtSyncerValue) = statement;
		(yylhs.value.AosJqlStmtSyncerValue)->setOp(JQLTypes::eCreate);
	}
#line 6028 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 537:
#line 4798 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtSyncer * statement = new AosJqlStmtSyncer;
		statement->setName((yystack_[1].value.AosExprValue));
		gAosJQLParser.appendStatement(statement);
		(yylhs.value.AosJqlStmtSyncerValue) = statement;
		(yylhs.value.AosJqlStmtSyncerValue)->setOp(JQLTypes::eDrop);
	}
#line 6040 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 538:
#line 4809 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtTable* statement = new AosJqlStmtTable;
		AosJqlTableOption *opt = (yystack_[2].value.AosJqlTableOptionValue);
		AosJqlCreateDefineGroup *defineGroup = (yystack_[4].value.AosJqlCreateDefineGroupValue);
		
		statement->setName((yystack_[6].value.strval));
		statement->setExists((yystack_[7].value.bool_val));
		if (opt) statement->setOption(opt);

		/*add indexes and columns to the table*/
		if (defineGroup) 
		{
			/*statement->setIndexes(defineGroup->indexes);*/
			statement->setColumns(defineGroup->columns);
		}

		gAosJQLParser.appendStatement(statement);
		delete opt;
		delete defineGroup;
		statement->setSchemaName((yystack_[1].value.strval));

		(yylhs.value.AosJqlStmtTableValue) = statement;
		(yylhs.value.AosJqlStmtTableValue)->setOp(JQLTypes::eCreate);
		//cout << "Create Table:" << " name " << $4 << endl;
	}
#line 6070 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 539:
#line 4836 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtTable* statement = new AosJqlStmtTable;
		statement->setName((yystack_[3].value.strval));
		AosJqlTableOption *opt = (yystack_[2].value.AosJqlTableOptionValue); 
		if (opt) statement->setOption(opt);
		gAosJQLParser.appendStatement(statement);
		statement->setSchemaName((yystack_[1].value.strval));
		(yylhs.value.AosJqlStmtTableValue) = statement;
		(yylhs.value.AosJqlStmtTableValue)->setOp(JQLTypes::eCreate);
	}
#line 6085 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 540:
#line 4848 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtTable* statement = new AosJqlStmtTable;

		statement->setName((yystack_[3].value.strval));
		statement->setLikeTable((yystack_[1].value.strval));
		gAosJQLParser.appendStatement(statement);

		(yylhs.value.AosJqlStmtTableValue) = statement;
		(yylhs.value.AosJqlStmtTableValue)->setOp(JQLTypes::eCreate);
		//cout << "Create Table Statement like found: " << $3 << " from " << $5 << endl;
	}
#line 6101 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 541:
#line 4862 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtTable* statement = new AosJqlStmtTable;
		statement->setName((yystack_[6].value.strval));
		statement->setVirtual(true);
		statement->setSchemaName((yystack_[5].value.strval));
		statement->mVirtualColumns = (yystack_[2].value.AosJqlTableVirtualFieldDefListValue);
		(yylhs.value.AosJqlStmtTableValue) = statement;
		(yylhs.value.AosJqlStmtTableValue)->setOp(JQLTypes::eCreate);
		gAosJQLParser.appendStatement(statement);
	}
#line 6116 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 542:
#line 4876 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlTableVirtualFieldDefListValue) = new vector<AosJqlTableVirtulFieldDefPtr>;
		AosJqlTableVirtulFieldDefPtr vv = OmnNew AosJqlTableVirtulFieldDef();
		vv->mName = (yystack_[5].value.strval);
		vv->mValues = (yystack_[3].value.AosExprListValue);
		vv->mType= (yystack_[0].value.strval);
		(yylhs.value.AosJqlTableVirtualFieldDefListValue)->push_back(vv);
	}
#line 6129 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 543:
#line 4886 "Parser.yy" // lalr1.cc:847
    {
		AosJqlTableVirtulFieldDefPtr vv = OmnNew AosJqlTableVirtulFieldDef();
		vv->mName = (yystack_[5].value.strval);
		vv->mValues = (yystack_[3].value.AosExprListValue);
		vv->mType = (yystack_[0].value.strval);
		(yylhs.value.AosJqlTableVirtualFieldDefListValue)->push_back(vv);
	}
#line 6141 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 544:
#line 4897 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = 0;
	}
#line 6149 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 545:
#line 4902 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = (yystack_[0].value.strval);
	}
#line 6157 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 546:
#line 4908 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = (yystack_[0].value.strval);
	}
#line 6165 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 547:
#line 4913 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDatabase* db1 = new AosJqlStmtDatabase;
		db1->setName((yystack_[2].value.strval));
		db1->setOp(JQLTypes::eCreate);
		gAosJQLParser.appendStatement(db1);
		AosJqlStmtDatabase* db2= new AosJqlStmtDatabase;
		db2->setName((yystack_[2].value.strval));
		db2->setOp(JQLTypes::eUse); 
		gAosJQLParser.appendStatement(db2);
		(yylhs.value.strval) = (yystack_[0].value.strval);
	}
#line 6181 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 548:
#line 4926 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = (yystack_[0].value.strval);
	}
#line 6189 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 549:
#line 4931 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtDatabase* db1 = new AosJqlStmtDatabase;
		db1->setName((yystack_[2].value.strval));
		db1->setOp(JQLTypes::eCreate);
		gAosJQLParser.appendStatement(db1);
		AosJqlStmtDatabase* db2= new AosJqlStmtDatabase;
		db2->setName((yystack_[2].value.strval));
		db2->setOp(JQLTypes::eUse); 
		gAosJQLParser.appendStatement(db2);
		(yylhs.value.strval) = (yystack_[0].value.strval);
	}
#line 6205 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 550:
#line 4946 "Parser.yy" // lalr1.cc:847
    {
		AosJqlCreateDefineGroup* defineGroup = new AosJqlCreateDefineGroup;
		defineGroup->columns = new vector<AosJqlColumnPtr>;
		/*defineGroup->indexes = new vector<AosJqlIndex*>;*/

		(yylhs.value.AosJqlCreateDefineGroupValue) = defineGroup;
		//cout << "Init table definition group" << endl;

		(yylhs.value.AosJqlCreateDefineGroupValue)->columns->push_back((yystack_[0].value.AosJqlColumnValue));
		//cout << "Add a column definition to table definition group" << endl;
	}
#line 6221 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 551:
#line 4959 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlCreateDefineGroupValue) = (yystack_[2].value.AosJqlCreateDefineGroupValue);

		(yylhs.value.AosJqlCreateDefineGroupValue)->columns->push_back((yystack_[0].value.AosJqlColumnValue));
		//cout << "Add a column definition to table definition group" << endl;
	}
#line 6232 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 552:
#line 4968 "Parser.yy" // lalr1.cc:847
    {
		AosJqlColumn* column = new AosJqlColumn;

		column->name = (yystack_[1].value.strval);
		column->type = (yystack_[0].value.AosJqlColumnValue)->type;
		column->size = (yystack_[0].value.AosJqlColumnValue)->size;
		column->format = (yystack_[0].value.AosJqlColumnValue)->format;
		column->defaultVal = NULL;
		column->nullable = true; //default permits null value

		(yylhs.value.AosJqlColumnValue) = column;
		delete (yystack_[0].value.AosJqlColumnValue);
		//cout << "Create Table column found:" << $1 << endl;
	}
#line 6251 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 553:
#line 4984 "Parser.yy" // lalr1.cc:847
    {
		AosJqlColumn* column = (yystack_[2].value.AosJqlColumnValue);
		AosValueRslt value;

		(yystack_[0].value.AosExprValue)->getValue(0, 0, value);
		OmnString str = value.getStr();

		column->defaultVal = str;

		(yylhs.value.AosJqlColumnValue) = column;
		//cout << "Set column default: " << str << endl;
	}
#line 6268 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 554:
#line 4998 "Parser.yy" // lalr1.cc:847
    {
		AosJqlColumn* column = (yystack_[1].value.AosJqlColumnValue);
		AosValueRslt value;

		column->nullable = true;

		(yylhs.value.AosJqlColumnValue) = column;
		//cout << "Set column default: " << str << endl;
	}
#line 6282 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 555:
#line 5009 "Parser.yy" // lalr1.cc:847
    {
		AosJqlColumn* column = (yystack_[2].value.AosJqlColumnValue);
		AosValueRslt value;

		column->nullable = false;

		(yylhs.value.AosJqlColumnValue) = column;
		//cout << "Set column default: " << str << endl;
	}
#line 6296 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 556:
#line 5020 "Parser.yy" // lalr1.cc:847
    {
		AosJqlColumn* column = (yystack_[2].value.AosJqlColumnValue);
		AosValueRslt value;

		column->comment = (yystack_[0].value.strval);

		(yylhs.value.AosJqlColumnValue) = column;
		//cout << "Set column comments: " << $$->comment << endl;
	}
#line 6310 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 557:
#line 5032 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlColumnValue) = new AosJqlColumn;
		(yylhs.value.AosJqlColumnValue)->type = AosDataType::eInt32;
		(yylhs.value.AosJqlColumnValue)->size = AosDataType::getValueSize((yylhs.value.AosJqlColumnValue)->type); 
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	}
#line 6321 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 558:
#line 5040 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlColumnValue) = new AosJqlColumn;
		(yylhs.value.AosJqlColumnValue)->type = AosDataType::eInt32;
		(yylhs.value.AosJqlColumnValue)->size = AosDataType::getValueSize((yylhs.value.AosJqlColumnValue)->type); 
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	}
#line 6332 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 559:
#line 5048 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlColumnValue) = new AosJqlColumn;
		(yylhs.value.AosJqlColumnValue)->type = AosDataType::eInt32;
		(yylhs.value.AosJqlColumnValue)->size = AosDataType::getValueSize((yylhs.value.AosJqlColumnValue)->type); 
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	}
#line 6343 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 560:
#line 5056 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlColumnValue) = new AosJqlColumn;
		(yylhs.value.AosJqlColumnValue)->type = AosDataType::eInt32;
		(yylhs.value.AosJqlColumnValue)->size = AosDataType::getValueSize((yylhs.value.AosJqlColumnValue)->type); 
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	}
#line 6354 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 561:
#line 5064 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlColumnValue) = new AosJqlColumn;
		(yylhs.value.AosJqlColumnValue)->type = AosDataType::eInt32;
		(yylhs.value.AosJqlColumnValue)->size = AosDataType::getValueSize((yylhs.value.AosJqlColumnValue)->type); 
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	}
#line 6365 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 562:
#line 5072 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlColumnValue) = new AosJqlColumn;
		(yylhs.value.AosJqlColumnValue)->type = AosDataType::eNumber;
		(yylhs.value.AosJqlColumnValue)->size = AosDataType::getValueSize((yylhs.value.AosJqlColumnValue)->type); 
		(yylhs.value.AosJqlColumnValue)->v1 = (yystack_[3].value.ll_value);
		(yylhs.value.AosJqlColumnValue)->v2 = (yystack_[1].value.ll_value);
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	}
#line 6378 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 563:
#line 5082 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlColumnValue) = new AosJqlColumn;
		(yylhs.value.AosJqlColumnValue)->type = AosDataType::eNumber;
		(yylhs.value.AosJqlColumnValue)->size = AosDataType::getValueSize((yylhs.value.AosJqlColumnValue)->type); 
		(yylhs.value.AosJqlColumnValue)->v1 = (yystack_[1].value.ll_value);
		(yylhs.value.AosJqlColumnValue)->v2 = 0;
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	}
#line 6391 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 564:
#line 5092 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlColumnValue) = new AosJqlColumn;
		(yylhs.value.AosJqlColumnValue)->type = AosDataType::eInt64;
		(yylhs.value.AosJqlColumnValue)->size = AosDataType::getValueSize((yylhs.value.AosJqlColumnValue)->type); 
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	}
#line 6402 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 565:
#line 5101 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlColumnValue) = new AosJqlColumn;
		(yylhs.value.AosJqlColumnValue)->type = AosDataType::eInt64;
		(yylhs.value.AosJqlColumnValue)->size = (yystack_[1].value.ll_value);
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	}
#line 6413 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 566:
#line 5109 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlColumnValue) = new AosJqlColumn;
		(yylhs.value.AosJqlColumnValue)->type = AosDataType::eU32;
		(yylhs.value.AosJqlColumnValue)->size = AosDataType::getValueSize((yylhs.value.AosJqlColumnValue)->type); 
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	}
#line 6424 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 567:
#line 5117 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlColumnValue) = new AosJqlColumn;
		(yylhs.value.AosJqlColumnValue)->type = AosDataType::eU32;
		(yylhs.value.AosJqlColumnValue)->size = AosDataType::getValueSize((yylhs.value.AosJqlColumnValue)->type); 
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	}
#line 6435 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 568:
#line 5125 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlColumnValue) = new AosJqlColumn;
		(yylhs.value.AosJqlColumnValue)->type = AosDataType::eU32;
		(yylhs.value.AosJqlColumnValue)->size = AosDataType::getValueSize((yylhs.value.AosJqlColumnValue)->type); 
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	}
#line 6446 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 569:
#line 5133 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlColumnValue) = new AosJqlColumn;
		(yylhs.value.AosJqlColumnValue)->type = AosDataType::eU64;
		(yylhs.value.AosJqlColumnValue)->size = AosDataType::getValueSize((yylhs.value.AosJqlColumnValue)->type); 
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	}
#line 6457 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 570:
#line 5141 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlColumnValue) = new AosJqlColumn;
	    (yylhs.value.AosJqlColumnValue)->type = AosDataType::eDouble;
		(yylhs.value.AosJqlColumnValue)->size = AosDataType::getValueSize((yylhs.value.AosJqlColumnValue)->type); 
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	}
#line 6468 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 571:
#line 5149 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlColumnValue) = new AosJqlColumn;
	    (yylhs.value.AosJqlColumnValue)->type = AosDataType::eDouble;
		(yylhs.value.AosJqlColumnValue)->size = AosDataType::getValueSize((yylhs.value.AosJqlColumnValue)->type); 
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	}
#line 6479 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 572:
#line 5157 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlColumnValue) = new AosJqlColumn;
	    (yylhs.value.AosJqlColumnValue)->type = AosDataType::eDate;
		(yylhs.value.AosJqlColumnValue)->size = AosDataType::getValueSize((yylhs.value.AosJqlColumnValue)->type); 
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	}
#line 6490 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 573:
#line 5165 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlColumnValue) = new AosJqlColumn;
	    (yylhs.value.AosJqlColumnValue)->type = AosDataType::eChar;
		(yylhs.value.AosJqlColumnValue)->size = 1;
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	}
#line 6501 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 574:
#line 5173 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlColumnValue) = new AosJqlColumn;
	    (yylhs.value.AosJqlColumnValue)->type = AosDataType::eChar;
		(yylhs.value.AosJqlColumnValue)->size = (yystack_[1].value.ll_value);
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	}
#line 6512 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 575:
#line 5181 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlColumnValue) = new AosJqlColumn;
	    (yylhs.value.AosJqlColumnValue)->type = AosDataType::eChar;
		(yylhs.value.AosJqlColumnValue)->size = (yystack_[2].value.ll_value);
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	}
#line 6523 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 576:
#line 5189 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlColumnValue) = new AosJqlColumn;
	    (yylhs.value.AosJqlColumnValue)->type = AosDataType::eCharStr;
		(yylhs.value.AosJqlColumnValue)->size = (yystack_[1].value.ll_value);
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	}
#line 6534 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 577:
#line 5197 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlColumnValue) = new AosJqlColumn;
	    (yylhs.value.AosJqlColumnValue)->type = AosDataType::eCharStr;
		(yylhs.value.AosJqlColumnValue)->size = (yystack_[2].value.ll_value);
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	}
#line 6545 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 578:
#line 5205 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlColumnValue) = new AosJqlColumn;
	    (yylhs.value.AosJqlColumnValue)->type = AosDataType::eCharStr;
		(yylhs.value.AosJqlColumnValue)->size = (yystack_[1].value.ll_value);
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	}
#line 6556 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 579:
#line 5213 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlColumnValue) = new AosJqlColumn;
	    (yylhs.value.AosJqlColumnValue)->type = AosDataType::eCharStr;
		(yylhs.value.AosJqlColumnValue)->size = (yystack_[2].value.ll_value);
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	}
#line 6567 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 580:
#line 5221 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlColumnValue) = new AosJqlColumn;
	    (yylhs.value.AosJqlColumnValue)->type = AosDataType::eCharStr;
		(yylhs.value.AosJqlColumnValue)->size = 1024;
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	}
#line 6578 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 581:
#line 5229 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlColumnValue) = new AosJqlColumn;
	    (yylhs.value.AosJqlColumnValue)->type = AosDataType::eBuff;
		(yylhs.value.AosJqlColumnValue)->size = 1024;
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	}
#line 6589 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 582:
#line 5237 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlColumnValue) = new AosJqlColumn;
	    (yylhs.value.AosJqlColumnValue)->type = AosDataType::eDate;
		string tmp = (yystack_[1].value.strval);
	    (yylhs.value.AosJqlColumnValue)->format = (yystack_[1].value.strval);
		(yylhs.value.AosJqlColumnValue)->size = DATE_SIZE;
	}
#line 6601 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 583:
#line 5246 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlColumnValue) = new AosJqlColumn;
	    (yylhs.value.AosJqlColumnValue)->type = AosDataType::eBuff;
		(yylhs.value.AosJqlColumnValue)->size = (yystack_[1].value.ll_value);
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	}
#line 6612 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 584:
#line 5255 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlTableOptionValue) = new AosJqlTableOption;
		//cout << "Init a new table option struct" <<  endl;
	}
#line 6621 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 585:
#line 5261 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlTableOptionValue) = (yystack_[2].value.AosJqlTableOptionValue);
		(yylhs.value.AosJqlTableOptionValue)->comment = (yystack_[0].value.strval);
		//delete [] $2;
		//cout << "Table comment" << $$->comment <<  endl;
	}
#line 6632 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 586:
#line 5269 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlTableOptionValue) = (yystack_[2].value.AosJqlTableOptionValue);
		(yylhs.value.AosJqlTableOptionValue)->checksum = 0;
		//delete [] $2;
		//cout << "Table checksum" << $$->checksum <<  endl;
	}
#line 6643 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 587:
#line 5277 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlTableOptionValue) = (yystack_[2].value.AosJqlTableOptionValue);
		(yylhs.value.AosJqlTableOptionValue)->checksum = 1;
		//delete [] $2;
		//cout << "Table checksum" << $$->checksum <<  endl;
	}
#line 6654 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 588:
#line 5285 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlTableOptionValue) = (yystack_[2].value.AosJqlTableOptionValue);
		(yylhs.value.AosJqlTableOptionValue)->autoInc = (yystack_[0].value.u32_value);
		//delete [] $2;
		//cout << "Table auto increment" << $$->autoInc <<  endl;
	}
#line 6665 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 589:
#line 5293 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlTableOptionValue) = (yystack_[3].value.AosJqlTableOptionValue);
		(yylhs.value.AosJqlTableOptionValue)->inputDataFormat = "fixed";
	}
#line 6674 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 590:
#line 5299 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlTableOptionValue) = (yystack_[3].value.AosJqlTableOptionValue);
		(yylhs.value.AosJqlTableOptionValue)->inputDataFormat = "csv";
	}
#line 6683 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 591:
#line 5305 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlTableOptionValue) = (yystack_[15].value.AosJqlTableOptionValue);
		(yylhs.value.AosJqlTableOptionValue)->data_source = "hbase";
		(yylhs.value.AosJqlTableOptionValue)->db_name = (yystack_[10].value.strval), 
		(yylhs.value.AosJqlTableOptionValue)->db_addr = (yystack_[8].value.strval), 
		(yylhs.value.AosJqlTableOptionValue)->db_port = (yystack_[6].value.ll_value), 
		(yylhs.value.AosJqlTableOptionValue)->db_t_name = (yystack_[4].value.strval), 
		(yylhs.value.AosJqlTableOptionValue)->raw_keys = (yystack_[1].value.AosExprListValue);
	}
#line 6697 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 592:
#line 5318 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtTable* statement = new AosJqlStmtTable;
		statement->setName((yystack_[1].value.strval));
		statement->setExists((yystack_[2].value.bool_val));
		gAosJQLParser.appendStatement(statement);
		(yylhs.value.AosJqlStmtTableValue) = statement;
		(yylhs.value.AosJqlStmtTableValue)->setOp(JQLTypes::eDrop); 
	}
#line 6710 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 593:
#line 5330 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtTable* statement = new AosJqlStmtTable;
		statement->setFullOption((yystack_[4].value.u32_value));
		statement->setFromOption((yystack_[2].value.strval));
		statement->setLikeOption((yystack_[1].value.strval));

		gAosJQLParser.appendStatement(statement);
		(yylhs.value.AosJqlStmtTableValue) = statement;
		(yylhs.value.AosJqlStmtTableValue)->setOp(JQLTypes::eShow); 
	}
#line 6725 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 594:
#line 5343 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.u32_value) = 0;
	}
#line 6733 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 595:
#line 5348 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.u32_value) = 1;
	}
#line 6741 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 596:
#line 5354 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = NULL;
	}
#line 6749 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 597:
#line 5359 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = (yystack_[0].value.strval);
	}
#line 6757 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 598:
#line 5365 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = NULL;
	}
#line 6765 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 599:
#line 5370 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = (yystack_[0].value.strval);
	}
#line 6773 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 600:
#line 5377 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.bool_val) = false;
	}
#line 6781 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 601:
#line 5382 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.bool_val) = true;
	}
#line 6789 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 602:
#line 5388 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.bool_val) = false;
	}
#line 6797 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 603:
#line 5393 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.bool_val) = true;
	}
#line 6805 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 604:
#line 5400 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtTable* statement = new AosJqlStmtTable;
		statement->setName((yystack_[1].value.strval));
		gAosJQLParser.appendStatement(statement);
		(yylhs.value.AosJqlStmtTableValue) = statement;
		(yylhs.value.AosJqlStmtTableValue)->setOp(JQLTypes::eDescribe); 
	}
#line 6817 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 605:
#line 5410 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtTask *stmt = new AosJqlStmtTask;
		stmt->setTaskName((yystack_[4].value.strval));
		stmt->setConfParms((yystack_[2].value.AosExprNameValuesValue));
		gAosJQLParser.appendStatement(stmt);
		(yylhs.value.AosJqlStmtTaskValue) = stmt;
		(yylhs.value.AosJqlStmtTaskValue)->setOp(JQLTypes::eCreate);
	}
#line 6830 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 606:
#line 5424 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtTask *task = new AosJqlStmtTask;
		task->setTaskName((yystack_[30].value.strval));
		task->setInputDataSetNames((yystack_[26].value.AosExprListValue));
		task->setOutputDataSetNames((yystack_[21].value.AosExprListValue));
		task->setDataProcsNames((yystack_[11].value.AosExprListValue));
		task->setActionNames((yystack_[2].value.AosExprListValue));
		gAosJQLParser.appendStatement(task);   
		(yylhs.value.AosJqlStmtTaskValue) = task;                             
		(yylhs.value.AosJqlStmtTaskValue)->setOp(JQLTypes::eCreate);               
	}
#line 6846 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 607:
#line 5439 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtTask *task = new AosJqlStmtTask;
		task->setTaskName((yystack_[1].value.strval));
		gAosJQLParser.appendStatement(task);
		(yylhs.value.AosJqlStmtTaskValue) = task;
		(yylhs.value.AosJqlStmtTaskValue)->setOp(JQLTypes::eDrop);
	}
#line 6858 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 608:
#line 5450 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtTask *task = new AosJqlStmtTask;
		task->setOp(JQLTypes::eShow);
		gAosJQLParser.appendStatement(task);
		(yylhs.value.AosJqlStmtTaskValue) = task;
	}
#line 6869 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 609:
#line 5460 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtTask *task = new AosJqlStmtTask;
		task->setTaskName((yystack_[1].value.strval));
		gAosJQLParser.appendStatement(task);
		(yylhs.value.AosJqlStmtTaskValue) = task;
		(yylhs.value.AosJqlStmtTaskValue)->setOp(JQLTypes::eDescribe);
	}
#line 6881 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 610:
#line 5471 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJqlStmtQueryUnionValue) = (yystack_[1].value.AosJqlStmtQueryUnionValue);	
		gAosJQLParser.appendStatement((yystack_[1].value.AosJqlStmtQueryUnionValue));             
	}
#line 6890 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 611:
#line 5479 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtQueryUnion *stmt_union = new AosJqlStmtQueryUnion();
		AosJqlStmtQuery *query1 = new AosJqlStmtQuery((yystack_[2].value.AosJqlSelectValue));
		AosJqlStmtQuery *query2 = new AosJqlStmtQuery((yystack_[0].value.AosJqlSelectValue));

		stmt_union->setIsUnionAll((yystack_[1].value.bool_val));
		stmt_union->addQuery(query1);
		stmt_union->addQuery(query2);

		(yylhs.value.AosJqlStmtQueryUnionValue) = stmt_union;   
	}
#line 6906 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 612:
#line 5492 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtQueryUnion *stmt_union = (yystack_[2].value.AosJqlStmtQueryUnionValue);
		AosJqlStmtQuery *query = new AosJqlStmtQuery((yystack_[0].value.AosJqlSelectValue));

		stmt_union->addQuery(query);
		(yylhs.value.AosJqlStmtQueryUnionValue) = stmt_union;   
	}
#line 6918 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 613:
#line 5503 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.bool_val) = false;
	}
#line 6926 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 614:
#line 5508 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.bool_val) = true;
	}
#line 6934 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 615:
#line 5518 "Parser.yy" // lalr1.cc:847
    {
		 AosJqlStmtUpdate *u1 = new AosJqlStmtUpdate;
		 u1->table = (yystack_[4].value.AosExprValue);
		 u1->update_expr_list = (yystack_[2].value.AosExprListValue);
		 if ((yystack_[1].value.AosJqlWhereValue)) u1->opt_where = (yystack_[1].value.AosJqlWhereValue)->getWhereExpr().getPtr();

		 AosJqlStmtUpdateItem * update = new AosJqlStmtUpdateItem(u1);
		 gAosJQLParser.appendStatement(update);
		 (yylhs.value.AosJqlStmtUpdateItemValue) = update;

		 //cout << "found UPDATA table_references SET update_asgn_list " << endl;
	}
#line 6951 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 616:
#line 5534 "Parser.yy" // lalr1.cc:847
    { 
		 if ((yystack_[1].value.subtok) != AosExprComparison::eEqual) yyerror("bad update assignment to %s", (yystack_[1].value.subtok));

		 (yylhs.value.AosExprListValue) = new AosExprList;

		 AosExprObj *pExpr1 = new AosExprString((yystack_[2].value.strval));
		 AosExprObj *pExpr = (yystack_[0].value.AosExprValue); 
		 AosExpr	*pExpr2 = new AosExprComparison(pExpr1, AosExprComparison::eEqual, pExpr);    
		 pExpr2->setExpr(pExpr1, 1);
		 pExpr2->setExpr(pExpr, 2);

		 free((yystack_[2].value.strval));

		 (yylhs.value.AosExprListValue)->push_back(pExpr2);
	}
#line 6971 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 617:
#line 5558 "Parser.yy" // lalr1.cc:847
    { 
		 if ((yystack_[1].value.subtok) != AosExprComparison::eEqual) yyerror("bad update assignment to %s", (yystack_[1].value.subtok));

		 AosExprObj *pExpr1 = new AosExprString((yystack_[2].value.strval));
		 AosExprObj *pExpr = (yystack_[0].value.AosExprValue); 
		 AosExpr	*pExpr2 = new AosExprComparison(pExpr1, AosExprComparison::eEqual, pExpr);    
		 pExpr2->setExpr(pExpr1, 1);
		 pExpr2->setExpr(pExpr, 2);
		 free((yystack_[2].value.strval));
		 (yylhs.value.AosExprListValue)->push_back(pExpr2);
	}
#line 6987 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 618:
#line 5580 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtUserMgr *user_mgr = new AosJqlStmtUserMgr;
		user_mgr->setUserName((yystack_[4].value.strval));
		user_mgr->setPwd((yystack_[1].value.strval));
		gAosJQLParser.appendStatement(user_mgr);
		(yylhs.value.AosJqlStmtUserMgrValue) = user_mgr;
		(yylhs.value.AosJqlStmtUserMgrValue)->setOp(JQLTypes::eCreate);
	}
#line 7000 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 619:
#line 5593 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtUserMgr *user_mgr = new AosJqlStmtUserMgr;
		user_mgr->setUserName((yystack_[1].value.strval));
		gAosJQLParser.appendStatement(user_mgr);
		(yylhs.value.AosJqlStmtUserMgrValue) = user_mgr;
		(yylhs.value.AosJqlStmtUserMgrValue)->setOp(JQLTypes::eDrop);
	}
#line 7012 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 620:
#line 5604 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtUserMgr *user_mgr = new AosJqlStmtUserMgr;
		gAosJQLParser.appendStatement(user_mgr);
		(yylhs.value.AosJqlStmtUserMgrValue) = user_mgr;
		(yylhs.value.AosJqlStmtUserMgrValue)->setOp(JQLTypes::eShow);
	}
#line 7023 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 621:
#line 5614 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtUserMgr *user_mgr = new AosJqlStmtUserMgr;
		user_mgr->setUserName((yystack_[1].value.strval));
		gAosJQLParser.appendStatement(user_mgr);
		(yylhs.value.AosJqlStmtUserMgrValue) = user_mgr;
		(yylhs.value.AosJqlStmtUserMgrValue)->setOp(JQLTypes::eDescribe);
	}
#line 7035 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 622:
#line 5625 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtUserMgr *user_mgr = new AosJqlStmtUserMgr;
		user_mgr->setUserName((yystack_[4].value.strval));
		user_mgr->setPwd((yystack_[1].value.strval));
		gAosJQLParser.appendStatement(user_mgr);
		(yylhs.value.AosJqlStmtUserMgrValue) = user_mgr;
		(yylhs.value.AosJqlStmtUserMgrValue)->setOp(JQLTypes::eAlter);	
	}
#line 7048 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 623:
#line 5645 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtVirtualField *virtual_field = new AosJqlStmtVirtualField;
		virtual_field->setName((yystack_[16].value.strval));
		virtual_field->setIsInfoField((yystack_[15].value.bool_val));
		virtual_field->setType((yystack_[13].value.strval));
        virtual_field->setDataType((yystack_[12].value.AosJQLDataFieldTypeInfoPtr));
		virtual_field->setMaxLen((yystack_[11].value.ll_value));
		virtual_field->setOffset((yystack_[10].value.ll_value));
		virtual_field->setDefault((yystack_[9].value.strval));
		virtual_field->setTable((yystack_[7].value.strval));
		virtual_field->setMapName((yystack_[4].value.strval));
		virtual_field->setFields((yystack_[2].value.AosExprListValue));
		gAosJQLParser.appendStatement(virtual_field);
		(yylhs.value.AosJqlStmtVirtualFieldValue) = virtual_field;
		(yylhs.value.AosJqlStmtVirtualFieldValue)->setOp(JQLTypes::eCreate);
	}
#line 7069 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 624:
#line 5671 "Parser.yy" // lalr1.cc:847
    {
        AosJqlStmtFieldExpr *stmt = new AosJqlStmtFieldExpr;
        stmt->setName((yystack_[10].value.strval));
        stmt->setType((yystack_[8].value.strval));
        stmt->setDataType((yystack_[7].value.AosJQLDataFieldTypeInfoPtr));
        stmt->setMaxLen((yystack_[6].value.ll_value));
        stmt->setOffset((yystack_[5].value.ll_value));
        stmt->setExpr((yystack_[3].value.AosExprValue));
        stmt->setTable((yystack_[1].value.strval));
        gAosJQLParser.appendStatement(stmt);
        (yylhs.value.AosJqlStmtFieldExprValue) = stmt;
        (yylhs.value.AosJqlStmtFieldExprValue)->setOp(JQLTypes::eCreate);
    }
#line 7087 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 625:
#line 5688 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtVirtualField *virtual_field = new AosJqlStmtVirtualField;
		virtual_field->setName((yystack_[1].value.strval));
		gAosJQLParser.appendStatement(virtual_field);
		(yylhs.value.AosJqlStmtVirtualFieldValue) = virtual_field;
		(yylhs.value.AosJqlStmtVirtualFieldValue)->setOp(JQLTypes::eDrop);
	}
#line 7099 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 626:
#line 5699 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtVirtualField *virtual_field = new AosJqlStmtVirtualField;
		gAosJQLParser.appendStatement(virtual_field);
		(yylhs.value.AosJqlStmtVirtualFieldValue) = virtual_field;
		(yylhs.value.AosJqlStmtVirtualFieldValue)->setOp(JQLTypes::eShow);
	}
#line 7110 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 627:
#line 5709 "Parser.yy" // lalr1.cc:847
    {
		AosJqlStmtVirtualField *virtual_field = new AosJqlStmtVirtualField;
		virtual_field->setName((yystack_[1].value.strval));
		gAosJQLParser.appendStatement(virtual_field);
		(yylhs.value.AosJqlStmtVirtualFieldValue) = virtual_field;
		(yylhs.value.AosJqlStmtVirtualFieldValue)->setOp(JQLTypes::eDescribe);
	}
#line 7122 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 628:
#line 5719 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJQLDataFieldTypeInfoPtr) = NULL;
	}
#line 7130 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 629:
#line 5724 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.AosJQLDataFieldTypeInfoPtr) = (yystack_[0].value.AosJQLDataFieldTypeInfoPtr);
	}
#line 7138 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 630:
#line 5730 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.bool_val) = false;
	}
#line 7146 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 631:
#line 5735 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.bool_val) = true;
	}
#line 7154 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 632:
#line 5740 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup((yystack_[0].value.strval));
	}
#line 7162 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 633:
#line 5745 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7170 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 634:
#line 5750 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7178 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 635:
#line 5755 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7186 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 636:
#line 5760 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7194 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 637:
#line 5765 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7202 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 638:
#line 5770 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7210 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 639:
#line 5775 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7218 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 640:
#line 5780 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7226 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 641:
#line 5785 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7234 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 642:
#line 5790 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7242 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 643:
#line 5795 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7250 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 644:
#line 5800 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7258 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 645:
#line 5805 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7266 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 646:
#line 5810 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7274 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 647:
#line 5815 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7282 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 648:
#line 5820 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7290 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 649:
#line 5825 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7298 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 650:
#line 5830 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7306 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 651:
#line 5835 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7314 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 652:
#line 5840 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7322 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 653:
#line 5845 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7330 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 654:
#line 5850 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7338 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 655:
#line 5855 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7346 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 656:
#line 5860 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7354 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 657:
#line 5865 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7362 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 658:
#line 5870 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7370 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 659:
#line 5875 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7378 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 660:
#line 5880 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7386 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 661:
#line 5885 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7394 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 662:
#line 5890 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7402 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 663:
#line 5895 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7410 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 664:
#line 5900 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7418 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 665:
#line 5905 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7426 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 666:
#line 5910 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7434 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 667:
#line 5915 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7442 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 668:
#line 5920 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7450 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 669:
#line 5925 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7458 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 670:
#line 5930 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7466 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 671:
#line 5935 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7474 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 672:
#line 5940 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7482 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 673:
#line 5945 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7490 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 674:
#line 5950 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7498 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 675:
#line 5955 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7506 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 676:
#line 5960 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7514 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 677:
#line 5965 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7522 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 678:
#line 5970 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7530 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 679:
#line 5975 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7538 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 680:
#line 5980 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7546 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 681:
#line 5985 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7554 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 682:
#line 5990 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7562 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 683:
#line 5995 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7570 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 684:
#line 6000 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7578 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 685:
#line 6005 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7586 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 686:
#line 6010 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7594 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 687:
#line 6015 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7602 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 688:
#line 6020 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7610 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 689:
#line 6025 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7618 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 690:
#line 6030 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7626 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 691:
#line 6035 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7634 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 692:
#line 6040 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7642 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 693:
#line 6045 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7650 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 694:
#line 6050 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7658 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 695:
#line 6055 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7666 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 696:
#line 6060 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7674 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 697:
#line 6065 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7682 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 698:
#line 6070 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7690 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 699:
#line 6075 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7698 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 700:
#line 6080 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7706 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 701:
#line 6085 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7714 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 702:
#line 6090 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7722 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 703:
#line 6095 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7730 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 704:
#line 6100 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7738 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 705:
#line 6105 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7746 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 706:
#line 6110 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7754 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 707:
#line 6115 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7762 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 708:
#line 6120 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7770 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 709:
#line 6125 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7778 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 710:
#line 6130 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7786 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 711:
#line 6135 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7794 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 712:
#line 6140 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7802 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 713:
#line 6145 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7810 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 714:
#line 6150 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7818 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 715:
#line 6155 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7826 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 716:
#line 6160 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7834 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 717:
#line 6165 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7842 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 718:
#line 6170 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7850 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 719:
#line 6175 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7858 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 720:
#line 6180 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7866 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 721:
#line 6185 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7874 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 722:
#line 6190 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7882 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 723:
#line 6195 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7890 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 724:
#line 6200 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7898 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 725:
#line 6205 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7906 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 726:
#line 6210 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7914 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 727:
#line 6215 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7922 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 728:
#line 6220 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7930 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 729:
#line 6225 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7938 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 730:
#line 6230 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7946 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 731:
#line 6235 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7954 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 732:
#line 6240 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7962 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 733:
#line 6245 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7970 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 734:
#line 6250 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7978 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 735:
#line 6255 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7986 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 736:
#line 6260 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 7994 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 737:
#line 6265 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8002 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 738:
#line 6270 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8010 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 739:
#line 6275 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8018 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 740:
#line 6280 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8026 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 741:
#line 6285 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8034 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 742:
#line 6290 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8042 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 743:
#line 6295 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8050 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 744:
#line 6300 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8058 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 745:
#line 6305 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8066 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 746:
#line 6310 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8074 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 747:
#line 6315 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8082 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 748:
#line 6320 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8090 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 749:
#line 6325 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8098 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 750:
#line 6330 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8106 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 751:
#line 6335 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8114 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 752:
#line 6340 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8122 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 753:
#line 6345 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8130 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 754:
#line 6350 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8138 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 755:
#line 6355 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8146 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 756:
#line 6360 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8154 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 757:
#line 6365 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8162 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 758:
#line 6370 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8170 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 759:
#line 6375 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8178 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 760:
#line 6380 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8186 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 761:
#line 6385 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8194 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 762:
#line 6390 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8202 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 763:
#line 6395 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8210 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 764:
#line 6400 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8218 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 765:
#line 6405 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8226 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 766:
#line 6410 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8234 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 767:
#line 6415 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8242 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 768:
#line 6420 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8250 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 769:
#line 6425 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8258 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 770:
#line 6430 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8266 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 771:
#line 6435 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8274 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 772:
#line 6440 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8282 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 773:
#line 6445 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8290 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 774:
#line 6450 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8298 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 775:
#line 6455 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8306 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 776:
#line 6460 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8314 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 777:
#line 6465 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8322 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 778:
#line 6470 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8330 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 779:
#line 6475 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8338 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 780:
#line 6480 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8346 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 781:
#line 6485 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8354 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 782:
#line 6490 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8362 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 783:
#line 6495 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8370 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 784:
#line 6500 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8378 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 785:
#line 6505 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8386 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 786:
#line 6510 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8394 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 787:
#line 6515 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8402 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 788:
#line 6520 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8410 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 789:
#line 6525 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8418 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 790:
#line 6530 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8426 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 791:
#line 6535 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8434 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 792:
#line 6540 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8442 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 793:
#line 6545 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8450 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 794:
#line 6550 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8458 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 795:
#line 6555 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8466 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 796:
#line 6560 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8474 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 797:
#line 6565 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8482 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 798:
#line 6570 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8490 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 799:
#line 6575 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8498 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 800:
#line 6580 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8506 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 801:
#line 6585 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8514 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 802:
#line 6590 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8522 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 803:
#line 6595 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8530 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 804:
#line 6600 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8538 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 805:
#line 6605 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8546 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 806:
#line 6610 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8554 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 807:
#line 6615 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8562 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 808:
#line 6620 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8570 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 809:
#line 6625 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8578 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 810:
#line 6630 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8586 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 811:
#line 6635 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8594 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 812:
#line 6640 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8602 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 813:
#line 6645 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8610 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 814:
#line 6650 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8618 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 815:
#line 6655 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8626 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 816:
#line 6660 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8634 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 817:
#line 6665 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8642 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 818:
#line 6670 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8650 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 819:
#line 6675 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8658 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 820:
#line 6680 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8666 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 821:
#line 6685 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8674 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 822:
#line 6690 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8682 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 823:
#line 6695 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8690 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 824:
#line 6700 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8698 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 825:
#line 6705 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8706 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 826:
#line 6710 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8714 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 827:
#line 6715 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8722 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 828:
#line 6720 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8730 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 829:
#line 6725 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8738 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 830:
#line 6730 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8746 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 831:
#line 6735 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8754 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 832:
#line 6740 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8762 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 833:
#line 6745 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8770 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 834:
#line 6750 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8778 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 835:
#line 6755 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8786 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 836:
#line 6760 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8794 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 837:
#line 6765 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8802 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 838:
#line 6770 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8810 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 839:
#line 6775 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8818 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 840:
#line 6780 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8826 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 841:
#line 6785 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8834 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 842:
#line 6790 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8842 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 843:
#line 6795 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8850 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 844:
#line 6800 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8858 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 845:
#line 6805 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8866 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 846:
#line 6810 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8874 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 847:
#line 6815 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8882 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 848:
#line 6820 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8890 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 849:
#line 6825 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8898 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 850:
#line 6830 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8906 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 851:
#line 6835 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8914 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 852:
#line 6840 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8922 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 853:
#line 6845 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8930 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 854:
#line 6850 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8938 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 855:
#line 6855 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8946 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 856:
#line 6860 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8954 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 857:
#line 6865 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8962 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 858:
#line 6870 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8970 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 859:
#line 6875 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8978 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 860:
#line 6880 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8986 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 861:
#line 6885 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 8994 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 862:
#line 6890 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 9002 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 863:
#line 6895 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 9010 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 864:
#line 6900 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 9018 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 865:
#line 6905 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 9026 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 866:
#line 6910 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 9034 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 867:
#line 6915 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 9042 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 868:
#line 6920 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 9050 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 869:
#line 6925 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 9058 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 870:
#line 6930 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 9066 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 871:
#line 6935 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 9074 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 872:
#line 6940 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 9082 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 873:
#line 6945 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 9090 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 874:
#line 6950 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 9098 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 875:
#line 6955 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 9106 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 876:
#line 6960 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 9114 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 877:
#line 6965 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 9122 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 878:
#line 6970 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 9130 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 879:
#line 6975 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 9138 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 880:
#line 6980 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 9146 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 881:
#line 6985 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 9154 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 882:
#line 6990 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 9162 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 883:
#line 6995 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 9170 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 884:
#line 7000 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 9178 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 885:
#line 7005 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 9186 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 886:
#line 7010 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 9194 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 887:
#line 7015 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 9202 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 888:
#line 7020 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 9210 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 889:
#line 7025 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 9218 "Parser.tab.cc" // lalr1.cc:847
    break;

  case 890:
#line 7030 "Parser.yy" // lalr1.cc:847
    {
		(yylhs.value.strval) = strdup(yytext);
	}
#line 9226 "Parser.tab.cc" // lalr1.cc:847
    break;


#line 9230 "Parser.tab.cc" // lalr1.cc:847
            default:
              break;
            }
        }
      catch (const syntax_error& yyexc)
        {
          error (yyexc);
          YYERROR;
        }
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;
      YY_STACK_PRINT ();

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, yylhs);
    }
    goto yynewstate;

  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        error (yyla.location, yysyntax_error_ (yystack_[0].state,
                                           yyempty ? yyempty_ : yyla.type_get ()));
      }


    yyerror_range[1].location = yyla.location;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.type_get () == yyeof_)
          YYABORT;
        else if (!yyempty)
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyempty = true;
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:

    /* Pacify compilers like GCC when the user code never invokes
       YYERROR and the label yyerrorlab therefore never appears in user
       code.  */
    if (false)
      goto yyerrorlab;
    yyerror_range[1].location = yystack_[yylen - 1].location;
    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    goto yyerrlab1;

  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    {
      stack_symbol_type error_token;
      for (;;)
        {
          yyn = yypact_[yystack_[0].state];
          if (!yy_pact_value_is_default_ (yyn))
            {
              yyn += yyterror_;
              if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == yyterror_)
                {
                  yyn = yytable_[yyn];
                  if (0 < yyn)
                    break;
                }
            }

          // Pop the current state because it cannot handle the error token.
          if (yystack_.size () == 1)
            YYABORT;

          yyerror_range[1].location = yystack_[0].location;
          yy_destroy_ ("Error: popping", yystack_[0]);
          yypop_ ();
          YY_STACK_PRINT ();
        }

      yyerror_range[2].location = yyla.location;
      YYLLOC_DEFAULT (error_token.location, yyerror_range, 2);

      // Shift the error token.
      error_token.state = yyn;
      yypush_ ("Shifting", error_token);
    }
    goto yynewstate;

    // Accept.
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;

    // Abort.
  yyabortlab:
    yyresult = 1;
    goto yyreturn;

  yyreturn:
    if (!yyempty)
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack"
                 << std::endl;
        // Do not try to display the values of the reclaimed symbols,
        // as their printer might throw an exception.
        if (!yyempty)
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
  }

  void
  Parser::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what());
  }

  // Generate an error message.
  std::string
  Parser::yysyntax_error_ (state_type, symbol_number_type) const
  {
    return YY_("syntax error");
  }


  const short int Parser::yypact_ninf_ = -1669;

  const short int Parser::yytable_ninf_ = -818;

  const short int
  Parser::yypact_[] =
  {
    3944, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669,  7859,
    7859, -1669,  7859, -1669,   170, -1669,   321,    22, -1669, -1669,
     330,  5251, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669,
     -16,   -26, -1669,    74, -1669, -1669, -1669, -1669, -1669, -1669,
   -1669,  8185, -1669, 12088,   184, -1669, -1669, -1669, -1669, -1669,
     168, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669,
   -1669, -1669, -1669,   297, -1669, -1669, -1669, -1669, -1669, -1669,
   -1669, -1669, -1669, -1669, -1669, -1669, -1669, 13703, -1669, -1669,
   -1669, -1669, -1669,  1689, -1669, -1669, -1669, -1669, -1669, -1669,
   -1669, -1669, -1669,   131,   140, -1669, -1669, -1669, -1669, -1669,
    5577, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669,
   -1669, -1669, -1669, -1669, -1669, -1669, -1669,   376, -1669, -1669,
   -1669, -1669, -1669, -1669, -1669, -1669, -1669,  1570, -1669, -1669,
   -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669,
   -1669, -1669, -1669, -1669, -1669, -1669,  1654, -1669, -1669, -1669,
   -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669,   237,
   -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669,
   -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669,   171, -1669,
   -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669,
   -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669,
   -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669,
   -1669, -1669, -1669,   400, -1669, -1669, -1669, -1669,   422, -1669,
   -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669,
   -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669,
   -1669, -1669, -1669, -1669,   -32, -1669, -1669, -1669, -1669, -1669,
   -1669,   434, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669,
   -1669,   493, -1669,  7859, -1669, -1669, -1669, -1669, -1669, -1669,
   -1669, -1669, -1669, -1669, -1669, -1669,   271,  7859,  2962, -1669,
   -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669,
   -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669,
   -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669,
   -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669,
   -1669, -1669, -1669, -1669,   514, -1669, -1669, -1669, -1669, -1669,
   -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669,
   -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669,
   -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669,
   -1669, -1669, -1669, -1669, -1669, -1669,    37, -1669, -1669, -1669,
   -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669,
   -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669,
   -1669,  -112, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669,
   -1669, -1669, -1669,    57, -1669, -1669, -1669,   536, -1669, -1669,
   -1669, -1669, -1669, -1669,   248,   248,   247,   517,  7859,    83,
     359, 13703, 13703, 13703,   482,   255,   746, 14026, 13703,   340,
   13703, 13703, 13703,  7859,   469,   563,   524, 14349,   537,   405,
   13703, 13703, 13703, 13703,    -5,   437, 13703, 13703,   561,  4599,
      46,   363, 13703, 13703, 13703, 13703, 13703,   369,   934,  7859,
   11762, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669,   332,
     337,   344,   357,   375, -1669, -1669,   386, -1669, -1669, -1669,
      24, -1669, 13703,  4925,   361, 13703,   391,   398,   404, 14672,
   13703, 13703, 13703,   412, 13703, 13703, 13703, 13703, 13703,  7859,
   13703, 13703, 13703, 13703, 13703, 13703,   553, 13703,  7859,  7859,
     993,   614,   608,   614,   227,   300,   438,   449,   457, -1669,
     478,   485,   -22,   489,   498,   519,   538,   544,   577,   587,
     605,   575,   613,   639,   380, 13703, 14995, 13703, 13703, 13703,
   13703, 13703, 13703, 13703, 13703, 13703, 13703,   610, 13703, 13703,
     801, 13703, 13703,  7859, 13703,  7859,  7859, 13703,   710, -1669,
    7859,  1742,  -142,   837,    38,   155, -1669, -1669,  7859,  7859,
     556,   152,  7859,  7859,  7859,   176,  7859,  7859,  7859,  7859,
    7859,  7859,  7859,  7859,  7859,   864, -1669,   866,   779, -1669,
     875, -1669,   875, 13703,   897,  3288,   922,  1742,   640,   739,
   13703, 15318, 13703, 13703, 13703,    45,   642,   693, -1669,  7859,
     954, 14995,   768,   803,     4,   966,    -4,   814,   907,  1024,
   13703,   751,   923, 13703,   829, -1669,   943,   833,   835,   836,
     714, 13703, 15641,   838,   716,   965, 10141,   556,   152, 10467,
   10790, 11113,  8511, 11436,  7859,   864,   216,   953, -1669, -1669,
   -1669,  7859,   -34,   719,   720,   723,   729, 13703, 13703,  1742,
     724,   583,  1035,  1037,  1038,  1039,  1040,  1041,  8837, -1669,
     732, -1669, -1669, -1669,    75,   120,   133,   146,   160,   173,
   12411,     8, -1669, -1669, -1669, -1669, -1669, -1669, 14995,   738,
   -1669, -1669, -1669, 13703, 13703, 13703, 13703, 13703,    49,   740,
     741,   742,  1033, 14995,   745,   749,   750,   752,   753,   641,
     754,   755,   756,   757,   759,   760, 13703,   761,   159,   164,
    7859,  7859,   997,  7859,  1015,   765,   766,   769,   770,   772,
     778,   791, -1669, -1669, -1669, -1669, -1669,   940, 13703, -1669,
   -1669, -1669, -1669, -1669, -1669, -1669, -1669,   793, -1669,   984,
   13703, 13703, 13703, 13703, 13703,   798,  1104,   800,  1106,   804,
     805,   807,   809,   810,   812,   813,   815,   816,   818, 13703,
     820,   901,   825,   839, 13703,   175,    72,   811,   946,   274,
     842,   894,  1099, -1669,  7859,   911, -1669, -1669,   607,   774,
    7859,  1028, -1669,  1688,  1147,   228,   848,  7859,   301,   289,
     374,   182,   182,    70,    70,    70,   247, -1669,   981, -1669,
   -1669, -1669, -1669,  1157, -1669,   851,  5903,  7859, -1669,   181,
    1155, -1669, 13703,   854,   975,   856,    14,   857,    11,  1105,
   -1669,  1161, -1669,  1115,   183,  1152,    43,  1110, 15964,   998,
    1161,  1168,  1112,  1161,  1001,  1020,   890,  1148,  1174, 13703,
    1042,  1065, 13703,  1066,  1067, 13703, 14995,    12,  1043,   902,
   13703,  1179,   905,  1188, 13703,   848, 10467,   194,  7859, 13703,
    1197, -1669,  1161, -1669, -1669, -1669, -1669,  1046,    27,  1227,
    1213, -1669,   208,   908,   909,   910,   913,   914,   912,   666,
   -1669, -1669, 16287, -1669, -1669,  9163, -1669,  6229,   825, -1669,
     916,   918,   919,   921,   924,  1166,  1161, -1669, -1669, -1669,
   -1669,   925, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669,
   -1669, -1669, -1669, -1669,   926, -1669, -1669, -1669,  1287,  1742,
   -1669,  1310, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669,
    1232,   930,   940, -1669, 13703,  1253,   932,   933,   935,   936,
     937, -1669,  1235, -1669, 13703, -1669, -1669, -1669, -1669, -1669,
   -1669, -1669, -1669, -1669, -1669,   939, -1669,  1241,  7859,  1205,
   -1669, -1669,    47, -1669,  1186,  1161,   356,   945,  7859,  1217,
   -1669,  1107,  3944,  3618,  1742,  1229,   217, -1669,  7859,  7859,
    1688, 13703,  1081,  4925, 13703, -1669,   948,   219,   225, -1669,
   -1669,  1195,  1161,   951,  1161,  1189,  1161,  1161,  1254,  1161,
   13703,   952,   229,    52,  1097, -1669, 14995, 13703,   303,  1207,
   13703,  1217,   958,   231,  1267,   969,   277, 13703, 13703,  7859,
   13703,  1280,   980,  1163, 13703,   338, 13703, 13703,  1274,  1236,
    1283, -1669,  1258, 13703,  1161,  1045,  1311, -1669,  1243,   127,
     349,    18,   304, -1669,  1116,  7859,  1008,  -134,  1181, 13703,
    1009,  7859,  1014, -1669,  1323, -1669, -1669, -1669, -1669,  1325,
    1326, -1669, -1669, -1669,     6,  6555, 12411,    28, -1669, -1669,
   -1669,  1276, -1669,  1289, -1669, -1669, -1669, -1669, -1669, 13703,
     333, -1669, -1669,  7859,  7859,  1032, -1669,  1031, -1669, 13703,
    1044, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669,  1047,
    1742,   358,  1048,  1183,  1098,  1050, 13703,   360, -1669, -1669,
   -1669,   362,   940,  1354, -1669,  3944, -1669,  4270,  1356, -1669,
    1742,   364, -1669,  1083,  1162,    16, -1669, -1669, -1669, -1669,
   13703,   366,  7859,   368, 13057,   370,   372,   206,   377,    44,
    7859,  1058,  1360,   482, -1669,   405,   349,  1059,  1062,   379,
     258,  1645,  1269, 13703,  1364,   294,  1367,  1321,  1068, 13703,
       1,  1069,  7859,  1070,  1372,  7859,  1072,   -76,  1073,   381,
    1333,  1085,  1141,  7859,  1094,  1230, 16610,  1355,  1370,  1371,
    1100,  1237,  1347,  1422,  1122,   383, 13703, -1669,  1428,  1433,
   -1669,   161,  1231,  1240,  1275,  1138,  1149,  7859,  1247,  1742,
   -1669,  1153, 13703,  1473, -1669,  1742,  1156,  1164,  1165,  1169,
    1456,  1170,  1395,  9163,  1438, 13703,  1048,  1435,  1173,  1742,
    1742,  1472, -1669, -1669, -1669, -1669, -1669,  7859,  9489,  1175,
   -1669,  1176,  1478, -1669,  1441,  1178, -1669,  1180,  1182, -1669,
    1198, -1669,  1184, -1669,  1462,  1191,  1430,  1192,   393,  1193,
   16933,  1423,  1204,  1211,  1377,  1410, 13703,  1303,  1214,  1452,
   13703,  1742, -1669,  1212,  1393,  7859, -1669, -1669, 13703,  1399,
    7859, -1669,  1519,  1218,  1219,  1220, -1669,  1221,  1381, -1669,
    1382,  1383,  1225,  1233,   -33, -1669, -1669, -1669, -1669,   476,
   -1669, -1669, -1669, -1669, -1669,  1279, -1669,  1474,  1454,  7859,
   -1669,   424, -1669,  1533,   440, -1669,  1534, -1669, -1669,  1238,
   13703, -1669,  1536,   442,    30, -1669,  1537,  1538,  1239,  1511,
    1257,  1262,  7859,  1263,  1520,  1303,  1347,  1566,  1268,  1394,
   -1669, -1669,  1574, -1669,   109,  1575,  1576,  1272,  7859, -1669,
   -1669,  1578, -1669,  1549,  7859, -1669, -1669, -1669,  1277, 13380,
   -1669,  1447, -1669, -1669,  1563,  1459, -1669,  1290, 13703, -1669,
     608, -1669,  1742,   444,  1742,   466, -1669,  7859, -1669, 13703,
   -1669, -1669, -1669,  1291, -1669,  1593, -1669,  1551, -1669,  1411,
   -1669,  1293,  1392,  1535,  1297, -1669, -1669,  1601,  1490, -1669,
    1408,  1415, -1669, 13703,  1312,  7859,  1611,  1567,    58,   303,
     258, -1669,  1742, -1669,  1614,  1619,  1622,  1624, -1669, -1669,
   -1669,  1625,  1626, -1669,  1628, -1669, -1669,  1568, 13703,  1587,
     470,  1331,  1334,  1449,  1460, -1669,  1338,  1436,  1495,  1401,
      73, -1669, -1669, -1669,  1623,  7859,  7859,   474, 13703, 12734,
    1415,  1303,  1341, -1669, 13703, -1669, -1669, -1669,   118, -1669,
   -1669,  7859,   145,  1443,  1627,  1742, -1669,  1395,  1346,  6881,
    7859,  1584, -1669,   -55, -1669,  9815,   500,   137, -1669,  1376,
   13703,  1652,  1141,  1451, -1669, -1669,  1522,  1653,  1660,  1661,
    1581,  1361, -1669,  1742, -1669,  7859,  1506,  1527,  1380,  1385,
    1387,   502,  -107,   125,   141,  1388, 13703,  1629,  1389,  1217,
   -1669, -1669,  1636,  1507, -1669,  1492,  1498,  1707,  1692,  1424,
    1482,   504,   506,  1615,   508,  1445, -1669,  1546,  1415,  1303,
    1470,  1748, -1669,     7,   510, -1669,   725, -1669,  1749,  1448,
   -1669,  7859,  1450,  1453,  1457,  1742,  1733,   940,  1715,  1685,
     513,  1742,  1162,  1716,  1685,  1744,  1756,  1467,   515,  1772,
   13703,  1658, -1669, -1669,  1693,  1471, -1669,  1742,  1476,  1477,
   -1669, -1669, -1669, -1669,  1777,  1479, -1669,  1480, -1669,  1481,
   -1669, -1669,  1529,  1743,  1485,  1496,  1499,  1804,  1808, 13703,
    1603,  1504,  7859,  1812,  1730,  1217,  1814,  1512, 13703,  7859,
    7859,  1581,  1513, 13703, -1669, -1669, -1669, -1669,   351,  7859,
    1768,  1671,  7207,   521,  1515,  6229,  7533,  7859,  1521,  1569,
    1656,  1655,  1525,  1572,  1655,  1830,  1787, -1669,  1690,  1633,
    1839,  1840,  1841, -1669,  7859,  7859,  1540, -1669, -1669, -1669,
    1845, 13703,  7859,   108, -1669,  7859,    87, -1669,  1637,  1642,
    7207,  1402,   523, -1669,  1544,  1547,  1552,  1640, -1669,  1550,
     525,  1429,  1695, -1669,  1786,  1696,  1699, -1669, 12734,  1657,
   13703,  1561,   527, -1669, -1669,  6229,   529,   531,  1565,   806,
   -1669,  1564,  1673,  1675,  1621,  1876,  1577,  1573, -1669,  1630,
    1579,  1600, 13703,  1682,  1883,  1789,  1687, -1669,   557,   559,
   -1669,  1634,  1676,   565,  1586,  1585,   567,  1875,  1717,  1718,
    1591,  1879,  1599,  1904,   571,  7859,     5,  7859,  1905,  7859,
   -1669,  1906,  7859,  1857, 13703, 13703,  1635,  1538,  1739,  1750,
     242, 13703,  1751,  1606,  7207,  1605,  1612, -1669,  1616, -1669,
    6229, -1669, -1669, -1669,  7859,  7859,  7859,  1880,  1617, -1669,
    7859, -1669,  1618, -1669,  1902,  1632, 13703,  1778,   240,  1788,
    1924,   351,  1638,  1930, 13703, -1669,  7859, -1669,  1745,  1753,
    1754,  1937, -1669,  1726, -1669, -1669,  1681,  1561,  1773,  1775,
    1641, -1669,  1639,   103,  1890,   573,  1943,  1643,  1881, 13703,
    1945,  1785,  1662,  1955,  1725,  1765,  1637,  7859, -1669, -1669,
   -1669,  6229,   596, -1669,  1742,  1742,  7859, -1669,  7859,   598,
    7859,  1961, -1669,  1637,  1762, -1669, -1669,  1303,  1663,  1699,
   -1669,  1834,  1755,   601,  1897,  1792,  1969, -1669,  1970,  1669,
    7859,  1670, -1669,  1911,  1927,  1975,  1929, -1669, -1669,  1719,
      69, -1669,  1978, -1669, -1669,  1303,  1899,  1679,  1561,  1678,
   -1669,  1742,   626, -1669,   630,  1706,  1683, 13703,  1684, -1669,
    1750, 13703, 13703, -1669,  1799,  1987,  1829, -1669,  7859,   865,
    7859, 13703,   632, 12734,  1303,  1989, 13703,  1779, 13703, -1669,
    1415,  7859, -1669, -1669,  1162,  1162,  1713, -1669,  1637, -1669,
    1691,  1747,  1694,  1950,  1821,  1698,   636,  1996,  2001,   643,
    1973,  1702,  1942,  1303,  1217, -1669,  1703, 13703,  1705,  1581,
    1352,  1685,  1685,  1990,  1708, -1669,  1709, -1669,  1953,  1829,
   -1669,    35,  1769,  1712, -1669, 13703, -1669, 13703,  1217,  1714,
    7859,  1746, -1669,   124,  1303,  1655,  1655,  2014, -1669,  7859,
    1837,  1720,  1854,  1723,  2020,  1724,  1832,  1999,  1728, -1669,
     645, 13703,  1912,  1735, -1669,  1415,  1729,  1731,  1761,   647,
    1986, -1669,  7859, -1669,  1712, -1669,  2042, 13703, -1669,  1740,
    1752, -1669,  7859,  1581, -1669, -1669,  2024, -1669,  1757,   904,
    1759, -1669,  1850, -1669, -1669,   651, -1669,  2046,  7859,  2047,
    2048, -1669,  2053, -1669, -1669,   657,  1807,  1712, -1669,  1866,
    2054,  1760,  1985,  1712, -1669,  1877,  1763,  2002, -1669,  1882,
    2006,  1764,  7859,   661,  1766, -1669
  };

  const unsigned short int
  Parser::yydefact_[] =
  {
       0,   660,   713,   729,   731,   732,   735,   866,   712,     0,
       0,   736,     0,   707,   261,   632,   254,   260,   636,   637,
     262,     0,   236,   639,   640,   641,   642,   643,   644,   645,
     646,   647,   648,   649,   650,   651,   652,   653,   654,   655,
     656,     0,   657,   255,   658,   659,   427,   661,   348,   662,
     663,   664,   665,   666,   745,   667,   668,   669,   670,   671,
     672,   673,   674,   675,   676,   677,   678,   679,   680,   681,
     682,   683,   684,   685,   686,   687,   688,     0,   689,   690,
     691,   692,   693,     0,   694,   695,   696,   697,   698,   699,
     700,   701,   702,   703,   704,   705,   706,   708,   709,   710,
     711,   714,   715,   716,   717,   718,   719,   720,   721,   722,
     723,   724,   725,   726,   727,   728,   730,   263,   734,   737,
     738,   739,   740,   741,   742,   743,   744,   594,   746,   747,
     748,   749,   750,   751,   752,   753,   754,   755,   756,   757,
     758,   759,   760,   761,   762,   763,     0,   764,   765,   766,
     767,   768,   769,   770,   771,   889,   772,   773,   774,     0,
     775,   776,   777,   778,   779,   780,   781,   782,   783,   784,
     785,   786,   787,   788,   789,   790,   791,   792,   793,   794,
     795,   796,   797,   798,   799,   800,   801,   802,   803,   804,
     805,   806,   807,   808,   809,   810,   811,   812,   813,   814,
     815,   816,   817,   818,   819,   820,   821,   822,   823,   824,
     825,   826,   827,   828,   829,   830,   831,   832,   833,   834,
     835,   836,   837,   838,   839,   840,   841,   842,   843,   844,
     845,   846,   847,   848,   849,   850,   851,   852,   890,   853,
     854,   855,   856,   857,     0,   858,   859,   860,   861,   862,
     863,   864,   865,   867,   868,   869,   870,   871,   872,   873,
     874,   329,   875,   876,   877,   878,   879,   880,   881,   882,
     883,   884,   885,   886,   887,   888,     0,     0,     0,     2,
     118,   119,   122,    61,    62,    63,    64,    53,    54,    55,
      56,    57,    58,    59,    60,    49,    50,    51,    52,    67,
      69,    68,    70,    14,    16,    15,    17,    13,    26,    28,
      20,    24,     7,    86,    87,    88,    89,    95,    97,    96,
      98,    83,    84,   117,     0,   120,   123,    11,    29,    30,
      21,    65,     5,   112,   113,   114,   115,    35,    38,    39,
      40,    41,    42,    43,    44,    36,    37,    85,    71,    99,
      90,    93,    92,    94,    91,   106,    77,    31,    32,    33,
      34,    18,    25,    27,    19,     4,     0,   121,    66,   100,
     101,   102,   103,   104,   105,    82,    78,    79,    80,    81,
      10,    23,     9,    22,    12,     8,    45,    47,    46,    48,
     116,     0,     6,    72,    74,    75,    76,    73,   108,   107,
     109,   110,   111,   252,   646,   647,   649,   255,   658,   663,
     675,   828,   833,   864,   283,   284,   269,     0,     0,   642,
     676,   679,   681,   684,   695,     0,   602,   746,   770,   776,
     786,   787,   803,     0,   821,   831,   836,   837,   841,   842,
     851,   852,   890,   853,   856,   857,   859,   888,     0,     0,
     252,     0,     0,     0,     0,     0,     0,   837,     0,     0,
     660,   633,   634,   635,   638,   703,   704,   711,   733,   734,
       0,     0,     0,     0,   184,   185,     0,   189,   793,   876,
       0,   183,     0,     0,   354,     0,     0,     0,     0,     0,
       0,     0,     0,   600,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   307,   306,   307,     0,     0,     0,     0,     0,   595,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   402,     0,     0,     0,   330,
       0,   309,     0,     0,   309,     0,     1,     3,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   251,     0,   613,   425,
       0,   610,     0,     0,     0,     0,   258,   286,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   172,     0,
       0,     0,   810,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   171,     0,     0,     0,     0,
     602,     0,     0,     0,   486,     0,   713,   729,   731,   732,
     866,   712,     0,   736,     0,   858,     0,     0,   239,   237,
     238,     0,     0,     0,     0,     0,     0,     0,     0,   285,
     875,     0,     0,     0,     0,     0,     0,     0,     0,   128,
       0,   439,   428,   429,   430,   431,   434,   435,   433,   432,
     318,     0,   436,   352,   349,   350,   353,   351,     0,     0,
     233,   234,   216,   745,   673,   677,   686,   765,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   230,   215,   211,   343,   608,   451,     0,   374,
     509,   620,   533,   421,   242,   246,   396,     0,   367,   596,
       0,     0,     0,     0,     0,     0,   548,     0,   546,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   355,     0,     0,     0,     0,   403,   404,   341,
       0,     0,     0,   298,     0,     0,   296,   297,   270,   273,
       0,     0,   302,   271,     0,   276,     0,     0,   278,   279,
     282,   264,   265,   266,   267,   268,   280,   281,   482,   614,
     427,   611,   612,   256,   253,     0,     0,     0,   289,     0,
       0,   248,     0,     0,   635,     0,     0,     0,     0,     0,
     229,   311,   213,     0,     0,     0,   584,     0,     0,     0,
     311,     0,     0,   311,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   635,     0,
       0,     0,     0,   488,     0,   729,   732,   265,   322,     0,
     319,   506,   311,   372,   414,   508,   379,     0,   319,     0,
       0,   130,     0,     0,     0,     0,     0,     0,   875,     0,
     378,   316,   671,   438,   317,   444,   424,     0,   355,   535,
       0,     0,     0,     0,     0,     0,   311,   232,   214,   209,
     601,     0,   344,   373,   607,   415,   511,   537,   619,   522,
     423,   395,   241,   245,     0,   366,   287,   288,     0,   308,
     300,     0,   299,   249,   250,   210,   165,   224,   141,   156,
       0,     0,   451,   626,     0,   598,     0,     0,     0,     0,
       0,   231,     0,   604,     0,   339,   375,   609,   510,   621,
     534,   422,   397,   243,   247,     0,   368,     0,     0,     0,
     126,   413,   388,   295,     0,   311,     0,     0,     0,   319,
     398,     0,     0,     0,   310,     0,     0,   301,     0,     0,
     272,     0,   480,     0,     0,   290,     0,     0,     0,   291,
     259,     0,   311,     0,   311,     0,   311,   311,     0,   311,
       0,     0,     0,     0,     0,   603,     0,     0,   544,     0,
     743,   319,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   544,
       0,   631,     0,     0,   311,     0,     0,   484,     0,   490,
     194,   326,     0,   323,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   191,     0,   188,   186,   187,   193,     0,
       0,   129,   315,   314,   260,     0,   318,   319,   440,   443,
     442,     0,   437,     0,   208,   164,   223,   140,   155,     0,
       0,   592,   625,     0,     0,   452,   376,     0,   597,     0,
       0,   212,   166,   225,   142,   157,   549,   547,   627,     0,
     357,     0,     0,     0,     0,     0,     0,     0,   405,   406,
     401,     0,   451,     0,   331,     0,   332,     0,     0,   274,
     277,     0,   483,     0,   388,     0,   257,   294,   293,   292,
       0,     0,     0,     0,     0,     0,     0,   151,     0,     0,
       0,     0,     0,     0,   170,     0,   194,     0,     0,     0,
     550,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   628,     0,     0,     0,     0,   487,     0,     0,
     491,   493,     0,     0,     0,     0,     0,   325,     0,   320,
     235,     0,     0,     0,   615,   616,     0,     0,     0,     0,
       0,     0,   449,   444,   454,     0,     0,     0,     0,   304,
     305,     0,   377,   599,   593,   371,   356,     0,     0,     0,
     127,     0,     0,   512,     0,     0,   342,     0,     0,   334,
     672,   335,     0,   275,     0,     0,     0,     0,     0,     0,
       0,   221,     0,     0,     0,     0,     0,   147,     0,     0,
       0,   312,   226,     0,   173,     0,   540,   584,     0,     0,
       0,   554,     0,   572,   581,     0,   557,   573,   561,   558,
     559,   560,     0,     0,   564,   571,   570,   580,   552,     0,
     545,   588,   586,   587,   585,     0,   539,     0,     0,     0,
     338,     0,   369,     0,     0,   605,     0,   407,   505,     0,
       0,   618,     0,     0,   319,   514,     0,   767,     0,     0,
       0,     0,     0,     0,     0,   147,   628,     0,     0,     0,
     489,   492,     0,   494,   496,     0,     0,     0,     0,   321,
     324,     0,   507,     0,     0,   131,   190,   192,     0,   258,
     448,     0,   445,   441,     0,   471,   446,     0,     0,   384,
     303,   453,   358,   696,   360,     0,   125,     0,   389,     0,
     381,   340,   387,     0,   622,     0,   479,     0,   167,     0,
     163,   836,     0,     0,     0,   217,   135,     0,     0,   152,
       0,   153,   146,     0,     0,     0,     0,   197,     0,   544,
     551,   555,   553,   556,     0,     0,     0,     0,   568,   566,
     567,     0,     0,   569,     0,   590,   589,     0,     0,     0,
       0,     0,     0,     0,     0,   536,     0,     0,     0,     0,
       0,   200,   201,   417,     0,     0,     0,     0,     0,     0,
     153,   147,     0,   399,     0,   495,   497,   498,   499,   195,
     196,     0,     0,     0,     0,   617,   132,   449,     0,     0,
       0,   463,   347,   388,   359,     0,     0,   388,   328,     0,
       0,     0,     0,     0,   222,   218,     0,     0,     0,     0,
     149,     0,   227,   313,   174,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   319,
     370,   124,     0,     0,   416,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   629,     0,   153,   147,
       0,     0,   501,   502,     0,   175,   177,   327,     0,     0,
     447,     0,   665,   688,   455,   472,     0,   451,     0,   390,
     696,   362,   388,     0,   390,     0,     0,     0,     0,     0,
       0,     0,   148,   154,     0,     0,   228,   198,     0,     0,
     538,   582,   583,   563,     0,     0,   574,     0,   576,     0,
     578,   565,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   319,     0,     0,     0,     0,
       0,   149,     0,     0,   500,   503,   504,   485,   199,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   477,     0,
       0,   392,     0,     0,   392,     0,     0,   160,     0,     0,
       0,     0,     0,   143,     0,     0,     0,   575,   577,   579,
       0,     0,     0,     0,   336,     0,   411,   138,   136,     0,
       0,   531,     0,   529,     0,     0,     0,     0,   541,     0,
       0,     0,     0,   400,     0,     0,   204,   176,     0,     0,
       0,   527,     0,   525,   450,   461,     0,     0,   688,   468,
     466,   464,     0,   473,     0,     0,     0,     0,   513,     0,
       0,     0,     0,     0,     0,     0,     0,   150,     0,     0,
     562,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   523,     0,     0,     0,
     240,     0,     0,     0,     0,     0,     0,     0,     0,   206,
     181,     0,     0,     0,     0,   462,     0,   456,     0,   457,
       0,   470,   469,   465,     0,     0,     0,   475,     0,   391,
       0,   385,     0,   382,     0,     0,     0,     0,     0,     0,
       0,   199,     0,     0,     0,   345,     0,   337,     0,     0,
       0,     0,   410,     0,   134,   139,     0,   532,     0,     0,
       0,   530,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   151,   181,   136,     0,   521,   526,
     459,   461,     0,   467,   478,   474,     0,   426,     0,     0,
       0,     0,   162,   136,     0,   159,   158,   147,     0,   204,
     380,     0,     0,     0,     0,     0,     0,   412,     0,     0,
       0,     0,   516,     0,     0,     0,     0,   542,   624,     0,
       0,   205,     0,   168,   182,   147,     0,     0,   528,     0,
     458,   476,     0,   393,     0,     0,     0,     0,     0,   145,
     206,     0,     0,   346,     0,     0,   411,   137,     0,     0,
       0,     0,     0,     0,   147,     0,     0,     0,     0,   207,
     153,     0,   133,   460,   388,   388,     0,   220,   136,   144,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   147,   319,   543,     0,     0,     0,   149,
     151,   390,   390,     0,     0,   169,     0,   161,     0,   411,
     409,   523,     0,   523,   524,     0,   418,     0,   319,     0,
       0,     0,   364,   202,   147,   392,   392,     0,   219,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   244,
       0,     0,     0,     0,   178,   153,     0,     0,     0,     0,
       0,   408,     0,   515,   523,   517,     0,     0,   394,     0,
       0,   180,     0,   149,   386,   383,     0,   591,     0,     0,
       0,   419,     0,   623,   365,     0,   179,     0,     0,     0,
       0,   518,     0,   203,   481,     0,     0,   523,   420,     0,
       0,     0,     0,   523,   519,     0,     0,     0,   520,     0,
       0,     0,     0,     0,     0,   606
  };

  const short int
  Parser::yypgoto_[] =
  {
   -1669, -1669,  -274, -1669, -1669, -1669, -1669, -1669, -1669, -1413,
     616, -1669, -1669, -1669, -1669, -1317, -1585, -1541, -1442, -1669,
   -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669,  1079, -1669,
     465,   491,   306, -1438,   947, -1669, -1461, -1669,   283,   244,
   -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669,
   -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669,
   -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669,
       0, -1669,   876, -1669,  1594,  -250,   339, -1008,  -876, -1669,
   -1669,   899, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669,
   -1669, -1669, -1669, -1669, -1669, -1669,  1210,  -965,   883, -1669,
   -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669,
   -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1129, -1532, -1563,
   -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669,
   -1668, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669,
    -573,  1300,  -482,  -891, -1669,   888, -1669, -1669, -1669,   646,
    -935, -1669, -1669,   313, -1669,   371, -1669, -1669, -1669, -1669,
   -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669,
   -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1669,
   -1669, -1669, -1502,   467,   382,   250,   402, -1669, -1669, -1669,
   -1669, -1669, -1669, -1669, -1019,  -596, -1669,   843, -1669,   845,
   -1669, -1669, -1669, -1669, -1669, -1669,  1493, -1669, -1669, -1669,
   -1669, -1669, -1669, -1669,  1736, -1669, -1669, -1669, -1669, -1669,
   -1669, -1669, -1669, -1669, -1669, -1669, -1669, -1322,   584,  1036
  };

  const short int
  Parser::yydefgoto_[] =
  {
      -1,   278,   279,   280,   281,   969,  1239,   282,   283,  1712,
    1323,   284,   285,   286,   287,  1401,  1565,  1267,  1490,   288,
     289,   290,  1759,   291,   292,   293,   294,   295,   448,  1407,
    1534,  1535,  1794,   480,  1204,  1496,  1187,  1954,  1729,  1792,
     296,   297,   298,   299,   300,   301,   302,   303,  1394,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   451,   313,
     314,   315,   316,   317,   318,   319,   320,   321,   322,   323,
     561,   511,   512,   513,   722,   565,  1012,   893,  1056,   325,
    1052,  1053,   326,   560,   983,  1127,   327,   328,   329,   979,
     330,   331,  1643,   332,   484,   688,   970,  1671,  1240,  1375,
     333,   334,   335,   336,   337,   338,   339,   340,   341,   342,
     343,   344,   345,   346,   347,   348,   349,  1115,  1621,  1687,
     350,   351,   352,   353,   354,   355,   356,   778,   977,   357,
    1710,   358,   359,   360,   361,  1872,   362,   363,   364,   365,
     366,   483,  1735,   682,  1077,  1078,  1079,  1080,  1081,  1362,
     941,  1365,  1676,  1736,  1547,  1680,  1681,  1743,  1471,  1747,
    1807,  1683,   367,  1134,   992,   368,   862,   863,  1049,  1201,
    1344,  1458,  1533,  1607,   369,   370,   371,   372,   373,   374,
     375,   376,  1780,  1672,  1673,  1652,  1653,   377,   378,   379,
     380,   381,   382,  1524,  1168,   757,  1159,  1160,  1298,  1018,
     383,   384,   533,   945,  1100,   703,   611,   385,   386,   387,
     388,   389,   390,   391,   590,   392,   878,   393,   394,   395,
     396,   397,   398,   399,   400,   401,   402,  1335,  1042,   403
  };

  const short int
  Parser::yytable_[] =
  {
     324,   681,  1060,  1111,   567,  1255,  1082,  1097,  1527,   414,
     415,  1526,   416,   562,  1451,   836,  1662,   811,  1450,   812,
    1191,   449,  1624,   568,   569,   570,   571,   572,   668,   573,
     574,   575,  1220,   576,   577,   578,   579,   580,   581,   582,
     583,   458,   584,   568,   569,   570,   571,   572,   417,   573,
     574,   575,  1016,   576,   577,   578,   579,   580,   581,   582,
     583,  1690,   584,   568,   569,   570,   571,   572,  1222,   573,
     574,   575,   593,   576,   577,   578,   579,   580,   581,   582,
     583,  -812,   584,   593,  1055,  1055,  1601,  1055,  1008,  1040,
    -812,  -812,   898,  -812,   584,  1308,  -812,  -812,  1005,  -812,
     510,  -812,  1605,  1122,   839,   587,  1113,   911,   568,   569,
     570,   571,   572,   598,   573,   574,   575,  1707,   576,   577,
     578,   579,   580,   581,   582,   583,  -813,   584,  1269,   829,
     647,  1518,   556,  1708,  1528,  -813,  -813,  1952,  -813,  -814,
    1423,  -813,  -813,  1316,  -813,  1171,  -813,  1666,  -814,  -814,
     895,  -814,  -815,  1575,  -814,  -814,   630,  -814,   895,  -814,
    1153,  -815,  -815,   588,  -815,   791,  -816,  -815,  -815,  1877,
    -815,  1114,  -815,   783,   453,  -816,  -816,  1258,  -816,  -817,
     737,  -816,  -816,   796,  -816,   797,  -816,  1247,  -817,  -817,
     631,  -817,   784,   454,  -817,  -817,  -633,  -817,  1895,  -817,
    1152,  1224,   581,   582,   583,  1211,   584,  1311,   842,  1778,
    1314,   455,  1602,  1548,  -269,  -269,  -269,  -812,  -269,   591,
     452,   568,   569,   570,   571,   572,  1576,   573,   574,   575,
    1730,   576,   577,   578,   579,   580,   581,   582,   583,  1942,
     584,   575,   738,   576,   577,   578,   579,   580,   581,   582,
     583,  1941,   584,  1845,   725,  1317,   557,   726,   718,   719,
    1039,   905,  -813,   576,   577,   578,   579,   580,   581,   582,
     583,  1279,   584,  1114,   456,  -814,  1709,   564,   324,   568,
     569,   570,   571,   572,   974,   573,   574,   575,  -815,   576,
     577,   578,   579,   580,   581,   582,   583,   871,   584,  1424,
    1819,   792,  -816,   775,   872,   777,   578,   579,   580,   581,
     582,   583,   588,   584,   585,  -817,  1264,   577,   578,   579,
     580,   581,   582,   583,  1933,   584,  1334,   599,   843,  1154,
    1155,  1270,  1606,  1309,   585,  1779,   840,  1953,  -635,   896,
    1041,   418,   897,  1009,  1549,   819,  1006,  -634,  1554,   594,
     897,  1467,  -628,  1205,   585,   669,  -638,   418,  1519,   834,
     594,  1059,  1223,  1114,   784,  1779,   585,  1280,   589,  1934,
    1878,   786,  1956,  1957,   727,  1017,   830,   831,   595,  1935,
    1936,   906,   728,  1847,   482,  1577,   729,  1162,  1986,   595,
    1498,  1497,   579,   580,   581,   582,   583,   730,   584,   585,
    1856,  1579,  -733,  1163,   975,  1553,  -812,  1281,  1795,  -812,
     485,  1185,  1476,   683,   684,   648,   649,   650,   597,  1943,
    1158,  1945,  1202,  1622,  1185,   685,  1456,   686,  1282,  1265,
    1186,  1457,   600,   619,  1531,  1903,  1834,   549,  1909,  1532,
     601,  1203,  1704,  1665,   602,   550,  1199,  1705,  1440,   646,
    1200,  -813,   555,   750,  -813,   603,   551,  1815,  1578,   659,
     661,   751,  1980,   508,  -814,   752,  1793,  -814,  1266,  1164,
    1165,  1816,   509,  1166,  1580,  1914,   753,  -815,   585,   784,
    -815,  1342,  1537,   680,   554,  1343,   486,   487,   787,   784,
     585,  -816,   926,   784,  -816,  2001,   731,   927,   784,   709,
    1858,  2006,  1221,   553,  -817,   651,   559,  -817,   973,   784,
     552,  1135,   585,  1973,   999,   784,  1014,   784,   558,   568,
     569,   570,   571,   572,   585,   573,   574,   575,  1880,   576,
     577,   578,   579,   580,   581,   582,   583,   563,   584,   459,
     986,  1063,  1064,   585,   585,  1118,  1119,   596,   868,   615,
    1129,   784,  1138,   784,   604,   608,   779,  1904,  1139,   784,
     782,   620,  1151,  1152,  1173,  1152,   997,   998,   788,   789,
     585,  1167,   793,   794,   795,   754,   798,   799,   800,   801,
     802,   803,   804,   805,   806,   585,  1928,   609,   568,   569,
     570,   571,   572,   621,   573,   574,   575,   585,   576,   577,
     578,   579,   580,   581,   582,   583,   978,   584,   622,   687,
    1176,  1152,  1618,   569,   570,   571,   572,  1955,   573,   574,
     575,  -170,   576,   577,   578,   579,   580,   581,   582,   583,
     625,   584,   632,  1585,  1302,  1303,   788,  1206,  1207,   793,
     794,   795,   414,   800,   867,   635,   568,   569,   570,   571,
     572,   870,   573,   574,   575,   657,   576,   577,   578,   579,
     580,   581,   582,   583,   662,   584,  1228,  1152,   889,   663,
     585,   568,   569,   570,   571,   572,   664,   573,   574,   575,
    1706,   576,   577,   578,   579,   580,   581,   582,   583,   665,
     584,  1236,  1237,  1245,  1152,  1246,   784,  1253,   784,  1257,
    1152,  1259,  1152,  1262,  1152,  1263,  1152,   666,  1124,  1126,
    1268,  1152,  1277,  1278,  1319,   784,  1338,  1152,   667,  1656,
     928,   929,   690,   931,   702,  1082,  1389,  1237,  1121,   691,
     568,   569,   570,   571,   572,   692,   573,   574,   575,  1131,
     576,   577,   578,   579,   580,   581,   582,   583,   716,   584,
    1777,   721,  -743,  1425,  1426,  1911,  1912,  1431,  1237,  -743,
    -743,  -743,  -743,   723,  -743,  -743,  -743,  -743,  -743,   732,
    -743,   747,  -743,  1433,  1237,  1438,  1439,  -361,  -361,  1179,
     733,   570,   571,   572,   984,   573,   574,   575,   734,   576,
     577,   578,   579,   580,   581,   582,   583,   990,   584,  1474,
    1475,   749,  -630,  1509,   784,   769,  -743,  1523,   784,   735,
     585,   568,   569,   570,   571,   572,   736,   573,   574,   575,
     739,   576,   577,   578,   579,   580,   581,   582,   583,   740,
     584,   772,  1848,  1552,  1237,  1573,  1574,  1594,   784,  1595,
     784,  1597,  1598,  1608,  1609,   586,  -363,  -363,  1628,  1439,
     741,  1249,   781,  1251,  1674,   784,  1716,  1717,  1723,   784,
    1733,  1734,  1737,  1738,  1739,   897,   990,   785,  1051,   742,
     568,   569,   570,   571,   572,   743,   573,   574,   575,   585,
     576,   577,   578,   579,   580,   581,   582,   583,   790,   584,
    1761,  1609,  1762,   784,   807,  1076,   808,   680,  1765,   784,
    1768,  1237,   809,   585,  1776,  1734,  1836,   784,   744,   568,
     569,   570,   571,   572,   881,   573,   574,   575,   745,   576,
     577,   578,   579,   580,   581,   582,   583,   814,   584,  1850,
     897,  1853,   784,  1324,  1863,   784,   746,   585,   810,   568,
     569,   570,   571,   572,   748,   573,   574,   575,   820,   576,
     577,   578,   579,   580,   581,   582,   583,   822,   584,  1884,
     784,   833,   585,  1885,   784,  1901,  1902,   835,  1110,  1921,
    1717,   821,   917,   832,  1741,  1742,  1924,   784,  1969,   784,
    1977,   784,   324,   324,  1993,   784,   837,  1544,  1130,   658,
    1999,  1237,   838,   680,  2014,  1237,   841,  1071,   568,   569,
     570,   571,   572,   844,   573,   574,   575,   845,   576,   577,
     578,   579,   580,   581,   582,   583,   848,   584,   851,   849,
     852,   585,   853,  1995,   854,   855,   610,   860,  1929,   568,
     569,   570,   571,   572,   861,   573,   574,   575,   864,   576,
     577,   578,   579,   580,   581,   582,   583,  2013,   584,   869,
     873,   874,  1948,  1041,   875,  1209,   880,   450,   610,  1430,
     876,  1215,   882,   890,   883,   884,   885,   886,   887,   899,
     585,   907,   908,   909,   910,   564,   912,   930,  -743,   481,
     913,   914,  1447,   915,   916,   918,   919,   920,   921,  1897,
     922,   923,   925,  1229,  1230,   932,   933,   934,  1462,   940,
     935,   936,   585,   937,   568,   569,   570,   571,   572,   938,
     573,   574,   575,   488,   576,   577,   578,   579,   580,   581,
     582,   583,   939,   584,   943,   324,   944,   324,  1989,   951,
     952,   953,   954,  1677,   967,   955,   956,   976,   957,  1898,
     958,   959,  1110,   960,   961,   784,   962,   963,   720,   964,
    1271,   966,   568,   569,   570,   571,   572,   968,   573,   574,
     575,   585,   576,   577,   578,   579,   580,   581,   582,   583,
     971,   584,  1110,   980,   981,  1110,   985,   987,  1990,  1023,
     989,   991,  1026,   994,   995,  1000,  1002,  1003,  1004,  1007,
    1010,  1011,  1013,  1015,  1019,  1521,  1522,  1022,  1024,  1025,
     585,  1027,   568,   569,   570,   571,   572,  1051,   573,   574,
     575,  1057,   576,   577,   578,   579,   580,   581,   582,   583,
    1028,   584,  1029,  1076,  1031,  1030,  1034,  1036,  1037,  1046,
     585,  1033,  1043,  1048,  1044,  1058,  1047,  1372,  1374,  1061,
    1062,  1065,  1066,  1067,  1070,  1090,  1068,  1084,  1069,  1085,
    1086,   982,  1087,  1089,  1055,  1088,  1091,  1092,  1802,  1095,
     846,  1096,  1099,  1101,  1102,  1106,  1103,  1104,  1105,   988,
    1108,  1109,  1112,  1116,  1055,  1408,  1120,  1133,  1123,  1128,
    1412,  1137,  1140,  1142,  1147,  1144,  1157,  1150,  1169,   585,
    1172,  1613,   568,   569,   570,   571,   572,  1174,   573,   574,
     575,  1175,   576,   577,   578,   579,   580,   581,   582,   583,
    1181,   584,  1182,  1192,  1117,   568,   569,   570,   571,   572,
     585,   573,   574,   575,  1183,   576,   577,   578,   579,   580,
     581,   582,   583,  1190,   584,  1193,  1163,  1198,  1197,  1210,
    1214,  1141,  1212,  1143,  1196,  1145,  1146,  1216,  1148,  1660,
    1217,  1208,  1218,  1219,  1465,  1225,  1226,   568,   569,   570,
     571,   572,  1232,   573,   574,   575,  1231,   576,   577,   578,
     579,   580,   581,   582,   583,  1234,   584,  1110,  1235,  1241,
    1238,  1243,  1242,  1195,  1248,  1699,  1252,  1254,  1114,  1272,
    1273,  1275,  1703,  1276,  1299,   585,  1301,  1304,  1305,  1306,
    1310,  1312,  1313,  1315,  1318,  1493,  1320,   568,   569,   570,
     571,   572,  1322,   573,   574,   575,  1321,   576,   577,   578,
     579,   580,   581,   582,   583,  1325,   584,  1326,  1329,  1330,
    1331,  1334,  1332,  1333,   568,   569,   570,   571,   572,  1093,
     573,   574,   575,   585,   576,   577,   578,   579,   580,   581,
     582,   583,  1336,   584,  1337,  1340,  1345,   605,   606,   607,
    1341,  1536,  1094,   613,   614,  1346,   616,   617,   618,   680,
    1545,  1347,  1785,   624,  1348,  1551,   626,   627,   628,   629,
    1349,  1351,   633,   634,  1352,  1354,  1359,  1355,   652,   653,
     654,   655,   656,   585,  1361,  1567,  1364,  1356,  1357,  1371,
    1809,  1368,  1358,  1360,  1369,  1378,  1376,  1379,  1377,  1380,
    1383,  1381,  1385,  1382,  1387,  1384,  1823,  1393,   670,  1397,
    1398,   689,  1386,  1388,  1390,   698,   699,   700,   701,  1400,
     704,   705,   706,   707,   708,  1395,   710,   711,   712,   713,
     714,   715,  1396,   717,  1403,  1402,  1406,  1405,  1411,  1413,
    1414,  1415,  1416,  1417,  1418,  1419,  1420,  1421,  1852,  1427,
    1854,  1428,  1429,  1432,  1434,  1422,  1437,  1441,  1442,  1435,
    1443,   755,   758,   759,   760,   761,   762,   763,   764,   765,
     766,   767,   768,   585,   770,   771,  1444,   773,   774,  1445,
     776,  1715,  1651,   780,  1446,  1448,  1452,  1449,  1454,  1453,
    1661,  1455,  1459,  1460,  1461,  1463,   585,  1464,  1466,  1536,
     514,  1468,  1110,  1469,  1266,   680,   680,  1679,  1724,  1470,
    1899,  1472,  1478,  1479,  1480,  1482,  1481,  1483,  1485,   813,
    1484,  1486,  1487,  1488,  1536,  1489,   823,   825,   826,   827,
     828,  1494,  1495,  1492,  1499,  1110,  1500,   758,   585,  1501,
    1110,  1502,  1503,  1504,   515,  1505,   847,  1506,   516,   850,
     517,  1508,  1510,   518,  1512,  1511,  1513,   857,   859,  1514,
    1516,  1515,  1517,  1520,  1529,   680,  1538,  1539,  1541,  1546,
    1950,  1555,  1557,  1561,   519,  1559,  1560,  1562,  1563,  1959,
    1564,  1569,  1566,   877,   879,  -818,  -818,  -818,   585,   573,
     574,   575,  1568,   576,   577,   578,   579,   580,   581,   582,
     583,  1570,   584,  1583,  1587,  1110,   894,  1651,  1571,  1783,
    1572,  1581,  1985,  1586,   758,   585,  1588,  1584,  1589,   900,
     901,   902,   903,   904,  1110,   520,  1283,  1590,   534,   758,
     680,   535,  1591,  1593,  1679,  1804,  1805,   568,   569,   570,
     571,   572,   924,   573,   574,   575,  1592,   576,   577,   578,
     579,   580,   581,   582,   583,  1596,   584,  1600,  1603,   521,
     522,   523,  1284,   489,   942,  1604,   490,  1599,   491,  1611,
    1612,   492,  1614,  1617,  1620,  1615,   946,   947,   948,   949,
     950,  1616,  1619,  1623,  1625,  1285,  1626,  1110,  1627,  1629,
    1631,   680,  1633,  1632,  1636,   965,  1851,   524,  1634,  1635,
     972,  1640,  1637,  1638,  1639,   536,  1641,  1642,   537,  1286,
    1287,  1288,  1289,  1290,  1291,  1292,  1293,  1644,  1294,  1295,
    1869,  1645,  1296,  1297,  1646,  1647,  1650,   525,  1649,  1654,
    1655,  1657,   526,  1658,  1663,  1668,  1669,  1675,  1682,  1684,
     493,  1686,  1689,   494,   538,  1685,  1688,   527,  1001,   528,
    1691,   529,   530,   531,  1692,  1693,  1694,   532,  1651,  1695,
    1696,  1697,   539,  1700,  1021,  1701,  1713,  1711,  1718,  1719,
    1721,  1910,  1722,  1720,  1725,  1032,  1726,  1731,  1035,   495,
     540,  1038,   758,  1727,  1728,  1237,  1045,  1740,  1744,  1746,
    1050,  1745,  1748,  1749,  1751,  1054,  1754,   496,   497,  1750,
    1753,  1752,  1756,  1757,  1758,  1760,  1767,  1763,  1766,  1764,
     541,  1769,  1772,  1771,  1770,   498,   542,   499,  1073,  1773,
    1774,  1775,  1782,  1784,  1786,  1789,  1790,  1798,  1791,   897,
    1797,   543,   544,   545,   546,  1800,  1806,   547,  1801,  1808,
    1810,   548,  1811,  1814,  1818,   500,  1817,  1821,  1826,  1824,
    1825,   501,  1979,  1812,  1827,  1828,  1829,  1835,  1830,  1820,
    1831,  1833,  1832,  1837,  1838,  1841,   502,   503,   504,   505,
    1098,  1839,   506,  1842,   585,  1844,   507,  1266,  1110,  1793,
    1107,  1855,  1857,  1843,  1859,  1861,  1862,  1864,  1865,  1866,
    1867,  1868,  1870,  1871,  1873,  1874,  1875,  1876,  1879,  1881,
    1882,  1883,  1110,  1886,  1887,  1889,  1893,  1894,  1709,  1905,
    1907,  1913,  1915,  1918,  1919,  1917,  1922,  1132,  1916,  1920,
    1136,  1923,  1925,  1926,  1927,  1930,  1932,  1940,   585,  1938,
    1937,  1939,  1779,  1944,  1958,  1949,  1149,  1951,  1960,  1962,
    1964,  1961,   758,  1161,  1963,  1965,  1170,  1966,  1967,  1968,
    1974,  1971,  1975,  1177,  1178,  1976,  1180,  1972,  1978,  1981,
    1184,  1983,  1188,  1189,  1987,  1992,  1994,  1996,  1997,  1194,
    1998,  2000,  2002,  1984,  2003,  2005,  2009,  2007,  2011,  1988,
    1991,  2004,  1156,  2010,  2008,  1213,  2012,  2015,  1558,  1698,
    1667,  1846,  1860,  1274,  1890,  1370,  1350,   724,  1083,  1367,
     993,  1363,   894,  1540,  1849,  1803,  1799,  1714,  1896,  1781,
    1610,  1410,  1409,   856,     0,  1227,     0,   592,     0,     0,
       0,     0,     0,     0,     0,  1233,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  1244,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  1256,     0,     0,     0,
    1261,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  1300,
       0,     0,     0,     0,     0,  1307,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  1328,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  1339,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  1353,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  1366,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  1392,     0,     0,     0,
       0,     0,  1399,     0,     0,     0,  1404,     0,     0,     0,
       0,     0,     0,     0,  1161,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  1436,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   894,     0,     0,     0,     0,
       0,     0,     0,     0,  1473,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  1477,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  1491,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  1507,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  1525,   481,     0,     0,     0,     0,
    1530,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  1556,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  1582,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  1630,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  1648,     0,     0,     0,     0,
       0,     0,     0,     0,  1659,     0,     0,     0,     0,  1664,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  1702,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   481,     0,  1732,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  1755,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    1787,  1788,     0,     0,     0,     0,     0,  1796,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  1813,     0,     0,     0,     0,     0,     0,     0,
    1822,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  1840,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  1888,     0,     0,     0,  1891,  1892,     0,
       0,     0,     0,     0,     0,     0,     0,  1900,     0,   481,
       0,     0,  1906,     0,  1908,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  1931,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  1946,   566,  1947,     0,     0,     1,     2,     0,     3,
       4,     5,     6,     7,     8,     9,    10,     0,     0,    11,
       0,    12,     0,     0,     0,    13,     0,  1970,     0,    14,
      15,    16,    17,    18,    19,     0,    20,    21,    22,    23,
      24,    25,    26,  1982,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,     0,
       0,    42,    43,    44,    45,    46,    47,    48,     0,    49,
       0,    50,    51,    52,    53,    54,    55,    56,    57,     0,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,     0,     0,    74,    75,
      76,    77,     0,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,     0,     0,     0,     0,     0,    97,     0,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   107,     0,
     108,   109,   110,     0,     0,   111,   112,   113,   114,   115,
     116,   117,   118,   119,     0,   120,   121,     0,   122,   123,
     124,     0,     0,   125,   126,   127,   128,   129,   130,   131,
     132,   133,   134,   135,   136,   137,   138,     0,   139,     0,
       0,     0,     0,     0,   140,     0,     0,     0,     0,     0,
     141,   142,   143,   144,   145,   146,   147,   148,   149,   150,
     151,   152,   153,   154,   155,   156,   157,   158,   159,   160,
     161,   162,   163,   164,   165,   166,   167,   168,   169,   170,
     171,   172,   173,   174,   175,   176,     0,   177,   178,   179,
     180,   181,   182,   183,   184,   185,   186,   187,   188,   189,
       0,   190,   191,   192,   193,   194,   195,   196,     0,   197,
     198,   199,   200,   201,   202,   203,   204,   205,   206,   207,
     208,   209,   210,   211,   212,   213,   214,   215,   216,   217,
     218,   219,   220,   221,   222,   223,   224,     0,   225,   226,
     227,   228,   229,   230,   231,   232,   233,   234,   235,   236,
     237,   238,   239,   240,   241,   242,   243,   244,   245,   246,
     247,   248,   249,   250,   251,   252,   253,   254,   255,   256,
     257,   258,   259,   260,   261,   262,   263,   264,   265,   266,
     267,   268,   269,   270,   271,   272,   273,   274,   275,     0,
       0,   276,     1,     2,   277,     3,     4,     5,     6,     7,
       8,     9,    10,     0,     0,    11,     0,    12,   815,     0,
       0,    13,     0,     0,     0,    14,    15,    16,    17,    18,
      19,     0,    20,     0,     0,    23,    24,    25,    26,     0,
      27,    28,    29,   404,   405,    32,   406,    34,    35,    36,
      37,    38,    39,    40,     0,     0,     0,    42,   407,   408,
      45,     0,    47,     0,     0,    49,     0,   409,    51,    52,
      53,    54,    55,    56,    57,     0,    58,    59,    60,    61,
      62,   410,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,     0,     0,    74,    75,    76,     0,     0,    78,
      79,    80,    81,    82,     0,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,     0,     0,
       0,     0,     0,    97,   816,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,     0,   108,   109,   110,     0,
       0,   111,   112,   113,   114,   115,   116,   117,   118,   119,
       0,   120,   121,     0,   122,   123,   124,     0,     0,   125,
     126,     0,   128,   129,   130,   131,   132,   133,   134,   135,
     136,   137,   138,     0,   139,     0,     0,     0,     0,     0,
     140,     0,     0,     0,     0,     0,   141,   142,   143,   144,
     145,     0,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,     0,   160,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176,     0,   177,   178,   179,   180,   181,   182,   183,
     184,   185,   186,   187,   188,   189,     0,   190,   191,   192,
     193,   194,   195,   196,   817,   197,   198,   199,   200,   201,
     202,   203,   204,   205,   206,   207,   208,   209,   210,   211,
     212,   411,   214,   215,   216,   217,   412,   219,   220,   221,
     222,   223,   224,     0,   225,   226,   227,   228,   229,   230,
     231,   232,   233,   234,   235,   236,   237,   238,   239,   240,
     241,   242,   243,     0,   245,   246,   247,   248,   249,   250,
     413,   252,   253,   254,   255,   256,   257,   258,   259,   260,
       0,   262,   263,   264,   265,   266,   267,   268,   269,   270,
     271,   272,   273,   274,   275,     0,     0,     0,     0,     0,
     277,   818,     1,     2,     0,     3,     4,     5,     6,     7,
       8,     9,    10,     0,     0,    11,     0,    12,     0,     0,
       0,    13,     0,     0,     0,    14,    15,    16,    17,    18,
      19,     0,    20,    21,    22,    23,    24,    25,    26,     0,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,     0,     0,    42,    43,    44,
      45,    46,    47,    48,     0,    49,     0,    50,    51,    52,
      53,    54,    55,    56,    57,     0,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,     0,     0,    74,    75,    76,    77,     0,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,     0,     0,
       0,     0,     0,    97,     0,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,  1125,   108,   109,   110,     0,
       0,   111,   112,   113,   114,   115,   116,   117,   118,   119,
       0,   120,   121,     0,   122,   123,   124,     0,     0,   125,
     126,   127,   128,   129,   130,   131,   132,   133,   134,   135,
     136,   137,   138,     0,   139,     0,     0,     0,     0,     0,
     140,     0,     0,     0,     0,     0,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176,     0,   177,   178,   179,   180,   181,   182,   183,
     184,   185,   186,   187,   188,   189,     0,   190,   191,   192,
     193,   194,   195,   196,     0,   197,   198,   199,   200,   201,
     202,   203,   204,   205,   206,   207,   208,   209,   210,   211,
     212,   213,   214,   215,   216,   217,   218,   219,   220,   221,
     222,   223,   224,     0,   225,   226,   227,   228,   229,   230,
     231,   232,   233,   234,   235,   236,   237,   238,   239,   240,
     241,   242,   243,   244,   245,   246,   247,   248,   249,   250,
     251,   252,   253,   254,   255,   256,   257,   258,   259,   260,
     261,   262,   263,   264,   265,   266,   267,   268,   269,   270,
     271,   272,   273,   274,   275,     0,     0,   276,     1,     2,
     277,     3,     4,     5,     6,     7,     8,     9,    10,     0,
       0,    11,     0,    12,     0,     0,     0,    13,     0,     0,
       0,    14,    15,    16,    17,    18,    19,     0,    20,    21,
      22,    23,    24,    25,    26,     0,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,     0,     0,    42,    43,    44,    45,    46,    47,    48,
       0,    49,     0,    50,    51,    52,    53,    54,    55,    56,
      57,     0,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,     0,     0,
      74,    75,    76,    77,     0,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,     0,     0,     0,     0,     0,    97,
       0,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,     0,   108,   109,   110,     0,     0,   111,   112,   113,
     114,   115,   116,   117,   118,   119,     0,   120,   121,     0,
     122,   123,   124,     0,     0,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   137,   138,     0,
     139,     0,     0,     0,     0,     0,   140,     0,     0,     0,
       0,     0,   141,   142,   143,   144,   145,   146,   147,   148,
     149,   150,   151,   152,   153,   154,   155,   156,   157,   158,
     159,   160,   161,   162,   163,   164,   165,   166,   167,   168,
     169,   170,   171,   172,   173,   174,   175,   176,     0,   177,
     178,   179,   180,   181,   182,   183,   184,   185,   186,   187,
     188,   189,     0,   190,   191,   192,   193,   194,   195,   196,
       0,   197,   198,   199,   200,   201,   202,   203,   204,   205,
     206,   207,   208,   209,   210,   211,   212,   213,   214,   215,
     216,   217,   218,   219,   220,   221,   222,   223,   224,     0,
     225,   226,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,   238,   239,   240,   241,   242,   243,   244,
     245,   246,   247,   248,   249,   250,   251,   252,   253,   254,
     255,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,     0,     0,   276,     1,     2,   277,     3,     4,     5,
       6,     7,     8,     9,    10,     0,     0,    11,     0,    12,
       0,     0,     0,    13,     0,     0,     0,    14,    15,    16,
      17,    18,    19,     0,    20,    21,    22,    23,    24,    25,
      26,     0,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,     0,     0,    42,
      43,    44,    45,    46,    47,    48,     0,    49,     0,    50,
      51,    52,    53,    54,    55,    56,    57,     0,    58,    59,
    1250,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,     0,     0,    74,    75,    76,    77,
       0,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
       0,     0,     0,     0,     0,    97,     0,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,     0,   108,   109,
     110,     0,     0,   111,   112,   113,   114,   115,   116,   117,
     118,   119,     0,   120,   121,     0,   122,   123,   124,     0,
       0,   125,   126,   127,   128,   129,   130,   131,   132,   133,
     134,   135,   136,   137,   138,     0,   139,     0,     0,     0,
       0,     0,   140,     0,     0,     0,     0,     0,   141,   142,
     143,   144,   145,   146,   147,   148,   149,   150,   151,   152,
     153,   154,   155,   156,   157,   158,   159,   160,   161,   162,
     163,   164,   165,   166,   167,   168,   169,   170,   171,   172,
     173,   174,   175,   176,     0,   177,   178,   179,   180,   181,
     182,   183,   184,   185,   186,   187,   188,   189,     0,   190,
     191,   192,   193,   194,   195,   196,     0,   197,   198,   199,
     200,   201,   202,   203,   204,   205,   206,   207,   208,   209,
     210,   211,   212,   213,   214,   215,   216,   217,   218,   219,
     220,   221,   222,   223,   224,     0,   225,   226,   227,   228,
     229,   230,   231,   232,   233,   234,   235,   236,   237,   238,
     239,   240,   241,   242,   243,   244,   245,   246,   247,   248,
     249,   250,   251,   252,   253,   254,   255,   256,   257,   258,
     259,   260,   261,   262,   263,   264,   265,   266,   267,   268,
     269,   270,   271,   272,   273,   274,   275,     0,     0,   276,
       0,     0,   277,     1,   636,   569,   637,   638,   639,     6,
     640,   641,   642,    10,   576,   577,   643,   579,   644,   581,
     582,   583,    13,   584,     0,     0,    14,    15,    16,    17,
      18,    19,     0,    20,     0,     0,    23,    24,    25,    26,
       0,    27,    28,    29,   404,   405,    32,   406,    34,    35,
      36,    37,    38,    39,    40,     0,     0,     0,    42,   407,
     408,    45,     0,    47,     0,     0,    49,     0,   409,    51,
      52,    53,    54,    55,    56,    57,     0,    58,    59,    60,
      61,    62,   410,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,     0,     0,    74,    75,    76,     0,     0,
      78,    79,    80,    81,    82,     0,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,     0,
       0,     0,     0,     0,    97,     0,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,     0,   108,   109,   110,
       0,     0,   111,   112,   113,   114,   115,   116,   117,   118,
     119,     0,   120,   121,     0,   122,   123,   124,     0,     0,
     125,   126,     0,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,     0,   139,     0,     0,     0,     0,
       0,   140,     0,     0,     0,     0,     0,   141,   142,   143,
     144,   145,     0,   147,   148,   149,   150,   151,   152,   153,
     154,   155,   156,   157,   158,     0,   160,   161,   162,   163,
     164,   165,   166,   167,   168,   169,   170,   171,   172,   173,
     174,   175,   176,     0,   177,   178,   179,   180,   181,   182,
     183,   184,   185,   186,   187,   188,   189,     0,   190,   191,
     192,   193,   194,   195,   196,     0,   197,   198,   199,   200,
     201,   202,   203,   204,   205,   206,   207,   208,   209,   210,
     211,   212,   411,   214,   215,   216,   217,   412,   219,   220,
     221,   222,   223,   224,     0,   225,   226,   227,   228,   229,
     230,   231,   232,   233,   234,   235,   236,   237,   238,   239,
     240,   241,   242,   243,     0,   645,   246,   247,   248,   249,
     250,   413,   252,   253,   254,   255,   256,   257,   258,   259,
     260,     0,   262,   263,   264,   265,   266,   267,   268,   269,
     270,   271,   272,   273,   274,   275,     0,     0,     0,     1,
       2,   277,     3,     4,     5,     6,     7,     8,     9,    10,
       0,     0,    11,     0,    12,   671,     0,     0,    13,     0,
       0,     0,    14,    15,    16,    17,    18,    19,     0,    20,
       0,     0,    23,    24,    25,    26,     0,    27,    28,    29,
     404,   405,    32,   406,    34,    35,    36,    37,    38,    39,
      40,     0,     0,     0,    42,   407,   408,    45,     0,    47,
       0,     0,    49,     0,   409,    51,    52,    53,    54,    55,
      56,    57,     0,    58,    59,    60,    61,    62,   410,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,     0,
       0,    74,    75,    76,     0,     0,    78,    79,    80,    81,
      82,     0,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,     0,     0,     0,   672,     0,
      97,     0,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,     0,   108,   109,   110,     0,     0,   111,   112,
     113,   114,   115,   116,   117,   118,   119,     0,   120,   121,
       0,   122,   123,   124,     0,     0,   125,   126,     0,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
       0,   139,     0,     0,     0,     0,     0,   140,     0,     0,
       0,     0,     0,   141,   142,   143,   144,   145,     0,   147,
     148,   149,   150,   151,   152,   153,   154,   155,   156,   157,
     158,     0,   160,   161,   162,   163,   164,   165,   166,   167,
     168,   169,   170,   171,   172,   173,   174,   175,   176,     0,
     177,   178,   179,   180,   181,   182,   183,   184,   185,   186,
     187,   188,   189,     0,   190,   191,   192,   193,   194,   195,
     196,   673,   674,   675,   676,   677,   678,   679,   203,   204,
     205,   206,   207,   208,   209,   210,   211,   212,   411,   214,
     215,   216,   217,   412,   219,   220,   221,   222,   223,   224,
       0,   225,   226,   227,   228,   229,   230,   231,   232,   233,
     234,   235,   236,   237,   238,   239,   240,   241,   242,   243,
       0,   245,   246,   247,   248,   249,   250,   413,   252,   253,
     254,   255,   256,   257,   258,   259,   260,     0,   262,   263,
     264,   265,   266,   267,   268,   269,   270,   271,   272,   273,
     274,   275,     0,     0,     0,     1,     2,   277,     3,     4,
       5,     6,     7,     8,     9,    10,     0,     0,    11,     0,
      12,     0,     0,     0,    13,     0,     0,     0,    14,    15,
      16,    17,    18,    19,     0,    20,     0,     0,    23,    24,
      25,   419,     0,    27,    28,    29,   404,   405,    32,   406,
      34,    35,    36,    37,    38,    39,    40,     0,     0,     0,
      42,   407,   408,    45,     0,    47,     0,     0,    49,     0,
     409,    51,    52,    53,    54,    55,    56,    57,     0,    58,
      59,    60,    61,    62,   410,   420,    65,    66,   421,    68,
     422,    70,    71,   423,    73,     0,     0,    74,    75,    76,
       0,     0,    78,    79,    80,    81,    82,     0,    84,   424,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,   425,     0,     0,     0,     0,    97,     0,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,     0,   108,
     109,   110,     0,     0,   111,   112,   113,   114,   115,   116,
     117,   118,   119,     0,   120,   121,     0,   122,   123,   124,
       0,     0,   426,   126,     0,   427,   129,   130,   131,   132,
     133,   134,   135,   136,   137,   138,     0,   139,     0,     0,
       0,     0,     0,   140,     0,     0,     0,     0,     0,   141,
     142,   143,   144,   145,     0,   147,   148,   149,   150,   151,
     152,   428,   154,   155,   156,   157,   158,     0,   160,   429,
     162,   163,   164,   165,   166,   167,   168,   169,   170,   430,
     431,   173,   174,   175,   176,     0,   177,   178,   179,   180,
     181,   182,   183,   184,   185,   186,   187,   432,   189,   433,
     190,   191,   192,   193,   194,   195,   196,     0,   197,   198,
     199,   200,   201,   202,   203,   204,   205,   434,   207,   208,
     209,   210,   211,   212,   411,   214,   215,   435,   217,   412,
     219,   220,   436,   437,   223,   224,     0,   225,   438,   439,
     228,   229,   230,   231,   232,   233,   234,   235,   440,   441,
     442,   443,   240,   241,   444,   445,     0,   245,   446,   247,
     248,   249,   250,   413,   252,   253,   254,   255,   256,   257,
     258,   259,   260,     0,   262,   263,   264,   265,   266,   267,
     268,   269,   270,   271,   272,   273,   274,   447,     0,     0,
       0,     1,     2,   277,     3,     4,     5,     6,     7,     8,
       9,    10,     0,     0,    11,     0,    12,     0,     0,     0,
      13,     0,     0,     0,    14,    15,    16,    17,    18,    19,
       0,    20,     0,     0,    23,    24,    25,    26,     0,    27,
      28,    29,   404,   405,    32,   406,    34,    35,    36,    37,
      38,    39,    40,     0,     0,     0,    42,   407,   408,    45,
       0,    47,     0,     0,    49,     0,   409,    51,    52,    53,
      54,    55,    56,    57,     0,    58,    59,    60,    61,    62,
     410,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,     0,     0,    74,    75,    76,     0,     0,    78,    79,
      80,    81,    82,     0,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,     0,     0,     0,
       0,     0,    97,     0,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,  -304,   108,   109,   110,     0,     0,
     111,   112,   113,   114,   115,   116,   117,   118,   119,     0,
     120,   121,  -304,   122,   123,   124,     0,     0,   125,   126,
       0,   128,   129,   130,   131,   132,   133,   134,   135,   136,
     137,   138,     0,   139,     0,     0,     0,     0,     0,   140,
       0,     0,     0,     0,     0,   141,   142,   143,   144,   145,
       0,   147,   148,   149,   150,   151,   152,   153,   154,   155,
     156,   157,   158,     0,   160,   161,   162,   163,   164,   165,
     166,   167,   168,   169,   170,   171,   172,   173,   174,   175,
     176,     0,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189,     0,   190,   191,   192,   193,
     194,   195,   196,     0,   197,   198,   199,   200,   201,   202,
     203,   204,   205,   206,   207,   208,   209,   210,   211,   212,
     411,   214,   215,   216,   217,   412,   219,   220,   221,   222,
     223,   224,     0,   225,   226,   227,   228,   229,   230,   231,
     232,   233,   234,   235,   236,   237,   238,   239,   240,   241,
     242,   243,     0,   245,   246,   247,   248,   249,   250,   413,
     252,   253,   254,   255,   256,   257,   258,   259,   260,     0,
     262,   263,   264,   265,   266,   267,   268,   269,   270,   271,
     272,   273,   274,   275,     0,     0,     0,     1,     2,   277,
       3,     4,     5,     6,     7,     8,     9,    10,     0,     0,
      11,     0,    12,   996,     0,     0,    13,     0,     0,     0,
      14,    15,    16,    17,    18,    19,     0,    20,     0,     0,
      23,    24,    25,    26,     0,    27,    28,    29,   404,   405,
      32,   406,    34,    35,    36,    37,    38,    39,    40,     0,
       0,     0,    42,   407,   408,    45,     0,    47,     0,     0,
      49,     0,   409,    51,    52,    53,    54,    55,    56,    57,
       0,    58,    59,    60,    61,    62,   410,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,     0,     0,    74,
      75,    76,     0,     0,    78,    79,    80,    81,    82,     0,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,     0,     0,     0,     0,     0,    97,     0,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
       0,   108,   109,   110,     0,     0,   111,   112,   113,   114,
     115,   116,   117,   118,   119,     0,   120,   121,     0,   122,
     123,   124,     0,     0,   125,   126,     0,   128,   129,   130,
     131,   132,   133,   134,   135,   136,   137,   138,     0,   139,
       0,     0,     0,     0,     0,   140,     0,     0,     0,     0,
       0,   141,   142,   143,   144,   145,     0,   147,   148,   149,
     150,   151,   152,   153,   154,   155,   156,   157,   158,     0,
     160,   161,   162,   163,   164,   165,   166,   167,   168,   169,
     170,   171,   172,   173,   174,   175,   176,     0,   177,   178,
     179,   180,   181,   182,   183,   184,   185,   186,   187,   188,
     189,     0,   190,   191,   192,   193,   194,   195,   196,     0,
     197,   198,   199,   200,   201,   202,   203,   204,   205,   206,
     207,   208,   209,   210,   211,   212,   411,   214,   215,   216,
     217,   412,   219,   220,   221,   222,   223,   224,     0,   225,
     226,   227,   228,   229,   230,   231,   232,   233,   234,   235,
     236,   237,   238,   239,   240,   241,   242,   243,     0,   245,
     246,   247,   248,   249,   250,   413,   252,   253,   254,   255,
     256,   257,   258,   259,   260,     0,   262,   263,   264,   265,
     266,   267,   268,   269,   270,   271,   272,   273,   274,   275,
       0,     0,     0,     1,     2,   277,     3,     4,     5,     6,
       7,     8,     9,    10,     0,     0,    11,     0,    12,   671,
       0,     0,    13,     0,     0,     0,    14,    15,    16,    17,
      18,    19,     0,    20,     0,     0,    23,    24,    25,    26,
       0,    27,    28,    29,   404,   405,    32,   406,    34,    35,
      36,    37,    38,    39,    40,     0,     0,     0,    42,   407,
     408,    45,     0,    47,     0,     0,    49,     0,   409,    51,
      52,    53,    54,    55,    56,    57,     0,    58,    59,    60,
      61,    62,   410,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,     0,     0,    74,    75,    76,     0,     0,
      78,    79,    80,    81,    82,     0,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,     0,
       0,     0,     0,     0,    97,     0,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,     0,   108,   109,   110,
       0,     0,   111,   112,   113,   114,   115,   116,   117,   118,
     119,     0,   120,   121,     0,   122,   123,   124,     0,     0,
     125,   126,     0,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,     0,   139,     0,     0,     0,     0,
       0,   140,     0,     0,     0,     0,     0,   141,   142,   143,
     144,   145,     0,   147,   148,   149,   150,   151,   152,   153,
     154,   155,   156,   157,   158,     0,   160,   161,   162,   163,
     164,   165,   166,   167,   168,   169,   170,   171,   172,   173,
     174,   175,   176,     0,   177,   178,   179,   180,   181,   182,
     183,   184,   185,   186,   187,   188,   189,     0,   190,   191,
     192,   193,   194,   195,   196,     0,   197,   198,   199,   200,
     201,   202,   203,   204,   205,   206,   207,   208,   209,   210,
     211,   212,   411,   214,   215,   216,   217,   412,   219,   220,
     221,   222,   223,   224,     0,   225,   226,   227,   228,   229,
     230,   231,   232,   233,   234,   235,   236,   237,   238,   239,
     240,   241,   242,   243,     0,   245,   246,   247,   248,   249,
     250,   413,   252,   253,   254,   255,   256,   257,   258,   259,
     260,     0,   262,   263,   264,   265,   266,   267,   268,   269,
     270,   271,   272,   273,   274,   275,     0,     0,     0,     1,
       2,   277,     3,     4,     5,     6,     7,     8,     9,    10,
       0,     0,    11,     0,    12,     0,     0,     0,    13,     0,
       0,     0,    14,    15,    16,    17,    18,    19,     0,    20,
       0,     0,    23,    24,    25,    26,     0,    27,    28,    29,
     404,   405,    32,   406,    34,    35,    36,    37,    38,    39,
      40,     0,     0,     0,    42,   407,   408,    45,   810,    47,
       0,     0,    49,     0,   409,    51,    52,    53,    54,    55,
      56,    57,     0,    58,    59,    60,    61,    62,   410,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,     0,
       0,    74,    75,    76,     0,     0,    78,    79,    80,    81,
      82,     0,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,     0,     0,     0,     0,     0,
      97,     0,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,     0,   108,   109,   110,     0,     0,   111,   112,
     113,   114,   115,   116,   117,   118,   119,     0,   120,   121,
       0,   122,   123,   124,     0,     0,   125,   126,     0,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
       0,   139,     0,     0,     0,     0,     0,   140,     0,     0,
       0,     0,     0,   141,   142,   143,   144,   145,     0,   147,
     148,   149,   150,   151,   152,   153,   154,   155,   156,   157,
     158,     0,   160,   161,   162,   163,   164,   165,   166,   167,
     168,   169,   170,   171,   172,   173,   174,   175,   176,     0,
     177,   178,   179,   180,   181,   182,   183,   184,   185,   186,
     187,   188,   189,     0,   190,   191,   192,   193,   194,   195,
     196,     0,   197,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,   208,   209,   210,   211,   212,   411,   214,
     215,   216,   217,   412,   219,   220,   221,   222,   223,   224,
       0,   225,   226,   227,   228,   229,   230,   231,   232,   233,
     234,   235,   236,   237,   238,   239,   240,   241,   242,   243,
       0,   245,   246,   247,   248,   249,   250,   413,   252,   253,
     254,   255,   256,   257,   258,   259,   260,     0,   262,   263,
     264,   265,   266,   267,   268,   269,   270,   271,   272,   273,
     274,   275,     0,     0,     0,     1,     2,   277,     3,     4,
       5,     6,     7,     8,     9,    10,     0,     0,    11,     0,
      12,   671,     0,     0,    13,     0,     0,     0,    14,    15,
      16,    17,    18,    19,     0,    20,     0,     0,    23,    24,
      25,    26,     0,    27,    28,    29,   404,   405,    32,   406,
      34,    35,    36,    37,    38,    39,    40,     0,     0,     0,
      42,   407,   408,    45,     0,    47,     0,     0,    49,     0,
     409,    51,  1542,    53,    54,    55,    56,    57,     0,    58,
      59,    60,    61,    62,   410,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,     0,     0,    74,    75,  1543,
       0,     0,    78,    79,    80,    81,    82,     0,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,     0,     0,     0,     0,     0,    97,     0,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,     0,   108,
     109,   110,     0,     0,   111,   112,   113,   114,   115,   116,
     117,   118,   119,     0,   120,   121,     0,   122,   123,   124,
       0,     0,   125,   126,     0,   128,   129,   130,   131,   132,
     133,   134,   135,   136,   137,   138,     0,   139,     0,     0,
       0,     0,     0,   140,     0,     0,     0,     0,     0,   141,
     142,   143,   144,   145,     0,   147,   148,   149,   150,   151,
     152,   153,   154,   155,   156,   157,   158,     0,   160,   161,
     162,   163,   164,   165,   166,   167,   168,   169,   170,   171,
     172,   173,   174,   175,   176,     0,   177,   178,   179,   180,
     181,   182,   183,   184,   185,   186,   187,   188,   189,     0,
     190,   191,   192,   193,   194,   195,   196,     0,   197,   198,
     199,   200,   201,   202,   203,   204,   205,   206,   207,   208,
     209,   210,   211,   212,   411,   214,   215,   216,   217,   412,
     219,   220,   221,   222,   223,   224,     0,   225,   226,   227,
     228,   229,   230,   231,   232,   233,   234,   235,   236,   237,
     238,   239,   240,   241,   242,   243,     0,   245,   246,   247,
     248,   249,   250,   413,   252,   253,   254,   255,   256,   257,
     258,   259,   260,     0,   262,   263,   264,   265,   266,   267,
     268,   269,   270,   271,   272,   273,   274,   275,     0,     0,
       0,     1,     2,   277,     3,     4,     5,     6,     7,     8,
       9,    10,     0,     0,    11,     0,    12,     0,     0,     0,
      13,     0,     0,     0,    14,    15,    16,    17,    18,    19,
       0,    20,     0,     0,    23,    24,    25,    26,     0,    27,
      28,    29,   404,   405,    32,   406,    34,    35,    36,    37,
      38,    39,    40,     0,     0,     0,    42,   407,   408,    45,
       0,    47,     0,     0,    49,     0,   409,    51,    52,    53,
      54,    55,    56,    57,     0,    58,    59,    60,    61,    62,
     410,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,     0,     0,    74,    75,    76,     0,     0,    78,    79,
      80,    81,    82,     0,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,     0,     0,     0,
       0,     0,    97,     0,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,     0,   108,   109,   110,     0,     0,
     111,   112,   113,   114,   115,   116,   117,   118,   119,     0,
     120,   121,     0,   122,   123,   124,     0,     0,   125,   126,
       0,   128,   129,   130,   131,   132,   133,   134,   135,   136,
     137,   138,     0,   139,     0,     0,     0,     0,     0,   140,
       0,     0,     0,     0,     0,   141,   142,   143,   144,   145,
       0,   147,   148,   149,   150,   151,   152,   153,   154,   155,
     156,   157,   158,     0,   160,   161,   162,   163,   164,   165,
     166,   167,   168,   169,   170,   171,   172,   173,   174,   175,
     176,     0,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189,     0,   190,   191,   192,   193,
     194,   195,   196,  1670,   197,   198,   199,   200,   201,   202,
     203,   204,   205,   206,   207,   208,   209,   210,   211,   212,
     411,   214,   215,   216,   217,   412,   219,   220,   221,   222,
     223,   224,     0,   225,   226,   227,   228,   229,   230,   231,
     232,   233,   234,   235,   236,   237,   238,   239,   240,   241,
     242,   243,     0,   245,   246,   247,   248,   249,   250,   413,
     252,   253,   254,   255,   256,   257,   258,   259,   260,     0,
     262,   263,   264,   265,   266,   267,   268,   269,   270,   271,
     272,   273,   274,   275,     0,     0,     0,     1,     2,   277,
       3,     4,     5,     6,     7,     8,     9,    10,     0,     0,
      11,     0,    12,   671,     0,     0,    13,     0,     0,     0,
      14,    15,    16,    17,    18,    19,     0,    20,     0,     0,
      23,    24,    25,    26,     0,    27,    28,    29,   404,   405,
      32,   406,    34,    35,    36,    37,    38,    39,    40,     0,
       0,     0,    42,   407,   408,    45,     0,    47,     0,     0,
      49,     0,   409,    51,    52,    53,    54,    55,    56,    57,
       0,    58,    59,    60,    61,    62,   410,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,     0,     0,    74,
      75,  1678,     0,     0,    78,    79,    80,    81,    82,     0,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,     0,     0,     0,     0,     0,    97,     0,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
       0,   108,   109,   110,     0,     0,   111,   112,   113,   114,
     115,   116,   117,   118,   119,     0,   120,   121,     0,   122,
     123,   124,     0,     0,   125,   126,     0,   128,   129,   130,
     131,   132,   133,   134,   135,   136,   137,   138,     0,   139,
       0,     0,     0,     0,     0,   140,     0,     0,     0,     0,
       0,   141,   142,   143,   144,   145,     0,   147,   148,   149,
     150,   151,   152,   153,   154,   155,   156,   157,   158,     0,
     160,   161,   162,   163,   164,   165,   166,   167,   168,   169,
     170,   171,   172,   173,   174,   175,   176,     0,   177,   178,
     179,   180,   181,   182,   183,   184,   185,   186,   187,   188,
     189,     0,   190,   191,   192,   193,   194,   195,   196,     0,
     197,   198,   199,   200,   201,   202,   203,   204,   205,   206,
     207,   208,   209,   210,   211,   212,   411,   214,   215,   216,
     217,   412,   219,   220,   221,   222,   223,   224,     0,   225,
     226,   227,   228,   229,   230,   231,   232,   233,   234,   235,
     236,   237,   238,   239,   240,   241,   242,   243,     0,   245,
     246,   247,   248,   249,   250,   413,   252,   253,   254,   255,
     256,   257,   258,   259,   260,     0,   262,   263,   264,   265,
     266,   267,   268,   269,   270,   271,   272,   273,   274,   275,
       0,     0,     0,     1,     2,   277,     3,     4,     5,     6,
       7,     8,     9,    10,     0,     0,    11,     0,    12,     0,
       0,     0,    13,     0,     0,     0,    14,    15,    16,    17,
      18,    19,     0,    20,     0,     0,    23,    24,    25,    26,
       0,    27,    28,    29,   404,   405,    32,   406,    34,    35,
      36,    37,    38,    39,    40,     0,     0,     0,    42,   407,
     408,    45,     0,    47,     0,     0,    49,     0,   409,    51,
      52,    53,    54,    55,    56,    57,     0,    58,    59,    60,
      61,    62,   410,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,     0,     0,    74,    75,    76,     0,     0,
      78,    79,    80,    81,    82,     0,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,     0,
       0,     0,     0,     0,    97,     0,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,     0,   108,   109,   110,
       0,     0,   111,   112,   113,   114,   115,   116,   117,   118,
     119,     0,   120,   121,     0,   122,   123,   124,     0,     0,
     125,   126,     0,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,     0,   139,     0,     0,     0,     0,
       0,   140,     0,     0,     0,     0,     0,   141,   142,   143,
     144,   145,     0,   147,   148,   149,   150,   151,   152,   153,
     154,   155,   156,   157,   158,     0,   160,   161,   162,   163,
     164,   165,   166,   167,   168,   169,   170,   171,   172,   173,
     174,   175,   176,     0,   177,   178,   179,   180,   181,   182,
     183,   184,   185,   186,   187,   188,   189,     0,   190,   191,
     192,   193,   194,   195,   196,     0,   197,   198,   199,   200,
     201,   202,   203,   204,   205,   206,   207,   208,   209,   210,
     211,   212,   411,   214,   215,   216,   217,   412,   219,   220,
     221,   222,   223,   224,     0,   225,   226,   227,   228,   229,
     230,   231,   232,   233,   234,   235,   236,   237,   238,   239,
     240,   241,   242,   243,     0,   245,   246,   247,   248,   249,
     250,   413,   252,   253,   254,   255,   256,   257,   258,   259,
     260,     0,   262,   263,   264,   265,   266,   267,   268,   269,
     270,   271,   272,   273,   274,   275,     0,     0,     0,     1,
       2,   277,     3,     4,     5,     6,     7,     8,     9,    10,
       0,     0,    11,     0,    12,     0,     0,     0,    13,     0,
       0,     0,    14,    15,    16,    17,    18,    19,     0,    20,
       0,     0,    23,    24,    25,    26,     0,    27,    28,    29,
     404,   405,    32,   406,    34,    35,    36,    37,    38,    39,
      40,     0,     0,     0,    42,   407,   408,    45,     0,    47,
       0,     0,    49,     0,   409,    51,    52,    53,    54,    55,
      56,    57,     0,    58,    59,    60,    61,    62,   410,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,     0,
       0,    74,    75,    76,     0,     0,    78,    79,    80,    81,
      82,     0,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,     0,     0,     0,     0,     0,
      97,     0,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,     0,   108,   109,   110,     0,     0,   111,   112,
     113,   114,   115,   116,   117,   118,   119,     0,   120,   121,
       0,   122,   123,   124,     0,     0,   125,   126,     0,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
       0,   139,     0,     0,     0,     0,     0,   140,     0,     0,
       0,     0,     0,   141,   142,   143,   144,   145,     0,   147,
     148,   149,   150,   151,   152,   153,   154,   155,   156,   157,
     158,     0,   160,   161,   162,   163,   164,   165,   166,   167,
     168,   169,   170,   171,   172,   173,   174,   175,   176,     0,
     177,   178,   179,   180,   181,   182,   183,   184,   185,   186,
     187,   188,   189,     0,   190,   191,   192,   193,   194,   195,
     196,     0,   197,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,   208,   209,   210,   211,   212,   411,   214,
     215,   216,   217,   412,   219,   220,   221,   457,   223,   224,
       0,   225,   226,   227,   228,   229,   230,   231,   232,   233,
     234,   235,   236,   237,   238,   239,   240,   241,   242,   243,
       0,   245,   246,   247,   248,   249,   250,   413,   252,   253,
     254,   255,   256,   257,   258,   259,   260,     0,   262,   263,
     264,   265,   266,   267,   268,   269,   270,   271,   272,   273,
     274,   275,     0,     0,     0,     1,     2,   277,   865,     4,
     866,     6,     7,     8,     9,    10,     0,     0,    11,     0,
      12,     0,     0,     0,    13,     0,     0,     0,    14,    15,
      16,    17,    18,    19,     0,    20,     0,     0,    23,    24,
      25,    26,     0,    27,    28,    29,   404,   405,    32,   406,
      34,    35,    36,    37,    38,    39,    40,     0,     0,     0,
      42,   407,   408,    45,     0,    47,     0,     0,    49,     0,
     409,    51,    52,    53,    54,    55,    56,    57,     0,    58,
      59,    60,    61,    62,   410,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,     0,     0,    74,    75,    76,
       0,     0,    78,    79,    80,    81,    82,     0,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,     0,     0,     0,     0,     0,    97,     0,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,     0,   108,
     109,   110,     0,     0,   111,   112,   113,   114,   115,   116,
     117,   118,   119,     0,   120,   121,     0,   122,   123,   124,
       0,     0,   125,   126,     0,   128,   129,   130,   131,   132,
     133,   134,   135,   136,   137,   138,     0,   139,     0,     0,
       0,     0,     0,   140,     0,     0,     0,     0,     0,   141,
     142,   143,   144,   145,     0,   147,   148,   149,   150,   151,
     152,   153,   154,   155,   156,   157,   158,     0,   160,   161,
     162,   163,   164,   165,   166,   167,   168,   169,   170,   171,
     172,   173,   174,   175,   176,     0,   177,   178,   179,   180,
     181,   182,   183,   184,   185,   186,   187,   188,   189,     0,
     190,   191,   192,   193,   194,   195,   196,     0,   197,   198,
     199,   200,   201,   202,   203,   204,   205,   206,   207,   208,
     209,   210,   211,   212,   411,   214,   215,   216,   217,   412,
     219,   220,   221,   222,   223,   224,     0,   225,   226,   227,
     228,   229,   230,   231,   232,   233,   234,   235,   236,   237,
     238,   239,   240,   241,   242,   243,     0,   245,   246,   247,
     248,   249,   250,   413,   252,   253,   254,   255,   256,   257,
     258,   259,   260,     0,   262,   263,   264,   265,   266,   267,
     268,   269,   270,   271,   272,   273,   274,   275,     0,     0,
       0,     1,     2,   277,     3,     4,     5,     6,     7,     8,
       9,    10,     0,     0,    11,     0,    12,     0,     0,     0,
      13,     0,     0,     0,    14,    15,    16,    17,    18,    19,
       0,    20,     0,     0,    23,    24,    25,    26,     0,    27,
      28,    29,   404,   405,    32,   406,    34,    35,    36,    37,
      38,    39,    40,     0,     0,     0,    42,   407,   408,    45,
       0,    47,     0,     0,    49,     0,   409,    51,    52,    53,
      54,    55,    56,    57,     0,    58,    59,    60,    61,    62,
     410,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,     0,     0,    74,    75,    76,     0,     0,    78,    79,
      80,    81,    82,     0,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,     0,     0,     0,
       0,     0,    97,     0,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,     0,   108,   109,   110,     0,     0,
     111,   112,   113,   114,   115,   116,   117,   118,   119,     0,
     120,   121,     0,   122,   123,   124,     0,     0,   125,   126,
       0,   128,   129,   130,   131,   132,   133,   134,   135,   136,
     137,   138,     0,   139,     0,     0,     0,     0,     0,   140,
       0,     0,     0,     0,     0,   141,   142,   143,   144,   145,
       0,   147,   148,   149,   150,   151,   152,   153,   154,   155,
     156,   157,   158,     0,   160,   161,   162,   163,   164,   165,
     166,   167,   168,   169,   170,   171,   172,   173,   174,   175,
     176,     0,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189,     0,   190,   191,   192,   193,
     194,   195,   196,     0,   197,   198,   199,   200,   201,   202,
     203,   204,   205,   206,   207,   208,   209,   210,   211,   212,
     411,   214,   215,   216,   217,   412,   219,   220,   221,   222,
     223,   224,     0,   225,   226,   227,   228,   229,   230,   231,
     232,   233,   234,   235,   236,   237,   238,   239,   240,   241,
     242,   243,     0,   245,   246,   247,   248,   249,   250,   413,
     252,   253,   254,   255,   256,   257,   258,   259,   260,     0,
     888,   263,   264,   265,   266,   267,   268,   269,   270,   271,
     272,   273,   274,   275,     0,     0,     0,     1,     2,   277,
       3,     4,     5,     6,     7,     8,     9,    10,     0,     0,
      11,     0,    12,     0,     0,     0,    13,     0,     0,     0,
      14,    15,    16,  1074,    18,    19,     0,    20,     0,     0,
      23,    24,    25,    26,     0,    27,    28,    29,   404,   405,
      32,   406,    34,    35,    36,    37,    38,    39,    40,     0,
       0,     0,    42,   407,   408,    45,     0,    47,     0,     0,
      49,     0,   409,    51,    52,    53,    54,    55,    56,    57,
       0,    58,    59,    60,    61,    62,   410,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,     0,     0,    74,
      75,    76,     0,     0,    78,    79,    80,    81,    82,     0,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,     0,     0,     0,     0,     0,    97,     0,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
       0,   108,   109,   110,     0,     0,   111,   112,   113,   114,
     115,   116,   117,   118,   119,     0,   120,   121,     0,   122,
     123,   124,     0,     0,   125,   126,     0,   128,   129,   130,
     131,   132,   133,   134,   135,   136,   137,   138,     0,   139,
       0,     0,     0,     0,     0,   140,     0,     0,     0,     0,
       0,   141,   142,   143,   144,   145,     0,   147,   148,   149,
     150,   151,   152,   153,   154,   155,   156,   157,   158,     0,
     160,   161,   162,   163,   164,   165,   166,   167,   168,   169,
     170,   171,   172,   173,   174,   175,   176,     0,   177,   178,
     179,   180,   181,   182,   183,   184,   185,   186,   187,   188,
     189,     0,   190,   191,   192,   193,   194,   195,   196,     0,
     197,   198,   199,   200,   201,   202,   203,   204,   205,   206,
     207,   208,   209,   210,   211,   212,   411,   214,   215,   216,
     217,   412,   219,   220,   221,   222,   223,   224,     0,   225,
     226,   227,   228,   229,   230,   231,   232,   233,   234,   235,
     236,   237,   238,   239,   240,   241,   242,   243,     0,   245,
     246,   247,   248,   249,   250,   413,   252,   253,   254,   255,
     256,   257,   258,   259,   260,     0,   262,   263,   264,   265,
     266,   267,   268,   269,   270,   271,   272,   273,   274,   275,
       0,     0,     0,     1,     2,  1075,     3,     4,     5,     6,
       7,     8,     9,    10,     0,     0,    11,     0,    12,     0,
       0,     0,    13,     0,     0,     0,    14,    15,    16,    17,
      18,    19,     0,    20,     0,     0,    23,    24,    25,    26,
       0,    27,    28,    29,   404,   405,    32,   406,    34,    35,
      36,    37,    38,    39,    40,     0,     0,     0,    42,   407,
     408,    45,     0,    47,     0,     0,    49,     0,   409,    51,
      52,    53,    54,    55,    56,    57,     0,    58,    59,    60,
      61,    62,   410,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,     0,     0,    74,    75,    76,     0,     0,
      78,    79,    80,    81,    82,     0,    84,    85,  1373,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,     0,
       0,     0,     0,     0,    97,     0,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,     0,   108,   109,   110,
       0,     0,   111,   112,   113,   114,   115,   116,   117,   118,
     119,     0,   120,   121,     0,   122,   123,   124,     0,     0,
     125,   126,     0,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,     0,   139,     0,     0,     0,     0,
       0,   140,     0,     0,     0,     0,     0,   141,   142,   143,
     144,   145,     0,   147,   148,   149,   150,   151,   152,   153,
     154,   155,   156,   157,   158,     0,   160,   161,   162,   163,
     164,   165,   166,   167,   168,   169,   170,   171,   172,   173,
     174,   175,   176,     0,   177,   178,   179,   180,   181,   182,
     183,   184,   185,   186,   187,   188,   189,     0,   190,   191,
     192,   193,   194,   195,   196,     0,   197,   198,   199,   200,
     201,   202,   203,   204,   205,   206,   207,   208,   209,   210,
     211,   212,   411,   214,   215,   216,   217,   412,   219,   220,
     221,   222,   223,   224,     0,   225,   226,   227,   228,   229,
     230,   231,   232,   233,   234,   235,   236,   237,   238,   239,
     240,   241,   242,   243,     0,   245,   246,   247,   248,   249,
     250,   413,   252,   253,   254,   255,   256,   257,   258,   259,
     260,     0,   262,   263,   264,   265,   266,   267,   268,   269,
     270,   271,   272,   273,   274,   275,     0,     0,     0,     1,
       2,   277,     3,     4,     5,     6,     7,     8,     9,    10,
       0,     0,    11,     0,    12,     0,     0,     0,    13,     0,
       0,     0,    14,    15,    16,    17,    18,    19,     0,    20,
       0,     0,    23,    24,    25,    26,     0,    27,    28,    29,
     404,   405,    32,   406,    34,    35,    36,    37,    38,    39,
      40,     0,     0,     0,    42,   407,   408,    45,     0,    47,
       0,     0,    49,     0,   409,    51,    52,    53,    54,    55,
      56,    57,     0,    58,    59,    60,    61,    62,   410,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,     0,
       0,    74,    75,    76,     0,     0,    78,    79,    80,    81,
      82,     0,    84,    85,  1550,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,     0,     0,     0,     0,     0,
      97,     0,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,     0,   108,   109,   110,     0,     0,   111,   112,
     113,   114,   115,   116,   117,   118,   119,     0,   120,   121,
       0,   122,   123,   124,     0,     0,   125,   126,     0,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
       0,   139,     0,     0,     0,     0,     0,   140,     0,     0,
       0,     0,     0,   141,   142,   143,   144,   145,     0,   147,
     148,   149,   150,   151,   152,   153,   154,   155,   156,   157,
     158,     0,   160,   161,   162,   163,   164,   165,   166,   167,
     168,   169,   170,   171,   172,   173,   174,   175,   176,     0,
     177,   178,   179,   180,   181,   182,   183,   184,   185,   186,
     187,   188,   189,     0,   190,   191,   192,   193,   194,   195,
     196,     0,   197,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,   208,   209,   210,   211,   212,   411,   214,
     215,   216,   217,   412,   219,   220,   221,   222,   223,   224,
       0,   225,   226,   227,   228,   229,   230,   231,   232,   233,
     234,   235,   236,   237,   238,   239,   240,   241,   242,   243,
       0,   245,   246,   247,   248,   249,   250,   413,   252,   253,
     254,   255,   256,   257,   258,   259,   260,     0,   262,   263,
     264,   265,   266,   267,   268,   269,   270,   271,   272,   273,
     274,   275,     0,     0,     0,     1,     0,   277,     3,     4,
       5,     6,     7,     8,     9,    10,     0,     0,    11,     0,
      12,     0,     0,     0,    13,     0,     0,     0,    14,    15,
      16,    17,    18,    19,     0,    20,     0,     0,    23,    24,
      25,    26,     0,    27,    28,    29,   404,   405,    32,   406,
      34,    35,    36,    37,    38,    39,    40,     0,     0,     0,
      42,   407,   408,    45,     0,    47,     0,     0,    49,     0,
     409,    51,    52,    53,    54,    55,    56,    57,     0,    58,
      59,    60,    61,    62,   410,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,     0,     0,    74,    75,    76,
       0,     0,    78,    79,    80,    81,    82,     0,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,     0,     0,     0,     0,     0,    97,     0,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,     0,   108,
     109,   110,     0,     0,   111,   112,   113,   114,   115,   116,
     117,   118,   119,     0,   120,   121,     0,   122,   123,   124,
       0,     0,   125,   126,     0,   128,   129,   130,   131,   132,
     133,   134,   135,   136,   137,   138,     0,   139,     0,     0,
       0,     0,     0,   140,     0,     0,     0,     0,     0,   141,
     142,   143,   144,   145,     0,   147,   148,   149,   150,   151,
     152,   153,   154,   155,   156,   157,   158,     0,   160,   161,
     162,   163,   164,   165,   166,   167,   168,   169,   170,   171,
     172,   173,   174,   175,   176,     0,   177,   178,   179,   180,
     181,   182,   183,   184,   185,   186,   187,   188,   189,     0,
     190,   191,   192,   193,   194,   195,   196,     0,   197,   198,
     199,   200,   201,   202,   203,   204,   205,   206,   207,   208,
     209,   210,   211,   212,   411,   214,   215,   216,   217,   412,
     219,   220,   221,   222,   223,   224,     0,   225,   226,   227,
     228,   229,   230,   231,   232,   233,   234,   235,   236,   237,
     238,   239,   240,   241,   242,   243,     0,   245,   246,   247,
     248,   249,   250,   413,   252,   253,   254,   255,   256,   257,
     258,   259,   260,     0,   262,   263,   264,   265,   266,   267,
     268,   269,   270,   271,   272,   273,   274,   275,     0,     0,
       0,     1,     0,   277,  -818,  -818,  -818,     6,     7,     8,
       9,    10,     0,     0,    11,     0,    12,     0,     0,     0,
      13,     0,     0,     0,    14,    15,    16,    17,    18,    19,
       0,    20,     0,     0,    23,    24,    25,    26,     0,    27,
      28,    29,   404,   405,    32,   406,    34,    35,    36,    37,
      38,    39,    40,     0,     0,     0,    42,   407,   408,    45,
       0,    47,     0,     0,    49,     0,   409,    51,    52,    53,
      54,    55,    56,    57,     0,    58,    59,    60,    61,    62,
     410,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,     0,     0,    74,    75,    76,     0,     0,    78,    79,
      80,    81,    82,     0,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,     0,     0,     0,
       0,     0,    97,     0,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,     0,   108,   109,   110,     0,     0,
     111,   112,   113,   114,   115,   116,   117,   118,   119,     0,
     120,   121,     0,   122,   123,   124,     0,     0,   125,   126,
       0,   128,   129,   130,   131,   132,   133,   134,   135,   136,
     137,   138,     0,   139,     0,     0,     0,     0,     0,   140,
       0,     0,     0,     0,     0,   141,   142,   143,   144,   145,
       0,   147,   148,   149,   150,   151,   152,   153,   154,   155,
     156,   157,   158,     0,   160,   161,   162,   163,   164,   165,
     166,   167,   168,   169,   170,   171,   172,   173,   174,   175,
     176,     0,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189,     0,   190,   191,   192,   193,
     194,   195,   196,     0,   197,   198,   199,   200,   201,   202,
     203,   204,   205,   206,   207,   208,   209,   210,   211,   212,
     411,   214,   215,   216,   217,   412,   219,   220,   221,   222,
     223,   224,     0,   225,   226,   227,   228,   229,   230,   231,
     232,   233,   234,   235,   236,   237,   238,   239,   240,   241,
     242,   243,     0,   245,   246,   247,   248,   249,   250,   413,
     252,   253,   254,   255,   256,   257,   258,   259,   260,     0,
     262,   263,   264,   265,   266,   267,   268,   269,   270,   271,
     272,   273,   274,   275,     1,     0,     0,     0,     0,   277,
       6,     0,     8,     9,    10,     0,     0,    11,     0,    12,
       0,     0,     0,    13,     0,     0,     0,    14,    15,    16,
      17,    18,    19,     0,    20,     0,     0,    23,    24,    25,
      26,     0,    27,    28,    29,   404,   405,    32,   406,    34,
      35,    36,    37,    38,    39,    40,     0,     0,     0,    42,
     407,   408,    45,     0,    47,     0,     0,    49,     0,   409,
      51,    52,    53,    54,    55,    56,    57,     0,    58,    59,
      60,    61,    62,   410,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,     0,     0,    74,    75,    76,     0,
       0,    78,    79,    80,    81,    82,     0,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
       0,     0,     0,     0,     0,    97,     0,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,     0,   108,   109,
     110,     0,     0,   111,   112,   113,   114,   115,   116,   117,
     118,   119,     0,   120,   121,     0,   122,   123,   124,     0,
       0,   125,   126,     0,   128,   129,   130,   131,   132,   133,
     134,   135,   136,   137,   138,     0,   139,     0,     0,     0,
       0,     0,   140,     0,     0,     0,     0,     0,   141,   142,
     143,   144,   145,     0,   147,   148,   149,   150,   151,   152,
     153,   154,   155,   156,   157,   158,     0,   160,   161,   162,
     163,   164,   165,   166,   167,   168,   169,   170,   171,   172,
     173,   174,   175,   176,     0,   177,   178,   179,   180,   181,
     182,   183,   184,   185,   186,   187,   188,   189,     0,   190,
     191,   192,   193,   194,   195,   196,     0,   197,   198,   199,
     200,   201,   202,   203,   204,   205,   206,   207,   208,   209,
     210,   211,   212,   411,   214,   215,   216,   217,   412,   219,
     220,   221,   222,   223,   224,     0,   225,   226,   227,   228,
     229,   230,   231,   232,   233,   234,   235,   236,   237,   238,
     239,   240,   241,   242,   243,     0,   245,   246,   247,   248,
     249,   250,   413,   252,   253,   254,   255,   256,   257,   258,
     259,   260,     0,   262,   263,   264,   265,   266,   267,   268,
     269,   270,   271,   272,   273,   274,   275,     1,     0,     0,
       0,     0,   277,     6,     0,     0,     9,    10,     0,     0,
      11,     0,    12,     0,     0,     0,    13,     0,     0,     0,
      14,    15,    16,    17,    18,    19,     0,    20,     0,     0,
      23,    24,    25,    26,     0,    27,    28,    29,   404,   405,
      32,   406,    34,    35,    36,    37,    38,    39,    40,     0,
       0,     0,    42,   407,   408,    45,     0,    47,     0,     0,
      49,     0,   409,    51,    52,    53,    54,    55,    56,    57,
       0,    58,    59,    60,    61,    62,   410,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,     0,     0,    74,
      75,    76,     0,     0,    78,    79,    80,    81,    82,     0,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,     0,     0,     0,     0,     0,    97,     0,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
       0,   108,   109,   110,     0,     0,   111,   112,   113,   114,
     115,   116,   117,   118,   119,     0,   120,   121,     0,   122,
     123,   124,     0,     0,   125,   126,     0,   128,   129,   130,
     131,   132,   133,   134,   135,   136,   137,   138,     0,   139,
       0,     0,     0,     0,     0,   140,     0,     0,     0,     0,
       0,   141,   142,   143,   144,   145,     0,   147,   148,   149,
     150,   151,   152,   153,   154,   155,   156,   157,   158,     0,
     160,   161,   162,   163,   164,   165,   166,   167,   168,   169,
     170,   171,   172,   173,   174,   175,   176,     0,   177,   178,
     179,   180,   181,   182,   183,   184,   185,   186,   187,   188,
     189,     0,   190,   191,   192,   193,   194,   195,   196,     0,
     197,   198,   199,   200,   201,   202,   203,   204,   205,   206,
     207,   208,   209,   210,   211,   212,   411,   214,   215,   216,
     217,   412,   219,   220,   221,   222,   223,   224,     0,   225,
     226,   227,   228,   229,   230,   231,   232,   233,   234,   235,
     236,   237,   238,   239,   240,   241,   242,   243,     0,   245,
     246,   247,   248,   249,   250,   413,   252,   253,   254,   255,
     256,   257,   258,   259,   260,     0,   262,   263,   264,   265,
     266,   267,   268,   269,   270,   271,   272,   273,   274,   275,
       1,     0,     0,     0,     0,   277,     6,     0,     0,     0,
      10,     0,     0,     0,     0,    12,     0,     0,     0,    13,
       0,     0,     0,    14,    15,    16,    17,    18,    19,     0,
      20,     0,     0,    23,    24,    25,    26,     0,    27,    28,
      29,   404,   405,    32,   406,    34,    35,    36,    37,    38,
      39,    40,     0,     0,     0,    42,   407,   408,    45,     0,
      47,     0,     0,    49,     0,   409,    51,    52,    53,    54,
      55,    56,    57,     0,    58,    59,    60,    61,    62,   410,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
       0,     0,    74,    75,    76,     0,     0,    78,    79,    80,
      81,    82,     0,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,     0,     0,     0,     0,
       0,    97,     0,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,     0,   108,   109,   110,     0,     0,   111,
     112,   113,   114,   115,   116,   117,   118,   119,     0,   120,
     121,     0,   122,   123,   124,     0,     0,   125,   126,     0,
     128,   129,   130,   131,   132,   133,   134,   135,   136,   137,
     138,     0,   139,     0,     0,     0,     0,     0,   140,     0,
       0,     0,     0,     0,   141,   142,   143,   144,   145,     0,
     147,   148,   149,   150,   151,   152,   153,   154,   155,   156,
     157,   158,     0,   160,   161,   162,   163,   164,   165,   166,
     167,   168,   169,   170,   171,   172,   173,   174,   175,   176,
       0,   177,   178,   179,   180,   181,   182,   183,   184,   185,
     186,   187,   188,   189,     0,   190,   191,   192,   193,   194,
     195,   196,     0,   197,   198,   199,   200,   201,   202,   203,
     204,   205,   206,   207,   208,   209,   210,   211,   212,   411,
     214,   215,   216,   217,   412,   219,   220,   221,   222,   223,
     224,     0,   225,   226,   227,   228,   229,   230,   231,   232,
     233,   234,   235,   236,   237,   238,   239,   240,   241,   242,
     243,     0,   245,   246,   247,   248,   249,   250,   413,   252,
     253,   254,   255,   256,   257,   258,   259,   260,     0,   262,
     263,   264,   265,   266,   267,   268,   269,   270,   271,   272,
     273,   274,   275,     0,     0,     0,     0,     2,   277,     3,
       4,     5,     6,     7,     8,     9,    10,     0,     0,    11,
       0,    12,     0,     0,     0,    13,     0,     0,     0,    14,
      15,    16,    17,    18,    19,     0,    20,     0,     0,    23,
      24,    25,    26,     0,    27,    28,    29,   404,   405,    32,
     406,    34,    35,    36,    37,    38,    39,    40,     0,     0,
       0,    42,   407,   408,    45,     0,    47,     0,     0,    49,
       0,   409,    51,    52,    53,    54,    55,    56,    57,     0,
      58,    59,    60,    61,    62,   410,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,     0,     0,    74,    75,
      76,     0,     0,    78,    79,    80,    81,    82,     0,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,     0,     0,     0,     0,     0,    97,     0,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   107,     0,
     108,   109,   110,     0,     0,   111,   112,   113,   114,   115,
     116,   117,   118,   119,     0,   120,   121,     0,   122,   123,
     124,     0,     0,   125,   126,     0,   128,   129,   130,   131,
     132,   133,   134,   135,   136,   137,   138,     0,   139,     0,
       0,     0,     0,     0,   140,     0,     0,     0,     0,     0,
     141,   142,   143,   144,   145,     0,   147,   148,   149,   150,
     151,   152,   153,   154,   155,   156,   157,   158,     0,   160,
     161,   162,   163,   164,   165,   166,   167,   168,   169,   170,
     171,   172,   173,   174,   175,   176,     0,   177,   178,   179,
     180,   181,   182,   183,   184,   185,   186,   187,   188,   189,
       0,   190,   191,   192,   193,   194,   195,   196,     0,   197,
     198,   199,   200,   201,   202,   203,   204,   205,   206,   207,
     208,   209,   210,   211,   212,   411,   214,   215,   216,   217,
     412,   219,   220,   221,   222,   223,   224,     0,   225,   226,
     227,   228,   229,   230,   231,   232,   233,   234,   235,   236,
     237,   238,   239,   240,   241,   242,   243,     0,   245,   246,
     247,   248,   249,   250,   413,   252,   253,   254,   255,   256,
     257,   258,   259,   260,     0,   660,   263,   264,   265,   266,
     267,   268,   269,   270,   271,   272,   273,   274,   275,     0,
       0,   459,   460,     2,   277,     3,     4,     5,     6,     7,
       8,     0,     0,     0,     0,    11,     0,     0,     0,     0,
       0,    13,     0,     0,     0,   461,    15,   462,   463,    18,
      19,     0,   464,     0,     0,    23,    24,    25,    26,     0,
      27,    28,    29,   404,   405,    32,   406,    34,    35,    36,
      37,    38,    39,    40,     0,     0,     0,    42,     0,   408,
      45,     0,    47,     0,     0,    49,     0,   409,    51,    52,
      53,    54,    55,    56,    57,     0,    58,    59,    60,    61,
      62,   410,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,     0,     0,    74,    75,    76,     0,     0,    78,
      79,    80,    81,    82,     0,    84,    85,    86,    87,    88,
      89,    90,    91,    92,   465,   466,    95,    96,     0,     0,
       0,     0,     0,    97,     0,    98,    99,   467,   101,   102,
     103,   104,   105,   106,   107,     0,   108,   109,   110,     0,
       0,   111,   112,   113,   114,   115,   116,   468,   469,   119,
       0,   120,   121,     0,   122,   123,   124,     0,     0,   125,
     126,     0,   128,   129,   130,   131,   132,   133,   134,   135,
     136,   137,   138,   470,   139,     0,     0,     0,   471,   472,
     140,   473,   474,   475,   476,     0,   477,   142,   143,   144,
     145,     0,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,     0,   160,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176,     0,   177,   478,   179,   180,   181,   182,   183,
     184,   185,   186,   187,   188,   189,     0,   190,   191,   192,
     193,   194,   195,   196,     0,   197,   198,   199,   200,   201,
     202,   203,   204,   205,   206,   207,   208,   209,   210,   211,
     212,   411,   214,   215,   216,   217,   412,   219,   220,   221,
     222,   223,   224,     0,   225,   226,   227,   228,   229,   230,
     231,   232,   233,   234,   235,   236,   237,   238,   239,   240,
     241,   242,   243,     0,   245,   246,   247,   248,   249,   250,
     413,   252,   253,   254,   255,   256,   257,   258,   259,   260,
       0,   262,   479,   264,   265,   266,   267,   268,   269,   270,
     271,   272,   273,   274,   275,     1,   636,   569,   637,   638,
     639,     6,   640,   641,   575,     0,   576,   577,   643,   579,
     580,   581,   582,   583,    13,   584,     0,     0,   461,    15,
     462,   891,    18,    19,     0,   464,     0,     0,    23,    24,
      25,    26,     0,    27,    28,    29,   404,   405,    32,   406,
      34,    35,    36,    37,    38,    39,    40,     0,     0,     0,
      42,     0,   408,    45,     0,    47,     0,     0,    49,     0,
     409,    51,    52,    53,    54,    55,    56,    57,     0,    58,
     892,    60,    61,    62,   410,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,     0,     0,    74,    75,    76,
       0,     0,    78,    79,    80,    81,    82,     0,    84,    85,
      86,    87,    88,    89,    90,    91,    92,   465,   466,    95,
      96,     0,     0,     0,     0,     0,    97,     0,    98,    99,
     467,   101,   102,   103,   104,   105,   106,   107,     0,   108,
     109,   110,     0,     0,   111,   112,   113,   114,   115,   116,
     468,   118,   119,     0,   120,   121,     0,   122,   123,   124,
       0,     0,   125,   126,     0,   128,   129,   130,   131,   132,
     133,   134,   135,   136,   137,   138,     0,   139,     0,     0,
       0,     0,     0,   140,     0,     0,     0,     0,     0,   141,
     142,   143,   144,   145,     0,   147,   148,   149,   150,   151,
     152,   153,   154,   155,   156,   157,   158,     0,   160,   161,
     162,   163,   164,   165,   166,   167,   168,   169,   170,   171,
     172,   173,   174,   175,   176,     0,   177,   478,   179,   180,
     181,   182,   183,   184,   185,   186,   187,   188,   189,     0,
     190,   191,   192,   193,   194,   195,   196,     0,   197,   198,
     199,   200,   201,   202,   203,   204,   205,   206,   207,   208,
     209,   210,   211,   212,   411,   214,   215,   216,   217,   412,
     219,   220,   221,   222,   223,   224,     0,   225,   226,   227,
     228,   229,   230,   231,   232,   233,   234,   235,   236,   237,
     238,   239,   240,   241,   242,   243,     0,   645,   246,   247,
     248,   249,   250,   413,   252,   253,   254,   255,   256,   257,
     258,   259,   260,     0,   262,   479,   264,   265,   266,   267,
     268,   269,   270,   271,   272,   273,   274,   275,     1,     2,
       0,     3,     4,     5,     6,     7,     8,     0,     0,     0,
       0,    11,     0,     0,     0,     0,     0,    13,     0,     0,
       0,   461,    15,   462,   463,    18,    19,     0,   464,     0,
       0,    23,    24,    25,    26,     0,    27,    28,    29,   404,
     405,    32,   406,    34,    35,    36,    37,    38,    39,    40,
       0,     0,     0,    42,     0,   408,    45,     0,    47,     0,
       0,    49,     0,   409,    51,    52,    53,    54,    55,    56,
      57,     0,    58,    59,    60,    61,    62,   410,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,     0,     0,
      74,    75,    76,     0,     0,    78,    79,    80,    81,    82,
       0,    84,    85,    86,    87,    88,    89,    90,    91,    92,
     465,   466,    95,    96,     0,     0,     0,     0,     0,    97,
       0,    98,    99,   467,   101,   102,   103,   104,   105,   106,
     107,     0,   108,   109,   110,     0,     0,   111,   112,   113,
     114,   115,   116,   468,   469,   119,     0,   120,   121,     0,
     122,   123,   124,     0,     0,   125,   126,     0,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   137,   138,   470,
     139,     0,     0,     0,   471,   472,   140,   473,   474,   475,
     476,     0,   477,   142,   143,   144,   145,     0,   147,   148,
     149,   150,   151,   152,   153,   154,   155,   156,   157,   158,
       0,   160,   161,   162,   163,   164,   165,   166,   167,   168,
     169,   170,   171,   172,   173,   174,   175,   176,     0,   177,
     478,   179,   180,   181,   182,   183,   184,   185,   186,   187,
     188,   189,     0,   190,   191,   192,   193,   194,   195,   196,
       0,   197,   198,   199,   200,   201,   202,   203,   204,   205,
     206,   207,   208,   209,   210,   211,   212,   411,   214,   215,
     216,   217,   412,   219,   220,   221,   222,   223,   224,     0,
     225,   226,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,   238,   239,   240,   241,   242,   243,     0,
     245,   246,   247,   248,   249,   250,   413,   252,   253,   254,
     255,   256,   257,   258,   259,   260,     0,   262,   479,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,     1,     2,     0,     3,     4,     5,     6,     7,     8,
       0,     0,     0,     0,    11,     0,     0,     0,     0,     0,
      13,     0,     0,     0,   461,    15,   462,   463,    18,    19,
       0,   464,     0,     0,    23,    24,    25,    26,     0,    27,
      28,    29,   404,   405,    32,   406,    34,    35,    36,    37,
      38,    39,    40,     0,     0,     0,    42,     0,   408,    45,
       0,    47,     0,     0,    49,     0,   409,    51,    52,    53,
      54,    55,    56,    57,  1260,    58,    59,    60,    61,    62,
     410,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,     0,     0,    74,    75,    76,     0,     0,    78,    79,
      80,    81,    82,     0,    84,    85,    86,    87,    88,    89,
      90,    91,    92,   465,   466,    95,    96,     0,     0,     0,
       0,     0,    97,     0,    98,    99,   467,   101,   102,   103,
     104,   105,   106,   107,     0,   108,   109,   110,     0,     0,
     111,   112,   113,   114,   115,   116,   468,   118,   119,     0,
     120,   121,     0,   122,   123,   124,     0,     0,   125,   126,
       0,   128,   129,   130,   131,   132,   133,   134,   135,   136,
     137,   138,     0,   139,     0,     0,     0,     0,     0,   140,
       0,     0,     0,     0,     0,   141,   142,   143,   144,   145,
       0,   147,   148,   149,   150,   151,   152,   153,   154,   155,
     156,   157,   158,     0,   160,   161,   162,   163,   164,   165,
     166,   167,   168,   169,   170,   171,   172,   173,   174,   175,
     176,     0,   177,   478,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189,     0,   190,   191,   192,   193,
     194,   195,   196,     0,   197,   198,   199,   200,   201,   202,
     203,   204,   205,   206,   207,   208,   209,   210,   211,   212,
     411,   214,   215,   216,   217,   412,   219,   220,   221,   222,
     223,   224,     0,   225,   226,   227,   228,   229,   230,   231,
     232,   233,   234,   235,   236,   237,   238,   239,   240,   241,
     242,   243,     0,   245,   246,   247,   248,   249,   250,   413,
     252,   253,   254,   255,   256,   257,   258,   259,   260,     0,
     262,   479,   264,   265,   266,   267,   268,   269,   270,   271,
     272,   273,   274,   275,     1,     2,     0,     3,     4,     5,
       6,     7,     8,     0,     0,     0,     0,    11,     0,     0,
       0,     0,     0,    13,     0,     0,   820,   461,    15,   462,
     891,    18,    19,     0,   464,     0,     0,    23,    24,    25,
      26,     0,    27,    28,    29,   404,   405,    32,   406,    34,
      35,    36,    37,    38,    39,    40,     0,     0,     0,    42,
       0,   408,    45,     0,    47,     0,     0,    49,     0,   409,
      51,    52,    53,    54,    55,    56,    57,     0,    58,   892,
      60,    61,    62,   410,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,     0,     0,    74,    75,    76,     0,
       0,    78,    79,    80,    81,    82,     0,    84,    85,    86,
      87,    88,    89,    90,    91,    92,   465,   466,    95,    96,
       0,     0,     0,     0,     0,    97,     0,    98,    99,   467,
     101,   102,   103,   104,   105,   106,   107,     0,   108,   109,
     110,     0,     0,   111,   112,   113,   114,   115,   116,   468,
     118,   119,     0,   120,   121,     0,   122,   123,   124,     0,
       0,   125,   126,     0,   128,   129,   130,   131,   132,   133,
     134,   135,   136,   137,   138,     0,   139,     0,     0,     0,
       0,     0,   140,     0,     0,     0,     0,     0,   141,   142,
     143,   144,   145,     0,   147,   148,   149,   150,   151,   152,
     153,   154,   155,   156,   157,   158,     0,   160,   161,   162,
     163,   164,   165,   166,   167,   168,   169,   170,   171,   172,
     173,   174,   175,   176,     0,   177,   478,   179,   180,   181,
     182,   183,   184,   185,   186,   187,   188,   189,     0,   190,
     191,   192,   193,   194,   195,   196,     0,   197,   198,   199,
     200,   201,   202,   203,   204,   205,   206,   207,   208,   209,
     210,   211,   212,   411,   214,   215,   216,   217,   412,   219,
     220,   221,   222,   223,   224,     0,   225,   226,   227,   228,
     229,   230,   231,   232,   233,   234,   235,   236,   237,   238,
     239,   240,   241,   242,   243,     0,   245,   246,   247,   248,
     249,   250,   413,   252,   253,   254,   255,   256,   257,   258,
     259,   260,     0,   262,   479,   264,   265,   266,   267,   268,
     269,   270,   271,   272,   273,   274,   275,     1,     2,     0,
       3,     4,     5,     6,     7,     8,     0,     0,     0,     0,
      11,     0,     0,     0,     0,     0,    13,     0,     0,     0,
     461,    15,   462,   463,    18,    19,     0,   464,     0,     0,
      23,    24,    25,    26,     0,    27,    28,    29,   404,   405,
      32,   406,    34,    35,    36,    37,    38,    39,    40,     0,
       0,     0,    42,     0,   408,    45,     0,    47,     0,     0,
      49,     0,   409,    51,    52,    53,    54,    55,    56,    57,
       0,    58,    59,    60,    61,    62,   410,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,     0,     0,    74,
      75,    76,     0,     0,    78,    79,    80,    81,    82,     0,
      84,    85,    86,    87,    88,    89,    90,    91,    92,   465,
     466,    95,    96,     0,     0,     0,     0,     0,    97,     0,
      98,    99,   467,   101,   102,   103,   104,   105,   106,   107,
       0,   108,   109,   110,     0,     0,   111,   112,   113,   114,
     115,   116,   468,   118,   119,     0,   120,   121,     0,   122,
     123,   124,     0,     0,   125,   126,     0,   128,   129,   130,
     131,   132,   133,   134,   135,   136,   137,   138,     0,   139,
       0,     0,     0,     0,     0,   140,     0,     0,     0,     0,
       0,   141,   142,   143,   144,   145,     0,   147,   148,   149,
     150,   151,   152,   153,   154,   155,   156,   157,   158,     0,
     160,   161,   162,   163,   164,   165,   166,   167,   168,   169,
     170,   171,   172,   173,   174,   175,   176,     0,   177,   478,
     179,   180,   181,   182,   183,   184,   185,   186,   187,   188,
     189,     0,   190,   191,   192,   193,   194,   195,   196,     0,
     197,   198,   199,   200,   201,   202,   203,   204,   205,   206,
     207,   208,   209,   210,   211,   212,   411,   214,   215,   216,
     217,   412,   219,   220,   221,   222,   223,   224,     0,   225,
     226,   227,   228,   229,   230,   231,   232,   233,   234,   235,
     236,   237,   238,   239,   240,   241,   242,   243,     0,   245,
     246,   247,   248,   249,   250,   413,   252,   253,   254,   255,
     256,   257,   258,   259,   260,     0,   262,   479,   264,   265,
     266,   267,   268,   269,   270,   271,   272,   273,   274,   275,
       1,     2,     0,     3,     4,     5,     6,     7,     8,     0,
       0,     0,     0,    11,     0,     0,     0,     0,     0,    13,
       0,     0,     0,   461,    15,   462,   463,    18,    19,     0,
     464,     0,     0,    23,    24,    25,    26,     0,    27,    28,
      29,   404,   405,    32,   406,    34,    35,    36,    37,    38,
      39,    40,     0,     0,     0,    42,     0,   408,    45,     0,
      47,     0,     0,    49,     0,   409,    51,    52,    53,    54,
      55,    56,    57,     0,    58,    59,    60,    61,    62,   410,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
       0,     0,    74,    75,    76,     0,     0,    78,    79,    80,
      81,    82,     0,    84,    85,    86,    87,    88,    89,    90,
      91,    92,   465,   466,    95,    96,     0,     0,     0,     0,
       0,    97,     0,    98,    99,   467,   101,   102,   103,   104,
     105,   106,   107,     0,   108,   109,   110,     0,     0,   111,
     112,   113,   114,   115,   116,   468,   118,   119,     0,   120,
     121,     0,   122,   123,   124,     0,     0,   125,   126,     0,
     128,   129,   130,   131,   132,   133,   134,   135,   136,   137,
     138,     0,   139,     0,     0,     0,     0,     0,   140,     0,
       0,     0,     0,     0,   141,   142,   143,   144,   145,     0,
     147,   148,   149,   150,   151,   152,   153,   154,   155,   156,
     157,   158,     0,   160,   161,   162,   163,   164,   165,   166,
     167,   168,   169,   170,   171,   172,   173,   174,   175,   176,
       0,   177,   478,   179,   180,   181,   182,   183,   184,   185,
     186,   187,   188,   189,     0,   190,   191,   192,   193,   194,
     612,   196,     0,   197,   198,   199,   200,   201,   202,   203,
     204,   205,   206,   207,   208,   209,   210,   211,   212,   411,
     214,   215,   216,   217,   412,   219,   220,   221,   222,   223,
     224,     0,   225,   226,   227,   228,   229,   230,   231,   232,
     233,   234,   235,   236,   237,   238,   239,   240,   241,   242,
     243,     0,   245,   246,   247,   248,   249,   250,   413,   252,
     253,   254,   255,   256,   257,   258,   259,   260,     0,   262,
     479,   264,   265,   266,   267,   268,   269,   270,   271,   272,
     273,   274,   275,     1,     2,     0,     3,     4,     5,     6,
       7,     8,     0,     0,     0,     0,    11,     0,     0,     0,
       0,     0,    13,     0,     0,     0,   461,    15,   462,   463,
      18,    19,     0,   464,     0,     0,    23,    24,    25,    26,
       0,    27,    28,    29,   404,   405,    32,   406,    34,    35,
      36,    37,    38,    39,    40,     0,     0,     0,    42,     0,
     408,    45,     0,    47,     0,     0,    49,     0,   409,    51,
      52,    53,    54,    55,    56,    57,     0,    58,    59,    60,
      61,    62,   410,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,     0,     0,    74,    75,    76,     0,     0,
      78,    79,    80,    81,    82,     0,    84,    85,    86,    87,
      88,    89,    90,    91,    92,   465,   466,    95,    96,     0,
       0,     0,     0,     0,    97,     0,    98,    99,   467,   101,
     102,   103,   104,   105,   106,   107,     0,   108,   109,   110,
       0,     0,   111,   112,   113,   114,   115,   116,   468,   118,
     119,     0,   120,   121,     0,   122,   123,   124,     0,     0,
     125,   126,     0,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,     0,   139,     0,     0,     0,     0,
       0,   140,     0,     0,     0,     0,     0,   141,   142,   143,
     144,   145,     0,   147,   148,   149,   150,   151,   152,   153,
     154,   155,   156,   157,   158,     0,   160,   161,   162,   163,
     164,   165,   166,   167,   168,   169,   170,   171,   172,   173,
     174,   175,   176,     0,   177,   478,   179,   180,   181,   182,
     183,   184,   185,   186,   187,   188,   189,     0,   190,   191,
     192,   193,   194,   195,   196,     0,   197,   198,   199,   200,
     201,   202,   203,   204,   205,   206,   207,   208,   209,   210,
     211,   212,   411,   214,   215,   216,   217,   412,   219,   220,
     221,   222,   223,   224,     0,   225,   226,   227,   228,   229,
     230,   231,   232,   233,   234,   623,   236,   237,   238,   239,
     240,   241,   242,   243,     0,   245,   246,   247,   248,   249,
     250,   413,   252,   253,   254,   255,   256,   257,   258,   259,
     260,     0,   262,   479,   264,   265,   266,   267,   268,   269,
     270,   271,   272,   273,   274,   275,     1,     2,     0,     3,
       4,     5,     6,     7,     8,     0,     0,     0,     0,    11,
       0,     0,     0,     0,     0,    13,     0,     0,     0,   461,
      15,   462,   463,    18,    19,     0,   464,     0,     0,    23,
      24,    25,    26,     0,    27,    28,    29,   404,   405,    32,
     406,    34,    35,    36,    37,    38,    39,    40,     0,     0,
       0,    42,     0,   408,    45,     0,    47,     0,     0,    49,
       0,   409,    51,    52,    53,   693,    55,    56,    57,     0,
      58,    59,    60,   694,    62,   410,    64,   695,    66,    67,
      68,    69,    70,    71,    72,    73,     0,     0,   696,    75,
      76,     0,     0,    78,    79,    80,    81,    82,     0,    84,
      85,    86,    87,    88,    89,    90,    91,    92,   465,   466,
      95,    96,     0,     0,     0,     0,     0,    97,     0,    98,
      99,   467,   101,   102,   103,   104,   105,   106,   107,     0,
     108,   109,   110,     0,     0,   111,   112,   113,   114,   115,
     116,   468,   118,   119,     0,   120,   121,     0,   122,   123,
     124,     0,     0,   125,   126,     0,   128,   129,   130,   131,
     132,   133,   134,   135,   136,   137,   138,     0,   139,     0,
       0,     0,     0,     0,   140,     0,     0,     0,     0,     0,
     141,   142,   143,   144,   145,     0,   147,   697,   149,   150,
     151,   152,   153,   154,   155,   156,   157,   158,     0,   160,
     161,   162,   163,   164,   165,   166,   167,   168,   169,   170,
     171,   172,   173,   174,   175,   176,     0,   177,   478,   179,
     180,   181,   182,   183,   184,   185,   186,   187,   188,   189,
       0,   190,   191,   192,   193,   194,   195,   196,     0,   197,
     198,   199,   200,   201,   202,   203,   204,   205,   206,   207,
     208,   209,   210,   211,   212,   411,   214,   215,   216,   217,
     412,   219,   220,   221,   222,   223,   224,     0,   225,   226,
     227,   228,   229,   230,   231,   232,   233,   234,   235,   236,
     237,   238,   239,   240,   241,   242,   243,     0,   245,   246,
     247,   248,   249,   250,   413,   252,   253,   254,   255,   256,
     257,   258,   259,   260,     0,   262,   479,   264,   265,   266,
     267,   268,   269,   270,   271,   272,   273,   274,   275,     1,
       2,     0,     3,     4,     5,     6,     7,     8,     0,     0,
       0,     0,    11,     0,     0,     0,     0,     0,    13,     0,
       0,     0,   461,    15,   462,   756,    18,    19,     0,   464,
       0,     0,    23,    24,    25,    26,     0,    27,    28,    29,
     404,   405,    32,   406,    34,    35,    36,    37,    38,    39,
      40,     0,     0,     0,    42,     0,   408,    45,     0,    47,
       0,     0,    49,     0,   409,    51,    52,    53,    54,    55,
      56,    57,     0,    58,    59,    60,    61,    62,   410,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,     0,
       0,    74,    75,    76,     0,     0,    78,    79,    80,    81,
      82,     0,    84,    85,    86,    87,    88,    89,    90,    91,
      92,   465,   466,    95,    96,     0,     0,     0,     0,     0,
      97,     0,    98,    99,   467,   101,   102,   103,   104,   105,
     106,   107,     0,   108,   109,   110,     0,     0,   111,   112,
     113,   114,   115,   116,   468,   118,   119,     0,   120,   121,
       0,   122,   123,   124,     0,     0,   125,   126,     0,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
       0,   139,     0,     0,     0,     0,     0,   140,     0,     0,
       0,     0,     0,   141,   142,   143,   144,   145,     0,   147,
     148,   149,   150,   151,   152,   153,   154,   155,   156,   157,
     158,     0,   160,   161,   162,   163,   164,   165,   166,   167,
     168,   169,   170,   171,   172,   173,   174,   175,   176,     0,
     177,   478,   179,   180,   181,   182,   183,   184,   185,   186,
     187,   188,   189,     0,   190,   191,   192,   193,   194,   195,
     196,     0,   197,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,   208,   209,   210,   211,   212,   411,   214,
     215,   216,   217,   412,   219,   220,   221,   222,   223,   224,
       0,   225,   226,   227,   228,   229,   230,   231,   232,   233,
     234,   235,   236,   237,   238,   239,   240,   241,   242,   243,
       0,   245,   246,   247,   248,   249,   250,   413,   252,   253,
     254,   255,   256,   257,   258,   259,   260,     0,   262,   479,
     264,   265,   266,   267,   268,   269,   270,   271,   272,   273,
     274,   275,     1,     2,     0,     3,     4,     5,     6,     7,
       8,     0,     0,     0,     0,    11,     0,     0,     0,     0,
       0,    13,     0,     0,     0,   461,    15,   462,   824,    18,
      19,     0,   464,     0,     0,    23,    24,    25,    26,     0,
      27,    28,    29,   404,   405,    32,   406,    34,    35,    36,
      37,    38,    39,    40,     0,     0,     0,    42,     0,   408,
      45,     0,    47,     0,     0,    49,     0,   409,    51,    52,
      53,    54,    55,    56,    57,     0,    58,    59,    60,    61,
      62,   410,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,     0,     0,    74,    75,    76,     0,     0,    78,
      79,    80,    81,    82,     0,    84,    85,    86,    87,    88,
      89,    90,    91,    92,   465,   466,    95,    96,     0,     0,
       0,     0,     0,    97,     0,    98,    99,   467,   101,   102,
     103,   104,   105,   106,   107,     0,   108,   109,   110,     0,
       0,   111,   112,   113,   114,   115,   116,   468,   118,   119,
       0,   120,   121,     0,   122,   123,   124,     0,     0,   125,
     126,     0,   128,   129,   130,   131,   132,   133,   134,   135,
     136,   137,   138,     0,   139,     0,     0,     0,     0,     0,
     140,     0,     0,     0,     0,     0,   141,   142,   143,   144,
     145,     0,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,     0,   160,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176,     0,   177,   478,   179,   180,   181,   182,   183,
     184,   185,   186,   187,   188,   189,     0,   190,   191,   192,
     193,   194,   195,   196,     0,   197,   198,   199,   200,   201,
     202,   203,   204,   205,   206,   207,   208,   209,   210,   211,
     212,   411,   214,   215,   216,   217,   412,   219,   220,   221,
     222,   223,   224,     0,   225,   226,   227,   228,   229,   230,
     231,   232,   233,   234,   235,   236,   237,   238,   239,   240,
     241,   242,   243,     0,   245,   246,   247,   248,   249,   250,
     413,   252,   253,   254,   255,   256,   257,   258,   259,   260,
       0,   262,   479,   264,   265,   266,   267,   268,   269,   270,
     271,   272,   273,   274,   275,     1,     2,     0,     3,     4,
       5,     6,     7,     8,     0,     0,     0,     0,    11,     0,
       0,     0,     0,     0,    13,     0,     0,     0,   461,    15,
     462,   858,    18,    19,     0,   464,     0,     0,    23,    24,
      25,    26,     0,    27,    28,    29,   404,   405,    32,   406,
      34,    35,    36,    37,    38,    39,    40,     0,     0,     0,
      42,     0,   408,    45,     0,    47,     0,     0,    49,     0,
     409,    51,    52,    53,    54,    55,    56,    57,     0,    58,
      59,    60,    61,    62,   410,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,     0,     0,    74,    75,    76,
       0,     0,    78,    79,    80,    81,    82,     0,    84,    85,
      86,    87,    88,    89,    90,    91,    92,   465,   466,    95,
      96,     0,     0,     0,     0,     0,    97,     0,    98,    99,
     467,   101,   102,   103,   104,   105,   106,   107,     0,   108,
     109,   110,     0,     0,   111,   112,   113,   114,   115,   116,
     468,   118,   119,     0,   120,   121,     0,   122,   123,   124,
       0,     0,   125,   126,     0,   128,   129,   130,   131,   132,
     133,   134,   135,   136,   137,   138,     0,   139,     0,     0,
       0,     0,     0,   140,     0,     0,     0,     0,     0,   141,
     142,   143,   144,   145,     0,   147,   148,   149,   150,   151,
     152,   153,   154,   155,   156,   157,   158,     0,   160,   161,
     162,   163,   164,   165,   166,   167,   168,   169,   170,   171,
     172,   173,   174,   175,   176,     0,   177,   478,   179,   180,
     181,   182,   183,   184,   185,   186,   187,   188,   189,     0,
     190,   191,   192,   193,   194,   195,   196,     0,   197,   198,
     199,   200,   201,   202,   203,   204,   205,   206,   207,   208,
     209,   210,   211,   212,   411,   214,   215,   216,   217,   412,
     219,   220,   221,   222,   223,   224,     0,   225,   226,   227,
     228,   229,   230,   231,   232,   233,   234,   235,   236,   237,
     238,   239,   240,   241,   242,   243,     0,   245,   246,   247,
     248,   249,   250,   413,   252,   253,   254,   255,   256,   257,
     258,   259,   260,     0,   262,   479,   264,   265,   266,   267,
     268,   269,   270,   271,   272,   273,   274,   275,     1,     2,
       0,     3,     4,     5,     6,     7,     8,     0,     0,     0,
       0,    11,     0,     0,     0,     0,     0,    13,     0,     0,
       0,   461,    15,   462,   463,    18,    19,     0,   464,     0,
       0,    23,    24,    25,    26,     0,    27,    28,    29,   404,
     405,    32,   406,    34,    35,    36,    37,    38,    39,    40,
       0,     0,     0,    42,     0,   408,    45,     0,    47,     0,
       0,    49,     0,   409,    51,    52,    53,    54,    55,    56,
      57,     0,    58,    59,    60,    61,    62,   410,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,     0,     0,
      74,    75,    76,     0,     0,    78,    79,    80,    81,    82,
       0,    84,    85,    86,    87,    88,    89,    90,    91,    92,
     465,   466,    95,    96,     0,     0,     0,     0,     0,    97,
       0,    98,    99,   467,   101,   102,   103,   104,   105,   106,
     107,     0,   108,   109,   110,     0,     0,   111,   112,   113,
     114,   115,   116,   468,   118,   119,     0,   120,   121,     0,
     122,   123,   124,     0,     0,  1020,   126,     0,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   137,   138,     0,
     139,     0,     0,     0,     0,     0,   140,     0,     0,     0,
       0,     0,   141,   142,   143,   144,   145,     0,   147,   148,
     149,   150,   151,   152,   153,   154,   155,   156,   157,   158,
       0,   160,   161,   162,   163,   164,   165,   166,   167,   168,
     169,   170,   171,   172,   173,   174,   175,   176,     0,   177,
     478,   179,   180,   181,   182,   183,   184,   185,   186,   187,
     188,   189,     0,   190,   191,   192,   193,   194,   195,   196,
       0,   197,   198,   199,   200,   201,   202,   203,   204,   205,
     206,   207,   208,   209,   210,   211,   212,   411,   214,   215,
     216,   217,   412,   219,   220,   221,   222,   223,   224,     0,
     225,   226,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,   238,   239,   240,   241,   242,   243,     0,
     245,   246,   247,   248,   249,   250,   413,   252,   253,   254,
     255,   256,   257,   258,   259,   260,     0,   262,   479,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,     1,     2,     0,     3,     4,     5,     6,     7,     8,
       0,     0,     0,     0,    11,     0,     0,     0,     0,     0,
      13,     0,     0,     0,   461,    15,   462,  1072,    18,    19,
       0,   464,     0,     0,    23,    24,    25,    26,     0,    27,
      28,    29,   404,   405,    32,   406,    34,    35,    36,    37,
      38,    39,    40,     0,     0,     0,    42,     0,   408,    45,
       0,    47,     0,     0,    49,     0,   409,    51,    52,    53,
      54,    55,    56,    57,     0,    58,    59,    60,    61,    62,
     410,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,     0,     0,    74,    75,    76,     0,     0,    78,    79,
      80,    81,    82,     0,    84,    85,    86,    87,    88,    89,
      90,    91,    92,   465,   466,    95,    96,     0,     0,     0,
       0,     0,    97,     0,    98,    99,   467,   101,   102,   103,
     104,   105,   106,   107,     0,   108,   109,   110,     0,     0,
     111,   112,   113,   114,   115,   116,   468,   118,   119,     0,
     120,   121,     0,   122,   123,   124,     0,     0,   125,   126,
       0,   128,   129,   130,   131,   132,   133,   134,   135,   136,
     137,   138,     0,   139,     0,     0,     0,     0,     0,   140,
       0,     0,     0,     0,     0,   141,   142,   143,   144,   145,
       0,   147,   148,   149,   150,   151,   152,   153,   154,   155,
     156,   157,   158,     0,   160,   161,   162,   163,   164,   165,
     166,   167,   168,   169,   170,   171,   172,   173,   174,   175,
     176,     0,   177,   478,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189,     0,   190,   191,   192,   193,
     194,   195,   196,     0,   197,   198,   199,   200,   201,   202,
     203,   204,   205,   206,   207,   208,   209,   210,   211,   212,
     411,   214,   215,   216,   217,   412,   219,   220,   221,   222,
     223,   224,     0,   225,   226,   227,   228,   229,   230,   231,
     232,   233,   234,   235,   236,   237,   238,   239,   240,   241,
     242,   243,     0,   245,   246,   247,   248,   249,   250,   413,
     252,   253,   254,   255,   256,   257,   258,   259,   260,     0,
     262,   479,   264,   265,   266,   267,   268,   269,   270,   271,
     272,   273,   274,   275,     1,     2,     0,     3,     4,     5,
       6,     7,     8,     0,     0,     0,     0,    11,     0,     0,
       0,     0,     0,    13,     0,     0,     0,   461,    15,   462,
     463,    18,    19,     0,   464,     0,     0,    23,    24,    25,
      26,     0,    27,    28,    29,   404,   405,    32,   406,    34,
      35,    36,    37,    38,    39,    40,     0,     0,     0,    42,
       0,   408,    45,     0,    47,     0,     0,    49,     0,   409,
      51,    52,    53,    54,    55,    56,    57,     0,    58,    59,
      60,    61,    62,   410,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,     0,     0,    74,    75,    76,     0,
       0,    78,    79,    80,    81,    82,     0,    84,    85,    86,
      87,    88,    89,    90,    91,    92,   465,   466,    95,    96,
       0,     0,     0,     0,     0,    97,     0,    98,    99,   467,
     101,   102,   103,   104,   105,   106,   107,     0,   108,   109,
     110,     0,     0,   111,   112,   113,   114,   115,   116,   468,
     118,   119,     0,   120,   121,     0,   122,   123,   124,     0,
       0,   125,   126,     0,   128,   129,   130,   131,   132,   133,
     134,   135,   136,   137,   138,     0,   139,     0,     0,     0,
       0,     0,   140,     0,     0,     0,     0,     0,   141,   142,
     143,   144,   145,     0,   147,   148,   149,  1327,   151,   152,
     153,   154,   155,   156,   157,   158,     0,   160,   161,   162,
     163,   164,   165,   166,   167,   168,   169,   170,   171,   172,
     173,   174,   175,   176,     0,   177,   478,   179,   180,   181,
     182,   183,   184,   185,   186,   187,   188,   189,     0,   190,
     191,   192,   193,   194,   195,   196,     0,   197,   198,   199,
     200,   201,   202,   203,   204,   205,   206,   207,   208,   209,
     210,   211,   212,   411,   214,   215,   216,   217,   412,   219,
     220,   221,   222,   223,   224,     0,   225,   226,   227,   228,
     229,   230,   231,   232,   233,   234,   235,   236,   237,   238,
     239,   240,   241,   242,   243,     0,   245,   246,   247,   248,
     249,   250,   413,   252,   253,   254,   255,   256,   257,   258,
     259,   260,     0,   262,   479,   264,   265,   266,   267,   268,
     269,   270,   271,   272,   273,   274,   275,     1,     2,     0,
       3,     4,     5,     6,     7,     8,     0,     0,     0,     0,
      11,     0,     0,     0,     0,     0,    13,     0,     0,     0,
     461,    15,   462,   463,    18,    19,     0,   464,     0,     0,
      23,    24,    25,    26,     0,    27,    28,    29,   404,   405,
      32,   406,    34,    35,    36,    37,    38,    39,    40,     0,
       0,     0,    42,     0,   408,    45,     0,    47,     0,     0,
      49,     0,   409,    51,    52,    53,    54,    55,    56,    57,
       0,    58,    59,    60,    61,    62,   410,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,     0,     0,    74,
      75,    76,     0,     0,    78,    79,    80,    81,    82,     0,
      84,    85,    86,    87,    88,    89,    90,    91,    92,   465,
     466,    95,    96,     0,     0,     0,     0,     0,    97,     0,
      98,    99,   467,   101,   102,   103,   104,   105,   106,   107,
       0,   108,   109,   110,     0,     0,   111,   112,   113,   114,
     115,   116,   468,   118,   119,     0,   120,   121,     0,   122,
     123,   124,     0,     0,   125,   126,     0,   128,   129,   130,
     131,   132,   133,   134,   135,   136,   137,   138,     0,   139,
       0,     0,     0,     0,     0,   140,     0,     0,     0,     0,
       0,   141,   142,   143,   144,   145,     0,   147,   148,   149,
     150,   151,   152,   153,   154,   155,   156,   157,   158,     0,
     160,   161,   162,   163,   164,   165,   166,   167,   168,   169,
     170,   171,   172,   173,   174,   175,   176,     0,   177,   478,
     179,   180,   181,   182,   183,   184,   185,   186,   187,   188,
     189,     0,   190,   191,   192,   193,   194,   195,   196,     0,
     197,   198,   199,   200,   201,   202,   203,   204,   205,   206,
     207,   208,   209,   210,   211,   212,   411,   214,   215,   216,
     217,   412,   219,   220,  1391,   222,   223,   224,     0,   225,
     226,   227,   228,   229,   230,   231,   232,   233,   234,   235,
     236,   237,   238,   239,   240,   241,   242,   243,     0,   245,
     246,   247,   248,   249,   250,   413,   252,   253,   254,   255,
     256,   257,   258,   259,   260,     0,   262,   479,   264,   265,
     266,   267,   268,   269,   270,   271,   272,   273,   274,   275
  };

  const short int
  Parser::yycheck_[] =
  {
       0,   483,   878,   968,   278,  1134,   897,   942,  1450,     9,
      10,  1449,    12,   263,  1336,   611,  1601,   590,  1335,   592,
    1039,    21,  1554,     5,     6,     7,     8,     9,     4,    11,
      12,    13,    26,    15,    16,    17,    18,    19,    20,    21,
      22,    41,    24,     5,     6,     7,     8,     9,    26,    11,
      12,    13,     9,    15,    16,    17,    18,    19,    20,    21,
      22,  1624,    24,     5,     6,     7,     8,     9,  1076,    11,
      12,    13,    26,    15,    16,    17,    18,    19,    20,    21,
      22,     6,    24,    26,    57,    57,  1528,    57,    77,    77,
      15,    16,   688,    18,    24,    94,    21,    22,    84,    24,
     100,    26,    95,   979,   100,    68,    59,   703,     5,     6,
       7,     8,     9,    30,    11,    12,    13,    30,    15,    16,
      17,    18,    19,    20,    21,    22,     6,    24,    84,    84,
      84,    58,   164,    46,  1451,    15,    16,    13,    18,     6,
     173,    21,    22,   219,    24,  1021,    26,  1608,    15,    16,
     142,    18,     6,   260,    21,    22,   161,    24,   142,    26,
     108,    15,    16,   275,    18,    13,     6,    21,    22,   100,
      24,   226,    26,   315,   200,    15,    16,  1142,    18,     6,
     202,    21,    22,     7,    24,     9,    26,  1122,    15,    16,
     195,    18,   334,   219,    21,    22,    26,    24,  1866,    26,
     334,  1077,    20,    21,    22,   339,    24,  1172,   212,   204,
    1175,   237,  1529,   268,    20,    21,    22,   142,    24,   331,
     236,     5,     6,     7,     8,     9,   333,    11,    12,    13,
    1668,    15,    16,    17,    18,    19,    20,    21,    22,   204,
      24,    13,   264,    15,    16,    17,    18,    19,    20,    21,
      22,  1919,    24,  1794,    27,   331,   288,    30,   508,   509,
     856,   212,   142,    15,    16,    17,    18,    19,    20,    21,
      22,    13,    24,   226,   200,   142,   189,   277,   278,     5,
       6,     7,     8,     9,   212,    11,    12,    13,   142,    15,
      16,    17,    18,    19,    20,    21,    22,   331,    24,   332,
    1761,   149,   142,   553,   338,   555,    17,    18,    19,    20,
      21,    22,   275,    24,   296,   142,   110,    16,    17,    18,
      19,    20,    21,    22,  1909,    24,    84,   244,   332,   277,
     278,   287,   325,   332,   296,   330,   332,   213,   332,   331,
     328,   335,   334,   332,  1473,   595,   332,    26,  1477,   303,
     334,  1359,   110,   335,   296,   331,    26,   335,   285,   609,
     303,   334,   334,   226,   334,   330,   296,   109,   331,  1910,
     301,   333,  1935,  1936,    74,   332,   331,   332,   332,  1911,
    1912,   332,    82,  1796,   200,   260,    86,    84,  1973,   332,
    1409,   333,    18,    19,    20,    21,    22,    97,    24,   296,
    1813,   260,    26,   100,   332,   268,   331,   149,  1730,   334,
     242,    73,  1377,    52,    53,    52,    53,    54,   418,  1921,
    1016,  1923,    73,  1552,    73,    64,   317,    66,   170,   223,
      92,   322,    73,   433,   316,  1873,   333,   200,  1880,   321,
      81,    92,   334,    92,    85,   208,   319,   339,  1324,   449,
     323,   331,    30,    73,   334,    96,   219,   217,   333,   459,
     460,    81,  1964,   332,   331,    85,   224,   334,   262,   166,
     167,   231,   332,   170,   333,  1888,    96,   331,   296,   334,
     334,   320,   337,   483,    84,   324,   189,   190,   333,   334,
     296,   331,   333,   334,   334,  1997,   196,   333,   334,   499,
    1817,  2003,  1075,   332,   331,   142,    13,   334,   333,   334,
     273,   993,   296,  1955,   333,   334,   333,   334,    84,     5,
       6,     7,     8,     9,   296,    11,    12,    13,  1845,    15,
      16,    17,    18,    19,    20,    21,    22,   266,    24,     3,
     790,   333,   334,   296,   296,   189,   190,    30,   332,   209,
     333,   334,   333,   334,   195,    73,   556,  1874,   333,   334,
     560,    92,   333,   334,   333,   334,   816,   817,   568,   569,
     296,   268,   572,   573,   574,   195,   576,   577,   578,   579,
     580,   581,   582,   583,   584,   296,  1903,   332,     5,     6,
       7,     8,     9,    30,    11,    12,    13,   296,    15,    16,
      17,    18,    19,    20,    21,    22,   332,    24,    84,   248,
     333,   334,  1547,     6,     7,     8,     9,  1934,    11,    12,
      13,    84,    15,    16,    17,    18,    19,    20,    21,    22,
     225,    24,   195,  1509,   340,   341,   636,   333,   334,   639,
     640,   641,   642,   643,   644,    84,     5,     6,     7,     8,
       9,   651,    11,    12,    13,   286,    15,    16,    17,    18,
      19,    20,    21,    22,   332,    24,   333,   334,   668,   332,
     296,     5,     6,     7,     8,     9,   332,    11,    12,    13,
    1645,    15,    16,    17,    18,    19,    20,    21,    22,   332,
      24,   333,   334,   333,   334,   333,   334,   333,   334,   333,
     334,   333,   334,   333,   334,   333,   334,   332,   982,   983,
     333,   334,   333,   334,   333,   334,   333,   334,   332,  1595,
     720,   721,   331,   723,   312,  1616,   333,   334,   978,   331,
       5,     6,     7,     8,     9,   331,    11,    12,    13,   989,
      15,    16,    17,    18,    19,    20,    21,    22,   195,    24,
    1715,   137,     6,   277,   278,  1884,  1885,   333,   334,    13,
      14,    15,    16,   155,    18,    19,    20,    21,    22,   331,
      24,   196,    26,   333,   334,   333,   334,   333,   334,  1029,
     331,     7,     8,     9,   784,    11,    12,    13,   331,    15,
      16,    17,    18,    19,    20,    21,    22,   797,    24,   333,
     334,   162,    77,   333,   334,   195,    60,   333,   334,   331,
     296,     5,     6,     7,     8,     9,   331,    11,    12,    13,
     331,    15,    16,    17,    18,    19,    20,    21,    22,   331,
      24,    30,  1797,   333,   334,   333,   334,   333,   334,   333,
     334,   333,   334,   333,   334,   331,   333,   334,   333,   334,
     331,  1125,   142,  1127,   333,   334,   333,   334,   333,   334,
     333,   334,   333,   334,   333,   334,   866,    30,   868,   331,
       5,     6,     7,     8,     9,   331,    11,    12,    13,   296,
      15,    16,    17,    18,    19,    20,    21,    22,   332,    24,
     333,   334,   333,   334,    30,   895,    30,   897,   333,   334,
     333,   334,   123,   296,   333,   334,   333,   334,   331,     5,
       6,     7,     8,     9,   331,    11,    12,    13,   331,    15,
      16,    17,    18,    19,    20,    21,    22,    30,    24,   333,
     334,   333,   334,  1183,   333,   334,   331,   296,    63,     5,
       6,     7,     8,     9,   331,    11,    12,    13,    26,    15,
      16,    17,    18,    19,    20,    21,    22,   218,    24,   333,
     334,   268,   296,   333,   334,   333,   334,    13,   968,   333,
     334,   331,   331,   331,   168,   169,   333,   334,   333,   334,
     333,   334,   982,   983,   333,   334,   218,  1469,   988,    55,
     333,   334,   189,   993,   333,   334,    30,   331,     5,     6,
       7,     8,     9,   189,    11,    12,    13,   100,    15,    16,
      17,    18,    19,    20,    21,    22,   265,    24,   189,    96,
      77,   296,   189,  1988,   189,   189,   312,   189,  1904,     5,
       6,     7,     8,     9,   318,    11,    12,    13,    73,    15,
      16,    17,    18,    19,    20,    21,    22,  2012,    24,    96,
     331,   331,  1928,   328,   331,  1055,   332,    21,   312,  1309,
     331,  1061,    27,   331,    27,    27,    27,    27,    27,   331,
     296,   331,   331,   331,    41,  1075,   331,    80,   332,    43,
     331,   331,  1332,   331,   331,   331,   331,   331,   331,   224,
     331,   331,   331,  1093,  1094,    80,   331,   331,  1348,   159,
     331,   331,   296,   331,     5,     6,     7,     8,     9,   331,
      11,    12,    13,    77,    15,    16,    17,    18,    19,    20,
      21,    22,   331,    24,   331,  1125,   142,  1127,   224,   331,
      26,   331,    26,  1615,   233,   331,   331,   191,   331,   274,
     331,   331,  1142,   331,   331,   334,   331,   331,   155,   331,
    1150,   331,     5,     6,     7,     8,     9,   332,    11,    12,
      13,   296,    15,    16,    17,    18,    19,    20,    21,    22,
     331,    24,  1172,   331,   280,  1175,   265,   149,   274,   840,
     332,   200,   843,    26,   333,    30,   332,   212,   332,   332,
      85,    30,    77,    41,    84,  1445,  1446,   199,    30,    87,
     296,   200,     5,     6,     7,     8,     9,  1207,    11,    12,
      13,   872,    15,    16,    17,    18,    19,    20,    21,    22,
     200,    24,   332,  1223,    50,    77,   161,   161,   161,    50,
     296,   189,   189,    45,   332,   189,   331,  1237,  1238,    12,
      27,   333,   333,   333,   332,   906,   333,   331,   334,   331,
     331,   152,   331,    87,    57,   331,   331,   331,  1740,    27,
     236,   331,     9,   331,   331,    30,   331,   331,   331,   122,
     331,    30,    67,    87,    57,  1275,   331,   196,   171,    50,
    1280,   333,    87,   332,    30,    96,   189,   335,    81,   296,
     332,  1541,     5,     6,     7,     8,     9,    30,    11,    12,
      13,   332,    15,    16,    17,    18,    19,    20,    21,    22,
      30,    24,   332,    30,   975,     5,     6,     7,     8,     9,
     296,    11,    12,    13,   161,    15,    16,    17,    18,    19,
      20,    21,    22,    59,    24,    77,   100,    94,    27,   331,
     331,  1002,   161,  1004,   299,  1006,  1007,   333,  1009,  1599,
      27,   235,    27,    27,  1354,    79,    67,     5,     6,     7,
       8,     9,   331,    11,    12,    13,   334,    15,    16,    17,
      18,    19,    20,    21,    22,   331,    24,  1377,   331,   196,
     332,   331,   284,  1044,    30,  1635,    30,   304,   226,   331,
      30,   332,  1642,   331,   125,   296,    32,    30,    77,   331,
     331,   331,    30,   331,   331,  1405,    73,     5,     6,     7,
       8,     9,   271,    11,    12,    13,   331,    15,    16,    17,
      18,    19,    20,    21,    22,   331,    24,   197,    73,    59,
      59,    84,   332,   196,     5,     6,     7,     8,     9,   152,
      11,    12,    13,   296,    15,    16,    17,    18,    19,    20,
      21,    22,    30,    24,   332,    27,   225,   421,   422,   423,
      27,  1461,   152,   427,   428,   225,   430,   431,   432,  1469,
    1470,   196,  1722,   437,   336,  1475,   440,   441,   442,   443,
     331,   234,   446,   447,   331,    12,    30,   331,   452,   453,
     454,   455,   456,   296,    99,  1495,    58,   333,   333,    27,
    1750,    66,   333,   333,   331,    27,   331,    66,   332,   331,
     312,   331,    50,   331,    84,   331,  1766,    94,   482,   142,
     110,   485,   331,   331,   331,   489,   490,   491,   492,   226,
     494,   495,   496,   497,   498,   331,   500,   501,   502,   503,
     504,   505,   331,   507,    92,   331,   153,   335,   149,    30,
     332,   332,   332,   332,   173,   173,   173,   332,  1808,   280,
    1810,    87,   108,    30,    30,   332,    30,    30,    30,   331,
     331,   535,   536,   537,   538,   539,   540,   541,   542,   543,
     544,   545,   546,   296,   548,   549,    75,   551,   552,   332,
     554,   189,  1592,   557,   332,   332,    30,    77,   204,   331,
    1600,    27,    27,    27,   332,    27,   296,    58,   331,  1609,
      40,   164,  1612,    50,   262,  1615,  1616,  1617,   189,   160,
    1870,   331,   331,    30,    73,   332,   215,   235,   331,   593,
      95,    30,   142,   225,  1634,   220,   600,   601,   602,   603,
     604,    30,    75,   331,    30,  1645,    27,   611,   296,    27,
    1650,    27,    27,    27,    84,    27,   620,    89,    88,   623,
      90,    74,   331,    93,   215,   331,   206,   631,   632,   331,
     175,   235,   271,    50,   333,  1675,   233,    50,   332,    95,
    1930,   305,    30,    30,   114,   234,   164,    27,    27,  1939,
     109,   164,   331,   657,   658,     7,     8,     9,   296,    11,
      12,    13,   196,    15,    16,    17,    18,    19,    20,    21,
      22,   331,    24,    84,   207,  1715,   680,  1717,   333,  1719,
     333,   333,  1972,    87,   688,   296,   234,   338,   230,   693,
     694,   695,   696,   697,  1734,   165,    91,    30,    84,   703,
    1740,    87,    50,   261,  1744,  1745,  1746,     5,     6,     7,
       8,     9,   716,    11,    12,    13,   332,    15,    16,    17,
      18,    19,    20,    21,    22,   150,    24,   221,   298,   199,
     200,   201,   127,    84,   738,    27,    87,   332,    89,    30,
     332,    92,   332,    50,    99,   332,   750,   751,   752,   753,
     754,   334,    77,    77,    50,   150,    40,  1797,   331,    27,
     142,  1801,   331,   110,    27,   769,  1806,   237,   332,   332,
     774,   282,   333,   333,   333,   161,    73,   332,   164,   174,
     175,   176,   177,   178,   179,   180,   181,   331,   183,   184,
    1830,   332,   187,   188,    30,    27,   332,   267,   235,    27,
     110,    27,   272,   331,   331,    77,   175,   332,   327,   280,
     161,   196,   280,   164,   200,   199,   331,   287,   822,   289,
      30,   291,   292,   293,    77,   175,   233,   297,  1868,    30,
      30,    30,   218,   333,   838,    30,   234,   240,   334,   332,
     240,  1881,   332,   331,   189,   849,   100,   230,   852,   200,
     236,   855,   856,   197,   195,   334,   860,   332,   334,   224,
     864,   228,   281,    27,   331,   869,   306,   218,   219,   332,
     331,   281,   230,    30,   125,   228,   331,   283,   332,   243,
     266,    46,   331,   205,   207,   236,   272,   238,   892,    50,
     331,    27,    27,    27,    77,   300,   197,   331,   188,   334,
     189,   287,   288,   289,   290,   333,    66,   293,   332,   332,
     332,   297,    50,   175,    30,   266,   168,    27,   204,   214,
     207,   272,  1962,   331,    27,   239,   285,    77,   195,   331,
     195,   332,   331,    30,   331,    30,   287,   288,   289,   290,
     944,   100,   293,   198,   296,    30,   297,   262,  1988,   224,
     954,    30,   230,   331,   331,   161,   241,   100,   206,    30,
      30,   332,   332,    92,    77,    30,    77,   288,    30,   110,
     331,   333,  2012,   307,   331,   331,   217,    30,   189,    30,
     241,   308,   331,    73,   203,   331,    30,   991,   281,   331,
     994,    30,    59,   331,    92,   332,   331,    84,   296,   331,
      50,   332,   330,   274,    30,   331,  1010,   301,   211,   195,
      30,   331,  1016,  1017,   331,   331,  1020,   225,    59,   331,
     331,   149,   331,  1027,  1028,   304,  1030,   332,    82,    27,
    1034,   331,  1036,  1037,    50,   225,    30,    30,    30,  1043,
      27,   274,   216,   331,    30,   100,    84,   210,    82,   332,
     331,   331,  1013,   211,   331,  1059,   332,   331,  1482,  1634,
    1609,  1795,  1819,  1156,  1860,  1229,  1207,   513,   898,  1226,
     810,  1223,  1076,  1467,  1801,  1744,  1734,  1650,  1868,  1717,
    1536,  1278,  1277,   630,    -1,  1089,    -1,   391,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1099,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,  1116,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,  1140,    -1,    -1,    -1,
    1144,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1163,
      -1,    -1,    -1,    -1,    -1,  1169,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,  1186,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,  1196,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1212,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,  1225,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,  1260,    -1,    -1,    -1,
      -1,    -1,  1266,    -1,    -1,    -1,  1270,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1278,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,  1320,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1359,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1368,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1379,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1403,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1428,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1448,  1449,    -1,    -1,    -1,    -1,
    1454,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,  1480,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,  1506,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,  1560,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1589,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1598,    -1,    -1,    -1,    -1,  1603,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1641,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1668,    -1,  1670,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1692,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    1724,  1725,    -1,    -1,    -1,    -1,    -1,  1731,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,  1756,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    1764,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1789,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  1857,    -1,    -1,    -1,  1861,  1862,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1871,    -1,  1873,
      -1,    -1,  1876,    -1,  1878,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  1907,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,  1925,     0,  1927,    -1,    -1,     4,     5,    -1,     7,
       8,     9,    10,    11,    12,    13,    14,    -1,    -1,    17,
      -1,    19,    -1,    -1,    -1,    23,    -1,  1951,    -1,    27,
      28,    29,    30,    31,    32,    -1,    34,    35,    36,    37,
      38,    39,    40,  1967,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    -1,
      -1,    59,    60,    61,    62,    63,    64,    65,    -1,    67,
      -1,    69,    70,    71,    72,    73,    74,    75,    76,    -1,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    -1,    -1,    96,    97,
      98,    99,    -1,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,    -1,    -1,    -1,    -1,    -1,   125,    -1,   127,
     128,   129,   130,   131,   132,   133,   134,   135,   136,    -1,
     138,   139,   140,    -1,    -1,   143,   144,   145,   146,   147,
     148,   149,   150,   151,    -1,   153,   154,    -1,   156,   157,
     158,    -1,    -1,   161,   162,   163,   164,   165,   166,   167,
     168,   169,   170,   171,   172,   173,   174,    -1,   176,    -1,
      -1,    -1,    -1,    -1,   182,    -1,    -1,    -1,    -1,    -1,
     188,   189,   190,   191,   192,   193,   194,   195,   196,   197,
     198,   199,   200,   201,   202,   203,   204,   205,   206,   207,
     208,   209,   210,   211,   212,   213,   214,   215,   216,   217,
     218,   219,   220,   221,   222,   223,    -1,   225,   226,   227,
     228,   229,   230,   231,   232,   233,   234,   235,   236,   237,
      -1,   239,   240,   241,   242,   243,   244,   245,    -1,   247,
     248,   249,   250,   251,   252,   253,   254,   255,   256,   257,
     258,   259,   260,   261,   262,   263,   264,   265,   266,   267,
     268,   269,   270,   271,   272,   273,   274,    -1,   276,   277,
     278,   279,   280,   281,   282,   283,   284,   285,   286,   287,
     288,   289,   290,   291,   292,   293,   294,   295,   296,   297,
     298,   299,   300,   301,   302,   303,   304,   305,   306,   307,
     308,   309,   310,   311,   312,   313,   314,   315,   316,   317,
     318,   319,   320,   321,   322,   323,   324,   325,   326,    -1,
      -1,   329,     4,     5,   332,     7,     8,     9,    10,    11,
      12,    13,    14,    -1,    -1,    17,    -1,    19,    20,    -1,
      -1,    23,    -1,    -1,    -1,    27,    28,    29,    30,    31,
      32,    -1,    34,    -1,    -1,    37,    38,    39,    40,    -1,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    -1,    -1,    -1,    59,    60,    61,
      62,    -1,    64,    -1,    -1,    67,    -1,    69,    70,    71,
      72,    73,    74,    75,    76,    -1,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    -1,    -1,    96,    97,    98,    -1,    -1,   101,
     102,   103,   104,   105,    -1,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,    -1,    -1,
      -1,    -1,    -1,   125,   126,   127,   128,   129,   130,   131,
     132,   133,   134,   135,   136,    -1,   138,   139,   140,    -1,
      -1,   143,   144,   145,   146,   147,   148,   149,   150,   151,
      -1,   153,   154,    -1,   156,   157,   158,    -1,    -1,   161,
     162,    -1,   164,   165,   166,   167,   168,   169,   170,   171,
     172,   173,   174,    -1,   176,    -1,    -1,    -1,    -1,    -1,
     182,    -1,    -1,    -1,    -1,    -1,   188,   189,   190,   191,
     192,    -1,   194,   195,   196,   197,   198,   199,   200,   201,
     202,   203,   204,   205,    -1,   207,   208,   209,   210,   211,
     212,   213,   214,   215,   216,   217,   218,   219,   220,   221,
     222,   223,    -1,   225,   226,   227,   228,   229,   230,   231,
     232,   233,   234,   235,   236,   237,    -1,   239,   240,   241,
     242,   243,   244,   245,   246,   247,   248,   249,   250,   251,
     252,   253,   254,   255,   256,   257,   258,   259,   260,   261,
     262,   263,   264,   265,   266,   267,   268,   269,   270,   271,
     272,   273,   274,    -1,   276,   277,   278,   279,   280,   281,
     282,   283,   284,   285,   286,   287,   288,   289,   290,   291,
     292,   293,   294,    -1,   296,   297,   298,   299,   300,   301,
     302,   303,   304,   305,   306,   307,   308,   309,   310,   311,
      -1,   313,   314,   315,   316,   317,   318,   319,   320,   321,
     322,   323,   324,   325,   326,    -1,    -1,    -1,    -1,    -1,
     332,   333,     4,     5,    -1,     7,     8,     9,    10,    11,
      12,    13,    14,    -1,    -1,    17,    -1,    19,    -1,    -1,
      -1,    23,    -1,    -1,    -1,    27,    28,    29,    30,    31,
      32,    -1,    34,    35,    36,    37,    38,    39,    40,    -1,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    -1,    -1,    59,    60,    61,
      62,    63,    64,    65,    -1,    67,    -1,    69,    70,    71,
      72,    73,    74,    75,    76,    -1,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    -1,    -1,    96,    97,    98,    99,    -1,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,    -1,    -1,
      -1,    -1,    -1,   125,    -1,   127,   128,   129,   130,   131,
     132,   133,   134,   135,   136,   137,   138,   139,   140,    -1,
      -1,   143,   144,   145,   146,   147,   148,   149,   150,   151,
      -1,   153,   154,    -1,   156,   157,   158,    -1,    -1,   161,
     162,   163,   164,   165,   166,   167,   168,   169,   170,   171,
     172,   173,   174,    -1,   176,    -1,    -1,    -1,    -1,    -1,
     182,    -1,    -1,    -1,    -1,    -1,   188,   189,   190,   191,
     192,   193,   194,   195,   196,   197,   198,   199,   200,   201,
     202,   203,   204,   205,   206,   207,   208,   209,   210,   211,
     212,   213,   214,   215,   216,   217,   218,   219,   220,   221,
     222,   223,    -1,   225,   226,   227,   228,   229,   230,   231,
     232,   233,   234,   235,   236,   237,    -1,   239,   240,   241,
     242,   243,   244,   245,    -1,   247,   248,   249,   250,   251,
     252,   253,   254,   255,   256,   257,   258,   259,   260,   261,
     262,   263,   264,   265,   266,   267,   268,   269,   270,   271,
     272,   273,   274,    -1,   276,   277,   278,   279,   280,   281,
     282,   283,   284,   285,   286,   287,   288,   289,   290,   291,
     292,   293,   294,   295,   296,   297,   298,   299,   300,   301,
     302,   303,   304,   305,   306,   307,   308,   309,   310,   311,
     312,   313,   314,   315,   316,   317,   318,   319,   320,   321,
     322,   323,   324,   325,   326,    -1,    -1,   329,     4,     5,
     332,     7,     8,     9,    10,    11,    12,    13,    14,    -1,
      -1,    17,    -1,    19,    -1,    -1,    -1,    23,    -1,    -1,
      -1,    27,    28,    29,    30,    31,    32,    -1,    34,    35,
      36,    37,    38,    39,    40,    -1,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    -1,    -1,    59,    60,    61,    62,    63,    64,    65,
      -1,    67,    -1,    69,    70,    71,    72,    73,    74,    75,
      76,    -1,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    -1,    -1,
      96,    97,    98,    99,    -1,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,    -1,    -1,    -1,    -1,    -1,   125,
      -1,   127,   128,   129,   130,   131,   132,   133,   134,   135,
     136,    -1,   138,   139,   140,    -1,    -1,   143,   144,   145,
     146,   147,   148,   149,   150,   151,    -1,   153,   154,    -1,
     156,   157,   158,    -1,    -1,   161,   162,   163,   164,   165,
     166,   167,   168,   169,   170,   171,   172,   173,   174,    -1,
     176,    -1,    -1,    -1,    -1,    -1,   182,    -1,    -1,    -1,
      -1,    -1,   188,   189,   190,   191,   192,   193,   194,   195,
     196,   197,   198,   199,   200,   201,   202,   203,   204,   205,
     206,   207,   208,   209,   210,   211,   212,   213,   214,   215,
     216,   217,   218,   219,   220,   221,   222,   223,    -1,   225,
     226,   227,   228,   229,   230,   231,   232,   233,   234,   235,
     236,   237,    -1,   239,   240,   241,   242,   243,   244,   245,
      -1,   247,   248,   249,   250,   251,   252,   253,   254,   255,
     256,   257,   258,   259,   260,   261,   262,   263,   264,   265,
     266,   267,   268,   269,   270,   271,   272,   273,   274,    -1,
     276,   277,   278,   279,   280,   281,   282,   283,   284,   285,
     286,   287,   288,   289,   290,   291,   292,   293,   294,   295,
     296,   297,   298,   299,   300,   301,   302,   303,   304,   305,
     306,   307,   308,   309,   310,   311,   312,   313,   314,   315,
     316,   317,   318,   319,   320,   321,   322,   323,   324,   325,
     326,    -1,    -1,   329,     4,     5,   332,     7,     8,     9,
      10,    11,    12,    13,    14,    -1,    -1,    17,    -1,    19,
      -1,    -1,    -1,    23,    -1,    -1,    -1,    27,    28,    29,
      30,    31,    32,    -1,    34,    35,    36,    37,    38,    39,
      40,    -1,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    -1,    -1,    59,
      60,    61,    62,    63,    64,    65,    -1,    67,    -1,    69,
      70,    71,    72,    73,    74,    75,    76,    -1,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    -1,    -1,    96,    97,    98,    99,
      -1,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
      -1,    -1,    -1,    -1,    -1,   125,    -1,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,    -1,   138,   139,
     140,    -1,    -1,   143,   144,   145,   146,   147,   148,   149,
     150,   151,    -1,   153,   154,    -1,   156,   157,   158,    -1,
      -1,   161,   162,   163,   164,   165,   166,   167,   168,   169,
     170,   171,   172,   173,   174,    -1,   176,    -1,    -1,    -1,
      -1,    -1,   182,    -1,    -1,    -1,    -1,    -1,   188,   189,
     190,   191,   192,   193,   194,   195,   196,   197,   198,   199,
     200,   201,   202,   203,   204,   205,   206,   207,   208,   209,
     210,   211,   212,   213,   214,   215,   216,   217,   218,   219,
     220,   221,   222,   223,    -1,   225,   226,   227,   228,   229,
     230,   231,   232,   233,   234,   235,   236,   237,    -1,   239,
     240,   241,   242,   243,   244,   245,    -1,   247,   248,   249,
     250,   251,   252,   253,   254,   255,   256,   257,   258,   259,
     260,   261,   262,   263,   264,   265,   266,   267,   268,   269,
     270,   271,   272,   273,   274,    -1,   276,   277,   278,   279,
     280,   281,   282,   283,   284,   285,   286,   287,   288,   289,
     290,   291,   292,   293,   294,   295,   296,   297,   298,   299,
     300,   301,   302,   303,   304,   305,   306,   307,   308,   309,
     310,   311,   312,   313,   314,   315,   316,   317,   318,   319,
     320,   321,   322,   323,   324,   325,   326,    -1,    -1,   329,
      -1,    -1,   332,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    -1,    -1,    27,    28,    29,    30,
      31,    32,    -1,    34,    -1,    -1,    37,    38,    39,    40,
      -1,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    -1,    -1,    -1,    59,    60,
      61,    62,    -1,    64,    -1,    -1,    67,    -1,    69,    70,
      71,    72,    73,    74,    75,    76,    -1,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    -1,    -1,    96,    97,    98,    -1,    -1,
     101,   102,   103,   104,   105,    -1,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,    -1,
      -1,    -1,    -1,    -1,   125,    -1,   127,   128,   129,   130,
     131,   132,   133,   134,   135,   136,    -1,   138,   139,   140,
      -1,    -1,   143,   144,   145,   146,   147,   148,   149,   150,
     151,    -1,   153,   154,    -1,   156,   157,   158,    -1,    -1,
     161,   162,    -1,   164,   165,   166,   167,   168,   169,   170,
     171,   172,   173,   174,    -1,   176,    -1,    -1,    -1,    -1,
      -1,   182,    -1,    -1,    -1,    -1,    -1,   188,   189,   190,
     191,   192,    -1,   194,   195,   196,   197,   198,   199,   200,
     201,   202,   203,   204,   205,    -1,   207,   208,   209,   210,
     211,   212,   213,   214,   215,   216,   217,   218,   219,   220,
     221,   222,   223,    -1,   225,   226,   227,   228,   229,   230,
     231,   232,   233,   234,   235,   236,   237,    -1,   239,   240,
     241,   242,   243,   244,   245,    -1,   247,   248,   249,   250,
     251,   252,   253,   254,   255,   256,   257,   258,   259,   260,
     261,   262,   263,   264,   265,   266,   267,   268,   269,   270,
     271,   272,   273,   274,    -1,   276,   277,   278,   279,   280,
     281,   282,   283,   284,   285,   286,   287,   288,   289,   290,
     291,   292,   293,   294,    -1,   296,   297,   298,   299,   300,
     301,   302,   303,   304,   305,   306,   307,   308,   309,   310,
     311,    -1,   313,   314,   315,   316,   317,   318,   319,   320,
     321,   322,   323,   324,   325,   326,    -1,    -1,    -1,     4,
       5,   332,     7,     8,     9,    10,    11,    12,    13,    14,
      -1,    -1,    17,    -1,    19,    20,    -1,    -1,    23,    -1,
      -1,    -1,    27,    28,    29,    30,    31,    32,    -1,    34,
      -1,    -1,    37,    38,    39,    40,    -1,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    -1,    -1,    -1,    59,    60,    61,    62,    -1,    64,
      -1,    -1,    67,    -1,    69,    70,    71,    72,    73,    74,
      75,    76,    -1,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    -1,
      -1,    96,    97,    98,    -1,    -1,   101,   102,   103,   104,
     105,    -1,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,    -1,    -1,    -1,   123,    -1,
     125,    -1,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,    -1,   138,   139,   140,    -1,    -1,   143,   144,
     145,   146,   147,   148,   149,   150,   151,    -1,   153,   154,
      -1,   156,   157,   158,    -1,    -1,   161,   162,    -1,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
      -1,   176,    -1,    -1,    -1,    -1,    -1,   182,    -1,    -1,
      -1,    -1,    -1,   188,   189,   190,   191,   192,    -1,   194,
     195,   196,   197,   198,   199,   200,   201,   202,   203,   204,
     205,    -1,   207,   208,   209,   210,   211,   212,   213,   214,
     215,   216,   217,   218,   219,   220,   221,   222,   223,    -1,
     225,   226,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,    -1,   239,   240,   241,   242,   243,   244,
     245,   246,   247,   248,   249,   250,   251,   252,   253,   254,
     255,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
      -1,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
      -1,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,    -1,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,    -1,    -1,    -1,     4,     5,   332,     7,     8,
       9,    10,    11,    12,    13,    14,    -1,    -1,    17,    -1,
      19,    -1,    -1,    -1,    23,    -1,    -1,    -1,    27,    28,
      29,    30,    31,    32,    -1,    34,    -1,    -1,    37,    38,
      39,    40,    -1,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    -1,    -1,    -1,
      59,    60,    61,    62,    -1,    64,    -1,    -1,    67,    -1,
      69,    70,    71,    72,    73,    74,    75,    76,    -1,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    -1,    -1,    96,    97,    98,
      -1,    -1,   101,   102,   103,   104,   105,    -1,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,    -1,    -1,    -1,    -1,   125,    -1,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,    -1,   138,
     139,   140,    -1,    -1,   143,   144,   145,   146,   147,   148,
     149,   150,   151,    -1,   153,   154,    -1,   156,   157,   158,
      -1,    -1,   161,   162,    -1,   164,   165,   166,   167,   168,
     169,   170,   171,   172,   173,   174,    -1,   176,    -1,    -1,
      -1,    -1,    -1,   182,    -1,    -1,    -1,    -1,    -1,   188,
     189,   190,   191,   192,    -1,   194,   195,   196,   197,   198,
     199,   200,   201,   202,   203,   204,   205,    -1,   207,   208,
     209,   210,   211,   212,   213,   214,   215,   216,   217,   218,
     219,   220,   221,   222,   223,    -1,   225,   226,   227,   228,
     229,   230,   231,   232,   233,   234,   235,   236,   237,   238,
     239,   240,   241,   242,   243,   244,   245,    -1,   247,   248,
     249,   250,   251,   252,   253,   254,   255,   256,   257,   258,
     259,   260,   261,   262,   263,   264,   265,   266,   267,   268,
     269,   270,   271,   272,   273,   274,    -1,   276,   277,   278,
     279,   280,   281,   282,   283,   284,   285,   286,   287,   288,
     289,   290,   291,   292,   293,   294,    -1,   296,   297,   298,
     299,   300,   301,   302,   303,   304,   305,   306,   307,   308,
     309,   310,   311,    -1,   313,   314,   315,   316,   317,   318,
     319,   320,   321,   322,   323,   324,   325,   326,    -1,    -1,
      -1,     4,     5,   332,     7,     8,     9,    10,    11,    12,
      13,    14,    -1,    -1,    17,    -1,    19,    -1,    -1,    -1,
      23,    -1,    -1,    -1,    27,    28,    29,    30,    31,    32,
      -1,    34,    -1,    -1,    37,    38,    39,    40,    -1,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    -1,    -1,    -1,    59,    60,    61,    62,
      -1,    64,    -1,    -1,    67,    -1,    69,    70,    71,    72,
      73,    74,    75,    76,    -1,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    -1,    -1,    96,    97,    98,    -1,    -1,   101,   102,
     103,   104,   105,    -1,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,    -1,    -1,    -1,
      -1,    -1,   125,    -1,   127,   128,   129,   130,   131,   132,
     133,   134,   135,   136,   137,   138,   139,   140,    -1,    -1,
     143,   144,   145,   146,   147,   148,   149,   150,   151,    -1,
     153,   154,   155,   156,   157,   158,    -1,    -1,   161,   162,
      -1,   164,   165,   166,   167,   168,   169,   170,   171,   172,
     173,   174,    -1,   176,    -1,    -1,    -1,    -1,    -1,   182,
      -1,    -1,    -1,    -1,    -1,   188,   189,   190,   191,   192,
      -1,   194,   195,   196,   197,   198,   199,   200,   201,   202,
     203,   204,   205,    -1,   207,   208,   209,   210,   211,   212,
     213,   214,   215,   216,   217,   218,   219,   220,   221,   222,
     223,    -1,   225,   226,   227,   228,   229,   230,   231,   232,
     233,   234,   235,   236,   237,    -1,   239,   240,   241,   242,
     243,   244,   245,    -1,   247,   248,   249,   250,   251,   252,
     253,   254,   255,   256,   257,   258,   259,   260,   261,   262,
     263,   264,   265,   266,   267,   268,   269,   270,   271,   272,
     273,   274,    -1,   276,   277,   278,   279,   280,   281,   282,
     283,   284,   285,   286,   287,   288,   289,   290,   291,   292,
     293,   294,    -1,   296,   297,   298,   299,   300,   301,   302,
     303,   304,   305,   306,   307,   308,   309,   310,   311,    -1,
     313,   314,   315,   316,   317,   318,   319,   320,   321,   322,
     323,   324,   325,   326,    -1,    -1,    -1,     4,     5,   332,
       7,     8,     9,    10,    11,    12,    13,    14,    -1,    -1,
      17,    -1,    19,    20,    -1,    -1,    23,    -1,    -1,    -1,
      27,    28,    29,    30,    31,    32,    -1,    34,    -1,    -1,
      37,    38,    39,    40,    -1,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    -1,
      -1,    -1,    59,    60,    61,    62,    -1,    64,    -1,    -1,
      67,    -1,    69,    70,    71,    72,    73,    74,    75,    76,
      -1,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    -1,    -1,    96,
      97,    98,    -1,    -1,   101,   102,   103,   104,   105,    -1,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,    -1,    -1,    -1,    -1,    -1,   125,    -1,
     127,   128,   129,   130,   131,   132,   133,   134,   135,   136,
      -1,   138,   139,   140,    -1,    -1,   143,   144,   145,   146,
     147,   148,   149,   150,   151,    -1,   153,   154,    -1,   156,
     157,   158,    -1,    -1,   161,   162,    -1,   164,   165,   166,
     167,   168,   169,   170,   171,   172,   173,   174,    -1,   176,
      -1,    -1,    -1,    -1,    -1,   182,    -1,    -1,    -1,    -1,
      -1,   188,   189,   190,   191,   192,    -1,   194,   195,   196,
     197,   198,   199,   200,   201,   202,   203,   204,   205,    -1,
     207,   208,   209,   210,   211,   212,   213,   214,   215,   216,
     217,   218,   219,   220,   221,   222,   223,    -1,   225,   226,
     227,   228,   229,   230,   231,   232,   233,   234,   235,   236,
     237,    -1,   239,   240,   241,   242,   243,   244,   245,    -1,
     247,   248,   249,   250,   251,   252,   253,   254,   255,   256,
     257,   258,   259,   260,   261,   262,   263,   264,   265,   266,
     267,   268,   269,   270,   271,   272,   273,   274,    -1,   276,
     277,   278,   279,   280,   281,   282,   283,   284,   285,   286,
     287,   288,   289,   290,   291,   292,   293,   294,    -1,   296,
     297,   298,   299,   300,   301,   302,   303,   304,   305,   306,
     307,   308,   309,   310,   311,    -1,   313,   314,   315,   316,
     317,   318,   319,   320,   321,   322,   323,   324,   325,   326,
      -1,    -1,    -1,     4,     5,   332,     7,     8,     9,    10,
      11,    12,    13,    14,    -1,    -1,    17,    -1,    19,    20,
      -1,    -1,    23,    -1,    -1,    -1,    27,    28,    29,    30,
      31,    32,    -1,    34,    -1,    -1,    37,    38,    39,    40,
      -1,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    -1,    -1,    -1,    59,    60,
      61,    62,    -1,    64,    -1,    -1,    67,    -1,    69,    70,
      71,    72,    73,    74,    75,    76,    -1,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    -1,    -1,    96,    97,    98,    -1,    -1,
     101,   102,   103,   104,   105,    -1,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,    -1,
      -1,    -1,    -1,    -1,   125,    -1,   127,   128,   129,   130,
     131,   132,   133,   134,   135,   136,    -1,   138,   139,   140,
      -1,    -1,   143,   144,   145,   146,   147,   148,   149,   150,
     151,    -1,   153,   154,    -1,   156,   157,   158,    -1,    -1,
     161,   162,    -1,   164,   165,   166,   167,   168,   169,   170,
     171,   172,   173,   174,    -1,   176,    -1,    -1,    -1,    -1,
      -1,   182,    -1,    -1,    -1,    -1,    -1,   188,   189,   190,
     191,   192,    -1,   194,   195,   196,   197,   198,   199,   200,
     201,   202,   203,   204,   205,    -1,   207,   208,   209,   210,
     211,   212,   213,   214,   215,   216,   217,   218,   219,   220,
     221,   222,   223,    -1,   225,   226,   227,   228,   229,   230,
     231,   232,   233,   234,   235,   236,   237,    -1,   239,   240,
     241,   242,   243,   244,   245,    -1,   247,   248,   249,   250,
     251,   252,   253,   254,   255,   256,   257,   258,   259,   260,
     261,   262,   263,   264,   265,   266,   267,   268,   269,   270,
     271,   272,   273,   274,    -1,   276,   277,   278,   279,   280,
     281,   282,   283,   284,   285,   286,   287,   288,   289,   290,
     291,   292,   293,   294,    -1,   296,   297,   298,   299,   300,
     301,   302,   303,   304,   305,   306,   307,   308,   309,   310,
     311,    -1,   313,   314,   315,   316,   317,   318,   319,   320,
     321,   322,   323,   324,   325,   326,    -1,    -1,    -1,     4,
       5,   332,     7,     8,     9,    10,    11,    12,    13,    14,
      -1,    -1,    17,    -1,    19,    -1,    -1,    -1,    23,    -1,
      -1,    -1,    27,    28,    29,    30,    31,    32,    -1,    34,
      -1,    -1,    37,    38,    39,    40,    -1,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    -1,    -1,    -1,    59,    60,    61,    62,    63,    64,
      -1,    -1,    67,    -1,    69,    70,    71,    72,    73,    74,
      75,    76,    -1,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    -1,
      -1,    96,    97,    98,    -1,    -1,   101,   102,   103,   104,
     105,    -1,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,    -1,    -1,    -1,    -1,    -1,
     125,    -1,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,    -1,   138,   139,   140,    -1,    -1,   143,   144,
     145,   146,   147,   148,   149,   150,   151,    -1,   153,   154,
      -1,   156,   157,   158,    -1,    -1,   161,   162,    -1,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
      -1,   176,    -1,    -1,    -1,    -1,    -1,   182,    -1,    -1,
      -1,    -1,    -1,   188,   189,   190,   191,   192,    -1,   194,
     195,   196,   197,   198,   199,   200,   201,   202,   203,   204,
     205,    -1,   207,   208,   209,   210,   211,   212,   213,   214,
     215,   216,   217,   218,   219,   220,   221,   222,   223,    -1,
     225,   226,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,    -1,   239,   240,   241,   242,   243,   244,
     245,    -1,   247,   248,   249,   250,   251,   252,   253,   254,
     255,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
      -1,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
      -1,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,    -1,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,    -1,    -1,    -1,     4,     5,   332,     7,     8,
       9,    10,    11,    12,    13,    14,    -1,    -1,    17,    -1,
      19,    20,    -1,    -1,    23,    -1,    -1,    -1,    27,    28,
      29,    30,    31,    32,    -1,    34,    -1,    -1,    37,    38,
      39,    40,    -1,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    -1,    -1,    -1,
      59,    60,    61,    62,    -1,    64,    -1,    -1,    67,    -1,
      69,    70,    71,    72,    73,    74,    75,    76,    -1,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    -1,    -1,    96,    97,    98,
      -1,    -1,   101,   102,   103,   104,   105,    -1,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,    -1,    -1,    -1,    -1,    -1,   125,    -1,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,    -1,   138,
     139,   140,    -1,    -1,   143,   144,   145,   146,   147,   148,
     149,   150,   151,    -1,   153,   154,    -1,   156,   157,   158,
      -1,    -1,   161,   162,    -1,   164,   165,   166,   167,   168,
     169,   170,   171,   172,   173,   174,    -1,   176,    -1,    -1,
      -1,    -1,    -1,   182,    -1,    -1,    -1,    -1,    -1,   188,
     189,   190,   191,   192,    -1,   194,   195,   196,   197,   198,
     199,   200,   201,   202,   203,   204,   205,    -1,   207,   208,
     209,   210,   211,   212,   213,   214,   215,   216,   217,   218,
     219,   220,   221,   222,   223,    -1,   225,   226,   227,   228,
     229,   230,   231,   232,   233,   234,   235,   236,   237,    -1,
     239,   240,   241,   242,   243,   244,   245,    -1,   247,   248,
     249,   250,   251,   252,   253,   254,   255,   256,   257,   258,
     259,   260,   261,   262,   263,   264,   265,   266,   267,   268,
     269,   270,   271,   272,   273,   274,    -1,   276,   277,   278,
     279,   280,   281,   282,   283,   284,   285,   286,   287,   288,
     289,   290,   291,   292,   293,   294,    -1,   296,   297,   298,
     299,   300,   301,   302,   303,   304,   305,   306,   307,   308,
     309,   310,   311,    -1,   313,   314,   315,   316,   317,   318,
     319,   320,   321,   322,   323,   324,   325,   326,    -1,    -1,
      -1,     4,     5,   332,     7,     8,     9,    10,    11,    12,
      13,    14,    -1,    -1,    17,    -1,    19,    -1,    -1,    -1,
      23,    -1,    -1,    -1,    27,    28,    29,    30,    31,    32,
      -1,    34,    -1,    -1,    37,    38,    39,    40,    -1,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    -1,    -1,    -1,    59,    60,    61,    62,
      -1,    64,    -1,    -1,    67,    -1,    69,    70,    71,    72,
      73,    74,    75,    76,    -1,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    -1,    -1,    96,    97,    98,    -1,    -1,   101,   102,
     103,   104,   105,    -1,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,    -1,    -1,    -1,
      -1,    -1,   125,    -1,   127,   128,   129,   130,   131,   132,
     133,   134,   135,   136,    -1,   138,   139,   140,    -1,    -1,
     143,   144,   145,   146,   147,   148,   149,   150,   151,    -1,
     153,   154,    -1,   156,   157,   158,    -1,    -1,   161,   162,
      -1,   164,   165,   166,   167,   168,   169,   170,   171,   172,
     173,   174,    -1,   176,    -1,    -1,    -1,    -1,    -1,   182,
      -1,    -1,    -1,    -1,    -1,   188,   189,   190,   191,   192,
      -1,   194,   195,   196,   197,   198,   199,   200,   201,   202,
     203,   204,   205,    -1,   207,   208,   209,   210,   211,   212,
     213,   214,   215,   216,   217,   218,   219,   220,   221,   222,
     223,    -1,   225,   226,   227,   228,   229,   230,   231,   232,
     233,   234,   235,   236,   237,    -1,   239,   240,   241,   242,
     243,   244,   245,   246,   247,   248,   249,   250,   251,   252,
     253,   254,   255,   256,   257,   258,   259,   260,   261,   262,
     263,   264,   265,   266,   267,   268,   269,   270,   271,   272,
     273,   274,    -1,   276,   277,   278,   279,   280,   281,   282,
     283,   284,   285,   286,   287,   288,   289,   290,   291,   292,
     293,   294,    -1,   296,   297,   298,   299,   300,   301,   302,
     303,   304,   305,   306,   307,   308,   309,   310,   311,    -1,
     313,   314,   315,   316,   317,   318,   319,   320,   321,   322,
     323,   324,   325,   326,    -1,    -1,    -1,     4,     5,   332,
       7,     8,     9,    10,    11,    12,    13,    14,    -1,    -1,
      17,    -1,    19,    20,    -1,    -1,    23,    -1,    -1,    -1,
      27,    28,    29,    30,    31,    32,    -1,    34,    -1,    -1,
      37,    38,    39,    40,    -1,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    -1,
      -1,    -1,    59,    60,    61,    62,    -1,    64,    -1,    -1,
      67,    -1,    69,    70,    71,    72,    73,    74,    75,    76,
      -1,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    -1,    -1,    96,
      97,    98,    -1,    -1,   101,   102,   103,   104,   105,    -1,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,    -1,    -1,    -1,    -1,    -1,   125,    -1,
     127,   128,   129,   130,   131,   132,   133,   134,   135,   136,
      -1,   138,   139,   140,    -1,    -1,   143,   144,   145,   146,
     147,   148,   149,   150,   151,    -1,   153,   154,    -1,   156,
     157,   158,    -1,    -1,   161,   162,    -1,   164,   165,   166,
     167,   168,   169,   170,   171,   172,   173,   174,    -1,   176,
      -1,    -1,    -1,    -1,    -1,   182,    -1,    -1,    -1,    -1,
      -1,   188,   189,   190,   191,   192,    -1,   194,   195,   196,
     197,   198,   199,   200,   201,   202,   203,   204,   205,    -1,
     207,   208,   209,   210,   211,   212,   213,   214,   215,   216,
     217,   218,   219,   220,   221,   222,   223,    -1,   225,   226,
     227,   228,   229,   230,   231,   232,   233,   234,   235,   236,
     237,    -1,   239,   240,   241,   242,   243,   244,   245,    -1,
     247,   248,   249,   250,   251,   252,   253,   254,   255,   256,
     257,   258,   259,   260,   261,   262,   263,   264,   265,   266,
     267,   268,   269,   270,   271,   272,   273,   274,    -1,   276,
     277,   278,   279,   280,   281,   282,   283,   284,   285,   286,
     287,   288,   289,   290,   291,   292,   293,   294,    -1,   296,
     297,   298,   299,   300,   301,   302,   303,   304,   305,   306,
     307,   308,   309,   310,   311,    -1,   313,   314,   315,   316,
     317,   318,   319,   320,   321,   322,   323,   324,   325,   326,
      -1,    -1,    -1,     4,     5,   332,     7,     8,     9,    10,
      11,    12,    13,    14,    -1,    -1,    17,    -1,    19,    -1,
      -1,    -1,    23,    -1,    -1,    -1,    27,    28,    29,    30,
      31,    32,    -1,    34,    -1,    -1,    37,    38,    39,    40,
      -1,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    -1,    -1,    -1,    59,    60,
      61,    62,    -1,    64,    -1,    -1,    67,    -1,    69,    70,
      71,    72,    73,    74,    75,    76,    -1,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    -1,    -1,    96,    97,    98,    -1,    -1,
     101,   102,   103,   104,   105,    -1,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,    -1,
      -1,    -1,    -1,    -1,   125,    -1,   127,   128,   129,   130,
     131,   132,   133,   134,   135,   136,    -1,   138,   139,   140,
      -1,    -1,   143,   144,   145,   146,   147,   148,   149,   150,
     151,    -1,   153,   154,    -1,   156,   157,   158,    -1,    -1,
     161,   162,    -1,   164,   165,   166,   167,   168,   169,   170,
     171,   172,   173,   174,    -1,   176,    -1,    -1,    -1,    -1,
      -1,   182,    -1,    -1,    -1,    -1,    -1,   188,   189,   190,
     191,   192,    -1,   194,   195,   196,   197,   198,   199,   200,
     201,   202,   203,   204,   205,    -1,   207,   208,   209,   210,
     211,   212,   213,   214,   215,   216,   217,   218,   219,   220,
     221,   222,   223,    -1,   225,   226,   227,   228,   229,   230,
     231,   232,   233,   234,   235,   236,   237,    -1,   239,   240,
     241,   242,   243,   244,   245,    -1,   247,   248,   249,   250,
     251,   252,   253,   254,   255,   256,   257,   258,   259,   260,
     261,   262,   263,   264,   265,   266,   267,   268,   269,   270,
     271,   272,   273,   274,    -1,   276,   277,   278,   279,   280,
     281,   282,   283,   284,   285,   286,   287,   288,   289,   290,
     291,   292,   293,   294,    -1,   296,   297,   298,   299,   300,
     301,   302,   303,   304,   305,   306,   307,   308,   309,   310,
     311,    -1,   313,   314,   315,   316,   317,   318,   319,   320,
     321,   322,   323,   324,   325,   326,    -1,    -1,    -1,     4,
       5,   332,     7,     8,     9,    10,    11,    12,    13,    14,
      -1,    -1,    17,    -1,    19,    -1,    -1,    -1,    23,    -1,
      -1,    -1,    27,    28,    29,    30,    31,    32,    -1,    34,
      -1,    -1,    37,    38,    39,    40,    -1,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    -1,    -1,    -1,    59,    60,    61,    62,    -1,    64,
      -1,    -1,    67,    -1,    69,    70,    71,    72,    73,    74,
      75,    76,    -1,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    -1,
      -1,    96,    97,    98,    -1,    -1,   101,   102,   103,   104,
     105,    -1,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,    -1,    -1,    -1,    -1,    -1,
     125,    -1,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,    -1,   138,   139,   140,    -1,    -1,   143,   144,
     145,   146,   147,   148,   149,   150,   151,    -1,   153,   154,
      -1,   156,   157,   158,    -1,    -1,   161,   162,    -1,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
      -1,   176,    -1,    -1,    -1,    -1,    -1,   182,    -1,    -1,
      -1,    -1,    -1,   188,   189,   190,   191,   192,    -1,   194,
     195,   196,   197,   198,   199,   200,   201,   202,   203,   204,
     205,    -1,   207,   208,   209,   210,   211,   212,   213,   214,
     215,   216,   217,   218,   219,   220,   221,   222,   223,    -1,
     225,   226,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,    -1,   239,   240,   241,   242,   243,   244,
     245,    -1,   247,   248,   249,   250,   251,   252,   253,   254,
     255,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
      -1,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
      -1,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,    -1,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,    -1,    -1,    -1,     4,     5,   332,     7,     8,
       9,    10,    11,    12,    13,    14,    -1,    -1,    17,    -1,
      19,    -1,    -1,    -1,    23,    -1,    -1,    -1,    27,    28,
      29,    30,    31,    32,    -1,    34,    -1,    -1,    37,    38,
      39,    40,    -1,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    -1,    -1,    -1,
      59,    60,    61,    62,    -1,    64,    -1,    -1,    67,    -1,
      69,    70,    71,    72,    73,    74,    75,    76,    -1,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    -1,    -1,    96,    97,    98,
      -1,    -1,   101,   102,   103,   104,   105,    -1,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,    -1,    -1,    -1,    -1,    -1,   125,    -1,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,    -1,   138,
     139,   140,    -1,    -1,   143,   144,   145,   146,   147,   148,
     149,   150,   151,    -1,   153,   154,    -1,   156,   157,   158,
      -1,    -1,   161,   162,    -1,   164,   165,   166,   167,   168,
     169,   170,   171,   172,   173,   174,    -1,   176,    -1,    -1,
      -1,    -1,    -1,   182,    -1,    -1,    -1,    -1,    -1,   188,
     189,   190,   191,   192,    -1,   194,   195,   196,   197,   198,
     199,   200,   201,   202,   203,   204,   205,    -1,   207,   208,
     209,   210,   211,   212,   213,   214,   215,   216,   217,   218,
     219,   220,   221,   222,   223,    -1,   225,   226,   227,   228,
     229,   230,   231,   232,   233,   234,   235,   236,   237,    -1,
     239,   240,   241,   242,   243,   244,   245,    -1,   247,   248,
     249,   250,   251,   252,   253,   254,   255,   256,   257,   258,
     259,   260,   261,   262,   263,   264,   265,   266,   267,   268,
     269,   270,   271,   272,   273,   274,    -1,   276,   277,   278,
     279,   280,   281,   282,   283,   284,   285,   286,   287,   288,
     289,   290,   291,   292,   293,   294,    -1,   296,   297,   298,
     299,   300,   301,   302,   303,   304,   305,   306,   307,   308,
     309,   310,   311,    -1,   313,   314,   315,   316,   317,   318,
     319,   320,   321,   322,   323,   324,   325,   326,    -1,    -1,
      -1,     4,     5,   332,     7,     8,     9,    10,    11,    12,
      13,    14,    -1,    -1,    17,    -1,    19,    -1,    -1,    -1,
      23,    -1,    -1,    -1,    27,    28,    29,    30,    31,    32,
      -1,    34,    -1,    -1,    37,    38,    39,    40,    -1,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    -1,    -1,    -1,    59,    60,    61,    62,
      -1,    64,    -1,    -1,    67,    -1,    69,    70,    71,    72,
      73,    74,    75,    76,    -1,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    -1,    -1,    96,    97,    98,    -1,    -1,   101,   102,
     103,   104,   105,    -1,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,    -1,    -1,    -1,
      -1,    -1,   125,    -1,   127,   128,   129,   130,   131,   132,
     133,   134,   135,   136,    -1,   138,   139,   140,    -1,    -1,
     143,   144,   145,   146,   147,   148,   149,   150,   151,    -1,
     153,   154,    -1,   156,   157,   158,    -1,    -1,   161,   162,
      -1,   164,   165,   166,   167,   168,   169,   170,   171,   172,
     173,   174,    -1,   176,    -1,    -1,    -1,    -1,    -1,   182,
      -1,    -1,    -1,    -1,    -1,   188,   189,   190,   191,   192,
      -1,   194,   195,   196,   197,   198,   199,   200,   201,   202,
     203,   204,   205,    -1,   207,   208,   209,   210,   211,   212,
     213,   214,   215,   216,   217,   218,   219,   220,   221,   222,
     223,    -1,   225,   226,   227,   228,   229,   230,   231,   232,
     233,   234,   235,   236,   237,    -1,   239,   240,   241,   242,
     243,   244,   245,    -1,   247,   248,   249,   250,   251,   252,
     253,   254,   255,   256,   257,   258,   259,   260,   261,   262,
     263,   264,   265,   266,   267,   268,   269,   270,   271,   272,
     273,   274,    -1,   276,   277,   278,   279,   280,   281,   282,
     283,   284,   285,   286,   287,   288,   289,   290,   291,   292,
     293,   294,    -1,   296,   297,   298,   299,   300,   301,   302,
     303,   304,   305,   306,   307,   308,   309,   310,   311,    -1,
     313,   314,   315,   316,   317,   318,   319,   320,   321,   322,
     323,   324,   325,   326,    -1,    -1,    -1,     4,     5,   332,
       7,     8,     9,    10,    11,    12,    13,    14,    -1,    -1,
      17,    -1,    19,    -1,    -1,    -1,    23,    -1,    -1,    -1,
      27,    28,    29,    30,    31,    32,    -1,    34,    -1,    -1,
      37,    38,    39,    40,    -1,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    -1,
      -1,    -1,    59,    60,    61,    62,    -1,    64,    -1,    -1,
      67,    -1,    69,    70,    71,    72,    73,    74,    75,    76,
      -1,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    -1,    -1,    96,
      97,    98,    -1,    -1,   101,   102,   103,   104,   105,    -1,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,    -1,    -1,    -1,    -1,    -1,   125,    -1,
     127,   128,   129,   130,   131,   132,   133,   134,   135,   136,
      -1,   138,   139,   140,    -1,    -1,   143,   144,   145,   146,
     147,   148,   149,   150,   151,    -1,   153,   154,    -1,   156,
     157,   158,    -1,    -1,   161,   162,    -1,   164,   165,   166,
     167,   168,   169,   170,   171,   172,   173,   174,    -1,   176,
      -1,    -1,    -1,    -1,    -1,   182,    -1,    -1,    -1,    -1,
      -1,   188,   189,   190,   191,   192,    -1,   194,   195,   196,
     197,   198,   199,   200,   201,   202,   203,   204,   205,    -1,
     207,   208,   209,   210,   211,   212,   213,   214,   215,   216,
     217,   218,   219,   220,   221,   222,   223,    -1,   225,   226,
     227,   228,   229,   230,   231,   232,   233,   234,   235,   236,
     237,    -1,   239,   240,   241,   242,   243,   244,   245,    -1,
     247,   248,   249,   250,   251,   252,   253,   254,   255,   256,
     257,   258,   259,   260,   261,   262,   263,   264,   265,   266,
     267,   268,   269,   270,   271,   272,   273,   274,    -1,   276,
     277,   278,   279,   280,   281,   282,   283,   284,   285,   286,
     287,   288,   289,   290,   291,   292,   293,   294,    -1,   296,
     297,   298,   299,   300,   301,   302,   303,   304,   305,   306,
     307,   308,   309,   310,   311,    -1,   313,   314,   315,   316,
     317,   318,   319,   320,   321,   322,   323,   324,   325,   326,
      -1,    -1,    -1,     4,     5,   332,     7,     8,     9,    10,
      11,    12,    13,    14,    -1,    -1,    17,    -1,    19,    -1,
      -1,    -1,    23,    -1,    -1,    -1,    27,    28,    29,    30,
      31,    32,    -1,    34,    -1,    -1,    37,    38,    39,    40,
      -1,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    -1,    -1,    -1,    59,    60,
      61,    62,    -1,    64,    -1,    -1,    67,    -1,    69,    70,
      71,    72,    73,    74,    75,    76,    -1,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    -1,    -1,    96,    97,    98,    -1,    -1,
     101,   102,   103,   104,   105,    -1,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,    -1,
      -1,    -1,    -1,    -1,   125,    -1,   127,   128,   129,   130,
     131,   132,   133,   134,   135,   136,    -1,   138,   139,   140,
      -1,    -1,   143,   144,   145,   146,   147,   148,   149,   150,
     151,    -1,   153,   154,    -1,   156,   157,   158,    -1,    -1,
     161,   162,    -1,   164,   165,   166,   167,   168,   169,   170,
     171,   172,   173,   174,    -1,   176,    -1,    -1,    -1,    -1,
      -1,   182,    -1,    -1,    -1,    -1,    -1,   188,   189,   190,
     191,   192,    -1,   194,   195,   196,   197,   198,   199,   200,
     201,   202,   203,   204,   205,    -1,   207,   208,   209,   210,
     211,   212,   213,   214,   215,   216,   217,   218,   219,   220,
     221,   222,   223,    -1,   225,   226,   227,   228,   229,   230,
     231,   232,   233,   234,   235,   236,   237,    -1,   239,   240,
     241,   242,   243,   244,   245,    -1,   247,   248,   249,   250,
     251,   252,   253,   254,   255,   256,   257,   258,   259,   260,
     261,   262,   263,   264,   265,   266,   267,   268,   269,   270,
     271,   272,   273,   274,    -1,   276,   277,   278,   279,   280,
     281,   282,   283,   284,   285,   286,   287,   288,   289,   290,
     291,   292,   293,   294,    -1,   296,   297,   298,   299,   300,
     301,   302,   303,   304,   305,   306,   307,   308,   309,   310,
     311,    -1,   313,   314,   315,   316,   317,   318,   319,   320,
     321,   322,   323,   324,   325,   326,    -1,    -1,    -1,     4,
       5,   332,     7,     8,     9,    10,    11,    12,    13,    14,
      -1,    -1,    17,    -1,    19,    -1,    -1,    -1,    23,    -1,
      -1,    -1,    27,    28,    29,    30,    31,    32,    -1,    34,
      -1,    -1,    37,    38,    39,    40,    -1,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    -1,    -1,    -1,    59,    60,    61,    62,    -1,    64,
      -1,    -1,    67,    -1,    69,    70,    71,    72,    73,    74,
      75,    76,    -1,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    -1,
      -1,    96,    97,    98,    -1,    -1,   101,   102,   103,   104,
     105,    -1,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,    -1,    -1,    -1,    -1,    -1,
     125,    -1,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,    -1,   138,   139,   140,    -1,    -1,   143,   144,
     145,   146,   147,   148,   149,   150,   151,    -1,   153,   154,
      -1,   156,   157,   158,    -1,    -1,   161,   162,    -1,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
      -1,   176,    -1,    -1,    -1,    -1,    -1,   182,    -1,    -1,
      -1,    -1,    -1,   188,   189,   190,   191,   192,    -1,   194,
     195,   196,   197,   198,   199,   200,   201,   202,   203,   204,
     205,    -1,   207,   208,   209,   210,   211,   212,   213,   214,
     215,   216,   217,   218,   219,   220,   221,   222,   223,    -1,
     225,   226,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,    -1,   239,   240,   241,   242,   243,   244,
     245,    -1,   247,   248,   249,   250,   251,   252,   253,   254,
     255,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
      -1,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
      -1,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,    -1,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,    -1,    -1,    -1,     4,    -1,   332,     7,     8,
       9,    10,    11,    12,    13,    14,    -1,    -1,    17,    -1,
      19,    -1,    -1,    -1,    23,    -1,    -1,    -1,    27,    28,
      29,    30,    31,    32,    -1,    34,    -1,    -1,    37,    38,
      39,    40,    -1,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    -1,    -1,    -1,
      59,    60,    61,    62,    -1,    64,    -1,    -1,    67,    -1,
      69,    70,    71,    72,    73,    74,    75,    76,    -1,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    -1,    -1,    96,    97,    98,
      -1,    -1,   101,   102,   103,   104,   105,    -1,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,    -1,    -1,    -1,    -1,    -1,   125,    -1,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,    -1,   138,
     139,   140,    -1,    -1,   143,   144,   145,   146,   147,   148,
     149,   150,   151,    -1,   153,   154,    -1,   156,   157,   158,
      -1,    -1,   161,   162,    -1,   164,   165,   166,   167,   168,
     169,   170,   171,   172,   173,   174,    -1,   176,    -1,    -1,
      -1,    -1,    -1,   182,    -1,    -1,    -1,    -1,    -1,   188,
     189,   190,   191,   192,    -1,   194,   195,   196,   197,   198,
     199,   200,   201,   202,   203,   204,   205,    -1,   207,   208,
     209,   210,   211,   212,   213,   214,   215,   216,   217,   218,
     219,   220,   221,   222,   223,    -1,   225,   226,   227,   228,
     229,   230,   231,   232,   233,   234,   235,   236,   237,    -1,
     239,   240,   241,   242,   243,   244,   245,    -1,   247,   248,
     249,   250,   251,   252,   253,   254,   255,   256,   257,   258,
     259,   260,   261,   262,   263,   264,   265,   266,   267,   268,
     269,   270,   271,   272,   273,   274,    -1,   276,   277,   278,
     279,   280,   281,   282,   283,   284,   285,   286,   287,   288,
     289,   290,   291,   292,   293,   294,    -1,   296,   297,   298,
     299,   300,   301,   302,   303,   304,   305,   306,   307,   308,
     309,   310,   311,    -1,   313,   314,   315,   316,   317,   318,
     319,   320,   321,   322,   323,   324,   325,   326,    -1,    -1,
      -1,     4,    -1,   332,     7,     8,     9,    10,    11,    12,
      13,    14,    -1,    -1,    17,    -1,    19,    -1,    -1,    -1,
      23,    -1,    -1,    -1,    27,    28,    29,    30,    31,    32,
      -1,    34,    -1,    -1,    37,    38,    39,    40,    -1,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    -1,    -1,    -1,    59,    60,    61,    62,
      -1,    64,    -1,    -1,    67,    -1,    69,    70,    71,    72,
      73,    74,    75,    76,    -1,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    -1,    -1,    96,    97,    98,    -1,    -1,   101,   102,
     103,   104,   105,    -1,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,    -1,    -1,    -1,
      -1,    -1,   125,    -1,   127,   128,   129,   130,   131,   132,
     133,   134,   135,   136,    -1,   138,   139,   140,    -1,    -1,
     143,   144,   145,   146,   147,   148,   149,   150,   151,    -1,
     153,   154,    -1,   156,   157,   158,    -1,    -1,   161,   162,
      -1,   164,   165,   166,   167,   168,   169,   170,   171,   172,
     173,   174,    -1,   176,    -1,    -1,    -1,    -1,    -1,   182,
      -1,    -1,    -1,    -1,    -1,   188,   189,   190,   191,   192,
      -1,   194,   195,   196,   197,   198,   199,   200,   201,   202,
     203,   204,   205,    -1,   207,   208,   209,   210,   211,   212,
     213,   214,   215,   216,   217,   218,   219,   220,   221,   222,
     223,    -1,   225,   226,   227,   228,   229,   230,   231,   232,
     233,   234,   235,   236,   237,    -1,   239,   240,   241,   242,
     243,   244,   245,    -1,   247,   248,   249,   250,   251,   252,
     253,   254,   255,   256,   257,   258,   259,   260,   261,   262,
     263,   264,   265,   266,   267,   268,   269,   270,   271,   272,
     273,   274,    -1,   276,   277,   278,   279,   280,   281,   282,
     283,   284,   285,   286,   287,   288,   289,   290,   291,   292,
     293,   294,    -1,   296,   297,   298,   299,   300,   301,   302,
     303,   304,   305,   306,   307,   308,   309,   310,   311,    -1,
     313,   314,   315,   316,   317,   318,   319,   320,   321,   322,
     323,   324,   325,   326,     4,    -1,    -1,    -1,    -1,   332,
      10,    -1,    12,    13,    14,    -1,    -1,    17,    -1,    19,
      -1,    -1,    -1,    23,    -1,    -1,    -1,    27,    28,    29,
      30,    31,    32,    -1,    34,    -1,    -1,    37,    38,    39,
      40,    -1,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    -1,    -1,    -1,    59,
      60,    61,    62,    -1,    64,    -1,    -1,    67,    -1,    69,
      70,    71,    72,    73,    74,    75,    76,    -1,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    -1,    -1,    96,    97,    98,    -1,
      -1,   101,   102,   103,   104,   105,    -1,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
      -1,    -1,    -1,    -1,    -1,   125,    -1,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,    -1,   138,   139,
     140,    -1,    -1,   143,   144,   145,   146,   147,   148,   149,
     150,   151,    -1,   153,   154,    -1,   156,   157,   158,    -1,
      -1,   161,   162,    -1,   164,   165,   166,   167,   168,   169,
     170,   171,   172,   173,   174,    -1,   176,    -1,    -1,    -1,
      -1,    -1,   182,    -1,    -1,    -1,    -1,    -1,   188,   189,
     190,   191,   192,    -1,   194,   195,   196,   197,   198,   199,
     200,   201,   202,   203,   204,   205,    -1,   207,   208,   209,
     210,   211,   212,   213,   214,   215,   216,   217,   218,   219,
     220,   221,   222,   223,    -1,   225,   226,   227,   228,   229,
     230,   231,   232,   233,   234,   235,   236,   237,    -1,   239,
     240,   241,   242,   243,   244,   245,    -1,   247,   248,   249,
     250,   251,   252,   253,   254,   255,   256,   257,   258,   259,
     260,   261,   262,   263,   264,   265,   266,   267,   268,   269,
     270,   271,   272,   273,   274,    -1,   276,   277,   278,   279,
     280,   281,   282,   283,   284,   285,   286,   287,   288,   289,
     290,   291,   292,   293,   294,    -1,   296,   297,   298,   299,
     300,   301,   302,   303,   304,   305,   306,   307,   308,   309,
     310,   311,    -1,   313,   314,   315,   316,   317,   318,   319,
     320,   321,   322,   323,   324,   325,   326,     4,    -1,    -1,
      -1,    -1,   332,    10,    -1,    -1,    13,    14,    -1,    -1,
      17,    -1,    19,    -1,    -1,    -1,    23,    -1,    -1,    -1,
      27,    28,    29,    30,    31,    32,    -1,    34,    -1,    -1,
      37,    38,    39,    40,    -1,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    -1,
      -1,    -1,    59,    60,    61,    62,    -1,    64,    -1,    -1,
      67,    -1,    69,    70,    71,    72,    73,    74,    75,    76,
      -1,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    -1,    -1,    96,
      97,    98,    -1,    -1,   101,   102,   103,   104,   105,    -1,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,    -1,    -1,    -1,    -1,    -1,   125,    -1,
     127,   128,   129,   130,   131,   132,   133,   134,   135,   136,
      -1,   138,   139,   140,    -1,    -1,   143,   144,   145,   146,
     147,   148,   149,   150,   151,    -1,   153,   154,    -1,   156,
     157,   158,    -1,    -1,   161,   162,    -1,   164,   165,   166,
     167,   168,   169,   170,   171,   172,   173,   174,    -1,   176,
      -1,    -1,    -1,    -1,    -1,   182,    -1,    -1,    -1,    -1,
      -1,   188,   189,   190,   191,   192,    -1,   194,   195,   196,
     197,   198,   199,   200,   201,   202,   203,   204,   205,    -1,
     207,   208,   209,   210,   211,   212,   213,   214,   215,   216,
     217,   218,   219,   220,   221,   222,   223,    -1,   225,   226,
     227,   228,   229,   230,   231,   232,   233,   234,   235,   236,
     237,    -1,   239,   240,   241,   242,   243,   244,   245,    -1,
     247,   248,   249,   250,   251,   252,   253,   254,   255,   256,
     257,   258,   259,   260,   261,   262,   263,   264,   265,   266,
     267,   268,   269,   270,   271,   272,   273,   274,    -1,   276,
     277,   278,   279,   280,   281,   282,   283,   284,   285,   286,
     287,   288,   289,   290,   291,   292,   293,   294,    -1,   296,
     297,   298,   299,   300,   301,   302,   303,   304,   305,   306,
     307,   308,   309,   310,   311,    -1,   313,   314,   315,   316,
     317,   318,   319,   320,   321,   322,   323,   324,   325,   326,
       4,    -1,    -1,    -1,    -1,   332,    10,    -1,    -1,    -1,
      14,    -1,    -1,    -1,    -1,    19,    -1,    -1,    -1,    23,
      -1,    -1,    -1,    27,    28,    29,    30,    31,    32,    -1,
      34,    -1,    -1,    37,    38,    39,    40,    -1,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    -1,    -1,    -1,    59,    60,    61,    62,    -1,
      64,    -1,    -1,    67,    -1,    69,    70,    71,    72,    73,
      74,    75,    76,    -1,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      -1,    -1,    96,    97,    98,    -1,    -1,   101,   102,   103,
     104,   105,    -1,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,    -1,    -1,    -1,    -1,
      -1,   125,    -1,   127,   128,   129,   130,   131,   132,   133,
     134,   135,   136,    -1,   138,   139,   140,    -1,    -1,   143,
     144,   145,   146,   147,   148,   149,   150,   151,    -1,   153,
     154,    -1,   156,   157,   158,    -1,    -1,   161,   162,    -1,
     164,   165,   166,   167,   168,   169,   170,   171,   172,   173,
     174,    -1,   176,    -1,    -1,    -1,    -1,    -1,   182,    -1,
      -1,    -1,    -1,    -1,   188,   189,   190,   191,   192,    -1,
     194,   195,   196,   197,   198,   199,   200,   201,   202,   203,
     204,   205,    -1,   207,   208,   209,   210,   211,   212,   213,
     214,   215,   216,   217,   218,   219,   220,   221,   222,   223,
      -1,   225,   226,   227,   228,   229,   230,   231,   232,   233,
     234,   235,   236,   237,    -1,   239,   240,   241,   242,   243,
     244,   245,    -1,   247,   248,   249,   250,   251,   252,   253,
     254,   255,   256,   257,   258,   259,   260,   261,   262,   263,
     264,   265,   266,   267,   268,   269,   270,   271,   272,   273,
     274,    -1,   276,   277,   278,   279,   280,   281,   282,   283,
     284,   285,   286,   287,   288,   289,   290,   291,   292,   293,
     294,    -1,   296,   297,   298,   299,   300,   301,   302,   303,
     304,   305,   306,   307,   308,   309,   310,   311,    -1,   313,
     314,   315,   316,   317,   318,   319,   320,   321,   322,   323,
     324,   325,   326,    -1,    -1,    -1,    -1,     5,   332,     7,
       8,     9,    10,    11,    12,    13,    14,    -1,    -1,    17,
      -1,    19,    -1,    -1,    -1,    23,    -1,    -1,    -1,    27,
      28,    29,    30,    31,    32,    -1,    34,    -1,    -1,    37,
      38,    39,    40,    -1,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    -1,    -1,
      -1,    59,    60,    61,    62,    -1,    64,    -1,    -1,    67,
      -1,    69,    70,    71,    72,    73,    74,    75,    76,    -1,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    -1,    -1,    96,    97,
      98,    -1,    -1,   101,   102,   103,   104,   105,    -1,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,    -1,    -1,    -1,    -1,    -1,   125,    -1,   127,
     128,   129,   130,   131,   132,   133,   134,   135,   136,    -1,
     138,   139,   140,    -1,    -1,   143,   144,   145,   146,   147,
     148,   149,   150,   151,    -1,   153,   154,    -1,   156,   157,
     158,    -1,    -1,   161,   162,    -1,   164,   165,   166,   167,
     168,   169,   170,   171,   172,   173,   174,    -1,   176,    -1,
      -1,    -1,    -1,    -1,   182,    -1,    -1,    -1,    -1,    -1,
     188,   189,   190,   191,   192,    -1,   194,   195,   196,   197,
     198,   199,   200,   201,   202,   203,   204,   205,    -1,   207,
     208,   209,   210,   211,   212,   213,   214,   215,   216,   217,
     218,   219,   220,   221,   222,   223,    -1,   225,   226,   227,
     228,   229,   230,   231,   232,   233,   234,   235,   236,   237,
      -1,   239,   240,   241,   242,   243,   244,   245,    -1,   247,
     248,   249,   250,   251,   252,   253,   254,   255,   256,   257,
     258,   259,   260,   261,   262,   263,   264,   265,   266,   267,
     268,   269,   270,   271,   272,   273,   274,    -1,   276,   277,
     278,   279,   280,   281,   282,   283,   284,   285,   286,   287,
     288,   289,   290,   291,   292,   293,   294,    -1,   296,   297,
     298,   299,   300,   301,   302,   303,   304,   305,   306,   307,
     308,   309,   310,   311,    -1,   313,   314,   315,   316,   317,
     318,   319,   320,   321,   322,   323,   324,   325,   326,    -1,
      -1,     3,     4,     5,   332,     7,     8,     9,    10,    11,
      12,    -1,    -1,    -1,    -1,    17,    -1,    -1,    -1,    -1,
      -1,    23,    -1,    -1,    -1,    27,    28,    29,    30,    31,
      32,    -1,    34,    -1,    -1,    37,    38,    39,    40,    -1,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    -1,    -1,    -1,    59,    -1,    61,
      62,    -1,    64,    -1,    -1,    67,    -1,    69,    70,    71,
      72,    73,    74,    75,    76,    -1,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    -1,    -1,    96,    97,    98,    -1,    -1,   101,
     102,   103,   104,   105,    -1,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,    -1,    -1,
      -1,    -1,    -1,   125,    -1,   127,   128,   129,   130,   131,
     132,   133,   134,   135,   136,    -1,   138,   139,   140,    -1,
      -1,   143,   144,   145,   146,   147,   148,   149,   150,   151,
      -1,   153,   154,    -1,   156,   157,   158,    -1,    -1,   161,
     162,    -1,   164,   165,   166,   167,   168,   169,   170,   171,
     172,   173,   174,   175,   176,    -1,    -1,    -1,   180,   181,
     182,   183,   184,   185,   186,    -1,   188,   189,   190,   191,
     192,    -1,   194,   195,   196,   197,   198,   199,   200,   201,
     202,   203,   204,   205,    -1,   207,   208,   209,   210,   211,
     212,   213,   214,   215,   216,   217,   218,   219,   220,   221,
     222,   223,    -1,   225,   226,   227,   228,   229,   230,   231,
     232,   233,   234,   235,   236,   237,    -1,   239,   240,   241,
     242,   243,   244,   245,    -1,   247,   248,   249,   250,   251,
     252,   253,   254,   255,   256,   257,   258,   259,   260,   261,
     262,   263,   264,   265,   266,   267,   268,   269,   270,   271,
     272,   273,   274,    -1,   276,   277,   278,   279,   280,   281,
     282,   283,   284,   285,   286,   287,   288,   289,   290,   291,
     292,   293,   294,    -1,   296,   297,   298,   299,   300,   301,
     302,   303,   304,   305,   306,   307,   308,   309,   310,   311,
      -1,   313,   314,   315,   316,   317,   318,   319,   320,   321,
     322,   323,   324,   325,   326,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    -1,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    -1,    -1,    27,    28,
      29,    30,    31,    32,    -1,    34,    -1,    -1,    37,    38,
      39,    40,    -1,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    -1,    -1,    -1,
      59,    -1,    61,    62,    -1,    64,    -1,    -1,    67,    -1,
      69,    70,    71,    72,    73,    74,    75,    76,    -1,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    -1,    -1,    96,    97,    98,
      -1,    -1,   101,   102,   103,   104,   105,    -1,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,    -1,    -1,    -1,    -1,    -1,   125,    -1,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,    -1,   138,
     139,   140,    -1,    -1,   143,   144,   145,   146,   147,   148,
     149,   150,   151,    -1,   153,   154,    -1,   156,   157,   158,
      -1,    -1,   161,   162,    -1,   164,   165,   166,   167,   168,
     169,   170,   171,   172,   173,   174,    -1,   176,    -1,    -1,
      -1,    -1,    -1,   182,    -1,    -1,    -1,    -1,    -1,   188,
     189,   190,   191,   192,    -1,   194,   195,   196,   197,   198,
     199,   200,   201,   202,   203,   204,   205,    -1,   207,   208,
     209,   210,   211,   212,   213,   214,   215,   216,   217,   218,
     219,   220,   221,   222,   223,    -1,   225,   226,   227,   228,
     229,   230,   231,   232,   233,   234,   235,   236,   237,    -1,
     239,   240,   241,   242,   243,   244,   245,    -1,   247,   248,
     249,   250,   251,   252,   253,   254,   255,   256,   257,   258,
     259,   260,   261,   262,   263,   264,   265,   266,   267,   268,
     269,   270,   271,   272,   273,   274,    -1,   276,   277,   278,
     279,   280,   281,   282,   283,   284,   285,   286,   287,   288,
     289,   290,   291,   292,   293,   294,    -1,   296,   297,   298,
     299,   300,   301,   302,   303,   304,   305,   306,   307,   308,
     309,   310,   311,    -1,   313,   314,   315,   316,   317,   318,
     319,   320,   321,   322,   323,   324,   325,   326,     4,     5,
      -1,     7,     8,     9,    10,    11,    12,    -1,    -1,    -1,
      -1,    17,    -1,    -1,    -1,    -1,    -1,    23,    -1,    -1,
      -1,    27,    28,    29,    30,    31,    32,    -1,    34,    -1,
      -1,    37,    38,    39,    40,    -1,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      -1,    -1,    -1,    59,    -1,    61,    62,    -1,    64,    -1,
      -1,    67,    -1,    69,    70,    71,    72,    73,    74,    75,
      76,    -1,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    -1,    -1,
      96,    97,    98,    -1,    -1,   101,   102,   103,   104,   105,
      -1,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,    -1,    -1,    -1,    -1,    -1,   125,
      -1,   127,   128,   129,   130,   131,   132,   133,   134,   135,
     136,    -1,   138,   139,   140,    -1,    -1,   143,   144,   145,
     146,   147,   148,   149,   150,   151,    -1,   153,   154,    -1,
     156,   157,   158,    -1,    -1,   161,   162,    -1,   164,   165,
     166,   167,   168,   169,   170,   171,   172,   173,   174,   175,
     176,    -1,    -1,    -1,   180,   181,   182,   183,   184,   185,
     186,    -1,   188,   189,   190,   191,   192,    -1,   194,   195,
     196,   197,   198,   199,   200,   201,   202,   203,   204,   205,
      -1,   207,   208,   209,   210,   211,   212,   213,   214,   215,
     216,   217,   218,   219,   220,   221,   222,   223,    -1,   225,
     226,   227,   228,   229,   230,   231,   232,   233,   234,   235,
     236,   237,    -1,   239,   240,   241,   242,   243,   244,   245,
      -1,   247,   248,   249,   250,   251,   252,   253,   254,   255,
     256,   257,   258,   259,   260,   261,   262,   263,   264,   265,
     266,   267,   268,   269,   270,   271,   272,   273,   274,    -1,
     276,   277,   278,   279,   280,   281,   282,   283,   284,   285,
     286,   287,   288,   289,   290,   291,   292,   293,   294,    -1,
     296,   297,   298,   299,   300,   301,   302,   303,   304,   305,
     306,   307,   308,   309,   310,   311,    -1,   313,   314,   315,
     316,   317,   318,   319,   320,   321,   322,   323,   324,   325,
     326,     4,     5,    -1,     7,     8,     9,    10,    11,    12,
      -1,    -1,    -1,    -1,    17,    -1,    -1,    -1,    -1,    -1,
      23,    -1,    -1,    -1,    27,    28,    29,    30,    31,    32,
      -1,    34,    -1,    -1,    37,    38,    39,    40,    -1,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    -1,    -1,    -1,    59,    -1,    61,    62,
      -1,    64,    -1,    -1,    67,    -1,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    -1,    -1,    96,    97,    98,    -1,    -1,   101,   102,
     103,   104,   105,    -1,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,    -1,    -1,    -1,
      -1,    -1,   125,    -1,   127,   128,   129,   130,   131,   132,
     133,   134,   135,   136,    -1,   138,   139,   140,    -1,    -1,
     143,   144,   145,   146,   147,   148,   149,   150,   151,    -1,
     153,   154,    -1,   156,   157,   158,    -1,    -1,   161,   162,
      -1,   164,   165,   166,   167,   168,   169,   170,   171,   172,
     173,   174,    -1,   176,    -1,    -1,    -1,    -1,    -1,   182,
      -1,    -1,    -1,    -1,    -1,   188,   189,   190,   191,   192,
      -1,   194,   195,   196,   197,   198,   199,   200,   201,   202,
     203,   204,   205,    -1,   207,   208,   209,   210,   211,   212,
     213,   214,   215,   216,   217,   218,   219,   220,   221,   222,
     223,    -1,   225,   226,   227,   228,   229,   230,   231,   232,
     233,   234,   235,   236,   237,    -1,   239,   240,   241,   242,
     243,   244,   245,    -1,   247,   248,   249,   250,   251,   252,
     253,   254,   255,   256,   257,   258,   259,   260,   261,   262,
     263,   264,   265,   266,   267,   268,   269,   270,   271,   272,
     273,   274,    -1,   276,   277,   278,   279,   280,   281,   282,
     283,   284,   285,   286,   287,   288,   289,   290,   291,   292,
     293,   294,    -1,   296,   297,   298,   299,   300,   301,   302,
     303,   304,   305,   306,   307,   308,   309,   310,   311,    -1,
     313,   314,   315,   316,   317,   318,   319,   320,   321,   322,
     323,   324,   325,   326,     4,     5,    -1,     7,     8,     9,
      10,    11,    12,    -1,    -1,    -1,    -1,    17,    -1,    -1,
      -1,    -1,    -1,    23,    -1,    -1,    26,    27,    28,    29,
      30,    31,    32,    -1,    34,    -1,    -1,    37,    38,    39,
      40,    -1,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    -1,    -1,    -1,    59,
      -1,    61,    62,    -1,    64,    -1,    -1,    67,    -1,    69,
      70,    71,    72,    73,    74,    75,    76,    -1,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    -1,    -1,    96,    97,    98,    -1,
      -1,   101,   102,   103,   104,   105,    -1,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
      -1,    -1,    -1,    -1,    -1,   125,    -1,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,    -1,   138,   139,
     140,    -1,    -1,   143,   144,   145,   146,   147,   148,   149,
     150,   151,    -1,   153,   154,    -1,   156,   157,   158,    -1,
      -1,   161,   162,    -1,   164,   165,   166,   167,   168,   169,
     170,   171,   172,   173,   174,    -1,   176,    -1,    -1,    -1,
      -1,    -1,   182,    -1,    -1,    -1,    -1,    -1,   188,   189,
     190,   191,   192,    -1,   194,   195,   196,   197,   198,   199,
     200,   201,   202,   203,   204,   205,    -1,   207,   208,   209,
     210,   211,   212,   213,   214,   215,   216,   217,   218,   219,
     220,   221,   222,   223,    -1,   225,   226,   227,   228,   229,
     230,   231,   232,   233,   234,   235,   236,   237,    -1,   239,
     240,   241,   242,   243,   244,   245,    -1,   247,   248,   249,
     250,   251,   252,   253,   254,   255,   256,   257,   258,   259,
     260,   261,   262,   263,   264,   265,   266,   267,   268,   269,
     270,   271,   272,   273,   274,    -1,   276,   277,   278,   279,
     280,   281,   282,   283,   284,   285,   286,   287,   288,   289,
     290,   291,   292,   293,   294,    -1,   296,   297,   298,   299,
     300,   301,   302,   303,   304,   305,   306,   307,   308,   309,
     310,   311,    -1,   313,   314,   315,   316,   317,   318,   319,
     320,   321,   322,   323,   324,   325,   326,     4,     5,    -1,
       7,     8,     9,    10,    11,    12,    -1,    -1,    -1,    -1,
      17,    -1,    -1,    -1,    -1,    -1,    23,    -1,    -1,    -1,
      27,    28,    29,    30,    31,    32,    -1,    34,    -1,    -1,
      37,    38,    39,    40,    -1,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    -1,
      -1,    -1,    59,    -1,    61,    62,    -1,    64,    -1,    -1,
      67,    -1,    69,    70,    71,    72,    73,    74,    75,    76,
      -1,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    -1,    -1,    96,
      97,    98,    -1,    -1,   101,   102,   103,   104,   105,    -1,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,    -1,    -1,    -1,    -1,    -1,   125,    -1,
     127,   128,   129,   130,   131,   132,   133,   134,   135,   136,
      -1,   138,   139,   140,    -1,    -1,   143,   144,   145,   146,
     147,   148,   149,   150,   151,    -1,   153,   154,    -1,   156,
     157,   158,    -1,    -1,   161,   162,    -1,   164,   165,   166,
     167,   168,   169,   170,   171,   172,   173,   174,    -1,   176,
      -1,    -1,    -1,    -1,    -1,   182,    -1,    -1,    -1,    -1,
      -1,   188,   189,   190,   191,   192,    -1,   194,   195,   196,
     197,   198,   199,   200,   201,   202,   203,   204,   205,    -1,
     207,   208,   209,   210,   211,   212,   213,   214,   215,   216,
     217,   218,   219,   220,   221,   222,   223,    -1,   225,   226,
     227,   228,   229,   230,   231,   232,   233,   234,   235,   236,
     237,    -1,   239,   240,   241,   242,   243,   244,   245,    -1,
     247,   248,   249,   250,   251,   252,   253,   254,   255,   256,
     257,   258,   259,   260,   261,   262,   263,   264,   265,   266,
     267,   268,   269,   270,   271,   272,   273,   274,    -1,   276,
     277,   278,   279,   280,   281,   282,   283,   284,   285,   286,
     287,   288,   289,   290,   291,   292,   293,   294,    -1,   296,
     297,   298,   299,   300,   301,   302,   303,   304,   305,   306,
     307,   308,   309,   310,   311,    -1,   313,   314,   315,   316,
     317,   318,   319,   320,   321,   322,   323,   324,   325,   326,
       4,     5,    -1,     7,     8,     9,    10,    11,    12,    -1,
      -1,    -1,    -1,    17,    -1,    -1,    -1,    -1,    -1,    23,
      -1,    -1,    -1,    27,    28,    29,    30,    31,    32,    -1,
      34,    -1,    -1,    37,    38,    39,    40,    -1,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    -1,    -1,    -1,    59,    -1,    61,    62,    -1,
      64,    -1,    -1,    67,    -1,    69,    70,    71,    72,    73,
      74,    75,    76,    -1,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      -1,    -1,    96,    97,    98,    -1,    -1,   101,   102,   103,
     104,   105,    -1,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,    -1,    -1,    -1,    -1,
      -1,   125,    -1,   127,   128,   129,   130,   131,   132,   133,
     134,   135,   136,    -1,   138,   139,   140,    -1,    -1,   143,
     144,   145,   146,   147,   148,   149,   150,   151,    -1,   153,
     154,    -1,   156,   157,   158,    -1,    -1,   161,   162,    -1,
     164,   165,   166,   167,   168,   169,   170,   171,   172,   173,
     174,    -1,   176,    -1,    -1,    -1,    -1,    -1,   182,    -1,
      -1,    -1,    -1,    -1,   188,   189,   190,   191,   192,    -1,
     194,   195,   196,   197,   198,   199,   200,   201,   202,   203,
     204,   205,    -1,   207,   208,   209,   210,   211,   212,   213,
     214,   215,   216,   217,   218,   219,   220,   221,   222,   223,
      -1,   225,   226,   227,   228,   229,   230,   231,   232,   233,
     234,   235,   236,   237,    -1,   239,   240,   241,   242,   243,
     244,   245,    -1,   247,   248,   249,   250,   251,   252,   253,
     254,   255,   256,   257,   258,   259,   260,   261,   262,   263,
     264,   265,   266,   267,   268,   269,   270,   271,   272,   273,
     274,    -1,   276,   277,   278,   279,   280,   281,   282,   283,
     284,   285,   286,   287,   288,   289,   290,   291,   292,   293,
     294,    -1,   296,   297,   298,   299,   300,   301,   302,   303,
     304,   305,   306,   307,   308,   309,   310,   311,    -1,   313,
     314,   315,   316,   317,   318,   319,   320,   321,   322,   323,
     324,   325,   326,     4,     5,    -1,     7,     8,     9,    10,
      11,    12,    -1,    -1,    -1,    -1,    17,    -1,    -1,    -1,
      -1,    -1,    23,    -1,    -1,    -1,    27,    28,    29,    30,
      31,    32,    -1,    34,    -1,    -1,    37,    38,    39,    40,
      -1,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    -1,    -1,    -1,    59,    -1,
      61,    62,    -1,    64,    -1,    -1,    67,    -1,    69,    70,
      71,    72,    73,    74,    75,    76,    -1,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    -1,    -1,    96,    97,    98,    -1,    -1,
     101,   102,   103,   104,   105,    -1,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,    -1,
      -1,    -1,    -1,    -1,   125,    -1,   127,   128,   129,   130,
     131,   132,   133,   134,   135,   136,    -1,   138,   139,   140,
      -1,    -1,   143,   144,   145,   146,   147,   148,   149,   150,
     151,    -1,   153,   154,    -1,   156,   157,   158,    -1,    -1,
     161,   162,    -1,   164,   165,   166,   167,   168,   169,   170,
     171,   172,   173,   174,    -1,   176,    -1,    -1,    -1,    -1,
      -1,   182,    -1,    -1,    -1,    -1,    -1,   188,   189,   190,
     191,   192,    -1,   194,   195,   196,   197,   198,   199,   200,
     201,   202,   203,   204,   205,    -1,   207,   208,   209,   210,
     211,   212,   213,   214,   215,   216,   217,   218,   219,   220,
     221,   222,   223,    -1,   225,   226,   227,   228,   229,   230,
     231,   232,   233,   234,   235,   236,   237,    -1,   239,   240,
     241,   242,   243,   244,   245,    -1,   247,   248,   249,   250,
     251,   252,   253,   254,   255,   256,   257,   258,   259,   260,
     261,   262,   263,   264,   265,   266,   267,   268,   269,   270,
     271,   272,   273,   274,    -1,   276,   277,   278,   279,   280,
     281,   282,   283,   284,   285,   286,   287,   288,   289,   290,
     291,   292,   293,   294,    -1,   296,   297,   298,   299,   300,
     301,   302,   303,   304,   305,   306,   307,   308,   309,   310,
     311,    -1,   313,   314,   315,   316,   317,   318,   319,   320,
     321,   322,   323,   324,   325,   326,     4,     5,    -1,     7,
       8,     9,    10,    11,    12,    -1,    -1,    -1,    -1,    17,
      -1,    -1,    -1,    -1,    -1,    23,    -1,    -1,    -1,    27,
      28,    29,    30,    31,    32,    -1,    34,    -1,    -1,    37,
      38,    39,    40,    -1,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    -1,    -1,
      -1,    59,    -1,    61,    62,    -1,    64,    -1,    -1,    67,
      -1,    69,    70,    71,    72,    73,    74,    75,    76,    -1,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    -1,    -1,    96,    97,
      98,    -1,    -1,   101,   102,   103,   104,   105,    -1,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,    -1,    -1,    -1,    -1,    -1,   125,    -1,   127,
     128,   129,   130,   131,   132,   133,   134,   135,   136,    -1,
     138,   139,   140,    -1,    -1,   143,   144,   145,   146,   147,
     148,   149,   150,   151,    -1,   153,   154,    -1,   156,   157,
     158,    -1,    -1,   161,   162,    -1,   164,   165,   166,   167,
     168,   169,   170,   171,   172,   173,   174,    -1,   176,    -1,
      -1,    -1,    -1,    -1,   182,    -1,    -1,    -1,    -1,    -1,
     188,   189,   190,   191,   192,    -1,   194,   195,   196,   197,
     198,   199,   200,   201,   202,   203,   204,   205,    -1,   207,
     208,   209,   210,   211,   212,   213,   214,   215,   216,   217,
     218,   219,   220,   221,   222,   223,    -1,   225,   226,   227,
     228,   229,   230,   231,   232,   233,   234,   235,   236,   237,
      -1,   239,   240,   241,   242,   243,   244,   245,    -1,   247,
     248,   249,   250,   251,   252,   253,   254,   255,   256,   257,
     258,   259,   260,   261,   262,   263,   264,   265,   266,   267,
     268,   269,   270,   271,   272,   273,   274,    -1,   276,   277,
     278,   279,   280,   281,   282,   283,   284,   285,   286,   287,
     288,   289,   290,   291,   292,   293,   294,    -1,   296,   297,
     298,   299,   300,   301,   302,   303,   304,   305,   306,   307,
     308,   309,   310,   311,    -1,   313,   314,   315,   316,   317,
     318,   319,   320,   321,   322,   323,   324,   325,   326,     4,
       5,    -1,     7,     8,     9,    10,    11,    12,    -1,    -1,
      -1,    -1,    17,    -1,    -1,    -1,    -1,    -1,    23,    -1,
      -1,    -1,    27,    28,    29,    30,    31,    32,    -1,    34,
      -1,    -1,    37,    38,    39,    40,    -1,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    -1,    -1,    -1,    59,    -1,    61,    62,    -1,    64,
      -1,    -1,    67,    -1,    69,    70,    71,    72,    73,    74,
      75,    76,    -1,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    -1,
      -1,    96,    97,    98,    -1,    -1,   101,   102,   103,   104,
     105,    -1,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,    -1,    -1,    -1,    -1,    -1,
     125,    -1,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,    -1,   138,   139,   140,    -1,    -1,   143,   144,
     145,   146,   147,   148,   149,   150,   151,    -1,   153,   154,
      -1,   156,   157,   158,    -1,    -1,   161,   162,    -1,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
      -1,   176,    -1,    -1,    -1,    -1,    -1,   182,    -1,    -1,
      -1,    -1,    -1,   188,   189,   190,   191,   192,    -1,   194,
     195,   196,   197,   198,   199,   200,   201,   202,   203,   204,
     205,    -1,   207,   208,   209,   210,   211,   212,   213,   214,
     215,   216,   217,   218,   219,   220,   221,   222,   223,    -1,
     225,   226,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,    -1,   239,   240,   241,   242,   243,   244,
     245,    -1,   247,   248,   249,   250,   251,   252,   253,   254,
     255,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
      -1,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
      -1,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,    -1,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,     4,     5,    -1,     7,     8,     9,    10,    11,
      12,    -1,    -1,    -1,    -1,    17,    -1,    -1,    -1,    -1,
      -1,    23,    -1,    -1,    -1,    27,    28,    29,    30,    31,
      32,    -1,    34,    -1,    -1,    37,    38,    39,    40,    -1,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    -1,    -1,    -1,    59,    -1,    61,
      62,    -1,    64,    -1,    -1,    67,    -1,    69,    70,    71,
      72,    73,    74,    75,    76,    -1,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    -1,    -1,    96,    97,    98,    -1,    -1,   101,
     102,   103,   104,   105,    -1,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,    -1,    -1,
      -1,    -1,    -1,   125,    -1,   127,   128,   129,   130,   131,
     132,   133,   134,   135,   136,    -1,   138,   139,   140,    -1,
      -1,   143,   144,   145,   146,   147,   148,   149,   150,   151,
      -1,   153,   154,    -1,   156,   157,   158,    -1,    -1,   161,
     162,    -1,   164,   165,   166,   167,   168,   169,   170,   171,
     172,   173,   174,    -1,   176,    -1,    -1,    -1,    -1,    -1,
     182,    -1,    -1,    -1,    -1,    -1,   188,   189,   190,   191,
     192,    -1,   194,   195,   196,   197,   198,   199,   200,   201,
     202,   203,   204,   205,    -1,   207,   208,   209,   210,   211,
     212,   213,   214,   215,   216,   217,   218,   219,   220,   221,
     222,   223,    -1,   225,   226,   227,   228,   229,   230,   231,
     232,   233,   234,   235,   236,   237,    -1,   239,   240,   241,
     242,   243,   244,   245,    -1,   247,   248,   249,   250,   251,
     252,   253,   254,   255,   256,   257,   258,   259,   260,   261,
     262,   263,   264,   265,   266,   267,   268,   269,   270,   271,
     272,   273,   274,    -1,   276,   277,   278,   279,   280,   281,
     282,   283,   284,   285,   286,   287,   288,   289,   290,   291,
     292,   293,   294,    -1,   296,   297,   298,   299,   300,   301,
     302,   303,   304,   305,   306,   307,   308,   309,   310,   311,
      -1,   313,   314,   315,   316,   317,   318,   319,   320,   321,
     322,   323,   324,   325,   326,     4,     5,    -1,     7,     8,
       9,    10,    11,    12,    -1,    -1,    -1,    -1,    17,    -1,
      -1,    -1,    -1,    -1,    23,    -1,    -1,    -1,    27,    28,
      29,    30,    31,    32,    -1,    34,    -1,    -1,    37,    38,
      39,    40,    -1,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    -1,    -1,    -1,
      59,    -1,    61,    62,    -1,    64,    -1,    -1,    67,    -1,
      69,    70,    71,    72,    73,    74,    75,    76,    -1,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    -1,    -1,    96,    97,    98,
      -1,    -1,   101,   102,   103,   104,   105,    -1,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,    -1,    -1,    -1,    -1,    -1,   125,    -1,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,    -1,   138,
     139,   140,    -1,    -1,   143,   144,   145,   146,   147,   148,
     149,   150,   151,    -1,   153,   154,    -1,   156,   157,   158,
      -1,    -1,   161,   162,    -1,   164,   165,   166,   167,   168,
     169,   170,   171,   172,   173,   174,    -1,   176,    -1,    -1,
      -1,    -1,    -1,   182,    -1,    -1,    -1,    -1,    -1,   188,
     189,   190,   191,   192,    -1,   194,   195,   196,   197,   198,
     199,   200,   201,   202,   203,   204,   205,    -1,   207,   208,
     209,   210,   211,   212,   213,   214,   215,   216,   217,   218,
     219,   220,   221,   222,   223,    -1,   225,   226,   227,   228,
     229,   230,   231,   232,   233,   234,   235,   236,   237,    -1,
     239,   240,   241,   242,   243,   244,   245,    -1,   247,   248,
     249,   250,   251,   252,   253,   254,   255,   256,   257,   258,
     259,   260,   261,   262,   263,   264,   265,   266,   267,   268,
     269,   270,   271,   272,   273,   274,    -1,   276,   277,   278,
     279,   280,   281,   282,   283,   284,   285,   286,   287,   288,
     289,   290,   291,   292,   293,   294,    -1,   296,   297,   298,
     299,   300,   301,   302,   303,   304,   305,   306,   307,   308,
     309,   310,   311,    -1,   313,   314,   315,   316,   317,   318,
     319,   320,   321,   322,   323,   324,   325,   326,     4,     5,
      -1,     7,     8,     9,    10,    11,    12,    -1,    -1,    -1,
      -1,    17,    -1,    -1,    -1,    -1,    -1,    23,    -1,    -1,
      -1,    27,    28,    29,    30,    31,    32,    -1,    34,    -1,
      -1,    37,    38,    39,    40,    -1,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      -1,    -1,    -1,    59,    -1,    61,    62,    -1,    64,    -1,
      -1,    67,    -1,    69,    70,    71,    72,    73,    74,    75,
      76,    -1,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    -1,    -1,
      96,    97,    98,    -1,    -1,   101,   102,   103,   104,   105,
      -1,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,    -1,    -1,    -1,    -1,    -1,   125,
      -1,   127,   128,   129,   130,   131,   132,   133,   134,   135,
     136,    -1,   138,   139,   140,    -1,    -1,   143,   144,   145,
     146,   147,   148,   149,   150,   151,    -1,   153,   154,    -1,
     156,   157,   158,    -1,    -1,   161,   162,    -1,   164,   165,
     166,   167,   168,   169,   170,   171,   172,   173,   174,    -1,
     176,    -1,    -1,    -1,    -1,    -1,   182,    -1,    -1,    -1,
      -1,    -1,   188,   189,   190,   191,   192,    -1,   194,   195,
     196,   197,   198,   199,   200,   201,   202,   203,   204,   205,
      -1,   207,   208,   209,   210,   211,   212,   213,   214,   215,
     216,   217,   218,   219,   220,   221,   222,   223,    -1,   225,
     226,   227,   228,   229,   230,   231,   232,   233,   234,   235,
     236,   237,    -1,   239,   240,   241,   242,   243,   244,   245,
      -1,   247,   248,   249,   250,   251,   252,   253,   254,   255,
     256,   257,   258,   259,   260,   261,   262,   263,   264,   265,
     266,   267,   268,   269,   270,   271,   272,   273,   274,    -1,
     276,   277,   278,   279,   280,   281,   282,   283,   284,   285,
     286,   287,   288,   289,   290,   291,   292,   293,   294,    -1,
     296,   297,   298,   299,   300,   301,   302,   303,   304,   305,
     306,   307,   308,   309,   310,   311,    -1,   313,   314,   315,
     316,   317,   318,   319,   320,   321,   322,   323,   324,   325,
     326,     4,     5,    -1,     7,     8,     9,    10,    11,    12,
      -1,    -1,    -1,    -1,    17,    -1,    -1,    -1,    -1,    -1,
      23,    -1,    -1,    -1,    27,    28,    29,    30,    31,    32,
      -1,    34,    -1,    -1,    37,    38,    39,    40,    -1,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    -1,    -1,    -1,    59,    -1,    61,    62,
      -1,    64,    -1,    -1,    67,    -1,    69,    70,    71,    72,
      73,    74,    75,    76,    -1,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    -1,    -1,    96,    97,    98,    -1,    -1,   101,   102,
     103,   104,   105,    -1,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,    -1,    -1,    -1,
      -1,    -1,   125,    -1,   127,   128,   129,   130,   131,   132,
     133,   134,   135,   136,    -1,   138,   139,   140,    -1,    -1,
     143,   144,   145,   146,   147,   148,   149,   150,   151,    -1,
     153,   154,    -1,   156,   157,   158,    -1,    -1,   161,   162,
      -1,   164,   165,   166,   167,   168,   169,   170,   171,   172,
     173,   174,    -1,   176,    -1,    -1,    -1,    -1,    -1,   182,
      -1,    -1,    -1,    -1,    -1,   188,   189,   190,   191,   192,
      -1,   194,   195,   196,   197,   198,   199,   200,   201,   202,
     203,   204,   205,    -1,   207,   208,   209,   210,   211,   212,
     213,   214,   215,   216,   217,   218,   219,   220,   221,   222,
     223,    -1,   225,   226,   227,   228,   229,   230,   231,   232,
     233,   234,   235,   236,   237,    -1,   239,   240,   241,   242,
     243,   244,   245,    -1,   247,   248,   249,   250,   251,   252,
     253,   254,   255,   256,   257,   258,   259,   260,   261,   262,
     263,   264,   265,   266,   267,   268,   269,   270,   271,   272,
     273,   274,    -1,   276,   277,   278,   279,   280,   281,   282,
     283,   284,   285,   286,   287,   288,   289,   290,   291,   292,
     293,   294,    -1,   296,   297,   298,   299,   300,   301,   302,
     303,   304,   305,   306,   307,   308,   309,   310,   311,    -1,
     313,   314,   315,   316,   317,   318,   319,   320,   321,   322,
     323,   324,   325,   326,     4,     5,    -1,     7,     8,     9,
      10,    11,    12,    -1,    -1,    -1,    -1,    17,    -1,    -1,
      -1,    -1,    -1,    23,    -1,    -1,    -1,    27,    28,    29,
      30,    31,    32,    -1,    34,    -1,    -1,    37,    38,    39,
      40,    -1,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    -1,    -1,    -1,    59,
      -1,    61,    62,    -1,    64,    -1,    -1,    67,    -1,    69,
      70,    71,    72,    73,    74,    75,    76,    -1,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    -1,    -1,    96,    97,    98,    -1,
      -1,   101,   102,   103,   104,   105,    -1,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
      -1,    -1,    -1,    -1,    -1,   125,    -1,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,    -1,   138,   139,
     140,    -1,    -1,   143,   144,   145,   146,   147,   148,   149,
     150,   151,    -1,   153,   154,    -1,   156,   157,   158,    -1,
      -1,   161,   162,    -1,   164,   165,   166,   167,   168,   169,
     170,   171,   172,   173,   174,    -1,   176,    -1,    -1,    -1,
      -1,    -1,   182,    -1,    -1,    -1,    -1,    -1,   188,   189,
     190,   191,   192,    -1,   194,   195,   196,   197,   198,   199,
     200,   201,   202,   203,   204,   205,    -1,   207,   208,   209,
     210,   211,   212,   213,   214,   215,   216,   217,   218,   219,
     220,   221,   222,   223,    -1,   225,   226,   227,   228,   229,
     230,   231,   232,   233,   234,   235,   236,   237,    -1,   239,
     240,   241,   242,   243,   244,   245,    -1,   247,   248,   249,
     250,   251,   252,   253,   254,   255,   256,   257,   258,   259,
     260,   261,   262,   263,   264,   265,   266,   267,   268,   269,
     270,   271,   272,   273,   274,    -1,   276,   277,   278,   279,
     280,   281,   282,   283,   284,   285,   286,   287,   288,   289,
     290,   291,   292,   293,   294,    -1,   296,   297,   298,   299,
     300,   301,   302,   303,   304,   305,   306,   307,   308,   309,
     310,   311,    -1,   313,   314,   315,   316,   317,   318,   319,
     320,   321,   322,   323,   324,   325,   326,     4,     5,    -1,
       7,     8,     9,    10,    11,    12,    -1,    -1,    -1,    -1,
      17,    -1,    -1,    -1,    -1,    -1,    23,    -1,    -1,    -1,
      27,    28,    29,    30,    31,    32,    -1,    34,    -1,    -1,
      37,    38,    39,    40,    -1,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    -1,
      -1,    -1,    59,    -1,    61,    62,    -1,    64,    -1,    -1,
      67,    -1,    69,    70,    71,    72,    73,    74,    75,    76,
      -1,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    -1,    -1,    96,
      97,    98,    -1,    -1,   101,   102,   103,   104,   105,    -1,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,    -1,    -1,    -1,    -1,    -1,   125,    -1,
     127,   128,   129,   130,   131,   132,   133,   134,   135,   136,
      -1,   138,   139,   140,    -1,    -1,   143,   144,   145,   146,
     147,   148,   149,   150,   151,    -1,   153,   154,    -1,   156,
     157,   158,    -1,    -1,   161,   162,    -1,   164,   165,   166,
     167,   168,   169,   170,   171,   172,   173,   174,    -1,   176,
      -1,    -1,    -1,    -1,    -1,   182,    -1,    -1,    -1,    -1,
      -1,   188,   189,   190,   191,   192,    -1,   194,   195,   196,
     197,   198,   199,   200,   201,   202,   203,   204,   205,    -1,
     207,   208,   209,   210,   211,   212,   213,   214,   215,   216,
     217,   218,   219,   220,   221,   222,   223,    -1,   225,   226,
     227,   228,   229,   230,   231,   232,   233,   234,   235,   236,
     237,    -1,   239,   240,   241,   242,   243,   244,   245,    -1,
     247,   248,   249,   250,   251,   252,   253,   254,   255,   256,
     257,   258,   259,   260,   261,   262,   263,   264,   265,   266,
     267,   268,   269,   270,   271,   272,   273,   274,    -1,   276,
     277,   278,   279,   280,   281,   282,   283,   284,   285,   286,
     287,   288,   289,   290,   291,   292,   293,   294,    -1,   296,
     297,   298,   299,   300,   301,   302,   303,   304,   305,   306,
     307,   308,   309,   310,   311,    -1,   313,   314,   315,   316,
     317,   318,   319,   320,   321,   322,   323,   324,   325,   326
  };

  const unsigned short int
  Parser::yystos_[] =
  {
       0,     4,     5,     7,     8,     9,    10,    11,    12,    13,
      14,    17,    19,    23,    27,    28,    29,    30,    31,    32,
      34,    35,    36,    37,    38,    39,    40,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    59,    60,    61,    62,    63,    64,    65,    67,
      69,    70,    71,    72,    73,    74,    75,    76,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    96,    97,    98,    99,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   125,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   138,   139,
     140,   143,   144,   145,   146,   147,   148,   149,   150,   151,
     153,   154,   156,   157,   158,   161,   162,   163,   164,   165,
     166,   167,   168,   169,   170,   171,   172,   173,   174,   176,
     182,   188,   189,   190,   191,   192,   193,   194,   195,   196,
     197,   198,   199,   200,   201,   202,   203,   204,   205,   206,
     207,   208,   209,   210,   211,   212,   213,   214,   215,   216,
     217,   218,   219,   220,   221,   222,   223,   225,   226,   227,
     228,   229,   230,   231,   232,   233,   234,   235,   236,   237,
     239,   240,   241,   242,   243,   244,   245,   247,   248,   249,
     250,   251,   252,   253,   254,   255,   256,   257,   258,   259,
     260,   261,   262,   263,   264,   265,   266,   267,   268,   269,
     270,   271,   272,   273,   274,   276,   277,   278,   279,   280,
     281,   282,   283,   284,   285,   286,   287,   288,   289,   290,
     291,   292,   293,   294,   295,   296,   297,   298,   299,   300,
     301,   302,   303,   304,   305,   306,   307,   308,   309,   310,
     311,   312,   313,   314,   315,   316,   317,   318,   319,   320,
     321,   322,   323,   324,   325,   326,   329,   332,   343,   344,
     345,   346,   349,   350,   353,   354,   355,   356,   361,   362,
     363,   365,   366,   367,   368,   369,   382,   383,   384,   385,
     386,   387,   388,   389,   391,   392,   393,   394,   395,   396,
     397,   398,   399,   401,   402,   403,   404,   405,   406,   407,
     408,   409,   410,   411,   412,   421,   424,   428,   429,   430,
     432,   433,   435,   442,   443,   444,   445,   446,   447,   448,
     449,   450,   451,   452,   453,   454,   455,   456,   457,   458,
     462,   463,   464,   465,   466,   467,   468,   471,   473,   474,
     475,   476,   478,   479,   480,   481,   482,   504,   507,   516,
     517,   518,   519,   520,   521,   522,   523,   529,   530,   531,
     532,   533,   534,   542,   543,   549,   550,   551,   552,   553,
     554,   555,   557,   559,   560,   561,   562,   563,   564,   565,
     566,   567,   568,   571,    45,    46,    48,    60,    61,    69,
      83,   263,   268,   302,   412,   412,   412,    26,   335,    40,
      84,    87,    89,    92,   108,   120,   161,   164,   200,   208,
     218,   219,   236,   238,   256,   266,   271,   272,   277,   278,
     287,   288,   289,   290,   293,   294,   297,   326,   370,   412,
     571,   400,   236,   200,   219,   237,   200,   272,   412,     3,
       4,    27,    29,    30,    34,   116,   117,   129,   149,   150,
     175,   180,   181,   183,   184,   185,   186,   188,   226,   314,
     375,   571,   200,   483,   436,   242,   189,   190,   571,    84,
      87,    89,    92,   161,   164,   200,   218,   219,   236,   238,
     266,   272,   287,   288,   289,   290,   293,   297,   332,   332,
     412,   413,   414,   415,    40,    84,    88,    90,    93,   114,
     165,   199,   200,   201,   237,   267,   272,   287,   289,   291,
     292,   293,   297,   544,    84,    87,   161,   164,   200,   218,
     236,   266,   272,   287,   288,   289,   290,   293,   297,   200,
     208,   219,   273,   332,    84,    30,   164,   288,    84,    13,
     425,   412,   417,   266,   412,   417,     0,   344,     5,     6,
       7,     8,     9,    11,    12,    13,    15,    16,    17,    18,
      19,    20,    21,    22,    24,   296,   331,    68,   275,   331,
     556,   331,   556,    26,   303,   332,    30,   412,    30,   244,
      73,    81,    85,    96,   195,   571,   571,   571,    73,   332,
     312,   548,   244,   571,   571,   209,   571,   571,   571,   412,
      92,    30,    84,   286,   571,   225,   571,   571,   571,   571,
     161,   195,   195,   571,   571,    84,     5,     7,     8,     9,
      11,    12,    13,    17,    19,   296,   412,    84,    52,    53,
      54,   142,   571,   571,   571,   571,   571,   286,    55,   412,
     313,   412,   332,   332,   332,   332,   332,   332,     4,   331,
     571,    20,   123,   246,   247,   248,   249,   250,   251,   252,
     412,   484,   485,    52,    53,    64,    66,   248,   437,   571,
     331,   331,   331,    73,    81,    85,    96,   195,   571,   571,
     571,   571,   312,   547,   571,   571,   571,   571,   571,   412,
     571,   571,   571,   571,   571,   571,   195,   571,   417,   417,
     155,   137,   416,   155,   416,    27,    30,    74,    82,    86,
      97,   196,   331,   331,   331,   331,   331,   202,   264,   331,
     331,   331,   331,   331,   331,   331,   331,   196,   331,   162,
      73,    81,    85,    96,   195,   571,    30,   537,   571,   571,
     571,   571,   571,   571,   571,   571,   571,   571,   571,   195,
     571,   571,    30,   571,   571,   417,   571,   417,   469,   412,
     571,   142,   412,   315,   334,    30,   333,   333,   412,   412,
     332,    13,   149,   412,   412,   412,     7,     9,   412,   412,
     412,   412,   412,   412,   412,   412,   412,    30,    30,   123,
      63,   482,   482,   571,    30,    20,   126,   246,   333,   417,
      26,   331,   218,   571,    30,   571,   571,   571,   571,    84,
     331,   332,   331,   268,   417,    13,   537,   218,   189,   100,
     332,    30,   212,   332,   189,   100,   236,   571,   265,    96,
     571,   189,    77,   189,   189,   189,   548,   571,    30,   571,
     189,   318,   508,   509,    73,     7,     9,   412,   332,    96,
     412,   331,   338,   331,   331,   331,   331,   571,   558,   571,
     332,   331,    27,    27,    27,    27,    27,    27,   313,   412,
     331,    30,    79,   419,   571,   142,   331,   334,   537,   331,
     571,   571,   571,   571,   571,   212,   332,   331,   331,   331,
      41,   537,   331,   331,   331,   331,   331,   331,   331,   331,
     331,   331,   331,   331,   571,   331,   333,   333,   412,   412,
      80,   412,    80,   331,   331,   331,   331,   331,   331,   331,
     159,   492,   571,   331,   142,   545,   571,   571,   571,   571,
     571,   331,    26,   331,    26,   331,   331,   331,   331,   331,
     331,   331,   331,   331,   331,   571,   331,   233,   332,   347,
     438,   331,   571,   333,   212,   332,   191,   470,   332,   431,
     331,   280,   152,   426,   412,   265,   417,   149,   122,   332,
     412,   200,   506,   483,    26,   333,    20,   417,   417,   333,
      30,   571,   332,   212,   332,    84,   332,   332,    77,   332,
      85,    30,   418,    77,   333,    41,     9,   332,   541,    84,
     161,   571,   199,   418,    30,    87,   418,   200,   200,   332,
      77,    50,   571,   189,   161,   571,   161,   161,   571,   537,
      77,   328,   570,   189,   332,   571,    50,   331,    45,   510,
     571,   412,   422,   423,   571,    57,   420,   418,   189,   334,
     420,    12,    27,   333,   334,   333,   333,   333,   333,   334,
     332,   331,    30,   571,    30,   332,   412,   486,   487,   488,
     489,   490,   485,   438,   331,   331,   331,   331,   331,    87,
     418,   331,   331,   152,   152,    27,   331,   492,   571,     9,
     546,   331,   331,   331,   331,   331,    30,   571,   331,    30,
     412,   439,    67,    59,   226,   459,    87,   418,   189,   190,
     331,   417,   420,   171,   344,   137,   344,   427,    50,   333,
     412,   417,   571,   196,   505,   484,   571,   333,   333,   333,
      87,   418,   332,   418,    96,   418,   418,    30,   418,   571,
     335,   333,   334,   108,   277,   278,   370,   189,   537,   538,
     539,   571,    84,   100,   166,   167,   170,   268,   536,    81,
     571,   420,   332,   333,    30,   332,   333,   571,   571,   417,
     571,    30,   332,   161,   571,    73,    92,   378,   571,   571,
      59,   536,    30,    77,   571,   418,   299,    27,    94,   319,
     323,   511,    73,    92,   376,   335,   333,   334,   235,   412,
     331,   339,   161,   571,   331,   412,   333,    27,    27,    27,
      26,   482,   419,   334,   420,    79,    67,   571,   333,   412,
     412,   334,   331,   571,   331,   331,   333,   334,   332,   348,
     440,   196,   284,   331,   571,   333,   333,   492,    30,   344,
      80,   344,    30,   333,   304,   459,   571,   333,   439,   333,
      77,   571,   333,   333,   110,   223,   262,   359,   333,    84,
     287,   412,   331,    30,   376,   332,   331,   333,   334,    13,
     109,   149,   170,    91,   127,   150,   174,   175,   176,   177,
     178,   179,   180,   181,   183,   184,   187,   188,   540,   125,
     571,    32,   340,   341,    30,    77,   331,   571,    94,   332,
     331,   439,   331,    30,   439,   331,   219,   331,   331,   333,
      73,   331,   271,   352,   417,   331,   197,   197,   571,    73,
      59,    59,   332,   196,    84,   569,    30,   332,   333,   571,
      27,    27,   320,   324,   512,   225,   225,   196,   336,   331,
     423,   234,   331,   571,    12,   331,   333,   333,   333,    30,
     333,    99,   491,   487,    58,   493,   571,   440,    66,   331,
     414,    27,   412,   109,   412,   441,   331,   332,    27,    66,
     331,   331,   331,   312,   331,    50,   331,    84,   331,   333,
     331,   271,   571,    94,   390,   331,   331,   142,   110,   571,
     226,   357,   331,    92,   571,   335,   153,   371,   412,   541,
     539,   149,   412,    30,   332,   332,   332,   332,   173,   173,
     173,   332,   332,   173,   332,   277,   278,   280,    87,   108,
     417,   333,    30,   333,    30,   331,   571,    30,   333,   334,
     420,    30,    30,   331,    75,   332,   332,   417,   332,    77,
     357,   569,    30,   331,   204,    27,   317,   322,   513,    27,
      27,   332,   417,    27,    58,   412,   331,   419,   164,    50,
     160,   500,   331,   571,   333,   334,   439,   571,   331,    30,
      73,   215,   332,   235,    95,   331,    30,   142,   225,   220,
     360,   571,   331,   412,    30,    75,   377,   333,   536,    30,
      27,    27,    27,    27,    27,    27,    89,   571,    74,   333,
     331,   331,   215,   206,   331,   235,   175,   271,    58,   285,
      50,   417,   417,   333,   535,   571,   375,   360,   357,   333,
     571,   316,   321,   514,   372,   373,   412,   337,   233,    50,
     491,   332,    71,    98,   484,   412,    95,   496,   268,   459,
     109,   412,   333,   268,   459,   305,   571,    30,   352,   234,
     164,    30,    27,    27,   109,   358,   331,   412,   196,   164,
     331,   333,   333,   333,   334,   260,   333,   260,   333,   260,
     333,   333,   571,    84,   338,   420,    87,   207,   234,   230,
      30,    50,   332,   261,   333,   333,   150,   333,   334,   332,
     221,   360,   357,   298,    27,    95,   325,   515,   333,   334,
     570,    30,   332,   417,   332,   332,   334,    50,   492,    77,
      99,   460,   459,    77,   460,    50,    40,   331,   333,    27,
     571,   142,   110,   331,   332,   332,    27,   333,   333,   333,
     282,    73,   332,   434,   331,   332,    30,    27,   571,   235,
     332,   412,   527,   528,    27,   110,   420,    27,   331,   571,
     417,   412,   358,   331,   571,    92,   378,   373,    77,   175,
     246,   439,   525,   526,   333,   332,   494,   484,    98,   412,
     497,   498,   327,   503,   280,   199,   196,   461,   331,   280,
     461,    30,    77,   175,   233,    30,    30,    30,   372,   417,
     333,    30,   571,   417,   334,   339,   439,    30,    46,   189,
     472,   240,   351,   234,   525,   189,   333,   334,   334,   332,
     331,   240,   332,   333,   189,   189,   100,   197,   195,   380,
     375,   230,   571,   333,   334,   484,   495,   333,   334,   333,
     332,   168,   169,   499,   334,   228,   224,   501,   281,    27,
     332,   331,   281,   331,   306,   571,   230,    30,   125,   364,
     228,   333,   333,   283,   243,   333,   332,   331,   333,    46,
     207,   205,   331,    50,   331,    27,   333,   439,   204,   330,
     524,   528,    27,   412,    27,   417,    77,   571,   571,   300,
     197,   188,   381,   224,   374,   569,   571,   189,   331,   526,
     333,   332,   484,   497,   412,   412,    66,   502,   332,   417,
     332,    50,   331,   571,   175,   217,   231,   168,    30,   378,
     331,    27,   571,   417,   214,   207,   204,    27,   239,   285,
     195,   195,   331,   332,   333,    77,   333,    30,   331,   100,
     571,    30,   198,   331,    30,   359,   374,   351,   439,   495,
     333,   412,   417,   333,   417,    30,   351,   230,   357,   331,
     380,   161,   241,   333,   100,   206,    30,    30,   332,   412,
     332,    92,   477,    77,    30,    77,   288,   100,   301,    30,
     357,   110,   331,   333,   333,   333,   307,   331,   571,   331,
     381,   571,   571,   217,    30,   472,   527,   224,   274,   417,
     571,   333,   334,   375,   357,    30,   571,   241,   571,   360,
     412,   459,   459,   308,   351,   331,   281,   331,    73,   203,
     331,   333,    30,    30,   333,    59,   331,    92,   357,   420,
     332,   571,   331,   358,   359,   460,   460,    50,   331,   332,
      84,   472,   204,   524,   274,   524,   571,   571,   420,   331,
     417,   301,    13,   213,   379,   357,   461,   461,    30,   417,
     211,   331,   195,   331,    30,   331,   225,    59,   331,   333,
     571,   149,   332,   360,   331,   331,   304,   333,    82,   412,
     524,    27,   571,   331,   331,   417,   358,    50,   332,   224,
     274,   331,   225,   333,    30,   439,    30,    30,    27,   333,
     274,   524,   216,    30,   331,   100,   524,   210,   331,    84,
     211,    82,   332,   439,   333,   331
  };

  const unsigned short int
  Parser::yyr1_[] =
  {
       0,   342,   343,   343,   344,   344,   344,   344,   344,   344,
     344,   344,   344,   344,   344,   344,   344,   344,   344,   344,
     344,   344,   344,   344,   344,   344,   344,   344,   344,   344,
     344,   344,   344,   344,   344,   344,   344,   344,   344,   344,
     344,   344,   344,   344,   344,   344,   344,   344,   344,   344,
     344,   344,   344,   344,   344,   344,   344,   344,   344,   344,
     344,   344,   344,   344,   344,   344,   344,   344,   344,   344,
     344,   344,   344,   344,   344,   344,   344,   344,   344,   344,
     344,   344,   344,   344,   344,   344,   344,   344,   344,   344,
     344,   344,   344,   344,   344,   344,   344,   344,   344,   344,
     344,   344,   344,   344,   344,   344,   344,   344,   344,   344,
     344,   344,   344,   344,   344,   344,   344,   344,   344,   344,
     344,   344,   344,   344,   345,   346,   347,   348,   349,   349,
     349,   349,   349,   350,   350,   350,   351,   351,   352,   352,
     353,   354,   355,   356,   356,   356,   356,   357,   357,   358,
     358,   359,   359,   360,   360,   361,   362,   363,   364,   364,
     365,   365,   365,   365,   366,   367,   368,   369,   369,   369,
     370,   370,   370,   371,   371,   372,   372,   373,   373,   373,
     373,   374,   374,   375,   375,   375,   375,   375,   375,   375,
     375,   375,   375,   375,   376,   376,   376,   377,   377,   378,
     378,   378,   379,   379,   380,   380,   381,   381,   382,   382,
     383,   383,   384,   385,   386,   387,   388,   389,   389,   389,
     389,   390,   390,   391,   392,   393,   394,   394,   394,   394,
     395,   396,   397,   398,   398,   399,   400,   400,   400,   400,
     401,   402,   403,   404,   405,   406,   407,   408,   409,   410,
     410,   411,   412,   412,   412,   412,   412,   412,   412,   412,
     412,   412,   412,   412,   412,   412,   412,   412,   412,   412,
     412,   412,   412,   412,   412,   412,   412,   412,   412,   412,
     412,   412,   412,   412,   412,   412,   412,   412,   412,   412,
     412,   412,   412,   412,   412,   412,   412,   412,   412,   412,
     412,   412,   412,   413,   414,   414,   415,   416,   416,   417,
     417,   418,   418,   418,   419,   419,   419,   419,   419,   420,
     420,   421,   422,   422,   422,   423,   423,   423,   424,   425,
     425,   426,   426,   427,   427,   427,   428,   428,   428,   429,
     430,   431,   431,   432,   433,   434,   434,   435,   436,   436,
     436,   436,   436,   437,   437,   438,   438,   439,   439,   440,
     441,   441,   441,   441,   442,   442,   443,   444,   445,   446,
     446,   447,   448,   449,   450,   451,   452,   453,   454,   455,
     456,   457,   457,   457,   457,   457,   457,   458,   459,   459,
     460,   460,   461,   461,   462,   463,   464,   465,   466,   467,
     467,   468,   469,   469,   470,   470,   470,   471,   471,   471,
     471,   472,   472,   473,   474,   475,   476,   476,   476,   477,
     477,   478,   479,   480,   481,   481,   482,   483,   483,   483,
     483,   483,   483,   483,   483,   483,   484,   484,   485,   485,
     486,   486,   487,   487,   488,   489,   489,   489,   490,   491,
     491,   492,   492,   492,   493,   493,   493,   493,   493,   494,
     494,   495,   495,   496,   496,   497,   498,   498,   499,   499,
     499,   500,   500,   501,   501,   502,   502,   503,   503,   504,
     505,   505,   506,   506,   507,   508,   509,   509,   510,   510,
     511,   511,   511,   512,   512,   512,   513,   513,   513,   514,
     514,   514,   515,   515,   515,   516,   517,   517,   518,   519,
     520,   521,   522,   522,   523,   523,   523,   523,   523,   523,
     523,   523,   523,   524,   524,   525,   525,   526,   526,   527,
     527,   528,   528,   529,   530,   531,   532,   533,   534,   534,
     534,   534,   535,   535,   536,   536,   537,   537,   537,   537,
     538,   538,   539,   539,   539,   539,   539,   540,   540,   540,
     540,   540,   540,   540,   540,   540,   540,   540,   540,   540,
     540,   540,   540,   540,   540,   540,   540,   540,   540,   540,
     540,   540,   540,   540,   541,   541,   541,   541,   541,   541,
     541,   541,   542,   543,   544,   544,   545,   545,   546,   546,
     547,   547,   548,   548,   549,   550,   550,   551,   552,   553,
     554,   555,   555,   556,   556,   557,   558,   558,   559,   560,
     561,   562,   563,   564,   565,   566,   567,   568,   569,   569,
     570,   570,   571,   571,   571,   571,   571,   571,   571,   571,
     571,   571,   571,   571,   571,   571,   571,   571,   571,   571,
     571,   571,   571,   571,   571,   571,   571,   571,   571,   571,
     571,   571,   571,   571,   571,   571,   571,   571,   571,   571,
     571,   571,   571,   571,   571,   571,   571,   571,   571,   571,
     571,   571,   571,   571,   571,   571,   571,   571,   571,   571,
     571,   571,   571,   571,   571,   571,   571,   571,   571,   571,
     571,   571,   571,   571,   571,   571,   571,   571,   571,   571,
     571,   571,   571,   571,   571,   571,   571,   571,   571,   571,
     571,   571,   571,   571,   571,   571,   571,   571,   571,   571,
     571,   571,   571,   571,   571,   571,   571,   571,   571,   571,
     571,   571,   571,   571,   571,   571,   571,   571,   571,   571,
     571,   571,   571,   571,   571,   571,   571,   571,   571,   571,
     571,   571,   571,   571,   571,   571,   571,   571,   571,   571,
     571,   571,   571,   571,   571,   571,   571,   571,   571,   571,
     571,   571,   571,   571,   571,   571,   571,   571,   571,   571,
     571,   571,   571,   571,   571,   571,   571,   571,   571,   571,
     571,   571,   571,   571,   571,   571,   571,   571,   571,   571,
     571,   571,   571,   571,   571,   571,   571,   571,   571,   571,
     571,   571,   571,   571,   571,   571,   571,   571,   571,   571,
     571,   571,   571,   571,   571,   571,   571,   571,   571,   571,
     571,   571,   571,   571,   571,   571,   571,   571,   571,   571,
     571,   571,   571,   571,   571,   571,   571,   571,   571,   571,
     571,   571,   571,   571,   571,   571,   571,   571,   571,   571,
     571,   571,   571,   571,   571,   571,   571,   571,   571,   571,
     571,   571,   571,   571,   571,   571,   571,   571,   571,   571,
     571
  };

  const unsigned char
  Parser::yyr2_[] =
  {
       0,     2,     1,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     9,     7,     1,     1,     3,     5,
       4,     7,     8,    15,    13,     8,     0,     4,     5,     7,
       5,     4,     5,    11,    16,    15,     8,     0,     3,     0,
       3,     0,     2,     0,     2,     5,     4,     5,     2,     2,
      11,    17,    14,     8,     5,     4,     5,     8,    14,    17,
       1,     2,     2,     0,     2,     1,     3,     1,    10,    12,
      11,     0,     2,     1,     1,     1,     4,     4,     4,     1,
       6,     4,     6,     4,     0,     3,     3,     0,     2,     0,
       3,     3,     0,     4,     0,     3,     0,     3,     5,     4,
       4,     3,     5,     4,     4,     3,     3,     8,     9,    18,
      16,     0,     2,     5,     4,     5,     7,     9,    10,     4,
       3,     4,     4,     3,     3,     6,     0,     2,     2,     2,
      12,     4,     3,     4,    18,     4,     3,     4,     4,     4,
       4,     2,     1,     3,     1,     1,     3,     5,     3,     5,
       1,     1,     1,     1,     3,     3,     3,     3,     3,     2,
       3,     3,     4,     3,     5,     6,     3,     5,     3,     3,
       3,     3,     3,     2,     2,     3,     3,     4,     4,     3,
       4,     4,     5,     5,     5,     4,     3,     3,     3,     4,
       4,     4,     3,     6,     0,     5,     1,     0,     2,     1,
       3,     0,     3,     5,     2,     2,     1,     1,     0,     0,
       2,     7,     0,     1,     3,     0,     1,     5,     8,     0,
       1,     2,     2,     0,     2,     2,    11,    13,     7,     4,
       7,     0,     3,     3,     4,     3,     5,     8,     0,     2,
       2,     2,     2,     1,     0,     0,     3,     1,     3,     3,
       1,     1,     3,     3,    17,    20,     4,     3,     4,     7,
       9,     6,     4,     4,     3,     4,     5,     6,     4,     4,
      14,     7,    12,    19,     7,    12,    19,     7,     0,     3,
       0,     3,     0,     4,    19,     4,     3,     4,     4,     8,
      11,     5,     0,     1,     0,     2,     2,     7,    19,    17,
      13,     0,     3,     4,     4,     4,     9,     8,    17,     6,
       8,     3,     4,     4,     4,     2,    13,     0,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     3,     2,     1,
       1,     3,     1,     1,     0,     3,     3,     5,     3,     0,
       5,     0,     2,     4,     0,     3,     6,     6,     8,     3,
       5,     0,     1,     0,     3,     2,     1,     3,     0,     1,
       1,     0,     2,     0,     2,     0,     2,     0,     3,     7,
       0,    17,     0,     2,     5,     7,     0,     3,     0,     3,
       0,     1,     2,     0,     1,     2,     0,     1,     1,     0,
       2,     1,     0,     1,     1,     7,     4,     7,     4,     3,
       4,     4,     6,    11,     7,    19,    14,    19,    21,    24,
      26,    13,     4,     0,     5,     1,     3,     1,     4,     1,
       3,     1,     3,     3,     4,     4,     8,     4,    10,     7,
       7,    11,     6,     8,     0,     2,     1,     3,     1,     3,
       1,     3,     2,     3,     2,     3,     3,     1,     1,     1,
       1,     1,     6,     4,     1,     4,     2,     2,     2,     2,
       1,     1,     1,     1,     4,     5,     4,     5,     4,     5,
       1,     1,     4,     4,     0,     3,     3,     3,     3,     4,
       4,    16,     5,     6,     0,     1,     0,     2,     0,     2,
       0,     2,     0,     3,     4,     7,    33,     4,     3,     4,
       2,     3,     3,     1,     2,     6,     3,     5,     7,     4,
       3,     4,     7,    20,    14,     5,     4,     5,     0,     3,
       0,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1
  };


#if YYDEBUG
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a yyntokens_, nonterminals.
  const char*
  const Parser::yytname_[] =
  {
  "$end", "error", "$undefined", "ASSIGN", "ASSIGNMENT", "LOGIC", "XOR",
  "IN", "IS", "LIKE", "REGEXP", "BETWEEN", "COMPARISON", "NOT", "'!'",
  "'|'", "'&'", "SHIFT", "'+'", "'-'", "'*'", "'/'", "'%'", "MOD", "'^'",
  "UMINUS", "'.'", "INTNUM", "NAME", "U8VAR", "STRING", "UNSIGNED_LONG",
  "UNSIGNED_INT", "BOOL", "DOUBLE_NUM", "CREATE", "DELETE", "DOCBEGIN",
  "DOCEND", "XMLDOC", "DOC", "EXISTS", "LOGIN", "LOGOUT", "RETRIEVE",
  "START", "STOP", "CHECK", "RESTART", "OVERRIDE", "BY", "ANDOP", "IGNORE",
  "LOW_PRIORITY", "QUICK", "SET", "UPDATE", "WHERE", "GROUP", "KEY",
  "USERVAR", "WAIT", "DOES", "SELECT", "DELAYED", "INSERT", "INTO",
  "VALUES", "INTO_OUTFILE", "COMPLETE", "LEFTSHIFT", "ROLLUP",
  "RIGHTSHIFT", "RECORD", "RECORDS", "PICKER", "PICKERS", "TYPE", "JIMOID",
  "AS", "END", "PROC", "PROCS", "DEBUG", "DATA", "SCANNER", "SCANNERS",
  "DATASET", "DATASETS", "DATABASE", "DATABASES", "DATE", "SCHEMA",
  "SCHEMAS", "WITH", "ORDER", "CONNECTOR", "CONNECTORS", "CUBE", "USE",
  "USING", "FILTER", "FILTERED", "ACCEPTED", "MATCHED", "MISMATCHED",
  "DROP", "DROPPED", "MULTIPLE", "DEFAULT", "VALUE", "SELECTOR",
  "SELECTORS", "INNER", "FULL", "OUTER", "LEFT", "RIGHT", "EXCLUSIVE",
  "XO", "JOIN", "OR", "AND", "ALL", "ANY", "MODE", "ACCUMULATE", "BINARY",
  "BOTH", "CASE", "CURRENT_DATE", "CURRENT_TIME", "CURRENT_TIMESTAMP",
  "DAY_HOUR", "DAY_MICROSECOND", "DAY_MINUTE", "DAY_SECOND", "ELSE",
  "FCOUNT", "FDATE_ADD", "FDATE_SUB", "FOR", "FROM", "FSUBSTRING", "FTRIM",
  "HOUR_MICROSECOND", "HOUR_MINUTE", "HOUR_SECOND", "INTERVAL", "NULLX",
  "NUMBER", "SOME", "THEN", "TRIM", "TRAILING", "WHEN", "YEAR",
  "YEAR_MONTH", "LEADING", "LIMIT", "HAVING", "TABLE", "TABLES", "SHOW",
  "INDEX", "INDEXES", "AUTO_INCREMENT", "CHECKSUM", "ASC", "DESC",
  "COMMENTS", "CONFIG", "BLOCKSIZE", "UNSIGNED", "BIT", "CHAR", "INT",
  "TINYINT", "SMALLINT", "MEDIUMINT", "VARCHAR", "VARCHAR2", "STR",
  "BIGINT", "DOUBLE", "DATETIME", "DECIMAL", "FLOAT", "TEXT", "ON", "OFF",
  "SUPPRESS", "DIR", "DESCRIBE", "VARIABLE", "FIELD", "FIELDS",
  "DELIMITER", "QUALIFIER", "TASKS", "JOB", "JOBS", "STATUS", "TIMES",
  "TIME", "PRIORITY", "RUN", "AT", "ACTOR", "JAVA", "ACTION", "ENGING",
  "INPUT", "MAPPING", "MAPTASK", "OUTPUT", "REDUCETASK", "SINGLE", "TASK",
  "SCHEDULE", "OFFSET", "EXPRESSION", "FORM", "FORMATTER", "FORMAT",
  "LENGTH", "MAX", "POS", "TO", "PRINT", "CODING", "COMBINE",
  "COMBINATIONS", "FILENAME", "ID", "SERVER", "SERVICE", "SERVICES",
  "SYNCER", "SIZE", "SPLIT", "IILNAME", "IIL", "OPERATOR", "REDUCE",
  "STINCT", "DISTINCT", "DISTINCTROW", "HIGH_PRIORITY", "SQL_BIG_RESULT",
  "SQL_CALC_FOUND_ROWS", "SQL_SMALL_RESULT", "STRAIGHT_JOIN", "UNIFORM",
  "PATH", "ENCODING", "STATIC", "DYNAMIC", "ROW", "PICKED", "BYTE",
  "BYTES", "IDFAMILY", "LOAD", "LOG", "IDENTIFIED", "USER", "USERS",
  "SOURCE", "DUAL", "JIMODB", "DIRECTORY", "STATISTICS", "STAT", "UNIT",
  "UNION", "OUTFILE", "CSV", "FIXED", "DIMENSIONS", "HBASE", "RAWKEY",
  "ADDRESS", "PORT", "THREAD", "MEASURES", "MODEL", "PARSER", "MAP",
  "DICTIONARY", "DISTRIBUTIONMAP", "DISTRIBUTIONMAPS", "MAPS", "VIRTUAL",
  "NICK", "LIST", "CONTAIN", "JIMOLOGIC", "CHECKPOINT", "ENDPOINT",
  "MATRIX", "RESULTS", "EXPORT", "XPATH", "TERMINATED", "ENCLOSED",
  "ESCAPED", "LINES", "STARTING", "SKIP", "INVALID", "SUB", "IF",
  "SYSTEM_ARGS", "LEFT_BRACKET", "RIGHT_BRACKET", "CACHE", "CYCLE",
  "INCREMENT", "MAXVALUE", "MINVALUE", "NOCACHE", "NOCYCLE", "NOMAXVALUE",
  "NOMINVALUE", "NOORDER", "SEQUENCE", "CONVERT", "INFOFIELD", "ALTER",
  "SHUFFLE", "';'", "'('", "')'", "','", "':'", "'['", "']'", "'{'", "'}'",
  "'0'", "'1'", "$accept", "statements", "statement", "stmt_create_actor",
  "stmt_run_actor", "par_key_names", "run_vals_list", "stmt_assignment",
  "stmt_create_data_connector", "split_size_opts",
  "stmt_data_connector_file_list", "stmt_drop_data_connector",
  "stmt_show_data_connector", "stmt_describe_data_connector",
  "stmt_create_data_field", "data_field_max_len",
  "data_field_default_value", "data_idfamily", "data_field_offset",
  "stmt_drop_data_field", "stmt_show_data_field",
  "stmt_describe_data_field", "iilmap_mode", "stmt_create_data_proc",
  "stmt_drop_data_proc", "stmt_show_data_proc", "stmt_describe_data_proc",
  "stmt_create_data_record", "data_record_type_opts", "trim_condition",
  "data_field_list", "data_field", "field_formater",
  "data_field_data_type", "record_length", "schema_picker",
  "record_delimiter", "field_mapping", "field_delimiter", "text_qualidier",
  "stmt_drop_data_record", "stmt_show_data_record",
  "stmt_describe_data_record", "stmt_create_database",
  "stmt_drop_database", "stmt_show_databases", "stmt_use_database",
  "stmt_create_datascanner", "with_order_opts", "stmt_drop_datascanner",
  "stmt_show_datascanner", "stmt_describe_datascanner",
  "stmt_create_dataset", "stmt_show_dataset", "stmt_describe_dataset",
  "stmt_drop_dataset", "stmt_debug", "delete_stmt", "delete_opts",
  "stmt_create_dict", "stmt_drop_dict", "stmt_show_dict",
  "stmt_describe_dict", "stmt_create_distributionmap",
  "stmt_drop_distributionmap", "stmt_show_distributionmap",
  "stmt_describe_distributionmap", "stmt_create_doc", "stmt_show_doc",
  "stmt_expr", "expr", "simple_case_expr", "when_case_list",
  "search_case_expr", "else_expr", "expr_list", "stmt_expr_name_values",
  "opt_as_alias", "opt_where", "stmt_create_genericobj",
  "stmt_genericobj_value_list", "stmt_genericobj_value",
  "stmt_execution_if", "if_not", "if_then_statements",
  "if_else_statements", "stmt_create_index", "stmt_describe_index",
  "stmt_list_index", "stmt_list_key_list", "show_iil_stmt",
  "drop_iil_stmt", "stmt_index_records", "insert_stmt", "insert_opts",
  "opt_into", "opt_col_names", "column_list", "insert_vals_list",
  "insert_vals", "stmt_create_jimo_logic", "stmt_drop_jimo_logic",
  "stmt_show_jimo_logic", "stmt_describe_jimo_logic", "stmt_create_job",
  "stmt_run_job", "stmt_stop_job", "stmt_drop_job", "stmt_show_job",
  "stmt_describe_job", "stmt_show_job_status", "stmt_show_job_log",
  "stmt_wait_job", "stmt_restart_job", "stmt_join_syncher",
  "load_data_stmt", "stmt_hbase", "stmt_max_threads", "stmt_max_task",
  "load_data_fields_opts", "stmt_create_map", "stmt_drop_map",
  "stmt_show_map", "stmt_describe_map", "stmt_list_map",
  "stmt_create_nick_field", "stmt_run_script_file", "parameters",
  "suppress_flag", "stmt_create_schedule", "schedule_on_priority",
  "stmt_run_schedule", "stmt_stop_schedule", "stmt_drop_schedule",
  "stmt_create_schema", "stmt_record_pick_list", "stmt_show_schema",
  "stmt_describe_schema", "stmt_drop_schema", "select_stmt",
  "jql_select_stmt", "select_opts", "select_expr_list", "select_expr",
  "table_references", "table_reference", "join_table", "table_factor",
  "table_subquery", "index_hint", "opt_limit", "stmt_group_by",
  "rollup_lists", "rollup_list", "stmt_order_by", "stmt_order_field",
  "stmt_order_field_list", "stmt_order_opt", "opt_having",
  "stmt_hack_format", "stmt_hack_into_file", "stmt_hack_convert_to",
  "stmt_select_into", "record_format", "set_job_name",
  "stmt_create_sequence", "sequence_info", "create_sequence_increment_by",
  "create_sequence_start_with", "create_sequence_max_value",
  "create_sequence_min_value", "create_sequence_cycle",
  "create_sequence_cache", "create_sequence_order", "stmt_create_service",
  "stmt_start_service", "stmt_stop_service", "stmt_show_service",
  "stmt_describe_service", "stmt_drop_service", "stmt_run_stat",
  "stmt_create_statistics", "opt_shuffle_field", "stmt_dimensions_list",
  "stmt_dimension_list", "stmt_stat_measure_list", "stmt_stat_measure",
  "stmt_show_statistics", "stmt_describe_statistics", "stmt_complete_iil",
  "stmt_create_syncer", "stmt_drop_syncer", "stmt_create_table",
  "stmt_virtual_table_field_list", "using_dataschema", "table_name",
  "create_table_definition_group", "column_definition", "data_type",
  "table_options", "stmt_drop_table", "stmt_show_tables", "opt_full",
  "opt_from_dbname", "opt_like", "stmt_table_if_exists",
  "stmt_table_if_not_exists", "stmt_describe", "stmt_create_task",
  "stmt_drop_task", "stmt_show_task", "stmt_describe_task", "stmt_union",
  "query_union", "stmt_union_opt", "update_stmt", "update_asgn_list",
  "stmt_create_user", "stmt_drop_user", "stmt_show_user",
  "stmt_describe_user", "stmt_alter_user", "stmt_create_virtual_field",
  "stmt_create_virtual_field_expr", "stmt_drop_virtual_field",
  "stmt_show_virtual_field", "stmt_describe_virtual_field",
  "stmt_data_type", "stmt_infofield", "W_WORD", YY_NULLPTR
  };


  const unsigned short int
  Parser::yyrline_[] =
  {
       0,   696,   696,   696,   700,   701,   702,   703,   704,   705,
     706,   707,   708,   709,   710,   711,   712,   713,   714,   715,
     716,   717,   718,   719,   720,   721,   722,   723,   724,   725,
     726,   727,   728,   729,   730,   731,   732,   733,   734,   735,
     736,   737,   738,   739,   740,   741,   742,   743,   744,   745,
     746,   747,   748,   749,   750,   751,   752,   753,   754,   755,
     756,   757,   758,   759,   760,   761,   762,   763,   764,   765,
     766,   767,   768,   769,   770,   771,   772,   773,   774,   775,
     776,   777,   778,   779,   780,   781,   782,   783,   784,   785,
     786,   787,   788,   789,   790,   791,   792,   793,   794,   795,
     796,   797,   798,   799,   800,   801,   802,   803,   804,   805,
     806,   807,   808,   809,   810,   811,   812,   813,   814,   815,
     816,   817,   818,   819,   824,   839,   854,   861,   867,   878,
     890,   901,   913,   932,   950,   967,   978,   982,   989,  1000,
    1012,  1023,  1033,  1044,  1062,  1092,  1107,  1119,  1123,  1130,
    1134,  1141,  1145,  1150,  1154,  1161,  1172,  1182,  1193,  1198,
    1205,  1220,  1238,  1254,  1266,  1277,  1287,  1298,  1308,  1329,
    1358,  1363,  1368,  1374,  1378,  1384,  1390,  1396,  1401,  1426,
    1456,  1482,  1486,  1492,  1498,  1504,  1510,  1517,  1524,  1531,
    1537,  1544,  1551,  1558,  1566,  1570,  1575,  1581,  1585,  1591,
    1595,  1600,  1606,  1610,  1616,  1620,  1627,  1635,  1642,  1651,
    1662,  1670,  1679,  1688,  1702,  1713,  1724,  1735,  1745,  1761,
    1788,  1814,  1818,  1823,  1834,  1844,  1855,  1865,  1879,  1894,
    1905,  1917,  1930,  1942,  1950,  1960,  1976,  1980,  1985,  1990,
    2018,  2033,  2043,  2052,  2061,  2085,  2096,  2106,  2118,  2128,
    2137,  2147,  2157,  2164,  2172,  2179,  2188,  2195,  2203,  2210,
    2219,  2239,  2246,  2251,  2266,  2283,  2300,  2317,  2334,  2351,
    2356,  2373,  2380,  2387,  2392,  2397,  2402,  2424,  2431,  2448,
    2465,  2482,  2487,  2495,  2502,  2509,  2517,  2522,  2527,  2532,
    2538,  2545,  2561,  2585,  2602,  2619,  2627,  2633,  2638,  2643,
    2649,  2655,  2661,  2669,  2679,  2683,  2691,  2698,  2702,  2708,
    2714,  2722,  2727,  2735,  2744,  2750,  2756,  2762,  2769,  2776,
    2780,  2787,  2801,  2805,  2811,  2819,  2823,  2829,  2839,  2856,
    2860,  2866,  2874,  2883,  2887,  2895,  2904,  2927,  2941,  2953,
    2966,  2980,  2984,  2991,  3001,  3013,  3019,  3027,  3040,  3043,
    3047,  3051,  3055,  3061,  3066,  3072,  3076,  3083,  3089,  3096,
    3103,  3109,  3113,  3118,  3123,  3143,  3167,  3178,  3188,  3198,
    3208,  3232,  3244,  3255,  3266,  3276,  3287,  3298,  3311,  3321,
    3333,  3347,  3357,  3372,  3390,  3400,  3414,  3434,  3446,  3450,
    3457,  3461,  3468,  3472,  3478,  3500,  3511,  3521,  3533,  3544,
    3554,  3570,  3582,  3586,  3593,  3598,  3603,  3609,  3620,  3639,
    3658,  3676,  3680,  3687,  3698,  3709,  3720,  3734,  3750,  3773,
    3785,  3799,  3811,  3822,  3833,  3841,  3851,  3875,  3879,  3884,
    3889,  3894,  3899,  3904,  3909,  3914,  3921,  3927,  3934,  3968,
    3978,  3984,  3991,  3997,  4006,  4014,  4023,  4033,  4043,  4056,
    4060,  4070,  4075,  4082,  4093,  4097,  4103,  4109,  4115,  4124,
    4133,  4141,  4147,  4154,  4158,  4166,  4177,  4183,  4190,  4194,
    4199,  4206,  4210,  4218,  4222,  4228,  4232,  4238,  4242,  4248,
    4263,  4267,  4280,  4284,  4290,  4301,  4320,  4324,  4330,  4334,
    4340,  4344,  4349,  4355,  4359,  4364,  4370,  4374,  4379,  4385,
    4389,  4394,  4400,  4404,  4409,  4414,  4428,  4438,  4450,  4462,
    4473,  4484,  4495,  4509,  4526,  4536,  4554,  4571,  4591,  4612,
    4634,  4657,  4670,  4681,  4685,  4692,  4698,  4705,  4710,  4723,
    4729,  4736,  4742,  4752,  4763,  4773,  4783,  4797,  4808,  4835,
    4847,  4860,  4875,  4885,  4897,  4901,  4907,  4912,  4925,  4930,
    4945,  4958,  4967,  4983,  4997,  5008,  5019,  5031,  5039,  5047,
    5055,  5063,  5071,  5081,  5091,  5100,  5108,  5116,  5124,  5132,
    5140,  5148,  5156,  5164,  5172,  5180,  5188,  5196,  5204,  5212,
    5220,  5228,  5236,  5245,  5255,  5260,  5268,  5276,  5284,  5292,
    5298,  5304,  5317,  5329,  5343,  5347,  5354,  5358,  5365,  5369,
    5377,  5381,  5388,  5392,  5399,  5409,  5419,  5438,  5449,  5459,
    5470,  5478,  5491,  5502,  5507,  5514,  5533,  5557,  5579,  5592,
    5603,  5613,  5624,  5636,  5664,  5687,  5698,  5708,  5719,  5723,
    5730,  5734,  5739,  5744,  5749,  5754,  5759,  5764,  5769,  5774,
    5779,  5784,  5789,  5794,  5799,  5804,  5809,  5814,  5819,  5824,
    5829,  5834,  5839,  5844,  5849,  5854,  5859,  5864,  5869,  5874,
    5879,  5884,  5889,  5894,  5899,  5904,  5909,  5914,  5919,  5924,
    5929,  5934,  5939,  5944,  5949,  5954,  5959,  5964,  5969,  5974,
    5979,  5984,  5989,  5994,  5999,  6004,  6009,  6014,  6019,  6024,
    6029,  6034,  6039,  6044,  6049,  6054,  6059,  6064,  6069,  6074,
    6079,  6084,  6089,  6094,  6099,  6104,  6109,  6114,  6119,  6124,
    6129,  6134,  6139,  6144,  6149,  6154,  6159,  6164,  6169,  6174,
    6179,  6184,  6189,  6194,  6199,  6204,  6209,  6214,  6219,  6224,
    6229,  6234,  6239,  6244,  6249,  6254,  6259,  6264,  6269,  6274,
    6279,  6284,  6289,  6294,  6299,  6304,  6309,  6314,  6319,  6324,
    6329,  6334,  6339,  6344,  6349,  6354,  6359,  6364,  6369,  6374,
    6379,  6384,  6389,  6394,  6399,  6404,  6409,  6414,  6419,  6424,
    6429,  6434,  6439,  6444,  6449,  6454,  6459,  6464,  6469,  6474,
    6479,  6484,  6489,  6494,  6499,  6504,  6509,  6514,  6519,  6524,
    6529,  6534,  6539,  6544,  6549,  6554,  6559,  6564,  6569,  6574,
    6579,  6584,  6589,  6594,  6599,  6604,  6609,  6614,  6619,  6624,
    6629,  6634,  6639,  6644,  6649,  6654,  6659,  6664,  6669,  6674,
    6679,  6684,  6689,  6694,  6699,  6704,  6709,  6714,  6719,  6724,
    6729,  6734,  6739,  6744,  6749,  6754,  6759,  6764,  6769,  6774,
    6779,  6784,  6789,  6794,  6799,  6804,  6809,  6814,  6819,  6824,
    6829,  6834,  6839,  6844,  6849,  6854,  6859,  6864,  6869,  6874,
    6879,  6884,  6889,  6894,  6899,  6904,  6909,  6914,  6919,  6924,
    6929,  6934,  6939,  6944,  6949,  6954,  6959,  6964,  6969,  6974,
    6979,  6984,  6989,  6994,  6999,  7004,  7009,  7014,  7019,  7024,
    7029
  };

  // Print the state stack on the debug stream.
  void
  Parser::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << i->state;
    *yycdebug_ << std::endl;
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  Parser::yy_reduce_print_ (int yyrule)
  {
    unsigned int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):" << std::endl;
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG

  // Symbol number corresponding to token number t.
  inline
  Parser::token_number_type
  Parser::yytranslate_ (int t)
  {
    static
    const token_number_type
    translate_table[] =
    {
     0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    14,     2,     2,     2,    22,    16,     2,
     332,   333,    20,    18,   334,    19,    26,    21,   340,   341,
       2,     2,     2,     2,     2,     2,     2,     2,   335,   331,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   336,     2,   337,    24,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   338,    15,   339,     2,     2,     2,     2,
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
       5,     6,     7,     8,     9,    10,    11,    12,    13,    17,
      23,    25,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189,   190,   191,   192,   193,   194,
     195,   196,   197,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,   208,   209,   210,   211,   212,   213,   214,
     215,   216,   217,   218,   219,   220,   221,   222,   223,   224,
     225,   226,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,   238,   239,   240,   241,   242,   243,   244,
     245,   246,   247,   248,   249,   250,   251,   252,   253,   254,
     255,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330
    };
    const unsigned int user_token_number_max_ = 575;
    const token_number_type undef_token_ = 2;

    if (static_cast<int>(t) <= yyeof_)
      return yyeof_;
    else if (static_cast<unsigned int> (t) <= user_token_number_max_)
      return translate_table[t];
    else
      return undef_token_;
  }


} // yy
#line 14072 "Parser.tab.cc" // lalr1.cc:1155
#line 7034 "Parser.yy" // lalr1.cc:1156

//*****************************************************************************************
/*
int 
main(int argc, char** argv)
{
    cout << "> ";
  
    extern FILE *yyin;
    yy::SQLParse sqlParser;  // create a SQL Parser

    for(int i=1; i<argc; i++)
    {
        if (strcmp(argv[i], "-d") == 0)
        {
      	    sqlParser.set_debug_level(1);
        }
        if (strcmp(argv[i], "-f") == 0)
        {
      	    yyin = fopen(argv[i+1], "r");
        }
    }
  
  	char *str = new char[100];
  	yy_scan_bytes(str, 10);

    int v = sqlParser.parse();  // and run it

    cout << "Leave main v : " << v << endl;
    return v;
}
*/

// Bison not create this function, so i create it
namespace yy {
    void
    Parser::error(location const &loc, const std::string& s)
    {
      std::cerr << "error at " << loc << ": " << s << std::endl;
    };

}

