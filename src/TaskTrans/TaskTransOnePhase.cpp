////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 08/05/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "TaskTrans/TaskTransOnePhase.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/TaskObj.h"
#include "XmlUtil/XmlTag.h"



AosTaskTransOnePhase::AosTaskTransOnePhase(const bool flag)
:
AosTaskTrans(AOSTASKTRANSTYPE_ONE_PHASE, AosTaskTransType::eOnePhase, flag)
{
	mTransType = AosTaskTransType::eOnePhase;
	if (flag)
	{
		AosTaskTransObjPtr thisptr(this, false);
		AosTaskTransObj::registerTaskTrans(thisptr);
	}
}


AosTaskTransOnePhase::AosTaskTransOnePhase(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
:
AosTaskTrans(AOSTASKTRANSTYPE_ONE_PHASE, AosTaskTransType::eOnePhase, false)
{
	mTransType = AosTaskTransType::eOnePhase;
	if (!config(def, rdata))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosTaskTransOnePhase::~AosTaskTransOnePhase()
{
}


bool
AosTaskTransOnePhase::config(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	return true;
}


AosXmlTagPtr
AosTaskTransOnePhase::createTransDoc(
		const u32 task_seqno,
		const bool is_public,
		const AosRundataPtr &rdata)
{
	// This function creates the task trans doc. The doc should have not
	// been created yet. Otherwise, it is an error.

	aos_assert_rr(!mTransDoc, rdata, mTransDoc);
	OmnString docstr = "<tasktrans ";
	OmnString pubstr = (mIsPublic)?"true":"false";
	docstr << AOSTAG_OTYPE << "=\"" << AOSOTYPE_TASK_TRANS
		<< "\" " << AOSTAG_CTNR_PUBLIC << "=\"" << pubstr  
		<< "\" " << AOSTAG_PUBLIC_DOC << "=\"" << pubstr
		<< "\" " << AOSTAG_STYPE << "=\"" << AOSSTYPE_TASK_TRANS
		<< "\" " << AOSTAG_TAG << "=\"" << mTransTags
		<< "\" " << AOSTAG_JOB_OBJID << "=\"" << mJobObjid
		<< "\" " << AOSTAG_JOB_DOCID << "=\"" << mJobDocid
		<< "\" " << AOSTAG_JOBSERVERID << "=\"" << mJobServerId
		<< "\" " << AOSTAG_STATUS << "=\"" << statusToStr(mStatus)
		<< "\" " << AOSTAG_FINISHED << "=\"" << "false"
		<< "\" " << AOSTAG_PARENTC << "=\"" << mTransContainer
		<< "\" " << AOSTAG_TRANS_TYPE << "=\"" << AosTaskTransType::toStr(mTransType)
		<< "\" " << AOSTAG_TASK_TRANSID << "=\"" << mTransId
		<< "\" " << AOSTAG_NEXT_TASK_TRANSID << "=\"" << mNextTransId
		<< "\" " << AOSTAG_TRANS_PROGRESS << "=\"0" 
		<< "\" >"
		<< "<description><![CDATA[" << mDescription << "]]>"
		<< "</description>"
		<< "</tasktrans>";

	try
	{
		mTransDoc = AosCreateDoc(docstr, is_public, rdata);
		aos_assert_r(mTransDoc, 0);
		return mTransDoc;
	}

	catch (...)
	{
		AosSetErrorU(rdata, "failed_create_task");
		AosLogError(rdata);
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


bool
AosTaskTransOnePhase::log(const OmnString &logkey, const AosRundataPtr &rdata) const
{
	OmnString docstr = "<translog ";
	docstr << AOSTAG_PARENTC << "=\"" << mLogContainer
		<< "\" " << AOSTAG_LOG_TYPE << "=\"" << AOSOTYPE_TASKTRANSLOG
		<< "\" " << AOSTAG_LOG_ENTRY_KEY << "=\"" << logkey 
		<< "\" " << AOSTAG_JOB_OBJID << "=\"" << mJobObjid
		<< "\" " << AOSTAG_JOB_DOCID << "=\"" << mJobDocid
		<< "\" " << AOSTAG_TASK_TRANSID << "=\"" << mTransId
		<< "\" >" 
		<< "</translog>";
	
	OmnScreen << docstr << endl;
	return AosAddLog(mLogContainer, docstr, rdata);
}


bool
AosTaskTransOnePhase::pause(const AosRundataPtr &rdata) 
{
	OmnNotImplementedYet;
	return false;
}


bool
AosTaskTransOnePhase::resume(const AosRundataPtr &rdata) 
{
	OmnNotImplementedYet;
	return false;
}


bool
AosTaskTransOnePhase::taskFinished(const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosTaskTransOnePhase::taskSuccessed(const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosTaskTransOnePhase::taskFailed(const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosTaskTransOnePhase::run(const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosTaskTransOnePhase::abort(const AosRundataPtr &rdata) 
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosTaskTransOnePhase::serializeTo(AosBuffPtr &buff, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}

	
bool
AosTaskTransOnePhase::serializeFrom(const AosBuffPtr &buff, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


int
AosTaskTransOnePhase::getProgress() const
{
	OmnNotImplementedYet;
	return -1;
}

	
OmnString
AosTaskTransOnePhase::toString() const
{
	OmnString ss;
	ss  << "TaskTransOnePhase: " 
		<< "\n    Type: " << mTransType
		<< "\n    Name: " << mName.data()
		<< "\n    TransId: " << mTransId
		<< "\n    Next TransId: " << mNextTransId
		<< "\n    Progress: " << mProgress
		<< "\n    Status: " << mStatus
		<< "\n    Job Objid: " << mJobObjid
		<< "\n    Job Docid: " << mJobDocid
		<< "\n    Job Server ID: " << mJobServerId;
	return ss;
}


bool
AosTaskTransOnePhase::modifyTransStatus(
		const AosXmlTagPtr &transDoc, 
		const OmnString &taskStatus, 
		const AosRundataPtr &rdata)
{
	if (!transDoc)
	{
		AosSetErrorU(rdata, "internal_error") << enderr;
		return false;
	}
		
	bool rslt = AosModifyDocAttr(transDoc, AOSTAG_STATUS, taskStatus, rdata);
	if (!rslt)
	{
		AosSetErrorU(rdata, "failed_modify_task_doc") << ": " << transDoc->toString();;
		AosLogError(rdata);
		return false;
	}
	return true;
}

