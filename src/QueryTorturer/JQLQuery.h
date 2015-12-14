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
// 2013/09/08	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_QueryTorturer_JQLQuery_h
#define Aos_QueryTorturer_JQLQuery_h

#include "QueryTorturer/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Rundata/Rundata.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
using namespace std;

class AosJQLQuery
{
	OmnDefineRCObject;

private:

public:
	AosJQLQuery();
	~AosJQLQuery();

	static bool runQuery(AosRundataPtr rdata, 
						 OmnString stmt); 

private:

};
#endif

