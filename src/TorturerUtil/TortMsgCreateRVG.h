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
#ifndef Aos_TorturerUtil_TortMsgCreateRVG_h
#define Aos_TorturerUtil_TortMsgCreateRVG_h
 
#include "TorturerUtil/TortMsg.h"
#include <string>

class OmnSerialFrom;

class AosTortMsgCreateRVG : public AosTortMsg
{
private:
	std::string		mName;
	int				mType;

public:
	AosTortMsgCreateRVG(OmnSerialFrom &buff, 
						const OmnTcpClientPtr &conn); 
	~AosTortMsgCreateRVG();

	virtual bool		proc();
	virtual std::string	toString() const;

private:
};

#endif

