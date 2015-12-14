////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 08/13/2010 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SearchEngine/CtnrMgr.h"

#include "SearchEngine/DocServer1.h"
#include "SearchEngine/DocServerCb.h"
#include "SEUtil/DocTags.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "SmartDoc/SmartDoc.h"


AosCtnrMgr::AosCtnrMgr()
{
}


AosCtnrMgr::~AosCtnrMgr()
{
}


/*
bool 
AosCtnrMgr::createSysCtnr(const OmnString &siteid, const OmnString &ctnr_objid)
{
	// It checks whether it is a system defined container. If not, 
	// it returns false. Otherwise, it creates the container.
	//
	// All system defined containers are in the format:
	// 	zkyctnr_ + <xxx>
	//
	if (ctnr_objid.length() < 9) return false;

	const char *data = ctnr_objid.data();
	if (data[0] != 'z' ||
		data[1] != 'k' ||
		data[2] != 'y' ||
		data[3] != 'c' ||
		data[4] != 't' ||
		data[5] != 'n' ||
		data[6] != 'r' ||
		data[7] != '_') return false;

	switch (data[8])
	{
	case 's':
		 if (ctnr_objid == AOSOBJID_SYSBACKUP) return createSysBackCtnr(siteid);
		 break;

	default:
		 break;
	}

	return false;
}
*/


/*
bool 
AosCtnrMgr::createSysBackCtnr(const OmnString &siteid)
{
	OmnString contents = "<ctnr ";
	contents << AOSTAG_OBJID << "=\"" << AOSOBJID_SYSBACKUP << "\" "
		<< AOSTAG_SITEID << "=\"" << siteid << "\" "
		<< AOSTAG_OTYPE << "=\"" << AOSOTYPE_CONTAINER << "\" "
		<< AOSTAG_SUBTYPE << "=\"" << AOSSTYPE_BACKUP_CTNR << "\">"
		<< "The container that contains all system backup logs"
		<< "</ctnr>";

	AosXmlRc errcode;
	OmnString errmsg;
	bool rslt = AosDocServer::getSelf()->createDocSafe1(contents, 
		siteid, AOSUSERID_SYSTEM, AOSAPPNAME_SYSTEM, "", 
		true, errcode, errmsg, false, false);
	aos_assert_r(rslt, false);
	return true;
}
*/

