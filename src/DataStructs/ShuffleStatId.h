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
// 09/26/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DataStructs_ShuffleStatId_h
#define AOS_DataStructs_ShuffleStatId_h

#include "DataStructs/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Buff.h"
#include "Util/Ptrs.h"

class AosShuffleStatId : virtual public OmnRCObject 
{
	OmnDefineRCObject;

private:
	AosStatIdMgrPtr		mStatIdMgr;
	AosBuffPtr			mCtrlBuff;
	AosXmlTagPtr		mCtrlDoc;
	int 				mDocsPerDistBlock;
public:
	AosShuffleStatId(
			const AosXmlTagPtr &control_doc,
			const AosBuffPtr &buff);
	~AosShuffleStatId();

	int getPhysicalIdByStatId(
			const AosRundataPtr &rdata,
			const u64 &stat_id);

	u32 getVirtualIdByStatId(
			const AosRundataPtr &rdata,
			const u64 &stat_id);

	u64 getBinaryDocid(const u64 &stat_id);

	//bool saveControlDoc(
	//		const AosRundataPtr &rdata,
	//		map<int, u64> &bsnap_ids);
private:
	u64  getDocid(
			const AosRundataPtr &rdata,
			const u64 &stat_id,
			int &vid);

	int  getVirtualIdByGroupId(const u64 &stat_id);
};
#endif
