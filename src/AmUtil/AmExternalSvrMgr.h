////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//
// Modification History:
// 2007-05-30: Created by Frank
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_AmUtil_AmExternalSvrMgr_h
#define Aos_AmUtil_AmExternalSvrMgr_h

#include "AmUtil/AmExternalSvr.h"
#include "AmUtil/AmRequestTypeId.h"
#include "AmUtil/Ptrs.h"
#include "Util/ValList.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosAmExternalSvrMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	bool 				mIsGood;
	char 				mDBConn[256];
	/* external server queue, 2-dimentional array */
	OmnVList<AosAmExternalSvrPtr> 	mSvrQueue[AosAmRequestSvrTypeId::eMaxSvrTypeId];
	
	/* redundant 2-dimentional array , just for search servers quickly 
	 * we have syncMsgSvr() to syncronize these two Queues
	 */
	OmnVList<AosAmExternalSvrPtr> 	mMsgQueue[AosAmMsgId::eMaxMsgId]; // have redundancy
public:
	AosAmExternalSvrMgr(const char * db_conn_str);
	~AosAmExternalSvrMgr();

	bool appendSvr(const AosAmExternalSvrPtr &server);
	bool removeSvr(const char *svr_name);
	AosAmExternalSvrPtr getSvr(const char *svr_name);
	bool getMsgSvrArray(const int msgId, OmnVList<AosAmExternalSvrPtr> &serverArray);

	bool isGood(){ return mIsGood; };
	bool initialize();
private:
	bool syncMsgSvr();
};

#endif // Aos_AmUtil_AmExternalSvrMgr_h

