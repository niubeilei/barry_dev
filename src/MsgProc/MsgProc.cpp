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
// 02/25/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "MsgProc/MsgProc.h"

#include "alarm_c/alarm.h"
#include "MsgProc/MsgRecvProc.h"
#include "MsgProc/MsgProcUtil.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/SoapServerObj.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"


static AosMsgProcPtr	sgMsgProcs[AosMsgProcType::eMax];
static OmnMutex			sgLock;
static bool				sgInited = false;

AosMsgProc::AosMsgProc(
		const OmnString &name,
		const AosMsgProcType::E type, 
		const bool regflag)
:
mType(type)
{
	if (regflag)
	{
		AosMsgProcPtr thisptr(this, false);
		registerMsgProc(thisptr, name);
	}
}


bool
AosMsgProc::registerMsgProc(const AosMsgProcPtr &proc, const OmnString &name)
{
	OmnString errmsg;
    aos_assert_r(AosMsgProcType::isValid(proc->mType), false);

	sgLock.lock();
    if (sgMsgProcs[proc->mType])
	{
		OmnAlarm << "MsgProc already registered: " << proc->mType << enderr;
		sgLock.unlock();
		return false;
	}
    sgMsgProcs[proc->mType] = proc;
	sgLock.unlock();
	
	bool rslt = AosMsgProcType::addName(name, proc->mType, errmsg);
	if (!rslt)
	{
		OmnAlarm << "Failed adding message procname: " << name << enderr;
		return false;
	}
	return true;
}


bool
AosMsgProc::initAllProcs()
{
	static AosMsgRecvProc lsReceiverProc(true);

	if (sgInited) return true;
	sgLock.lock();
	for (int i=AosMsgProcType::eInvalid+1; i<AosMsgProcType::eMax; i++)
	{
		if (!sgMsgProcs[i])
		{
			OmnAlarm << "Missing MsgProc Registration: " << i << enderr;
		}
	}
	sgInited = true;
	sgLock.unlock();
	return true;
}


bool
AosMsgProc::config(const AosXmlTagPtr &conf)
{
	aos_assert_r(conf, false);
	AosXmlTagPtr conf_tag = conf->getFirstChild("soap_server");
	if (!conf_tag) return true;
	AosXmlTagPtr record = conf_tag->getFirstChild();
	do
	{
		if (!record) break;

		OmnString procname = record->getAttrStr("proc_name");
		if (procname == "")
		{
			OmnAlarm << "procname is empty: " << conf_tag->toString();
			continue;
		}

		OmnString appid = record->getAttrStr("appid");
		if (appid == "")
		{
			OmnAlarm << "appid is empty: " << conf_tag->toString();
			continue;
		}

		AosMsgProcPtr proc = getProc(procname);
		if (!proc)
		{
			OmnAlarm << "proc not found: " << procname << enderr;
			continue;
		}

		OmnString servertype = record->getAttrStr("server_type");
		if (servertype == "")
		{
			OmnAlarm << "missing server type: " << conf_tag->toString() << enderr;
			continue;
		}

		if (servertype == AOSMSGSERVERTYPE_SOAP)
		{
			if (!AosRegisterSoapMsgProc(appid, proc))
			{
				OmnAlarm << "Failed register msg proc: " << appid << enderr;
			}
		}
		else
		{
			OmnAlarm << "Unrecognized msg server type: " << servertype << enderr;
			continue;
		}

	} while ((record = conf_tag->getNextChild()));

	return true;
}


AosMsgProcPtr
AosMsgProc::getProc(const OmnString &name)
{
	AosMsgProcType::E type = AosMsgProcType::toEnum(name);
	if (!AosMsgProcType::isValid(type))
	{
		OmnAlarm << "Msg proc type invalid: " << name << enderr;
		return 0;
	}

	return sgMsgProcs[type];
}

