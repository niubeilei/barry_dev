
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 09/27/2013	Create by Young
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SeLogClient_SeLogClient_h
#define AOS_SeLogClient_SeLogClient_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/Opr.h"
#include "Query/Ptrs.h"
#include "Rundata/Rundata.h"
#include "SeLogClient/Ptrs.h"
#include "SeLogUtil/LogNames.h"
#include "SEInterfaces/SeLogClientObj.h"
#include "SEInterfaces/Ptrs.h"
#include "Thread/Mutex.h"
#include "Thread/ThrdShellProc.h"
#include "Thread/ThreadedObj.h"
#include "Util/HashUtil.h"
#include "Util1/TimerObj.h"
#include "TransClient/Ptrs.h"
#include "TransUtil/RoundRobin.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "SeLogUtil/LogOpr.h"
#include "SeLogClient/LogGroup.h"

#include <vector>
#include <map>

OmnDefineSingletonClass(AosSeLogClientSingleton,
						AosSeLogClient,
						AosSeLogClientSelf,
						OmnSingletonObjId::eSeLogClient,
						"SeLogClient");

class AosSeLogClient :  public AosSeLogClientObj, 
						public OmnThreadedObj,
						public OmnTimerObj 
{
	OmnDefineRCObject;

private:
	static bool					mStartAddLog;
	static bool					mNeedToSend;

private:
	enum
	{
		eDftAgingRule = 0,
		eTimerSec = 1,
		eTimerUsec = 500,
		eDftWaitUsec = 500,
		eDftEntryNums = 2048,
		eDftMaxCacheSize = 50000000,  	// 50MB
		eDftKeepSize = 10000000,		// 10MB
		eDftKeepTime = 30,  			// 30Sec
		
		eMax = 1
	};

	u32 									mEntryNums;
	u32 									mWaitUsec;
	u32										mTimerSec;
	u32										mTimerUsec;
	u64										mCacheSize;
	u64										mMaxCacheSize;
		
	OmnMutexPtr    			  				mLock;
	OmnMutexPtr 							mLockRaw;
	OmnThreadPtr							mThread;
	OmnCondVarPtr							mCondVar;
	OmnThreadPtr							mSendFmtThrd;
	AosRundataPtr							mRundata;
	map<OmnString, AosLogGroupPtr>			mLogObjidMap;

public:
	AosSeLogClient();
	~AosSeLogClient();

	// Singleton Class Interface
	static AosSeLogClient*    	getSelf();
	virtual bool				start();
	virtual bool 				stop();
	virtual bool				config(const AosXmlTagPtr &def);

	static void					startLog();

	class addLogDocThrd : public OmnThrdShellProc
	{
		OmnDefineRCObject;

		AosSeLogClientPtr 	mClientObj;
		AosXmlTagPtr		mLog;
		AosRundataPtr		mRundata;
		
	public:
		addLogDocThrd(
			const AosSeLogClientPtr &clientObj,
			const AosXmlTagPtr &log,
			const AosRundataPtr &rdata)
		:
		OmnThrdShellProc("addLogDocThrd"),
		mClientObj(clientObj),
		mLog(log),
		mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
		{
		}

		virtual bool    run();          
		virtual bool    procFinished(); 
	};

	// ThreadedObj interface
	virtual bool	threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool	signal(const int threadLogicId);

	// TimerObj interface
	virtual void timeout(const int timerId,           
			        const OmnString &timerName,
					void *parm);

	// Log Interface
	virtual bool createLogger(	
							const AosRundataPtr &rdata, 
	 						const OmnString &log_name);

	virtual bool removeLogger(	
							const AosRundataPtr &rdata, 
	 						const OmnString &log_name);

	virtual u64 addLog(	const AosRundataPtr &rdata, 
	 						const OmnString &log_objid,
	 						const OmnString &entry);

	virtual bool addLog(
							const AosRundataPtr &rdata,
							const OmnString &log_objid, 
							const OmnString &log_name,
							const OmnString &entry);

	virtual bool	addSysLog(
				const bool &isSuccess,
				const OmnString &logname,
				const AosRundataPtr &rdata);
	
	virtual bool retrieveLog(
				const AosRundataPtr &rdata, 
				const OmnString &log_name,
	 			const u64 entry_id, 
				OmnString &entry,
				const AosLogCallerPtr &caller);

	u64 	addLog(
				const AosRundataPtr &rdata,
				const OmnString &log_objid, 
				const AosXmlTagPtr &log);

	bool 	addLog(
	 			const AosRundataPtr &rdata,
	 			const OmnString &log_objid, 
	 			const OmnString &log_name,
				const AosXmlTagPtr &log);

	u64 	addLogWithResp(
				const OmnString &pctr_objid, 
				const OmnString &log_name,
				const AosXmlTagPtr &log_contents,
				const AosRundataPtr &rdata);

	bool	createLogBySdoc(
				const AosXmlTagPtr &sdoc,
				const OmnString &logname,
				const OmnString &tagname, 
				const AosRundataPtr &rdata);

	bool	retrieveIILId(
				const OmnString &log_objid,
				OmnString &logname,
				const AosRundataPtr &rdata);	

	bool	retrieveLogs(
				const AosRundataPtr &rdata, 
				const OmnString &log_name,
	 			const vector<u64> &entry_id, 
				vector<OmnString> &entries,
				const AosLogCallerPtr &caller);
	
	bool 	addLoginLog(
				const AosXmlTagPtr &logdoc, 
				const AosRundataPtr &rdata)
	{
		aos_assert_rr(logdoc, rdata, false);
		OmnString ctnr_objid = logdoc->getAttrStr(AOSTAG_PARENTC);
		aos_assert_rr(ctnr_objid != "", rdata, false);
		return addLog(rdata, ctnr_objid, AOSLOGNAME_LOGIN, logdoc);
	}

	bool 				getLog( 		
							const OmnString &siteid,
							const OmnString &logtype,
							const OmnString &logid, 
							const int pagesize,
							const bool reverse,
							OmnString &contents, 
							AosXmlRc &errcode, 
							OmnString &errmsg);

	AosXmlTagPtr 		getLogEntry(
							const AosRundataPtr &rdata,
							const u64 &docid);

	bool				addInvalidReadEntry(
							const AosXmlTagPtr &doc, 
							const AosRundataPtr &rdata);

	bool 				createWhoVisitedLogEntry(
							u32 &seqno,
							u64 &offset,
							const AosRundataPtr &rdata);

	u64	 				addVersion(
							const AosXmlTagPtr &doc,
							const AosRundataPtr &rdata);
	
	bool 				getVersionDoc(
							const OmnString &log_objid,
							const OmnString &objid,
							const OmnString &version,
							AosXmlTagPtr &doc, 
							const AosRundataPtr &rdata);
private:
	bool				addLogTransResp(
							const AosTransPtr &trans,
							AosBuffPtr &resp,
							const AosRundataPtr &rdata);
	bool 				addLogTrans(
							const AosTransPtr &trans,
							const AosRundataPtr &rdata);

	u64					addToMap(
							const AosRundataPtr &rdata,
							const OmnString &log_objid,
							const OmnString &log);

public:
	// 09/18/2013 Create by Young
	OmnString 			getLog(
							const AosRundataPtr &rdata,
							const u64 logid);

	bool				agingWithTime(
							const AosRundataPtr &rdata, 
							const u64 &aging_time_sec);

	bool				agingWithSize(
							const AosRundataPtr &rdata,
							const u64 aging_size_bytes);

	bool				agingWithSizeAndTime(
							const AosRundataPtr &rdata, 
							const u64 &aging_size_bytes,
							const u64 &aging_time_sec);


private:
	// Young, 2013/11/07
	bool 				addReq(
							const AosRundataPtr &rdata,
							const AosTransPtr &trans,
							AosBuffPtr &resp);

	bool				deleteLogs(
							const u64 &groupId,
							const AosRundataPtr &rdata);

	bool 				modifyIILAndDeleteLogs(
							const OmnString &iilname,
							map<u64, u64> &groupIdMap,
							const AosRundataPtr &rdata);

	bool				deleteIILs(
							const AosRundataPtr &rdata,
							const OmnString &iilname,
							map<u64, u64> groupidMap);

	map<u64, u64>		runQuery(
							const AosRundataPtr &rdata,
							const OmnString &iilname,
							const AosQueryContextObjPtr &query_context);

	AosQueryContextObjPtr getQueryContext(
							const AosOpr opr = eAosOpr_an,
							const u64 block_size = 100000,
							const u64 u64value = 0);
public:
	// Chen Ding, 2014/11/08
	virtual bool addLog(	AosRundata *rdata, 
	 						const OmnString &log_name,
	 						AosBuff *entry);

};
#endif

