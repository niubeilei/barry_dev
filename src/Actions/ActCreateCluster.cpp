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
#include "Actions/ActCreateCluster.h"

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
#include "SysMsg/AddClusterMsg.h"
#include <string>
#include <vector>
using namespace std;

AosActCreateCluster::AosActCreateCluster(const bool flag)
:
AosSdocAction(AOSACTTYPE_CREATECLUSTER, AosActionType::eCreateCluster, flag)
{
}


AosActCreateCluster::~AosActCreateCluster()
{
}


bool	
AosActCreateCluster::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	//<action cluster_config_id="xxxx" norm_config_id="xxxx" args="xxxx">
	//</action>
	if (!sdoc)
	{
		AosSetError(rdata, "eMissingSmartDoc");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	OmnString cluster_config_id = sdoc->getAttrStr("cluster_config_id", "");
	aos_assert_r(cluster_config_id != "", false);
	AosXmlTagPtr cluster_config = AosGetDocByObjid(cluster_config_id, rdata);
	aos_assert_r(cluster_config, false);

	OmnString norm_config_id = sdoc->getAttrStr("norm_config_id", "");
	aos_assert_r(norm_config_id != "", false);
	AosXmlTagPtr norm_config = AosGetDocByObjid(norm_config_id, rdata);
	aos_assert_r(norm_config, false);

	OmnString args = sdoc->getAttrStr("args", "-s -log -config");
	aos_assert_r(args != "", false);

	// just need send to svr 0. Ketty 2014/03/24
	AosAppMsgPtr msg = OmnNew AosAddClusterMsg(
	         args, cluster_config, norm_config, 0, 0);
	aos_assert_r(msg, false);
	AosSendMsg(msg);
	rdata->setOk();
	return true;
}


AosActionObjPtr
AosActCreateCluster::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActCreateCluster(false);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}

