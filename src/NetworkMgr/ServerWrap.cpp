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
////////////////////////////////////////////////////////////////////////////
#include "NetworkMgr/ServerWrap.h"

#include "API/AosApi.h"
#include "SEUtil/ValueDefs.h"


AosServerWrap::AosServerWrap(
		const ServerType type,
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata) 
:
mName(""),
mAddr(""),
mPort(-1),
mServerType(type),
mPhysicalId(-1)
{
	if (!config(def, rdata))
	{
		OmnThrowException(rdata->getErrmsg());
	}
}


AosServerWrap::~AosServerWrap()
{
}


bool
AosServerWrap::config(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
{
	if (!def)
	{
		AosSetErrorU(rdata, "config_is_null") << enderr;
		return false;
	}

	mName = def->getAttrStr("name");
	mAddr = def->getAttrStr("addr");
	mPort = def->getAttrInt("port", -1);
	mNumPorts = def->getAttrInt("num_port", 0);
	mPhysicalId = def->getAttrInt("physical_id", -1);
	if (mPhysicalId < 0)
	{
		AosSetErrorU(rdata, "invalid_physical_id") << ": " << def->toString() << enderr;
		return false;
	}

	aos_assert_r(mAddr.isValid(), false);
	aos_assert_r(mPort > 0, false);
	aos_assert_r(mNumPorts != 0, false);

	return true;
}


//bool 
//AosServerWrap::stopServer(
//		const u64 &admin_transid, 
//		const AosNetworkMgrPtr &server_msg,
//		const AosRundataPtr &rdata)
//{
//	// This function sends a request to stop the remote server. 
//	// After that, it creates a server transaction and adds it 
//	// to NetworkMgr.
//	OmnString str = "<req ";
//	str << AOSTAG_ADMIN_TRANSID << "=\"" << admin_transid 
//		<< "\" " << AOSTAG_ADMIN_TRANSTYPE << "=\"" << AosAdminTransType::eStopServer
//		<< "\" " << AOSTAG_ADMIN_COMMAND << "=\"" << AOSCOMMAND_STOP_SERVER
//		<< "\" " << AOSTAG_SERVER_NAME << "=\"" << mName 
//		<< "\"/>";
//
//	try
//	{
//		AosXmlTagPtr cmd = AosXmlParser::parse(str);
//		AosServerTransPtr trans = OmnNew AosServerTrans(transid, 
//				AosServerTransType::eStopServer, 
//				cmd, rdata);
//		server_mgr->addServerTrans(transid, trans, rdata);
//		server_mgr->sendMsg(cmd, mAddr, mPort, rdata);
//	}
//
//	catch (...)
//	{
//		OmnAlarm << "Failed creating the message" << enderr;
//		return false;
//	}
//
//	return true;
//}


//bool
//AosServerWrap::verifyServer(const AosXmlTagPtr &response, const AosRundataPtr &rdata)
//{
//	aos_assert_r(response, false);
//	aos_assert_r(mName == response->getAttrStr(AOSTAG_SERVER_NAME), false);
//	return true;
//}


OmnString
AosServerWrap::getServerInfo(const AosXmlTagPtr &req, const AosRundataPtr &rdata)
{
	/*
	aos_assert_rr(req, rdata, "internal_error");
	if (!isServerLocal())
	{
	}

	// The server is local. 
	OmnString type = req->getAttrStr(AOSTAG_INFO_NAME);
	if (type == AOSVALUE_IPADDR)
	{
		return mAddr;
	}

	if (type == AOSVALUE_NUMBER_CPUS)
	{
		OmnString ss;
		ss << getNumCPUs();
		return mNumCPUs;
	}

	if (type == AOSVALUE_CPU_INFO)
	{
		return getCpuInfo();
	}

	return AOSDICTTERM("unrecog_server_info", rdata) << ": " << type;
	*/
	OmnNotImplementedYet;
	return "";
}


bool
AosServerWrap::isServerLocal() const
{
	aos_assert_r(mPhysicalId >= 0, true);
	return AosIsPhysicalIdLocal(mPhysicalId);
}


AosXmlTagPtr
AosServerWrap::getRuntimeInfo(const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return 0;
}

