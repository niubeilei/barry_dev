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
// 09/15/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Actions/SdocAction.h"

#include "Actions/AllActions.h"
#include "alarm_c/alarm.h"
#include "Rundata/Rundata.h"
#include "SmartDoc/SmartDoc.h"
#include "Thread/Mutex.h"
#include "Thread/ThreadPool.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"


static OmnThreadPoolPtr sgThreadPool = OmnNew OmnThreadPool("action", __FILE__, __LINE__);
extern AosActionObjPtr	sgActions[AosActionType::eMax+1];
static OmnMutex 		sgLock;

#include "Actions/AllActions.h"

AosSdocAction::AosSdocAction(
		const OmnString &name, 
		const u32 version)
:
AosActionObj(version)
{
	mIsTemplate = false;
}


AosSdocAction::AosSdocAction(
		const OmnString &name, 
		const AosActionType::E type, 
		const bool reg)
:
AosActionObj(0)
{
	mIsTemplate = false;
	mType = type;
	AosActionObjPtr thisptr(this, false);
	if (reg) 
	{
		mIsTemplate = true;
		registerAction(thisptr, name);
	}
}


AosSdocAction::AosSdocAction(const AosActionType::E type)
:
AosActionObj(0)
{
	mType = type;
}


AosSdocAction::~AosSdocAction()
{
}


bool
AosSdocAction::run(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}

bool
AosSdocAction::run(const AosBuffPtr &buff, const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}

bool
AosSdocAction::run(
		const AosTaskObjPtr &task, 
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}


bool
AosSdocAction::run(const char *value, const int len, const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}

bool
AosSdocAction::checkConfig(
		const AosXmlTagPtr &def,
		const AosTaskObjPtr &task,
		const AosRundataPtr &rdata)
{
	return true;
}


bool
AosSdocAction::addThreadShellProc(
		const OmnThrdShellProcPtr &runner,
		const AosRundataPtr &rdata)
{
	aos_assert_r(runner, false);
	aos_assert_r(sgThreadPool, false);
	return sgThreadPool->proc(runner);
}

bool
AosSdocAction::initAction(
		const AosTaskObjPtr &task, 
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	return true;

	//OmnNotImplementedYet;
	//return false;
}

bool
AosSdocAction::run(const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosSdocAction::finishedAction(const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}

bool
AosSdocAction::checkConfig(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &conf,
		set<OmnString> &data_col_ids)
{
	// Ketty 2013/12/27
	return true;
}

bool
AosSdocAction::config(const u64 task_docid,
		const AosXmlTagPtr &conf,
		const AosRundataPtr &rdata)
{
	// Ketty 2013/12/27
	return true;
}


AosJimoPtr
AosSdocAction::cloneJimo() const
{
	OmnShouldNeverComeHere;
	return 0;
}

