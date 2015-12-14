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
// 10/06/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEServer_AllTrans_h
#define Aos_SEServer_AllTrans_h

#include "DocTrans/AllDocTrans.h"
#include "IILTrans/AllIILTrans.h"
#include "LogTrans/AllLogTrans.h"
#include "JobTrans/AllJobTrans.h"
#include "SysTrans/AllSysTrans.h"
#include "SysMsg/AllSysMsg.h"
#include "CounterUtil/AllCounterTrans.h"
#include "AdminTrans/AllAdminTrans.h"

class AosAllTrans
{
public:
	static void init()
	{
		AosAllDocTrans::init();
		AosAllIILTrans::init();
		AosAllLogTrans::init();
		AosAllJobTrans::init();
		AosAllSysTrans::init();
		AosAllSysMsg::init();
		AosAllCounterTrans::init();
		AosAllAdminTrans::init();
	};

};

#endif
