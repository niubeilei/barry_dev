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
// 05/24/2011	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_ReqProc_RemoveMicroBlogMsg_h
#define Omn_ReqProc_RemoveMicroBlogMsg_h	

#include "ReqProc/ReqProc.h"
#include "SeReqProc/SeRequestProc.h"

class AosRemoveMicroBlogMsg: public AosSeRequestProc
{
	//Important: This class shoud not have any member data, it's not thead safe. 
	
public:
	AosRemoveMicroBlogMsg(const bool);
	~AosRemoveMicroBlogMsg() {}
	
	virtual bool proc(const AosRundataPtr &rdata);
	
private:
};
#endif

