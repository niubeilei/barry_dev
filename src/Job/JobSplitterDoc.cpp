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
// This job splitter splits jobs based on IIL size. The input is an IIL.
// It creates one task for each N number of entries in the IIL.
//
// mFileName:	
// mRecordSize: If the file is fixed length records, this is the record length.
//
// Modification History:
// 2012/10/16 Created by Ken
////////////////////////////////////////////////////////////////////////////
#include "Job/JobSplitterDoc.h"

#include "API/AosApi.h"
#include "Job/JobSplitterNames.h"
#include "TransUtil/RoundRobin.h"

/*
AosJobSplitterDoc::AosJobSplitterDoc(const bool flag)
:
AosJobSplitter(AOSJOBSPLITTER_DOC, AosJobSplitterId::eDoc, flag),
mLock(OmnNew OmnMutex())
{
}


AosJobSplitterDoc::~AosJobSplitterDoc()
{
}


bool
AosJobSplitterDoc::parseValCond(
		const AosXmlTagPtr &val_cond,
		const AosRundataPtr &rdata)
{
	if(!val_cond) return true;

	mValueCond.mValueType = val_cond->getAttrStr(AOSTAG_VALUE_TYPE);
	aos_assert_r(mValueCond.mValueType == "str" || mValueCond.mValueType == "u64", false);

	mValueCond.mOpr = AosOpr_toEnum(val_cond->getAttrStr(AOSTAG_OPR));
	aos_assert_r(AosOpr_valid(mValueCond.mOpr), false);

	AosXmlTagPtr vv1 = val_cond->getFirstChild(AOSTAG_VALUE1);
	if (vv1) mValueCond.mValue1 = vv1->getNodeText();

	AosXmlTagPtr vv2 = val_cond->getFirstChild(AOSTAG_VALUE2);
	if (vv2)
	{
		aos_assert_r(vv1, false);
		mValueCond.mValue2 = vv2->getNodeText();
	}

	mValueCond.mIsGroup = val_cond->getAttrBool(AOSTAG_ISGROUP, false);
	if(!mValueCond.mIsGroup) return true;

	AosXmlTagPtr group = val_cond->getFirstChild(AOSTAG_GROUP);
	aos_assert_r(group, false);

	mValueCond.mKeyGroupType = AosQueryContextObj::QueryKeyGroupingType_toEnum(group->getAttrStr(AOSTAG_QUERYKEYGROUPINGTYPE));
	mValueCond.mValueGroupType = AosQueryContextObj::QueryValueGroupingType_toEnum(group->getAttrStr(AOSTAG_QUERYVALUEGROUPINGTYPE));

	mValueCond.mNeedFilter = group->getAttrBool(AOSTAG_NEEDFILTER, false);
	if(!mValueCond.mNeedFilter) return true;

	AosXmlTagPtr filter = group->getFirstChild(AOSTAG_FILTER);
	aos_assert_r(filter, false);

	mValueCond.mFilterValueType = filter->getAttrStr(AOSTAG_VALUE_TYPE);
	aos_assert_r(mValueCond.mFilterValueType == "str" || mValueCond.mFilterValueType == "u64", false);

	mValueCond.mFilterValue = filter->getNodeText();
	mValueCond.mFilterOpr = AosOpr_toEnum(filter->getAttrStr(AOSTAG_OPR));
	aos_assert_r(AosOpr_valid(mValueCond.mFilterOpr), false);

	mValueCond.mFilterType = AosQueryContextObj::FilterType_toEnum(filter->getAttrStr(AOSTAG_FILTERTYPE));
	return true;
}

bool
AosJobSplitterDoc::config(
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	//<splitter type="doc" 
	//			docscannerid="xxxx" 
	//			sdoc_objid="xxxx" 
	//			iilname="xxxx" 
	//			blocksize="xxxx">
	//	<AOSTAG_VALUE_COND .../>
	//</splitter>
	aos_assert_r(sdoc, false);
	mDocScannerId << mJobDocid << "_";
	mDocScannerId << sdoc->getAttrStr("docscannerid", "");
	aos_assert_r(mDocScannerId != "", false);
	mTaskSdocObjid = sdoc->getAttrStr(AOSTAG_SDOC_OBJID, "");
	aos_assert_r(mTaskSdocObjid != "", false);


	mIILName = sdoc->getAttrStr(AOSTAG_IILNAME, "");
	aos_assert_r(mIILName != "", false);

	mBlockSize = sdoc->getAttrU64("blocksize", eDefaultBlockSize);
	AosXmlTagPtr val_cond = sdoc->getFirstChild(AOSTAG_VALUE_COND);
	if (val_cond)
	{
		bool rslt = parseValCond(val_cond, rdata);
		aos_assert_r(rslt, false);
	}

	// Create the query context
	mQueryContext = AosQueryContextObj::createQueryContextStatic();
	mQueryContext->setOpr(mValueCond.mOpr);
	mQueryContext->setReverse(false);
	//mQueryContext->setBlockSize(1000000);
	
	// The following sets the query condition
	if(mValueCond.mValueType == "u64")
	{
		mQueryContext->setU64Value(atoll(mValueCond.mValue1.data()));
		mQueryContext->setU64Value2(atoll(mValueCond.mValue2.data()));
	}
	else
	{
		mQueryContext->setStrValue(mValueCond.mValue1);
		mQueryContext->setStrValue2(mValueCond.mValue2);
	}
	return true;
}

bool
AosJobSplitterDoc::splitTasks(
		map<OmnString, AosTaskDataObjPtr> &tasks,
		const AosRundataPtr &rdata)
{
	aos_assert_r(mJobDocid != 0, false);
	tasks.clear();
	int num_physical =  AosGetNumPhysicals();
	AosTaskDataObjPtr data;
	mLock->lock();
	for (int i=0; i<num_physical; i++)
	{
		data = AosTaskDataObj::createTaskDataDoc(mDocScannerId, mTaskSdocObjid, i);
		OmnString task_id;
		task_id << mJobDocid << "_" << i;
		tasks[task_id] = data;
	}
	mLock->unlock();
	return true;
}

bool
AosJobSplitterDoc::runQuery(const AosRundataPtr &rdata)
{
	return AosIILClient::getSelf()->querySafe(mIILName, mDocScannerId, mBlockSize, mQueryContext, rdata);
}


AosJobSplitterObjPtr
AosJobSplitterDoc::create(
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata) const
{
	AosJobSplitterDoc * splitter = OmnNew AosJobSplitterDoc(false);
	bool rslt = splitter->config(sdoc, rdata);
	aos_assert_r(rslt, 0);
	return splitter;
}
*/
