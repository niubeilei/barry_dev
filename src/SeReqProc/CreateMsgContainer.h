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
#ifndef Omn_ReqProc_CreateMsgContainer_h
#define Omn_ReqProc_CreateMsgContainer_h	

#include "ReqProc/ReqProc.h"
#include "Util/String.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "SeReqProc/SeRequestProc.h"

class AosCreateMsgContainer: public AosSeRequestProc
{
	//Important: This class shoud not have any member data, it's not thead safe. 
public:
	enum E
	{
		eMaxFields = 50		
	};

public:
	AosCreateMsgContainer(const bool);
	~AosCreateMsgContainer() {}
	
	virtual bool proc(const AosRundataPtr &rdata);
private:
	AosXmlTagPtr createSubContainer(
		const OmnString &sub_objid,
		const AosRundataPtr &rdata);
	AosXmlTagPtr createMsgContainer(
		const OmnString &pid, 
		const OmnString &ctnr_objid,
		const OmnString &cid, 
		const AosRundataPtr &rdata);
	bool checkHasCloudId(
		const OmnString &ctnr_objid,
		const AosRundataPtr &rdata);
};
#endif

