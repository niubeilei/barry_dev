////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 2013/12/23 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#if 0
#include "StreamEngine/DocidProvider.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"

AosDocidProvider::AosDocidProvider(
		const AosRundataPtr &rdata,
		const AosTaskObjPtr	&task,
		const OmnString &rcd_type_key,
		const AosXmlTagPtr &record_doc,
		const int doc_size)

:
mRcdTypeKey(rcd_type_key),
mRecordDocid(0),
mDocSize(doc_size),
mCrtSizeid(0),
mStartDocids(0),
mRemainNum(0),
mLock(OmnNew OmnMutex())
{
	bool rslt = init(rdata, record_doc);
	aos_assert(rslt);	
}


AosDocidProvider::~AosDocidProvider()
{
}

bool
AosDocidProvider::init(	
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &record_doc,
		const AosTaskDataObjPtr &task_data)
{
	aos_assert_r(task_data, false);

	OmnScreen << "Docid Allocator ========================== get output from task doc ============" << endl;
	AosXmlTagPtr output;
	bool rslt = task_data->serializeTo(output, rdata);
	aos_assert_r(rslt && task_data, false);
		
	mStartDocids = output->getAttrU64(AOSCONFIG_STARTDOCID, 0);
	mRemainNum = output->getAttrInt64(AOSTAG_NUM_DOCS, -1);
	mCrtSizeid = AosGetSizeIdByDocid(mStartDocids);
	aos_assert_r(mCrtSizeid, false);
	
	mRecordDocid = record_doc->getAttrU64(AOSTAG_DOCID, 0); 
	aos_assert_r(mRecordDocid, false);
	return true;
}


bool
AosDocidProvider::init(	
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &record_doc)
{
	mRecordDocid = record_doc->getAttrU64(AOSTAG_DOCID, 0); 
	
	if(mRecordDocid != 0)
	{
		mCrtSizeid = AosCreateSizeId(mRecordDocid, rdata); 
		aos_assert_r(mCrtSizeid, false);
		return true;
	}
	
	// this AosCreateSizeId will create record_doc,
	AosXmlTagPtr clone_doc = record_doc->clone(AosMemoryCheckerArgsBegin); 

	mCrtSizeid = AosCreateSizeId(mDocSize, clone_doc, rdata); 
	mRecordDocid = clone_doc->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_r(mCrtSizeid, false);
	aos_assert_r(mRecordDocid, false);
	return true;
}


bool
AosDocidProvider::getNextDocids(
		const AosRundataPtr &rdata,
		u64 &docid,
		int &num_docids)
{
	mLock->lock();

	if (mRemainNum <=0)
	{
		getMoreDocids(rdata, mStartDocids, mRemainNum);	
	}
	
	aos_assert_r(mStartDocids > 0, false);
	aos_assert_r(mRemainNum > 0, false);
	
	docid = mStartDocids;
	//levi
	//OmnScreen << "num_docids : " << num_docids << "; docid:" << docid << endl;
	num_docids = mRemainNum > eIncDocidsEach ? eIncDocidsEach : mRemainNum; 
	
	mStartDocids += num_docids;
	mRemainNum -= num_docids;

	mLock->unlock();
	return true;
}

bool
AosDocidProvider::getMoreDocids(
		const AosRundataPtr &rdata,
		u64 &docid,
		int &num_docids)
{
	aos_assert_r(mCrtSizeid, false);
	bool overflow = false;
	bool rslt = AosGetDocids(mCrtSizeid, docid, num_docids, overflow, rdata);
	aos_assert_r(rslt, false);
	if (overflow)
	{
		// The current size overflows. Need to create a new sizeid. 
		u64 sizeid = AosCreateSizeId(mRecordDocid, rdata); 
		aos_assert_r(sizeid, false);

		mCrtSizeid = sizeid;
		num_docids = 0;
		//Seems need to reset overflow. Phil 2015/03/29
		overflow = false;  
		rslt = AosGetDocids(mCrtSizeid, docid, num_docids, overflow, rdata);
		aos_assert_r(rslt, false);

		if (overflow)
		{
			AosGetDocids(mCrtSizeid, docid, num_docids, overflow, rdata);
		}
		aos_assert_r(!overflow, false);
	}

	aos_assert_r(docid > 0, false);
	aos_assert_r(num_docids > 0, false);
	return true;	
}

#endif
