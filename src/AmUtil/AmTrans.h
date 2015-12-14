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
// 4/5/2007: Created by cding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_AmUtil_AmTrans_h
#define Aos_AmUtil_AmTrans_h

#include "AmUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "UtilComm/Ptrs.h"
#include "AmUtil/AmMsg.h"

#include "Debug/Rslt.h"

class AosAmTrans : virtual public OmnRCObject
{
	OmnDefineRCObject;

private: 

	OmnTcpClientPtr		mConn;
//	OmnConnBuffPtr		mBuff;
	OmnVList<AosAmMsgPtr> mMsgQueue;
	AosAmExternalSvrPtr mExternalSvr;

public:
	AosAmTrans(const OmnTcpClientPtr conn, const OmnConnBuffPtr buff);
	AosAmTrans(const AosAmTransPtr &trans);
	~AosAmTrans();

	OmnConnBuffPtr 	getTransBuff() const  { return mMsgQueue.crtValue()->getBuffer(); }
	OmnTcpClientPtr getConn() const;
	AosAmMsgPtr		getMsg() 
	{
		if(0 == mMsgQueue.crtValue()) 
			mMsgQueue.reset(); 
		return mMsgQueue.crtValue(); 
	}
	bool 			setMsg(const AosAmMsgPtr &msg);

	bool 			hasResponse();
	bool 			setErrMsg(const OmnString &value);
	OmnRslt 		sendOut();

	AosAmExternalSvrPtr getExternalSvr();
	bool 				setExternalSvr(const AosAmExternalSvrPtr & server);
	
	bool isRespPositive(){ return mMsgQueue.crtValue()->isRespPositive(); };
//	static createTrans();
	// Map the Access Type To the function
//	(int *)(const AosAmTransPtr &trans) AosAmMapTransTypeFuncPtr(AosAmTagId::E);
private:
};
#endif

