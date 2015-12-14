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
// handle the SEServer send request to MsgServer 
//
// Modification History:
// 02/12/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_RemoteBackupClt_RemoteBkReq_h
#define AOS_RemoteBackupClt_RemoteBkReq_h

#include "RemoteBackupUtil/RemoteBkType.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Buff.h"
#include "XmlUtil/Ptrs.h"


class AosRemoteBkReq : public OmnRCObject
{
	OmnDefineRCObject;
	
private:
	//AosRemoteBkType::E	mType;
	OmnString 		 	mType;	
	u64					mTransId;

	// doc
	AosXmlTagPtr		mDoc;
	// iil
	u64					mIILId;
	AosBuffPtr			mBuff;
	u32					mSiteid;
	//word 
	OmnString 			mWord;


public:
	AosRemoteBkReq(
			//const AosRemoteBkType::E type, 
			const OmnString &type, 
			const u64 transid,
			const AosXmlTagPtr &doc)
	:
	mType(type),
	mTransId(transid),
	mDoc(doc)
	{
	}

	AosRemoteBkReq(
			const OmnString &type,
			const u64 transid,
			const u64 iilid,
			const AosBuffPtr &buff,
			const u32 siteid)
	:
	mType(type),
	mTransId(transid),
	mIILId(iilid),
	mBuff(buff),
	mSiteid(siteid)
	{
	}

	AosRemoteBkReq(
			const OmnString &type, 
			const u64 transid,
			const u64 iilid,
			const OmnString &word,
			const u32 siteid)
	:
	mType(type),
	mTransId(transid),
	mIILId(iilid),
	mSiteid(siteid),
	mWord(word)
	{
	}

	~AosRemoteBkReq() {}

	bool toString(OmnString &str);
};
#endif

