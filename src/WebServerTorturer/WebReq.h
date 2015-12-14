////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 	Created: 10/18/2010 by Jozhi
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_WebServerTorturer_WebReq_h
#define AOS_WebServerTorturer_WebReq_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "WebServerTorturer/Ptrs.h"


class AosWebReq : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	OmnString 		mUri; 
	OmnString 		mMethod; 
	OmnString 		mParms; 
	OmnString 		mCookies; 
	OmnTcpClientPtr	mConn; 
	AosWebCltReqPtr	mRequester;
};
#endif

