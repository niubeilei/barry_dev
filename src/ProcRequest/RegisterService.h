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
// 07/28/2011	Created by Jackie zhao 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_ReqProc_RegisterService_h
#define Omn_ReqProc_RegisterService_h	

#include "ReqProc/ReqProc.h"
#include "SearchEngine/DocServer.h"
#include "Util/String.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "ProcRequest/ProcRequest.h"

#include "ProcServer/ProcServer.h"

class AosRegisterService: public AosProcRequest 
{
	//Important: This class shoud not have any member data, it's not thead safe. 
public:
	enum E
	{
		eMaxFields = 50		
	};

public:
	AosRegisterService(const bool);
	~AosRegisterService() {}
	
	virtual bool proc(const AosRundataPtr &rdata);
	void setProcServer(const AosProcServerPtr &procserver){mServer = procserver;}
private:
	AosProcServerPtr mServer;

};
#endif

