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
// 2011/01/19	Created by Ice
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DocTrans_DocTransProc_h
#define AOS_DocTrans_DocTransProc_h 

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "aosUtil/Types.h"


class AosDocTransProc : virtual public OmnRCObject 
{
public:
	
	virtual bool	finishTrans(
						vector<u64> &trans,
						const AosRundataPtr &rdata) = 0;
	virtual bool	finishTrans(
						const u64 &global_tid,
						const OmnString &resp_msg, 
						const AosRundataPtr &rdata) = 0;
	virtual bool	recoverHeaderBody(
						const u64 &entryid,
						const AosBuffPtr &headerBuff,
						const AosBuffPtr &bodyBuff) = 0;
};
#endif
