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
//   
//
// Modification History:
// 09/07/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SengTorturer_TestFileMgr_h
#define Aos_SengTorturer_TestFileMgr_h

#include "DocFileMgr/Ptrs.h"
#include "DfmUtil/DfmConfig.h"
#include "DocFileMgr/DfmUtil.h"
#include "SengTorturer/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

#define eAosInvFseqno 0xffffffff
#define eAosInvFoffset 0xffffffff
#define eAosInvDocSize 0
#define eAosInvDiskCap 0

class AosTestFileMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	enum
	{
		eDfmType = AosDfmType::eNormDfm,
		eMaxThread 		= 1000

	};
	
	AosDocFileMgrObjPtr mFileMgr[eMaxThread];
	AosDfmConfig 	mDfmConf;
	static const OmnString scDir;
	static const OmnString scFilename;

public:
	AosTestFileMgr();
	~AosTestFileMgr();

	bool	createFileMgr(
				const AosRundataPtr &rdata,
				const int tid);

	bool 	createDoc(
				const AosRundataPtr &rdata,
				const int tid,
				const u64 &docid, 
				const char *data,
				const int data_len);

	AosXmlTagPtr 	readDoc(
				const AosRundataPtr &rdata,
				const int tid,
				const u64 &docid AosMemoryCheckDecl);

	bool 	modifyDoc(
				const AosRundataPtr &rdata,
				const int tid,
				const u64 &docid,
				const char *data,
				const int data_len);

	bool 	removeDoc(
				const AosRundataPtr &rdata,
				const int tid, 
				const u64 &docid);

	bool	isDocDeleted(
				const AosRundataPtr &rdata,
				const int tid, 
				const u64 &docid);
};
#endif

