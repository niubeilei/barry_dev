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
// 2013/08/20 Created by Ken Lee 
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/CheckIILEntry.h"

#include "API/AosApi.h"
#include "SeReqProc/ReqidNames.h"
#include "SEServer/SeReqProc.h"
#include "XmlUtil/XmlTag.h"


AosCheckIILEntry::AosCheckIILEntry(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_CHECKIILENTRY, AosSeReqid::eCheckIILEntry, rflag)
{
}


bool 
AosCheckIILEntry::proc(const AosRundataPtr &rdata)
{
	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
		return false;
	}
	
	OmnString args = root->getChildTextByAttr("name", "args");
	OmnString key, value_str, objid;
	AosParseArgs(args, "key", key, "value", value_str, "objid", objid);
	
	if (key == "")
	{
		rdata->setError() << "Missing key";
		return false;
	}

	if (value_str == "")
	{
		rdata->setError() << "Missing value";
		return false;
	}
	
	u64 value = value_str.toU64();
	
	if (objid == "")
	{
		rdata->setError() << "Missing objid";
		return false;
	}

	AosXmlTagPtr xml = AosGetDocByObjid(objid, rdata);
	if (!xml)
	{
		rdata->setError() << "Missing xml";
		return false;
	}

	AosXmlTagPtr tag = xml->getFirstChild(true);
	if (!tag)
	{
		rdata->setError() << "Missing value";
		return false;
	}

	OmnString iilname;
	bool need_exist, rslt;
	bool check_rslt = true;
	u64 vv;

	while (tag)
	{
		iilname = tag->getAttrStr(AOSTAG_IILNAME);
		if (iilname == "")
		{
			rdata->setError() << "Missing iilname";
			return false;
		}

		need_exist = tag->getAttrBool("need_exist", true);

		rslt = AosIILClientObj::getIILClient()->getDocid(iilname, key, vv, rdata);
		if (!rslt)
		{
			rdata->setError() << "error";
			return false;
		}

		if (!need_exist)
		{
			if (vv > 0)
			{
				check_rslt = false;
				break;
			}
		}
		else
		{
			if (vv == 0 || vv != value)
			{
				check_rslt = false;
				break;
			}
		}
		tag = xml->getNextChild();
	}

	OmnString resp = "<Contents ";
	resp << AOSTAG_RESPONSE << "=\"" << check_rslt << "\"></Contents>";
	rdata->setResults(resp);
	rdata->setOk();
	return true;
}

