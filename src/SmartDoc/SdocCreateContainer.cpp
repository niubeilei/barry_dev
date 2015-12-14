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
// 04/13/2011	Created by Ketty
////////////////////////////////////////////////////////////////////////////
#include "SmartDoc/SdocCreateContainer.h"

#include "SEInterfaces/DocClientObj.h"
#include "UtilComm/TcpClient.h"
#include "Actions/ActSeqno.h"
#include "Rundata/Rundata.h"
#include "SEUtilServer/CloudidSvr.h"
#include "SEModules/LoginMgr.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlInterface/WebRequest.h"

AosSdocCreateContainer::AosSdocCreateContainer(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_CREATE_CTNR, AosSdocId::eCreateContainer, flag)
{
}


AosSdocCreateContainer::~AosSdocCreateContainer()
{
}

bool
AosSdocCreateContainer::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata) 
{
	if (!sdoc)
	{
		rdata->setError() << "Missing Smartdoc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	u32 siteid = rdata->getSiteid();
	OmnString caller_cid = AosCloudidSvr::getCloudid(rdata->getUserid(), rdata);
	OmnString ctnr_objid= rdata->getArg1(AOSARG_CTNR_OBJID);
	OmnString stype = rdata->getArg1(AOSARG_CTNR_SUBTYPE);
	//int ttl = 0;
	OmnString parent_ctnr_objid;

	AosXmlTagPtr ctnrDoc;
	AosXmlTagPtr tempCtnr = sdoc->getFirstChild(AOSTAG_CTNR_TEMP);
	bool check_ctnr = true;
	
	if(tempCtnr)
	{
		parent_ctnr_objid = tempCtnr->getAttrStr(AOSTAG_PARENTC, "");
		if(parent_ctnr_objid=="")
		{
			parent_ctnr_objid= rdata->getArg1(AOSARG_PARENT_CTNR_OBJID);
			check_ctnr = false;
		}

		tempCtnr = tempCtnr->clone(AosMemoryCheckerArgsBegin);
		tempCtnr->setAttr(AOSTAG_OBJID, ctnr_objid);		
		tempCtnr->setAttr(AOSTAG_SITEID, siteid);		
		tempCtnr->setAttr(AOSTAG_OTYPE, AOSOTYPE_CONTAINER);		
		tempCtnr->setAttr(AOSTAG_STYPE, AOSSTYPE_AUTO);		
		tempCtnr->setAttr(AOSTAG_PARENTC, parent_ctnr_objid);		
		tempCtnr->setAttr(AOSTAG_CREATOR, caller_cid);		
		tempCtnr->setAttr(AOSTAG_SUBTYPE, stype);
		
		AosXmlTagPtr dd = AosDocClientObj::getDocClient()->createDocSafe3(rdata, tempCtnr, "", "",
							true, false, false, false, false, check_ctnr, true);
		if(!dd)
		{
			rdata->setError() << "Create container Error!";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
		ctnrDoc = tempCtnr;

	}
	else
	{
		OmnString docstr = "<container ";
		docstr << AOSTAG_OBJID << "=\"" << ctnr_objid<< "\" "
			   << AOSTAG_SITEID << "=\"" << siteid << "\" "
			   << AOSTAG_OTYPE << "=\"" << AOSOTYPE_CONTAINER << "\" "
			   << AOSTAG_STYPE << "=\"" << AOSSTYPE_AUTO << "\" "
			   << AOSTAG_PARENTC << "=\"" << parent_ctnr_objid << "\" "
			   << AOSTAG_CREATOR << "=\"" << caller_cid << "\" ";
		if (stype != "")
		{	
			docstr << AOSTAG_SUBTYPE << "=\"" << stype << "\"";
		}
		docstr << " />";

		ctnrDoc = AosDocClientObj::getDocClient()->createDocSafe1(rdata, docstr, "", "",
							true, false, false, false, false, false);
	}

	if (!ctnrDoc)
	{
		rdata->setError() << "Create container Error!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	u64 ctnrDocid = ctnrDoc->getAttrU64(AOSTAG_DOCID, 0);
	
	// set AccessRecord
	AosXmlTagPtr tempAccess = sdoc->getFirstChild(AOSTAG_TEMPACCESS);
	if (tempAccess)
	{
		tempAccess = tempAccess->clone(AosMemoryCheckerArgsBegin);
		//Ice AdmDoc
		//OmnString accessObjid = AosObjid::createArdObjid(ctnrDocid);
		//tempAccess->setAttr(AOSTAG_OBJID, accessObjid);		
		tempAccess->setAttr(AOSTAG_OWNER_DOCID, ctnrDocid);		
		tempAccess->setAttr(AOSTAG_OTYPE, AOSOTYPE_ACCESS_RCD);		
		tempAccess->setAttr(AOSTAG_SUBTYPE, AOSDOCSTYPE_AUTO);		

		bool rslt = AosDocClientObj::getDocClient()->writeAccessDoc(ctnrDocid, tempAccess, rdata);
		if (!rslt)
		{
			rdata->setError() << "Create container Access Doc Error!";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
	}

	rdata->setRetrievedDoc(ctnrDoc, true);
	rdata->setOk();
	return true;
}


