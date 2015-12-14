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
// 03/09/2012 Created by Chen Ding
// 2013/10/25 Commented out by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "NetworkMgr/ServerComm.h"

#include "CommandProc/CommandProc.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"



AosServerComm::AosServerComm(const AosXmlTagPtr &def)
:
mLock(OmnNew OmnMutex()),
mLocalName(""),
mLocalAddr(""),
mLocalPort(-1),
mNumPorts(0),
mUdpComm(0),
mRundata(OmnNew AosRundata(AosMemoryCheckerArgsBegin))
{
	if (!def)
	{
		OmnAlarm << "Configuration is null" << enderr;
	}
	aos_assert(config(def));

	aos_assert(mRundata);
}


AosServerComm::~AosServerComm()
{
}

bool 
AosServerComm::config(const AosXmlTagPtr &config)
{
	aos_assert_r(config, false);
	mLocalName = config->getAttrStr("name");
	mLocalAddr = config->getAttrStr("addr");
	mLocalPort = config->getAttrInt("port", -1);
	mNumPorts = config->getAttrInt("num_port", 0);

	aos_assert_r(OmnIpAddr::isValidAddr(mLocalAddr), false);
	aos_assert_r(mLocalPort > 0, false);
	aos_assert_r(mNumPorts != 0, false);

	try
	{
		mUdpComm = OmnNew AosUdpXmlComm(mLocalAddr, mLocalPort, mLocalName);
		AosServerCommPtr thisptr(this, false);
		if (!mUdpComm->startReading(thisptr))
		{
			OmnAlarm << "Failed start reading the UDP Comm" << enderr;
		}
	}
	catch (...)
	{
		OmnAlarm << "Faild creating the UDP Comm. Configuration error" << enderr;
	}
	return true;
}


OmnString   
AosServerComm::getListenerName() const
{
	return mLocalName;
}


void        
AosServerComm::readingFailed()
{
}


bool        
AosServerComm::msgRead(
		const AosUdpXmlCommPtr &comm,
		const AosXmlTagPtr &request, 
		const OmnIpAddr &remote_addr, 
		const int remote_port)
{
	// A message was read from a connection 'comm'. It calls
	// AosNetworkMgr::msgRead(...)

	aos_assert_rr(request, mRundata, false);
	AosNetworkMgr::getSelf()->msgRead(comm, request, remote_addr, remote_port);
	return true;
	// OmnString type = request->getAttrStr("zky_cmd_type", "");
	// if (type == "")
	// {
	// 	mRundata->setError() << "Missing the command type attribute!";
	// 	OmnAlarm << mRundata->getErrmsg() << enderr;
	// 	return true;
	// }
	// AosCommandProcPtr proc = AosCommandProc::getProc(type);
	// aos_assert_rr(proc, mRundata, false);
	// bool rslt = proc->procCommand(request, mRundata);
	// aos_assert_rr(rslt, mRundata, false);
	// return true;
}


bool 
AosServerComm::sendMsg(
		const int virtual_id,
		const AosModuleId::E moduleId,
		const AosNetCommand::E command,
		const AosBuffPtr &buff, 
		const OmnIpAddr &remote_addr,
		const int remote_port,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(mUdpComm, rdata, false);
	aos_assert_rr(buff, rdata, false);
	aos_assert_rr(remote_addr.isValid(), rdata, false);
	aos_assert_rr(remote_port > 0, rdata, false);

	OmnString docstr = "<request ";
	docstr << AOSTAG_VIRTUAL_ID << "=\"" << virtual_id << "\" "
		<< AOSTAG_MODULE_ID << "=\"" << moduleId << "\" "
		<< AOSTAG_COMMAND_ID << "=\"" << command << "\">"
		<< "<![BDATA[" << buff->dataLen() << ":";
	docstr.append((char *)buff->data(), buff->dataLen());
	docstr << "]]></request>";

	AosXmlParser parser;
	AosXmlTagPtr request = parser.parse(docstr AosMemoryCheckerArgs);
	aos_assert_rr(request, rdata, false);

	bool rslt = mUdpComm->sendTo(request, remote_addr, remote_port);
	if (rslt)
	{
		OmnAlarm << "Send Faild" << enderr;
		return false;
	}
	return true;
}


bool 
AosServerComm::sendMsg(
		const int virtual_id,
		const AosModuleId::E moduleId,
		const AosNetCommand::E command,
		const AosXmlTagPtr &msg, 
		const OmnIpAddr &remote_addr,
		const int remote_port,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(mUdpComm, rdata, false);
	aos_assert_rr(msg, rdata, false);
	aos_assert_rr(remote_addr.isValid(), rdata, false);
	aos_assert_rr(remote_port > 0, rdata, false);

	msg->setAttr(AOSTAG_VIRTUAL_ID, virtual_id);
	msg->setAttr(AOSTAG_MODULE_ID, moduleId);
	msg->setAttr(AOSTAG_COMMAND_ID, command);

	bool rslt = mUdpComm->sendTo(msg, remote_addr, remote_port);
	if (rslt)
	{
		OmnAlarm << "Send Faild" << enderr;
		return false;
	}
	return true;
}


bool 
AosServerComm::sendMsg(
		const AosXmlTagPtr &msg, 
		const OmnIpAddr &remote_addr,
		const int remote_port,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(mUdpComm, rdata, false);
	aos_assert_rr(msg, rdata, false);
	aos_assert_rr(remote_addr.isValid(), rdata, false);
	aos_assert_rr(remote_port > 0, rdata, false);

	bool rslt = mUdpComm->sendTo(msg, remote_addr, remote_port);
	if (rslt)
	{
		OmnAlarm << "Send Faild" << enderr;
		return false;
	}
	return true;
}
#endif
