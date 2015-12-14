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
// 2015	Created by Jozhi
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_ReqProc_JqlLogin_h
#define Omn_ReqProc_JqlLogin_h	

#include "Util/String.h"
#include "Rundata/Rundata.h"
#include "SeReqProc/SeRequestProc.h"

class AosJqlLogin: public AosSeRequestProc
{
public:
public:
	AosJqlLogin(const bool);
	~AosJqlLogin() {}
	
	virtual bool proc(const AosRundataPtr &rdata);
private:
};
#endif

