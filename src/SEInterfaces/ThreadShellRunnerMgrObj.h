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
// 07/14/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_ThreadShellRunnerMgrObj_h
#define Aos_SEInterfaces_ThreadShellRunnerMgrObj_h

#include "SEInterfaces/Ptrs.h"
#include "ThreadShellRunners/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;

class AosValueRslt;

class AosThreadShellRunnerMgrObj : virtual public OmnRCObject
{
private:
	static AosThreadShellRunnerMgrObjPtr	smThreadShellRunnerMgrObj;

public:

	static AosThreadShellRunnerMgrObjPtr getThreadShellRunnerMgrObj() 
	{return smThreadShellRunnerMgrObj;}
	static void setThreadShellRunnerMgrObj(const AosThreadShellRunnerMgrObjPtr &d) 
	{smThreadShellRunnerMgrObj = d;}
};
#endif
