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
#include "SmartDoc/SdocCounter.h"

#include "SEInterfaces/DocClientObj.h"
#include "SEInterfaces/IILClientObj.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/DocTags.h"
#include "Security/Session.h"
#include "Security/SecurityMgr.h"
#include "Security/SessionMgr.h"
#include "Util/String.h"
#include "SEUtilServer/UserDocMgr.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"


AosSdocCounter::AosSdocCounter(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_COUNTER, AosSdocId::eCounter, flag)
{
}


AosSdocCounter::~AosSdocCounter()
{
}


bool
AosSdocCounter::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
 	// <smartdoc 
	//	zky_public_ctnr="true"
	//	zky_public_doc="true" 
	//	zky_sdoctp="Counter"  //this is sdoc type important!
	//	zky_iilname="vote_conter" //iilname
	//	zky_objid="sdoc_conter" //objid
	//	zky_count="0" //iil value
	//	zky_cntopt="visitor" //it decide count for who    visotor  or owner
	//	zky_cnttype="vote" //count type "nomal" or "vote"
	//	zky_subcnttype="zky_ml2"
	//	zky_siteid="100">
	//	<vote>
	//		<type name="zky_voteup" value="100" flag="true"></type> //true: add, false: subtract.
	//		<type name="zky_votedown" value="50" flag="false"></type>
	//	</vote>
	//</smartdoc>
	u32 siteid = rdata->getSiteid();

	if (!sdoc)
	{
		rdata->setError() << "Missing sdoc";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	OmnString nn = sdoc->getAttrStr(AOSTAG_IILNAME);
	if(nn == "")
	{
		rdata->setError() << "Failed to retrieve the IILNAME: ";
		OmnAlarm << "IILNAME is nill" << enderr;
		return false;
	}

	OmnString iilname = AOSZTG_CNTPREFIX;
	iilname << siteid << ":" << nn;

	//tell which type
	u64 userid;
	u64 value;
	bool flag = true;
	OmnString counter_opt = sdoc->getAttrStr(AOSTAG_COUNTER_OPERATION);
	OmnString counter_type = sdoc->getAttrStr(AOSTAG_COUNTER_TYPE);
	OmnString subCounter_type = sdoc->getAttrStr(AOSTAG_COUNTER_SUBTYPE);
	if(counter_opt == "")
	{
		OmnAlarm << "counter_opt is empty!" << enderr;
		return false;
	}

	if(counter_type == "" || subCounter_type == "")
	{
		OmnAlarm << "counter_type is empty!" << enderr;
		return false;
	}
	
	const char *data = counter_opt.data();
	switch (data[0])
	{
		case 'o':
			if(counter_opt == "owner")
			{
				userid = rdata->getAccessedDocUserid();
				if(userid == AOS_INVDID)
				{
					rdata->setError() << "userid is invalid !";
					OmnAlarm << rdata->getErrmsg() << enderr;
					return false;
				}
				AosXmlTagPtr type = sdoc->getFirstChild(counter_type);
				if(!type)
				{
					rdata->setError() << "No type be finded:" << counter_type;
					OmnAlarm << rdata->getErrmsg() << enderr;
					return false;
				}
				AosXmlTagPtr subType = type->getChildByAttr("name", subCounter_type);
				if(!subType)
				{
					rdata->setError() << "No subtype be finded:" << subCounter_type;
					OmnAlarm << rdata->getErrmsg() << enderr;
					return false;
				}
				value = subType->getAttrU64("value", 0);
				if (value == 0)
				{
					return true;
				}
				flag = subType->getAttrBool("flag");
			break;
			}
			return true;
		
		case 'v':
			if(counter_opt == "visitor")
			{
				userid = rdata->getUserid();
				if(userid == AOS_INVDID)
				{
					rdata->setError() << "userid is invalid !";
					OmnAlarm << rdata->getErrmsg() << enderr;
					return false;
				}
				AosXmlTagPtr type = sdoc->getFirstChild(counter_type);
				if(!type)
				{
					rdata->setError() << "No type be finded:" << counter_type;
					OmnAlarm << rdata->getErrmsg() << enderr;
					return false;
				}
				AosXmlTagPtr subType = type->getChildByAttr("name", subCounter_type);
				if(!subType)
				{
					rdata->setError() << "No subtype be finded:" << subCounter_type;
					OmnAlarm << rdata->getErrmsg() << enderr;
					return false;
				}
				value = subType->getAttrU64("value", 0);
				if (value == 0)
				{
					return true;
				}
				flag = subType->getAttrBool("flag");
			break;
			}
			return true;

		default:
			AosSetErrorUser(rdata, "counter_type is invalid !") << enderr;
			return false;
	}

	bool rslt;
	u64 counter = 0;
	//rslt = AosIILClient::getSelf()->updateKeyedValue(iilname, userid, flag, value, counter, rdata);
	rslt = AosIILClientObj::getIILClient()->updateKeyedValue(iilname, userid, flag, value, counter, rdata);
	if(!rslt)
	{
		rdata->setError() << "Failed to incremente counter: " << iilname;
		return false;
	}

	//this can show at client.
	OmnString counter_str;
	counter_str << counter;
	rdata->setResults(counter_str);
	//int ttl = 0;
	OmnString errmsg;
	//--Ketty
	/*rslt = AosDocServer::getSelf()->modifyAttrStr1(AOSAPPNAME_SYSTEM, userid, 
			sdoc->getAttrU64(AOSTAG_DOCID, 0), 
			sdoc->getAttrStr(AOSTAG_OBJID), 
			"zky_count", counter_str, "", false, false, 
			errcode, errmsg, ttl, __FILE__, __LINE__);*/
	rslt = AosDocClientObj::getDocClient()->modifyAttrStr1(rdata, 
			sdoc->getAttrU64(AOSTAG_DOCID, 0), 
			sdoc->getAttrStr(AOSTAG_OBJID), 
			"zky_count", counter_str, "", false, false, true);
	//rdata->setError(errcode, errmsg);
	return rslt;
}

