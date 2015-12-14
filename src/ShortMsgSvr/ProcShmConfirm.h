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
// 	Created: 06/23/2011 by Brian Zhang 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_ShortMsgSvr_ProcShmConfirm_h
#define AOS_ShortMsgSvr_ProcShmConfirm_h

#include "SEUtil/Ptrs.h"
#include "ShortMsgSvr/Ptrs.h"
#include "ShortMsgSvr/ShmReqProc.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "UtilComm/Ptrs.h"
#include "XmlInterface/XmlRc.h"



class AosProcShmConfirm : virtual public AosShmReqProc 
{
private:
	OmnString 				mSdocObjid;
	OmnString				mOperation;

public:
	AosProcShmConfirm(const bool regflag);
	~AosProcShmConfirm();
	virtual bool 	proc(const AosRundataPtr &rdata);
};
#endif

