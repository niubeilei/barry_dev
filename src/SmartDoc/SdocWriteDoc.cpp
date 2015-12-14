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
// 04/22/2011	Created by Linda
////////////////////////////////////////////////////////////////////////////
#include "SmartDoc/SdocWriteDoc.h"

#include "Actions/ActSeqno.h"
#include "SEUtil/Ptrs.h"
#include "SEBase/SeUtil.h"
#include "SEUtil/DocTags.h"
#include "Security/Session.h"
#include "Security/SecurityMgr.h"
#include "Security/SessionMgr.h"
#include "SmartDoc/SmartDoc.h"
#include "SmartDoc/SMDMgr.h"
#include "Util/String.h"
#include "SEUtilServer/UserDocMgr.h"
#include "SEServer/SeReqProc.h"
#include "Rundata/Rundata.h"
#include "SEModules/LoginMgr.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


AosSdocWriteDoc::AosSdocWriteDoc(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_WRITEDOC, AosSdocId::eWriteDoc, flag)
{
}


AosSdocWriteDoc::~AosSdocWriteDoc()
{
}


bool
AosSdocWriteDoc::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	//Zky3293, Linda, 2011/04/18
	//createdoc
 	// <smartdoc 
	//	zky_sdoctp="writedoc"  //this is sdoc type important!
	//	zky_siteid="100"
	//	zky_otype="zky_smtdoc">
	//		<actions>
	//			<action>
	//			.....
	//			</action>
	//		</actions>
	//		<seqno ...>
	//		</seqno>
	//
	//</smartdoc>
	//
	if (!sdoc)
	{
		rdata->setError() << "Missing Smartdoc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	bool rslt = createDoc(sdoc, rdata);
	if (!rslt) return false; 

	rdata->setOk();
	return true;
}

bool
AosSdocWriteDoc::createDoc(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	AosXmlTagPtr actions = sdoc->getFirstChild(AOSTAG_ACTIONS);
	if (!actions)
	{
		rdata->setError() << "Missing Actions doc";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	AosXmlTagPtr action = actions->getFirstChild(AOSTAG_ACTION);
	if (!action)
	{
		rdata->setError() << "Missing Actions doc";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	//parseActions(sdoc, mActions, rdata);
	//rdata->setCrtSmartdoc(this);
	//setSdoc(action);
	AosXmlTagPtr req = rdata->getReceivedDoc();
	if (!req)
	{
		rdata->setError()<< "Request incorrect!" ;
		OmnAlarm << rdata->getErrmsg() << enderr; 
		return false;
	}
	AosXmlTagPtr rootchild = req->getFirstChild(); 
	if(!rootchild)
	{
		rdata->setError()<< "Request incorrect!" ;
		OmnAlarm << rdata->getErrmsg() << enderr; 
		return false;
	}

	AosXmlTagPtr objdef = rootchild->getFirstChild("objdef");
	if (!objdef)
	{
		rdata->setError()<< "Missing objdef!" ;
		OmnAlarm << rdata->getErrmsg() << enderr; 
		return false;
	}

	return callActions(actions, rdata);
}

bool
AosSdocWriteDoc::callActions(const AosXmlTagPtr &actions, const AosRundataPtr &rdata)
{
	if (actions)
	{
		bool rslt = AosSdocAction::runActions(actions, rdata);
		if (!rslt) return false;
	}

	return true;
	//for (int i=0; i<(int) mActions.size(); i++)
	//{
	//	if (!mActions[i])
	//	{
	//		rdatga->setError() << "Failed to create the action: " << action->toString();
	///		OmnAlarm << rdata->getErrmsg() <<enderr;
	//		return false;
	//	}
	//	bool rslt = mActions[i]->run(action, rdata);
	//	if (!rslt)
	//	{
	//		return false;
	//	}
	//}
}

