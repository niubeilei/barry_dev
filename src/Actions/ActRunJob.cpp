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
// This action get a function from a dll lib,
// then run the function and get the return value.
//
// Modification History:
// 2012/02/23	Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActRunJob.h"


#include "aosDLL/DllMgr.h"
#include "API/AosApi.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "DocSelector/DocSelector.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DocSelObj.h"
#include "SEInterfaces/JobMgrObj.h"
#include "SmartDoc/SmartDoc.h"
#include "Timer/TimerMgr.h"
#include "Util/OmnNew.h"
#include "ValueSel/ValueSel.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"


AosActRunJob::AosActRunJob(const bool flag)
:
AosSdocAction(AOSACTTYPE_RUNJOB, AosActionType::eRunJob, flag)
{
}


AosActRunJob::~AosActRunJob()
{
}


bool	
AosActRunJob::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	// This action add an attribute to a doc. 
	//run command by [mid=jobmgr,cmdid=runjob,objid=zkydoid5924]
	if (!sdoc)
	{
		AosSetError(rdata, AosErrmsgId::eMissingSmartDoc);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosValueRslt valueRslt;
	bool rslt = AosValueSel::getValueStatic(valueRslt, sdoc, AOSTAG_VALUEDEF, rdata);
	OmnString args = valueRslt.getStr();

	OmnString job_objid = rdata->getArg1(AOSARG_USERDATA);
	AosXmlTagPtr job_doc = AosGetDocByObjid(job_objid, rdata);
	aos_assert_r(job_doc, false);

	// Chen Ding, 2013/06/10
	// rslt = AosModifyDocAttr(job_doc, "zky_scheduled", "false", rdata);
	rslt = AosModifyDocAttrStr(rdata, job_doc, "zky_scheduled", "false", true, true);
	aos_assert_r(rslt, false);

	AosJobMgrObjPtr jobmgr = AosJobMgrObj::getJobMgr();
	aos_assert_r(jobmgr, false);

	rslt = jobmgr->startJob(args, job_objid, 0, rdata);
	aos_assert_r(rslt, false);

	rdata->setOk();
	return true;
}


AosActionObjPtr
AosActRunJob::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActRunJob(false);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


