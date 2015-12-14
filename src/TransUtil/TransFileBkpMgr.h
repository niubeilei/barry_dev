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
// Modification History:
// 10/11/2012	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_TransUtil_TransFileBkpMgr_h
#define AOS_TransUtil_TransFileBkpMgr_h

#include "aosUtil/Types.h"
#include "SEUtil/Ptrs.h"
#include "Thread/Ptrs.h"
#include "TransUtil/Ptrs.h"
#include "TransUtil/Trans1.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/String.h"



class AosTransFileBkpMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	OmnString       mDirname;
	OmnTcpClientPtr	mSendConn[eMaxServerId];

	OmnFilePtr		mCrtHeaderFile[eMaxServerId];
	OmnFilePtr		mCrtBodyFile[eMaxServerId];
	OmnFilePtr		mCrtCleanFile[eMaxServerId];

	u32				mCrtHeaderSeqno;
	u32				mCrtBodySeqno;
	u32				mCrtCleanSeqno;

public:
	AosTransFileBkpMgr(const OmnString &dirname);
	~AosTransFileBkpMgr();

	bool	msgRecved(const AosBuffPtr &buff);

private:

};
#endif
