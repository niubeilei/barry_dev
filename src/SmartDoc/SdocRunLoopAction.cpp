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
#include "SmartDoc/SdocRunLoopAction.h"

#include "Actions/SdocAction.h"
#include "SEInterfaces/DocClientObj.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "ValueSel/ValueSel.h"
#include "API/AosApiC.h"

#if 0

AosSdocRunLoopAction::AosSdocRunLoopAction(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_RUNLOOPACTION, AosSdocId::eRunLoopAction, flag)
{
}


AosSdocRunLoopAction::~AosSdocRunLoopAction()
{
}


bool
AosSdocRunLoopAction::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata) 
{
	bool islogin = AosCheckIsLogin(rdata);
	if(!islogin) return false;

	if (!sdoc)
	{
		rdata->setError() << "Missing Smartdoc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	if (sdoc->getAttrStr(AOSTAG_OPERATION) == "simulate")
	{
		rdata->setSdocReturnStaus(AOSTAG_STATUS_BREAK);
	}

	AosXmlTagPtr actions = sdoc->getFirstChild(AOSTAG_ACTIONS);
	if (!actions)
	{
		AosSetError(rdata, AosErrmsgId::eMissingTags);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// Query all needed doc 
	AosValueRslt valueRslt;
	bool rslt = AosValueSel::getValueStatic(valueRslt, sdoc, AOSTAG_VALUEDEF, rdata);
	if (!rslt)
	{
		AosSetError(rdata, AosErrmsgId::eFailedGetValue);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	switch(valueRslt.getType())
	{
		case AosDataType::eXmlDoc:
			return runOneLpActions(valueRslt, actions, rdata);

		case AosDataType::eQueryRslt:
			return runMoreLpActions(valueRslt, actions, rdata);

		default:
			OmnAlarm << "Undefined type" << enderr;
			return false;
	}
	
	OmnShouldNeverComeHere;
	return false;
}


bool
AosSdocRunLoopAction::runOneLpActions(
		const AosValueRslt &valueRslt,
		const AosXmlTagPtr &actions,
		const AosRundataPtr &rdata)
{
	AosXmlTagPtr doc = valueRslt.getXmlValue(rdata.getPtr());
	if (!doc)
	{
		rdata->setError() << "Retrieve doc failed";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	rdata->setReceivedDoc(doc, true);
	bool rslt = AosSdocAction::runActions(actions, rdata);
	if (!rslt)
	{
		OmnAlarm << "Failed :" << doc->toString() << enderr;
		return false;
	}
	return  true;
}


bool
AosSdocRunLoopAction::runMoreLpActions(
		AosValueRslt &valueRslt,
		const AosXmlTagPtr &actions,
		const AosRundataPtr &rdata)
{
	while (valueRslt.hasMore())
	{
		u64 docid;
		if (!valueRslt.getU64Value(docid, rdata.getPtr())) return false;
		if (docid != 0)
		{
			AosXmlTagPtr doc = AosDocClientObj::getDocClient()->getDocByDocid(docid, rdata);
			//aos_assert_r(doc, false);
			if (!doc) continue;
			rdata->setReceivedDoc(doc, true);
			//aos_assert_r(AosSdocAction::runActions(actions, rdata), false);
			bool rslt = AosSdocAction::runActions(actions, rdata);
			if (!rslt)
			{
				OmnAlarm << "Failed docid :" << docid << enderr;
			}
			if (rdata->getSdocReturnStaus() == AOSTAG_STATUS_BREAK)
			{
				break;
			}
		}
	}
	
	return true;
}

#endif
