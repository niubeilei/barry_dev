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
// 	Created: 05/09/2010 by Brian 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_ShortMsgSvr_ProcSendShm_h
#define AOS_ShortMsgSvr_ProcSendShm_h

#include "SEUtil/Ptrs.h"
#include "ShortMsgSvr/ShmReqProc.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "UtilComm/Ptrs.h"
#include "XmlInterface/XmlRc.h"
#include "ShortMsgSvr/Ptrs.h"


class AosProcSendShm : virtual public AosShmReqProc 
{
private:
		OmnString				mOperation;

public:
	AosProcSendShm(const bool regflag);
	~AosProcSendShm();

	virtual bool 	proc(const AosRundataPtr &rdata);
};
#endif

