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
// 08/02/2011	Created by Jozhi
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_ReqProc_CheckLogin_h
#define Omn_ReqProc_CheckLogin_h	

#include "ReqProc/ReqProc.h"
#include "SeReqProc/SeRequestProc.h"
#include "Rundata/Rundata.h"

class AosCheckLogin: public AosSeRequestProc
{
	//Important: This class shoud not have any member data, it's not thead safe. 
	
public:
	AosCheckLogin(const bool);
	~AosCheckLogin() {}
	
	virtual bool proc(const AosRundataPtr &rdata);
	
private:
};
#endif

