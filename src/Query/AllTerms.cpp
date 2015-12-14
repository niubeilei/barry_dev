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
// 08/20/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Query/AllTerms.h"

#include "Query/TermArith.h"
#include "Query/TermContainer.h"
#include "Query/TermByDocid.h"
#include "Query/TermBatchQuery.h"
#include "Query/TermComment.h"
#include "Query/TermCounterCompare.h"
#include "Query/TermCounterMulti.h"
#include "Query/TermCounterSingle.h"
#include "Query/TermCounterSub.h"
#include "Query/TermCounterSub2.h"
#include "Query/TermCounterMultiSub.h"
#include "Query/TermCounterMultiSub2.h"
#include "Query/TermCounterAll.h"
#include "Query/TermDate.h"
#include "Query/TermDeletedDocs.h"
#include "Query/TermHitType.h"
#include "Query/TermGroup.h"		// Andy zhang, 2013/08/06
#include "Query/TermJoin.h"			// Andy zhang, 2013/08/12
#include "Query/TermFilter.h"			// Andy zhang, 2013/08/20
#include "Query/TermCounterOfUWDivOrder.h"			// Andy zhang, 2013/12/21
#include "Query/TermIIL.h"
#include "Query/TermIILType.h"
#include "Query/TermInrange.h"
#include "Query/TermMonitorLog.h"
#include "Query/TermTagAnd.h"
#include "Query/TermTagOr.h"
#include "Query/TermKeywordsAnd.h"
#include "Query/TermKeywordsOr.h"
#include "Query/TermUserCustom.h"
#include "Query/TermVersionDocs.h"
#include "Query/TermInlineSearch.h" // Ken Lee, 2011/12/06
#include "Query/TermRawIIL.h" 
#include "Query/TermRawCounter.h" 
#include "Query/TermJoin2.h" 		// Ken Lee, 2013/12/04
#include "Query/TermRunMysqlSelect.h"	//Andy Zhang, 2013/11/15
#include "Query/TermLog.h"
#include "Thread/Mutex.h"

OmnSPtr<AosQueryTerm> sgTerms[AosQueryType::eMax];
AosStr2U32_t 	sgQueryIdMap;
AosAllTerms 	gAllTerms;

AosAllTerms::AosAllTerms()
{
	static AosTermArith				sgArith(true);
	static AosTermByDocid			sgByDocid(true);
	static AosTermBatchQuery		sgBatchQuery(true);
	static AosTermComment			sgComment(true);
	static AosTermContainer			sgContainer(true);
	static AosTermCounterCompare	sgCounterCompare(true);
	static AosTermCounterMulti		sgCounterMulti(true);
	static AosTermCounterSingle		sgCounterSingle(true);
	static AosTermCounterSub		sgCounterSub(true);
	static AosTermCounterSub2		sgCounterSub2(true);
	static AosTermCounterMultiSub	sgCounterMultiSub(true);
	static AosTermCounterMultiSub2	sgCounterMultiSub2(true);
	static AosTermDate				sgDate(true);
	static AosTermDeletedDocs		sgDeletedDocs(true);
	static AosTermGroup				sgGroup(true);
	static AosTermFilter			sgFilter(true);
	static AosTermJoin				sgJoin(true);
	static AosTermIIL				sgIIL(true);
	static AosTermIILType			sgIILType(true);
	static AosTermInrange			sgInrange(true);
	static AosTermMonitorLog		sgMonitorLog(true);
	static AosTermTagAnd			sgTagAnd(true);
	static AosTermTagOr				sgTagOr(true);
	static AosTermKeywordsAnd		sgWordAnd(true);
	static AosTermKeywordsOr		sgWordOr(true);
	static AosTermUserCustom		sgUserCustom(true);
	static AosTermVersionDocs		sgVersionDocs(true);
	static AosTermInlineSearch		sgInlineSearch(true);	// Ken Lee, 2011/12/06
	static AosTermRawIIL			sgRawIIL(true); 
	static AosTermRawCounter		sgTermRawCounter(true); 
	static AosTermLog				sgLog(true);
	static AosTermCounterAll		sgTermCounterAll(true);
	static AosTermJoin2				sgTermJoin2(true);		// Ken Lee, 2013/12/04
	static AosTermRunMysqlSelect    sgTermRunMysqlSelect(true);
	static AosTermCounterOfUWDivOrder sgTermCounterOfUWDivOrder(true);
}

