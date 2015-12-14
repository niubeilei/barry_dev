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
// 08/08/2011	Created by tom
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActCreateLog.h"

#include "API/AosApi.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Util/OmnNew.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"
#include "SEUtil/DocTags.h"
#include "SearchEngine/DocMgr.h"
#include "Util/DynArray.h"
#include "Security/SessionMgr.h"
#include "Security/Ptrs.h"
#include "ValueSel/ValueSel.h"
#include "Actions/Ptrs.h"
#include "Actions/ActUtil.h"
#include "SmartDoc/Ptrs.h"
#include "SmartDoc/SmartDoc.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlDoc.h"
#include "DocSelector/DocSelector.h"
#include "SeLogClient/SeLogClient.h"
using namespace std;

#if 0
AosActCreateLog::AosActCreateLog(const bool flag)
:
AosSdocAction(AOSACTTYPE_CREATELOG, AosActionType::eCreateLog, flag)
{
}


/*
AosActCreateLog::AosActCreateLog(const AosXmlTagPtr &config)
:
AosSdocAction(AosActionType::eCreateLog, false)
{
}
*/


AosActCreateLog::~AosActCreateLog()
{
}


bool
AosActCreateLog::run(const AosXmlTagPtr &action_sdoc, const AosRundataPtr &rdata)
{
	// This function creates a log. The log (which is an XML doc) can be 
	// retrieved through action_sdoc.
	// Action Doc format
	// 	<action_sdoc zky_type=createlog >
	// 		<container>xxx</container>  //select the container
	// 		<logname>xxx</logname>  //select the logname
	//		<log>xxx</log>      //select the log
	//  </action_sdoc>
	
	aos_assert_r(rdata, false);
	aos_assert_rr(action_sdoc, rdata, false);
	
	AosValueRslt value;
	
	// Select the container objid, retrieve it by valueselector.
	bool rslt = AosValueSel::getValueStatic(value, action_sdoc, "container", rdata);
	if (!rslt || value.getTypeStr() != AOSDATATYPE_STRING)
	{
		AosSetError(rdata, "eFailedGetValue");
		OmnAlarm << rdata->getErrmsg() << ". Doc: " 
			<< action_sdoc->toString() << enderr;
		return false;
	}
	OmnString ctnr_objid = value.getStr();
	if (ctnr_objid == "")
	{
		AosSetError(rdata, "eContainerNotFound");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// Select the logname, retrieve it by valueselector
	rslt = AosValueSel::getValueStatic(value, action_sdoc, "logname", rdata);
	if (!rslt || value.getTypeStr() != AOSDATATYPE_STRING)
	{
		AosSetError(rdata, "eFailedGetValue");
		OmnAlarm << rdata->getErrmsg() << ". " 
			<< action_sdoc->toString() << enderr;
		return false;
	}
	OmnString logname = value.getStr();
	AosXmlTagPtr log = AosRunDocSelector(rdata, action_sdoc, "zky_docselector");
	if (!log)
	{
		AosSetError(rdata, "eSmartDocIncorrect");
		OmnAlarm << rdata->getErrmsg() << ". " 
			<< action_sdoc->toString() << enderr;
		return false;
	}

	//return AosSeLogClient::getSelf()->addLog(log, ctnr_objid, logname, rdata);
	return AosAddLog(rdata, ctnr_objid, logname, log->toString());
}


AosActionObjPtr
AosActCreateLog::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActCreateLog(false);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


#endif
