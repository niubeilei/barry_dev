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
// 2014/10/27	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DbTrans/DbTrans.h"

#include "Rundata/Rundata.h"
#include "SEModules/SnapshotIdMgr.h"
#include "API/AosApi.h"
#include "SEInterfaces/IILClientObj.h"
#include "SEInterfaces/DocClientObj.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosDbTrans_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosDbTrans(version);
		aos_assert_r(jimo, 0);
		return jimo;
	}

	catch (...)
	{
		AosSetErrorUser(rdata, "Failed creating jimo") << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}


AosDbTrans::AosDbTrans(const int version)
:
AosDbTransObj(version)
{
}


bool 
AosDbTrans::start(AosRundata *rdata)
{

	mSnapshotId = AosSnapshotIdMgr::getSelf()->createSnapshotId(rdata);
	aos_assert_r(mSnapshotId, false);
	OmnString transdoc="<trans ";
	transdoc << AOSTAG_OTYPE << "=\"" << AOSOTYPE_DBTRANS << "\" ";
	transdoc << AOSTAG_SNAPSHOTID << "=\"" << mSnapshotId << "\" ";
	transdoc << AOSTAG_STATUS << "=\"" << "start" << "\">";
	mDoc = AosCreateDoc(transdoc, true, rdata);	
	aos_assert_r(mDoc, false);
	return true;
}


bool 
AosDbTrans::commit(AosRundata *rdata)
{
	bool rslt = AosModifyDocAttrStr(rdata, mDoc, AOSTAG_STATUS, "commit", false, true);
	aos_assert_r(rslt, false);

	if (mSnapshots.empty())
	{
		for (int i=0; i<AosGetNumVirtuals(); i++)
		{
			SnapshotInfo snapshot;
			snapshot.mVirtualId = i;
			snapshot.mDocType = AosDocType::eNormalDoc;
			commitPriv(rdata, snapshot);
			snapshot.mVirtualId = i;
			snapshot.mDocType = AosDocType::eLogDoc;
			commitPriv(rdata, snapshot);
			snapshot.mVirtualId = i;
			snapshot.mDocType = AosDocType::eGroupedDoc;
			commitPriv(rdata, snapshot);
			snapshot.mVirtualId = i;
			snapshot.mDocType = AosDocType::eBinaryDoc;
			commitPriv(rdata, snapshot);
			snapshot.mVirtualId = i;
			snapshot.mDocType = AosDocType::eIIL;
			commitPriv(rdata, snapshot);
		}
	}

	for (size_t i=0; i<mSnapshots.size(); i++)	
	{
		return commitPriv(rdata, mSnapshots[i]);
	}

	rslt = AosModifyDocAttrStr(rdata, mDoc, AOSTAG_STATUS, "finish", false, true);
	aos_assert_r(rslt, false);
	return true;
}


bool 
AosDbTrans::commitPriv(AosRundata *rdata, SnapshotInfo &snapshot)
{
	if (snapshot.mDocType == AosDocType::eIIL)
	{
		AosIILClientObjPtr iilclient = AosIILClientObj::getIILClient();
		bool rslt = iilclient->commitSnapshot(snapshot.mVirtualId, mSnapshotId, 0, rdata);
		aos_assert_r(rslt, false);
	}
	else {
		AosDocClientObjPtr docclient = AosDocClientObj::getDocClient();
		bool rslt = docclient->commitSnapshot(rdata, snapshot.mVirtualId, snapshot.mDocType, mSnapshotId, 0); 
		aos_assert_r(rslt, false);
	}
	return true;
}


bool 
AosDbTrans::rollback(AosRundata *rdata)
{
	bool rslt = AosModifyDocAttrStr(rdata, mDoc, AOSTAG_STATUS, "rollback", false, true);
	aos_assert_r(rslt, false);

	if (mSnapshots.empty())
	{
		for (int i=0; i<AosGetNumVirtuals(); i++)
		{
			SnapshotInfo snapshot;
			snapshot.mVirtualId = i;
			snapshot.mDocType = AosDocType::eNormalDoc;
			rollbackPriv(rdata, snapshot);
			snapshot.mVirtualId = i;
			snapshot.mDocType = AosDocType::eLogDoc;
			rollbackPriv(rdata, snapshot);
			snapshot.mVirtualId = i;
			snapshot.mDocType = AosDocType::eGroupedDoc;
			rollbackPriv(rdata, snapshot);
			snapshot.mVirtualId = i;
			snapshot.mDocType = AosDocType::eBinaryDoc;
			rollbackPriv(rdata, snapshot);
			snapshot.mVirtualId = i;
			snapshot.mDocType = AosDocType::eIIL;
			rollbackPriv(rdata, snapshot);
		}
	}

	for (size_t i=0; i<mSnapshots.size(); i++)	
	{
		return rollbackPriv(rdata, mSnapshots[i]);
	}

	rslt = AosModifyDocAttrStr(rdata, mDoc, AOSTAG_STATUS, "finish", false, true);
	aos_assert_r(rslt, false);
	return true;
}


bool 
AosDbTrans::rollbackPriv(AosRundata *rdata, SnapshotInfo &snapshot)
{
	if (snapshot.mDocType == AosDocType::eIIL)
	{
		AosIILClientObjPtr iilclient = AosIILClientObj::getIILClient();
		bool rslt = iilclient->rollBackSnapshot(snapshot.mVirtualId, mSnapshotId, 0, rdata);
		aos_assert_r(rslt, false);
	}
	else {
		AosDocClientObjPtr docclient = AosDocClientObj::getDocClient();
		bool rslt = docclient->rollbackSnapshot(rdata, snapshot.mVirtualId, 
				snapshot.mDocType, mSnapshotId, 0); 
		aos_assert_r(rslt, false);
	}
	return true;
}
