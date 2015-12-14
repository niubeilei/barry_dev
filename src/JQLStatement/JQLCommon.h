////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//      
// Description: just for test
//
// Modification History:
// 01/04/2013 Created by Kerong.Li
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JQLStatement_JQLCommon_h
#define Aos_JQLStatement_JQLCommon_h

#include "JQLStatement/JqlDataStruct.h"
#include "JQLStatement/Ptrs.h"
#include "JQLExpr/Expr.h"

#include "SEInterfaces/SqlTableObj.h"
#include "SEInterfaces/ExprObj.h"
#include "Util/Number.h"
#include "Util/DataTypes.h"
#include "Util/ValueRslt.h"
#include "Util/String.h"
#include <vector>
using namespace std;

class AosJqlExpr;

struct AosJQLTableReference;
typedef vector<AosJQLTableReference> AosJQLTableReferenceList, AosJQLTableFactorByReference;
typedef vector<OmnString> AosStringListVec, *AosPStringList;

enum AosJQLStmtType
{
	eStmtType_Invalid = 0,
	
	//insert
	eStmtType_InsertDataByValues,  //insert into tbl_name () values ()
	eStmtType_InsertDataBySet,     //insert into tbl_name () set ...
	eStmtType_InsertDataBySelect,  //insert into tbl_name () select ...

    //delete
	eStmtType_DeleteDataFromTableName,
	eStmtType_DeleteDataFromTableReferences,
	eStmtType_DeleteDataUsingTableReferences,
	
	//select
    eStmtType_SelectDataNoFrom,
    eStmtType_SelectDataByFrom,

    //replace
	eStmtType_ReplaceDataByValues,
	eStmtType_ReplaceDataBySet,
	eStmtType_ReplaceDataBySelect,

    //update
	eStmtType_UpdateData,
	
	//create database
	eStmtType_CreateDataBase,
	eStmtType_CreateSchema,
	
	//set var
	eStmtType_SetVar,
	
	//create table
	eStmtType_CreateTableByColumnList,
	eStmtType_CreateTableByColumnListSelectStmt,
	eStmtType_CreateTableBySelectStmt,
	
	eStmtType_Max,
};




enum AosJQLStmtOperOption
{
	eStmtOper_Invalid = 0,
	
	eStmtOper_LowPriority,
	eStmtOper_HighPriority,
	eStmtOper_Delayed,
	eStmtOper_Ignore,
	eStmtOper_Quick,

	eStmtOper_Max,
};

enum AosJQLStmtSelectOption
{
	eSelectOpt_Invalid = 0,
	
	eSelectOpt_SelectAll,
	eSelectOpt_Distinct,
	eSelectOpt_DistinctRow,
	eSelectOpt_HighPriority,
	eSelectOpt_StraightJoin,
	eSelectOpt_SmallResult,
	eSelectOpt_BigResult,
	eSelectOpt_CallFoundRows,

	eSelectOpt_Max,
};

enum AosJQLIndexType
{
	eIndex_Invalid = 0,
	
	eIndex_UseIndex,
	eIndex_IgnoreIndex,
	eIndex_ForceIndex,
		
	eIndex_TypeMax,
};

enum AosJQLRegionType
{
	eRegion_Invalid = 0,
	
	eRegion_No,	
	eRegion_Any,
	eRegion_Some,
	eRegion_All,
	
	eRegion_Max,
};

enum AosJQLOperType
{
	eOper_Invalid = 0,
	
	eOper_No,
	eOper_Default,
	eOper_All,
	eOper_Dot,
	eOper_Add,
	eOper_Sub,
	eOper_Mul,
	eOper_Div,
	eOper_Mod,
	eOper_Neg,
	eOper_And,
	eOper_Or,
	eOper_Xor,
	eOper_CMPEqual,
	eOper_CMPNotEqual,
	eOper_CMPLess,
	eOper_CMPLarger,
	eOper_CMPLessEqual,
	eOper_CMPLargerEqual,
	eOper_BitOr,
	eOper_BitAnd,
	eOper_BitXor,
	eOper_LeftShift,
	eOper_RightShift,
	eOper_Not,
	eOper_Assign,
	eOper_IsNull,
	eOper_IsNotNull,
	eOper_IsBool,
	eOper_IsNotBool,
	eOper_BetweenAnd,
	eOper_InValList,
	eOper_NotInValList,
	eOper_InValListList,
	eOper_NotInValListList,
	eOper_OperType_CheckExists,
	
	//function call
	eOper_Call,
	eOper_CountAll,
	eOper_CountExpr,
	eOper_FSubStringByValList,
	eOper_FSubStringByExprFromExpr,
	eOper_FSubStringByExprFromExprForExpr,
	eOper_FTRIMByValList,

    //trim
	eOper_TrimLeading,
	eOper_TrimTrailing,
	eOper_TrimBoth,
	
	//date
	eOper_FDateAdd,
	eOper_FDateSub,
	eOper_IntervalYear,
	eOper_IntervalYearMonth,
	eOper_IntervalDayHour,
	eOper_IntervalDayMinute,
	eOper_IntervalDaySecond,
	eOper_IntervalDayMicroSecond,
	eOper_IntervalHourMinute,
	eOper_IntervalHourSecond,
	eOper_IntervalHourMicroSecond,
	
	eOper_Like,
	eOper_NotLike,
	eOper_Regexp,
	eOper_NotRegexp,
	eOper_CurrentTimeStamp,
	eOper_CurrentDate,
	eOper_CurrentTime,
	eOper_BinaryPrecUminus,
	eOper_WhenThen,
	eOper_CaseExprExprListEnd,
	eOper_CaseExprExprListElseEnd,
	eOper_CaseExprListEnd,
	eOper_CaseExprListElseEnd,
	eOper_CheckExists,
	
	eOper_Max,
};

enum AosJQLJoinType
{
	eJoin_Invalid = 0,
	
	eJoin_No,	
	eJoin_InnerJoin,
	eJoin_CrossJoin,
	eJoin_StraightJoin, //need use this
	eJoin_RightJoin,
	eJoin_LeftJoin,
	eJoin_RightOuterJoin,
	eJoin_LeftOuterJoin,
	
	eJoin_Max,
};

enum AosJQLCheckExists
{
	eCheckExists_Invalid = 0,
	
	eCheckExists_NoCheck,
	eCheckExists_NotExists,
	eCheckExists_Exists,
	
	eCheckExists_Max,
};

enum AosJQLColumnAttType
{
	eColumnAtt_Invalid = 0,
	
	eColumnAtt_None,
	eColumnAtt_Null,
	eColumnAtt_NotNull,
	eColumnAtt_DefaultString,
	eColumnAtt_DefaultIntNum,
	eColumnAtt_DefaultApproxNum,
	eColumnAtt_DefaultBool,
	eColumnAtt_AutoIncrement,
	eColumnAtt_UniqueColumnList,
	eColumnAtt_UniqueKey,
	eColumnAtt_PrimaryKey,
	eColumnAtt_Key,
	eColumnAtt_CommentString,
	
	eColumnAtt_Max,
};

enum AosJQLUnSignedZeroFillType
{
	eUZ_Invalid = 0,
	
	eUZ_None = 0,
	eUZ_UnSigned,
	eUZ_ZeroFill,
	eUZ_UnSignedAndZeroFill,

	eUZ_Max,
};

enum AosJQLSetOrCollateType
{
	eCSC_Invalid = 0,
	
	eCSC_None,
	eCSC_Set,
	eCSC_Collate,
	
	eCSC_Max,
};

struct AosJQLSetOrCollate
{
	AosJQLSetOrCollateType type;
	OmnString              value;
};
typedef vector<AosJQLSetOrCollate >  AosJQLSetOrCollateList;

typedef vector<AosJQLStmtOperOption> AosJQLStmtInsertOptionList, AosJQLStmtReplaceOptionList, AosJQLStmtUpdateOptionList, AosJQLStmtDeleteOptionList;
typedef vector<AosJQLStmtSelectOption> AosJQLStmtSelectOptionList;


//TableReference
enum AosJQLTableReferenceType
{
	eTRef_Invalid = 0,
	
	//table_factor
	eTRef_TableFactor,
	eTRef_TableFactorByTable,
	eTRef_TableFactorBySubQuery,
	eTRef_TableFactorByReference,

    //join_table
    eTRef_JoinTable,
	eTRef_JoinTableFactorOptcondition,
	eTRef_JoinTableFactorStraight,
	eTRef_JoinTableFactorExpr,
	eTRef_JoinTableFactorCondition,
	eTRef_JoinTableFactorNatural,
	
	eTRef_Max,
};

enum AosJQLJoinConditionType
{
	eJoinCondition_Invalid = 0,
	
	eJoinCondition_Expr,
	eJoinCondition_ColumnList,
	
	eJoinCondition_Max,
};

struct AosJQLJoinCondition
{
	AosJQLJoinConditionType type;
	AosStringListVec  column_list;
};

struct AosJQLIndexHint
{
	AosJQLIndexHint() {type = eIndex_Invalid; forjoin = false; };
	AosJQLIndexType   type;
	bool              forjoin;
	AosStringListVec     index_list;	
};

//factor
struct AosJQLTableFactorByTable
{
	OmnString  name;
	OmnString  alias;
	AosJQLIndexHint index_hint;
};

struct AosJQLTableFactorBySubQuery
{
	OmnString name;
};

struct AosJQLTableFactor
{
	AosJQLTableReferenceType      type;
	AosJQLTableFactorByTable      table_factor_by_table;
	AosJQLTableFactorBySubQuery   table_factor_by_subQuery;
	AosJQLTableFactorByReference  *pTable_factor_by_reference_list;
};

//join table
struct AosJQLJoinTableFactorOptcondition
{
	AosJQLJoinType       join_type;
    AosJQLTableReference *pTableReference;
    AosJQLTableFactor    *pFactor;
    AosJQLJoinCondition  *pJoinCondition;
};

struct AosJQLJoinTableFactorStraight
{
	AosJQLJoinType       join_type;
    AosJQLTableReference *pTableReference;
    AosJQLTableFactor    *pFactor;
};

struct AosJQLJoinTableFactorExpr
{
	AosJQLJoinType       join_type;
    AosJQLTableReference *pTableReference;
    AosJQLTableFactor    *pFactor;
};

struct AosJQLJoinTableFactorCondition
{
	AosJQLJoinType       join_type;
    AosJQLTableReference *pTableReference;
    AosJQLTableFactor    *pFactor;
    AosJQLJoinCondition  *pJoinCondition;// this no null
};

struct AosJQLJoinTableFactorNatural
{
	AosJQLJoinType       join_type;
    AosJQLTableReference *pTableReference;
    AosJQLTableFactor    *pFactor;
};

struct AosJQLJoinTable
{
	AosJQLTableReferenceType           type;
	AosJQLJoinTableFactorOptcondition  join_table_factor_Optcondition;
	AosJQLJoinTableFactorStraight      join_table_factor_straight;
	AosJQLJoinTableFactorExpr          join_table_factor_expr;
	AosJQLJoinTableFactorCondition     join_table_factor_condition;
	AosJQLJoinTableFactorNatural       join_table_factor_natural;
};


struct AosJQLTableReference
{
	AosJQLTableReferenceType type;
	
	//factor
	AosJQLTableFactor        table_factor;

	//join_table
	AosJQLJoinTable          join_table;
};


//Group by
struct AosJQLGroupByExpr
{
	bool           desc;
};

typedef vector<AosJQLGroupByExpr> AosJQLGroupByExprList, AosJQLOrderByExprList;

struct AosJQLGroupBy
{
	AosJQLGroupBy() { with_rollup = false; };
	AosJQLGroupByExprList  groupby_list;
	bool                   with_rollup;
};
  


enum AosJQLDataTypeDefine
{
	eDataType_Invalid = 0,
	
	eDataType_None,
	eDataType_Bit,
	eDataType_TinyInt,
	eDataType_SmallInt,
	eDataType_MediumInt,
	eDataType_Int,
	eDataType_Interger,
	eDataType_BigInt,
	eDataType_Real,
	eDataType_Double,
	eDataType_Float,
	eDataType_Decimal,
	eDataType_Date,
	eDataType_Time,
	eDataType_TimeStamp,
	eDataType_DateTime,
	eDataType_Year,
	eDataType_Char,
	eDataType_VarChar,
	eDataType_Binary,
	eDataType_VarBinary,
	eDataType_TinyBlob,
	eDataType_Blob,
	eDataType_MediumBlob,
	eDataType_LongBlob,
	eDataType_TinyText,
	eDataType_Text,
	eDataType_MediumText,
	eDataType_LongText,
	eDataType_Enum,
	eDataType_Set,
	  
	eDataType_MAX,
};

struct AosJQLData
{
	AosJQLData()
	{
		type = eDataType_Invalid;
		int_num = 0;
		opt_length = 0;
		opt_uz = eUZ_Invalid;
		opt_binary = 0;
	}
	
    AosJQLDataTypeDefine type;
    
    //data
    int  int_num;
    int opt_length;
    AosJQLUnSignedZeroFillType opt_uz;
    int  opt_binary;
    AosJQLSetOrCollateList opt_csc_list;
    AosStringListVec enum_list;
    
};
typedef vector<AosJQLData >  AosJQLDataList;



struct AosJQLColumnAtt
{
	AosJQLColumnAttType type;
	AosValueRslt        value;
	AosStringListVec       column_list;
};

typedef vector<AosJQLColumnAtt >  AosJQLColumnAttList;

//create table column definition
enum AosJQLTableColumnType
{
	eColumnType_Invalid = 0,
	
	eColumnType_None,	
	eColumnType_PrimaryKey,
	eColumnType_Key,
	eColumnType_Index,
	eColumnType_FullTextIndex,
	eColumnType_FullTextKey,
		
	eColumnType_Max,
};

struct AosJQLTableColumn
{
    AosJQLTableColumnType type;
    
    //data
    OmnString  column_name;
    AosJQLData data_type;
    AosJQLColumnAttList att_list;
    AosStringListVec       column_list;
};

typedef vector<AosJQLTableColumn >  AosJQLTableColumnList;

enum AosJQLStringTrimType
{
	eTrimType_Invalid = 0,
	
	eTrimType_None,	
	eTrimType_Leading,
	eTrimType_Trailing,
	eTrimType_Both,
	
	eTrimType_Max,
};

enum AosJQLIgnoreReplace
{
	eIgnoreReplace_Invalid = 0,
	
	eIgnoreReplace_None,	
	eIgnoreReplace_Ignore,
	eIgnoreReplace_Replace,
	
	eIgnoreReplace_Max,
};

class AosJQLSequenceInfo
{
private:
	OmnString 				mName;
	int 					mIncrementbyNum;
	map<string, string>		mAttrs;

public:
	AosJQLSequenceInfo() 
	{
		mAttrs["zky_otype"] = "sequence";
	}
	~AosJQLSequenceInfo() {}

	void setIncrementBy(const int num) { setAttr("increment_by", num); }
	void setStartWith(const int num) { setAttr("start_with", num); }
	void setMaxValue(const int num) { setAttr("max_value", num); }
	void setMinValue(int num) { setAttr("min_value", num); }
	void setIsCycle(const bool flag) 
	{ 
		string value = "false";
		if (flag) value = "true";
		setAttr("is_cycle", value);
	}
	void setCacheNum(const int num) { setAttr("cache_num", num); }
	void setIsOrder(const bool flag)
	{
		string value = "false";
		if (flag) value = "true";
		setAttr("is_order", value); 
	}
	map<string, string>	getAttrs() { return mAttrs; }


private:
	void setAttr(const string &key, const string &value)
	{
		if (key != "") mAttrs[key] = value;				
	}
	void setAttr(const string &key, const int64_t &value)
	{
		char buff[100];
		sprintf(buff, "%ld", value);
		mAttrs[key] = buff;
	}

	string getAttr(const string &key) 
	{
		if (mAttrs.count(key)) return mAttrs[key];
		return "";
	}

};


class AosJQLDataFieldTypeInfo : public AosJqlDataStruct
{
private:
	OmnString 	mType;	
	int			mFirst;
	int 		mSecond;

public:
	AosJQLDataFieldTypeInfo() {}
	~AosJQLDataFieldTypeInfo() {}
	
	void 			setType(OmnString type) { mType = type.toLower(); }
	void 			setValue(u32 p, int s) { mFirst = p; mSecond = s; }
	int				getFirst() { return mFirst; }
	int 			getSecond() { return mSecond; }
	OmnString 		getType() { return mType; }
};


struct AosJQLCreateBySelectStmt
{
    AosJQLIgnoreReplace type;
    
    //data
};

//data structures for table/index operations
class AosJqlColumn : public AosJqlDataStruct
{

public:
    OmnString  name;
    OmnString  comment;
    OmnString  sdatatype;
	AosDataType::E	type;
	int			size;
	u32			offset;  //for record
	bool		nullable;  
	OmnString	defaultVal;
	string		format;
	int 		precision;  // for number
	int			scale;		// for number
	int			v1;
	int			v2;
	//arvin 2015.07.27
	//JIMODB-96
	AosJqlColumn()
	:
	type(AosDataType::eInvalid),
	size(0),
	offset(0),
	precision(0),
	scale(0)
	{
		v1 = 0;
		v2 = 0;
	}
};

struct AosJqlSelectInto
{
	OmnString 	fname;

	AosJqlSelectInto() {}
	~AosJqlSelectInto() {}
	
	OmnString	getFileName() {
		return fname;
	}
};


enum AosJqlIndexSort
{
	eIndexCol_Invalid = 0,

	eIndexCol_Asc,
	eIndexCol_Desc,
	eIndexCol_Normal,

	eIndexCol_Max,
};

//for record picker
class AosJqlRecordPicker : public AosJqlDataStruct
{
public:
	OmnString record;
	OmnString matchStr;
	int		  offset;
};


struct AosJqlIndexColumn
{
	AosExprObj* name;
	AosJqlIndexSort sort;
	u32 length;

	AosJqlIndexColumn()
	:
	name(0),
	sort(eIndexCol_Normal),
	length(0)
	{

	}

	~AosJqlIndexColumn()
	{
	}
};

//for create schema statement
//field refers to column in a file
struct AosJqlField
{
    OmnString  name;
    OmnString  comment;
	OmnString  type;
	u32		   size;
};

///////////////////////////////////////////////////////////////////////
//SELECT
//    	[ALL | DISTINCT | DISTINCTROW ]
//    	[HIGH_PRIORITY]
//    	[STRAIGHT_JOIN]
//    	[SQL_SMALL_RESULT] [SQL_BIG_RESULT] [SQL_BUFFER_RESULT]
//    	[SQL_CACHE | SQL_NO_CACHE] [SQL_CALC_FOUND_ROWS]
//   select_expr [, select_expr ...]
//   	[FROM table_references
//   	[PARTITION partition_list]
//   	[WHERE where_condition]
//    	[GROUP BY {col_name | expr | position}
//    	[ASC | DESC], ... [WITH ROLLUP]]
//    	[HAVING where_condition]
//   	[ORDER BY {col_name | expr | position}
//   	[ASC | DESC], ...]
//   	[LIMIT {[offset,] row_count | row_count OFFSET offset}]
//   	[PROCEDURE procedure_name(argument_list)]
//   	[INTO OUTFILE 'file_name'
//   	[CHARACTER SET charset_name]
//   	export_options
//   	| INTO DUMPFILE 'file_name'
//   	| INTO var_name [, var_name]]
//
//////////////////////////////////////////////////////////////////////


struct AosSQLTableFactor
{
	AosJQLTableReferenceType type;
	AosExprObj* table_name;

	AosSQLTableFactor()       
	:                         
	table_name(0)           
	{                         
	}                         

	~AosSQLTableFactor()      
	{                         
		delete table_name;  
	}                         
};


struct AosTableReference
{
	AosJQLTableReferenceType type;
	AosSQLTableFactor*	table_factor;

	AosTableReference()
	:
	table_factor(0)
	{
	}

	~AosTableReference()
	{
		delete table_factor;
	}
};


typedef std::vector<AosExprObjPtr> AosExprList;

struct AosJqlStmtInsert
{
	std::string 	table_name;
	AosExprList* 	opt_col_names;
	AosExprList* 	insert_vals_list;
	std::string 	errmsg;

	AosJqlStmtInsert()
	:
	opt_col_names(0),
	insert_vals_list(0)
	{
	}

	~AosJqlStmtInsert()
	{
		delete opt_col_names;
		delete insert_vals_list;
	}
};


///////////////////////////////////////////////////////////////////////////
//
//	  UPDATE [LOW_PRIORITY] [IGNORE] table_reference
//		  SET col_name1={expr1|DEFAULT} [, col_name2={expr2|DEFAULT}] ...
//		  [WHERE where_condition]
//		  [ORDER BY ...]
//		  [LIMIT row_count]
//				
//////////////////////////////////////////////////////////////////////////

struct AosJqlStmtUpdate
{
	AosExprList*   		update_expr_list;
	AosExprObj*			table;
	AosExprObj*			opt_where;

	AosJqlStmtUpdate()
	:
	update_expr_list(0),
	table(0),
	opt_where(0)
	{

	}
	
	~AosJqlStmtUpdate()
	{
		delete update_expr_list;
		delete table;
		delete opt_where;
	}

};


////////////////////////////////////////////////////////////////////////
//
//	DELETE [LOW_PRIORITY] [QUICK] [IGNORE] FROM tbl_name
//		[PARTITION (partition_name,...)]
//		[WHERE where_condition]
//		[ORDER BY ...]
//		[LIMIT row_count]
//
///////////////////////////////////////////////////////////////////////

struct AosJqlStmtDelete
{
	AosExprList*  		 	delete_expr_list;
	AosExprObj*				table;
	AosExprObj*				opt_where;

	AosJqlStmtDelete()
	:
	delete_expr_list(0),
	table(0),
	opt_where(0)
	{
	}

	~AosJqlStmtDelete()
	{
		delete delete_expr_list;
		delete table;
		delete opt_where;
	}
};

////////////////////////////////////////////////////////////////////////
//
//	CREATE TABLE tbl_name
//	      create_table_definition [, create_table_definition ...]
//	      [COMMENT STRING] [CHECKSUM {'0'|'1'}] [AUTO_INCREMENT step]
//
//	Or
//
//  CREATE TABLE expr LIKE expr
//
///////////////////////////////////////////////////////////////////////
struct AosJqlTableOption
{
	//this struct is for tailing options
	OmnString comment;
	u32 checksum;
	u32 autoInc;
	OmnString inputDataFormat;
	OmnString data_source;
	AosExprList *raw_keys;
	OmnString db_name;
	OmnString db_addr;
	u32 db_port;
	OmnString db_t_name;
	
	AosJqlTableOption()
	:
	comment(0),
	checksum(0),
	autoInc(0),
	raw_keys(0)
	{
	}

	~AosJqlTableOption()
	{
	}
};

struct AosJqlIndexOption
{
	//this struct is for tailing options
	OmnString comment;
	OmnString type;
	u32 blockSize;
	
	AosJqlIndexOption()
	:
	comment(0),
	type(0),
	blockSize(0)
	{
	}

	~AosJqlIndexOption()
	{
	}
};

struct AosJqlCreateDefineGroup
{
	vector<AosJqlColumnPtr> *columns;

	AosJqlCreateDefineGroup()
	:
	columns(0)
	{
	}

	~AosJqlCreateDefineGroup()
	{
		//not release pointers for now. Since they
		//will be assigned to other object to use
	}
};

////////////////////////////////////////////////////////////////////////
//
//	CREATE INDEX index_name
//	ON tbl_name (index_col_name,...) [index_option]
//
//	index_col_name:
//	col_name [(length)] [ASC | DESC]
//
//	index_option:
//	BLOCKSIZE value | TYPE 'string' | COMMENT 'string' 
//
///////////////////////////////////////////////////////////////////////

#endif
