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
// 08/11/2011	Created by Brian Zhang 
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/CheckUnique.h"

#include "DbQuery/Query.h"
#include "SEUtil/IILName.h"
#include "SeReqProc/ReqidNames.h"
#include "SEServer/SeReqProc.h"
#include "SEInterfaces/BitmapObj.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/SmartDocObj.h"
#include "SmartDoc/SMDMgr.h"
#include "XmlUtil/XmlTag.h"


AosCheckUnique::AosCheckUnique(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_CHECKUNIQUE, AosSeReqid::eCheckUnique, rflag)
{
}


bool 
AosCheckUnique::proc(const AosRundataPtr &rdata)
{
	// This function check the whether the request is unique.
	AOSLOG_ENTER_R(rdata, false);

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSLOG_LEAVE(rdata);
		return false;
	}
	
	bool rslt;
	OmnString sobjid = root->getChildTextByAttr("name", "sobjid");
	if (sobjid != "")
	{
		rslt = AosSmartDocObj::procSmartdocsStatic(sobjid, rdata);
		if (!rslt)
		{
			AosSetError(rdata, AosErrmsgId::eFailedToRunSmartDoc);
			OmnAlarm << rdata->getErrmsg() 
				<< ". Smartdoc's objid: " << sobjid << enderr;
			AOSLOG_LEAVE(rdata);
			return false;
		}
		AOSLOG_LEAVE(rdata);
		return true;
	}

	// It checks whether a value is unique. This is determined by the 
	// following:
	// 	[container, attribute]
	// which means that all the data in the container should hold
	// different values of the attribute 'attribute'. This function
	// checks whether a new value is unique. The parameters
	// are passed in through 'args', in the following format:
	// 		args="container:xxx, attr:xxx, value:xxx"
	// The function assumes empty value is not allowed.
	OmnString args = root->getChildTextByAttr("name", "args");
	OmnString container, attr, value;
	AosParseArgs(args, "container", container, "attr", attr, "value", value);
	
	// Make sure the container is not empty
	if (container == "")
	{
		AosSetError(rdata, AosErrmsgId::eMissingContainer);
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	// Make sure the attribute name is not empty
	if (attr == "")
	{
		AosSetError(rdata, AosErrmsgId::eMissingAttrname);
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	
	// Make sure the value is not empty
	if (value == "")
	{
		AosSetError(rdata, AosErrmsgId::eMissingValue);
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	// Ready to query the attribute IIL. If no entries are found, it is unique.
	// AosBitmapObjPtr bitmap = AosBitmapObj::getBitmapStatic();
	AosBitmapObjPtr bitmap = AosGetBitmap();
	AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();
	
	OmnString attr_iilname = AosIILName::composeAttrIILName(attr);
	// SHAWN_WARN, how to set page size???????????
	query_context->setOpr(eAosOpr_eq);
	query_context->setStrValue(value);
	rslt = AosQueryColumn(attr_iilname, 0, bitmap, query_context, rdata);
	if (!rslt)
	{
		AosSetError(rdata, AosErrmsgId::eQueryFailed);
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	bool isUnique = true;
	OmnString resp = "<Contents ";
	if (bitmap->isEmpty())
	{
		// Nothing retrieved. It is unique
		resp << AOSTAG_RESPONSE << "=\"" << isUnique << "\"></Contents>";
		rdata->setResults(resp);
		rdata->setOk();
		AOSLOG_LEAVE(rdata);
		return true;
	}

	OmnString ctnr_iilname = AosIILName::composeCtnrMemberListing(container, "");
	//OmnString ctnr_iilname = AosIILName::composeCtnrMemberObjidIILName(container);
	// SHAWN_WARN, how to set page size???????????
	query_context->setOpr(eAosOpr_an);
	rslt = AosQueryColumn(ctnr_iilname, 0, bitmap, query_context, rdata);
	if (!rslt)
	{
		AosSetError(rdata, AosErrmsgId::eQueryFailed);
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	if (bitmap->isEmpty())
	{
		resp << AOSTAG_RESPONSE << "=\"" << isUnique << "\"></Contents>";
		rdata->setResults(resp);
		rdata->setOk();
		AOSLOG_LEAVE(rdata);
		return true;
	}

	isUnique = false;
	resp << AOSTAG_RESPONSE << "=\"" << isUnique << "\"></Contents>";
	rdata->setResults(resp);
	rdata->setOk();
		AOSLOG_LEAVE(rdata);
	return true;
}

