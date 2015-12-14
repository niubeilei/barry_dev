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
// 2014/01/21 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "DataCube/ReadDocUtil/ReadDocBySortUnit.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "API/AosApiG.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DataConnectorObj.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"

#include "MetaData/DocidMetaData.h"

AosReadDocBySortUnit::AosReadDocBySortUnit(
		const AosRundataPtr &rdata,
		const AosDocOprPtr &doc_opr,
		IdVector &docids)
:
mReqId(0),
mReaded(false),
mDocOpr(doc_opr),
mTotalDocids(docids),
mLock(OmnNew OmnMutex())
{
	sort(docids.begin(), docids.end());
	bool rslt = mDocOpr->shufferDocids(rdata, docids, mSvrIdDocidsMap);
	aos_assert(rslt);
}

AosReadDocBySortUnit::AosReadDocBySortUnit()
{
}

AosReadDocBySortUnit::~AosReadDocBySortUnit()
{
}

void
AosReadDocBySortUnit::include()
{
	OmnNew AosReadDocBySortUnit();
	
	AosDocOpr::include();
}

bool
AosReadDocBySortUnit::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc)
{
	OmnShouldNeverComeHere;
	return true;
}


AosDataConnectorObjPtr 
AosReadDocBySortUnit::cloneDataConnector()
{
	OmnShouldNeverComeHere;
	return 0;
}


AosJimoPtr 
AosReadDocBySortUnit::cloneJimo() const
{
	OmnShouldNeverComeHere;
	return 0;
}


void
AosReadDocBySortUnit::setCaller(const AosDataConnectorCallerObjPtr &caller)
{
	aos_assert(caller);
	mCaller = caller;
}


bool
AosReadDocBySortUnit::readData(const u64 reqid, const AosRundataPtr &rdata)
{
	// this func only called by one time.
	mLock->lock();
	if(isReadFinish())
	{
		mLock->unlock();
		if (mCaller) mCaller->callBack(reqid, 0, true);
	}

	mReqId = reqid;
	mReaded = true;
	mLock->unlock();

	AosAsyncRespCallerPtr resp_caller(this, false);
	map<u32, IdVector>::iterator itr = mSvrIdDocidsMap.begin();
	u64 seq_id = 0;
	for(; itr != mSvrIdDocidsMap.end(); itr++, seq_id++)
	{
		mDocOpr->sendReadDocTrans(rdata, seq_id,
				itr->first, itr->second, resp_caller);
	}
	return true;
}


bool
AosReadDocBySortUnit::isReadFinish()
{
	return mReaded;
}


void
AosReadDocBySortUnit::callback(
		const AosTransPtr &trans, 
		const AosBuffPtr &resp, 
		const bool svr_death)
{
	// this is trans's resp callback.
	aos_assert(!svr_death && resp);
//OmnScreen << "------------------ReadDocBySortUnit getResp." << endl;

	mLock->lock();
	u64 seq_id = resp->getU64(0);
	bool rslt = markRespRecved(seq_id);
	aos_assert_l(rslt, mLock);
	
	bool svr_rslt = resp->getU8(0);
	//aos_assert_l(svr_rslt, mLock);
	if(!svr_rslt)                                                                                         
	{                                                                                                     
		AosDiskStat disk_stat;                                                                            
		AosBuffPtr buff = OmnNew AosBuff(1000 AosMemoryCheckerArgs);                                      
		AosBuffDataPtr buff_data = OmnNew AosBuffData(0, buff, disk_stat);                                

		bool all_recved = isAllRespRecved();
		if (!all_recved)
		{
			mLock->unlock();
			return;
		}

		mTotalDocids.clear();                                                                             
		mSvrIdDocidsMap.clear();                                                                          
		mRespRecved.clear();                                                                              
		mAllResps.clear();                                                                                
		//mCaller = 0;                                                                                      
		mMetaResp = 0;                                                                                    


		mLock->unlock();                                                                                  

		aos_assert(mCaller);                                                                              
		OmnScreen << "------------------ReadDocBySortUnit send empty response. mReqId:" << mReqId << endl;    
		mCaller->callBack(mReqId, buff_data, isReadFinish());                                             

		return;                                                                                           
	}                                                                                                     

	mMetaResp = mDocOpr->getMetaResp(resp);

	rslt = cacheDocidsResp(resp);
	aos_assert_l(rslt, mLock);

	bool all_recved = isAllRespRecved();
	if (!all_recved)
	{
		mLock->unlock();
		return;
	}

	// Ken Lee, 2015/01/06
	//AosBuffPtr resp_buff = getAllResp();
	IdVector schemaids;
	AosBuffPtr resp_buff = mDocOpr->getAllResp(
		mMetaResp, mTotalDocids, schemaids, mAllResps);
	mLock->unlock();
	
	AosDiskStat disk_stat;
	AosBuffDataPtr buff_data = OmnNew AosBuffData(0, resp_buff, disk_stat);

	AosMetaDataPtr metadata = OmnNew AosDocidMetaData(mTotalDocids, schemaids);
	buff_data->setMetadata(metadata);

	mTotalDocids.clear();
	mSvrIdDocidsMap.clear();
	mRespRecved.clear();
	mAllResps.clear();
	//mCaller = 0;
	mMetaResp = 0;

	mLock->unlock();

	aos_assert(mCaller);
OmnScreen << "------------------ReadDocBySortUnit sendResp. mReqId:" << mReqId << endl;
	mCaller->callBack(mReqId, buff_data, isReadFinish());
}


bool
AosReadDocBySortUnit::markRespRecved(const u64 seq_id)
{
	set<u64>::iterator itr = mRespRecved.find(seq_id);
	aos_assert_r(itr == mRespRecved.end(), false);
	
	mRespRecved.insert(seq_id);
	return true;
}


bool
AosReadDocBySortUnit::cacheDocidsResp(const AosBuffPtr &big_resp)
{
	aos_assert_r(big_resp, false);

	bool finished, rslt;
	u64 docid;
	AosBuffPtr docid_resp;
	map<u64, AosBuffPtr>::iterator itr;
	while(1)
	{
		rslt = mDocOpr->getNextDocidResp(big_resp, finished, docid, docid_resp);	
		aos_assert_r(rslt, false);
		
		if(finished)	break;
		aos_assert_r(docid && docid_resp, false);
		
		itr = mAllResps.find(docid);
		aos_assert_r(itr == mAllResps.end(), false);
		mAllResps.insert(make_pair(docid, docid_resp));
	}

	return true;
}


bool
AosReadDocBySortUnit::isAllRespRecved()
{
	return mRespRecved.size() == mSvrIdDocidsMap.size();
}


AosBuffPtr
AosReadDocBySortUnit::getAllResp()
{
	AosBuffPtr big_resp = OmnNew AosBuff(100 AosMemoryCheckerArgs);
	
	if(mMetaResp)	big_resp->setBuff(mMetaResp);

	u64 docid = 0;
	map<u64, AosBuffPtr>::iterator itr;
	AosBuffPtr docid_resp;
	for (u32 i=0; i<mTotalDocids.size(); i++)
	{
		docid = mTotalDocids[i];
		
		itr = mAllResps.find(docid);
		//aos_assert_r(itr != mAllResps.end(), 0);
		if(itr == mAllResps.end())	continue;

		aos_assert_r(docid == itr->first, 0);
		docid_resp = itr->second;
		big_resp->setU64(docid);
		big_resp->setBuff(docid_resp);
	}
	
	return big_resp;
}

