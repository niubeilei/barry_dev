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
// Modification History:
// 2014/01/29 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_ParallelIIL_h
#define Aos_ParallelIIL_ParallelIIL_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;

class AosParalIILObj : virtual public OmnRCObject
{
public:
	virtual bool getIILNames(
				const AosRundataPtr &rdata, 
				const AosQueryReqObjPtr &query_req, 
				vector<OmnString> &iilnames) = 0;

	virtual bool getIILName(
						const AosRundataPtr &rdata, 
						const OmnString &base_name,
						const int epoch_day, 
						OmnString &iilname, 
						bool &period_created) = 0;
};
#endif

