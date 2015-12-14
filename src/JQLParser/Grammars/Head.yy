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


