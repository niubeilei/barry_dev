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
#ifndef AOS_DataScanner_Jimos_DataScannerConnector_h
#define AOS_DataScanner_Jimos_DataScannerConnector_h

#include "DataScanner/DataScanner.h"
#include "SEInterfaces/DataConnectorObj.h"
#include "Thread/Ptrs.h"


class AosDataScannerConnector : public AosDataScanner, public AosDataConnectorCallerObj
{
	OmnDefineRCObject;
	struct SortedBuff                                         
	{
		AosBuffDataPtr          mBuffData;
		int                     mSeqno;

		public:
		SortedBuff(
				const AosBuffDataPtr &buffdata, 
				const int seqno) 
		:
		mBuffData(buffdata),
		mSeqno(seqno)
		{
		}

		bool operator < (const SortedBuff &rhs) const
		{
			return mSeqno > rhs.mSeqno;
		}
		~SortedBuff(){}
	};
	
	enum
	{
		eDftBuffCacheNum = 1,
		eDefaultBuffSize = 10000000
	};

private:
	OmnMutexPtr				mLock;
	OmnMutexPtr				mLock2;
	AosRundataPtr			mRundata;
	OmnCondVarPtr			mCondVar;
	AosDataConnectorObjPtr	mDataConnector;
	AosXmlTagPtr 			mConfig;
	bool					mInComplete;
	bool					mIsFinished;
	int64_t					mOffset;
	AosBuffDataPtr  		mRemainingBuff;
	int						mBuffCacheNum;
//	queue<AosBuffDataPtr>   mBuffQueue;
	priority_queue<SortedBuff> mBuffQueue;
	int						mSeqno;
	int						mCallBackNum;
	int						mCrtSeqno;

public:
	
	AosDataScannerConnector(const AosRundataPtr &rdata, 
				const AosXmlTagPtr &def);
	
	AosDataScannerConnector(const AosRundataPtr &rdata, 
				const OmnString &objid);
	
	AosDataScannerConnector(const u32 version);
	
	~AosDataScannerConnector();

	// Data Scanner Interface
	virtual bool reset(const AosRundataPtr &rdata);
	virtual int 	getPhysicalId() const;
	virtual bool 	getNextBlock(AosBuffDataPtr &info, const AosRundataPtr &rdata);
	virtual bool	serializeTo(const AosBuffPtr &buff, const AosRundataPtr &rdata);
	virtual bool	serializeFrom(const AosBuffPtr &buff, const AosRundataPtr &rdata);
	virtual AosDataScannerObjPtr clone(
						const AosXmlTagPtr &def,  
						const AosRundataPtr &rdata);
	virtual int64_t	getTotalSize() const;

	virtual	bool	setRemainingBuff(
						const AosBuffDataPtr &buffdata,
						const bool incomplete,
						const int64_t proc_size,
						const AosRundataPtr &rdata);

	virtual bool	startReadData(const AosRundataPtr &rdata);
	
	virtual bool config(const AosRundataPtr &rdata, const AosXmlTagPtr &worker_doc);
	
	// jimo interface
	virtual AosJimoPtr cloneJimo() const;
	virtual bool config(
				const AosRundataPtr &rdata, 
				const AosXmlTagPtr &def,
				const AosXmlTagPtr &jimo_doc){return true;}

	// AosDataConnectorCallerObj interface
	virtual void	callBack(
						const u64 &reqId,
						const AosBuffDataPtr &buffdata,
						bool isFinished);
	
	virtual bool setValueBuff(
					const AosBuffPtr &buff,
					const AosRundataPtr &rdata);

private:
	bool initFile(const AosRundataPtr &rdata, const OmnString &objid);
	bool initFile(const AosRundataPtr &rdata, const AosXmlTagPtr &def);
};
#endif
