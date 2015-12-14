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
// 03/19/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DataAssembler_FileAssembler_h
#define AOS_DataAssembler_FileAssembler_h

#include "DataAssembler/DataAssembler.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/DataRecordType.h"

class AosFileAssembler : public AosDataAssembler
{
protected:
	i64						mBuffMaxSizeToSend;
	bool					mFinished;

	bool					mNeedDeleteIIL;
	bool					mTrueDelete;
	OmnString				mIILName;

// for test
	u64						mAppendDataLen;
	u64						mSendDataLen;
	i64						mEntryNum;

	AosXmlTagPtr			mConfig;
	AosBuffPtr				mBuff;
	AosDataRecordType::E 	mRecordType;
	OmnString				mFileName;

	friend class AosDataAssembler;
	
public:
	AosFileAssembler(
			const AosDataAssemblerType::E type,
			const OmnString &name, 
			const OmnString &asm_key,
			const u64 task_docid);
	~AosFileAssembler();

	virtual bool sendStart(AosRundata *rdata);
	virtual bool sendFinish(AosRundata *rdata);
	virtual bool waitFinish(AosRundata *rdata);

	virtual bool appendEntry(
					AosRundata *rdata,
					AosDataRecordObj *record);
	virtual bool appendEntry(
					const AosValueRslt &value,
					AosRundata *rdata);

	virtual AosXmlTagPtr getConfig() {return mConfig;}
protected:
	virtual bool config(
					const AosXmlTagPtr &def,
					AosRundata *rdata);
	
	bool configV1(
					const AosXmlTagPtr &def,
					AosRundata *rdata);

	virtual bool sendPriv(AosRundata *rdata);

private:
	static AosDataAssemblerObjPtr createFileAssemblerV1(
					const OmnString &asm_key,
					const u64 task_docid,
					const AosXmlTagPtr &def,
					AosRundata *rdata);

	bool 	createFile(AosRundata *rdata);
	void 	showDataAssemblerInfo();
};
#endif

