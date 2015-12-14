////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 2013/05/06 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SysConsole_Cluster_h
#define AOS_SysConsole_Cluster_h

#include "Rundata/Ptrs.h"
#include "TransServer/TransProc.h"
#include "Util/RCObject.h"
#include "Util/RCOBjImp.h"
#include <queue>
using namespace std;


class AosCluster : public OmnThreadedObj
{
	OmnDefineRCObject;

private:

public:
	AosCluster();
	~AosCluster();

private:
};
#endif

