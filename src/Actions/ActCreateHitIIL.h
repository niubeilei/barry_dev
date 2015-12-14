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
// 2012/02/23	Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Aos_SdocAction_ActCreateHitIIL_h
#define Aos_SdocAction_ActCreateHitIIL_h

#include "Actions/SdocAction.h"
#include "Actions/Ptrs.h"
#include "SEInterfaces/ActionCaller.h"
#include "IILUtil/IILUtil.h"
#include "Rundata/Ptrs.h"
#include "TransUtil/Ptrs.h"



class AosActCreateHitIIL : virtual public AosSdocAction, public AosActionCaller 
{
private:
	enum
	{
		eMaxBuffSize = 100000000  //100M
	};

	OmnString					mIILType;
	int							mRcdLen;
	AosDataScannerObjPtr		mScanner;
	AosRundataPtr				mRundata;
	u64							mFileId;
	int							mPhysicalId;
	int64_t 					mMaxLength;
	int64_t						mProcLength;
	int64_t						mCrtPos;
	int							mIILNameOffset;
	int							mIILNameLen;
	int							mValueOffset;
	int							mValueLen;
	u64							mTaskDocid;
	map<u32, u64>				mSnapMaps;
	bool						mComposeIILName;
	OmnString					mAttrName;
	OmnString					mTableName;
	bool						mBuildBitmap;

public:
	AosActCreateHitIIL(const bool flag);
	AosActCreateHitIIL(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	~AosActCreateHitIIL();

	virtual bool run(
			const AosTaskObjPtr &task, 
			const AosXmlTagPtr &sdoc,
			const AosRundataPtr &rdata);
	
	virtual AosActionObjPtr clone(
			const AosXmlTagPtr &def,
			const AosRundataPtr &rdata) const;

	virtual void callBack(
			const u64 &reqId, 
			const int64_t &expected_size, 
			const bool &finished);

	virtual bool createSnapShot(const AosTaskObjPtr &task, const AosRundataPtr &rdata);

private:
	bool 	config(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	bool	createHitIIL(
				const AosBuffPtr &buff,
				const AosRundataPtr &rdata);

	bool	strAddIIL(
				const AosBuffPtr &buff,
				const AosRundataPtr &rdata);
	//bool	sanitycheck(const AosBuffPtr &buff);
	bool	sanitycheck(const vector<u64> &docids);
	u64		getSnapShotId(const OmnString &iilname);
};
#endif

#endif
