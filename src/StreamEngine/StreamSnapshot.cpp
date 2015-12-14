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
// 05/14/2012 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "StreamEngine/StreamSnapshot.h"

#include "SEInterfaces/SysInfo.h"
#include "Debug/Debug.h"
#include <stdlib.h>
#include "API/AosApiG.h"

AosStreamSnapshot::AosStreamSnapshot()
{
	mDocType = AosDocType::eNormalDoc;
	mRDDId = 0;
	mVirtualId = 0;
	mSnapshotId = 0;
	mServiceDocid = 0;
	mIsCommit = false;
	mDataType = eDoc;
}

AosStreamSnapshot::AosStreamSnapshot(u64 rddId,
									  u64 serviceDocid,
									  u32 virtualId,	
									  u64 snapshotId,
									  DataType dataType,
								      AosDocType::E docType)
{
	mRDDId = rddId;
	mServiceDocid = serviceDocid;
	mVirtualId = virtualId;
	mSnapshotId = snapshotId;
	mDataType = dataType;
	mDocType = docType;
	mIsCommit = false;
}

AosStreamSnapshot::~AosStreamSnapshot()
{
}

///////////////////////////////////////////
//  snapshot method
///////////////////////////////////////////
bool
AosStreamSnapshot::commit(const AosRundataPtr &rdata)
{
	if (mIsCommit) return true;

	bool rslt;
	AosIILClientObjPtr iil_client = AosIILClientObj::getIILClient();
	aos_assert_r(iil_client, false);

	AosDocClientObjPtr doc_client = AosDocClientObj::getDocClient();
	aos_assert_r(doc_client, false);

	if (mDataType == eDoc)
	{
		rslt = doc_client->commitSnapshot(rdata, 
				mVirtualId, mDocType, mSnapshotId, mServiceDocid);
		aos_assert_r(rslt, false);

		OmnScreen << "Committed doc with Service docid: "
			<< mServiceDocid << " SnapshotId: " << mSnapshotId << endl;
	}
	else if (mDataType == eIIL)
	{
		rslt = iil_client->commitSnapshot(mVirtualId, 
				mSnapshotId, mServiceDocid, rdata);
		aos_assert_r(rslt, false);

		OmnScreen << "Committed iil with Service docid: "
			<< mServiceDocid << " SnapshotId: " << mSnapshotId << endl;
	}

	mIsCommit = true;
	return true;
}

