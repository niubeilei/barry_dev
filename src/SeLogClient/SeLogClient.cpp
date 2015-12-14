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
// How to Handle Docids
// 1. When a buff is full or it is ready to send, get a log docid
// 	  group docid from the backend. A group contains N log docids.
// 2. Get a doc docid.
// 3. Add the pair [log_group_docid, groupId] to an IIL.
// 4. Send the buff to the backend based on doc docid. 
//
// Modification History:
// 09/27/2013	Create by Young
////////////////////////////////////////////////////////////////////////////
#include "SeLogClient/SeLogClient.h"

#include "SingletonClass/SingletonImpl.cpp"
#include "alarm_c/alarm.h"
#include "API/AosApi.h"
#include "Alarm/Alarm.h"
#include "Actions/ActCreateDoc.h"
#include "Actions/Ptrs.h"
#include "Debug/Debug.h"
#include "SEInterfaces/DocClientObj.h"
//#include "DocProc/DocProc.h"
#include "DocTrans/BatchAddSmallDocTrans.h"
#include "DocTrans/AgingSmallDocTrans.h"
#include "EventMgr/EventTriggers.h"
#include "EventMgr/Event.h"
#include "SEUtil/IILName.h"
#include "LogTrans/RetrieveLogTrans.h"
#include "LogTrans/RetrieveLogsTrans.h"
#include "QueryUtil/QrUtil.h"
#include "Query/QueryReq.h"
#include "QueryClient/QueryTrans.h"
#include "QueryClient/QueryClient.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "SeLogUtil/VersionId.h"
#include "SeLogUtil/LogId.h"
#include "SEUtil/Docid.h"
#include "SEUtil/DocZTGs.h"
#include "SmartDoc/SmartDoc.h"
#include "Thread/ThreadPool.h"
#include "Util/OmnNew.h"
#include "AppMgr/App.h"
#include "Util1/Wait.h"
#include "Util1/Timer.h"
#include "Util/Opr.h"
#include "IILUtil/IILId.h"
#include "DocTrans/GetSmallDocTrans.h"
#include "SEInterfaces/IILClientObj.h"


OmnSingletonImpl(AosSeLogClientSingleton,
				 AosSeLogClient,
				 AosSeLogClientSelf,
				 "AosSeLogClient");


static OmnThreadPoolPtr sgThreadPool = OmnNew OmnThreadPool("selogclient", __FILE__, __LINE__);
bool AosSeLogClient::mStartAddLog = false;
bool AosSeLogClient::mNeedToSend = false;

AosSeLogClient::AosSeLogClient()
:
mEntryNums(0),
mTimerSec(1),
mTimerUsec(5000),
mCacheSize(0),
mMaxCacheSize(eDftMaxCacheSize),
mLock(OmnNew OmnMutex()),
//mLockRaw(mLock.getPtr()),
mCondVar(OmnNew OmnCondVar)
{
}


AosSeLogClient::~AosSeLogClient()
{
}


bool
AosSeLogClient::config(const AosXmlTagPtr &config)
{
	AosXmlTagPtr logConfig = config->getFirstChild("SeLogClt");
	if (!logConfig)
	{
		OmnAlarm << "No SeLogClient config!" << enderr;
		return false;
	}

	mWaitUsec = logConfig->getAttrInt("wait_usec", eDftWaitUsec);
	if (mWaitUsec < 10) mWaitUsec = 10;
	mEntryNums = logConfig->getAttrInt("entry_nums", eDftEntryNums);
	if (mEntryNums < eDftEntryNums) mEntryNums = eDftEntryNums;
	//mKeepTime = logConfig->getAttrInt("keep_time", eDftKeepTime);
	//if (mKeepTime < eDftKeepTime) mKeepTime = eDftKeepTime;
	//mKeepSize = logConfig->getAttrInt("keep_size", eDftKeepSize);
	//if (mKeepSize < eDftKeepSize) mKeepSize = eDftKeepSize;

	return true;
}


void
AosSeLogClient::startLog()
{
	mStartAddLog = true;
}


bool
AosSeLogClient::start()
{
	mLock->lock();
	OmnTimerObjPtr thisptr(this, false);
	OmnTimer::getSelf()->startTimer("selogtimer", mTimerSec, mTimerUsec, thisptr, 0);

	if (!mThread)
	{
		OmnThreadedObjPtr thisptr(this, false);
		mThread = OmnNew OmnThread(thisptr, "log_client", 0, false, true, __FILE__, __LINE__);
		mThread->start();
	}
	mLock->unlock();
	return true;
}


bool
AosSeLogClient::stop()
{
	return true;
}


u64
AosSeLogClient::addLog(
		const AosRundataPtr &rdata,
		const OmnString &log_objid, 
		const AosXmlTagPtr &log)
{
	aos_assert_r(log, false);

	// Young, 2013/11/29
	return addLog(rdata, log_objid, log->toString());
}
	

bool
AosSeLogClient::addLog(
		const AosRundataPtr &rdata,
		const OmnString &log_objid, 
		const OmnString &log_name,
		const OmnString &log_cont)
{
	AosXmlParser parser;
	AosXmlTagPtr log = parser.parse(log_cont, "" AosMemoryCheckerArgs); 
	return addLog(rdata, log_objid, log_name, log);
}


u64
AosSeLogClient::addVersion(
		const AosXmlTagPtr &doc, 
		const AosRundataPtr &rdata)
{
	return 1;
}


bool 
AosSeLogClient::getVersionDoc(
		const OmnString &log_objid,
		const OmnString &objid,
		const OmnString &version,
		AosXmlTagPtr &doc, 
		const AosRundataPtr &rdata)
{
	return true;
}


bool
AosSeLogClient::retrieveLog(
		const AosRundataPtr &rdata, 
		const OmnString &log_name,
	 	const u64 entry_id, 
		OmnString &entry,
		const AosLogCallerPtr &caller)
{
	// Chen Ding, 2013/06/17
	OmnNotImplementedYet;
	return false;
}


bool
AosSeLogClient::retrieveIILId(
		const OmnString &log_objid,
		OmnString &logname,
		const AosRundataPtr &rdata)
{
	// Ketty 2013/03/20
	OmnNotImplementedYet;
	return false;
}


bool                                     
AosSeLogClient::addInvalidReadEntry(     
		const AosXmlTagPtr &doc,     
		const AosRundataPtr &rdata)  
{                                        
	OmnNotImplementedYet;                
	return true;                         
}                                        


bool
AosSeLogClient::createLogBySdoc(
		const AosXmlTagPtr &sdoc,
		const OmnString &logname,
		const OmnString &tagname, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(sdoc, 0);
	AosXmlTagPtr log = AosSmartDoc::createDoc(sdoc, tagname, rdata);
	aos_assert_r(log, 0);

	return addLog(rdata, log->getAttrStr(AOSTAG_PARENTC), logname, log);
}

bool
AosSeLogClient::addLog(
        const AosRundataPtr &rdata,
        const OmnString &log_objid,               
        const OmnString &log_typeid,
        const AosXmlTagPtr &log)
{
	// 2014/11/04 Young
	//OmnNotImplementedYet;
	return true;
}


bool
AosSeLogClient::addSysLog(
		const bool &isSuccess,
		const OmnString &logname,
		const AosRundataPtr &rdata)
{
	// This funciton create system'log.
	if (!mStartAddLog) return true;

	aos_assert_r(logname != "", false);
	OmnString state = isSuccess ? "true" : "false";	
	// Retrieve the smartdoc defined by system which 
	// docid is AOSDOCID_SYSLOG_SMARTDOC
	AosXmlTagPtr smartdoc = AosDocClientObj::getDocClient()->getDocByObjid(
			AOSCTNR_SYSLOGSMARTDOC, rdata);
	if (!smartdoc)
	{
		AosSetError(rdata, AosErrmsgId::eInternalError);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosXmlTagPtr def = smartdoc->getFirstChild(logname);
	OmnString str = "<SysLog/>";

	AosXmlParser parser;
	AosXmlTagPtr log = parser.parse(str, "" AosMemoryCheckerArgs);
	aos_assert_r(log, false);

	if (def)
	{
		return false;
		/*
		AosActionObjPtr crtdoc = OmnNew AosActCreateDoc(false);
		aos_assert_r(crtdoc, false);
		aos_assert_r(crtdoc->run(def, rdata), false);
		log = rdata->getCreatedDoc();
		if (!log)
		{
			AosSetError(rdata, AosErrmsgId::eExceptionCreateDoc);
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
		*/
	}

	log->setAttr(AOSTAG_SYSSERVER, state);
	log->setAttr(AOSTAG_CREATOR, rdata->getCid());
	log->setAttr(AOSTAG_LOGNAME, logname);
	OmnString ctime = OmnGetTime(AosLocale::getDftLocale());
	log->setAttr(AOSTAG_CTIME, ctime);
	log->setAttr(AOSTAG_LOG_CTIME, ctime);
	log->setAttr(AOSTAG_PARENTC, AOSCTNR_SYSLOGCTNR);
	log->setAttr(AOSTAG_OTYPE, AOSOTYPE_LOG);
	log->setAttr(AOSTAG_CTNR_PUBLIC, "true");

	OmnString contents = rdata->getResults();
	//Ice Yu  2013/5/27
	//bool rslt = addLog(AOSCTNR_SYSLOGCTNR, logname, log, rdata);
	//aos_assert_r(rslt, false);
	if (rdata->getCid() == "")
	{
		rdata->setCid(AOSCLOUDID_GUEST);
	}	
	
	//log = AosCreateDoc(log, rdata);
	//aos_assert_r(log, false);

	// Ken Lee, 2013/06/13
	OmnThrdShellProcPtr runner = OmnNew addLogDocThrd(this, log, rdata);
	aos_assert_r(sgThreadPool, false);
	bool rslt = sgThreadPool->proc(runner);
	aos_assert_r(rslt, false);

	rdata->setResults(contents);
	return true;
}


bool
AosSeLogClient::retrieveLogs(
		const AosRundataPtr &rdata, 
		const OmnString &log_name,
	 	const vector<u64> &entry_id, 
		vector<OmnString> &entries,
		const AosLogCallerPtr &caller)
{
	// Chen Ding, 2013/06/17
	OmnNotImplementedYet;
	return false;
}


bool
AosSeLogClient::addLogTransResp(
		const AosTransPtr &trans,
		AosBuffPtr &resp,
		const AosRundataPtr &rdata)
{
	if (!mStartAddLog) return 0;

	bool timeout;
	bool rslt = AosSendTrans(rdata, trans, timeout, resp);
	if(!rslt || !resp)
	{
		rdata->setError() << "fail to addLogTrans: " << trans;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}
	if (timeout)
	{
		rdata->setError() << "Timeout";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}
	
	rslt = resp->getU8(0);
	aos_assert_r(rslt, false);
	
	return true;
}


bool
AosSeLogClient::addLogTrans(
		const AosTransPtr &trans,
		const AosRundataPtr &rdata)
{
	//if (!mStartAddLog) return true;
	
	bool rslt = AosSendTrans(rdata, trans); 
	if(!rslt)
	{
		rdata->setError() << "fail to addLogTrans: " << trans ;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	return true;
}


bool
AosSeLogClient::addLogDocThrd::run()
{
	AosXmlTagPtr clone = mLog->clone(AosMemoryCheckerArgsBegin);
	//AosXmlTagPtr log = AosCreateDoc(clone, mRundata);
	//aos_assert_r(log, false);

	//return true;
	
	return mClientObj->addLog(mRundata, "sys_log_objid", mLog->toString());
}


bool
AosSeLogClient::addLogDocThrd::procFinished()
{
	return true;
}


bool 
AosSeLogClient::createLogger(	
		const AosRundataPtr &rdata, 
	 	const OmnString &log_name)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosSeLogClient::removeLogger(	
		const AosRundataPtr &rdata, 
	 	const OmnString &log_name)
{
	OmnNotImplementedYet;
	return false;
}


u64
AosSeLogClient::addLog(
		const AosRundataPtr &rdata,
		const OmnString &log_objid,
		const OmnString &log)
{
	mRundata = rdata;
	aos_assert_r(log_objid != "", 0);
	u64 logid = addToMap(rdata, log_objid, log);
	aos_assert_r(logid > 0, 0);
	return logid;
}


u64
AosSeLogClient::addToMap(
		const AosRundataPtr &rdata,
		const OmnString &log_objid,
		const OmnString &log)
{
	mLock->lock();
	u64 logCreateTime =  OmnGetSecond();
	map<OmnString, AosLogGroupPtr>::iterator logObjidMapItr = mLogObjidMap.find(log_objid);
	if (logObjidMapItr == mLogObjidMap.end())
	{
		AosLogGroupPtr logGroup = OmnNew AosLogGroup(rdata, mEntryNums);
		aos_assert_rl(logGroup, mLock, 0);
		u64 logid = logGroup->putLogIntoBuff(logCreateTime, log);
		mLogObjidMap[log_objid] = logGroup;
		mLock->unlock();
		return logid;
	}
	mLock->unlock();
	
	AosLogGroupPtr logGroup = logObjidMapItr->second;
	aos_assert_r(logGroup, false);
	u64 logid = logGroup->putLogIntoBuff(logCreateTime, log);

	// u32 logEntryLen = entry_type.length() + userdata.length() + 
	// 		log.length() + sizeof(int) * 3 + sizeof(u64) * 2;
	// mCacheSize += logEntryLen;
	// if (mCacheSize > mMaxCacheSize)
	// {
	// 	    mNeedToSend = true;
	// 	    mCondVar->signal();
	// }

	if (logGroup->isFull())
	{
		// The buff is full. Need to send it.
		u64 groupId = logGroup->getDocid();
		u64 groupTime = logGroup->getCreatorTime();
		u64 groupSize = logGroup->getGroupSize();
		AosBuffPtr buff = logGroup->getBuff(rdata);
		aos_assert_r(buff && buff->dataLen() > 0, 0);

		// Build index for Query
		u64 userid = rdata->getUserid();
		AosXmlTagPtr logconf = AosXmlParser::parse(log AosMemoryCheckerArgs);
		aos_assert_r(logconf, 0);
		AosDocClientObj::getDocClient()->addCreateLogRequest(userid, logconf, rdata);

		// Build index for Aging
		AosIILClientObj::getIILClient()->addU64ValueDoc(AOSZTG_LOG_GROUP_TIME,
				groupTime, groupId, false, false, rdata);
		AosIILClientObj::getIILClient()->addU64ValueDoc(AOSZTG_LOG_GROUP_SIZE,
				groupSize, groupId, false, false, rdata);

		AosTransPtr trans = OmnNew AosBatchAddSmallDocTrans(groupId,
				mEntryNums, buff, rdata);
		addLogTrans(trans, rdata);
		return logid;
	}
	
	return logid;
}


bool
AosSeLogClient::signal(const int threadLogicId)
{
	return true;
}


bool
AosSeLogClient::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	// 1. start a new thread 
	mNeedToSend = false;
	while(state == OmnThrdStatus::eActive)
	{
		mLock->lock();
		if (mNeedToSend)
		{
			if (mLogObjidMap.size() <= 0)
			{
				mCondVar->wait(mLock);
				mLock->unlock();
				continue;
			}
		}
		else
		{
			mCondVar->wait(mLock);
			mLock->unlock();
			continue;
		}

		// 2. send buffs by Trans
		map<OmnString, AosLogGroupPtr>::iterator logObjidMapItr = mLogObjidMap.begin();
		aos_assert_rl(logObjidMapItr != mLogObjidMap.end(), mLock, false);
		for (u32 i=0; i<mLogObjidMap.size(); i++)
		{
			AosLogGroupPtr logGroup = logObjidMapItr->second;
			aos_assert_rl(logGroup, mLock, false);
			logObjidMapItr++;
			//if (logGroup->getSentFlag()) continue;

			u64 groupId = logGroup->getDocid();
			AosBuffPtr buff = logGroup->getBuff(mRundata);
			aos_assert_rl(buff, mLock, false);
			if (buff->dataLen() > 0)
			{
				AosTransPtr trans = OmnNew AosBatchAddSmallDocTrans(groupId, mEntryNums, buff, mRundata);
				addLogTrans(trans, mRundata);
			}
			//logGroup->setSentFlag();
		}

		//mCacheSize = 0;
		mNeedToSend = false;
		mLock->unlock();
	}

	return true;
}


void 
AosSeLogClient::timeout(
		const int timerId,           
		const OmnString &timerName,
		void *parm)
{
	mLock->lock();
	mNeedToSend = true;
	mCondVar->signal();
	mLock->unlock();

	OmnTimerObjPtr thisptr(this, false);
	OmnTimer::getSelf()->startTimer("selogtimer", mTimerSec, mTimerUsec, thisptr, 0);
}


OmnString
AosSeLogClient::getLog(
		const AosRundataPtr &rdata,
		const u64 logid)
{
	mLock->lock();
	aos_assert_r(rdata, "");
	AosTransPtr trans = OmnNew AosGetSmallDocTrans(logid, mEntryNums, false, true);
	
	AosBuffPtr resp;
	bool rslt = addReq(rdata, trans, resp);
	aos_assert_rl(rslt && resp, mLock, 0);
	rslt = resp->getU8(0);
	aos_assert_rl(rslt, mLock, "");
	u32 logLen = resp->getU32(0);
	//aos_assert_rl(logLen > 0, mLock, "");
	if (logLen <= 0) 
	{
		// This log isnot exist
		mLock->unlock();
		return "";
	}
	
	u64 logCtime = resp->getU64(0);
	aos_assert_r(logCtime > 0, "");
	OmnString log = resp->getOmnStr("");
	aos_assert_rl(log != "", mLock, "");
	mLock->unlock();

	return log;
}


bool 
AosSeLogClient::addReq(
		const AosRundataPtr &rdata,
		const AosTransPtr &trans,
		AosBuffPtr &resp)
{
	bool timeout;
	bool rslt = AosSendTrans(rdata, trans, timeout, resp);
	aos_assert_rr(rslt, rdata, false);
	if (timeout || !resp)
	{
		OmnString errmsg = timeout ? "SeLogClient no responding" : "Missing response!";
		rdata->setError() << errmsg;
		OmnAlarm << errmsg << enderr;
		return false;
	}
	return true;
}


bool
AosSeLogClient::deleteLogs(
		const u64 &groupId,
		const AosRundataPtr &rdata)
{
	aos_assert_r(groupId > 0, false);
	//aos_assert_r(AosDocType::getDocType(groupId) == AosDocType::eBinaryDoc, false);
	aos_assert_r(AosDocType::getDocType(groupId) == AosDocType::eLogDoc, false);
	AosTransPtr trans = OmnNew AosAgingSmallDocTrans(groupId, false, false);
	addLogTrans(trans, rdata);

	return true;
}


map<u64, u64>
AosSeLogClient::runQuery(
		const AosRundataPtr &rdata,
		const OmnString &iilname,
		const AosQueryContextObjPtr &queryContext)
{
	map<u64, u64> groupIdMap;
	AosQueryRsltObjPtr queryRslt = AosQueryRsltObj::getQueryRsltStatic(); 
	queryRslt->setWithValues(true);
	AosIILClientObj::getIILClient()->querySafe(iilname, queryRslt, 0, queryContext, rdata);

	u64 groupId = 0;
	u64 groupInfo = 0;
	bool finished = false;
	while (queryRslt->nextDocidValue(groupId, groupInfo, finished, rdata))
	{
		if (finished) break;
		aos_assert_r(groupId > 0 && groupInfo > 0, groupIdMap);
		groupIdMap[groupId] = groupInfo;
	}

	return groupIdMap;
}


bool 
AosSeLogClient::modifyIILAndDeleteLogs(
		const OmnString &iilname,
		map<u64, u64> &groupIdMap,
		const AosRundataPtr &rdata)
{
	map<u64, u64>::iterator itr = groupIdMap.begin();
	while (itr != groupIdMap.end())
	{
		u64 groupId = itr->first;
		u64 groupInfo = itr->second;
		AosIILClientObj::getIILClient()->removeU64ValueDoc(iilname, groupInfo, groupId, rdata);
		bool rslt = deleteLogs(groupId, rdata);
		aos_assert_r(rslt, false);
		itr++;
	}
	return true;
}


AosXmlTagPtr 		
AosSeLogClient::getLogEntry(
		const AosRundataPtr &rdata,
		const u64 &docid)
{
	aos_assert_r(docid > 0, 0);
	OmnString logstr = getLog(rdata, docid);
	aos_assert_r(logstr != "", NULL);
	
	return AosXmlParser::parse(logstr AosMemoryCheckerArgs);
}


bool
AosSeLogClient::agingWithTime(
		const AosRundataPtr &rdata,
		const u64 &aging_time_sec)
{
	// This function aging the logs that before aging_time_sec(second).
	// When delete logs, it will modify the index 
	aos_assert_r((u64)OmnGetSecond() > aging_time_sec, false); 

	// 1. query time's iil 
	map<u64, u64> groupid_time_map = runQuery(rdata, AOSZTG_LOG_GROUP_TIME,
			getQueryContext(eAosOpr_le, 1000000, aging_time_sec));

	// 2. pick out size's iil
	map<u64, u64> tmp_size_map = runQuery(rdata, AOSZTG_LOG_GROUP_SIZE,
			getQueryContext(eAosOpr_an, 1000000, 0));
	map<u64, u64> groupid_size_map;
	for (map<u64, u64>::iterator itr = groupid_time_map.begin(); 
			itr != groupid_time_map.end(); itr++)
	{
		aos_assert_r(tmp_size_map.count(itr->first) == 1, false);
		groupid_size_map[itr->first] = tmp_size_map[itr->first];
	}

	// 3. delete indexes
	deleteIILs(rdata, AOSZTG_LOG_GROUP_TIME, groupid_time_map);
	deleteIILs(rdata, AOSZTG_LOG_GROUP_SIZE, groupid_size_map);

	// 4. delete logs
	for (map<u64, u64>::iterator itr = groupid_time_map.begin();
			itr != groupid_time_map.end(); itr++)
	{
		bool rslt = deleteLogs(itr->first, rdata);
		aos_assert_r(rslt, false);
	}

	return true;
}


bool				
AosSeLogClient::agingWithSize(
		const AosRundataPtr &rdata,
		const u64 aging_size_bytes)
{
	// This function aging the logs that the earlist "aging_size_bytes"
	// When delete logs, it will modify the index 
	
	// 1. query size's indexes
	map<u64, u64> tmp_size_map = runQuery(rdata, AOSZTG_LOG_GROUP_SIZE,
			getQueryContext(eAosOpr_an, 1000000, 0));
	u64 totalSize = 0;
	map<u64, u64> groupid_size_map;
	for (map<u64, u64>::iterator itr = tmp_size_map.begin();
			itr != tmp_size_map.end(); itr++)
	{
		totalSize += itr->second;	
		if (totalSize >= aging_size_bytes) break;
		groupid_size_map[itr->first] = itr->second;
	}

	// 2. pick out time's indexes
	map<u64, u64> tmp_time_map = runQuery(rdata, AOSZTG_LOG_GROUP_TIME, 
			getQueryContext(eAosOpr_an, 1000000, 0));
	map<u64, u64> groupid_time_map;
	for (map<u64, u64>::iterator itr = groupid_size_map.begin();
			itr != groupid_size_map.end(); itr++)
	{
		aos_assert_r(tmp_time_map.count(itr->first) == 1, false);
		groupid_time_map[itr->first] = tmp_time_map[itr->first];
	}

	// 3. delete indexes
	deleteIILs(rdata, AOSZTG_LOG_GROUP_TIME, groupid_time_map);
	deleteIILs(rdata, AOSZTG_LOG_GROUP_SIZE, groupid_size_map);

	// 4. delete logs
	for (map<u64, u64>::iterator itr = groupid_time_map.begin();
			itr != groupid_time_map.end(); itr++)
	{
		bool rslt = deleteLogs(itr->first, rdata);
		aos_assert_r(rslt, false);
	}

	return true;
}


bool
AosSeLogClient::agingWithSizeAndTime(
		const AosRundataPtr &rdata, 
		const u64 &aging_size_bytes,
		const u64 &aging_time_sec)
{
	// This function aging the logs that the earlist "aging_size_bytes"
	// When delete logs, it will modify the index 
	
	// 1. query (size&time)'s indexes and join indexes
	// 1.1 query size's indexes
	map<u64, u64> whole_size_map = runQuery(rdata, AOSZTG_LOG_GROUP_SIZE, 
			getQueryContext(eAosOpr_an, 1000000, 0));
	u64 totalSize = 0;
	map<u64, u64> tmp_size_map;
	for (map<u64, u64>::iterator itr = whole_size_map.begin();
			itr != whole_size_map.end(); itr++)
	{
		totalSize += itr->second;	
		if (totalSize >= aging_size_bytes) break;
		tmp_size_map[itr->first] = itr->second;
	}

	// 1.2 query time's indexes
	map<u64, u64> tmp_time_map = runQuery(rdata, AOSZTG_LOG_GROUP_TIME, 
			getQueryContext(eAosOpr_le, 1000000, eAosOpr_le));

	map<u64, u64> whole_time_map = runQuery(rdata, AOSZTG_LOG_GROUP_TIME, 
			getQueryContext(eAosOpr_an, 1000000, 0));

	// 1.3 join docid, then query
	map<u64, u64> groupid_time_map;
	map<u64, u64> groupid_size_map;
	tmp_size_map.insert(tmp_time_map.begin(), tmp_time_map.end());
	for (map<u64, u64>::iterator itr = tmp_size_map.begin(); 
			itr != tmp_time_map.end(); itr++)
	{
		aos_assert_r(whole_size_map.count(itr->first), false);
		aos_assert_r(whole_time_map.count(itr->first), false);
		groupid_time_map[itr->first] = whole_time_map[itr->first];
		groupid_size_map[itr->first] = whole_size_map[itr->first];
	}

	// 3. delete indexes
	deleteIILs(rdata, AOSZTG_LOG_GROUP_TIME, groupid_time_map);
	deleteIILs(rdata, AOSZTG_LOG_GROUP_SIZE, groupid_size_map);

	// 4. delete logs
	for (map<u64, u64>::iterator itr = groupid_time_map.begin();
			itr != groupid_time_map.end(); itr++)
	{
		bool rslt = deleteLogs(itr->first, rdata);
		aos_assert_r(rslt, false);
	}

	return true;
}


bool
AosSeLogClient::deleteIILs(
		const AosRundataPtr &rdata,
		const OmnString &iilname,
		map<u64, u64> groupidMap)
{
	for(map<u64, u64>::iterator itr = groupidMap.begin();
			itr != groupidMap.end(); itr++)
	{
cout << "logDocid: " << itr->first << "\tloginfo: " << itr->second << endl;
		AosIILClientObj::getIILClient()->removeU64ValueDoc(iilname,
				itr->second, itr->first, rdata);
	}

	return true;
}


// Chen Ding, 2014/11/08
bool 
AosSeLogClient::addLog(	
		AosRundata *rdata, 
	 	const OmnString &log_name,
	 	AosBuff *entry)
{
	OmnNotImplementedYet;
	return false;
}


AosQueryContextObjPtr
AosSeLogClient::getQueryContext(
		const AosOpr opr,
		const u64 block_size, 
		const u64 u64value)
{
	AosQueryContextObjPtr queryContext = AosQueryContextObj::createQueryContextStatic();
	queryContext->setBlockSize(1000000);
	queryContext->setOpr(opr);
	queryContext->setStrValue(OmnStrUtil::itoa(u64value));
	return queryContext;
}
