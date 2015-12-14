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
#ifndef AOS_StreamEngine_RDD_h
#define AOS_StreamEngine_RDD_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Ptrs.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/TaskObj.h"
#include "SEInterfaces/RecordsetObj.h"
#include "SEInterfaces/DataProcObj.h"
#include "DataRecord/Recordset.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "XmlUtil/DocTypes.h"
#include "XmlUtil/Ptrs.h"
#include <map>
#include "StreamEngine/Ptrs.h"

#define RDD_STATUS_INIT "INIT"
#define RDD_STATUS_SENT "SENT"
#define RDD_STATUS_RECVED "RECVED"
#define RDD_STATUS_INPROCESS "INPROCESS"
#define RDD_STATUS_PROCESSED "PROCESSED"

class AosRDD : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	bool						mIsStreamRecordset;
	int							mStartNum;
	int							mFinishNum;
	int							mTotalNum;
	u64							mStartTime;
	u64							mFinishTime;

	u64 						mRDDId; 
	u64							mRecvTimeStamp;
	u64							mInUseTimeStamp;
	u64							mProcessedTimeStamp;
	u64							mDocId;
	u64							mProcTime;
	OmnString 					mDataId;
	OmnString					mServiceId;
	OmnString					mSendDataProcName;
	OmnString					mRecvDataProcName;
	OmnMutexPtr					mLock;
	AosXmlTagPtr				mDoc;
	AosRecordsetObjPtr			mRecordset;
	AosRecordsetObj*			mRecordsetRaw;
	map<OmnString, OmnString>	mFieldMap;

public:
	AosRDD();
	AosRDD(
		const OmnString &data_id,
		const OmnString &send_dp_name,
		const OmnString &recv_dp_name,
		const OmnString &service_id,
		const u64 &rdd_id,
		const AosRecordsetObjPtr &rs,
		const AosRundataPtr &rdata);

	AosRDD(const AosRDD &rdd, const AosRundataPtr &rdata);

	~AosRDD();

	AosRDDPtr	clone(const AosRundataPtr &rdata);

	bool start();
	bool finish(const int remain);
	bool isFinished();
	bool setStartTime();
	bool setFinishTime();
	bool setTotalNum(const int total);
	u64 getStartTime();
	u64 getFinishTime();
	bool sort(AosRundata* rdata, const AosCompareFunPtr &comp);
	bool appendRecord(AosRundata* rdata, AosDataRecordObjPtr &rcd);
	bool isEmpty();

	bool serializeTo(const AosRundataPtr &rdata, const AosBuffPtr &buff);

	bool serializeFrom(const AosRundataPtr &rdata, const AosBuffPtr &buff);

	bool isSameRDD(AosRDD *rdd);

	bool insertJobData(const AosRundataPtr &rdata);
	bool updateJobData(const AosRundataPtr &rdata);
	void buildFieldMap();

	//getters/setters
	void setRDDId(const u64 &rddId) 
	{ 
		mRDDId = rddId; 
	}
	void setIsStreamRecordset(const bool flag) { mIsStreamRecordset = flag;}
	void setDataId(const OmnString &dataId) { mDataId = dataId; }
	void setServiceId(const OmnString &serviceId) { mServiceId = serviceId; }
	void setData(const AosRecordsetObjPtr &rs) { mRecordset = rs; mRecordsetRaw = mRecordset.getPtr();}
	void setSendDataProcName(const OmnString &name) { mSendDataProcName = name; }
	void setRecvDataProcName(const OmnString &name) { mRecvDataProcName = name; }

	u64 getRDDId() { return mRDDId; }
	OmnString getDataId() { return mDataId; }
	OmnString getServiceId() { return mServiceId; }
	AosRecordsetObjPtr getData() { return mRecordset; }
	i64 getDataLen();
	i64 getEntryLen();
	OmnString getSendDataProcName() { return mSendDataProcName; }
	OmnString getRecvDataProcName() { return mRecvDataProcName; }
	u64	getProcTime() { return mProcTime; }

};

#endif
