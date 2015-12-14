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
// 07/17/2010 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DocUtil/SengUtil.h"

#include "Util/File.h"
#include "XmlUtil/XmlTag.h"


/*
OmnString AosGetPrivContainerName(
		const OmnString &siteid, 
		const OmnString &user_ctnr_objid,
		const OmnString &username, 
		const OmnString &ctnrname)
{
	// The container name 'ctnrname' is the name of a user's container. 
	// This function converts the name into the corresponding objid. 
	//
	// It retrieves the user docid based on [siteid, user_ctnr_objid, username]. 
	// After that, it retrieves the user's cloud id. The container objid
	// can then be retrieved from the doc.
	bool duplicated;
	OmnString errmsg;
	//AosXmlTagPtr doc = AosIILMgrObj::getIILMgr()->getCtnrMemRcd(siteid, user_ctnr_objid, 
	//		AOSTAG_USERNAME, username, duplicated, errmsg);
	if (duplicated)
	{
		OmnAlarm << "User name not unique: " << siteid 
			<< ":" << user_ctnr_objid << ":" << username << enderr;
	}

	if (!doc)
	{
		OmnAlarm << "User not found: " << siteid << ":" << user_ctnr_objid
			<< ":" << username << ":" << ctnrname << enderr;
		return "";
	}

	return doc->getAttrStr(AOSTAG_PARENT_CTNR);
}
*/

