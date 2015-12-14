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
// 12/18/2012: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_UploadServer_FileLoader_h
#define Aos_UploadServer_FileLoader_h

#include "Proggie/ReqDistr/NetReqProc.h"
#include "Proggie/ProggieUtil/Ptrs.h"
#include "Rundata/Ptrs.h"


class AosFileLoader : public AosNetReqProc
{
	OmnDefineRCObject;

private:

public:
	AosFileLoader();
	~AosFileLoader();
};
#endif
