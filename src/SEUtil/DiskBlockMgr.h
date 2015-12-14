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
// 07/19/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEUtil_DiskBlockMgr_h
#define AOS_SEUtil_DiskBlockMgr_h

#include "Thread/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/DocTags.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "XmlInterface/Server/Ptrs.h"



class AosDiskBlockMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	enum
	{
		eMaxBlocks = 30,
		eAosMaxFileSeqno = 500
	};

	OmnMutexPtr		mLock;
	OmnCondVarPtr	mCondVar;
	AosDiskBlockPtr	mBlocks[eMaxBlocks];
	int				mBlocksizes[eAosMaxFileSeqno];

	// tmp
	OmnString		mFnames[eMaxBlocks];
	int				mLines[eMaxBlocks];

public:
	AosDiskBlockMgr();
	~AosDiskBlockMgr();

	AosXmlTagPtr	getIdDef(const OmnString &name);


	AosDiskBlockPtr
	getBlock2(
		const OmnFilePtr &file, 
		const u32 seqno, 
		const u32 offset, 
		const u32 blocksize,
		const bool createnew);//enl

	AosDiskBlockPtr
	getBlock(
		const OmnFilePtr &file, 
		const u32 seqno, 
		const u32 offset, 
		const u32 blocksize,
		const bool createnew,
		const OmnString &fname, 
		const int lineno);
	bool returnBlock(const AosDiskBlockPtr &block);
	bool sanityCheck();
	bool	saveAllBlocks();
	/*bool readDoc(
	 		const OmnFilePtr &file,
			const u32 seqno,
	 		const u32 offset, 
	 		char *data, 
	 		const u32 docsize);*/

private:
};
#endif
