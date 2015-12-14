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
// 11/16/2015 Created by Barry
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DataAssembler_DataAssemblerBuff_h
#define AOS_DataAssembler_DataAssemblerBuff_h

#include "DataAssembler/DataAssembler.h"
#include "SEInterfaces/DataRecordType.h"


class AosDataAssemblerBuff : public AosDataAssembler
{
protected:
	i64						mBuffMaxSizeToSend;
	u64						mAppendDataLen;
	u64						mSendDataLen;
	i64						mEntryNum;

	AosBuffPtr				mBuff;
	AosDataRecordType::E 	mRecordType;

public:
	AosDataAssemblerBuff(
			const OmnString &asm_key,
			const u64 task_docid);
	~AosDataAssemblerBuff();

	virtual bool sendStart(AosRundata *rdata);
	virtual bool sendFinish(AosRundata *rdata);
	virtual bool waitFinish(AosRundata *rdata);
	virtual bool appendEntry(
					AosRundata *rdata,
					AosDataRecordObj *record);

	static AosDataAssemblerObjPtr createAssemblerBuff(
					const OmnString &asm_key,
					const u64 task_docid,
					const AosXmlTagPtr &def,
					AosRundata *rdata);

private:
	bool config(
					const AosXmlTagPtr &def,
					AosRundata *rdata);
	bool sendPriv(AosRundata *rdata);
	bool sendBuff(AosRundata *rdata);

};
#endif
