////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//	
// Description:
//
// Modification History:
// 2013/09/24 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JQLStatement_JOIN_STATEMENT_H
#define AOS_JQLStatement_JOIN_STATEMENT_H

#include <stdarg.h>
#include <cstdlib>
#include <vector>
#include <string>
#include <iostream>
#include <stdio.h>

#include "JQLStatement/JqlStatement.h"

enum AosStmtType
{
	STMT_ADDNODETOCLUSTER = 10001,
	STMT_CANCELCLUSTERCHANGES,
	STMT_COMMITCLUSTERCHANGES,
	STMT_CREATECLUSTER,
	STMT_CREATEDOCS,
	STMT_CREATEJOB,
	STMT_REMOVECLUSTER,
	STMT_REMOVENODEFROMCLUSTER,
	STMT_RETRIEVECLUSTER,
	STMT_RETRIEVECLUSTERLOCALCHANGES = 10010,
	STMT_SHUTDOWNCLUSTER,
	STMT_STARTCLUSTER,
	STMT_RUNJOB,
	STMT_CHECKCLUSTERSTATUS,
	STMT_STOPCLUSTER,
	STMT_STARTSERVER,
	STMT_RETRIEVEDOCBYOBJID,
	STMT_RETRIEVEDOCBYDOCID,

	STMT_ERROR,
	STMT_SUCCESS,

};

enum AosJoinType
{
    JOINTYPE_Unknown = 0,
    JOINTYPE_INNER,
    JOINTYPE_FULL_OUTER,
    JOINTYPE_LEFT_OUTER,
    JOINTYPE_RIGHT_OUTER,
    JOINTYPE_LEFT_OUTER_EXCLUSIVE,
    JOINTYPE_RIGHT_OUTER_EXCLUSIVE,
    JOINTYPE_XO_OUTER,
};


struct AosRecordPickerItem
{
	std::string type_name;
	std::string jimoid_name;
	std::string as_name;     

	void toString()
	{
		std::cout << "Type name kkk: " << type_name << ", "
			<< jimoid_name << ", " << as_name << std::endl;
	}
};

typedef std::vector<AosRecordPickerItem> AosRecordPickerList;

struct AosSelectorType
{
    std::string type_name;
};

typedef std::vector<AosSelectorType> AosSelectorTypeList; 

struct AosDataSetItem
{
    std::string data_set_name;
    std::string record_schema_name;
    std::string record_picker_using_name;
    std::string filter_name;
    std::string filtered_record_proc_name;
    std::string accepted_record_proc_name;
    std::string matched_record_proc_name;
    std::string mismatched_record_proc_name;
    std::string dropped_record_proc_name;
    std::string multiple_matched_records_proc_name;
    AosSelectorTypeList default_value_selectorList;
};
typedef std::vector<AosDataSetItem> AosDataSetItemList;


struct AosRecordProcItem
{
    std::string type_name;
    std::string as_name;     
};
typedef std::vector<AosRecordProcItem> AosRecordProcItemList;

struct AosJimoid
{
    std::string jimoid_name;
};

struct AosJoinStmtPhrase
{
	enum PhraseType
	{
    	PhraseType_Unknown,
    	PhraseType_RecordPickerList,
    	PhraseType_RecordProcList,
    	PhraseType_DataSetList,
    	PhraseType_Jimoid,
    	PhraseType_JoinType,
    	PhraseType_Max,
	};

   	PhraseType             	stmtType;
   	AosRecordPickerList     recordPickerList;
   	AosRecordProcItemList   recordProcItemList;
   	AosDataSetItemList      dataSetItemList;
   	AosJimoid               jimoid;
   	AosJoinType            	joinType;
};

struct AosJoinStatement : public AosJqlStatement
{
public:
	std::vector<AosJoinStmtPhrase>	phrases;

public:
	AosJoinStatement()
	{
	}

	void appendPhrase(AosJoinStmtPhrase *phrase)
	{
		phrases.push_back(*phrase);
	}

	void clear()
	{
		phrases.clear();
	}

	virtual void dump()
	{
		std::cout << "Join Statement, Number of phrases: " << phrases.size() << std::endl;
	}

	virtual AosJqlStatement * clone()
	{
		AosJoinStatement *stmt = new AosJoinStatement();
		stmt->phrases = phrases;
		return stmt;
	}

	virtual bool run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog);
};

#endif
