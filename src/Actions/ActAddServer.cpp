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
// This action sets an attribute:
// 	<action zky_type="AOSACTION_SETATTR" xpath="xxx">
// 		<doc .../>
// 		<value .../>
// 	</action>
// 	where <doc> is a Doc Selection tag. <value> is a Value Tag. 
// 	This is the value to be set.
//
// Modification History:
// 09/15/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActAddServer.h"

#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "DocSelector/DocSelector.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DocSelObj.h"
#include "SmartDoc/SmartDoc.h"
#include "Util/OmnNew.h"
#include "Util/DynArray.h"
#include "ValueSel/ValueSel.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
//#include "AdminTrans/AdminAddServerTrans.h"
#include "SysMsg/AddServerMsg.h"
#include <string>
#include <vector>
using namespace std;

AosActAddServer::AosActAddServer(const bool flag)
:
AosSdocAction(AOSACTTYPE_ADDSERVER, AosActionType::eAddServer, flag)
{
}


AosActAddServer::~AosActAddServer()
{
}

AosXmlTagPtr
AosActAddServer::getAdminConfig(const AosXmlTagPtr &svr_proxy_config)
{
	OmnString net_str;
	net_str << "<networkmgr "
			<< "zky_procid=\"1\" "
			<< "replic_policy=\"nobkp\" "
			<< "cube_grp_id=\"0\" "
			<< "proc_type=\"localserver\" "
			<< "vids=\"0\"><servers></servers></networkmgr>";
	AosXmlParser parser;
	AosXmlTagPtr admin_config = parser.parse(net_str, "" AosMemoryCheckerArgs);
	aos_assert_r(admin_config, 0);

	OmnString svr_str;
	svr_str << "<server>"
			<< "<Process proc_id=\"1\" "
			<< "proc_type=\"localserver\" "
			<< "cube_grp_id=\"0\"/></server>";
	AosXmlTagPtr svr_config = parser.parse(svr_str, "" AosMemoryCheckerArgs);
	aos_assert_r(svr_config, 0);


	AosXmlTagPtr servers_tag = admin_config->getFirstChild("servers");
	AosXmlTagPtr tags = svr_proxy_config->getFirstChild("servers");
	aos_assert_r(tags, 0);
	AosXmlTagPtr tag = tags->getFirstChild();
	while(tag)
	{
		int svr_id = tag->getAttrInt("server_id", -1);
		aos_assert_r(svr_id != -1, 0);

		AosXmlTagPtr clone_svr_tag = svr_config->clone(AosMemoryCheckerArgsBegin);
		aos_assert_r(clone_svr_tag, 0);
		clone_svr_tag->setAttr("server_id", svr_id);

		servers_tag->addNode(clone_svr_tag);

		tag = tags->getNextChild();
	}
	return admin_config;
}

bool	
AosActAddServer::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	//<action svr_proxy_config_id="xxxx">
	//</action>
	if (!sdoc)
	{
		AosSetError(rdata, "eMissingSmartDoc");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	OmnString svr_proxy_config_id = sdoc->getAttrStr("svr_proxy_config_id", "");
	aos_assert_r(svr_proxy_config_id != "", false);

	AosXmlTagPtr svr_proxy_config = AosGetDocByObjid(svr_proxy_config_id, rdata);
	aos_assert_r(svr_proxy_config, false);

	AosXmlTagPtr admin_config = getAdminConfig(svr_proxy_config);
	aos_assert_r(admin_config, false);

	bool rslt = false;
	AosXmlTagPtr sconfig, aconfig;
	sconfig = svr_proxy_config->clone(AosMemoryCheckerArgsBegin);
	aconfig = admin_config->clone(AosMemoryCheckerArgsBegin);
	
	u32 svr_id = AosGetSelfServerId();
	u32 clt_id = AosGetSelfClientId();
	sconfig->setAttr(AOSTAG_PHYSICAL_SERVER_ID, svr_id);
	sconfig->setAttr(AOSTAG_PHYSICAL_CLIENT_ID, svr_id);

	aconfig->setAttr(AOSTAG_PHYSICAL_SERVER_ID, svr_id);
	aconfig->setAttr(AOSTAG_PHYSICAL_CLIENT_ID, clt_id);
	
	rslt = AosNetworkMgrObj::getNetworkMgr()->configServers(sconfig);
	aos_assert_r(rslt, false);

	AosAppMsgPtr msg = OmnNew AosAddServerMsg(
	        sconfig, aconfig, AosGetSelfServerId(), 0);
	rslt = AosSendMsg(msg);
	aos_assert_r(rslt, false);

	rdata->setOk();
	return true;
}


AosActionObjPtr
AosActAddServer::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActAddServer(false);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
