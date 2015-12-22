/*
 this is comment 
*/

%language "C++" //create C++ files
%defines        //create head files
%locations

%define parser_class_name "Parser" //define your class name


%{
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

%}

%initial-action {
    @$.begin.filename = @$.end.filename = new std::string("stdin");
}

//%parse-param { cppcalc_ctx &ctx } //pass param(s) to SQLParse::SQLParse(...);
//%lex-param   { cppcalc_ctx &ctx }

%union {
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
}
	
	/* names and literal values */
       /* operators and precedence levels */
%right ASSIGN

%left <subtok> ASSIGNMENT /*:=*/ 
%left <subtok> LOGIC  /* || &&*/
%left XOR

%nonassoc IN IS LIKE REGEXP
%left BETWEEN
%left <subtok> COMPARISON /* == <> < > <= >= <=> */
%left NOT '!'
%left '|'
%left '&'
%left <subtok> SHIFT /* << >> */
%left '+' '-'
%left '*' '/' '%' MOD
%left '^'
%nonassoc UMINUS
%left '.'				// record field


%token <ll_value> INTNUM
%token <strval> NAME 
%token <strval> U8VAR 
%token <strval> STRING
%token <u64_value> UNSIGNED_LONG
%token <u32_value> UNSIGNED_INT 
%token <bool_val> BOOL
%token <double_val> DOUBLE_NUM
%token <strval> CREATE DELETE   DOCBEGIN DOCEND XMLDOC 	// Andy zhang 2013/10/17
%token <strval> DOC EXISTS
%token <strval> LOGIN LOGOUT						// Andy zhang 2013/10/18
%token <strval> RETRIEVE START STOP CHECK RESTART // Andy zhang 2013/10/18
%token <strval> OVERRIDE BY ANDOP 
%token IGNORE LOW_PRIORITY QUICK SET UPDATE WHERE GROUP KEY 
       /* user @abc names */

%token <strval> USERVAR

%token WAIT
%token DOES
%token ASSIGNMENT 
%token SELECT
%token DELAYED INSERT INTO VALUES 
%token INTO_OUTFILE
%token COMPLETE

%token LEFTSHIFT 
%token ROLLUP
%token RIGHTSHIFT
%token RECORD
%token RECORDS
%token PICKER
%token PICKERS
%token TYPE
%token JIMOID
%token AS
%token END
%token PROC
%token PROCS
%token DEBUG
%token DATA
%token SCANNER
%token SCANNERS
%token DATASET
%token DATASETS
%token DATABASE
%token DATABASES
%token DATE
%token SCHEMA
%token SCHEMAS
%token WITH
%token ORDER
%token CONNECTOR
%token CONNECTORS
%token CUBE
%token USE
%token USING
%token FILTER
%token FILTERED
%token ACCEPTED
%token MATCHED
%token MISMATCHED
%token DROP
%token DROPPED
%token MULTIPLE
%token DEFAULT
%token VALUE
%token SELECTOR
%token SELECTORS
%token INNER
%token FULL 
%token OUTER
%token LEFT
%token RIGHT
%token EXCLUSIVE
%token XO
%token JOIN
%token XOR
%token OR
%token AND
%token NOT
%token ALL
%token ANY
%token MOD
%token MODE
%token ACCUMULATE 
%token BINARY
%token BOTH
%token CASE
%token COMPARISON
%token LOGIC
%token CURRENT_DATE
%token CURRENT_TIME
%token CURRENT_TIMESTAMP
%token DAY_HOUR
%token DAY_MICROSECOND
%token DAY_MINUTE
%token DAY_SECOND
%token ELSE
%token FCOUNT
%token FDATE_ADD
%token FDATE_SUB
%token FOR
%token FROM
%token FSUBSTRING
%token FTRIM
%token HOUR_MICROSECOND
%token HOUR_MINUTE
%token HOUR_SECOND
%token IN
%token INTERVAL
%token IS
%token LIKE
%token NULLX
%token NUMBER 
%token REGEXP
%token SHIFT
%token SOME
%token THEN
%token TRIM
%token TRAILING
%token WHEN
%token YEAR
%token YEAR_MONTH
%token LEADING
%token LIMIT 
%token HAVING 

%token TABLE 
%token TABLES
%token SHOW 
%token INDEX
%token INDEXES
%token AUTO_INCREMENT
%token CHECKSUM
%token ASC
%token DESC
%token COMMENTS
%token CONFIG 
%token BLOCKSIZE
%token UNSIGNED
%token BIT
%token CHAR
%token INT 
%token TINYINT
%token SMALLINT
%token MEDIUMINT
%token VARCHAR 
%token VARCHAR2
%token STR 
%token BIGINT
%token DOUBLE
%token DATETIME
%token DECIMAL 
%token FLOAT
%token TEXT
%token ON
%token OFF
%token SUPPRESS
%token DIR 
%token DESCRIBE 
%token VARIABLE 
%token FIELD 
%token FIELDS 
%token DELIMITER 
%token QUALIFIER 
%token TASKS
%token JOB
%token JOBS
%token STATUS
%token TIMES
%token TIME
%token PRIORITY
%token RUN 
%token AT 
%token ACTOR
%token JAVA 

%token ACTION
%token ENGING
%token INPUT
%token MAPPING
%token MAPTASK
%token OUTPUT
%token REDUCETASK
%token SINGLE
%token TASK
%token SCHEDULE

%token OFFSET
%token EXPRESSION
%token FORM
%token FORMATTER
%token FORMAT
%token LENGTH
%token MAX
%token POS
%token TO 
%token PRINT

%token CODING
%token COMBINE
%token COMBINATIONS
%token FILENAME
%token ID
%token SERVER
%token SERVICE
%token SERVICES
%token SYNCER
%token SIZE
%token SPLIT
%token IILNAME
%token IIL
%token OPERATOR
%token REDUCE

%token STINCT
%token DISTINCT
%token DISTINCTROW
%token HIGH_PRIORITY
%token SQL_BIG_RESULT
%token SQL_CALC_FOUND_ROWS
%token SQL_SMALL_RESULT
%token STRAIGHT_JOIN          
%token UNIFORM 
%token PATH
%token ENCODING 
%token STATIC
%token DYNAMIC
%token ROW
%token PICKED
%token BYTE
%token BYTES
%token IDFAMILY
%token LOAD
%token LOG
%token IDENTIFIED 
%token USER
%token USERS
%token SOURCE
%token DUAL
%token JIMODB 
%token DIRECTORY 
%token STATISTICS 
%token STAT
%token UNIT 
%token UNION 
%token OUTFILE
%token CSV 
%token FIXED 
%token DIMENSIONS 
%token HBASE
%token RAWKEY
%token ADDRESS 
%token PORT 
%token THREAD
%token MEASURES 
%token MODEL 
%token PARSER 
%token MAP 
%token DICTIONARY 
%token DISTRIBUTIONMAP 
%token DISTRIBUTIONMAPS
%token MAPS 
%token VIRTUAL 
%token NICK 
%token LIST 
%token CONTAIN 
%token JIMOLOGIC CHECKPOINT ENDPOINT MATRIX RESULTS EXPORT XPATH
%token BETWEEN


%token TERMINATED 
%token ENCLOSED 
%token ESCAPED 
%token LINES  
%token STARTING 
%token SKIP INVALID SUB 
%token IF 
%token SYSTEM_ARGS 
%token LEFT_BRACKET
%token RIGHT_BRACKET

%token CACHE 
%token CYCLE 
%token INCREMENT 
%token MAXVALUE 
%token MINVALUE 
%token NOCACHE 
%token NOCYCLE 
%token NOMAXVALUE 
%token NOMINVALUE 
%token NOORDER 
%token SEQUENCE 
%token CONVERT
%token INFOFIELD
%token ALTER
%token SHUFFLE 
// WORD DEFINITION SECTION
%type <strval> W_WORD
//%type <strval> STATUS

// Young, 2015/02/11
%type <ll_value> create_sequence_increment_by create_sequence_start_with create_sequence_max_value create_sequence_min_value create_sequence_cache
%type <bool_val> create_sequence_cycle create_sequence_order stmt_infofield

%type <bool_val> stmt_order_opt stmt_union_opt with_order_opts suppress_flag stmt_table_if_exists stmt_table_if_not_exists if_not
%type <strval> opt_from_dbname opt_like data_field_default_value record_delimiter field_delimiter data_idfamily text_qualidier trim_condition field_formater
%type <strval> data_record_type_opts  CSV FIXED split_size_opts using_dataschema iilmap_mode set_job_name 
%type <AosExprValue> expr  stmt_hack_format stmt_hack_into_file stmt_hack_convert_to
%type <u32_value> opt_full stmt_max_task 
%type <ll_value> schedule_on_priority data_field_max_len data_field_offset record_length
%type <AosExprValue> opt_as_alias  data_field 
%type <AosJqlStmtIndexValue>  drop_iil_stmt stmt_list_index show_iil_stmt 

// select statements

%type <AosJQLStmtSelectOptionListValue> select_opts
%type <AosJqlStmtQueryValue> select_stmt 
%type <AosJqlStmtInsertItemValue> insert_stmt 
%type <AosJqlStmtUpdateItemValue> update_stmt 
%type <AosJqlStmtDeleteItemValue> delete_stmt 
%type <AosExprListValue>update_asgn_list insert_vals insert_vals_list column_list opt_col_names data_field_list  expr_list stmt_dimension_list load_data_fields_opts stmt_list_key_list par_key_names run_vals_list parameters field_mapping opt_shuffle_field

%type <AosJqlSelectFieldListValue> select_expr_list rollup_list
%type <AosJqlSelectFieldValue> select_expr 
%type <AosTableReferencesValue> table_references 
%type <AosJqlTableReferenceValue> table_reference 
%type <AosJqlTableFactorValue> table_factor 
%type <AosJqlJoinTableValue> join_table
//%type <AosJqlSubQueryTableValue> table_subquery 
%type <AosJqlStmtQueryValue> table_subquery 
%type <AosJqlLimitValue> opt_limit 
%type <AosJqlGroupByValue> stmt_group_by 
%type <AosJqlOrderByValue> stmt_order_by
%type <AosJqlWhereValue> opt_where 
%type <AosJqlHavingValue> opt_having 
%type <AosExprSearchCaseValue> search_case_expr      
%type <AosExprSimpleCaseValue> simple_case_expr when_case_list
%type <AosExprValue> else_expr schema_picker


//create database/table/index/dataset/schema statements
%type <AosJqlStmtDatabaseValue> stmt_create_database stmt_show_databases stmt_drop_database stmt_use_database
%type <AosJqlStmtTableValue> stmt_create_table
%type <AosJqlStmtSyncerValue> stmt_create_syncer
%type <AosJqlStmtIndexValue> stmt_create_index stmt_describe_index
%type <AosJqlStmtDatasetValue> stmt_create_dataset stmt_show_dataset stmt_describe_dataset stmt_drop_dataset
%type <AosJqlStmtDatascannerValue> stmt_create_datascanner stmt_show_datascanner stmt_drop_datascanner stmt_describe_datascanner
%type <AosJqlStmtSchemaValue> stmt_create_schema stmt_show_schema stmt_describe_schema stmt_drop_schema
%type <AosJqlRecordPicksValue> stmt_record_pick_list 
%type <AosJqlColumnValue> column_definition data_type
%type <AosJqlTableOptionValue> table_options
%type <AosJqlCreateDefineGroupValue> create_table_definition_group

//drop table	statements
%type <AosJqlStmtTableValue> stmt_drop_table
%type <AosJqlStmtSyncerValue> stmt_drop_syncer
%type <AosJqlTableVirtualFieldDefListValue> stmt_virtual_table_field_list 

// debug statements
%type <AosJqlStmtDebugValue> stmt_debug

// sequuence 
%type <AosJqlStmtSequenceValue> stmt_create_sequence 
%type <AosJQLSequenceInfoValue> sequence_info


//show tables
%type <AosJqlStmtTableValue> stmt_show_tables

//describe table
%type <AosJqlStmtTableValue> stmt_describe
// data record
%type <AosJqlStmtDataRecordValue> stmt_create_data_record stmt_drop_data_record stmt_show_data_record stmt_describe_data_record 
// data field
%type <AosJqlStmtDataFieldValue> stmt_create_data_field stmt_drop_data_field stmt_show_data_field stmt_describe_data_field
// data proc
%type <AosJqlStmtDataProcValue> stmt_create_data_proc stmt_drop_data_proc stmt_show_data_proc stmt_describe_data_proc

// data connector

%type <AosJqlStmtDataConnectorValue> stmt_create_data_connector stmt_drop_data_connector stmt_show_data_connector stmt_describe_data_connector
// Schedule
%type <AosJqlStmtScheduleValue> stmt_create_schedule stmt_run_schedule stmt_stop_schedule stmt_drop_schedule
// job
%type <AosJqlStmtJobValue> stmt_create_job stmt_run_job stmt_stop_job stmt_drop_job  stmt_show_job stmt_describe_job stmt_wait_job stmt_restart_job stmt_show_job_status stmt_show_job_log
%type <AosJqlStmtDocValue> stmt_create_doc stmt_show_doc
%type <AosJqlStmtTaskValue> stmt_create_task stmt_show_task  stmt_drop_task stmt_describe_task
%type <AosJqlStmtLoadDataValue>  load_data_stmt
%type <AosJqlStmtServiceValue> stmt_create_service stmt_stop_service stmt_drop_service stmt_show_service stmt_describe_service stmt_start_service
%type <AosJqlStmtUserMgrValue> stmt_create_user stmt_drop_user stmt_show_user stmt_describe_user stmt_alter_user
%type <AosJqlStmtRunScriptFileValue> stmt_run_script_file
%type <AosVecDirListValue> stmt_data_connector_file_list 
%type <AosJqlStmtStatisticsValue> stmt_create_statistics stmt_show_statistics stmt_describe_statistics
%type <AosJqlStmtStatValue> stmt_run_stat
%type <AosJqlStmtActorValue> stmt_create_actor stmt_run_actor
%type <AosMeasureValue> stmt_stat_measure 
%type <AosMeasuresValue> stmt_stat_measure_list 
%type <AosJqlStmtHBaseValue> stmt_hbase
%type <AosJqlOrderByFieldValue> stmt_order_field 
%type <AosJqlOrderByFieldListValue> stmt_order_field_list  
%type <AosJqlSelectValue> jql_select_stmt 
%type <AosJqlSelectIntoValue> stmt_select_into 
%type <AosStrListValue> record_format 
%type <AosExprListVecValue> stmt_dimensions_list stmt_index_records
%type <AosJqlStmtJoinSyncherValue> stmt_join_syncher 
%type <AosJqlStmtMapValues> stmt_create_map stmt_drop_map stmt_show_map stmt_describe_map stmt_list_map
%type <AosJqlStmtDictValues> stmt_create_dict stmt_drop_dict stmt_show_dict stmt_describe_dict
%type <AosJqlStmtDistributionMapValues> stmt_create_distributionmap stmt_drop_distributionmap stmt_show_distributionmap stmt_describe_distributionmap 
%type <AosJqlStmtVirtualFieldValue> stmt_create_virtual_field  stmt_drop_virtual_field  stmt_show_virtual_field stmt_describe_virtual_field  
%type <AosJqlStmtFieldExprValue> stmt_create_virtual_field_expr
%type <AosJqlStmtNickFieldValue> stmt_create_nick_field
%type <AosJqlStmtJimoLogicValue> stmt_create_jimo_logic stmt_drop_jimo_logic stmt_show_jimo_logic stmt_describe_jimo_logic
%type <AosJqlStmtQueryUnionValue>stmt_union query_union
%type <u32_value> stmt_max_threads 
%type <AosExprNameValuesValue> stmt_expr_name_values 
%type <AosJqlSelectFieldListVecValue> rollup_lists 


%type <AosJQLDataFieldTypeInfoPtr> data_field_data_type stmt_data_type

// new format
%type <AosJqlStmtGenericobjValue> stmt_create_genericobj 
%type <AosJqlGenericobjValuesValues> stmt_genericobj_value_list 
%type <AosJqlGenericobjValueValues> stmt_genericobj_value
%type <AosJqlStatementValue> statement 
%type <AosJqlStmtIFValue> stmt_execution_if 
%type <AosJqlStatementListValue> if_then_statements
%type <AosJqlStatementListValue> if_else_statements 
%type <AosJqlStmtAssignmentValue> stmt_assignment  

// for index index_hint
%type <AosExprListValue> index_hint 

%type <AosPJqlStmtExprValue> stmt_expr 



%type <strval> table_name

// For shawn : 2014/05/13
%type <AosJqlStmtCompleteIILValue> stmt_complete_iil

%{
	AosJQLParser			gAosJQLParser;
	//AosJoinStatement		gJoinStatement;
  extern int yylex(yy::Parser::semantic_type *yylval, 
                   yy::Parser::location_type *yylloc);
%}

// ************************************************
%start statements 



%%

statements:
	statement | statements statement
	;

statement:
	select_stmt{$$ = $1;}|
	insert_stmt{$$ = $1;}|
	update_stmt{$$ = $1;}|
	delete_stmt{$$ = $1;}|
	stmt_describe{$$ = $1;}|
	stmt_create_table{$$ = $1;}|
	stmt_create_syncer{$$ = $1;}|
	stmt_create_index{$$ = $1;}|
	stmt_show_tables{$$ = $1;}|
	stmt_create_dataset{$$ = $1;}|
	stmt_create_datascanner{$$ = $1;}|
	stmt_show_datascanner{$$ = $1;}|
	stmt_drop_datascanner{$$ = $1;}|
	stmt_describe_datascanner{$$ = $1;}|
	stmt_create_schema{$$ = $1;}|
	stmt_drop_schema{$$ = $1;}|
	stmt_drop_dataset{$$ = $1;}|
	show_iil_stmt{$$ = $1;}|
	stmt_drop_table{$$ = $1;}|
	stmt_drop_syncer{$$ = $1;}|
	stmt_debug{$$ = $1;}|
	stmt_show_schema{$$ = $1;}|
	stmt_show_dataset{$$ = $1;}|
	stmt_describe_schema{$$ = $1;}|
	stmt_describe_dataset{$$ = $1;}|
	stmt_describe_index{$$ = $1;}|
	stmt_list_index {$$ = $1;}|
	stmt_create_schedule{$$ = $1;}|
	stmt_run_schedule{$$ = $1;}|
	stmt_stop_schedule{$$ = $1;}|
	stmt_drop_schedule{$$ = $1;}|
	stmt_create_job{$$ = $1;}|
	stmt_wait_job{$$ = $1;}|
	stmt_restart_job{$$ = $1;}|
	stmt_run_job{$$ = $1;}|
	stmt_stop_job{$$ = $1;}|
	stmt_drop_job{$$ = $1;}|
	stmt_show_job{$$ = $1;}|
	stmt_describe_job{$$ = $1;}|
	stmt_show_job_status{$$ = $1;}|
	stmt_show_job_log{$$ = $1;}|
	stmt_create_task{$$ = $1;}|
	stmt_show_task{$$ = $1;}|
	stmt_drop_task{$$ = $1;}|
	stmt_describe_task{$$ = $1;}|
	stmt_create_data_record{$$ = $1;}|
	stmt_drop_data_record{$$ = $1;}|
	stmt_show_data_record{$$ = $1;}|
	stmt_describe_data_record{$$ = $1;}|
	stmt_create_data_field{$$ = $1;}|
	stmt_drop_data_field{$$ = $1;}|
	stmt_show_data_field{$$ = $1;}|
	stmt_describe_data_field{$$ = $1;}|
	stmt_create_data_proc{$$ = $1;}|
	stmt_drop_data_proc{$$ = $1;}|
	stmt_show_data_proc{$$ = $1;}|
	stmt_describe_data_proc{$$ = $1;}|
	stmt_create_data_connector{$$ = $1;}|
	stmt_drop_data_connector{$$ = $1;}|
	stmt_show_data_connector{$$ = $1;}|
	stmt_describe_data_connector{$$ = $1;}|
	drop_iil_stmt{$$ = $1;}|
	stmt_create_sequence{$$ = $1;}|
	stmt_create_database{$$ = $1;}|
	stmt_show_databases{$$ = $1;}|
	stmt_drop_database{$$ = $1;}|
	stmt_use_database{$$ = $1;}|
	load_data_stmt{$$ = $1;}|
	stmt_create_user{$$ = $1;}|
	stmt_alter_user{$$ = $1;}|
	stmt_drop_user{$$ = $1;}|
	stmt_show_user{$$ = $1;}|
	stmt_describe_user{$$ = $1;}|
	stmt_run_script_file{$$ = $1;}|
	stmt_create_statistics{$$ = $1;}|
	stmt_show_statistics{$$ = $1;}|
	stmt_describe_statistics{$$ = $1;}|
	stmt_complete_iil{$$ = $1;}|
	stmt_run_stat{$$ = $1;}|
	stmt_create_doc{$$ = $1;}|
	stmt_show_doc{$$ = $1;}|
	stmt_join_syncher{$$ = $1;}|
	stmt_create_dict{$$ = $1;}|
	stmt_drop_dict{$$ = $1;}|
	stmt_show_dict{$$ = $1;}|
	stmt_describe_dict{$$ = $1;}|
	stmt_create_map{$$ = $1;}|
	stmt_list_map{$$ = $1;}|
	stmt_show_map{$$ = $1;}|
	stmt_drop_map{$$ = $1;}|
	stmt_describe_map{$$ = $1;}|
	stmt_create_distributionmap{$$ = $1;}|
	stmt_show_distributionmap{$$ = $1;}|
	stmt_drop_distributionmap{$$ = $1;}|
	stmt_describe_distributionmap{$$ = $1;}|
	stmt_hbase{$$ = $1;}|
	stmt_create_service{$$ = $1;}|
	stmt_start_service{$$ = $1;}|
	stmt_stop_service{$$ = $1;}|
	stmt_show_service{$$ = $1;}|
	stmt_describe_service{$$ = $1;}|
	stmt_drop_service{$$ = $1;}|
	stmt_create_nick_field{$$ = $1;}|
	stmt_create_virtual_field_expr{$$ = $1;}|
	stmt_create_virtual_field{$$ = $1;}|
	stmt_drop_virtual_field{$$ = $1;}|
	stmt_show_virtual_field{$$ = $1;}|
	stmt_describe_virtual_field{$$ = $1;}|
	stmt_create_jimo_logic{$$ = $1;}|
	stmt_drop_jimo_logic{$$ = $1;}|
	stmt_show_jimo_logic{$$ = $1;}|
	stmt_describe_jimo_logic{$$ = $1;}|
	stmt_union{$$ = $1;}|
	stmt_expr{$$ = $1;}|
	stmt_create_actor{$$ = $1;}|
	stmt_run_actor{$$ = $1;}|
	stmt_create_genericobj{$$ = $1;}|
	stmt_select_into{$$ = $1;}|
	stmt_assignment{$$ = $1;}|
	stmt_execution_if{$$ = $1;}
	; 


stmt_create_actor:
	CREATE ACTOR JAVA STRING STRING STRING STRING STRING ';'
    {
		AosJqlStmtActor *actor= new AosJqlStmtActor;
	           actor->setName($4);
	           actor->setClassName($5);
	           actor->setPath($6);
	           actor->setType_language($7);
	           actor->setDescription($8);
			   gAosJQLParser.appendStatement(actor);
		       $$ = actor;
			   $$->setOp(JQLTypes::eCreate);
    }
    ;

stmt_run_actor:
	RUN ACTOR STRING par_key_names VALUES run_vals_list ';'
	{
		AosJqlStmtActor *actor = new AosJqlStmtActor;

		actor->setName($3);
		actor->setKeys($4);
		actor->setValues($6);
		gAosJQLParser.appendStatement(actor);

		$$ = actor;
		$$->setOp(JQLTypes::eRun);
	}
	;

par_key_names:
		opt_col_names
		{
			$$ = $1;
		}
		;

run_vals_list:
		insert_vals_list
		{
			$$ = $1;
		}
		;
stmt_assignment:
	USERVAR  data_field_data_type ';' 
	{
		OmnString name = $1;
		AosJqlStmtAssignment *stmt = new AosJqlStmtAssignment();
		stmt->setName(name);
		stmt->setType($2);
		$$ = stmt;
		$$->setOp(JQLTypes::eCreate);
		gAosJQLParser.appendStatement(stmt);
	}
	|
	USERVAR  data_field_data_type  ASSIGNMENT expr ';' 
	{
		OmnString name = $1;
		AosJqlStmtAssignment *stmt = new AosJqlStmtAssignment();
		stmt->setName(name);
		stmt->setType($2);
		stmt->setValue($4);
		$$ = stmt;
		$$->setOp(JQLTypes::eCreate);
		gAosJQLParser.appendStatement(stmt);
	}
	|
	USERVAR  ASSIGNMENT expr ';' 
	{
		OmnString name = $1;
		AosJqlStmtAssignment *stmt = new AosJqlStmtAssignment();
		stmt->setName(name);
		stmt->setValue($3);
		$$ = stmt;
		$$->setOp(JQLTypes::eCreate);
		gAosJQLParser.appendStatement(stmt);
	}
	|
	USERVAR  ASSIGNMENT SYSTEM_ARGS '(' INTNUM ')' ';'
	{
		OmnString name = $1;
		AosJqlStmtAssignment *stmt = new AosJqlStmtAssignment();
		stmt->setName(name);
		stmt->setIsParms(true);
		stmt->setParms($5);
		$$ = stmt;
		$$->setOp(JQLTypes::eCreate);
		gAosJQLParser.appendStatement(stmt);
	}
	|
	USERVAR  data_field_data_type ASSIGNMENT SYSTEM_ARGS '(' INTNUM ')' ';'
	{
		OmnString name = $1;
		AosJqlStmtAssignment *stmt = new AosJqlStmtAssignment();
		stmt->setName(name);
		stmt->setType($2);
		stmt->setIsParms(true);
		stmt->setParms($6);
		$$ = stmt;
		$$->setOp(JQLTypes::eCreate);
		gAosJQLParser.appendStatement(stmt);
	}

	;



		
stmt_create_data_connector:
	CREATE W_WORD DATA CONNECTOR W_WORD 
	SERVER ID INTNUM
	FILENAME STRING 
	CHAR CODING W_WORD 
	split_size_opts ';'
	{
		AosJqlStmtDataConnector *data_connector = new AosJqlStmtDataConnector;
		data_connector->setType($2);
		data_connector->setName($5);
		data_connector->setSvrId($8);
		data_connector->setFileName($10);
		data_connector->setCoding($13);
		data_connector->setSplitSize($14);
		gAosJQLParser.appendStatement(data_connector);
		$$ = data_connector;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	CREATE DIRECTORY DATA CONNECTOR W_WORD 
	'(' stmt_data_connector_file_list ')'
	CHAR CODING W_WORD  
	split_size_opts ';'
	{
		//	[FILENAME MATCH PATTERN pattern]
		AosJqlStmtDataConnector *data_connector = new AosJqlStmtDataConnector;
		data_connector->setType("DIRECTORY");
		data_connector->setName($5);
		data_connector->setDirList($7);
		data_connector->setCoding($11);
		data_connector->setSplitSize($12);
		gAosJQLParser.appendStatement(data_connector);
		$$ = data_connector;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	CREATE DATA CONNECTOR W_WORD '(' stmt_expr_name_values ')' ';'
	{
		AosJqlStmtDataConnector *stmt = new AosJqlStmtDataConnector;
		stmt->setName($4);
		stmt->setConfParms($6);
		gAosJQLParser.appendStatement(stmt);
		$$ = stmt;
		$$->setOp(JQLTypes::eCreate);
	};

split_size_opts:
	{
		$$ = NULL;
	}
	|
	SPLIT BY SIZE STRING
	{
		$$ = $4;	
	};
	

stmt_data_connector_file_list:
	 DIRECTORY STRING SERVER ID INTNUM 
	 {
	 	typedef AosJqlStmtDataConnector::AosDirList DirList; 
		vector<DirList*> *dls = new vector<DirList*>;
	 	DirList *dl = new DirList();  
		dl->mFileName = $2;
		dl->mSvrId = $5;
		$$ = dls;
		$$->push_back(dl);
	 }
	 |
	 stmt_data_connector_file_list ',' DIRECTORY STRING SERVER ID INTNUM 
	 {
	 	typedef AosJqlStmtDataConnector::AosDirList DirList; 
	 	DirList *dl = new DirList();  
		dl->mFileName = $4;
		dl->mSvrId = $7;
		$$->push_back(dl);
	 }
	 ;


stmt_drop_data_connector:
	DROP DATA CONNECTOR W_WORD ';'
	{
		AosJqlStmtDataConnector *data_connector = new AosJqlStmtDataConnector;
		data_connector->setName($4);
		gAosJQLParser.appendStatement(data_connector);
		$$ = data_connector;
		$$->setOp(JQLTypes::eDrop);
	}
	;

stmt_show_data_connector:
	SHOW DATA CONNECTORS ';'
	{
		AosJqlStmtDataConnector *data_connector = new AosJqlStmtDataConnector;
		gAosJQLParser.appendStatement(data_connector);
		$$ = data_connector;
		$$->setOp(JQLTypes::eShow);
	}
	;

stmt_describe_data_connector:
	DESCRIBE DATA CONNECTOR W_WORD ';'
	{
		AosJqlStmtDataConnector *data_connector = new AosJqlStmtDataConnector;
		data_connector->setName($4);
		gAosJQLParser.appendStatement(data_connector);
		$$ = data_connector;
		$$->setOp(JQLTypes::eDescribe);
	}
	;

stmt_create_data_field:
	CREATE DATA FIELD W_WORD TYPE STRING
	data_idfamily
	data_field_max_len 
	data_field_offset
	data_field_default_value ';'
	{
		AosJqlStmtDataField *data_field = new AosJqlStmtDataField;
		data_field->setName($4);
		data_field->setType($6);
		data_field->setIdfamily($7);
		data_field->setMaxLen($8);
		data_field->setOffset($9);
		data_field->setDefValue($10);
		gAosJQLParser.appendStatement(data_field);
		$$ = data_field;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	CREATE DATA FIELD W_WORD TYPE STRING
	VALUE FROM STRING
	INDEX W_WORD STRING
	iilmap_mode ASC 
	data_field_max_len ';'
	{
		AosJqlStmtDataField *data_field = new AosJqlStmtDataField;
		OmnString mode = $13;

		data_field->setName($4);
		data_field->setType($6);
		data_field->setFromFieldName($9);
		data_field->setIILName($12);
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
		$$ = data_field;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	CREATE DATA FIELD W_WORD TYPE STRING FORMATTER
	VALUE FROM STRING 
	FROM STRING TO STRING ';' 
	{
		AosJqlStmtDataField *data_field = new AosJqlStmtDataField;
		data_field->setName($4);
		data_field->setType($6);
		data_field->setFromFieldName($10);
		data_field->setFromName($12);
		data_field->setToName($14);
		gAosJQLParser.appendStatement(data_field);
		$$ = data_field;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	CREATE DATA FIELD W_WORD '(' stmt_expr_name_values ')' ';'
	{
		AosJqlStmtDataField* stmt = new AosJqlStmtDataField;
		stmt->setName($4);
		stmt->setConfParms($6);
		gAosJQLParser.appendStatement(stmt);
		$$ = stmt;
		$$->setOp(JQLTypes::eCreate);
	}
    ;

data_field_max_len:
	{	
		$$ = 0;
	}
	|
	MAX LENGTH INTNUM
	{
		$$ = $3;
	}
	;

data_field_default_value:
	{
		$$ = NULL;
	}
	|
	DEFAULT VALUE STRING 
	{
		$$ = $3; 
	}	
	;

data_idfamily:
	{
		$$ = NULL;
	}
	|
	IDFAMILY W_WORD 
	{
		$$ = $2;
	}
data_field_offset:
	{
		$$ = -1;
	}
	|
	OFFSET INTNUM
	{
		$$ = $2;
	}
	;

stmt_drop_data_field:
	DROP DATA FIELD W_WORD ';'
	{
		AosJqlStmtDataField *data_field = new AosJqlStmtDataField;
		data_field->setName($4);
		gAosJQLParser.appendStatement(data_field);
		$$ = data_field;
		$$->setOp(JQLTypes::eDrop);
	}
	;

stmt_show_data_field:
	SHOW DATA FIELDS ';'
	{
		AosJqlStmtDataField *data_field = new AosJqlStmtDataField;
		gAosJQLParser.appendStatement(data_field);
		$$ = data_field;
		$$->setOp(JQLTypes::eShow);
	}
	;

stmt_describe_data_field:
	DESCRIBE DATA FIELD W_WORD ';'
	{
		AosJqlStmtDataField *data_field = new AosJqlStmtDataField;
		data_field->setName($4);
		gAosJQLParser.appendStatement(data_field);
		$$ = data_field;
		$$->setOp(JQLTypes::eDescribe);
	}
	;

iilmap_mode:
	MODE COMBINE
	{
		$$ = OmnString("combine").getBuffer();
	}
	|
	MODE SINGLE
	{
		$$ = OmnString("single").getBuffer();
	}
	;

stmt_create_data_proc:
	CREATE DATA PROC STRING
	INPUT '(' column_list ')'
	OUTPUT STRING ';'
	{
		AosJqlStmtDataProc *data_proc = new AosJqlStmtDataProc;
		data_proc->setName($4);
		data_proc->setType("compose");
		data_proc->setInputs($7);
		data_proc->setOutput($10);

		gAosJQLParser.appendStatement(data_proc);
		$$ = data_proc;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	CREATE INDEX REDUCE TASK DATA PROC W_WORD 
	DATASET W_WORD 
	DATA RECORD W_WORD
	OPERATOR W_WORD 
	IILNAME W_WORD ';' 
	{
		AosJqlStmtDataProc *data_proc = new AosJqlStmtDataProc;
		data_proc->setName($7);
		data_proc->setType("index");
		data_proc->setDatasetName($9);
		data_proc->setDataRecordName($12);
		data_proc->setOperator($14);
		data_proc->setIILName($16);
		gAosJQLParser.appendStatement(data_proc);
		$$ = data_proc;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	CREATE DOC REDUCE TASK W_WORD 
	DATASET W_WORD 
	DATA RECORD W_WORD
	DOC TYPE W_WORD ';'
	{
		AosJqlStmtDataProc *data_proc = new AosJqlStmtDataProc;
		data_proc->setName($5);
		data_proc->setType("doc");
		data_proc->setDatasetName($7);
		data_proc->setDataRecordName($10);
		data_proc->setDocType($13);
		gAosJQLParser.appendStatement(data_proc);
		$$ = data_proc;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	CREATE DATA PROC W_WORD '(' stmt_expr_name_values ')' ';'
	{
		AosJqlStmtDataProc *stmt = new AosJqlStmtDataProc;
		stmt->setName($4);
		stmt->setConfParms($6);
		gAosJQLParser.appendStatement(stmt);
		$$ = stmt;
		$$->setOp(JQLTypes::eCreate);
	};


stmt_drop_data_proc:
	DROP DATA PROC W_WORD ';'
	{
		AosJqlStmtDataProc *data_proc = new AosJqlStmtDataProc;
		data_proc->setName($4);
		gAosJQLParser.appendStatement(data_proc);
		$$ = data_proc;
		$$->setOp(JQLTypes::eDrop);
	}
	;

stmt_show_data_proc:
	SHOW DATA PROCS ';' 
	{
		AosJqlStmtDataProc *data_proc = new AosJqlStmtDataProc;
		gAosJQLParser.appendStatement(data_proc);
		$$ = data_proc;
		$$->setOp(JQLTypes::eShow);
	}
	;

stmt_describe_data_proc:
	DESCRIBE DATA PROC W_WORD ';'  
	{
		AosJqlStmtDataProc *data_proc = new AosJqlStmtDataProc;
		data_proc->setName($4);
		gAosJQLParser.appendStatement(data_proc);
		$$ = data_proc;
		$$->setOp(JQLTypes::eDescribe);
	}
	;

stmt_create_data_record:
	CREATE DATA RECORD W_WORD '(' stmt_expr_name_values ')' ';'
	{
		AosJqlStmtDataRecord *stmt = new AosJqlStmtDataRecord;
		stmt->setName($4);
		stmt->setConfParms($6);
		gAosJQLParser.appendStatement(stmt);
		$$ = stmt;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	CREATE data_record_type_opts DATA RECORD W_WORD
	record_length
	FIELDS '(' data_field_list ')'
	record_delimiter
	field_delimiter
	text_qualidier ';'
	{
		AosJqlStmtDataRecord *data_record = new AosJqlStmtDataRecord;
		OmnString name = $5;
		data_record->setName(name);
		data_record->setType($2);
		data_record->setRecordLength($6);
		data_record->setFieldNames($9);
		data_record->setRecordDelimiter($11);
		data_record->setFieldDelimiter($12);
		data_record->setTextQualidier($13);
		gAosJQLParser.appendStatement(data_record);
		$$ = data_record;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	CREATE SCHEMA W_WORD 
	SOURCE TYPE data_record_type_opts 
	record_length
	trim_condition
	schema_picker
	FIELDS '(' data_field_list ')'
	record_delimiter
	field_delimiter
	text_qualidier ';'
	{
		AosJqlStmtDataRecord *data_record = new AosJqlStmtDataRecord;
		OmnString type = "schema";
		OmnString name = $3;
		data_record->setName(name);
		data_record->setRecordType(type);
		data_record->setType($6);
		data_record->setRecordLength($7);
		data_record->setTrimCondition($8);
		data_record->setScheamPicker($9);
		data_record->setFieldNames($12);
		data_record->setRecordDelimiter($14);
		data_record->setFieldDelimiter($15);
		data_record->setTextQualidier($16);
		gAosJQLParser.appendStatement(data_record);
		$$ = data_record;
		$$->setOp(JQLTypes::eCreate);
	};

data_record_type_opts:
	CSV
	{
		$$ = OmnString("csv").getBuffer();
	}
	|
	FIXED LENGTH
	{
		$$ = OmnString("fixbin").getBuffer();
	}
	|
	MULTIPLE RECORD
	{
		$$ = OmnString("multi").getBuffer();
	};

trim_condition:
	{
		$$ = NULL;
	}
	|
	TRIM STRING
	{
		$$ = $2;	
	}

data_field_list:
	data_field
	{
		$$ = new AosExprList; 
		$$->push_back($1);
	}
	|
	data_field_list ',' data_field
	{
		 $$->push_back($3);
	};

data_field:
	expr 
	{
		$$ = $1;
	}
	|
	expr stmt_infofield TYPE data_field_data_type
	field_formater
	data_idfamily
	data_field_max_len
	data_field_offset
	data_field_default_value
	field_mapping
	{
		AosJqlStmtDataFieldPtr data_field = OmnNew AosJqlStmtDataField; 
		//data_field->setName($1->getValue(0).toLower());
		data_field->setName($1->getValue(0));
		data_field->setIsInfoField($2);
		data_field->setType($4);
		data_field->setFormater($5);
		data_field->setIdfamily($6);
		data_field->setMaxLen($7);
		data_field->setOffset($8); 
		data_field->setDefValue($9);
		data_field->setMapping($10);
		gAosJQLParser.appendStatement(data_field); 
		data_field->setOp(JQLTypes::eCreate); 
		//$$ = new AosExprString(($1->getValue(0).toLower()).data());
		$$ = new AosExprString(($1->getValue(0)).data());
	}
	|
	expr stmt_infofield TYPE data_field_data_type
	stmt_data_type
	field_formater
	VALUE expr 
	data_idfamily
	data_field_max_len
	data_field_offset
	data_field_default_value
	{
		AosJqlStmtDataField *data_field = new AosJqlStmtDataField; 
		//data_field->setName($1->getValue(0).toLower());
		data_field->setName($1->getValue(0));
		data_field->setIsInfoField($2);
		data_field->setType($4);                                      
        data_field->setDataType($5);
		data_field->setFormater($6);
		if ($8)
		{
			data_field->setValue($8->dump());
		}
		data_field->setIdfamily($9);
		data_field->setMaxLen($10);
		data_field->setOffset($11); 
		data_field->setDefValue($12);
		gAosJQLParser.appendStatement(data_field); 
		data_field->setOp(JQLTypes::eCreate); 
		//$$ = new AosExprString(($1->getValue(0).toLower()).data());
		$$ = new AosExprString(($1->getValue(0)).data());
	}
	|
	expr stmt_infofield TYPE data_field_data_type
	field_formater
	data_idfamily
	data_field_max_len
	data_field_offset
	data_field_default_value
	NOT NULLX
	{
		AosJqlStmtDataField *data_field = new AosJqlStmtDataField; 
		//data_field->setName($1->getValue(0).toLower());
		data_field->setName($1->getValue(0));
		data_field->setIsInfoField($2);
		data_field->setType($4); 
		data_field->setFormater($5);
		data_field->setIdfamily($6);
		data_field->setMaxLen($7);
		data_field->setOffset($8); 
		data_field->setDefValue($9);
		data_field->setNOTNULL("true");
		gAosJQLParser.appendStatement(data_field); 
		data_field->setOp(JQLTypes::eCreate); 
		//$$ = new AosExprString(($1->getValue(0).toLower()).data());
		$$ = new AosExprString(($1->getValue(0)).data());
	};

field_formater:
	{
		$$ = "";
	}
	|
	FORMAT STRING
	{
		$$ = $2;
	};

data_field_data_type:
	W_WORD
	{
		$$ = new AosJQLDataFieldTypeInfo();
		$$->setType($1);
	}
	|
	DOUBLE
	{
		$$ = new AosJQLDataFieldTypeInfo();
		$$->setType("double");
	}
	|
	DATETIME	
	{
		$$ = new AosJQLDataFieldTypeInfo();
		$$->setType("datetime");
	}
	|
	VARCHAR '(' INTNUM ')'
	{
		$$ = new AosJQLDataFieldTypeInfo();
		$$->setType("varchar");
		$$->setValue($3, 0);
	}
	|
	VARCHAR2 '(' INTNUM ')'
	{
		$$ = new AosJQLDataFieldTypeInfo();
		$$->setType("varchar");
		$$->setValue($3, 0);
	}
	|
	CHAR '(' INTNUM ')'
	{
		$$ = new AosJQLDataFieldTypeInfo();
		$$->setType("varchar");
		$$->setValue($3, 0);
	}
	|
	TEXT
	{
		$$ = new AosJQLDataFieldTypeInfo();
		$$->setType("text");
	}
	|
	NUMBER '(' INTNUM ',' INTNUM ')'
	{
		$$ = new AosJQLDataFieldTypeInfo();
		$$->setType("number");
		$$->setValue($3, $5);
	}
	|
	NUMBER '(' INTNUM ')'
	{
		$$ = new AosJQLDataFieldTypeInfo();
		$$->setType("number");
		$$->setValue($3, 0);
	}
	|
	DECIMAL '(' INTNUM ',' INTNUM ')'
	{
		$$ = new AosJQLDataFieldTypeInfo();
		$$->setType("decimal");
		$$->setValue($3, $5);
	}
	|
	BIGINT '(' INTNUM ')'
	{
		$$ = new AosJQLDataFieldTypeInfo();
		$$->setType("bigint");
		$$->setValue($3, 0);
	} ;

record_length:
	{
		$$ = 0;
	}
	|
	RECORD LENGTH INTNUM
	{
		$$ = $3;
	}
	|
	SCHEMA LENGTH INTNUM
	{
		$$ = $3;
	};

schema_picker:
	{
		$$ = 0;
	}
	|
	PICKER expr
	{
		$$ = $2;
	};

record_delimiter:
	{
		$$ = NULL;
	}
	|
	RECORD DELIMITER STRING 
	{
		$$ = $3;
	};
	|
	SCHEMA DELIMITER STRING 
	{
		$$ = $3;
	};

field_mapping:
	{
		$$ = NULL;
	}
	|
	MAPPING '(' expr_list ')'
	{
		$$ = $3;	
	};

field_delimiter:
	{
		$$ = NULL;
	}
	|
	FIELD DELIMITER STRING 
	{
		/*$$ = $3->getValue(0).getBuffer();*/
		$$ = $3;
	};

text_qualidier:
	{
		$$ = NULL;
	}
	|
/*	TEXT QUALIFIER expr 
	{
		$$ = $3->getValue(0).getBuffer();
	};*/
	TEXT QUALIFIER STRING 
	{
		/*$$ = $3->getValue(0).getBuffer();*/
		$$ = $3;
	};

stmt_drop_data_record:
	DROP DATA RECORD W_WORD ';'
	{
		AosJqlStmtDataRecord *data_record = new AosJqlStmtDataRecord;
		data_record->setName($4);
		gAosJQLParser.appendStatement(data_record);
		$$ = data_record;
		$$->setOp(JQLTypes::eDrop);
	}
	|
	DROP SCHEMA W_WORD ';'
	{
		AosJqlStmtDataRecord *data_record = new AosJqlStmtDataRecord;
		gAosJQLParser.appendStatement(data_record);
		OmnString name = $3;
		data_record->setName(name);
		$$ = data_record;
		$$->setOp(JQLTypes::eDrop);
	};

stmt_show_data_record:
	SHOW DATA RECORDS ';'
	{
		AosJqlStmtDataRecord *data_record = new AosJqlStmtDataRecord;
		gAosJQLParser.appendStatement(data_record);
		$$ = data_record;
		$$->setOp(JQLTypes::eShow);
	}
	|
	SHOW SCHEMAS ';'
	{
		AosJqlStmtDataRecord *stmt = new AosJqlStmtDataRecord;
	 	gAosJQLParser.appendStatement(stmt);
		$$ = stmt;
		$$->setOp(JQLTypes::eShow);
	};

stmt_describe_data_record:
	DESCRIBE DATA RECORD W_WORD ';'
	{
		AosJqlStmtDataRecord *data_record = new AosJqlStmtDataRecord;
		data_record->setName($4);
		gAosJQLParser.appendStatement(data_record);
		$$ = data_record;
		$$->setOp(JQLTypes::eDescribe);
	};
stmt_create_database:
	CREATE DATABASE W_WORD ';'
	{
		AosJqlStmtDatabase* statement = new AosJqlStmtDatabase;
		
		statement->setName($3);

		gAosJQLParser.appendStatement(statement);

		$$ = statement;
		$$->setOp(JQLTypes::eCreate);
		//cout << "Create database:" << " name " << $3 << endl;
	};

stmt_drop_database:
	DROP DATABASE W_WORD ';'
	{
		AosJqlStmtDatabase* statement = new AosJqlStmtDatabase;
		statement->setName($3);
		gAosJQLParser.appendStatement(statement);
		$$ = statement;
		$$->setOp(JQLTypes::eDrop); 
	}
	;

stmt_show_databases:
	SHOW DATABASES ';'
	{
		AosJqlStmtDatabase* statement = new AosJqlStmtDatabase;

		gAosJQLParser.appendStatement(statement);
		$$ = statement;
		$$->setOp(JQLTypes::eShow); 
	}
	;

stmt_use_database:
	USE W_WORD ';'
	{
		AosJqlStmtDatabase* statement = new AosJqlStmtDatabase;

		gAosJQLParser.appendStatement(statement);
		statement->setName($2);
		$$ = statement;
		$$->setOp(JQLTypes::eUse); 
	}
	;
stmt_create_datascanner:
	CREATE DATA SCANNER W_WORD '(' stmt_expr_name_values ')' ';'
	{
		AosJqlStmtDatascanner* stmt = new AosJqlStmtDatascanner;
		stmt->setName($4);
		stmt->setConfParms($6);
		gAosJQLParser.appendStatement(stmt);
		$$ = stmt;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	CREATE DATA SCANNER W_WORD DATA CONNECTOR W_WORD with_order_opts';'
	{
	 	AosJqlStmtDatascanner* statement = new AosJqlStmtDatascanner;
		statement->setName($4);
		//OmnString conn_name = "";
		//conn_name << $4 << "_conn";
		//statement->setConnectorName(conn_name);
		statement->setConnectorName($7);

		if ($8) statement->setOrder();
		gAosJQLParser.appendStatement(statement);

		$$ = statement;
  		$$->setOp(JQLTypes::eCreate);
	}
	|
	CREATE DATA SCANNER W_WORD 
	DATA CONNECTOR TYPE W_WORD 
	SERVER ID INTNUM
	FILENAME STRING 
	CHAR CODING W_WORD 
	split_size_opts ';'
	{
		AosJqlStmtDatascanner* statement = new AosJqlStmtDatascanner;
		statement->setName($4);
		OmnString conn_name = "";
		conn_name << $4 << "_conn";
		statement->setConnectorName(conn_name);
		statement->setConnectorType($8);
		statement->setSvrId($11);	
		statement->setFileName($13);
		statement->setEncoding($16);
		statement->setSplitSize($17);

		statement->setOrder();
		gAosJQLParser.appendStatement(statement);

		$$ = statement;
		$$->setOp(JQLTypes::eCreate);
		//cout << "Create Datascanner: " << $4 << endl;

	}
	|
	CREATE DATA SCANNER W_WORD 
	DATA CONNECTOR TYPE DIRECTORY 
	'(' stmt_data_connector_file_list ')'
	CHAR CODING W_WORD 
	split_size_opts ';'
	{
		AosJqlStmtDatascanner* statement = new AosJqlStmtDatascanner;
		statement->setName($4);
		OmnString conn_name = "";
		conn_name << $4 << "_conn";
		statement->setConnectorName(conn_name);
		statement->setConnectorType("DIRECTORY");
		statement->setDirList($10);
		statement->setEncoding($14);
		statement->setSplitSize($15);

		//statement->setOrder();
		gAosJQLParser.appendStatement(statement);

		$$ = statement;
		$$->setOp(JQLTypes::eCreate);
	}
	;


with_order_opts:
	{
		$$ = false;
	}
	|
	WITH ORDER
	{
		$$ = true;	
	};
stmt_drop_datascanner:
	DROP DATA SCANNER W_WORD ';'
	{
		AosJqlStmtDatascanner *data_datascanner = new AosJqlStmtDatascanner;
		data_datascanner->setName($4);
		gAosJQLParser.appendStatement(data_datascanner);
		$$ = data_datascanner;
		$$->setOp(JQLTypes::eDrop);
	}
	;

stmt_show_datascanner:
	SHOW DATA SCANNERS ';'
	{
		AosJqlStmtDatascanner *data_datascanner = new AosJqlStmtDatascanner;
		gAosJQLParser.appendStatement(data_datascanner);
		$$ = data_datascanner;
		$$->setOp(JQLTypes::eShow);
	}
	;

stmt_describe_datascanner:
	DESCRIBE DATA SCANNER W_WORD ';'
	{
		AosJqlStmtDatascanner *data_datascanner = new AosJqlStmtDatascanner;
		data_datascanner->setName($4);
		gAosJQLParser.appendStatement(data_datascanner);
		$$ = data_datascanner;
		$$->setOp(JQLTypes::eDescribe);
	}
	;

stmt_create_dataset:
	CREATE DATASET W_WORD '(' stmt_expr_name_values ')' ';'
	{
		AosJqlStmtDataset* stmt = new AosJqlStmtDataset;
		stmt->setName($3);
		stmt->setConfParms($5);
		gAosJQLParser.appendStatement(stmt);
		$$ = stmt;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	CREATE DATASET W_WORD DATA SCANNER W_WORD PARSER W_WORD ';'
	{
		AosJqlStmtDataset* statement = new AosJqlStmtDataset;

		statement->setName($3);
		statement->setScanner($6);
		statement->setSchema($8);
		gAosJQLParser.appendStatement(statement);

		$$ = statement;
		$$->setOp(JQLTypes::eCreate);
		//cout << "Create dataset: " << $3 << endl;
	}
	|
	CREATE DATASET W_WORD DATA SCANNER W_WORD DATA SCHEMA W_WORD ';'
	{
		AosJqlStmtDataset* statement = new AosJqlStmtDataset;

		statement->setName($3);
		statement->setScanner($6);
		statement->setSchema($9);
		
		gAosJQLParser.appendStatement(statement);

		$$ = statement;
		$$->setOp(JQLTypes::eCreate);
		//cout << "Create dataset: " << $3 << endl;
	}
	|
	CREATE DATASET W_WORD ';'
	{
		AosJqlStmtDataset* statement = new AosJqlStmtDataset;
		statement->setName($3); 
		gAosJQLParser.appendStatement(statement);  
		                                           
		$$ = statement;                            
		$$->setOp(JQLTypes::eCreate);              
	};

stmt_show_dataset:
	SHOW DATASETS ';'
	{
		AosJqlStmtDataset* statement = new AosJqlStmtDataset;

		gAosJQLParser.appendStatement(statement);

		$$ = statement;
		$$->setOp(JQLTypes::eShow);
		//cout << "List all the Datasets" << endl;
	};

stmt_describe_dataset:
	DESCRIBE DATASET W_WORD ';'
	{
		AosJqlStmtDataset* statement = new AosJqlStmtDataset;

		gAosJQLParser.appendStatement(statement);

		$$ = statement;
		$$->setName($3);
		$$->setOp(JQLTypes::eDescribe);
		//cout << "Display the Dataset: " << $3 << endl;
	};

stmt_drop_dataset:
	DROP DATASET W_WORD ';'
	{
		AosJqlStmtDataset* statement = new AosJqlStmtDataset;
		statement->setName($3);

		gAosJQLParser.appendStatement(statement);
		$$ = statement;
		$$->setOp(JQLTypes::eDrop);
	}
	;

stmt_debug:
	DEBUG ON ';'
	{
		AosJqlStmtDebug* statement = new AosJqlStmtDebug;
		statement->setSwitch("on");
		gAosJQLParser.appendStatement(statement);
		$$ = statement;
	}
	|
	DEBUG OFF ';'
	{
		AosJqlStmtDebug* statement = new AosJqlStmtDebug;
		statement->setSwitch("off");
		gAosJQLParser.appendStatement(statement);
		$$ = statement;
	}
	;

delete_stmt: 
	DELETE 
		delete_opts
	FROM expr opt_where ';'
	{
		AosJqlStmtDelete *d = new AosJqlStmtDelete;
		d->table= $4;
		if ($5) d->opt_where = $5->getWhereExpr().getPtr();

		AosJqlStmtDeleteItem *del = new AosJqlStmtDeleteItem(d);	
		gAosJQLParser.appendStatement(del);
		$$ = del;
	}
	;

delete_opts:
	/* nil */ 
	{
		//cout << "============ " << endl;
	}
	| 
	delete_opts LOW_PRIORITY 
	{
		//cout << "============  delete_opts LOW_PRIORITY  " << endl;
	}
	|
	delete_opts QUICK 
	{
		//cout << "============  delete_opts QUICK " << endl;
	}
	|
	delete_opts IGNORE 
	{
		//cout << "============ delete_opts IGNORE " << endl;
	}
	;
/*
delete_list: 
	expr
	{
		//$$ = new AosExprList;
		//$$->push_back($1);
		//cout << "============   W_WORD opt_dot_star " << endl;
	}
	| 
	delete_list ',' expr 
	{
		//$$->push_back($3);
		//cout << "============  delete_list ',' W_WORD opt_dot_star " <<endl;
	}
	'*'
	{
	    //$ = new AosExprList;                       
		//$$->push_back(new AosExprAtomic("all"));
	}
	;
*/

stmt_create_dict:
	CREATE DICTIONARY W_WORD ON TABLE W_WORD
	KEY '(' expr_list ')'
	opt_where ';'
	{
		AosJqlStmtDictionary *stmt = new AosJqlStmtDictionary;
		stmt->setDictName($3);
		stmt->setTableName($6);
		stmt->setKeys($9);
		if ($11) stmt->setWhereCond($11->getWhereExpr().getPtr());
		stmt->setOp(JQLTypes::eCreate);               
		gAosJQLParser.appendStatement(stmt);   
		$$ = stmt;                             
	};

stmt_drop_dict:
	DROP DICTIONARY W_WORD ';'
	{
		AosJqlStmtDictionary *Dict = new AosJqlStmtDictionary;
		Dict->setDictName($3);
		gAosJQLParser.appendStatement(Dict);   
		$$ = Dict;                             
		$$->setOp(JQLTypes::eDrop);               
	};

stmt_show_dict:
	SHOW DICTIONARY ';'
	{
		AosJqlStmtDictionary *Dict = new AosJqlStmtDictionary;
		gAosJQLParser.appendStatement(Dict);   
		$$ = Dict;                             
		$$->setOp(JQLTypes::eShow);               
	};

stmt_describe_dict:
	DESCRIBE DICTIONARY W_WORD ';'
	{
		AosJqlStmtDictionary *Dict = new AosJqlStmtDictionary;
		Dict->setDictName($3);
		gAosJQLParser.appendStatement(Dict);   
		$$ = Dict;                             
		$$->setOp(JQLTypes::eDescribe);               
	};
stmt_create_distributionmap:
	CREATE DISTRIBUTIONMAP W_WORD ON W_WORD
	KEY '(' expr_list ')'
	NUMBER INTNUM
	SPLIT INTNUM
	TYPE STRING
	data_field_max_len 
	opt_where ';'
	{
		AosJqlStmtDistributionMap *map = new AosJqlStmtDistributionMap;
		map->setMapName($3);
		map->setTableName($5);
		map->setKeys($8);
		map->setMaxNums($11);
		map->setSpliteNum($13);
		map->setDataType($15);
		map->setMaxLen($16);
		if ($17)
			map->setWhereCond($17->getWhereExpr().getPtr());
		map->setOp(JQLTypes::eCreate);               
		gAosJQLParser.appendStatement(map);   
		$$ = map;                             
	};

stmt_drop_distributionmap:
	DROP DISTRIBUTIONMAP W_WORD ';'
	{
		AosJqlStmtDistributionMap *map = new AosJqlStmtDistributionMap;
		map->setMapName($3);
		gAosJQLParser.appendStatement(map);   
		$$ = map;                             
		$$->setOp(JQLTypes::eDrop);               
	}
	;

stmt_show_distributionmap:
	SHOW DISTRIBUTIONMAPS ';'
	{
		AosJqlStmtDistributionMap *map = new AosJqlStmtDistributionMap;
		gAosJQLParser.appendStatement(map);   
		$$ = map;                             
		$$->setOp(JQLTypes::eShow);               
	}
	;

stmt_describe_distributionmap:
	DESCRIBE DISTRIBUTIONMAP W_WORD ';'
	{
		AosJqlStmtDistributionMap *map = new AosJqlStmtDistributionMap;
		map->setMapName($3);
		gAosJQLParser.appendStatement(map);   
		$$ = map;                             
		$$->setOp(JQLTypes::eDescribe);               
	}
	;


stmt_create_doc:
	CREATE DOC STRING ';'
	{
		AosJqlStmtDoc *doc = new AosJqlStmtDoc;
		doc->setOp(JQLTypes::eCreate);               
		doc->setDocConf($3);
		gAosJQLParser.appendStatement(doc);   
		$$ = doc;                             
	};

stmt_show_doc:
	SHOW DOC INTNUM ';'
	{
		AosJqlStmtDoc *doc = new AosJqlStmtDoc;
		doc->setOp(JQLTypes::eShow);               
		doc->setDocDocid($3);
		gAosJQLParser.appendStatement(doc);   
		$$ = doc;                             
	}
	|
	SHOW DOC STRING ';'
	{
		AosJqlStmtDoc *doc = new AosJqlStmtDoc;
		doc->setOp(JQLTypes::eShow);               
		doc->setDocObjid($3);
		gAosJQLParser.appendStatement(doc);   
		$$ = doc;                             
	}
	;
stmt_expr:
	expr ';'
	{	
		AosExprObjPtr expr = $1;
		gAosJQLParser.setExpr(expr);
		$$ = new AosJqlStmtExpr(expr);
		gAosJQLParser.appendStatement($$);
	}
	;

expr: 
	W_WORD
	{
	    //cout << "this is name: " << $1 << endl;
		$$ = new AosExprFieldName($1);
		if ($1) free($1); 
	}
	|
	W_WORD XPATH STRING 
	{
	    //cout << "this is name: " << $1 << endl;
		$$ = new AosExprFieldName($1, $3);
		if ($1) free($1); 
		if ($3) free($3); 
	}
	|
	U8VAR
	{
		//cout << "this is U8VAR: " << $1 << endl;
		$$ = new AosExprString($1);
		if ($1) free($1);
	}
	|
	USERVAR
	{
		OmnString name = $1;
		AosExprUserVar *expr = new AosExprUserVar();
		expr->setName(name);
		$$ = expr;
		if ($1) free($1);
	}
	|
	W_WORD '.' W_WORD 
	{ 
		$$ = new AosExprMemberOpt("", $1, $3);
		if ($1) free($1);
		if ($3) free($3);
	}
    | 
	W_WORD '.' W_WORD '.' W_WORD
	{
		$$ = new AosExprMemberOpt($1, $3, $5);
		if ($1) free($1);
		if ($3) free($3);
		if ($5) free($5);
	}
	|
	STRING	'.' STRING 
	{ 
		$$ = new AosExprMemberOpt("", $1, $3);
		if ($1) free($1);
		if ($3) free($3);
	}
    | 
	STRING '.' STRING '.' STRING 
	{
		$$ = new AosExprMemberOpt($1, $3, $5);
		if ($1) free($1);
		if ($3) free($3);
		if ($5) free($5);
	}
	|

	STRING        
	{ 
	    //cout << "this is string" << endl;
		u32 len = strlen($1);
		if (len > 0)
		{
			char *newch = new char[len+1]; 
			strncpy(newch, $1 , len);
			newch[len] = '\0';
			$$ = new AosExprString(newch);
			delete [] newch;
		}
		else
		{
			$$ = new AosExprString("");
			//cout << "Parser string error!!: " << $1 << endl;
		}
		free($1); 
	}
    | 
	INTNUM	
	{ 
		int64_t e1 = $1;
		//cout << "Found number: " << $1 << endl;
		$$ = new AosExprNumber(e1);
	}
    | 
	DOUBLE_NUM     
	{ 
		$$ = new AosExprDouble($1);
	}
	|
	NULLX
	{
		$$ = new AosExprNull();
	}
	/*
    | 
	BOOL          
	{ 
		int64_t e1 = $1;
        $$ = new AosExprNumber(e1);
	}
	*/
    ;

expr: 
	expr '+' expr 
	{ 
		AosExprObj* e1 = $1;
		AosExprObj* e2 = $3;
		// AosValuePtr value = OmnNew AosValueInt64(0);

		//cout << "Found expression + " << endl;
        $$ = new AosExprArith(e1, AosExprArith::eAdd, e2);
		// if (!$$->getValue(0, 0, (AosValuePtr &)value)) 
		// {
		// 	//cout << "failed to get add result: " << $1 << " + " << $3 << endl;
		// } else 
		// {
		// 	//cout << value->toString() << endl;
		// }
	}
    | 
	expr '-' expr
	{ 
		AosExprObj* e1 = $1;
		AosExprObj* e2 = $3;
		// AosValuePtr value = OmnNew AosValueInt64(0);

		//cout << "Found expression -"  << endl;
        $$ = new AosExprArith(e1, AosExprArith::eMinus, e2);
		// if (!$$->getValue(0, 0, (AosValuePtr &)value)) 
		// {
		// 	//cout << "failed to get minus result: " << $1 << " + " << $3 << endl;
		// } else 
		// {
		// 	//cout << value->toString() << endl;
		// }
	}
    | 
	expr '*' expr
	{ 
		AosExprObj* e1 = $1;
		AosExprObj* e2 = $3;
		// AosValuePtr value = OmnNew AosValueInt64(0);

		//cout << "Found expression *" << endl;
        $$ = new AosExprArith(e1, AosExprArith::eMultiply, e2);
		// if (!$$->getValue(0, 0, (AosValuePtr &)value)) 
		// {
		// 	//cout << "failed to get multiply result: " << $1 << " + " << $3 << endl;
		// } else 
		// {
		// 	//cout << value->toString() << endl;
		// }
	}
    | 
	expr '/' expr
	{ 
		AosExprObj* e1 = $1;
		AosExprObj* e2 = $3;
		// AosValuePtr value = OmnNew AosValueInt64(0);

		//cout << "Found expression /" << endl;
        $$ = new AosExprArith(e1, AosExprArith::eDivide, e2);
		// if (!$$->getValue(0, 0, (AosValuePtr &)value)) 
		// {
		// 	//cout << "failed to get divide result: " << $1 << " + " << $3 << endl;
		// } else 
		// {
		// 	//cout << value->toString() << endl;
		// }
	}
    | 
	expr '%' expr
	{ 
		AosExprObj* e1 = $1;
		AosExprObj* e2 = $3;
		// AosValuePtr value = OmnNew AosValueU64(0);

		//cout << "Found expression %" << endl;
        $$ = new AosExprArith(e1, AosExprArith::eMod, e2);
		// if (!$$->getValue(0, 0, (AosValuePtr &)value)) 
		// {
		// 	//cout << "failed to get mod result: " << $1 << " % " << $3 << endl;
		// } else 
		// {
		// 	//cout << value->toString() << endl;
		// }
	}
    | 
	'-' expr %prec UMINUS 
	{ 
		//$$ = new AosExprUnaryMinus($2);
	}
    | 
	expr LOGIC expr
	{ 
		AosExprObj *e1 = $1;
		AosExprObj *e2 = $3;
		//$$ = new AosExprLogic(e1, (AosExprLogic::Operator)$2, e2);
       // $$ = new AosExprArith(e1, AosExprArith::eConcat, e2);

		// Young, 2014/11/05
		AosExpr* logicExpr = new AosExprLogic(e1, (AosExprLogic::Operator)$2, e2);
		if (e1->getType() == AosExprType::eComparison || e2->getType() == AosExprType::eComparison)
		{
			logicExpr->setExpr(e1, 1);
			logicExpr->setExpr(e2, 2);
		}
		$$ = logicExpr;
	}
    | 
	expr LIKE expr
	{ 
		AosExprObj *e1 = $1;
		AosExprObj *e2 = $3;
		$$ = new AosExprLike(e1, e2, false);
	}
    | 
	expr NOT LIKE expr
	{ 
		AosExprObj *e1 = $1;
		AosExprObj *e2 = $4;
		$$ = new AosExprLike(e1, e2, true);
	}
    | 
	expr XOR expr
	{ 
        //$$ = new AosExprXor($1, $3);
	}
	|
	expr IN '(' expr_list ')'
	{
		$$ = new AosExprIn($1, $4);
	}
    | 
	expr NOT IN '(' expr_list ')'
	{
		$$ = new AosExprNotIn($1, $5);
	}
	|
	expr COMPARISON expr
	{ 
		AosExprObj *e1 = $1;
		AosExprObj *e2 = $3;
		AosExprType::E expr_type = e1->getType();
		if (expr_type == AosExprType::eComparison)
		{
			AosExprBinary* expr = dynamic_cast<AosExprBinary*>(e1);
			AosExprObjPtr e3 = expr->getRHS();

			AosExpr *rhs= new AosExprComparison(e3, (AosExprComparison::Operator)$2, e2);     
			$$ = new AosExprLogic(e1, AosExprLogic::eAnd, rhs);
		}
		else
		{
			AosExpr *expr= new AosExprComparison(e1, (AosExprComparison::Operator)$2, e2);
			expr->setExpr(e1, 1);
			expr->setExpr(e2, 2);
			$$ = expr;
		}
	}
	|
	expr BETWEEN expr AND expr
	{
		AosExpr *rhs= new AosExprComparison($1, AosExprComparison::eLargerEqual, $3);
		AosExpr *lhs= new AosExprComparison($1, AosExprComparison::eLessEqual, $5);     
		$$ = new AosExprLogic(rhs, AosExprLogic::eAnd, lhs);
	}
	|
	expr '|' expr
	{ 
		AosExprObj* e1 = $1;
		AosExprObj* e2 = $3;
		// AosValuePtr value = OmnNew AosValueU32(0);

		//cout << "Found expression |" << endl;
        $$ = new AosExprBitArith(e1, AosExprBitArith::eBitOr, e2);
		// if (!$$->getValue(0, 0, (AosValuePtr &)value)) 
		// {
		// 	//cout << "failed to get bitor result: " << $1 << " | " << $3 << endl;
		// } else 
		// {
		// 	//cout << value->toString() << endl;
		// }
	}
    | 
	expr '&' expr
	{ 
		AosExprObj* e1 = $1;
		AosExprObj* e2 = $3;
		// AosValuePtr value = OmnNew AosValueU32(0);

		//cout << "Found expression &" << endl;
        $$ = new AosExprBitArith(e1, AosExprBitArith::eBitAnd, e2);
		// if (!$$->getValue(0, 0, (AosValuePtr &)value)) 
		// {
		// 	//cout << "failed to get bitand result: " << $1 << " & " << $3 << endl;
		// } else 
		// {
		// 	//cout << value->toString() << endl;
		// }
	}
    | 
	expr '^' expr 
	{ 
		AosExprObj* e1 = $1;
		AosExprObj* e2 = $3;
		// AosValuePtr value = OmnNew AosValueU32(0);

		//cout << "Found expression ^" << endl;
        $$ = new AosExprBitArith(e1, AosExprBitArith::eBitXor, e2);
		// if (!$$->getValue(0, 0, (AosValuePtr &)value)) 
		// {
		// 	//cout << "failed to get bitand result: " << $1 << " ^ " << $3 << endl;
		// } else 
		// {
		// 	//cout << value->toString() << endl;
		// }
	}
	|
	expr CONTAIN STRING
	{
		$$ = new AosExprContain($1, $3);
	}
    | 
	expr SHIFT expr
	{ 
        // $$ = new AosSQLExpr;
		// $$->setOperType( $2==1 ? eOper_LeftShift : eOper_RightShift);
		// $$->setExpr($1, 1);
		// $$->setExpr($3, 2);
	}
    | 
	NOT expr
	{ 
        // $$ = new AosSQLExpr;
		// $$->setOperType(eOper_Not);
		// $$->setExpr($2, 1);
	}
    | 
	'!' expr
	{ 
        // $$ = new AosSQLExpr;
		// $$->setOperType(eOper_Not);
		// $$->setExpr($2, 1);
	}
    | 
	USERVAR ASSIGN expr
	{ 
        // $$ = new AosSQLExpr;
		// $$->setOperType(eOper_Assign);
		// $$->setValue($1);
		// $$->setExpr($3, 1);
	}
	|
	STRING ':' expr
	{
		$$ = new AosExprNameValue($1, $3);
	}
	|
	LEFT '(' expr_list ')'
	{
		$$ = new AosExprGenFunc("left", $3);
	}
	|
	RIGHT '(' expr_list ')'
	{
		$$ = new AosExprGenFunc("right", $3);
	}
	|
	W_WORD '(' ')'
	{
		AosExprList* expr_list = new AosExprList;
		$$ = new AosExprGenFunc($1, expr_list);
	}
	|
	W_WORD '(' '*' ')'
	{
		AosExprList* expr_list = new AosExprList;
		expr_list->push_back(new AosExprFieldName("*"));
		$$ = new AosExprGenFunc($1, expr_list);
	}
	|
	W_WORD '(' expr_list ')'
	{
		OmnString fname = $1;
		if (fname.toLower() == "avg" && $3->size() == 1)
		{
			AosExprObjPtr lhs = OmnNew AosExprGenFunc("sum", (*$3)[0]);
			AosExprObjPtr lhs1 = OmnNew AosExprGenFunc("to_double", lhs);
			AosExprObjPtr rhs = OmnNew AosExprGenFunc("count", (*$3)[0]);
			$$ = new AosExprArith(lhs1, AosExprArith::eDivide, rhs);
		}
		else
		{
			$$ = new AosExprGenFunc($1, $3);
		}
	}
	|
	W_WORD '(' DISTINCT expr_list ')'
	{
		OmnString fname = $1;
		AosValueRslt v;

		if (fname.toLower() == "count")
		{
			if ($4->size() == 1)
			{
				AosExprObjPtr expr = (*$4)[0];
				bool rslt = expr->getValue(0, 0, v);
				aos_assert_r(rslt, false);
				OmnString rname = v.getStr();
				if (rname == "*") 
				{
					(*$4)[0] = new AosExprFieldName("_rec_count");
				}
			}
			
			fname = "dist_count";
		}
		$$ = new AosExprGenFunc(fname, $4);
	}
	|
	W_WORD '(' ACCUMULATE expr_list ')'
	{
		OmnString fname = $1;
		OmnString newFname = "accu_";
		AosValueRslt v;

		fname = fname.toLower();
		newFname << fname;
		aos_assert_r(fname == "count" ||
					 fname == "sum" ||
					 fname == "max" ||
					 fname == "min",  false);

		aos_assert_r($4->size() == 1, false);
		$$ = new AosExprGenFunc(newFname, $4);
	}
	|
	W_WORD '(' ACCUMULATE '*' ')'
	{
		OmnString fname = $1;
		OmnString newFname = "accu_";
		AosValueRslt v;

		fname = fname.toLower();
		newFname << fname;
		aos_assert_r(fname == "count",  false);

		AosExprList* exprList = new AosExprList;
		AosExprFieldName* expr = new AosExprFieldName("_rec_count");
		exprList->push_back(expr);

		$$ = new AosExprGenFunc(newFname, exprList);
	}
	|
	MAX '(' expr_list ')'
	{
		//max is an exception since there is 
		//a token "MAX", therefore we need to
		//add one explicit line for it
		$$ = new AosExprGenFunc("max", $3);
	}
	|
	'(' expr ')' 
	{
		//cout << "Found expression" << endl;
		$$ = new AosExprBrackets($2);
	}
	|
	'(' expr_list ')'
	{
		$$ = new AosExprBrackets(*($2));
	}
	|
	LEFT_BRACKET expr_list RIGHT_BRACKET
	{
		$$ = new AosExprBrackets(*($2));
	}
	|
	CASE search_case_expr else_expr END
	{
		//cout << "Found case1............" << endl;
		$$ = new AosExprCase($2, 0, $3);
	}                                          
	|
	CASE simple_case_expr else_expr END
	{
		//cout << "Found case2............" << endl;
		$$ = new AosExprCase(0, $2, $3);
	}
	|
	expr IS NOT NULLX
	{
		//cout << "Found is not null ..." << endl;
		$$ = new AosExprIsNotNull($1);
	}
	|
	expr IS NULLX
	{
		//cout << "Found is not null ..." << endl;
		$$ = new AosExprIsNull($1);
	}
	;    

simple_case_expr:
	 expr WHEN expr THEN expr when_case_list
	 {
		AosExprSimpleCase::AosJqlWhenCaseExpr *aa = new AosExprSimpleCase::AosJqlWhenCaseExpr($3, $5);
		$6->appendWhenCase(aa);
	 	//$6->appendPair($3, $5);
		$6->setCaseExpr($1);
		$$ = $6;
	 };

when_case_list:
	 {
	 	$$ = new AosExprSimpleCase();
	 }
	 |
	 when_case_list WHEN expr THEN expr
	 {
	 	//$1->appendPair($3, $5);
		AosExprSimpleCase::AosJqlWhenCaseExpr *aa = new AosExprSimpleCase::AosJqlWhenCaseExpr($3, $5);
		$1->appendWhenCase(aa);
	 };

search_case_expr:
	 when_case_list
	 {
		 $$ = new AosExprSearchCase($1);
	 };

else_expr:
	 /* nill */
	 {
		 $$ = 0;
	 }
	 |
	 ELSE expr
	 {
		 $$ = $2;
	 };

expr_list:
	expr
	{
		$$ = new AosExprList;
		$$->push_back($1);
	}
	|
	expr_list ',' expr
	{
		$$ = $1;
		$$->push_back($3);
	}
	;

stmt_expr_name_values:
	{
		typedef vector<AosExprNameValuePtr> AosExprNameValues;
		$$ = new AosExprNameValues;
	}
	|
	STRING ':' expr
	{
		typedef vector<AosExprNameValuePtr> AosExprNameValues;
		AosExprNameValue *expr = new AosExprNameValue($1, $3);
		$$ = new AosExprNameValues;
		$$->push_back(expr);
	}
	|
	stmt_expr_name_values ','  STRING ':' expr 
	{
		$$ = $1;
		AosExprNameValue *expr = new AosExprNameValue($3, $5);
		$$->push_back(expr);
	}
	;

opt_as_alias: 
  	AS W_WORD 
  	{ 
		$$ = new AosExprFieldName($2);  
		if ($2) free($2);            
  	}
	|
	AS STRING
	{
		$$ = new AosExprString($2);  
		if ($2) free($2);            
	}
	|
	STRING
	{
		$$ = new AosExprString($1);  
		if ($1) free($1);            
	}
	|
	W_WORD              
  	{ 
		$$ = new AosExprFieldName($1);  
		if ($1) free($1);            
  	}
	| 
	/* nil */
  	{
		$$ = NULL;
  	}
	;

opt_where:         
    /* nil */      
    {              
        $$ = 0;    
    }              
    |              
    WHERE expr     
    {              
		$$ = new AosJqlWhere();
		$$->setWhereExpr($2);
    }
	;
stmt_create_genericobj:
	CREATE expr expr 
	'(' stmt_genericobj_value_list ')' ';'
	{
		AosJqlStmtGenericobj * statement = new AosJqlStmtGenericobj;
		statement->setType($2);
		statement->setName($3);
//		statement->setGenericobjValues($5);
		gAosJQLParser.appendStatement(statement);
		$$ = statement;
		$$->setOp(JQLTypes::eCreate);
	}
	;

stmt_genericobj_value_list:
	{
		$$ = NULL;
	}
	|
	stmt_genericobj_value
	{
		$$ = new vector<AosGenericValueObjPtr>();
		$$->push_back($1);
	}
	|
	stmt_genericobj_value_list ',' stmt_genericobj_value
	{
		$$ = $1;
		$$->push_back($3);
	}
	;

stmt_genericobj_value:
	{
		$$ = NULL;
	}
	|
	expr
	{
		$$ = new AosGenericValueObj();
		$$->mValue = $1;
	}
	|
	expr ':' '[' expr_list ']'
	{
		$$ = new AosGenericValueObj();
		$$->mName = $1;
		$$->mArrayValues = *$4;
	}
	;


stmt_execution_if:
	IF if_not expr
	if_then_statements
	if_else_statements
	END IF ';'
	{
		AosJqlStmtIF *stmt = new AosJqlStmtIF;
		stmt->setNot($2);
		stmt->setCond($3);
		stmt->setThen($4);
		stmt->setElse($5);
		gAosJQLParser.appendStatement(stmt);
		$$ = stmt;
		$$->setOp(JQLTypes::eRun);
	}
    ;

if_not:
	{
		$$ = false;
	}
	|
	NOT
	{
		$$ = true;
	}

if_then_statements:
	THEN statement 
	{
		typedef vector<AosJqlStatementPtr> AosJqlStatementList;
		$$ = new AosJqlStatementList; 
		$$->push_back($2);
		gAosJQLParser.finishParse();
	}
	|
	if_then_statements statement
	{
		$$ = $1;
		$$->push_back($2);
		gAosJQLParser.finishParse();
	}
	;

if_else_statements:
	{
		$$ = NULL;
	}
	|
	ELSE  statement
	{
		typedef vector<AosJqlStatementPtr> AosJqlStatementList;
		$$ = new AosJqlStatementList; 
		$$->push_back($2);
		gAosJQLParser.finishParse();
	}
	|
	if_else_statements statement
	{
		$$ = $1;
		$$->push_back($2);
		gAosJQLParser.finishParse();
	}
	;

stmt_create_index:
	CREATE INDEX W_WORD ON TABLE W_WORD 
	'(' expr_list ')'
	opt_where ';'
	{
		AosJqlStmtIndex* statement = new AosJqlStmtIndex;
		statement->setIndexName($3);
		statement->setTableName($6);
		//statement->setFieldIndexColumns($8);
		statement->setWhereConds($10);
		// statement->setFieldIndexColumns($9);
		gAosJQLParser.appendStatement(statement);

		AosJqlStmtIndex::AosFieldIdxCols *field_list = new AosJqlStmtIndex::AosFieldIdxCols; 
		field_list->mFieldList = $8;
		field_list->mIndexName= "";
		field_list->mType = "";

		statement->setFieldList(field_list);

		$$ = statement;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	CREATE INDEX W_WORD ON TABLE W_WORD 
	WITH MULTIPLE RECORDS 
	'{' stmt_index_records	'}' ';'
	{
		AosJqlStmtIndex* statement = new AosJqlStmtIndex;
		statement->setIndexName($3);
		statement->setTableName($6);
		statement->setIndexRecords($11);
		gAosJQLParser.appendStatement(statement);

		$$ = statement;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	CREATE INDEX W_WORD ON W_WORD opt_where ';'
	{
		AosJqlStmtIndex* stmt = new AosJqlStmtIndex;
		stmt->setIndexName($3);
		stmt->setTableName($5);
		stmt->setWhereConds($6);
		gAosJQLParser.appendStatement(stmt);
		$$ = stmt;
		$$->setOp(JQLTypes::eCreate);
	};

stmt_describe_index:
	DESCRIBE INDEX W_WORD ';'
	{
		AosJqlStmtIndex* statement = new AosJqlStmtIndex;
		gAosJQLParser.appendStatement(statement);
		$$ = statement;
		$$->setIndexName($3);
		$$->setOp(JQLTypes::eDescribe);

		cout << "Describe index: " << $3 << endl;
	}
	;

stmt_list_index:
	LIST INDEX expr stmt_list_key_list opt_where opt_limit';'
	{
		AosJqlStmtIndex* statement = new AosJqlStmtIndex;
		gAosJQLParser.appendStatement(statement);
		$$ = statement;
		$$->setIndexName($3->getValue(0));
		$$->setKeysExpr($4);
		$$->setWhereConds($5);
		$$->setLimit($6);
		$$->setOp(JQLTypes::eList);
	}
	;

stmt_list_key_list:
	{
		$$ = NULL;
	}
	|
	'(' expr_list ')'
	{
		$$ = $2;
	}
	;

show_iil_stmt:
   	SHOW INDEXES ';'
    {
		AosJqlStmtIndex *show_iil = new AosJqlStmtIndex();	
		gAosJQLParser.appendStatement(show_iil);
		$$ = show_iil;
		$$->setOp(JQLTypes::eShow); 
    }
    ;

drop_iil_stmt:
	DROP INDEX W_WORD ';'
	{
		AosJqlStmtIndex *drop_iil = new AosJqlStmtIndex();
		drop_iil->setIndexName($3);
		gAosJQLParser.appendStatement(drop_iil);
		$$ = drop_iil;
		$$->setOp(JQLTypes::eDrop); 
		free($3);
	}
	;

stmt_index_records:
    '(' expr_list ')'	
	{
		$$ = new vector<AosExprList *>;
		$$->push_back($2);
	}
	|
	stmt_index_records ',' '(' expr_list ')'
	{
		$$ = $1;
		$$->push_back($4);
	}
	;

insert_stmt: 
	INSERT insert_opts opt_into table_name opt_col_names VALUES insert_vals_list ';'
	{
		AosJqlStmtInsert* insert = new AosJqlStmtInsert;
		insert->table_name = $4;
		insert->opt_col_names = $5;
		insert->insert_vals_list = $7;
		$$ = new  AosJqlStmtInsertItem(insert);
		gAosJQLParser.appendStatement($$);
	}
	;

insert_opts: 
	/* nil */ 
	{  
	}
	| 
	insert_opts LOW_PRIORITY 
	{
	}
	| 
	insert_opts DELAYED 
	{ 
	}
	|
	insert_opts HIGH_PRIORITY 
	{ 
	}
	|
	insert_opts IGNORE 
	{ 
	}
	;

opt_into: 
	INTO 
	{
	}
	| 
	/* nil */
	{
	}
	;

opt_col_names: 
	/* nil */ 
	{
		$$ = new AosExprList;
	}
	| 
	'(' column_list ')' 
	{ 
		$$ = $2; 
	}
	;

column_list: 
	expr	
	{
		$$ = new AosExprList;
		$$->push_back($1);
	}
	|
	column_list ',' expr 
	{ 
		$$->push_back($3);
	}
	;

insert_vals_list: 
	'(' insert_vals ')' 
	{ 
		$$ = $2;
	}
	;

insert_vals:
	expr 
	{ 
		$$ = new AosExprList;
		$$->push_back($1); 
	}
	| 
	DEFAULT 
	{ 
	}
	| 
	insert_vals ',' expr 
	{ 
		$$->push_back($3); 
	}
	| 
	insert_vals ',' DEFAULT 
	{ 
	}
	;
stmt_create_jimo_logic:
	CREATE JIMOLOGIC W_WORD ON W_WORD 
	ENDPOINT W_WORD
	TIME W_WORD
	CHECKPOINT W_WORD
	USING MATRIX W_WORD
	RESULTS W_WORD ';'
	{
		AosJqlStmtJimoLogic *logic = new AosJqlStmtJimoLogic;
		logic->setName($3);
		logic->setTableName($5);
		logic->setEndPoint($7);
		logic->setTime($9);
		logic->setCheckPoint($11);
		logic->setMatrixName($14);
		logic->setResultsName($16);
		gAosJQLParser.appendStatement(logic);
		$$ = logic;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	CREATE JIMOLOGIC W_WORD ON W_WORD 
	ENDPOINT W_WORD
	TIME W_WORD
	CHECKPOINT W_WORD
	USING MATRIX W_WORD
	USING IILNAME W_WORD
	RESULTS W_WORD ';'
	{
		AosJqlStmtJimoLogic *logic = new AosJqlStmtJimoLogic;
		logic->setName($3);
		logic->setTableName($5);
		logic->setEndPoint($7);
		logic->setTime($9);
		logic->setCheckPoint($11);
		logic->setMatrixName($14);
		logic->setSecondName($17);
		logic->setResultsName($19);
		gAosJQLParser.appendStatement(logic);
		$$ = logic;
		$$->setOp(JQLTypes::eCreate);
	}
    ;

stmt_drop_jimo_logic:
	DROP JIMOLOGIC W_WORD ';'
	{
		AosJqlStmtJimoLogic *logic = new AosJqlStmtJimoLogic;
		logic->setName($3);
		gAosJQLParser.appendStatement(logic);
		$$ = logic;
		$$->setOp(JQLTypes::eDrop);
	}
	;

stmt_show_jimo_logic:
	SHOW JIMOLOGIC ';'
	{
		AosJqlStmtJimoLogic *logic = new AosJqlStmtJimoLogic;
		gAosJQLParser.appendStatement(logic);
		$$ = logic;
		$$->setOp(JQLTypes::eShow);
	}
	;

stmt_describe_jimo_logic:
	DESCRIBE JIMOLOGIC W_WORD ';'
	{
		AosJqlStmtJimoLogic *logic = new AosJqlStmtJimoLogic;
		logic->setName($3);
		gAosJQLParser.appendStatement(logic);
		$$ = logic;
		$$->setOp(JQLTypes::eDescribe);
	}
	;
stmt_create_job:
	CREATE JOB W_WORD '(' stmt_expr_name_values ')' ';'
	{
		AosJqlStmtJob *stmt = new AosJqlStmtJob;
		stmt->setJobName($3);
		stmt->setConfParms($5);
		gAosJQLParser.appendStatement(stmt);
		$$ = stmt;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	CREATE JOB W_WORD 
	USING TASKS '(' column_list ')' ';'
    {
		AosJqlStmtJob *job = new AosJqlStmtJob;
		job->setJobName($3);
		job->setTaskNames($7);
		gAosJQLParser.appendStatement(job);   
		$$ = job;                             
		$$->setOp(JQLTypes::eCreate);               
	}
    ;

//stmt_run_job:
//	RUN JOB W_WORD ';'
//	{
//		AosJqlStmtJob *job = new AosJqlStmtJob;
//		job->setJobName($3);
//		gAosJQLParser.appendStatement(job);   
//		$$ = job;                             
//		$$->setOp(JQLTypes::eRun);               
//	}
//	;

stmt_run_job:
	RUN JOB W_WORD FILENAME STRING ';'
	{
		AosJqlStmtJob *job = new AosJqlStmtJob;
		job->setJobName($3);
		job->setJobFileName($5);
		gAosJQLParser.appendStatement(job);   
		$$ = job;                             
		$$->setOp(JQLTypes::eRun);               
	}
	;

stmt_stop_job:
	STOP JOB W_WORD ';'
	{
		AosJqlStmtJob *job = new AosJqlStmtJob;
		job->setJobName($3);
		gAosJQLParser.appendStatement(job);   
		$$ = job;                             
		$$->setOp(JQLTypes::eStop);               
	}
	;

stmt_drop_job:
	DROP JOB W_WORD ';'
	{
		AosJqlStmtJob *job = new AosJqlStmtJob;
		job->setJobName($3);
		gAosJQLParser.appendStatement(job);   
		$$ = job;                             
		$$->setOp(JQLTypes::eDrop);               
	}
	;

stmt_show_job:
	SHOW JOBS ';'
	{
		AosJqlStmtJob *job = new AosJqlStmtJob;
		gAosJQLParser.appendStatement(job);   
		$$ = job;                             
		$$->setOp(JQLTypes::eShow);               
	}
	;

stmt_describe_job:
	DESCRIBE JOB W_WORD ';'
	{
		AosJqlStmtJob *job = new AosJqlStmtJob;
		job->setJobName($3);
		gAosJQLParser.appendStatement(job);   
		$$ = job;                             
		$$->setOp(JQLTypes::eDescribe);               
	}
	;

stmt_show_job_status:
	SHOW JOB STATUS opt_limit';'
	{
		AosJqlStmtJob *job = new AosJqlStmtJob;
		/*job->setJobName($3);*/
		job->setLimit($4);
		job->setOp(JQLTypes::eShowStatus);               
		gAosJQLParser.appendStatement(job);   
		$$ = job; 
	};

stmt_show_job_log:
	SHOW JOB LOG W_WORD opt_limit';'
	{
		AosJqlStmtJob *job = new AosJqlStmtJob;
		job->setLimit($5);
		job->setIsLog(true);
		job->setJobName($4);
		job->setOp(JQLTypes::eShow);               
		gAosJQLParser.appendStatement(job);   
		$$ = job; 
	};


stmt_wait_job:
	WAIT JOB W_WORD ';'
	{
		AosJqlStmtJob *job = new AosJqlStmtJob;
		job->setJobName($3);
		job->setOp(JQLTypes::eWait);               
		gAosJQLParser.appendStatement(job);   
		$$ = job; 
	};

stmt_restart_job:
	RESTART JOB W_WORD ';'
	{
		AosJqlStmtJob *job = new AosJqlStmtJob;
		job->setJobName($3);
		gAosJQLParser.appendStatement(job);   
		$$ = job;                             
		$$->setOp(JQLTypes::eRestart);               
	}
	;


stmt_join_syncher:
	CREATE JOIN '(' expr_list ')' 
	ON '(' expr ')' 
	INDEX '(' expr_list ')' ';'
	{
		AosJqlStmtJoinSyncher *stmt = new AosJqlStmtJoinSyncher;
		stmt->setJoinType("inner_join");
		stmt->setJoinTables($4);
		stmt->setCondition($8);
		stmt->setJoinIndexs($12);
		stmt->setOp(JQLTypes::eCreate);
		gAosJQLParser.appendStatement(stmt);
		$$ = stmt;
	};
load_data_stmt:
	LOAD DATA W_WORD '(' stmt_expr_name_values ')' ';' 
	{
		AosJqlStmtLoadData *stmt = new AosJqlStmtLoadData();
		stmt->setLoadDataName($3);
		stmt->setConfParms($5);
		gAosJQLParser.appendStatement(stmt);
		$$ = stmt;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	LOAD DATA W_WORD INPUT DATASET W_WORD INTO W_WORD stmt_max_threads stmt_max_task load_data_fields_opts ';'
    {
		AosJqlStmtLoadData *load_data = new AosJqlStmtLoadData();
		load_data->setVersionNum(2);
		load_data->setLoadDataName($3);
		load_data->setFromDataSet($6);
		load_data->setToDataSet($8);
		load_data->setMaxThread($9);
		load_data->setTaskNum($10);
		load_data->setOptionFields($11);
		gAosJQLParser.appendStatement(load_data); 
		$$ = load_data;                           
		$$->setOp(JQLTypes::eCreate);          
    }
	|
	LOAD DATA W_WORD INPUT DATASET W_WORD INTO W_WORD 
	SOURCE TYPE HBASE RAWKEY '(' expr_list ')' stmt_max_threads stmt_max_task  load_data_fields_opts ';'
    {
		AosJqlStmtLoadData *load_data = new AosJqlStmtLoadData();
		load_data->setVersionNum(2);
		load_data->setLoadDataName($3);
		load_data->setFromDataSet($6);
		load_data->setToDataSet($8);
		load_data->setIsHbase(true);
		load_data->setRawkeyList($14);
		load_data->setMaxThread($16);
		load_data->setTaskNum($17);
		load_data->setOptionFields($18);
		gAosJQLParser.appendStatement(load_data); 
		$$ = load_data;                           
		$$->setOp(JQLTypes::eCreate);
    }
	|
	DROP DATA W_WORD '(' stmt_expr_name_values ')' ';' 
	{
		AosJqlStmtLoadData *stmt = new AosJqlStmtLoadData();
		stmt->setLoadDataName($3);
		stmt->setConfParms($5);
		gAosJQLParser.appendStatement(stmt);
		$$ = stmt;
		$$->setOp(JQLTypes::eDrop);
	}
	|
	DROP DATA W_WORD INPUT DATASET W_WORD INTO W_WORD stmt_max_threads stmt_max_task load_data_fields_opts ';'
    {
		AosJqlStmtLoadData *load_data = new AosJqlStmtLoadData();
		load_data->setLoadDataName($3);
		load_data->setFromDataSet($6);
		load_data->setToDataSet($8);
		load_data->setMaxThread($9);
		load_data->setOptionFields($11);
		load_data->setTaskNum($10);
		gAosJQLParser.appendStatement(load_data); 
		$$ = load_data;                           
		$$->setOp(JQLTypes::eDrop);
    }
	|
	DROP DATA W_WORD INPUT DATASET W_WORD INTO W_WORD 
	SOURCE TYPE HBASE RAWKEY '(' expr_list ')' stmt_max_threads stmt_max_task  load_data_fields_opts ';'
    {
		AosJqlStmtLoadData *load_data = new AosJqlStmtLoadData();
		load_data->setLoadDataName($3);
		load_data->setFromDataSet($6);
		load_data->setToDataSet($8);
		load_data->setIsHbase(true);
		load_data->setRawkeyList($14);
		load_data->setMaxThread($16);
		load_data->setOptionFields($18);
		load_data->setTaskNum($17);
		gAosJQLParser.appendStatement(load_data); 
		$$ = load_data;                           
		$$->setOp(JQLTypes::eDrop);
    }
	;

	
stmt_hbase:
	EXPORT DATA FROM HBASE CONFIG STRING ';'
	{
		AosJqlStmtHBase *hbase = new AosJqlStmtHBase();
		hbase->setConfig($6);
		$$ = hbase;                           
		gAosJQLParser.appendStatement(hbase); 
		$$->setOp(JQLTypes::eRun);          
	}
    ;
	

stmt_max_threads:
	{
		$$ = 1;
	}
	|
	MAX THREAD INTNUM
	{
		$$ = $3;
	}
	;

stmt_max_task:
	{
		$$ = 1;
	}
	|
	USE TASKS INTNUM
	{
		$$ = $3;
	}
	;

load_data_fields_opts:
	{
		$$ = new AosExprList; 
	}
	|
	FIELDS '(' expr_list ')'
	{
		$$ = $3;
	};

stmt_create_map:
	CREATE MAP W_WORD ON TABLE W_WORD
	KEY '(' expr_list ')'
	VALUE '(' expr ')'
	TYPE data_field_data_type
	data_field_max_len 
	opt_where ';'
	{
		AosJqlStmtMap *map = new AosJqlStmtMap;
		map->setMapName($3);
		map->setTableName($6);
		map->setKeys($9);
		map->setValue($13);
		map->setDataType($16);
		map->setMaxLen($17);
		if ($18)
			map->setWhereCond($18->getWhereExpr().getPtr());
		map->setOp(JQLTypes::eCreate);               
		gAosJQLParser.appendStatement(map);   
		$$ = map;                             
	};

stmt_drop_map:
	DROP MAP W_WORD ';'
	{
		AosJqlStmtMap *map = new AosJqlStmtMap;
		map->setMapName($3);
		gAosJQLParser.appendStatement(map);   
		$$ = map;                             
		$$->setOp(JQLTypes::eDrop);               
	}
	;

stmt_show_map:
	SHOW MAPS ';'
	{
		AosJqlStmtMap *map = new AosJqlStmtMap;
		gAosJQLParser.appendStatement(map);   
		$$ = map;                             
		$$->setOp(JQLTypes::eShow);               
	}
	;

stmt_describe_map:
	DESCRIBE MAP W_WORD ';'
	{
		AosJqlStmtMap *map = new AosJqlStmtMap;
		map->setMapName($3);
		gAosJQLParser.appendStatement(map);   
		$$ = map;                             
		$$->setOp(JQLTypes::eDescribe);               
	}
	;


stmt_list_map:
	LIST MAP W_WORD ';'
	{
		AosJqlStmtMap *map = new AosJqlStmtMap;
		map->setMapName($3);
		gAosJQLParser.appendStatement(map);   
		$$ = map;                             
		$$->setOp(JQLTypes::eList);               
	}
	;

stmt_create_nick_field:
	CREATE NICK FIELD W_WORD '(' stmt_expr_name_values ')' ';'
	{
		AosJqlStmtNickField *stmt = new AosJqlStmtNickField;
		stmt->setName($4);
		stmt->setConfParms($6);
		gAosJQLParser.appendStatement(stmt);
		$$ = stmt;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	CREATE NICK FIELD STRING
		ON W_WORD '(' STRING ')' 
	    data_field_max_len ';'
	{
		AosJqlStmtNickField *field = new AosJqlStmtNickField;
		field->setName($4);
		field->setTable($6);
		field->setOrigFieldName($8);
		field->setMaxLen($10);
		gAosJQLParser.appendStatement(field);
		$$ = field;
		$$->setOp(JQLTypes::eCreate);
	}
    ;

stmt_run_script_file:
	SOURCE STRING parameters suppress_flag ';'
    {
		AosJqlStmtRunScriptFile *stmt = new AosJqlStmtRunScriptFile;
		stmt->setFileName($2);
		stmt->setParameters($3);
		stmt->setSuppress($4);
		gAosJQLParser.appendStatement(stmt);   
		$$ = stmt;                             
	}
    ;

parameters:
	{
		$$ = NULL;
	}
	|
	expr_list
	{
		$$ = $1;
	}
	;

suppress_flag:
	{
		/* nil  */
		$$ = false;
	}
	|
	SUPPRESS ON
	{
		$$ = true;
	}
	|
	SUPPRESS OFF 
	{
		$$ = false;
	}
	;
stmt_create_schedule:
	CREATE SCHEDULE W_WORD 
	ON JOB W_WORD ';'
    {
		AosJqlStmtSchedule *schedule = new AosJqlStmtSchedule;
		schedule->setScheduleName($3);
		schedule->setJobName($6);
		gAosJQLParser.appendStatement(schedule);   
		$$ = schedule;                             
		$$->setOp(JQLTypes::eCreate);               
	}
	|
	CREATE SCHEDULE W_WORD 
	ON JOB W_WORD 
	SCHEDULE STRING RUN AT STRING STRING 
	STOP AT RUN STRING TIMES
	schedule_on_priority ';'
	{
		AosJqlStmtSchedule *schedule = new AosJqlStmtSchedule;
		schedule->setScheduleName($3);
		schedule->setJobName($6);
		schedule->setDateType($8);
		schedule->setDate($11);
		schedule->setRunTime($12);
		schedule->setStopTime($16);
		schedule->setPriority($18);
		gAosJQLParser.appendStatement(schedule);   
		$$ = schedule;                             
		$$->setOp(JQLTypes::eCreate);               
	}
	|
	CREATE SCHEDULE W_WORD 
	ON JOB W_WORD 
	SCHEDULE STRING RUN AT STRING 
	STOP AT TIME STRING
	schedule_on_priority ';'
	{
		AosJqlStmtSchedule *schedule = new AosJqlStmtSchedule;
		schedule->setScheduleName($3);
		schedule->setJobName($6);
		schedule->setDateType($8);
		schedule->setDate($12);
		schedule->setStopTime($15);
		schedule->setPriority($16);
		gAosJQLParser.appendStatement(schedule);   
		$$ = schedule;                             
		$$->setOp(JQLTypes::eCreate);               

	}
	|
	CREATE SCHEDULE W_WORD 
	ON JOB W_WORD 
	SCHEDULE STRING RUN AT STRING 
	schedule_on_priority ';'
	{
		AosJqlStmtSchedule *schedule = new AosJqlStmtSchedule;
		schedule->setScheduleName($3);
		schedule->setJobName($6);
		schedule->setDateType($8);
		schedule->setDate($11);
		schedule->setPriority($12);
		gAosJQLParser.appendStatement(schedule);   
		$$ = schedule;                             
		$$->setOp(JQLTypes::eCreate);               
	}
    ;

schedule_on_priority:
	{
		$$ = 0;
	}
	|
	ON PRIORITY INTNUM 
	{
		$$ = $3;
	}
	;

stmt_run_schedule:
	RUN SCHEDULE W_WORD ';'
	{
		AosJqlStmtSchedule *schedule = new AosJqlStmtSchedule;
		schedule->setScheduleName($3);
		gAosJQLParser.appendStatement(schedule);   
		$$ = schedule;                             
		$$->setOp(JQLTypes::eRun);               
	}
	;

stmt_stop_schedule:
	STOP SCHEDULE W_WORD ';'
	{
		AosJqlStmtSchedule *schedule = new AosJqlStmtSchedule;
		schedule->setScheduleName($3);
		gAosJQLParser.appendStatement(schedule);   
		$$ = schedule;                             
		$$->setOp(JQLTypes::eStop);               
	}
	;

stmt_drop_schedule:
	DROP SCHEDULE W_WORD ';'
	{
		AosJqlStmtSchedule *schedule = new AosJqlStmtSchedule;
		schedule->setScheduleName($3);
		gAosJQLParser.appendStatement(schedule);   
		$$ = schedule;                             
		$$->setOp(JQLTypes::eDrop);               
	}
	;

stmt_create_schema:
	CREATE STATIC SCHEMA W_WORD
	TYPE W_WORD 
	RECORD W_WORD ';'
	{
		AosJqlStmtSchema* statement = new AosJqlStmtSchema;
		statement->setName($4);
		statement->setType($6);
		statement->setRecord($8);
		statement->setSchemaType("static");
		gAosJQLParser.appendStatement(statement);
		$$ = statement;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	CREATE PARSER W_WORD 
	TYPE W_WORD
	SCHEMA W_WORD ';'
	{
		AosJqlStmtSchema* statement = new AosJqlStmtSchema;
		OmnString type = "parser";
		statement->setRecordType(type);
		statement->setName($3);
		statement->setType($5);
		statement->setRecord($7);
		statement->setSchemaType("static");
		gAosJQLParser.appendStatement(statement);
		$$ = statement;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	CREATE PARSER W_WORD 
	TYPE W_WORD 
	record_delimiter
	RECORD PICKER BY BYTES INTNUM ',' INTNUM 
	'(' stmt_record_pick_list ')' ';'
	{
		AosJqlStmtSchema* statement = new AosJqlStmtSchema;
		statement->setName($3);
		statement->setType("fixedlength");
		statement->setRecordLen(2000);
		statement->setRecordDelimiter($6);
		statement->setSchemaPos($11);
		statement->setSchemaLen($13);
		statement->setPickers($15);
		statement->setSchemaType("static");
		gAosJQLParser.appendStatement(statement);
		$$ = statement;
		$$->setOp(JQLTypes::eCreate);
	}
	;


stmt_record_pick_list:
	SCHEMA W_WORD KEY W_WORD LENGTH INTNUM 
	{
	 	$$ = new vector<AosJqlRecordPickerPtr>;
		AosJqlRecordPickerPtr record_pick = OmnNew AosJqlRecordPicker;
		record_pick->record = $2;
		record_pick->matchStr = $4;
		record_pick->offset = $6;
		free($2);
		free($4);
		$$->push_back(record_pick);
	}
	|
	stmt_record_pick_list ',' SCHEMA W_WORD KEY W_WORD LENGTH INTNUM
	{
		$$ = $1;
		AosJqlRecordPickerPtr record_pick = OmnNew AosJqlRecordPicker;
		record_pick->record = $4;
		record_pick->matchStr = $6;
		record_pick->offset = $8;
		free($4);
		free($6);
		$$->push_back(record_pick);
	}
	;

stmt_show_schema:
	SHOW PARSER ';'
	{
		AosJqlStmtSchema* statement = new AosJqlStmtSchema;

		gAosJQLParser.appendStatement(statement);

		$$ = statement;
		$$->setOp(JQLTypes::eShow);
		//cout << "List all the Schema " << endl;
	};

stmt_describe_schema:
	DESCRIBE PARSER W_WORD ';'
	{
		AosJqlStmtSchema* statement = new AosJqlStmtSchema;
		gAosJQLParser.appendStatement(statement);

		$$ = statement;
		$$->setName($3);
		$$->setOp(JQLTypes::eDescribe);
	};
  
stmt_drop_schema:
	DROP PARSER W_WORD ';'
	{
		AosJqlStmtSchema* statement = new AosJqlStmtSchema;
		statement->setName($3);

		gAosJQLParser.appendStatement(statement);
		$$ = statement;
		$$->setOp(JQLTypes::eDrop);
	}
	;
select_stmt:
	SELECT select_opts select_expr_list';'
	{
		AosJqlStmtQuery * select = new AosJqlStmtQuery();
		gAosJQLParser.appendStatement(select);             
		select->setExprs($3);
		$$ = select;   
	}
	|
	jql_select_stmt ';'
	{
		AosJqlStmtQuery * select = new AosJqlStmtQuery($1);
		gAosJQLParser.appendStatement(select);
		$$ = select;
	//	$$->mQueryStr =gAosJQLParser.getCurrJQL();
	}
	;

jql_select_stmt:
	SELECT
        select_opts select_expr_list
    FROM
        table_references opt_where stmt_group_by opt_having stmt_order_by  opt_limit 
	stmt_hack_convert_to stmt_hack_format  stmt_hack_into_file
    {
		$$ =new AosJqlSelect;
		//s1->mType = $2;
		$$->mFieldListPtr = $3;
		$$->mTableReferencesPtr = $5;
		$$->mWherePtr = $6;
		$$->mGroupByPtr = $7;
		$$->mHavingPtr = $8;
		$$->mOrderByPtr = $9;
		$$->mLimitPtr = $10;
		$$->mHackConvertTo = $11;
		$$->mHackFormat = $12;
		$$->mHackIntoFile = $13;
    }
    ;


select_opts:
	/* nil */
	{ 
		$$ = new AosJQLStmtSelectOptionList; 
	}
	| 
	select_opts ALL                 
	{ 
		$$->push_back(eSelectOpt_SelectAll); 
	}
	| 
	select_opts DISTINCT            
	{
		$$->push_back(eSelectOpt_Distinct);
	}
	| 
	select_opts DISTINCTROW         
	{
		$$->push_back(eSelectOpt_DistinctRow);
	}
	| 
	select_opts HIGH_PRIORITY       
	{
		$$->push_back(eSelectOpt_HighPriority);
	}
	| 
	select_opts STRAIGHT_JOIN       
	{
		$$->push_back(eSelectOpt_StraightJoin);
	}
	| 
	select_opts SQL_SMALL_RESULT    
	{
		$$->push_back(eSelectOpt_SmallResult);
	}
	| 
	select_opts SQL_BIG_RESULT      
	{
		$$->push_back(eSelectOpt_BigResult);
	}
	| 
	select_opts SQL_CALC_FOUND_ROWS 
	{
		$$->push_back(eSelectOpt_CallFoundRows);
	}
	;

select_expr_list: 
	select_expr 
	{ 
		$$ = new AosJqlSelectFieldList;
		$$->push_back($1);
	}
	| 
	select_expr_list ',' select_expr 
	{
		$$->push_back($3); 
	}
	;

select_expr: 
	expr opt_as_alias
	{
		AosJqlSelectField *field = new AosJqlSelectField; 
		if ($1->getType() == AosExprType::eGenFunc)
		{
			AosExprGenFuncPtr expr_func;
			expr_func = dynamic_cast<AosExprGenFunc*>($1);
			AosExprList *parmlist = expr_func->getParmList();
			OmnString func_name = expr_func->getFuctName();
			if (func_name.toLower() == "avg" && parmlist->size() == 1)
			{
				AosExprObjPtr lhs = OmnNew AosExprGenFunc("sum", (*parmlist)[0]);
				AosExprObjPtr rhs = OmnNew AosExprGenFunc("count", (*parmlist)[0]);
				AosExprObjPtr expr = new AosExprArith(lhs, AosExprArith::eDivide, rhs);
				field->setField(expr);
				if ($2)
					field->setAliasName($2);
				else
					field->setAliasName(OmnNew AosExprString(expr_func->getStatFieldName().getBuffer()));
			}
			else
			{
				field->setField($1);
				field->setAliasName($2);
			}
		}
		else
		{
			field->setField($1);
			field->setAliasName($2);
		}
		$$ = field;
	}
	|
	'*'
	{
		 AosJqlSelectField *field = new AosJqlSelectField;
		 AosExprObjPtr expr = OmnNew AosExprFieldName("*");
		 field->setField(expr);
		 $$ = field;
	}
	;

table_references:    
	table_reference 
	{ 
		$$ = new AosTableReferences;
		$$->push_back($1);
	}
	| 
	table_references ',' table_reference 
	{ 
		$$->push_back($3); 
	}
	;

table_reference:  
	table_factor
	{
		$$ = new AosJqlTableReference;
		$$->setTableFactor($1);
	}
	|
	join_table
	{
		$$ = new AosJqlTableReference;
		$$->setJqlJoinTable($1);
	}
	;

join_table:
	/* nil */
	{
		$$ = new AosJqlJoinTable;
	}
	;



table_factor:
    expr opt_as_alias index_hint	
	{
		$$ = new AosJqlTableFactor;
		$$->setType(eTRef_TableFactorByTable);
		$$->setAliasName($2);
		$$->setTable($1);
		$$->setIndexHint($3);
	}
	|
	table_subquery AS W_WORD
	{
		$$ = new AosJqlTableFactor;
		/*$$->setVirtualTable($1);*/
		$$->setQuery($1);
		$$->setTable(new AosExprFieldName($3));
		$$->setAliasName(new AosExprFieldName($3));

	}
	|
	STRING '.' STRING opt_as_alias index_hint
	{
		$$ = new AosJqlTableFactor;
		$$->setTable(new AosExprMemberOpt("", $1, $3));
		$$->setAliasName($4);
		$$->setIndexHint($5);
	}
	;

table_subquery:
	'(' jql_select_stmt ')'
	{
		/*
		$$ = new AosJqlSubQueryTable;
		$$->setVirtualTable($2);
		$$->setName($5);
		*/
		AosJqlStmtQuery *select = new AosJqlStmtQuery($2);
		$$ = select;
	}
	;

index_hint:
	{
		$$ = NULL;
	}
	|
	USE INDEX '(' expr_list ')' 
	{
		/* index hint */			
		$$ = $4;
	}
	;


opt_limit:                                  
	 /* nil */ 
	{
		$$ = 0;
	//  $$ = new AosExprList;
	}
	| 
	LIMIT INTNUM 
	{
	  $$ = new AosJqlLimit;
	  $$->setOffset(0);
	  $$->setRowCount($2);
	}
	|
	LIMIT INTNUM ',' INTNUM
	{
	  $$ = new AosJqlLimit;
	  if ($2 < 0) $2 = 0;
	  $$->setOffset($2);
	  $$->setRowCount($4);
	}
	;


stmt_group_by:
	{
		$$ = 0;
	}
	|
	GROUP BY select_expr_list 
	{
		$$ = new AosJqlGroupBy;
		$$->setGroupFieldList($3);
	}
	|
	GROUP BY ROLLUP '(' rollup_lists ')'
	{
		$$ = new AosJqlGroupBy;
		$$->setRollupLists($5);
	}
	|
	GROUP BY CUBE '(' select_expr_list ')'
	{
		$$ = new AosJqlGroupBy;
		$$->setCubeLists($5);
	}
	|
	GROUP BY select_expr_list ',' CUBE '(' select_expr_list ')'
	{
		$$ = new AosJqlGroupBy;
		$$->setGroupFieldList($3);
		$$->setCubeLists($7);
	}
	;

rollup_lists:
	'(' rollup_list ')'
	{
		vector<AosJqlSelectFieldList*> *rollupLists;

		rollupLists = new vector<AosJqlSelectFieldList*>;
		rollupLists->push_back($2);
		$$ = rollupLists;
	}
	|
	rollup_lists ',' '(' rollup_list ')'
	{
		$$ = $1;
		$$->push_back($4);
	}
	;

rollup_list:
	{
		/*nil, init a expression list*/
		AosJqlSelectFieldList *rollupList = new AosJqlSelectFieldList;
		$$ = rollupList;
	}
	|
	select_expr_list
	{
		$$ = $1;
	}
	;
	
stmt_order_by:
	{
		$$ = 0;
	}
	|
	ORDER BY stmt_order_field_list
	{
		$$ = new AosJqlOrderBy;	
		$$->setOrderFieldList($3);
	}
	;

stmt_order_field:
	expr stmt_order_opt
	{
		$$ =  new AosJqlOrderByField;
		AosJqlSelectField *field = new AosJqlSelectField;
		field->setField($1);
		$$->mField = field;
		$$->mIsAsc = $2;
	}
	;

stmt_order_field_list:
	stmt_order_field
	{
		$$ = new AosJqlOrderByFieldList;
		$$->push_back($1);
	}
	|
	stmt_order_field_list ',' stmt_order_field
	{
		$$->push_back($3);
	}
	;

stmt_order_opt:
	{
		$$ = true;
	}
	|
	DESC
	{
		$$ = false;
	}
	|
	ASC
	{
		$$ = true;
	}
	;

opt_having:
	{
		$$ = 0;
	}
	|
	HAVING expr 
	{
		$$ = new AosJqlHaving();
		$$->setHavingExpr($2);
	}
	;

stmt_hack_format:
	{
		$$ = NULL;
	}
	|
	FORMAT expr
	{
		$$ = $2;
	};

stmt_hack_into_file:
	{
		$$ = NULL;
	}
	|
	INTO expr
	{
		$$ = $2;
	};

stmt_hack_convert_to:
	{
		$$ = NULL;
	}
	|
	CONVERT TO expr
	{
		$$ = $3;
	}
	;
stmt_select_into:
	jql_select_stmt INTO_OUTFILE STRING set_job_name record_format stmt_max_threads ';'
	{
		AosJqlStmtSelectInto * selectinto = new AosJqlStmtSelectInto();
		selectinto->setSelect($1);
		selectinto->setFilePath($3);
		selectinto->setJobName($4);
		selectinto->setFormat($5);
		selectinto->setMaxThreads($6);
		gAosJQLParser.appendStatement(selectinto);             
		$$ = selectinto;   
	}
	;


record_format:
	{	
		$$ = 0;
	}
	|
 	FIELDS TERMINATED BY STRING ENCLOSED BY STRING  ESCAPED BY STRING LINES STARTING BY STRING TERMINATED BY STRING	
	{
		vector<OmnString> *v = new vector<OmnString>; 
		v->push_back($4);
		v->push_back($7);
		v->push_back($10);
		v->push_back($14);
		v->push_back($17);
		$$ = v;
	}
	;

set_job_name:
	{
		$$ = 0;
	}
	|
	JOB W_WORD 
	{
		$$ = $2;
	}
	;
stmt_create_sequence:
	CREATE SEQUENCE W_WORD sequence_info ';'
	{
		AosJqlStmtSequence* stmt = new AosJqlStmtSequence;
		stmt->setName($3);
		stmt->setSequenceInfo($4);
		gAosJQLParser.appendStatement(stmt);
		$$ = stmt;
		$$->setOp(JQLTypes::eCreate);
	};

sequence_info:
	create_sequence_increment_by
	create_sequence_start_with
	create_sequence_max_value
	create_sequence_min_value
	create_sequence_cycle
	create_sequence_cache
	create_sequence_order
	{
		$$ = new AosJQLSequenceInfo;
		$$->setIncrementBy($1);
		$$->setStartWith($2);
		$$->setMaxValue($3);
		$$->setMinValue($4);
		$$->setIsCycle($5);
		$$->setCacheNum($6);
		$$->setIsOrder($7);
	};

create_sequence_increment_by:
	{
		$$ = 1;
	}
	|
	INCREMENT BY INTNUM
	{
		$$ = $3; 
	};

create_sequence_start_with:
	{
		$$ = 1;
	}
	|
	START WITH INTNUM
	{
		$$ = $3;	
	};

create_sequence_max_value:
	{
		$$ = ~(1 << 31);
	}
	|
	NOMAXVALUE
	{
		$$ = ~(1 << 31);
	}
	|
	MAXVALUE INTNUM
	{
		$$ = $2;
	};

create_sequence_min_value:
	{
		$$ = 1 << 31;
	}
	|
	NOMINVALUE
	{
		$$ = 1 << 31;
	}
	|
	MINVALUE INTNUM
	{
		$$ = $2;
	};

create_sequence_cycle:
	{
		$$ = false;
	}
	|
	CYCLE
	{
		$$ = true;
	}
	|
	NOCYCLE
	{
		$$ = false;
	};

create_sequence_cache:
	{
		$$ = 20;
	}
	|
	CACHE INTNUM
	{
		$$ = $2;
	}
	|
	NOCACHE
	{
		$$ = 20;
	};

create_sequence_order:
	{
		$$ = false;
	}
	|
	ORDER
	{
		$$ = true;
	}
	|
	NOORDER
	{
		$$ = false;
	};
stmt_create_service:
	CREATE SERVICE W_WORD USING JOB W_WORD ';' 
	{
		AosJqlStmtService* statement = new AosJqlStmtService;

		statement->setServiceName($3);
		statement->setJobName($6);
		gAosJQLParser.appendStatement(statement);

		$$ = statement;
		$$->setOp(JQLTypes::eCreate);
	}
	;

stmt_start_service:
   	START SERVICE W_WORD ';'
    {
		AosJqlStmtService* statement = new AosJqlStmtService();	
		gAosJQLParser.appendStatement(statement);

		statement->setServiceName($3);
		$$ = statement;
		$$->setOp(JQLTypes::eStart); 
    }
	|
	START SERVICE W_WORD '{' stmt_expr_name_values '}' ';'
	{
		AosJqlStmtService *stmt = new AosJqlStmtService;
		stmt->setServiceName($3);
		stmt->setConfParms($5);
		gAosJQLParser.appendStatement(stmt);
		$$ = stmt;
		$$->setOp(JQLTypes::eStart);
	}
    ;

stmt_stop_service:
   	STOP SERVICES W_WORD ';'
    {
		AosJqlStmtService* statement = new AosJqlStmtService();	
		gAosJQLParser.appendStatement(statement);

		statement->setServiceName($3);
		$$ = statement;
		$$->setOp(JQLTypes::eStop); 
    }
    ;

stmt_show_service:
   	SHOW SERVICES ';'
    {
		AosJqlStmtService* statement = new AosJqlStmtService();	
		gAosJQLParser.appendStatement(statement);

		$$ = statement;
		$$->setOp(JQLTypes::eShow); 
    }
    ;

stmt_describe_service:
   	DESCRIBE SERVICE W_WORD ';'
    {
		AosJqlStmtService* statement = new AosJqlStmtService();	
		gAosJQLParser.appendStatement(statement);

		statement->setServiceName($3);
		$$ = statement;
		$$->setOp(JQLTypes::eDescribe); 
    }
    ;
stmt_drop_service:
   	DROP SERVICE W_WORD ';'
    {
		AosJqlStmtService* statement = new AosJqlStmtService();	
		gAosJQLParser.appendStatement(statement);

		statement->setServiceName($3);
		$$ = statement;
		$$->setOp(JQLTypes::eDrop); 
    }
    ;
stmt_run_stat:
	RUN STAT W_WORD W_WORD  stmt_max_threads ';'
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
	|
	RUN STAT W_WORD W_WORD
	KEY FIELDS '(' column_list ')' stmt_max_threads ';'
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
	;
stmt_create_statistics:
	CREATE STATISTICS W_WORD ON TABLE W_WORD ';'
	{
		AosJqlStmtStatistics* statement = new AosJqlStmtStatistics;
		statement->setStatName($3);
		statement->setTableName($6);
		gAosJQLParser.appendStatement(statement);
		$$ = statement;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	CREATE STATISTICS MODEL W_WORD ON TABLE expr_list 
	opt_where GROUP BY '(' stmt_dimensions_list ')'
	MEASURES '(' stmt_stat_measure_list ')' 
	opt_shuffle_field  ';'
	{
		AosJqlStmtStatistics* statement = new AosJqlStmtStatistics;
		statement->setStatName($4);
		statement->setTableNames($7);
		if ($8 != 0)
			statement->setWhereCond($8->getWhereExpr().getPtr());
		statement->setGroupByFields($12);
		statement->setMeasures($16);
		statement->setShuffleField($18);
		gAosJQLParser.appendStatement(statement);
		$$ = statement;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	CREATE STATISTICS MODEL W_WORD ON TABLE expr_list 
	opt_where MEASURES '(' stmt_stat_measure_list ')'
	opt_shuffle_field  ';'
	{
		AosJqlStmtStatistics* statement = new AosJqlStmtStatistics;
		statement->setStatName($4);
		statement->setTableNames($7);
		if ($8 != 0)
			statement->setWhereCond($8->getWhereExpr().getPtr());
		statement->setMeasures($11);
		statement->setShuffleField($13);
		
		gAosJQLParser.appendStatement(statement);
		$$ = statement;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	CREATE STATISTICS MODEL W_WORD ON TABLE expr_list 
	opt_where MEASURES '(' stmt_stat_measure_list ')'
	TIME FIELD expr UNIT STRING 
	opt_shuffle_field  ';'
	{
		AosJqlStmtStatistics* statement = new AosJqlStmtStatistics;
		statement->setStatName($4);
		statement->setTableNames($7);
		if ($8 != 0)
			statement->setWhereCond($8->getWhereExpr().getPtr());
		statement->setMeasures($11);
		statement->setTimeFieldName($15);
		statement->setTimeUnit($17);
		statement->setShuffleField($18);

		gAosJQLParser.appendStatement(statement);
		$$ = statement;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	CREATE STATISTICS MODEL W_WORD ON TABLE expr_list 
	opt_where MEASURES '(' stmt_stat_measure_list ')'
	TIME FIELD expr FORMAT STRING UNIT STRING
	opt_shuffle_field  ';'
	{
		AosJqlStmtStatistics* statement = new AosJqlStmtStatistics;
		statement->setStatName($4);
		statement->setTableNames($7);
		if ($8 != 0)
			statement->setWhereCond($8->getWhereExpr().getPtr());
		statement->setMeasures($11);
		statement->setTimeFieldName($15);
		statement->setTimeOriginalFormat($17);
		statement->setTimeUnit($19);
		statement->setShuffleField($20);

		gAosJQLParser.appendStatement(statement);
		$$ = statement;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	CREATE STATISTICS MODEL W_WORD ON TABLE expr_list 
	opt_where GROUP BY '(' stmt_dimensions_list ')'
	MEASURES '(' stmt_stat_measure_list ')'
	TIME FIELD expr UNIT STRING 
	opt_shuffle_field  ';'
	{
		AosJqlStmtStatistics* statement = new AosJqlStmtStatistics;
		statement->setStatName($4);
		statement->setTableNames($7);
		if ($8 != 0)
			statement->setWhereCond($8->getWhereExpr().getPtr());
		statement->setGroupByFields($12);
		statement->setMeasures($16);
		statement->setTimeFieldName($20);
		statement->setTimeUnit($22);
		statement->setShuffleField($23);

		gAosJQLParser.appendStatement(statement);
		$$ = statement;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	CREATE STATISTICS MODEL W_WORD ON TABLE expr_list 
	opt_where GROUP BY '(' stmt_dimensions_list ')'
	MEASURES '(' stmt_stat_measure_list ')'
	TIME FIELD expr FORMAT STRING UNIT STRING
	opt_shuffle_field ';'
	{
		AosJqlStmtStatistics* statement = new AosJqlStmtStatistics;
		statement->setStatName($4);
		statement->setTableNames($7);
		if ($8 != 0)
			statement->setWhereCond($8->getWhereExpr().getPtr());
		statement->setGroupByFields($12);
		statement->setMeasures($16);
		statement->setTimeFieldName($20);
		statement->setTimeOriginalFormat($22);
		statement->setTimeUnit($24);
		statement->setShuffleField($25);

		gAosJQLParser.appendStatement(statement);
		$$ = statement;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	UPDATE STATISTICS MODEL W_WORD ON TABLE W_WORD 
	GROUP BY '(' stmt_dimensions_list ')' ';'
	{
		AosJqlStmtStatistics* statement = new AosJqlStmtStatistics;
		statement->setStatName($4);
		statement->setTableName($7);
		statement->setDimensions($11);

		gAosJQLParser.appendStatement(statement);
		$$ = statement;
		$$->setOp(JQLTypes::eUpdate);
	}
	|
	DROP STATISTICS W_WORD ';'
	{
		AosJqlStmtStatistics* statement = new AosJqlStmtStatistics;
		statement->setStatName($3);
		gAosJQLParser.appendStatement(statement);
		$$ = statement;
		$$->setOp(JQLTypes::eDrop);
	}
	;

opt_shuffle_field:
	 {
	 	$$ = NULL;
	 }
	 |
	 SHUFFLE FIELD '('expr_list')'
	 {
	 	$$ = new AosExprList;
		$$ = $4;
	 }
	 ;
stmt_dimensions_list:
	stmt_dimension_list
	{
		$$ = new vector<AosExprList*>;
		$$->push_back($1);
	}
	|
	stmt_dimensions_list ',' stmt_dimension_list
	{
		$$->push_back($3);
	}
	;

stmt_dimension_list:
	column_list 
	{
		$$ = $1;
	}
	|
	DISTINCT W_WORD ON column_list
	{
		OmnString distName = "distinct:";
		AosExprObj *expr;
		
		$$ = $4;
		distName << $2;
		expr = new AosExprFieldName(distName.getBuffer());
		$$->push_back(expr);
	}
	;

stmt_stat_measure_list:
		stmt_stat_measure
		{
			$$ = new vector<AosJqlStmtStatistics::AosMeasure*>;
			$$->push_back($1);
		}
		|
		stmt_stat_measure_list ',' stmt_stat_measure  
		{
			$$->push_back($3);
		}
		;

stmt_stat_measure:
		expr
		{
			$$ = new AosJqlStmtStatistics::AosMeasure;
			$$->mExpr = $1;
		}
		|
		expr ON column_list
		{
			$$ = new AosJqlStmtStatistics::AosMeasure;
			$$->mExpr = $1;
			$$->mExprList = $3;
		}
		;


stmt_show_statistics:
	SHOW STATISTICS ';'
	{
	    AosJqlStmtStatistics *statistics = new AosJqlStmtStatistics;
	  	gAosJQLParser.appendStatement(statistics);   
	    $$ = statistics;                             
		$$->setOp(JQLTypes::eShow);
	}
	;


stmt_describe_statistics:
	DESCRIBE STATISTICS W_WORD ';'
	{
		AosJqlStmtStatistics *statistics = new AosJqlStmtStatistics;
		statistics->setStatName($3);
		gAosJQLParser.appendStatement(statistics);   
		$$ = statistics;                             
		$$->setOp(JQLTypes::eDescribe);        
	}
	;
stmt_complete_iil:
	COMPLETE IIL W_WORD ';'
	{
		AosJqlStmtCompleteIIL *statement = new AosJqlStmtCompleteIIL;
		OmnString iilname = $3;
		statement->setIILName(iilname);
		gAosJQLParser.appendStatement(statement);

		$$ = statement;
	};
stmt_create_syncer:
	CREATE SYNCER expr SERVICE
	'(' expr_list ')' ';'
	{
		AosJqlStmtSyncer * statement = new AosJqlStmtSyncer;
		statement->setName($3);
		statement->setParms($6);
		gAosJQLParser.appendStatement(statement);
		$$ = statement;
		$$->setOp(JQLTypes::eCreate);
	}
	;


stmt_drop_syncer:
	DROP SYNCER expr ';'
	{
		AosJqlStmtSyncer * statement = new AosJqlStmtSyncer;
		statement->setName($3);
		gAosJQLParser.appendStatement(statement);
		$$ = statement;
		$$->setOp(JQLTypes::eDrop);
	}
	;

stmt_create_table:
	CREATE TABLE stmt_table_if_not_exists table_name '(' create_table_definition_group ')' table_options using_dataschema ';'
	{
		AosJqlStmtTable* statement = new AosJqlStmtTable;
		AosJqlTableOption *opt = $8;
		AosJqlCreateDefineGroup *defineGroup = $6;
		
		statement->setName($4);
		statement->setExists($3);
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
		statement->setSchemaName($9);

		$$ = statement;
		$$->setOp(JQLTypes::eCreate);
		//cout << "Create Table:" << " name " << $4 << endl;
	}
	|
	CREATE TABLE stmt_table_if_not_exists table_name table_options using_dataschema ';'
	{
		AosJqlStmtTable* statement = new AosJqlStmtTable;
		statement->setName($4);
		AosJqlTableOption *opt = $5; 
		if (opt) statement->setOption(opt);
		gAosJQLParser.appendStatement(statement);
		statement->setSchemaName($6);
		$$ = statement;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	CREATE TABLE stmt_table_if_not_exists table_name LIKE table_name ';'
	{
		AosJqlStmtTable* statement = new AosJqlStmtTable;

		statement->setName($4);
		statement->setLikeTable($6);
		gAosJQLParser.appendStatement(statement);

		$$ = statement;
		$$->setOp(JQLTypes::eCreate);
		//cout << "Create Table Statement like found: " << $3 << " from " << $5 << endl;
	}
	|
	CREATE VIRTUAL TABLE stmt_table_if_not_exists table_name using_dataschema 
	FIELDS '(' stmt_virtual_table_field_list ')' ';'
	{
		AosJqlStmtTable* statement = new AosJqlStmtTable;
		statement->setName($5);
		statement->setVirtual(true);
		statement->setSchemaName($6);
		statement->mVirtualColumns = $9;
		$$ = statement;
		$$->setOp(JQLTypes::eCreate);
		gAosJQLParser.appendStatement(statement);
	}
	;

stmt_virtual_table_field_list:
	W_WORD '(' expr_list ')' TYPE STRING 
	{
		$$ = new vector<AosJqlTableVirtulFieldDefPtr>;
		AosJqlTableVirtulFieldDefPtr vv = OmnNew AosJqlTableVirtulFieldDef();
		vv->mName = $1;
		vv->mValues = $3;
		vv->mType= $6;
		$$->push_back(vv);
	}
	|
	stmt_virtual_table_field_list ',' W_WORD '(' expr_list ')' TYPE STRING 
	{
		AosJqlTableVirtulFieldDefPtr vv = OmnNew AosJqlTableVirtulFieldDef();
		vv->mName = $3;
		vv->mValues = $5;
		vv->mType = $8;
		$$->push_back(vv);
	}
	;


using_dataschema:
	{
		$$ = 0;
	}
	|
	USING W_WORD
	{
		$$ = $2;
	};

table_name:
	W_WORD
	{
		$$ = $1;
	}
	|
	W_WORD '.' W_WORD
	{
		AosJqlStmtDatabase* db1 = new AosJqlStmtDatabase;
		db1->setName($1);
		db1->setOp(JQLTypes::eCreate);
		gAosJQLParser.appendStatement(db1);
		AosJqlStmtDatabase* db2= new AosJqlStmtDatabase;
		db2->setName($1);
		db2->setOp(JQLTypes::eUse); 
		gAosJQLParser.appendStatement(db2);
		$$ = $3;
	}
	|
	STRING
	{
		$$ = $1;
	}
	|
	STRING '.' STRING
	{
		AosJqlStmtDatabase* db1 = new AosJqlStmtDatabase;
		db1->setName($1);
		db1->setOp(JQLTypes::eCreate);
		gAosJQLParser.appendStatement(db1);
		AosJqlStmtDatabase* db2= new AosJqlStmtDatabase;
		db2->setName($1);
		db2->setOp(JQLTypes::eUse); 
		gAosJQLParser.appendStatement(db2);
		$$ = $3;
	}
	;

create_table_definition_group:
	column_definition
	{
		AosJqlCreateDefineGroup* defineGroup = new AosJqlCreateDefineGroup;
		defineGroup->columns = new vector<AosJqlColumnPtr>;
		/*defineGroup->indexes = new vector<AosJqlIndex*>;*/

		$$ = defineGroup;
		//cout << "Init table definition group" << endl;

		$$->columns->push_back($1);
		//cout << "Add a column definition to table definition group" << endl;
	};
	|
	create_table_definition_group ',' column_definition
	{
		$$ = $1;

		$$->columns->push_back($3);
		//cout << "Add a column definition to table definition group" << endl;
	};

column_definition:
	W_WORD data_type
	{
		AosJqlColumn* column = new AosJqlColumn;

		column->name = $1;
		column->type = $2->type;
		column->size = $2->size;
		column->format = $2->format;
		column->defaultVal = NULL;
		column->nullable = true; //default permits null value

		$$ = column;
		delete $2;
		//cout << "Create Table column found:" << $1 << endl;
	};
	|
	column_definition DEFAULT expr
	{
		AosJqlColumn* column = $1;
		AosValueRslt value;

		$3->getValue(0, 0, value);
		OmnString str = value.getStr();

		column->defaultVal = str;

		$$ = column;
		//cout << "Set column default: " << str << endl;
	};	
	|
	column_definition NULLX
	{
		AosJqlColumn* column = $1;
		AosValueRslt value;

		column->nullable = true;

		$$ = column;
		//cout << "Set column default: " << str << endl;
	};	
	|	
	column_definition NOT NULLX
	{
		AosJqlColumn* column = $1;
		AosValueRslt value;

		column->nullable = false;

		$$ = column;
		//cout << "Set column default: " << str << endl;
	};	
	|	
	column_definition COMMENTS STRING
	{
		AosJqlColumn* column = $1;
		AosValueRslt value;

		column->comment = $3;

		$$ = column;
		//cout << "Set column comments: " << $$->comment << endl;
	};	

data_type:
    BIT
	{
		$$ = new AosJqlColumn;
		$$->type = AosDataType::eInt32;
		$$->size = AosDataType::getValueSize($$->type); 
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	};
	|
    TINYINT
	{
		$$ = new AosJqlColumn;
		$$->type = AosDataType::eInt32;
		$$->size = AosDataType::getValueSize($$->type); 
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	};
	|
    SMALLINT
	{
		$$ = new AosJqlColumn;
		$$->type = AosDataType::eInt32;
		$$->size = AosDataType::getValueSize($$->type); 
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	};
	|
    MEDIUMINT
	{
		$$ = new AosJqlColumn;
		$$->type = AosDataType::eInt32;
		$$->size = AosDataType::getValueSize($$->type); 
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	};
	|
    INT
	{
		$$ = new AosJqlColumn;
		$$->type = AosDataType::eInt32;
		$$->size = AosDataType::getValueSize($$->type); 
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	}
	|
	NUMBER '(' INTNUM  ',' INTNUM ')'
	{
		$$ = new AosJqlColumn;
		$$->type = AosDataType::eNumber;
		$$->size = AosDataType::getValueSize($$->type); 
		$$->v1 = $3;
		$$->v2 = $5;
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	}
	|
	NUMBER '(' INTNUM ')'
	{
		$$ = new AosJqlColumn;
		$$->type = AosDataType::eNumber;
		$$->size = AosDataType::getValueSize($$->type); 
		$$->v1 = $3;
		$$->v2 = 0;
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	}
	|
	BIGINT
	{
		$$ = new AosJqlColumn;
		$$->type = AosDataType::eInt64;
		$$->size = AosDataType::getValueSize($$->type); 
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	};
	|

    BIGINT '(' INTNUM ')'
	{
		$$ = new AosJqlColumn;
		$$->type = AosDataType::eInt64;
		$$->size = $3;
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	};
	|
    SMALLINT UNSIGNED
	{
		$$ = new AosJqlColumn;
		$$->type = AosDataType::eU32;
		$$->size = AosDataType::getValueSize($$->type); 
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	};
	|
    MEDIUMINT UNSIGNED
	{
		$$ = new AosJqlColumn;
		$$->type = AosDataType::eU32;
		$$->size = AosDataType::getValueSize($$->type); 
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	};
	|
    INT UNSIGNED
	{
		$$ = new AosJqlColumn;
		$$->type = AosDataType::eU32;
		$$->size = AosDataType::getValueSize($$->type); 
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	};
	|
	BIGINT UNSIGNED
	{
		$$ = new AosJqlColumn;
		$$->type = AosDataType::eU64;
		$$->size = AosDataType::getValueSize($$->type); 
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	};
	|
    FLOAT	
	{
		$$ = new AosJqlColumn;
	    $$->type = AosDataType::eDouble;
		$$->size = AosDataType::getValueSize($$->type); 
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	};
	|
	DOUBLE
	{
		$$ = new AosJqlColumn;
	    $$->type = AosDataType::eDouble;
		$$->size = AosDataType::getValueSize($$->type); 
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	}
	|
	DATE
	{
		$$ = new AosJqlColumn;
	    $$->type = AosDataType::eDate;
		$$->size = AosDataType::getValueSize($$->type); 
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	}
	|
	CHAR 
	{
		$$ = new AosJqlColumn;
	    $$->type = AosDataType::eChar;
		$$->size = 1;
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	};
	|
	CHAR '(' INTNUM ')'
	{
		$$ = new AosJqlColumn;
	    $$->type = AosDataType::eChar;
		$$->size = $3;
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	};
	|
	CHAR '(' INTNUM BYTE')'
	{
		$$ = new AosJqlColumn;
	    $$->type = AosDataType::eChar;
		$$->size = $3;
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	}
	|
	VARCHAR '(' INTNUM ')'
	{
		$$ = new AosJqlColumn;
	    $$->type = AosDataType::eCharStr;
		$$->size = $3;
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	}
	|
	VARCHAR '(' INTNUM  BYTE ')'
	{
		$$ = new AosJqlColumn;
	    $$->type = AosDataType::eCharStr;
		$$->size = $3;
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	}
	|
	VARCHAR2 '(' INTNUM ')'
	{
		$$ = new AosJqlColumn;
	    $$->type = AosDataType::eCharStr;
		$$->size = $3;
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	}
	|
	VARCHAR2 '(' INTNUM  BYTE ')'
	{
		$$ = new AosJqlColumn;
	    $$->type = AosDataType::eCharStr;
		$$->size = $3;
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	}
	|
	TEXT
	{
		$$ = new AosJqlColumn;
	    $$->type = AosDataType::eCharStr;
		$$->size = 1024;
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	};
	|
	BINARY
	{
		$$ = new AosJqlColumn;
	    $$->type = AosDataType::eBuff;
		$$->size = 1024;
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	};
    |
	DATE '(' STRING ')'
	{
		$$ = new AosJqlColumn;
	    $$->type = AosDataType::eDate;
		string tmp = $3;
	    $$->format = $3;
		$$->size = DATE_SIZE;
	}
	|
	BINARY '(' INTNUM ')'
	{
		$$ = new AosJqlColumn;
	    $$->type = AosDataType::eBuff;
		$$->size = $3;
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	};

table_options:
	/*nil*/		 
	{
		$$ = new AosJqlTableOption;
		//cout << "Init a new table option struct" <<  endl;
	};
	|
	table_options COMMENTS STRING 
	{
		$$ = $1;
		$$->comment = $3;
		//delete [] $2;
		//cout << "Table comment" << $$->comment <<  endl;
	};
	|
	table_options CHECKSUM '0'
	{
		$$ = $1;
		$$->checksum = 0;
		//delete [] $2;
		//cout << "Table checksum" << $$->checksum <<  endl;
	};
	|
	table_options CHECKSUM '1'
	{
		$$ = $1;
		$$->checksum = 1;
		//delete [] $2;
		//cout << "Table checksum" << $$->checksum <<  endl;
	};
	|
	table_options AUTO_INCREMENT UNSIGNED_INT 
	{
		$$ = $1;
		$$->autoInc = $3;
		//delete [] $2;
		//cout << "Table auto increment" << $$->autoInc <<  endl;
	}
	|
	table_options DATA MODE FIXED 
	{
		$$ = $1;
		$$->inputDataFormat = "fixed";
	}
	|
	table_options DATA MODE CSV
	{
		$$ = $1;
		$$->inputDataFormat = "csv";
	}
	|
	table_options SOURCE TYPE HBASE DATABASE W_WORD ADDRESS STRING PORT INTNUM TABLE W_WORD RAWKEY '(' expr_list ')'
	{
		$$ = $1;
		$$->data_source = "hbase";
		$$->db_name = $6, 
		$$->db_addr = $8, 
		$$->db_port = $10, 
		$$->db_t_name = $12, 
		$$->raw_keys = $15;
	}
	;

stmt_drop_table:
	DROP TABLE stmt_table_if_exists table_name ';'
	{
		AosJqlStmtTable* statement = new AosJqlStmtTable;
		statement->setName($4);
		statement->setExists($3);
		gAosJQLParser.appendStatement(statement);
		$$ = statement;
		$$->setOp(JQLTypes::eDrop); 
	}
	;

stmt_show_tables:
	SHOW opt_full TABLES opt_from_dbname opt_like ';'
	{
		AosJqlStmtTable* statement = new AosJqlStmtTable;
		statement->setFullOption($2);
		statement->setFromOption($4);
		statement->setLikeOption($5);

		gAosJQLParser.appendStatement(statement);
		$$ = statement;
		$$->setOp(JQLTypes::eShow); 
	}
	;

opt_full:
	{
		$$ = 0;
	}
	|
	FULL
	{
		$$ = 1;
	}
	;

opt_from_dbname:
	{
		$$ = NULL;
	}
	|
	FROM W_WORD
	{
		$$ = $2;
	}
	;

opt_like:
	{
		$$ = NULL;
	}
	|
	LIKE W_WORD
	{
		$$ = $2;
	}
	;


stmt_table_if_exists:
	{
		$$ = false;
	}
	|
	IF EXISTS
	{
		$$ = true;
	}
	;

stmt_table_if_not_exists:
	{
		$$ = false;
	}
	|
	IF NOT EXISTS
	{
		$$ = true;
	}
	;

stmt_describe:
	DESCRIBE TABLE table_name ';'
	{
		AosJqlStmtTable* statement = new AosJqlStmtTable;
		statement->setName($3);
		gAosJQLParser.appendStatement(statement);
		$$ = statement;
		$$->setOp(JQLTypes::eDescribe); 
	}
	;
stmt_create_task:
	CREATE TASK W_WORD '(' stmt_expr_name_values ')' ';'
	{
		AosJqlStmtTask *stmt = new AosJqlStmtTask;
		stmt->setTaskName($3);
		stmt->setConfParms($5);
		gAosJQLParser.appendStatement(stmt);
		$$ = stmt;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	CREATE TASK W_WORD 
	INPUT DATASET '('column_list ')' 
	OUTPUT DATASET '('column_list ')' 
	MAPTASK USING SINGLE RECORD DATA ENGING PROCS '('column_list ')'
	REDUCETASK USING ACTION DATA ENGING PROCS '('column_list ')'  ';'
	{
		AosJqlStmtTask *task = new AosJqlStmtTask;
		task->setTaskName($3);
		task->setInputDataSetNames($7);
		task->setOutputDataSetNames($12);
		task->setDataProcsNames($22);
		task->setActionNames($31);
		gAosJQLParser.appendStatement(task);   
		$$ = task;                             
		$$->setOp(JQLTypes::eCreate);               
	}
    ;

stmt_drop_task:
	DROP TASK W_WORD ';'
	{
		AosJqlStmtTask *task = new AosJqlStmtTask;
		task->setTaskName($3);
		gAosJQLParser.appendStatement(task);
		$$ = task;
		$$->setOp(JQLTypes::eDrop);
	}
	;

stmt_show_task:
	SHOW TASKS ';'
	{
		AosJqlStmtTask *task = new AosJqlStmtTask;
		task->setOp(JQLTypes::eShow);
		gAosJQLParser.appendStatement(task);
		$$ = task;
	}
	;

stmt_describe_task:
	DESCRIBE TASK W_WORD ';'
	{
		AosJqlStmtTask *task = new AosJqlStmtTask;
		task->setTaskName($3);
		gAosJQLParser.appendStatement(task);
		$$ = task;
		$$->setOp(JQLTypes::eDescribe);
	}
	;

stmt_union:
	query_union ';'
	{
		$$ = $1;	
		gAosJQLParser.appendStatement($1);             
	}
	;

query_union:
	jql_select_stmt stmt_union_opt jql_select_stmt
	{
		AosJqlStmtQueryUnion *stmt_union = new AosJqlStmtQueryUnion();
		AosJqlStmtQuery *query1 = new AosJqlStmtQuery($1);
		AosJqlStmtQuery *query2 = new AosJqlStmtQuery($3);

		stmt_union->setIsUnionAll($2);
		stmt_union->addQuery(query1);
		stmt_union->addQuery(query2);

		$$ = stmt_union;   
	}
	|
	query_union stmt_union_opt jql_select_stmt 
	{
		AosJqlStmtQueryUnion *stmt_union = $1;
		AosJqlStmtQuery *query = new AosJqlStmtQuery($3);

		stmt_union->addQuery(query);
		$$ = stmt_union;   
	}
	;

stmt_union_opt:
	UNION
	{
		$$ = false;
	}
	|
	UNION ALL
	{
		$$ = true;
	}
	;

update_stmt: 
	UPDATE 
		expr	
	SET 
		update_asgn_list opt_where ';'
	{
		 AosJqlStmtUpdate *u1 = new AosJqlStmtUpdate;
		 u1->table = $2;
		 u1->update_expr_list = $4;
		 if ($5) u1->opt_where = $5->getWhereExpr().getPtr();

		 AosJqlStmtUpdateItem * update = new AosJqlStmtUpdateItem(u1);
		 gAosJQLParser.appendStatement(update);
		 $$ = update;

		 //cout << "found UPDATA table_references SET update_asgn_list " << endl;
	}
	;

update_asgn_list:
	W_WORD COMPARISON expr 
	{ 
		 if ($2 != AosExprComparison::eEqual) yyerror("bad update assignment to %s", $2);

		 $$ = new AosExprList;

		 AosExprObj *pExpr1 = new AosExprString($1);
		 AosExprObj *pExpr = $3; 
		 AosExpr	*pExpr2 = new AosExprComparison(pExpr1, AosExprComparison::eEqual, pExpr);    
		 pExpr2->setExpr(pExpr1, 1);
		 pExpr2->setExpr(pExpr, 2);

		 free($1);

		 $$->push_back(pExpr2);
	}
	/*
	|
	W_WORD '.' W_WORD COMPARISON expr 
	{ 
		 //cout << "FOUND update_asgn_list  W_WORD '.' W_WORD COMPARISON expr " << endl;
	}
	*/
	|
	update_asgn_list ',' W_WORD COMPARISON expr
	{ 
		 if ($4 != AosExprComparison::eEqual) yyerror("bad update assignment to %s", $4);

		 AosExprObj *pExpr1 = new AosExprString($3);
		 AosExprObj *pExpr = $5; 
		 AosExpr	*pExpr2 = new AosExprComparison(pExpr1, AosExprComparison::eEqual, pExpr);    
		 pExpr2->setExpr(pExpr1, 1);
		 pExpr2->setExpr(pExpr, 2);
		 free($3);
		 $$->push_back(pExpr2);
	}
	/*
	|
	update_asgn_list ',' W_WORD '.' W_WORD COMPARISON expr
	{ 
		 //cout << "FOUND update_asgn_list ',' W_WORD '.' W_WORD COMPARISON expr" << endl;
	}
	*/
	;

stmt_create_user:
	CREATE USER STRING IDENTIFIED BY STRING ';'
	{
		AosJqlStmtUserMgr *user_mgr = new AosJqlStmtUserMgr;
		user_mgr->setUserName($3);
		user_mgr->setPwd($6);
		gAosJQLParser.appendStatement(user_mgr);
		$$ = user_mgr;
		$$->setOp(JQLTypes::eCreate);
	}
    ;


stmt_drop_user:
	DROP USER W_WORD ';'
	{
		AosJqlStmtUserMgr *user_mgr = new AosJqlStmtUserMgr;
		user_mgr->setUserName($3);
		gAosJQLParser.appendStatement(user_mgr);
		$$ = user_mgr;
		$$->setOp(JQLTypes::eDrop);
	}
	;

stmt_show_user:
	SHOW USERS ';'
	{
		AosJqlStmtUserMgr *user_mgr = new AosJqlStmtUserMgr;
		gAosJQLParser.appendStatement(user_mgr);
		$$ = user_mgr;
		$$->setOp(JQLTypes::eShow);
	}
	;

stmt_describe_user:
	DESCRIBE USER W_WORD ';'
	{
		AosJqlStmtUserMgr *user_mgr = new AosJqlStmtUserMgr;
		user_mgr->setUserName($3);
		gAosJQLParser.appendStatement(user_mgr);
		$$ = user_mgr;
		$$->setOp(JQLTypes::eDescribe);
	}
	;

stmt_alter_user:
	ALTER USER STRING IDENTIFIED BY STRING ';'
	{
		AosJqlStmtUserMgr *user_mgr = new AosJqlStmtUserMgr;
		user_mgr->setUserName($3);
		user_mgr->setPwd($6);
		gAosJQLParser.appendStatement(user_mgr);
		$$ = user_mgr;
		$$->setOp(JQLTypes::eAlter);	
	}
	;

stmt_create_virtual_field:
	CREATE VIRTUAL FIELD W_WORD
		stmt_infofield
		TYPE STRING
		stmt_data_type
		data_field_max_len 
		data_field_offset
		data_field_default_value
		ON W_WORD 
		USING MAP W_WORD '(' expr_list ')' ';'
	{
		AosJqlStmtVirtualField *virtual_field = new AosJqlStmtVirtualField;
		virtual_field->setName($4);
		virtual_field->setIsInfoField($5);
		virtual_field->setType($7);
        virtual_field->setDataType($8);
		virtual_field->setMaxLen($9);
		virtual_field->setOffset($10);
		virtual_field->setDefault($11);
		virtual_field->setTable($13);
		virtual_field->setMapName($16);
		virtual_field->setFields($18);
		gAosJQLParser.appendStatement(virtual_field);
		$$ = virtual_field;
		$$->setOp(JQLTypes::eCreate);
	}
    ;

stmt_create_virtual_field_expr:
    CREATE VIRTUAL FIELD W_WORD
        TYPE STRING                                                    
		stmt_data_type
        data_field_max_len
        data_field_offset
        EXPRESSION expr
        ON W_WORD ';'
    {
        AosJqlStmtFieldExpr *stmt = new AosJqlStmtFieldExpr;
        stmt->setName($4);
        stmt->setType($6);
        stmt->setDataType($7);
        stmt->setMaxLen($8);
        stmt->setOffset($9);
        stmt->setExpr($11);
        stmt->setTable($13);
        gAosJQLParser.appendStatement(stmt);
        $$ = stmt;
        $$->setOp(JQLTypes::eCreate);
    };


stmt_drop_virtual_field:
	DROP VIRTUAL FIELD W_WORD ';'
	{
		AosJqlStmtVirtualField *virtual_field = new AosJqlStmtVirtualField;
		virtual_field->setName($4);
		gAosJQLParser.appendStatement(virtual_field);
		$$ = virtual_field;
		$$->setOp(JQLTypes::eDrop);
	}
	;

stmt_show_virtual_field:
	SHOW VIRTUAL FIELDS ';'
	{
		AosJqlStmtVirtualField *virtual_field = new AosJqlStmtVirtualField;
		gAosJQLParser.appendStatement(virtual_field);
		$$ = virtual_field;
		$$->setOp(JQLTypes::eShow);
	}
	;

stmt_describe_virtual_field:
	DESCRIBE VIRTUAL FIELD W_WORD ';'
	{
		AosJqlStmtVirtualField *virtual_field = new AosJqlStmtVirtualField;
		virtual_field->setName($4);
		gAosJQLParser.appendStatement(virtual_field);
		$$ = virtual_field;
		$$->setOp(JQLTypes::eDescribe);
	}
	;

stmt_data_type:
	{
		$$ = NULL;
	}
	|
	DATA TYPE data_field_data_type
	{
		$$ = $3;
	}
	;

stmt_infofield:
	{
		$$ = false;
	}
	|
	INFOFIELD
	{
		$$ = true;
	};
W_WORD:
	NAME 
	{
		$$ = strdup($1);
	}
	|
	INTNUM
	{
		$$ = strdup(yytext);
	}
	|
	U8VAR
	{
		$$ = strdup(yytext);
	} 
	|
	STRING
	{
		$$ = strdup(yytext);
	}
	|
	UNSIGNED_LONG
	{
		$$ = strdup(yytext);
	}
	|
	UNSIGNED_INT
	{
		$$ = strdup(yytext);
	} 
	|
	DOUBLE_NUM
	{
		$$ = strdup(yytext);
	}
	|
	DOCBEGIN
	{
		$$ = strdup(yytext);
	} 
	|
	DOCEND
	{
		$$ = strdup(yytext);
	} 
	|
	XMLDOC
	{
		$$ = strdup(yytext);
	}
	|
	DOC
	{
		$$ = strdup(yytext);
	} 
	|
	LOGIN
	{
		$$ = strdup(yytext);
	} 
	|
	LOGOUT
	{
		$$ = strdup(yytext);
	}
	|
	RETRIEVE
	{
		$$ = strdup(yytext);
	} 
	|
	START
	{
		$$ = strdup(yytext);
	} 
	|
	STOP
	{
		$$ = strdup(yytext);
	} 
	|
	CHECK
	{
		$$ = strdup(yytext);
	} 
	|
	RESTART
	{
		$$ = strdup(yytext);
	}
	|
	OVERRIDE
	{
		$$ = strdup(yytext);
	} 
	|
	BY
	{
		$$ = strdup(yytext);
	} 
	|
	ANDOP
	{
		$$ = strdup(yytext);
	} 
	|
	IGNORE
	{
		$$ = strdup(yytext);
	} 
	|
	LOW_PRIORITY
	{
		$$ = strdup(yytext);
	} 
	|
	QUICK
	{
		$$ = strdup(yytext);
	} 
	|
	SET
	{
		$$ = strdup(yytext);
	} 
	|
	KEY
	{
		$$ = strdup(yytext);
	} 
	|
	WAIT
	{
		$$ = strdup(yytext);
	}
	|
	DOES
	{
		$$ = strdup(yytext);
	}
	|
	ASSIGNMENT
	{
		$$ = strdup(yytext);
	}
	|
	DELAYED
	{
		$$ = strdup(yytext);
	}
	|
	VALUES
	{
		$$ = strdup(yytext);
	}
	|
	COMPLETE
	{
		$$ = strdup(yytext);
	}	
	|
	LEFTSHIFT 
	{
		$$ = strdup(yytext);
	}
	|
	ROLLUP
	{
		$$ = strdup(yytext);
	}
	|
	RIGHTSHIFT
	{
		$$ = strdup(yytext);
	}
	|
	RECORDS
	{
		$$ = strdup(yytext);
	}
	|
	PICKER
	{
		$$ = strdup(yytext);
	}
	|
	PICKERS
	{
		$$ = strdup(yytext);
	}
	|
	JIMOID
	{
		$$ = strdup(yytext);
	}
	|
	AS
	{
		$$ = strdup(yytext);
	}
	|
	END
	{
		$$ = strdup(yytext);
	}
	|
	PROC
	{
		$$ = strdup(yytext);
	}
	|
	PROCS
	{
		$$ = strdup(yytext);
	}
	|
	DEBUG
	{
		$$ = strdup(yytext);
	}
	|
	DATA
	{
		$$ = strdup(yytext);
	}
	|
	SCANNER
	{
		$$ = strdup(yytext);
	}
	|
	SCANNERS
	{
		$$ = strdup(yytext);
	}
	|
	DATASET
	{
		$$ = strdup(yytext);
	}
	|
	DATASETS
	{
		$$ = strdup(yytext);
	}
	|
	DATABASE
	{
		$$ = strdup(yytext);
	}
	|
	DATABASES
	{
		$$ = strdup(yytext);
	}
	|
	DATE
	{
		$$ = strdup(yytext);
	}
	|
	SCHEMA
	{
		$$ = strdup(yytext);
	}
	|
	SCHEMAS
	{
		$$ = strdup(yytext);
	}
	|
	CONNECTOR
	{
		$$ = strdup(yytext);
	}
	|
	CONNECTORS
	{
		$$ = strdup(yytext);
	}
	|
	CUBE
	{
		$$ = strdup(yytext);
	}
	|
	FILTER
	{
		$$ = strdup(yytext);
	}
	|
	FILTERED
	{
		$$ = strdup(yytext);
	}
	|
	ACCEPTED
	{
		$$ = strdup(yytext);
	}
	|
	MATCHED
	{
		$$ = strdup(yytext);
	}
	|
	MISMATCHED
	{
		$$ = strdup(yytext);
	}
	|
	DROPPED
	{
		$$ = strdup(yytext);
	}
	|
	MULTIPLE
	{
		$$ = strdup(yytext);
	}
	|
	DEFAULT
	{
		$$ = strdup(yytext);
	}
	|
	VALUE
	{
		$$ = strdup(yytext);
	}
	|
	SELECTOR
	{
		$$ = strdup(yytext);
	}
	|
	SELECTORS
	{
		$$ = strdup(yytext);
	}
	|
	INNER
	{
		$$ = strdup(yytext);
	}
	|
	FULL
	{
		$$ = strdup(yytext);
	} 
	|
	OUTER
	{
		$$ = strdup(yytext);
	}
	|
	LEFT
	{
		$$ = strdup(yytext);
	}
	|
	RIGHT
	{
		$$ = strdup(yytext);
	}
	|
	EXCLUSIVE
	{
		$$ = strdup(yytext);
	}
	|
	XO
	{
		$$ = strdup(yytext);
	}
	|
	MOD
	{
		$$ = strdup(yytext);
	}
	|
	MODE
	{
		$$ = strdup(yytext);
	}
	|
	BINARY
	{
		$$ = strdup(yytext);
	}
	|
	BOTH
	{
		$$ = strdup(yytext);
	}
	|
	CASE
	{
		$$ = strdup(yytext);
	}
	|
	COMPARISON
	{
		$$ = strdup(yytext);
	}
	|
	LOGIC
	{
		$$ = strdup(yytext);
	}
	|
	CURRENT_DATE
	{
		$$ = strdup(yytext);
	}
	|
	CURRENT_TIME
	{
		$$ = strdup(yytext);
	}
	|
	CURRENT_TIMESTAMP
	{
		$$ = strdup(yytext);
	}
	|
	DAY_HOUR
	{
		$$ = strdup(yytext);
	}
	|
	DAY_MICROSECOND
	{
		$$ = strdup(yytext);
	}
	|
	DAY_MINUTE
	{
		$$ = strdup(yytext);
	}
	|
	DAY_SECOND
	{
		$$ = strdup(yytext);
	}
	|
	FCOUNT
	{
		$$ = strdup(yytext);
	}
	|
	FDATE_ADD
	{
		$$ = strdup(yytext);
	}
	|
	FDATE_SUB
	{
		$$ = strdup(yytext);
	}
	|
	FSUBSTRING
	{
		$$ = strdup(yytext);
	}
	|
	FTRIM
	{
		$$ = strdup(yytext);
	}
	|
	HOUR_MICROSECOND
	{
		$$ = strdup(yytext);
	}
	|
	HOUR_MINUTE
	{
		$$ = strdup(yytext);
	}
	|
	HOUR_SECOND
	{
		$$ = strdup(yytext);
	}
	|
	IN
	{
		$$ = strdup(yytext);
	}
	|
	INTERVAL
	{
		$$ = strdup(yytext);
	}
	|
	IS
	{
		$$ = strdup(yytext);
	}
	|
	LIKE
	{
		$$ = strdup(yytext);
	}
	|
	NULLX
	{
		$$ = strdup(yytext);
	}
	|
	NUMBER
	{
		$$ = strdup(yytext);
	} 
	|
	REGEXP
	{
		$$ = strdup(yytext);
	}
	|
	SHIFT
	{
		$$ = strdup(yytext);
	}
	|
	SOME
	{
		$$ = strdup(yytext);
	}
	|
	TRIM
	{
		$$ = strdup(yytext);
	}
	|
	TRAILING
	{
		$$ = strdup(yytext);
	}
	|
	YEAR
	{
		$$ = strdup(yytext);
	}
	|
	YEAR_MONTH
	{
		$$ = strdup(yytext);
	}
	|
	LEADING
	{
		$$ = strdup(yytext);
	}
	|
	TABLE
	{
		$$ = strdup(yytext);
	} 
	|
	TABLES
	{
		$$ = strdup(yytext);
	}
	|
	RECORD
	{
		$$ = strdup(yytext);
	}
	|
	INDEX
	{
		$$ = strdup(yytext);
	}
	|
	INDEXES
	{
		$$ = strdup(yytext);
	}
	|
	AUTO_INCREMENT
	{
		$$ = strdup(yytext);
	}
	|
	CHECKSUM
	{
		$$ = strdup(yytext);
	}
	|
	ASC
	{
		$$ = strdup(yytext);
	}
	|
	DESC
	{
		$$ = strdup(yytext);
	}
	|
	COMMENTS
	{
		$$ = strdup(yytext);
	}
	|
	CONFIG
	{
		$$ = strdup(yytext);
	} 
	|
	BLOCKSIZE
	{
		$$ = strdup(yytext);
	}
	|
	UNSIGNED
	{
		$$ = strdup(yytext);
	}
	|
	BIT
	{
		$$ = strdup(yytext);
	}
	|
	INT
	{
		$$ = strdup(yytext);
	} 
	|
	STR
	{
		$$ = strdup(yytext);
	} 
	|
	TEXT
	{
		$$ = strdup(yytext);
	}
	|
	ON
	{
		$$ = strdup(yytext);
	}
	|
	OFF
	{
		$$ = strdup(yytext);
	}
	|
	SUPPRESS
	{
		$$ = strdup(yytext);
	}
	|
	DIR
	{
		$$ = strdup(yytext);
	} 
	|
	VARIABLE
	{
		$$ = strdup(yytext);
	} 
	|
	FIELD
	{
		$$ = strdup(yytext);
	} 
	|
	FIELDS
	{
		$$ = strdup(yytext);
	} 
	|
	DELIMITER
	{
		$$ = strdup(yytext);
	} 
	|
	QUALIFIER
	{
		$$ = strdup(yytext);
	} 
	|
	TASKS
	{
		$$ = strdup(yytext);
	}
	|
	JOB
	{
		$$ = strdup(yytext);
	}
	|
	JOBS
	{
		$$ = strdup(yytext);
	}
	|
	TIMES
	{
		$$ = strdup(yytext);
	}
	|
	TIME
	{
		$$ = strdup(yytext);
	}
	|
	PRIORITY
	{
		$$ = strdup(yytext);
	}
	|
	AT
	{
		$$ = strdup(yytext);
	} 
	|
	ACTOR
	{
		$$ = strdup(yytext);
	}
	|
	JAVA
	{
		$$ = strdup(yytext);
	} 
	|
	ACTION
	{
		$$ = strdup(yytext);
	}
	|
	ENGING
	{
		$$ = strdup(yytext);
	}
	|
	INPUT
	{
		$$ = strdup(yytext);
	}
	|
	MAPPING
	{
		$$ = strdup(yytext);
	}
	|
	MAPTASK
	{
		$$ = strdup(yytext);
	}
	|
	OUTPUT
	{
		$$ = strdup(yytext);
	}
	|
	REDUCETASK
	{
		$$ = strdup(yytext);
	}
	|
	SINGLE
	{
		$$ = strdup(yytext);
	}
	|
	TASK
	{
		$$ = strdup(yytext);
	}
	|
	SCHEDULE
	{
		$$ = strdup(yytext);
	}
	|
	OFFSET
	{
		$$ = strdup(yytext);
	}
	|
	EXPRESSION
	{
		$$ = strdup(yytext);
	}
	|
	FORM
	{
		$$ = strdup(yytext);
	}
	|
	FORMATTER
	{
		$$ = strdup(yytext);
	}
	|
	LENGTH
	{
		$$ = strdup(yytext);
	}
	|
	MAX
	{
		$$ = strdup(yytext);
	}
	|
	POS
	{
		$$ = strdup(yytext);
	}
	|
	TO
	{
		$$ = strdup(yytext);
	} 
	|
	PRINT
	{
		$$ = strdup(yytext);
	}
	|
	CODING
	{
		$$ = strdup(yytext);
	}
	|
	COMBINE
	{
		$$ = strdup(yytext);
	}
	|
	COMBINATIONS
	{
		$$ = strdup(yytext);
	}
	|
	FILENAME
	{
		$$ = strdup(yytext);
	}
	|
	ID
	{
		$$ = strdup(yytext);
	}
	|
	SERVER
	{
		$$ = strdup(yytext);
	}
	|
	SERVICE
	{
		$$ = strdup(yytext);
	}
	|
	SERVICES
	{
		$$ = strdup(yytext);
	}
	|
	SIZE
	{
		$$ = strdup(yytext);
	}
	|
	SPLIT
	{
		$$ = strdup(yytext);
	}
	|
	IILNAME
	{
		$$ = strdup(yytext);
	}
	|
	IIL
	{
		$$ = strdup(yytext);
	}
	|
	OPERATOR
	{
		$$ = strdup(yytext);
	}
	|
	REDUCE
	{
		$$ = strdup(yytext);
	}
	|
	STINCT
	{
		$$ = strdup(yytext);
	}
	|
	DISTINCTROW
	{
		$$ = strdup(yytext);
	}
	|
	HIGH_PRIORITY
	{
		$$ = strdup(yytext);
	}
	|
	SQL_BIG_RESULT
	{
		$$ = strdup(yytext);
	}
	|
	SQL_CALC_FOUND_ROWS
	{
		$$ = strdup(yytext);
	}
	|
	SQL_SMALL_RESULT
	{
		$$ = strdup(yytext);
	}
	|
	STRAIGHT_JOIN
	{
		$$ = strdup(yytext);
	}          
	|
	UNIFORM
	{
		$$ = strdup(yytext);
	} 
	|
	PATH
	{
		$$ = strdup(yytext);
	}
	|
	ENCODING
	{
		$$ = strdup(yytext);
	} 
	|
	STATIC
	{
		$$ = strdup(yytext);
	}
	|
	DYNAMIC
	{
		$$ = strdup(yytext);
	}
	|
	ROW
	{
		$$ = strdup(yytext);
	}
	|
	PICKED
	{
		$$ = strdup(yytext);
	}
	|
	BYTE
	{
		$$ = strdup(yytext);
	}
	|
	BYTES
	{
		$$ = strdup(yytext);
	}
	|
	IDFAMILY
	{
		$$ = strdup(yytext);
	}
	|
	LOAD
	{
		$$ = strdup(yytext);
	}
	|
	LOG
	{
		$$ = strdup(yytext);
	}
	|
	IDENTIFIED
	{
		$$ = strdup(yytext);
	} 
	|
	USER
	{
		$$ = strdup(yytext);
	}
	|
	USERS
	{
		$$ = strdup(yytext);
	}
	|
	SOURCE
	{
		$$ = strdup(yytext);
	}
	|
	DUAL
	{
		$$ = strdup(yytext);
	}
	|
	JIMODB
	{
		$$ = strdup(yytext);
	} 
	|
	DIRECTORY
	{
		$$ = strdup(yytext);
	} 
	|
	STATISTICS
	{
		$$ = strdup(yytext);
	} 
	|
	STAT
	{
		$$ = strdup(yytext);
	}
	|
	UNIT
	{
		$$ = strdup(yytext);
	} 
	|
	OUTFILE
	{
		$$ = strdup(yytext);
	}
	|
	CSV
	{
		$$ = strdup(yytext);
	} 
	|
	FIXED
	{
		$$ = strdup(yytext);
	} 
	|
	DIMENSIONS
	{
		$$ = strdup(yytext);
	} 
	|
	HBASE
	{
		$$ = strdup(yytext);
	}
	|
	RAWKEY
	{
		$$ = strdup(yytext);
	}
	|
	ADDRESS
	{
		$$ = strdup(yytext);
	} 
	|
	PORT
	{
		$$ = strdup(yytext);
	} 
	|
	THREAD
	{
		$$ = strdup(yytext);
	}
	|
	MEASURES
	{
		$$ = strdup(yytext);
	} 
	|
	MODEL
	{
		$$ = strdup(yytext);
	} 
	|
	PARSER
	{
		$$ = strdup(yytext);
	} 
	|
	MAP
	{
		$$ = strdup(yytext);
	} 
	|
	DISTRIBUTIONMAP
	{
		$$ = strdup(yytext);
	} 
	|
	DISTRIBUTIONMAPS
	{
		$$ = strdup(yytext);
	}
	|
	MAPS
	{
		$$ = strdup(yytext);
	} 
	|
	VIRTUAL
	{
		$$ = strdup(yytext);
	} 
	|
	NICK
	{
		$$ = strdup(yytext);
	} 
	|
	CONTAIN
	{
		$$ = strdup(yytext);
	} 
	|
	JIMOLOGIC
	{
		$$ = strdup(yytext);
	} 
	|
	CHECKPOINT
	{
		$$ = strdup(yytext);
	} 
	|
	ENDPOINT
	{
		$$ = strdup(yytext);
	} 
	|
	MATRIX
	{
		$$ = strdup(yytext);
	} 
	|
	RESULTS
	{
		$$ = strdup(yytext);
	} 
	|
	EXPORT
	{
		$$ = strdup(yytext);
	} 
	|
	XPATH
	{
		$$ = strdup(yytext);
	}
	|
	BETWEEN
	{
		$$ = strdup(yytext);
	}
	|
	TERMINATED
	{
		$$ = strdup(yytext);
	} 
	|
	ENCLOSED
	{
		$$ = strdup(yytext);
	} 
	|
	ESCAPED
	{
		$$ = strdup(yytext);
	} 
	|
	LINES
	{
		$$ = strdup(yytext);
	}  
	|
	STARTING
	{
		$$ = strdup(yytext);
	} 
	|
	SKIP
	{
		$$ = strdup(yytext);
	} 
	|
	INVALID
	{
		$$ = strdup(yytext);
	} 
	|
	SUB
	{
		$$ = strdup(yytext);
	} 
	|
	SYSTEM_ARGS
	{
		$$ = strdup(yytext);
	} 
	|
	LEFT_BRACKET
	{
		$$ = strdup(yytext);
	}
	|
	RIGHT_BRACKET
	{
		$$ = strdup(yytext);
	}
	|
	CACHE
	{
		$$ = strdup(yytext);
	} 
	|
	CYCLE
	{
		$$ = strdup(yytext);
	} 
	|
	INCREMENT
	{
		$$ = strdup(yytext);
	} 
	|
	MAXVALUE
	{
		$$ = strdup(yytext);
	} 
	|
	MINVALUE
	{
		$$ = strdup(yytext);
	} 
	|
	NOCACHE
	{
		$$ = strdup(yytext);
	} 
	|
	NOCYCLE
	{
		$$ = strdup(yytext);
	} 
	|
	NOMAXVALUE
	{
		$$ = strdup(yytext);
	} 
	|
	NOMINVALUE
	{
		$$ = strdup(yytext);
	} 
	|
	NOORDER
	{
		$$ = strdup(yytext);
	} 
	|
	SEQUENCE
	{
		$$ = strdup(yytext);
	}
	|
	STATUS
	{
		$$ = strdup(yytext);
	} 
	|
	DICTIONARY	
	{
		$$ = strdup(yytext);
	} 
	;
%%
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

