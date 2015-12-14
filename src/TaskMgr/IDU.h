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
// 04/28/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef AOS_TaskMgr_IDU_h
#define AOS_TaskMgr_IDU_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/TaskObj.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "XmlUtil/DocTypes.h"
#include "XmlUtil/Ptrs.h"
#include <map>

#define IDU_STATUS_SENT "SENT"
#define IDU_STATUS_RECVED "RECVED"
#define IDU_STATUS_INPROCESS "INPROCESS"
#define IDU_STATUS_PROCESSED "PROCESSED"

class AosIDU
{
	//int			mPhysicalId;
	AosTaskObjPtr	mTask;
	OmnString 		mDataId; 
	u64 			mIDUId; 
	int				mSegId; 
	int				mSendPhyId; 
	u64				mSendTaskId; 
	int				mRecvPhyId; 
	u64				mRecvTaskId;  
	OmnString		mStatus;
	int				mProcTime; //in second
	AosBuffPtr		mBuff;
	OmnMutexPtr		mLock;
	AosXmlTagPtr	mDoc;
	u64				mDocId;

	//time info
	u64				mSendTimeStamp;
	u64				mRecvTimeStamp;
	u64				mInUseTimeStamp;
	u64				mProcessedTimeStamp;

	//fieldMap to system table
	map<OmnString, OmnString>	mFieldMap;

public:
	AosIDU(
			AosBuffPtr confBuff,
			AosBuffPtr dataBuff);

	AosIDU();
	~AosIDU();


	bool serializeTo(
			AosBuffPtr &buff);

	bool serializeFrom(
			AosBuffPtr &buff);

	bool send(AosRundataPtr rdata);
	u64 recv(AosRundataPtr rdata,
			AosBuffPtr confBuff,
			AosBuffPtr dataBuff);

	bool inProcess(AosRundataPtr rdata);
	bool processed(AosRundataPtr rdata);
	bool isInProcess(){ return (mStatus == IDU_STATUS_INPROCESS); }
	bool isProcessed(){ return (mStatus == IDU_STATUS_PROCESSED); }
	bool isSameIDU(AosIDU *idu);

	bool insertJobData(const AosRundataPtr &rdata);
	bool updateJobData(const AosRundataPtr &rdata);
	void buildFieldMap();

	//getters/setters
	void setBuff(AosBuffPtr buff) { mBuff = buff; }
	AosBuffPtr getBuff() { return mBuff; }
	void setIDUId(u64 iduId) { mIDUId = iduId; }
	u64 getIDUId() { return mIDUId; }
	void setDataId(OmnString dataId) { mDataId = dataId; }
	void setSegId(int segId) { mSegId = segId; }
	void setSendPhyId(int phyId) { mSendPhyId = phyId; }
	void setSendTaskId(u64 taskId) { mSendTaskId = taskId; }
	void setRecvPhyId(int phyId) { mRecvPhyId = phyId; }
	void setRecvTaskId(u64 taskId) { mRecvTaskId = taskId; }

};

#endif
#endif
