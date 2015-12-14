////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 03/17/2015 Created by Phil
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_StreamEngine_StreamSnapshot_h
#define AOS_StreamEngine_StreamSnapshot_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "XmlUtil/DocTypes.h"
#include "XmlUtil/Ptrs.h"
#include <map>


//
//This class helps to manage data snapshots
//
class AosStreamSnapshot
{
public: 
	enum DataType
	{
		eIIL,
		eDoc,
		eInvalid
	};

private:
	u64				mRDDId;
	u64				mServiceDocid;
	u32 			mVirtualId;	
	u64 			mSnapshotId;
	bool 			mIsCommit;
	DataType		mDataType;
	AosDocType::E 	mDocType;

public:
	AosStreamSnapshot();
	AosStreamSnapshot(u64 rddId,
			u64 serviceDocid,
			u32 virtualId,	
			u64 snapshotId,
			DataType dataType,
			AosDocType::E docType);

	~AosStreamSnapshot();

	bool commit(const AosRundataPtr &rdata);

};

#endif
