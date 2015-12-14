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
// 06/23/2011 Created by Brian Zhang 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_ShortMsgSvr_HandlerShm2Shm_h
#define AOS_ShortMsgSvr_HandlerShm2Shm_h

#include "SEUtil/Ptrs.h"
#include "ShortMsgSvr/Ptrs.h"
#include "ShortMsgSvr/ShmReqProc.h"
#include "ShortMsgSvr/ShmHandler.h"
#include "ShortMsgUtil/ShmHandlerId.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "UtilComm/Ptrs.h"
#include "XmlInterface/XmlRc.h"



class AosHandlerShm2Shm : virtual public OmnRCObject, virtual public AosShmHandler
{

private:
	bool 						mExclusive;
	OmnString					mModemCellnum;
	AosShmHandlerId::E 			mOperation;


public:
	AosHandlerShm2Shm(
			const AosShmHandlerId::E id,
			const bool exclusive,
			const OmnString &modem_cellnum);
	~AosHandlerShm2Shm();
	virtual bool 	procMsg(
						const OmnString &sender,
						const OmnString &modem_cellnum, 
						const OmnString &msg, 
						bool &stop_flag,
						bool &remove_flag,
						const AosRundataPtr &rdata);
};
#endif

