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
//	This class is used to manage a group of files, used to store 
//	either IILs or Documents. There are a number of files in the
//  group, each with a unique sequence number. Each file can 
//  store up to a given amount of data. 
//	
//	All documents are stored in 'mVersionDocname' + seqno. There is 
//	a Document Index, that is stored in files 'mVersionDocname' + 
//		'Idx_' + seqno
//
// Modification History:
// 03/03/2011	Created by Ketty
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEModules_MonitorLogMgr_h
#define AOS_SEModules_MonitorLogMgr_h

#include "Rundata/Ptrs.h"
#include "SearchEngine/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Util/Buff.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/DynArray.h"
#include "SEUtil/VersionDocMgr.h"

class AosMonitorLogMgr : public OmnRCObject
{
	OmnDefineRCObject;

private:
	u64	mPreDocLoc;

public:
	enum
	{
		eDocSizeOffset = 0,
		ePreDocLocOffset = 4,
		eBIdOffset = 12,
		eDocBodyOffset = 16,

		eDocHeaderSize = 16,
	};

	
	AosMonitorLogMgr();
	AosMonitorLogMgr(
		const OmnString &dirname, 
		const OmnString &fname);
	~AosMonitorLogMgr();

	void start();

	bool saveDoc(	u64 &offset,
					const OmnString &data,
					const AosRundataPtr &rdata);

	AosXmlTagPtr readDoc(
					const u32 seqno,
					const u64 offset,
					u64 &preDocLoc,
					u32 &browserId, 
					const AosRundataPtr &rdata);
};
#endif
