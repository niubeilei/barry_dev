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
// 06/03/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Aos_Rundata_RundataCallback_h
#define Aos_Rundata_RundataCallback_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosRundataCallback : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	virtual bool setErrorEntry(
						const OmnString &fname, 
						const int line, 
						const bool is_user_error,
						const OmnString &errmsg) = 0;
};
#endif
#endif
