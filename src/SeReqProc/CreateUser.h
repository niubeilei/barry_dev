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
#ifndef Omn_ReqProc_CreateUser_h
#define Omn_ReqProc_CreateUser_h	

#include "ReqProc/ReqProc.h"
#include "Util/String.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "SeReqProc/SeRequestProc.h"

class AosCreateUser: public AosSeRequestProc
{
	//Important: This class shoud not have any member data, it's not thead safe. 
public:
	enum E
	{
		eMaxFields = 50		
	};

public:
	AosCreateUser(const bool);
	~AosCreateUser() {}
	
	virtual bool proc(const AosRundataPtr &rdata);

	bool createUserAccounts(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &root,
		const AosXmlTagPtr &userobj);
	bool 
	createOneUserAcct(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &root,
		const AosXmlTagPtr &userobj);
	
private:
};
#endif

