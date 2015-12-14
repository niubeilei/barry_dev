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
// 06/15/2011	Created by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_MsgServer_Connect_h
#define Omn_MsgServer_Connect_h	

#include "MsgServer/MsgRequestProc.h"
#include "Rundata/Rundata.h"

class AosConnect: public AosMsgRequestProc
{
public:
	AosConnect(const bool);
	~AosConnect() {}
	
	virtual bool proc(
			const AosXmlTagPtr &request,
			const AosRundataPtr &rdata);
	
private:
};
#endif
