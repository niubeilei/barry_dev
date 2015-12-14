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
// 01/23/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_TorturerUtil_TortMsg_h
#define Aos_TorturerUtil_TortMsg_h
 
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "UtilComm/Ptrs.h"
#include <string>

class AosTortMsg : virtual public OmnRCObject
{
	OmnDefineRCObject;

protected:
	bool			mIsGood;
	OmnTcpClientPtr	mConn;

public:
	AosTortMsg(const OmnTcpClientPtr &conn);
	~AosTortMsg();

	virtual bool			proc() = 0;
	virtual std::string 	toString() const = 0;
	bool	isGood() const {return mIsGood;}
};

#endif

