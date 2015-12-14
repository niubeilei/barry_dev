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
// 2011/01/22	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_ReqProc_RegisterHook_h
#define Omn_ReqProc_RegisterHook_h

#include "SeReqProc/SeRequestProc.h"
#include "Util/String.h"

class AosRegisterHook : public AosSeRequestProc
{
	OmnDefineRCObject;

public:
	enum
	{
		eMaxRegisterPerRequest = 30
	};

public:
	AosRegisterHook(const bool regflag);
	~AosRegisterHook() {}

	virtual bool proc(const AosRundataPtr &rdata);
};
#endif

