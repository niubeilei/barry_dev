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
// 2011/01/22	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_TransClient_TransCltProc_h
#define Omn_TransClient_TransCltProc_h

#include "Rundata/Rundata.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"



class AosTransCltProc : virtual public OmnRCObject
{
public:
	virtual void proc(const u32 module_id, const AosXmlTagPtr &msg) = 0;
};
#endif

