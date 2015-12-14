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
// 2013/04/26	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SdocAction_ActDeleteGroupDoc_h
#define Aos_SdocAction_ActDeleteGroupDoc_h

#include "Actions/SdocAction.h"
#include "Actions/Ptrs.h"
#include "DataAssembler/DataAssemblerType.h"
#include "SEInterfaces/ActionCaller.h"
#include "SEInterfaces/SengineImportDocObj.h"
#include "Thread/ThrdShellProc.h"
#include "Thread/Sem.h"
#include "Rundata/Ptrs.h"
#include <queue>
#include <map>
#include "Debug/Debug.h"


class AosActDeleteGroupDoc : virtual public AosSdocAction,
						   public AosActionCaller
{
	enum
	{
		eMaxBlockSize = 100000000 //100M
	};

	struct FileInfo
	{
		u64 	mFileId;
		int		mPhysicalId;
	};

	AosDataScannerObjPtr			mScanner;
	AosRundataPtr					mRundata;
	OmnMutexPtr     				mLock;
	OmnCondVarPtr   				mCondVar;
	u64								mTaskDocid;
	vector<FileInfo>				mFileInfos;

public:
	AosActDeleteGroupDoc(const bool flag);
	~AosActDeleteGroupDoc();

	// ActionCaller Interface
	virtual void callBack(
			const u64 &reqId, 
			const int64_t &expected_size, 
			const bool &finished); 

	virtual bool run(
			const AosTaskObjPtr &task, 
			const AosXmlTagPtr &sdoc,
			const AosRundataPtr &rdata);

	virtual AosActionObjPtr clone(
			const AosXmlTagPtr &def,
			const AosRundataPtr &rdata) const;
	virtual bool createSnapShot(const AosTaskObjPtr &task, const AosRundataPtr &rdata);
	virtual bool storageEngineError(const AosRundataPtr &rdata);
private:
	bool deleteFiles();

	class deleteDocThrd : public OmnThrdShellProc
	{
		OmnDefineRCObject;
	private:
		AosActDeleteGroupDoc*	mDelGroupDoc;
		AosBuffPtr				mBuff;
		bool					mFinished;
	public:
		deleteDocThrd(
				AosActDeleteGroupDoc* delgroupdoc,
				const AosBuffPtr &buff,
				const bool &finished)
			:
		OmnThrdShellProc("deleteDocThrd"),             
		mDelGroupDoc(delgroupdoc),                                       
		mBuff(buff),
		mFinished(finished)
		{
		}

		virtual bool    run();          
		virtual bool    procFinished(){return true;} 
	};
public:
	void deleteData(AosBuffPtr &buff, const bool &finished);
	
};
#endif

