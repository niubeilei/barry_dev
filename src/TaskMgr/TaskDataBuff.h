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
#ifndef AOS_TaskMgr_TaskDataBuff_h
#define AOS_TaskMgr_TaskDataBuff_h

#include "TaskMgr/TaskData.h"
#include "TaskMgr/IDU.h"

class AosTaskDataBuff : public AosTaskData
{
	//int							mPhysicalId;
	AosTaskObjPtr					mTask;
	int								mPkts;
	int								mBytes;
	AosXmlTagPtr					mDoc;
	map<OmnString, OmnString>		mFieldMap;
	OmnString						mActionName;
	OmnString						mAsmName;
	OmnString						mInOut;
	OmnString						mDataId;

	//Data coming from upstreams
	//New data is appended to a list
	//mBuff is the current working buff
	//later on, this buffer can be saved
	//in a dataset
	AosBuffPtr								mBuff;
	deque<AosBuffPtr>						mBuffList;

	//list of IDU data
	map<u64, vector<AosIDU*>*>				mIDUMap;

	//iduId->current SegId
	map<u64, int>							mSegMap;

	OmnMutexPtr								mLock;

public:
	AosTaskDataBuff(const bool flag);
	AosTaskDataBuff(const AosTaskObjPtr task);
	~AosTaskDataBuff();

	virtual int			getPkts() const {return mPkts;}
	virtual void		addPkts(int num) { mPkts += num;}
	virtual int			getBytes() const {return mBytes;}
	virtual void		addBytes(int num) { mBytes += num;}
	virtual bool		insertJobDataStat(const AosRundataPtr &rdata);
	virtual bool		updateJobDataStat(const AosRundataPtr &rdata);
	virtual void		setActionName(OmnString name) { mActionName = name; }
	virtual void		setAsmName(OmnString name) { mAsmName = name; }
	virtual OmnString	getInOut() { return mInOut; }
	virtual void		setInOut(OmnString inout) { mInOut = inout; }
	virtual void 		setDataId(OmnString dataId);
	virtual OmnString   getDataId() { return mDataId; }

	virtual bool 		addBuff(AosBuffPtr buff);
	virtual u64			recvTaskData(AosBuffPtr confBuff, AosBuffPtr dataBuff);
	virtual bool		sendTaskData(u64 iduId, int phyId, AosBuffPtr dataBuff);
	virtual AosIDU*		findNextData(u64 iduId);
	virtual AosBuffPtr 	getNextBuff(u64 iduId);
	virtual bool		hasNextBuff(u64 iduId);
	virtual int			getNextSegId(u64 iduId);
	virtual bool 		updateCounter(int pkts, int bytes);

	virtual bool serializeTo(
				AosXmlTagPtr &xml,
				const AosRundataPtr &rdata);

	virtual bool serializeFrom(
				const AosXmlTagPtr &xml,
				const AosRundataPtr &rdata);

	virtual AosTaskDataObjPtr create() const;

	virtual AosTaskDataObjPtr create(
				const AosXmlTagPtr &def,
				const AosRundataPtr &rdata) const;
							
	virtual bool isDataExist(const AosRundataPtr &rdata);

	virtual bool dataProcessed(AosRundataPtr rdata, u64 iduId);

private:
	bool	config(
				const AosXmlTagPtr &def,
				const AosRundataPtr &rdata);

};
#endif
#endif
