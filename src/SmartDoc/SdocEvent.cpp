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
// 05/18/2011	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#if 0
#include "SmartDoc/SdocEvent.h"

#include "UtilComm/TcpClient.h"
#include "Actions/ActSeqno.h"
#include "Rundata/Rundata.h"
#include "SEUtilServer/CloudidSvr.h"
#include "SEModules/LoginMgr.h"
#include "SmartDoc/SMDMgr.h"
#include "XmlInterface/WebRequest.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"

AosSdocEvent::AosSdocEvent(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_EVENT, AosSdocId::eEvent, flag)
{
}


AosSdocEvent::~AosSdocEvent()
{
}


bool
AosSdocEvent::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata) 
{
	//<smartdoc zky_sdoctp="event" zky_otype="zky_smtdoc">
	//<EVENT_CTNR_ADDMEMBER>
	//	<AOSTAG_OBJID>xxx</AOSTAG_OBJID>
	//	<AOSTAG_OBJID>xxx</AOSTAG_OBJID>
	//	<AOSTAG_OBJID>xxx</AOSTAG_OBJID>
	//	...
	//</EVENT_CTNR_ADDMEMBER>
	//<EVENT_CTNR_MODFIYMEMBER>
	//	<AOSTAG_OBJID>xxx</AOSTAG_OBJID>
	//	<AOSTAG_OBJID>xxx</AOSTAG_OBJID>
	//	<AOSTAG_OBJID>xxx</AOSTAG_OBJID>
	//	...
	//</EVENT_CTNR_MODFIYMEMBEER>
	//...
	//</smartdoc>
	
	aos_assert_r(rdata, false);
	if (!sdoc)
	{
		rdata->setError() << "Missing Smartdoc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
//	AosSessionPtr session = rdata->getSession();
//	if (!session)
//	{
//		rdata->setError() << "User not logged in!";
//		OmnAlarm << rdata->getErrmsg()<< enderr;
//		return false;
//	}

	OmnString event = getEvent();
	if (event == "")
	{
		rdata->setError() << "Event is NULL!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosXmlTagPtr eventxml = sdoc->getFirstChild(event);
	if (eventxml)
	{
		AosXmlTagPtr objidxml = eventxml->getFirstChild();
		while(objidxml)
		{
			OmnString objid = objidxml->getNodeText();
			if (objid!="")
			{
				AosSMDMgr::procSmartdocs(objid, rdata, event);
				//AosSMDMgr::procSmartdocs(event, objid, rdata);
			}
			objidxml = eventxml->getNextChild();
		}
	}
	return true;
}
#endif
