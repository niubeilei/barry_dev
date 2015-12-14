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
// 2013/10/08 Created by Barry Niu
////////////////////////////////////////////////////////////////////////////
#include "JQLExpr/ExprGetDocid.h"

#include "API/AosApi.h"
#include "SEInterfaces/TaskObj.h"
#include "TaskMgr/Task.h"

AosExprGetDocid::AosExprGetDocid()
{
}

AosExprGetDocid::~AosExprGetDocid()
{
}


bool
AosExprGetDocid::getValue(
		AosRundata *rdata,
		AosDataRecordObj *record,
		AosValueRslt &value)
{
	u64 docid = getNextDocid(rdata, record);
	aos_assert_r(docid > 0, false);
	value.setU64(docid);
	return true;
}

bool
AosExprGetDocid::getValue(
		AosRundata *rdata,
		vector<AosDataRecordObj *> &records,
		AosValueRslt &value)
{
	OmnShouldNeverComeHere;
	return false;
}

u64
AosExprGetDocid::getNextDocid(
		AosRundata *rdata,
		AosDataRecordObj *record)
{
	if (mNumDocids <= 0)
	{
		aos_assert_r(record, 0);
		u64 task_docid= record->getTaskDocid();
		AosTaskObjPtr task = AosTask::getTaskStatic(task_docid, rdata);
		aos_assert_r(task, 0);
		OmnString key;
		AosXmlTagPtr doc;
		task->getNextDocids(rdata, mDocids, mNumDocids, key, mDocSize, doc);
	}

	aos_assert_r(mNumDocids > 0, 0);
	aos_assert_r(mDocids > 0, 0);

	u64 docid = mDocids++;
	mNumDocids--;
	return docid;
}


OmnString 
AosExprGetDocid::dump() const
{
	return "getdocid()";
} 


OmnString 
AosExprGetDocid::dumpByNoEscape() const
{
	return dump();
} 

OmnString 
AosExprGetDocid::dumpByStat() const
{
	return dump();
} 

OmnString 
AosExprGetDocid::dumpByNoQuote()const
{
	return dump();
}

AosExprObjPtr
AosExprGetDocid::cloneExpr() const                            
{
	try
	{
		AosExprGetDocidPtr expr = OmnNew AosExprGetDocid();
		expr->mDocids = mDocids;
		expr->mNumDocids = mNumDocids;
		expr->mDocSize = mDocSize;
		return expr; 
	}   

	catch (...)
	{
		OmnAlarm << "failed create expr" << enderr;
		return 0;
	}   

	OmnShouldNeverComeHere;
	return 0;
}


AosJimoPtr
AosExprGetDocid::cloneJimo() const
{
	return OmnNew AosExprGetDocid(*this);
}


