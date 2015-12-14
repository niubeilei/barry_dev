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
// 2013/03/27	Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_QueryType_h
#define Aos_SEInterfaces_QueryType_h


#include "Util/String.h"

#define AOSTERMTYPE_AND					"and"
#define AOSTERMTYPE_ARITH				"arith"
#define AOSTERMTYPE_BYDOCID				"bydocid"
#define AOSTERMTYPE_BATCH_QUERY			"batchquery"
#define AOSTERMTYPE_COMMENT				"comment"
#define AOSTERMTYPE_CONTAINER			"container"
#define AOSTERMTYPE_COUNTER_COMPARE		"counterCompare"
#define AOSTERMTYPE_COUNTER_MULTI		"counterMulti"
#define AOSTERMTYPE_COUNTER_SINGLE		"counterSingle"
#define AOSTERMTYPE_COUNTER_SUB			"counterSub"
#define AOSTERMTYPE_COUNTER_SUB2		"counterSub2"
#define AOSTERMTYPE_COUNTER_MULTISUB    "counterMultiSub"
#define AOSTERMTYPE_COUNTER_MULTISUB2   "counterMultiSub2"
#define AOSTERMTYPE_COUNTER_ALL   		"counterAll"
#define AOSTERMTYPE_DATE				"date"
#define AOSTERMTYPE_DELETED_DOCS		"deldocs"
#define AOSTERMTYPE_HITTYPE				"hittype"
#define AOSTERMTYPE_IIL					"iil"
#define AOSTERMTYPE_IILTYPE				"iiltype"
#define AOSTERMTYPE_KEYWORDS			"keywords"
#define AOSTERMTYPE_MONITOR_LOG			"monitorlog"
#define AOSTERMTYPE_NOTEXIST			"notexist"
#define AOSTERMTYPE_OR					"or"
#define AOSTERMTYPE_RANGE				"range"
#define AOSTERMTYPE_TAG					"tag"
#define AOSTERMTYPE_TAG_AND				"tagand"
#define AOSTERMTYPE_TAG_OR				"tagor"
#define AOSTERMTYPE_TAGS				"tags"
#define AOSTERMTYPE_TEMPLATE			"template"
#define AOSTERMTYPE_KEYWORDS_AND		"wordand"
#define AOSTERMTYPE_KEYWORDS_OR			"wordor"
#define AOSTERMTYPE_VERSION_DOCS		"verdocs"
#define AOSTERMTYPE_USERCUSTOM			"usercustom"

#define AOSTERMTYPE_LOGS				"logs"

#define AOSTERMTYPE_INLINESEARCH		"inlinesearch"
#define AOSTERMTYPE_RAWIIL				"rawiil"
#define AOSTERMTYPE_RAWCOUNTER			"rawcounter"
#define AOSTERMTYPE_GROUP				"group"			// Andy Zhang, 2013/08/06
#define AOSTERMTYPE_JOIN				"join"			// Andy Zhang, 2013/08/12
#define AOSTERMTYPE_RUNMYSQLSELECT		"runmysqlselect"			// Andy Zhang, 2013/11/15
#define AOSTERMTYPE_FILTER				"filter"			// Andy Zhang, 2013/08/20
#define AOSTERMTYPE_COUNTEROFUWDIVORDER "UWDivOrder"
#define AOSTERMTYPE_JOIN2				"join2"			// Ken Lee, 2013/12/04
#define AOSTERMTYPE_JIMO				"jimo"			// Ken Lee, 2013/12/04


class AosQueryType
{
public:
	enum E 
	{
		eInvalid,

		eAnd,
		eArith,
		eByDocid,
		eBatchQuery,
		eContainer,
		eComment,
		eCounterSingle,
		eCounterCompare,
		eCounterMulti,
		eCounterSub,
		eCounterSub2,
		eCounterMultiSub,
		eCounterMultiSub2,
		eRunMysqlSelect,
		eGroup,			// Andy Zhang, 2013/08/06
		eJoin,			// Andy Zhang, 2013/08/12
		eFilter,		// Andy Zhang, 2013/08/20
		eCounterOfUWDivOrder,		// Andy Zhang, 2013/08/20
		eJoin2,			// Ken Lee, 2013/12/04
		eCounterAll,
		eDate,
		eDeletedDocs,
		eHitType,
		eIIL,
		eIILType,
		eOr,
		eKeywordsAnd,
		eKeywordsOr,
		eMonitorLog,
		eNotExist,
		eRange,
		eTag,
		eTagAnd,
		eTagOr,
		eTemplate,
		eVersionDocs,
		eUserCustom,
		eLog,
		eInlineSearch,	// Ken Lee, 2011/12/05
		eRawIIL,
		eRawCounter,
		eQueryProc,		// Chen Ding, 2013/12/06
		eJimo,		// Chen Ding, 2013/12/06

		eMax
	};

public:

	static bool isValid(const E id) {return id > eInvalid && id < eMax;}
	static E toEnum(const OmnString &reqid);
	static bool addName(const OmnString &name, const E e, OmnString &errmsg);
	static bool isValidCounterType(const E id) {return id >= eCounterSingle && id <= eCounterAll;}
	static OmnString toStr(const E code);
	static bool init();
};
#endif

