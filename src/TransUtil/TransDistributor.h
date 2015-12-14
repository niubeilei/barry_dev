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
// 05/31/2011 	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_TransUtil_TransDistributor_h
#define AOS_TransUtil_TransDistributor_h

#include "SEUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "aosUtil/Types.h"
#include "TransUtil/Ptrs.h"
#include "XmlUtil/Ptrs.h"

class AosTransDistributor : virtual public OmnRCObject
{

public:
	virtual int   routeReq(const u64 &dist_id) = 0;
};
#endif

