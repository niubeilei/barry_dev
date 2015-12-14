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
// 10/16/2010: Created by Ketty
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEServerCgi_SEServer_FcgiSeReqProc_h
#define Aos_SEServerCgi_SEServer_FcgiSeReqProc_h

#include "SEServer/Ptrs.h"
#include "SEServerCGI/FcgiReqProc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Proggie/ReqDistr/NetReqProc.h"

class AosFcgiSeReqProc : public AosFcgiReqProc 
{
public:
	AosFcgiSeReqProc(const AosNetReqProcPtr &req);
	~AosFcgiSeReqProc();

    virtual bool procRequest(FCGX_Request &request);

private:
	bool	getSeRequest(FCGX_Request &request, char* data, OmnString &trans_id, OmnString &seRequest);
};
#endif

