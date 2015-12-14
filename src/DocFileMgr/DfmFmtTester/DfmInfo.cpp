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
//
// Modification History:
// 04/11/2013	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "DocFileMgr/DfmFmtTester/DfmInfo.h"

#include "SEInterfaces/DocFileMgrObj.h"
#include "API/AosApi.h"
#include "DocTrans/GetDocTesterTrans.h"
#include "DocTrans/DocTesterTrans.h"
#include "DocFileMgr/DfmFmtTester/DocInfo.h"
#include "DfmUtil/DfmDocNorm.h"
#include "DfmUtil/DfmDocDatalet.h"
#include "Thread/RwLock.h"

AosDfmInfo::AosDfmInfo(
		const AosRundataPtr &rdata,
		const u32 vid,
		const bool show_log)
:
mLock(OmnNew OmnMutex()),
mSiteid(rdata->getSiteid()),
mCrtDocInfoNum(0),
mVId(vid),
mShowLog(show_log)
{
}


AosDfmInfo::AosDfmInfo()
:
mLock(OmnNew OmnMutex())
{
}


AosDfmInfo::~AosDfmInfo()
{
}




AosDocInfo3Ptr
AosDfmInfo::addDoc(const AosRundataPtr &rdata, const u64 docid)
{
	if(mCrtDocInfoNum >= eMaxDocs) return 0;
	
	u32 pattern, repeat;
	AosBuffPtr data = AosDocInfo3::randData(pattern, repeat);

	AosTransPtr trans = OmnNew AosDocTesterTrans(1, docid, 
			data->data(), data->dataLen(), true, true);
	AosBuffPtr resp;
	bool svr_death;
	bool rslt = AosSendTrans(rdata, trans, resp, svr_death);
	aos_assert_r(rslt, 0);
	if(svr_death)	return 0;

	aos_assert_r(resp, 0);
	rslt = resp->getU8(0);
	aos_assert_r(rslt, 0);

	mLock->lock();
	AosDocInfo3Ptr doc_info = OmnNew AosDocInfo3(docid, pattern, repeat);
	mDocInfo.insert(make_pair(docid, doc_info));	
	mCrtDocInfoNum++;
	mLock->unlock();

	if(mShowLog)
	{
		OmnScreen << "DfmTester; createDoc"
			<< "; act_docid:" << docid
			<< "; virtual_id:" << mVId << "; "
			<< "size:" << data->dataLen() << "; "
			<< "pattern:" << pattern << "; "
			<< "repeat:" << repeat << "; "
			<< endl;
	}
	
	return doc_info;
}


bool
AosDfmInfo::randModifyDoc(const AosRundataPtr &rdata)
{
	mLock->lock();
	AosDocInfo3Ptr doc_info = randGetDocInfo(); 
	if(!doc_info)
	{
		mLock->unlock();
		return true;
	}
	doc_info->lock();
	mLock->unlock();

	u32 pattern, repeat;
	AosBuffPtr data = AosDocInfo3::randData(pattern, repeat);

	AosTransPtr trans = OmnNew AosDocTesterTrans(1, doc_info->getDocid(),
			data->data(), data->dataLen(), true, true);
	AosBuffPtr resp;
	bool svr_death;
	bool rslt = AosSendTrans(rdata, trans, resp, svr_death);
	if (svr_death)
	{
		doc_info->unlock();
		return true;
	}

	aos_assert_r(rslt && resp, false);
	rslt = resp->getU8(0);
	aos_assert_r(rslt, false);
	doc_info->reset(pattern, repeat);
	doc_info->unlock();

	if (mShowLog)
	{
		OmnScreen << "DfmTester; modifyDoc"                                        
			<< "; act_docid:" << doc_info->getDocid() 
			<< "; docid:" << doc_info->getLocalDocid() 
			<< "; virtual_id:" << mVId << "; "
			<< "size:" << data->dataLen() << "; "
			<< "pattern:" << pattern << "; "
			<< "repeat:" << repeat << "; "
			<< endl;
	}
	return true; 
}


bool
AosDfmInfo::randDeleteDoc(const AosRundataPtr &rdata)
{
	mLock->lock();
	AosDocInfo3Ptr doc_info  = randGetDocInfo(); 
	if(!doc_info)
	{
		mLock->unlock();
		return true;
	}
	doc_info->lock();
	
	u64 docid = doc_info->getDocid();
	map<u64, AosDocInfo3Ptr>::iterator itr = mDocInfo.find(docid);
	if(itr == mDocInfo.end())
	{
		doc_info->unlock();
		mLock->unlock();
		return true;
	}
	
	mDocInfo.erase(itr);
	mCrtDocInfoNum--;
	mLock->unlock();

	//u64 docid = doc_info->getDocid();

	//map<u64, AosDocInfo3Ptr>::iterator itr = mDocInfo.find(docid);
	//if(itr == mDocInfo.end())
	//{
	//	doc_info->unlock();
	//	return true;
	//}

	AosTransPtr trans = OmnNew AosDocTesterTrans(2, docid, 0, 0, true, true);
	AosBuffPtr resp;
	bool svr_death;
	bool rslt = AosSendTrans(rdata, trans, resp, svr_death);
	if (svr_death)
	{
		doc_info->unlock();
		return true;
	}

	aos_assert_r(rslt && resp, false);
	rslt = resp->getU8(0);
	aos_assert_r(rslt, false);
	
	//mLock->lock();
	//mDocInfo.erase(itr);
	//mCrtDocInfoNum--;
	//mLock->unlock();
	
	doc_info->unlock();
	
	if (mShowLog)
	{
		OmnScreen << "DfmTester; deleteDoc"
			<< "; act_docid:" << docid 
			<< "; virtual_id:" << mVId << "; "
			<< "pattern:" << doc_info->getPattern() << "; "
			<< "repeat:" << doc_info->getRepeat() << "; "
			<< endl;
	}
	return true;
}


bool
AosDfmInfo::randReadDoc(const AosRundataPtr &rdata)
{
	mLock->lock();
	AosDocInfo3Ptr doc_info  = randGetDocInfo(); 
	if(!doc_info)
	{
		mLock->unlock();
		return true;
	}
	doc_info->lock();
	mLock->unlock();

	u32 loc_docid = doc_info->getLocalDocid();

	if (mShowLog)
	{
		OmnScreen << "DfmTester; start readDoc"
			<< "; act_docid:" << doc_info->getDocid() 
			<< "; docid:" << loc_docid  
			<< "; virtual_id:" << mVId 
			<< endl;
	}

	AosTransPtr trans = OmnNew AosGetDocTesterTrans(doc_info->getDocid(), true, true);
	AosBuffPtr resp;
	bool svr_death;
	bool rslt = AosSendTrans(rdata, trans, resp, svr_death);
	if(svr_death)
	{
		doc_info->unlock();	
		return true;
	}

	aos_assert_r(rslt && resp, false);
	u8 ss = resp->getU8(0);
	if(ss == 3)
	{
		// means this trans proc timeout.
		doc_info->unlock();
		return true;
	}
	//rslt = resp->getU8(0);
	aos_assert_r(ss, false);

	u32 doc_len = resp->getU32(0);
	aos_assert_r(doc_len, 0);
	AosBuffPtr doc_buff = resp->getBuff(doc_len, false AosMemoryCheckerArgs);

	bool is_same = doc_info->isSameData(doc_buff);
	doc_info->unlock();
	
	if(!is_same)
	{
		OmnAlarm << "error!" << enderr;
		return false;
	}

	return true;
}


AosDocInfo3Ptr
AosDfmInfo::randGetDocInfo()
{
	//mLock->lock();
	if(mDocInfo.size() == 0)
	{
		//mLock->unlock();
		return 0;
	}
	
	int idx = rand() % mDocInfo.size();
	map<u64, AosDocInfo3Ptr>::iterator itr = mDocInfo.begin();
	while(idx--)	itr++;
	AosDocInfo3Ptr doc_info = itr->second;
	
	//mLock->unlock();
	return doc_info;
}


bool
AosDfmInfo::serializeFrom(
		const AosBuffPtr &buff)
{
	aos_assert_r(buff, false);
	
	mVId = buff->getU32(0);
	mSiteid = buff->getU32(0);
	u32 id = buff->getU32(0);

	u32 snap_nums = buff->getU32(0);

	u32 doc_nums = buff->getU32(0);
	AosDocInfo3Ptr doc_info;
	u64 docid;
	for(u32 i=0; i<doc_nums; i++)
	{
		doc_info = AosDocInfo3::serializeFromStatic(buff);	
		aos_assert_r(doc_info, false);
		docid = doc_info->getDocid();
		
		mDocInfo.insert(make_pair(docid, doc_info));	
	}

	mCrtDocInfoNum = buff->getU32(0);
	buff->getU32(0);
	return true;
}


bool
AosDfmInfo::serializeTo(const AosBuffPtr &buff)
{
	aos_assert_r(buff, false);
	
	buff->setU32(mVId);
	buff->setU32(mSiteid);
	u32 id = 10;
	buff->setU32(id);
	
	//if (mShowLog)
	{
		OmnScreen << "DfmInfo:: serializeTo"
			<< "; doc Num:" << mDocInfo.size() 
			<< endl;
	}
	
	buff->setU32(0);
	
	buff->setU32(mDocInfo.size());
	map<u64, AosDocInfo3Ptr>::iterator doc_itr = mDocInfo.begin();
	for(; doc_itr != mDocInfo.end(); doc_itr++)
	{
		AosDocInfo3Ptr doc = doc_itr->second;
		aos_assert_r(doc, false);
		if (mShowLog)
		{
			OmnScreen << "DfmInfo:: serialize doc:" 
				<< "; docid:" << doc->getDocid() 
				<< "; pattern:" << doc->getPattern() 
				<< endl;
		}
		doc->serializeTo(buff);
	}

	buff->setU32(mCrtDocInfoNum);
	buff->setU32(0);
	return true;
}


