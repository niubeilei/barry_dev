////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: RetransMgr.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Retrans_RetransMgr_h
#define Omn_Retrans_RetransMgr_h

#include "aosUtil/Types.h"
#include "Debug/Rslt.h"
#include "Message/Ptrs.h"
#include "RetransMgr/Ptrs.h"
#include "RetransMgr/RetransTypes.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/Array1000.h"
#include "Util/RCObjImp.h"
#include "UtilComm/Ptrs.h"
#include "XmlParser/Ptrs.h"


OmnDefineSingletonClass(OmnRetransMgrSingleton,
						OmnRetransMgr,
						OmnRetransMgrSelf,
						OmnSingletonObjId::eRetransMgr, 
						"RetransMgr");	


class OmnRetransMgr : public OmnThreadedObj
{
	OmnDefineRCObject;

private:
	enum
	{
		eDefaultTimerSec = 0,
		eDefaultTimerUsec = 500000,		// 500ms
		eDefaultSchedule = 0,
		eDefaultTooManyReq = 1000
	};

	OmnMutexPtr			mLock;
	OmnCondVarPtr		mCondVar;
	OmnThreadPtr		mThread;

	int					mTimerSec;
	int					mTimerUsec;
	OmnRetransSchedules	mSchedules;
	bool				mTooManyReqs;
	int					mTooManyReqShreshold;

	OmnArray1000<OmnRetransTransPtr>		mList;

public:
	OmnRetransMgr();
	~OmnRetransMgr();

	static OmnRetransMgr *	getSelf();
	bool			 start();
	bool			 stop();
	OmnRslt			 config(const OmnXmlParserPtr &def);
	OmnRetransTransPtr addTrans(const OmnMsgPtr &msg,
                          const OmnRetransRequesterPtr &requester, 
						  void *userData);
	OmnRetransTransPtr addTrans(const OmnMsgPtr &msg,
                          const OmnRetransRequesterPtr &requester, 
						  const int scheduleId,
						  void *userData);
	void			 check(const int64_t &msec);

	//
	// OmnThreadedObj interface
	//
	virtual bool threadFunc(OmnThrdStatus::E &state, 
							const OmnSPtr<OmnThread> &thread);
	virtual bool signal(const int threadLogicId);
    virtual bool checkThread(OmnString &errmsg, const int tid) const; 

private:
	void	createDefaultSchedules();
};
#endif
