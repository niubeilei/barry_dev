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
// 05/21/2012 Created by Chen Ding
// 05/28/2012 Moved from Util/DataAssembler.h
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataAssembler_DataAssembler_h
#define Aos_DataAssembler_DataAssembler_h

#include "DataAssembler/DataAssemblerType.h"
#include "DataAssembler/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/TaskObj.h"
#include "SEInterfaces/DataAssemblerObj.h"
#include "SEInterfaces/DataProcStatus.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/ValueRslt.h"
#include "Util/LocalFile.h"


class AosDataAssembler : public AosDataAssemblerObj
{
	OmnDefineRCObject;

public:
	enum
	{
		eDftBuffMinLen = 10000000,	// 10M
		//eDftBuffMaxLen = 500000000,	// 500M
		eDftBuffMaxLen = 50000000,	// 50M
		//eDftBuffLen = 100000000,	// 100M
		eDftBuffLen = 10000000,	// 10M
		eDftQueueSize = 48,
		eDftQueueMinSize = 10,
		eDftQueueMaxSize = 80,
		eDftMaxFileSize = 500000000 //500M
	};
	
	static int64_t			sgDftBuffLen;
	static int64_t			sgDftQueueSize;

protected:
	OmnMutexPtr				mLock;
	OmnMutex *				mLockRaw;
	AosDataAssemblerType::E	mType;
	OmnString				mTypeName;
	OmnString				mAsmKey;
	u64						mTaskDocid;
	// JACKIE-HADOOP
	bool					mIsHadoop;
	
	AosDataRecordObjPtr		mOutputRecord;		// Ketty 2014/01/09
	AosDataRecordObj *		mOutputRecordRaw;	// Ketty 2014/01/09

	OmnLocalFilePtr			mFile;				// Barry 2015/03/31
public:
	AosDataAssembler(
			const AosDataAssemblerType::E type, 
			const OmnString &name,
			const OmnString &asm_key,
			const u64 task_docid);
	~AosDataAssembler();

	//virtual void 	setTask(const AosTaskObjPtr &task) {mTask = task;}

	virtual u64 	getNextDocid(AosRundata *rdata);
	virtual AosDataRecordObjPtr cloneDataRecord(AosRundata *rdata);
	virtual AosXmlTagPtr getConfig (){return NULL;}

	virtual bool addEntry( 		
						AosRundata *rdata, 
						AosDataRecordObj *record, 
						const u64 docid);

	/*
	virtual AosDataAssemblerObjPtr createIILAssembler(
						AosRundata *rdata, 
						const OmnString &col_id,
						const AosTaskObjPtr &task, 
						const AosXmlTagPtr &def);
	virtual AosDataAssemblerObjPtr createDocAssembler(
						AosRundata *rdata,
						const OmnString &asm_key,
						const AosTaskObjPtr &task, 
						const AosXmlTagPtr &def);


	static AosDataAssemblerObjPtr createAssembler(
						const OmnString &asm_key,
						const AosTaskObjPtr &task, 
						const AosXmlTagPtr &def, 
						AosRundata *rdata);

	static bool		checkIILAsmConfigStatic(
						const AosXmlTagPtr &def,
						AosRundata *rdata);
	*/

	static AosDataAssemblerObjPtr createAssembler(
						const OmnString &asm_key,
						const u64 task_docid, 
						const AosXmlTagPtr &data_col_def, 
						AosRundata *rdata);

	static bool		checkConfigStatic(
						const AosXmlTagPtr &def,
						AosRundata *rdata);

	static bool		init(const AosXmlTagPtr &def);


	// JACKIE-HADOOP
	virtual bool isHadoop() const
	{
		return mIsHadoop;
	}

	// Ketty 2014/01/09
	virtual AosDataRecordObjPtr	getOutputTemplate();
	virtual bool appendEntry(
					AosRundata *rdata,
					AosDataRecordObj *record);
	virtual bool appendEntry(
					const AosValueRslt &value,
					AosRundata *rdata);
	virtual bool appendSortedFile(const u64 &file_id, const AosRundataPtr &rdata){ return false;}
	virtual void tmpFilePost() { }
	virtual void outPutPost() { }
	virtual void streamPost() { }
	virtual void mergeFilePost() { }
	virtual bool waitFinish(AosRundata *rdata) {return false;}
protected:
	bool			config(
						const AosXmlTagPtr &def,
						AosRundata *rdata);

	void showDataAssemblerInfo(
			const char *file,
			const int line,
			const OmnString &action);

};

#endif

