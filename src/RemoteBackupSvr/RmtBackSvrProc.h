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
#ifndef AOS_RemoteBackupSvr_RmtBackSvrProc_h
#define AOS_RemoteBackupSvr_RmtBackSvrProc_h

#include "Util/RCObject.h"


class AosRmtBackSvrProc : virtual public OmnRCObject
{
public:
	virtual bool proc(const AosWebRequestPtr &req, const AosRundataPtr &rdata) = 0;
};
#endif

