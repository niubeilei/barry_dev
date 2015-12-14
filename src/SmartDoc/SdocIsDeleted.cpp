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
// 04/14/2011	Created by Linda
////////////////////////////////////////////////////////////////////////////
#include "SmartDoc/SdocIsDeleted.h"

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
// #include "UserMgmt/Pyemail.h"
#include "SEUtilServer/UserDocMgr.h"
#include "SEServer/SeReqProc.h"
#include "Rundata/Rundata.h"
#include "SEModules/LoginMgr.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


AosSdocIsDeleted::AosSdocIsDeleted(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_ISDELETED, AosSdocId::eIsDeleted, flag)
{
}


AosSdocIsDeleted::~AosSdocIsDeleted()
{
}


bool
AosSdocIsDeleted::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	//Zky3233, Linda, 2011/04/18
 	// <smartdoc 
	//	zky_sdoctp="isdeleted"  //this is sdoc type important!
	//	zky_siteid="100"
	//	zky_otype="zky_smtdoc">
	//<actions>
	//	<action zky_otype="zky_actdoc" type="avercode">
	//		<config minutes="5" EncryptType="MD5">
	//		</config>
	//	</action>
	//</actions>
	//</smartdoc>
	//
	if (!sdoc)
	{
		rdata->setError() << "Missing Smartdoc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosXmlTagPtr actions = sdoc->getFirstChild(AOSTAG_ACTIONS);
	if (!actions)
	{
		rdata->setError() << "Missing Actions doc";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	if (actions)
	{
		bool rslt = AosSdocAction::runActions(actions, rdata);
		if (!rslt) return false;
	}
	rdata->setOk();
	return true;
}

