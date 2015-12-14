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
// Modification History:
// 09/17/2011	Created by Ketty
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_StorageMgr_ConfigMgr_h
#define AOS_StorageMgr_ConfigMgr_h

#include "DocFileMgr/Ptrs.h"
#include "DocFileMgr/DfmProc.h"
#include "DfmUtil/DfmDocNorm.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/Buff.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/File.h"
#include "UtilHash/Ptrs.h"
#include "StorageMgr/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "DfmWrapper/DfmWrapper.h"



class AosConfigMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;
	enum
	{
		eHeaderSize = 12,
	};

private:
	AosDocFileMgrObjPtr		mDocFileMgr;
	AosVirtualFileSysPtr	mVfs;
	
	//static AosDfmDocNormPtr smDfmDoc;	// Ketty 2013/01/17

public:
	AosConfigMgr(const AosVirtualFileSysPtr &vfs);
	~AosConfigMgr();

	bool	init();
	bool 	stop();

	AosXmlTagPtr getDoc(const u64 &docid, AosRundata *rdata);
	bool		 saveDoc(
					const u64 &docid,
					const AosXmlTagPtr &doc, 
					AosRundata *rdata);
};
#endif
