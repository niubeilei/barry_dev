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
// 12/15/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEUtil_VersionDocMgr_h
#define AOS_SEUtil_VersionDocMgr_h

#include "SearchEngine/Ptrs.h"
#include "Thread/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "Util/Buff.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/DynArray.h"
#include "XmlUtil/Ptrs.h"



class AosVersionDocMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum
	{
		eMaxFiles = 500,
		eDocOffset = 1000,
		eDocSizeOffset = 4
	};

protected:
	OmnMutexPtr		mLock;
	AosLockMonitorPtr mLockMonitor; 
	u32				mCrtSeqno;
	u64				mMaxFilesize;
	OmnString		mDirname;
	OmnString		mFilename;

	OmnFilePtr		mFiles[eMaxFiles];
	u64 			mFilesizes[eMaxFiles];
	bool			mServerReadonly;

public:
	AosVersionDocMgr();
	AosVersionDocMgr(
		const OmnString &dirname, 
		const OmnString &fname);
	~AosVersionDocMgr();

	bool	stop();
	bool 	
	saveDoc(
		u32 &seqno,
		u64 &offset,
		const u64 docsize,
		const char *data);

	bool 	init(
				const OmnString &dirname, 
				const OmnString &fname
				);
	AosXmlTagPtr	getXmlDoc2(const u32 seqno, const u32 offset);
	AosXmlTagPtr	readDoc(const u32 seqno, const u32 offset);
	static int getReservedSize() {return eDocOffset;}
	//james 2010-12-8
	bool 	
	saveDocASC(
			u32 &seqno,
			u64 &offset,
			u64 &docsize,
			const OmnString &data);
	AosXmlTagPtr	readDocASC(const u32 seqno, const u32 offset);
protected:
	bool 	findFilePriv(const u64 &newsize, u32 seqno);
	OmnFilePtr openFilePriv(const u32 seqno AosMemoryCheckDecl);
};
#endif
