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
//
// Modification History:
// 2013/11/01 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Dataset_Jimos_DatasetByDataScanner_h
#define Aos_Dataset_Jimos_DatasetByDataScanner_h

#include "Dataset/Dataset.h"
#include "SEInterfaces/TaskObj.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"

#include <queue>
class AosDatasetByDataScanner : public AosDataset
{
	OmnDefineRCObject;

	enum
	{
		eInvalid,
		eFailed,
		eFinished,
		eInternalError,
		eFatchingData,
		eDftMaxRecordsets = 5,
		mDefaultBuffSize = 100000000
	};

protected:
	AosRundataPtr				mRundata;
	OmnMutexPtr					mLock;
	OmnCondVarPtr				mCondVar;
	int							mOffset;
	AosBuffDataPtr				mDataBuff;
	AosBuffDataPtr				mRemainingBuff;
	int							mMaxRecordsets;
//	AosDataAssemblerObjPtr		mErrorRcdCollector;
	AosRecordsetObjPtr			mRecordset;
	queue<AosRecordsetObjPtr>	mRecordsets;
	int64_t						mNumEntries;
	int64_t						mNumEntriesInDoc;
	AosXmlTagPtr 				mConf;
	OmnString					mObjid;
	int							mStatus;
	OmnThreadPtr				mThread;
	AosDataSplitterObjPtr		mSplitter;
	queue<AosBuffDataPtr>		mBuffQueue;
	bool						mInComplete;
	bool						mFinished;
	int64_t						mUnrecNumEntries;
	int64_t						mProcSize;
	OmnString 					mName;

protected:
	AosSchemaObjPtr				mSchema;	
	AosDataScannerObjPtr		mDataScanner;

public:
	AosDatasetByDataScanner(
		const OmnString &type, 
		const int version);
	AosDatasetByDataScanner(const int version);
	~AosDatasetByDataScanner();
	
	virtual bool	reset(const AosRundataPtr &rdata);
	virtual int64_t	getNumEntries();
	virtual int64_t	getTotalSize() const;
	virtual void	setTaskDocid(const u64 task_docid);

	virtual bool	nextRecordset(
						const AosRundataPtr &rdata, 
						AosRecordsetObjPtr &recordset);

	virtual bool	stop(
						const AosRundataPtr &rdata, 
						AosRecordsetObjPtr &recordset);

	virtual bool	addDataConnector(
						const AosRundataPtr &rdata, 
	 					const AosDataConnectorObjPtr &data_cube);
	
	virtual bool	addUnrecogContents(
						const AosRundataPtr &rdata,
						const AosBuffDataPtr &buff_data,
						const char *data,
						const int64_t &start_pos,
						const int64_t &length);
	
	virtual bool	addInvalidContents(
						const AosRundataPtr &rdata,
						AosDataRecordObj * record);
	
	virtual bool	addFilterContents(
						const AosRundataPtr &rdata,
						AosDataRecordObj * record);

	virtual bool	sendStart(const AosRundataPtr &rdata);
	virtual bool	sendFinish(const AosRundataPtr &rdata);

	virtual AosRecordsetObjPtr	getRecordset(){return mRecordset;}
	virtual AosRecordsetObjPtr  cloneRecordset(AosRundata *rdata){return mRecordset->clone(rdata);}

	virtual bool	getRecord(const OmnString &name, AosDataRecordObjPtr &record);
	
	virtual bool	getRecords(vector<AosDataRecordObjPtr> &records);

	virtual bool	config(const AosRundataPtr &rdata, const AosXmlTagPtr &worker_doc);

	// Jimo Interface
	virtual AosJimoPtr cloneJimo()  const;
	
	virtual bool	setValueBuff(
						const AosBuffPtr &buff,
						const AosRundataPtr &rdata);

	virtual int64_t	calculateNumEntries(const AosRundataPtr &rdata);
	virtual bool	readingFinished();
	void 			showDatasetInfo(
						const char *file,
						const int line,
						const OmnString &action);

	virtual void report(
			const char *file,
			const int line,
			const OmnString &action,
			const OmnString &msg);

};
#endif

