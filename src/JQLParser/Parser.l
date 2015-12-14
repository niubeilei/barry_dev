/* recognize tokens for the C++ SQL Parse */

%option noyywrap case-insensitive
%option yylineno


%{
#ifndef YYDEBUG
#define YYDEBUG 1
#endif

# include <stdarg.h>
# include <cstdlib>
#include <vector>
#include <string>


using namespace std;

//your files
#include "API/AosApi.h"
#include "Util/String.h"
#include "JQLParser/JQLParser.h"
#include "JQLParser/ParserInclude.h"
#include "Parser.tab.hh"

extern AosJQLParser gAosJQLParser;

//yang 2015-08-03
extern std::vector<std::string> gTokenBuf;
extern void getToken(const char* token);

// CHENDING
#undef YY_INPUT
#define YY_INPUT(b,r,s) AosJQLParser_ReadInput(b,&r,s)
#define YY_DECL int yylex(yy::Parser::semantic_type *yylval, yy::Parser::location_type *yylloc)

int yycolumn = 1;
//#define YY_USER_ACTION yylloc->columns (yyleng);
#define YY_USER_ACTION yylloc->begin.line = yylloc->end.line = yylineno; \
                       yylloc->begin.column = yycolumn; yylloc->end.column = yycolumn + yyleng - 1; \
					   yycolumn += yyleng;

typedef yy::Parser::token token;
const int MAX_LEN = 1000;

void yyerror(const char *s, ...);

int oldstate;
char *errMesg = new char[256];


%}

%x COMMENT
%s BTWMODE

%%

%{
	yylloc->step();
%}


AND   { yylval->subtok = AosExprLogic::eAnd; getToken(yytext); return token::LOGIC; }
OR   { yylval->subtok = AosExprLogic::eOr; getToken(yytext); return token::LOGIC; }

ACCEPTED   			{ getToken(yytext);/*yang 2015-08-03*/ return token::ACCEPTED; }
ACTION				{ getToken(yytext); return token::ACTION; }
ACTOR				{ getToken(yytext); return token::ACTOR; }
ACCUMULATE			{ getToken(yytext); return token::ACCUMULATE; }
ADDRESS				{ getToken(yytext); return token::ADDRESS;}
ALTER				{ getToken(yytext); return token::ALTER;}
ALL					{ getToken(yytext); return token::ALL;}
AT					{ getToken(yytext); return token::AT; }
AS	   				{ getToken(yytext); return token::AS; }
ASC  				{ getToken(yytext); return token::ASC; }
AUTO_INCREMENT 		{ getToken(yytext); return token::AUTO_INCREMENT; }
BETWEEN				{ getToken(yytext); return token::BETWEEN;}
BIT 				{ getToken(yytext); return token::BIT; }
BIGINT 				{ getToken(yytext); return token::BIGINT; }
BINARY 				{ getToken(yytext); return token::BINARY; }
BLOCKSIZE			{ getToken(yytext); return token::BLOCKSIZE; }
BY					{ getToken(yytext); return token::BY; }
BYTE				{ getToken(yytext); return token::BYTE; }
BYTES				{ getToken(yytext); return token::BYTES; }
CACHE				{ getToken(yytext); return token::CACHE;}
CASE				{ getToken(yytext); return token::CASE;}
COMBINE				{ getToken(yytext); return token::COMBINE;}
CONTAIN				{ getToken(yytext); return token::CONTAIN;}
CONVERT				{ getToken(yytext); return token::CONVERT;}
COMBINATIONS		{ getToken(yytext); return token::COMBINATIONS;}
CHECK				{ getToken(yytext); return token::CHECK;}
CHECKSUM 			{ getToken(yytext); return token::CHECKSUM; }
CHAR 				{ getToken(yytext); return token::CHAR; }
CODING				{ getToken(yytext); return token::CODING;}
CONNECTOR			{ getToken(yytext); return token::CONNECTOR;}	
CONNECTORS			{ getToken(yytext); return token::CONNECTORS;}	
CONFIG				{ getToken(yytext); return token::CONFIG;}	
COMMENTS 			{ getToken(yytext); return token::COMMENTS; }
COMPLETE 			{ getToken(yytext); return token::COMPLETE; }
CUBE	 			{ getToken(yytext); return token::CUBE; }
CSV		 			{ getToken(yytext); return token::CSV; }
CYCLE		 		{ getToken(yytext); return token::CYCLE; }
CREATE				{ getToken(yytext); return token::CREATE;}
DATA				{ getToken(yytext); return token::DATA; }
DATASET				{ getToken(yytext); return token::DATASET; }
DATASETS			{ getToken(yytext); return token::DATASETS; }
DATABASE			{ getToken(yytext); return token::DATABASE; }
DATABASES			{ getToken(yytext); return token::DATABASES; }
DATE				{ getToken(yytext); return token::DATE; }
DEBUG				{ getToken(yytext); return token::DEBUG; }
DELAYED				{ getToken(yytext); return token::DELAYED;}
DELETE				{ getToken(yytext); return token::DELETE;}
DELIMITER 			{ getToken(yytext); return token::DELIMITER;}
DEFAULT    			{ getToken(yytext); return token::DEFAULT; } 
DESC  				{ getToken(yytext); return token::DESC; }
DESCRIBE			{ getToken(yytext); return token::DESCRIBE; }
DICTIONARY			{ getToken(yytext); return token::DICTIONARY;}
DIMENSIONS			{ getToken(yytext); return token::DIMENSIONS;}
DIR  				{ getToken(yytext); return token::DIR; }
DIRECTORY			{ getToken(yytext); return token::DIRECTORY; }
DISTINCT			{ getToken(yytext); return token::DISTINCT;}
DISTINCTROW			{ getToken(yytext); return token::DISTINCTROW; }
DOC					{ getToken(yytext); return token::DOC; }
DOCBEGIN			{ getToken(yytext); return token::DOCBEGIN;}
DOCEND				{ getToken(yytext); return token::DOCEND;}
DOES				{ getToken(yytext); return token::DOES; }
DOUBLE 				{ getToken(yytext); return token::DOUBLE; }
DECIMAL 			{ getToken(yytext); return token::DECIMAL; }
DROP    			{ getToken(yytext); return token::DROP; }
DROPPED    			{ getToken(yytext); return token::DROPPED; }
DUAL				{ getToken(yytext); return token::DUAL; }
DYNAMIC    			{ getToken(yytext); return token::DYNAMIC; }
ELSE				{ getToken(yytext); return token::ELSE;}
ENCLOSED			{ getToken(yytext); return token::ENCLOSED; }
ENCODING	   		{ getToken(yytext); return token::ENCODING; }
END	   				{ getToken(yytext); return token::END; }
ENGING				{ getToken(yytext); return token::ENGING; }
ESCAPED				{ getToken(yytext); return token::ESCAPED; }
EXCLUSIVE  			{ getToken(yytext); return token::EXCLUSIVE; } 
EXISTS 				{ getToken(yytext); return token::EXISTS; }
EXPRESSION			{ getToken(yytext); return token::EXPRESSION; }
EXPORT				{ getToken(yytext); return token::EXPORT;}
FIELD				{ getToken(yytext); return token::FIELD;}
FIELDS				{ getToken(yytext); return token::FIELDS;}
FILENAME   			{ getToken(yytext); return token::FILENAME; }
FILTER     			{ getToken(yytext); return token::FILTER; }
FILTERED   			{ getToken(yytext); return token::FILTERED; }
FIXED		 		{ getToken(yytext); return token::FIXED; }
FLOAT   			{ getToken(yytext); return token::FLOAT; }
FROM				{ getToken(yytext); return token::FROM; } 
FORMATTER			{ getToken(yytext); return token::FORMATTER; }
FORMAT				{ getToken(yytext); return token::FORMAT; }
FULL				{ getToken(yytext); return token::FULL; } 
GROUP				{ getToken(yytext); return token::GROUP; }
HAVING				{ getToken(yytext); return token::HAVING;}
HBASE				{ getToken(yytext); return token::HBASE;}
HIGH_PRIORITY		{ getToken(yytext); return token::HIGH_PRIORITY; }
JIMODB				{ getToken(yytext); return token::JIMODB; }
ID					{ getToken(yytext); return token::ID; }
IDENTIFIED			{ getToken(yytext); return token::IDENTIFIED;}
IDFAMILY			{ getToken(yytext); return token::IDFAMILY; }
IIL					{ getToken(yytext); return token::IIL; }
IILNAME				{ getToken(yytext); return token::IILNAME;}
IN					{ getToken(yytext); return token::IN;}
INCREMENT			{ getToken(yytext); return token::INCREMENT;}
INDEX 				{ getToken(yytext); return token::INDEX; }
INDEXES 			{ getToken(yytext); return token::INDEXES; }
INNER      			{ getToken(yytext); return token::INNER; } 
INPUT				{ getToken(yytext); return token::INPUT; }
INT 				{ getToken(yytext); return token::INT; }
INTO				{ getToken(yytext); return token::INTO;}
INTO_OUTFILE	    { getToken(yytext); return token::INTO_OUTFILE;}
INVALID 			{ getToken(yytext); return token::INVALID; }
INSERT				{ getToken(yytext); return token::INSERT;}
IGNORE				{ getToken(yytext); return token::IGNORE;}
IF					{ getToken(yytext); return token::IF;}
IS					{ getToken(yytext); return token::IS;}
JIMOID	   			{ getToken(yytext); return token::JIMOID; }
JIMOLOGIC	   		{ getToken(yytext); return token::JIMOLOGIC; }
CHECKPOINT	   		{ getToken(yytext); return token::CHECKPOINT; }
ENDPOINT	   		{ getToken(yytext); return token::ENDPOINT; }
MATRIX	   			{ getToken(yytext); return token::MATRIX; }
RESULTS	   			{ getToken(yytext); return token::RESULTS; }
JAVA				{ getToken(yytext); return token::JAVA; }
JOB					{ getToken(yytext); return token::JOB; }
JOBS				{ getToken(yytext); return token::JOBS; }
JOIN			    { getToken(yytext); return token::JOIN; } 
KEY					{ getToken(yytext); return token::KEY; }
LEFT			    { getToken(yytext); return token::LEFT; } 
LENGTH				{ getToken(yytext); return token::LENGTH;}
LIMIT			    { getToken(yytext); return token::LIMIT; } 
LIKE  				{ getToken(yytext); return token::LIKE; }
LINES 				{ getToken(yytext); return token::LINES ; }
LIST				{ getToken(yytext); return token::LIST;}
LOAD				{ getToken(yytext); return token::LOAD; }
LOG					{ getToken(yytext); return token::LOG; }
LOGIN				{ getToken(yytext); return token::LOGIN;}
LOGOUT				{ getToken(yytext); return token::LOGOUT;}
LOW_PRIORITY		{ getToken(yytext); return token::LOW_PRIORITY;}
MAPPING				{ getToken(yytext); return token::MAPPING; }
MAPTASK				{ getToken(yytext); return token::MAPTASK; }
MATCHED    			{ getToken(yytext); return token::MATCHED; }
MAXVALUE    		{ getToken(yytext); return token::MAXVALUE; }
MINVALUE    		{ getToken(yytext); return token::MINVALUE; }
MAX					{ getToken(yytext); return token::MAX; }
MAP					{ getToken(yytext); return token::MAP;}
MAPS				{ getToken(yytext); return token::MAPS;}
DISTRIBUTIONMAP		{ getToken(yytext); return token::DISTRIBUTIONMAP;}
DISTRIBUTIONMAPS	{ getToken(yytext); return token::DISTRIBUTIONMAPS;}
MEASURES			{ getToken(yytext); return token::MEASURES; }
MEDIUMINT 			{ getToken(yytext); return token::MEDIUMINT; }
MISMATCHED 			{ getToken(yytext); return token::MISMATCHED; }
MULTIPLE   			{ getToken(yytext); return token::MULTIPLE; } 
MOD			  	    { getToken(yytext); return token::MOD; }
MODE			  	{ getToken(yytext); return token::MODE; }
MODEL			  	{ getToken(yytext); return token::MODEL; }
NICK				{ getToken(yytext); return token::NICK; }
NOT				    { getToken(yytext); return token::NOT; }
NOORDER				{ getToken(yytext); return token::NOORDER; }
NOMAXVALUE			{ getToken(yytext); return token::NOMAXVALUE; }
NOMINVALUE			{ getToken(yytext); return token::NOMINVALUE; }
NOCACHE			 	{ getToken(yytext); return token::NOCACHE; }
NOCYCLE				{ getToken(yytext); return token::NOCYCLE; }
NULL			    { getToken(yytext); return token::NULLX; }
NUMBER				{ getToken(yytext); return token::NUMBER;}
ON 					{ getToken(yytext); return token::ON; }
OFF 				{ getToken(yytext); return token::OFF; }
OFFSET				{ getToken(yytext); return token::OFFSET; }
OPERATOR			{ getToken(yytext); return token::OPERATOR;}
ORDER				{ getToken(yytext); return token::ORDER; }
OUTER      			{ getToken(yytext); return token::OUTER; } 
OUTFILE				{ getToken(yytext); return token::OUTFILE; }
OUTPUT				{ getToken(yytext); return token::OUTPUT; }
OVERRIDE			{ getToken(yytext); return token::OVERRIDE; }
PATH				{ getToken(yytext); return token::PATH; }
PARSER				{ getToken(yytext); return token::PARSER; }
PICKED				{ getToken(yytext); return token::PICKED; }
PICKER				{ getToken(yytext); return token::PICKER; }
PICKERS				{ getToken(yytext); return token::PICKERS; }
PRINT				{ getToken(yytext); return token::PRINT; }
PROC				{ getToken(yytext); return token::PROC; }
PROCS				{ getToken(yytext); return token::PROCS; }
PRIORITY			{ getToken(yytext); return token::PRIORITY; }
PORT				{ getToken(yytext); return token::PORT;}
POS					{ getToken(yytext); return token::POS; }
QUICK				{ getToken(yytext); return token::QUICK;}
QUALIFIER			{ getToken(yytext); return token::QUALIFIER;}
ROLLUP				{ getToken(yytext); return token::ROLLUP; }
RAWKEY				{ getToken(yytext); return token::RAWKEY; }
RECORD				{ getToken(yytext); return token::RECORD; }
RECORDS				{ getToken(yytext); return token::RECORDS; }
REDUCE				{ getToken(yytext); return token::REDUCE; }
REDUCETASK			{ getToken(yytext); return token::REDUCETASK; }
RETRIEVE			{ getToken(yytext); return token::RETRIEVE; }
RIGHT				{ getToken(yytext); return token::RIGHT; } 
ROW					{ getToken(yytext); return token::ROW; } 
RUN					{ getToken(yytext); return token::RUN; } 
SCANNER 			{ getToken(yytext); return token::SCANNER; }
SCANNERS 			{ getToken(yytext); return token::SCANNERS; }
SCHEDULE			{ getToken(yytext); return token::SCHEDULE; }
SCHEMA				{ getToken(yytext); return token::SCHEMA; }
SCHEMAS				{ getToken(yytext); return token::SCHEMAS; }
SET					{ getToken(yytext); return token::SET;}
SELECT				{ getToken(yytext); return token::SELECT; } 
SEQUENCE			{ getToken(yytext); return token::SEQUENCE; } 
SELECTOR			{ getToken(yytext); return token::SELECTOR; } 
SELECTORS			{ getToken(yytext); return token::SELECTORS; } 
SERVER				{ getToken(yytext); return token::SERVER; }
SERVICE				{ getToken(yytext); return token::SERVICE; }
SERVICES			{ getToken(yytext); return token::SERVICES; }
SHOW				{ getToken(yytext); return token::SHOW; }
SINGLE				{ getToken(yytext); return token::SINGLE; }
SIZE				{ getToken(yytext); return token::SIZE; }
SMALLINT			{ getToken(yytext); return token::SMALLINT; }
SHUFFLE				{ getToken(yytext); return token::SHUFFLE; }
SPLIT				{ getToken(yytext); return token::SPLIT; }
SOURCE				{ getToken(yytext); return token::SOURCE; }
SQL_BIG_RESULT		{ getToken(yytext); return token::SQL_BIG_RESULT; }
SQL_CALC_FOUND_ROWS	{ getToken(yytext); return token::SQL_CALC_FOUND_ROWS; }
SQL_SMALL_RESULT	{ getToken(yytext); return token::SQL_SMALL_RESULT; }
START 				{ getToken(yytext); return token::START;}
STARTING			{ getToken(yytext); return token::STARTING;}
STAT				{ getToken(yytext); return token::STAT;}
STATUS				{ getToken(yytext); return token::STATUS;}
STATIC				{ getToken(yytext); return token::STATIC;}
STATISTICS			{ getToken(yytext); return token::STATISTICS; }	
STINCT				{ getToken(yytext); return token::STINCT; }	
STOP				{ getToken(yytext); return token::STOP;}
STRAIGHT_JOIN                   { getToken(yytext); return token::STRAIGHT_JOIN; }
SUB			        { getToken(yytext); return token::SUB; }
SUPPRESS			{ getToken(yytext); return token::SUPPRESS; }
SYNCER				{ getToken(yytext); return token::SYNCER; }
TABLE 				{ getToken(yytext); return token::TABLE; }
TABLES 				{ getToken(yytext); return token::TABLES; }
TASK				{ getToken(yytext); return token::TASK; }
TASKS				{ getToken(yytext); return token::TASKS; }
TEXT				{ getToken(yytext); return token::TEXT; }
TERMINATED			{ getToken(yytext); return token::TERMINATED; }
THEN				{ getToken(yytext); return token::THEN;}
TIME				{ getToken(yytext); return token::TIME; }
TIMES				{ getToken(yytext); return token::TIMES; }
TINYINT 			{ getToken(yytext); return token::TINYINT; }
THREAD				{ getToken(yytext); return token::THREAD;}
TO					{ getToken(yytext); return token::TO; }
TRIM				{ getToken(yytext); return token::TRIM; }
TYPE	   			{ getToken(yytext); return token::TYPE; }
UNIFORM 			{ getToken(yytext); return token::UNIFORM; }
UNIT				{ getToken(yytext); return token::UNIT; }
UNION				{ getToken(yytext); return token::UNION; }
UNSIGNED 			{ getToken(yytext); return token::UNSIGNED; }
UPDATE				{ getToken(yytext); return token::UPDATE;}
USE     			{ getToken(yytext); return token::USE; }
USING     			{ getToken(yytext); return token::USING; }
USER				{ getToken(yytext); return token::USER; }
USERS				{ getToken(yytext); return token::USERS; }
VALUE      			{ getToken(yytext); return token::VALUE; } 
VALUES 				{ getToken(yytext); return token::VALUES;}
VARCHAR 			{ getToken(yytext); return token::VARCHAR;}
VARCHAR2 			{ getToken(yytext); return token::VARCHAR2;}
VARIABLE			{ getToken(yytext); return token::VARIABLE;}
VIRTUAL				{ getToken(yytext); return token::VIRTUAL;}
WAIT				{ getToken(yytext); return token::WAIT;}
WHEN				{ getToken(yytext); return token::WHEN;}
WHERE				{ getToken(yytext); return token::WHERE;}
WITH				{ getToken(yytext); return token::WITH;}
XO         			{ getToken(yytext); return token::XO; } 
XOR	   				{ getToken(yytext); return token::XOR; }
XPATH				{ getToken(yytext); return token::XPATH;}
"["					{ getToken(yytext); return token::LEFT_BRACKET;}
"]"					{ getToken(yytext); return token::RIGHT_BRACKET;}
"SYSTEM_ARGS"   { getToken(yytext); return token::SYSTEM_ARGS; }

[-+&~|^/%*(),.;!:]   { getToken(yytext); return yytext[0]; }
   
   /* numbers */
-?[0-9]+ {
		OmnString str = yytext;
		yylval->ll_value = str.parseInt64(0); 
		getToken(yytext);
		return token::INTNUM; 
} 

-?[0-9]+"."[0-9]* |
-?"."[0-9]+	|
-?[0-9]+E[-+]?[0-9]+	|
-?[0-9]+"."[0-9]*E[-+]?[0-9]+ |
-?"."[0-9]*E[-+]?[0-9]+	{
		yylval->double_val = atof(yytext);
        getToken(yytext);
		return token::DOUBLE_NUM; 
}


    /* booleans */
TRUE	{ cout<< "true" <<endl; yylval->ll_value = 1; return token::BOOL; }
UNKNOWN	{ yylval->ll_value = -1; return token::BOOL; }
FALSE	{ yylval->ll_value = 0; return token::BOOL; }

   /* strings */
'(\\.|''|[^'\n])*'	|
\"(\\.|\"\"|[^"\n])*\"  { 
							u32 len = strlen(yytext);
							char *str = 0;
							str = strdup(&yytext[1]);
							str[len -2] = 0;
							yylval->strval = strdup(str);
							free(str);
							getToken(yytext);
							return token::STRING; 
						}

'(\\.|[^'\n])*$	        { yyerror("Unterminated string %s", yytext); }
\"(\\.|[^"\n])*$	{ yyerror("Unterminated string %s", yytext); }

    /* NAME */
 [_a-zA-Z0-9][_a-zA-Z0-9]*	{ 
	 						/*
							size_t len = strlen(yytext);
							size_t i = 0;
							for (; i<len; i++) yytext[i] = tolower(yytext[i]);
							*/
	 						yylval->strval = strdup(yytext); 
							getToken(yytext);
							return token :: NAME;
							}


   /* name */
`(\\.|``|[^`\n])*`  { 
							size_t len = strlen(yytext);
							char *str = 0;
							str = strdup(&yytext[1]);
							str[len -2] = 0;
							/*
							size_t i = 0;
							for (; i<len-2; i++) str[i] = tolower(str[i]);
							*/
							yylval->strval = strdup(str);
							free(str);
							getToken(yytext);
							return token::NAME; 
						}
 
`[^`\n]*$               { yyerror("unterminated quoted name %s", yytext); }

"&&"   { yylval->subtok = AosExprLogic::eAnd; getToken(yytext); return token::LOGIC; }
"||"   { yylval->subtok = AosExprLogic::eOr; getToken(yytext); return token::LOGIC; }
"="    { yylval->subtok = AosExprComparison::eEqual;      getToken(yytext); return token::COMPARISON; }
"<=>"  { yylval->subtok = AosExprComparison::eInvalid;        getToken(yytext); return token::COMPARISON; }
">="   { yylval->subtok = AosExprComparison::eLargerEqual; getToken(yytext); return token::COMPARISON; }
">"    { yylval->subtok = AosExprComparison::eLarger;      getToken(yytext); return token::COMPARISON; }
"<="   { yylval->subtok = AosExprComparison::eLessEqual;   getToken(yytext); return token::COMPARISON; }
"<"    { yylval->subtok = AosExprComparison::eLess;        getToken(yytext); return token::COMPARISON; }
"!="   |
"<>"   { yylval->subtok = AosExprComparison::eNotEqual;    getToken(yytext); return token::COMPARISON; }

"<<"   { yylval->subtok = 1; getToken(yytext); return token::SHIFT; }
">>"   { yylval->subtok = 2; getToken(yytext); return token::SHIFT; }
":="   { yylval->subtok = 2; getToken(yytext); return token::ASSIGNMENT; }


	/* XML DOC */
	/* remove this token for now since it conflicts */
	/*  with comparsion symbols <, > */
 /* \<(.*+)\>+			{yylval->strval = strdup(yytext); */
 /*							 return token::XMLDOC;} */

	/* user variables */
@[0-9a-z_.$]+ |
@\"[^"\n]+\" |
@`[^`\n]+` |
@'[^'\n]+' { yylval->strval = strdup(yytext+1);getToken(yytext); return token::USERVAR; }

@\"[^"\n]*$ { yyerror("unterminated quoted user variable %s", yytext); }
@`[^`\n]*$ { yyerror("unterminated quoted user variable %s", yytext); }
@'[^'\n]*$ { yyerror("unterminated quoted user variable %s", yytext); }


[ \t]       { yylloc->step(); }/* white space */

\b[\0-\x7F]+\b |
\b[\xC0-\xDF][\x80-\xBF]+\b | 
\b[\xE0-\xEF][\x80-\xBF][\x80-\xBF]+\b |
\b[\xF0-\xF7][\x80-\xBF][\x80-\xBF][\x80-\xBF]+\b |
\b[\xF8-\xFb][\x80-\xBF][\x80-\xBF][\x80-\xBF][\x80-\xBF]+\b |
\b[\xFc-\xFd][\x80-\xBF][\x80-\xBF][\x80-\xBF][\x80-\xBF][\x80-\xBF]+\b { yylval->strval = strdup(yytext); return token :: U8VAR;}

.           { yyerror("mystery character '%c'", *yytext); }
%%


int AosParseJQL(
		char * data, 
		const bool debug_flag)
{
	gAosJQLParser.finishParse(); 
	//cout << __FILE__ << ":" << __LINE__ << "Start parsing" << endl;
	gAosJQLParser.setContents(data);

	yy::Parser parser;  // create a SQL Parser
	if (debug_flag) parser.set_debug_level(1);
	
	//yang
	gTokenBuf.clear();
	yylex_destroy();
	//cout << "File Buff : " << data << endl;
	//cout << __FILE__ << ":" << __LINE__ << ": Befor parse" << endl;
	int v = parser.parse();  // and run it
	//cout << __FILE__ << ":" << __LINE__ << ": After parsing: " << v << endl;
	//gAosJQLParser.finishParse();

	return v;
}


void
emit(const char *s, ...)
{
	//extern yylineno;

	va_list ap;
	va_start(ap, s);

	printf("rpn: ");
	vfprintf(stdout, s, ap);
	printf("\n");
}


void
yyerror(const char *s, ...)
{
	//extern yylineno;

	va_list ap;
	va_start(ap, s);

	fprintf(stderr, "%d: error: ", 0);
	vfprintf(stderr, s, ap);
	fprintf(stderr, "\n");
	va_end(ap); // add by young
}
