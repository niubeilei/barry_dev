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
// 09/02/2013 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "IILUtil/IILDfmDocs.h"

#include "DfmUtil/DfmDoc.h"
#include "DocFileMgr/DfmLogUtil.h"
#include "SEInterfaces/DocFileMgrObj.h"

AosIILDfmDocs::AosIILDfmDocs(const u64 &root_iilid, const bool show_log)
:
mShowLog(show_log),
mLock(OmnNew OmnMutex()),
mBuff(0),
mTempDoc(0),
mRootIILId(root_iilid)
{
	mBuff = OmnNew AosBuff(eDftBuffSize AosMemoryCheckerArgs);
	mTempDoc = AosDfmDoc::cloneDoc(AosDfmDocType::eIIL, 0);
	mNum = 0;
}


AosIILDfmDocs::~AosIILDfmDocs()
{
	//OmnScreen << "del del delete mRootIILID:" << mRootIILId << ";" << endl;
}


bool
AosIILDfmDocs::addEntry(
		const AosRundataPtr &rdata,
		const AosDfmDocPtr &doc,
		vector<AosTransId> &trans_ids)
{
mNum++;
	AosBuffPtr doc_buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	bool rslt = AosDfmLogUtil::setDocInfoToBuff(rdata, doc_buff, doc, trans_ids); 
	aos_assert_r(rslt, false);

	u32 rcd_total = doc_buff->dataLen();
	aos_assert_r(rcd_total > 0 , false);

	mLock->lock();
	int64_t idx = mBuff->getCrtIdx();
	mIndex.insert(make_pair(doc->getDocid(), idx));

	mBuff->setU32(rcd_total);
	mBuff->setBuff(doc_buff->data(), doc_buff->dataLen());
	mLock->unlock();

	if (mShowLog)
	{
		OmnString trans_str;
		for(u32 i=0; i<trans_ids.size(); i++)
		{
			trans_str << trans_ids[i].toString() << "; ";
		}
		
		OmnScreen << "IILDfmDocs; addEntry; finish."
			<< "; docid:" << doc->getDocid()
			<< "; sts:" << (doc->getOpr() == AosDfmDoc::eSave?"save":"delete")
			<< "; body_len:" << rcd_total - 20
			<< "; rcd_total:" << rcd_total
			<< "; trans_ids:" << trans_str
			<< endl;
	}
	return true;
}


AosDfmDocPtr
AosIILDfmDocs::readDoc(
		const AosRundataPtr &rdata,
		const u64 &docid)
{
	mLock->lock();
	mapitr_t itr = mIndex.find(docid);
	if (itr == mIndex.end()) 
	{
		mLock->unlock();
		return 0;
	}

	int64_t crt_idx = mBuff->getCrtIdx();
	int64_t offset = itr->second;
	mBuff->setCrtIdx(offset);

	u32 rcd_total = mBuff->getU32(0);
	aos_assert_r(rcd_total, 0);
	
	AosBuffPtr buff = OmnNew AosBuff(rcd_total AosMemoryCheckerArgs);
	mBuff->getBuff(buff->data(), rcd_total);
	mBuff->setCrtIdx(crt_idx);
	mLock->unlock();

	AosDfmDocPtr doc = AosDfmLogUtil::getDocInfoFromBuff(rdata, mTempDoc, buff);
	return doc;

}


bool
AosIILDfmDocs::commit(
		const AosRundataPtr &rdata,
		const AosDocFileMgrObjPtr &doc_mgr)
{
	mLock->lock();
//OmnScreen << "=============== commit To Dfm index.size():" << mIndex.size() << ";mBuff.dataLen():" << mBuff->dataLen() << endl;
	bool rslt = doc_mgr->saveDocs(rdata, mIndex, mBuff);
	mIndex.clear();
	mLock->unlock();
	return rslt;
}

