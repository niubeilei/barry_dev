////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. 
//
// Modification History:
// 10/19/2012 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef AOS_DocServer_DocBatchReader_h
#define AOS_DocServer_DocBatchReader_h

#include "DocServer/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Thread/Mutex.h"
#include "Thread/ThreadedObj.h"
#include "Util/DataTypes.h"

#include <queue>

class AosDocBatchReader : public OmnThreadedObj 
{
	OmnDefineRCObject;

public:
	enum 
	{
		eMaxSize = 3
	};

	enum ReadPolicy
	{
		eInvalid,

		eReadFromAllVirtuals,
		eAllClientsReady,

		eMax
	};

	enum DocBatchType
	{
		eInvalid1,		
		eBatchRead,
		eBatchDelete,
		eMax1
	};

	struct AosStatus : public OmnRCObject
	{
	private:
		OmnDefineRCObject;

		enum E 
		{
			eStart = 1,
			eFinished = 2
		};

	public:
		E			status;
		int	 		num_data_msgs;
		int			total_num_data_msgs;

		AosStatus()
		:
		status(AosStatus::eStart),
		num_data_msgs(0),
		total_num_data_msgs(0)
		{
		}; 
		~AosStatus(){};
	};


	enum FieldType
	{
		eAttr,
		eText,
		eXml
	};


	struct FieldDef
	{
		FieldType		type;
		OmnString		fname;
		AosDataType::E	data_type;
	};


private:
	OmnMutexPtr                 mLock;
	OmnCondVarPtr               mCondVar;
	OmnThreadPtr                mThread;
	AosRundataPtr				mRundata;
	//OmnCondVarPtr               mWaitCondVar;

	OmnString 					mScannerId;
	map<u32, AosStatus>			mClientMap;	
	map<u32, AosBitmapObjPtr>	mBitmaps;
	map<u32, int>				mIndex;
	bool						mFinished;

	// read
	u32							mCrtVidIdx;
	u64 						mBlockSize;
	u32							mQueueSize;
	queue<AosBuffPtr>			mDataQueue;
	u64							mTotalReceivedSize;
	ReadPolicy					mReadPolicy;
	DocBatchType				mBatchType;

	// fnames
	vector<FieldDef>			mFields;
	// result is call back
	bool						mCallBack;
	int							mCallBackServerId;
	int							mNumCallBackDataMsgs;

public:
	AosDocBatchReader(
			const OmnString &scanner_id,
			const AosDocBatchReaderReqPtr &request,
			const AosRundataPtr &rdata);

	~AosDocBatchReader();

	// OmnThreadedObj interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);

	bool    checkThread(OmnString &err, const int thrdLogicId) const;

	bool	setStartClient(const u32 &client_id, const AosRundataPtr &rdata);
	bool	setFinishedClient(const u32 &client_id, const int total_num_data_msgs, const AosRundataPtr &rdata);
	bool	isFinished(const AosRundataPtr &rdata) ;
	bool	setStartRead(const u32 &client_id, const AosRundataPtr &rdata)
			{
				 OmnShouldNeverComeHere;
				 return true;
			}

	u64		getBlockSize() const {return mBlockSize;}

	u64 	getQueueSize() const {return mQueueSize;}

	bool	retrieveDocs(
			const OmnString &scanner_id,
			AosBuffPtr &resp_buff,
			const AosRundataPtr &rdata);

	bool	shuffle(
			const AosBitmapObjPtr &bitmap,
			const u32 &client_id,
			const AosRundataPtr &rdata);

	bool	deleteDocs(
			const OmnString &scanner_id,
			const AosRundataPtr &rdata);

	void 	setCallBack(const bool &b) {mCallBack = b;}
	void 	setCallBackServerId(const int &b) {mCallBackServerId = b;}
private:
	bool 	start();

	bool 	isReadyToReadLocked();

	bool 	allFinishedLocked();

	bool	readWhenAllClientsReady(
			AosBuffPtr &resp_buff,
			const AosRundataPtr &rdata);

	AosBuffPtr	waitOnReadyToRead(const AosRundataPtr &rdata);

	bool	readDocsLocked(
			const int &num_blocks,
			const AosRundataPtr &rdata);

	bool	readFromAllVirtuals(
			AosBuffPtr &resp_buff,
			const AosRundataPtr &rdata);

	ReadPolicy toEnum(const OmnString &read_policy)
	{
		if (read_policy == "allclientready") return eAllClientsReady;
		if (read_policy == "allfromallvirtuals") return eReadFromAllVirtuals;
		return eInvalid;
	}

	bool isValid(const ReadPolicy type)
	{
		return (type > eInvalid && type < eMax);
	}

	bool isValidByBatchType(const DocBatchType type)
	{
		return (type > eInvalid1 && type < eMax1);
	}

	DocBatchType toEnumByBatchType(const OmnString &name)
	{
		const char *data = name.data();
		switch (data[0])
		{
		case 'd':
			 if (name == "delete") return eBatchDelete;
			 break;

		case 'r':
			 if (name == "read") return eBatchRead;
			 break;

		default:
		     OmnAlarm << "Unrecognized  type :" << name << enderr;
		     break;
		}
		return eInvalid1; 
	}

	bool deleteDocsLocked(const AosRundataPtr &rdata);

	bool waitOnReadyToReturn();

	bool isDeleteFinished();

	bool callBackClientLocked(const AosRundataPtr &rdata);

	FieldType toEnumField(const OmnString &s)
	{
		if (s == "attr") return eAttr;
		if (s == "text") return eText;
		return eXml;
	}
};
#endif

#endif

