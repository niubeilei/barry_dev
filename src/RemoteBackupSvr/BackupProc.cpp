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
// 02/15/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "RemoteBackupSvr/BackupProc.h"

#include "alarm_c/alarm.h"
#include "RemoteBackupSvr/Ptrs.h"
#include "RemoteBackupSvr/DocBackupProc.h"
#include "RemoteBackupSvr/IILBackupProc.h"
#include "RemoteBackupUtil/RemoteBkType.h"
#include "TransClient/TransClient.h"
#include "Rundata/Rundata.h"
#include "SearchEngine/DocReq.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"

extern AosBackupProcPtr	sgProcs[AosRemoteBkType::eMax];
static OmnMutex			sgLock;
static bool				sgInited = false;

AosDocBackupProcPtr  AosBackupProc::smDocBackupProc = 0;
AosIILBackupProcPtr	 AosBackupProc::smIILBackupProc = 0;

AosBackupProc::AosBackupProc(
		const OmnString &name,
		const AosRemoteBkType::E type, 
		const bool regflag)
:
mType(type)
{
	if (regflag)
	{
		AosBackupProcPtr thisptr(this, false);
		registerProc(thisptr, name);
	}
}


AosBackupProcPtr 
AosBackupProc::getProc(const AosRemoteBkType::E type)
{
	aos_assert_r(AosRemoteBkType::isValid(type), 0);
	return sgProcs[type];
}


bool
AosBackupProc::registerProc(const AosBackupProcPtr &access, const OmnString &name)
{
	OmnScreen << "Register BackupProc: " << name << ":" << access->mType << endl;
    aos_assert_r(AosRemoteBkType::isValid(access->mType), false);

	sgLock.lock();
    if (sgProcs[access->mType])
	{
		OmnAlarm << "BackupProc already registered: " << access->mType << enderr;
		sgLock.unlock();
		return false;
	}
    sgProcs[access->mType] = access;
	sgLock.unlock();
	
	bool rslt = AosRemoteBkType::addName(name, access->mType);
	if (!rslt)
	{
		OmnAlarm << "Failed adding backup proc name: " << name << enderr;
		return false;
	}
	return true;
}


bool
AosBackupProc::checkRegistrations()
{
	if (sgInited) return true;
	sgLock.lock();
	for (int i=AosRemoteBkType::eInvalid+1; i<AosRemoteBkType::eMax; i++)
	{
		if (!sgProcs[i])
		{
			OmnAlarm << "Missing Security BackupProc Registration: " << i << enderr;
		}
	}
	smDocBackupProc = OmnNew AosDocBackupProc();
	smDocBackupProc->config(OmnApp::getAppConfig());

	smIILBackupProc = OmnNew AosIILBackupProc();
	smIILBackupProc->config(OmnApp::getAppConfig());
	AosTransClient::sRecover();
	sgInited = true;
	sgLock.unlock();
	return true;
}

/*
bool
AosBackupProc::startDocBackProc()
{
	if (sgIILInited) return true;
	sgLock.lock();
	for (int i=AosRemoteBkType::eInvalid+1; i<AosRemoteBkType::eMax; i++)
	{
		if (!sgProcs[i])
		{
			OmnAlarm << "Missing Security BackupProc Registration: " << i << enderr;
		}
	}

	smDocBackupProc = OmnNew AosDocBackupProc();
	smDocBackupProc->config(OmnApp::getAppConfig());
	if (!sgInited) AosTransClient::sRecover();
	sgIILInited = true;
	sgLock.unlock();
	return true;
}

bool
AosBackupProc::startIILBackProc()
{
	if (sgInited) return true;
	sgLock.lock();
	for (int i=AosRemoteBkType::eInvalid+1; i<AosRemoteBkType::eMax; i++)
	{
		if (!sgProcs[i])
		{
			OmnAlarm << "Missing Security BackupProc Registration: " << i << enderr;
		}
	}

	smIILBackupProc = OmnNew AosIILBackupProc();
	smIILBackupProc->config(OmnApp::getAppConfig());
	if (!sgIILInited) AosTransClient::sRecover();
	sgInited = true;
	sgLock.unlock();
	return true;
}
*/

bool 
AosBackupProc::addDocReq(
		const OmnString &req, 
		const u64 &docid, 
		const AosRundataPtr &rdata)
{
	//aos_assert_rr(checkRegistrations(), rdata, false);
	//aos_assert_rr(startDocBackProc(), rdata, false);
	aos_assert_rr(smDocBackupProc, rdata, false);
	return smDocBackupProc->addReq(rdata, req, docid); 
}

bool
AosBackupProc::addIILReq(
		const OmnString &req,
		const u64 &iilid,
		const AosRundataPtr &rdata)
{
	//aos_assert_rr(checkRegistrations(), rdata, false);
	//aos_assert_rr(startIILBackProc(), rdata, false);
	aos_assert_rr(smIILBackupProc, rdata, false);
	return smIILBackupProc->addReq(rdata, req, iilid);
}
