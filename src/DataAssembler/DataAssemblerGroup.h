////////////////////////////////////////////////////////////////////////////
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
// 06/13/2015 Created by Jozhi
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DataAssembler_DataAssemblerGroup_h
#define AOS_DataAssembler_DataAssemblerGroup_h

#include "DataAssembler/DataAssembler.h"
#include "Thread/Mutex.h"
#include "SEInterfaces/DataAssemblerObj.h"
#include "SEInterfaces/DataRecordObj.h"
#include "Rundata/Rundata.h"
#include "Util/ValueRslt.h"
#include "XmlUtil/XmlTag.h"
#include <vector>


class AosDataAssemblerGroup : public AosDataAssembler
{
private:
	int								mShuffleNum;
	int								mShuffleId;
	AosValueRslt					mValue;
	OmnString						mShuffleType;
	vector<AosDataAssemblerObjPtr>	mAssemblers;
	vector<AosDataAssemblerObj*>	mRawAssemblers;
	AosExprObjPtr					mShuffleField;
	AosExprObj*						mRawShuffleField;

	OmnMutexPtr						mLock;
	OmnMutex*						mRawLock;

public:
	AosDataAssemblerGroup(
			const OmnString &asm_key,
			const u64 task_docid);
	~AosDataAssemblerGroup();

	virtual bool sendStart(AosRundata *rdata);

	virtual bool sendFinish(AosRundata *rdata);
	virtual bool waitFinish(AosRundata *rdata);

	virtual bool appendEntry(
					AosRundata *rdata,
					AosDataRecordObj *record);

	virtual bool config(
					const AosXmlTagPtr &def,
					AosRundata *rdata);

	static bool checkConfigStatic(
					const AosXmlTagPtr &def,
					AosRundata* rdata);

	static AosDataAssemblerObjPtr createAssembler(
					const OmnString &asm_key, 
					const u64 task_docid, 
					const AosXmlTagPtr &def, 
					AosRundata* rdata);

private:
	int getShuffleId(AosDataRecordObj *record, AosRundata *rdata);
};
#endif

