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
// 2013/11/21 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DataScanner_Jimos_DataScannerParallel_h
#define AOS_DataScanner_Jimos_DataScannerParallel_h

#include "DataScanner/DataScanner.h"
#include "SEInterfaces/DataConnectorObj.h"
#include "Thread/Ptrs.h"


class AosDataScannerParallel : public AosDataScanner, public AosDataConnectorCallerObj
{
	OmnDefineRCObject;

	struct ConnectorBuff
	{
	public:
		bool					mInComplete;
		bool					mIsFinished;
		int64_t					mOffset;
		AosBuffDataPtr  		mRemainingBuff;
		AosBuffDataPtr			mCrtBuff;
		AosDataConnectorObjPtr	mConnectorFile;
		int64_t					mDataLen;
		int64_t					mProcSize;

	public:
		ConnectorBuff(const AosDataConnectorObjPtr &cube_file)
		:
		mInComplete(false),
		mIsFinished(false),
		mOffset(0),
		mRemainingBuff(0),
		mCrtBuff(0),
		mConnectorFile(cube_file),
		mDataLen(0),
		mProcSize(0)
		{
		}

		~ConnectorBuff()
		{
		}

		void reset()
		{
			mInComplete = false;
			mIsFinished = false;
			mRemainingBuff = 0;
			mCrtBuff = 0;
			mOffset = 0;
			mDataLen =0;
			mProcSize = 0;
		}
	};

	enum
	{
		eDftConnectorNum = 4,
		eDefaultBuffSize = 10000000
	};

private:
	OmnMutexPtr			mLock;
	OmnCondVarPtr		mCondVar;
	AosRundataPtr 		mRundata;
	AosDataConnectorObjPtr	mDataConnector;
	AosXmlTagPtr 		mConfig;
	int					mConnectorNum;
	vector<ConnectorBuff*>	mConnectors;
//	AosDatasetObjPtr    mCaller;
	queue<u64>			mReqIds;
	int					mReqId;
	bool				mNoConnector;
	int 				mCallBackNum;
	int 				mReadNum;
	int					mDataConnectorNum;
	int					mFinishedNum;
	int64_t				mReadSize;
	int64_t				mProcSize;

public:
	
	AosDataScannerParallel(const AosRundataPtr &rdata, 
				const AosXmlTagPtr &def);
	
	AosDataScannerParallel(const AosRundataPtr &rdata, 
				const OmnString &objid);
	
	AosDataScannerParallel(const u32 version);
	
	~AosDataScannerParallel();

	// Data Scanner Interface
	virtual int 	getPhysicalId() const;
	virtual bool 	getNextBlock(AosBuffDataPtr &info, const AosRundataPtr &rdata);
	virtual bool	serializeTo(const AosBuffPtr &buff, const AosRundataPtr &rdata);
	virtual bool	serializeFrom(const AosBuffPtr &buff, const AosRundataPtr &rdata);
	virtual AosDataScannerObjPtr clone(
						const AosXmlTagPtr &def,  
						const AosRundataPtr &rdata);
	virtual int64_t	getTotalSize() const;

	//virtual bool	readData(const AosRundataPtr &rdata);

	//virtual void    setCaller(const AosDatasetObjPtr &caller);

	virtual	bool	setRemainingBuff(
						const AosBuffDataPtr &buffdata,
						const bool incomplete,
						const int64_t proc_size,
						const AosRundataPtr &rdata);

	virtual bool	startReadData(const AosRundataPtr &rdata);
	
	// AosDataConnectorCallerObj interface
	virtual void	callBack(
						const u64 &reqId,
						const AosBuffDataPtr &buffdata,
						bool isFinished);

	// jimo interface
	virtual AosJimoPtr cloneJimo() const;
	virtual bool config(
				const AosRundataPtr &rdata, 
				const AosXmlTagPtr &def);
private:
	bool initFile(const AosRundataPtr &rdata, const OmnString &objid);
	bool initFile(const AosRundataPtr &rdata, const AosXmlTagPtr &def);
};
#endif
