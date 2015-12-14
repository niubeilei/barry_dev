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
// 2012/05/11	Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActIILBatchOprMerge.h"

#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/BitmapEngineObj.h"
#include "SEInterfaces/TaskDataObj.h"
#include "SEInterfaces/IILExecutorObj.h"
#include "SEInterfaces/IILMgrObj.h"
#include "SmartDoc/SmartDoc.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"


AosActIILBatchOprMerge::AosActIILBatchOprMerge(const bool flag)
:
AosSdocAction(AOSACTTYPE_IILBATCHOPRMERGE, AosActionType::eIILBatchOprMerge, flag)
{
}


AosActIILBatchOprMerge::~AosActIILBatchOprMerge()
{
}


bool
AosActIILBatchOprMerge::run(
		const AosTaskObjPtr &task, 
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	if (!sdoc)
	{
		AosSetError(rdata, "eMissingSmartDoc");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	if(!task)
	{
		AosSetError(rdata, "eMissingSmartDoc");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	AosXmlTagPtr dataCol_tag = sdoc->getFirstChild("data_collector");
	aos_assert_r(dataCol_tag, false);

	AosXmlTagPtr iiltag = dataCol_tag->getFirstChild("iilasm");
	aos_assert_r(iiltag, false);

	OmnString iilname = iiltag->getAttrStr(AOSTAG_IILNAME);
	aos_assert_r(iilname != "", false);

	bool true_delete = sdoc->getAttrBool(AOSTAG_TREUDELETE, false);
	
	int len = iiltag->getAttrInt(AOSTAG_LENGTH, -1);
	aos_assert_r(len > 0, false);

	OmnString type = iiltag->getAttrStr(AOSTAG_TYPE);
	aos_assert_r(type != "", false);

	if (type == "stradd")
	{
		u64 executor_id = 0;
		AosIILExecutorObjPtr executor = AosBitmapEngineObj::getBitmapEngine()->getIILExecutor(rdata);
		if(executor)
		{
			executor_id = executor->getExecutorID();
		}
		else
		{
			executor_id = 0;
		}
		
		AosIILClientObj::getIILClient()->StrBatchAddMerge(
			iilname, len, executor_id, true_delete, rdata);

		if(executor)
		{
			executor->finish(rdata, 0);
			AosBitmapEngineObj::getBitmapEngine()->IILExecutorFinished(executor_id);
		}
	}
	else if (type == "strinc")
	{
		OmnString inctype = iiltag->getAttrStr(AOSTAG_INCTYPE);
		AosIILUtil::AosIILIncType inc_type = AosIILUtil::IILIncType_toEnum(inctype);
		u64 init_docid = iiltag->getAttrU64(AOSTAG_INITDOCID, 0);

		AosIILClientObj::getIILClient()->StrBatchIncMerge(
			iilname, len, init_docid, inc_type, true_delete, rdata);
	}

	AosActionObjPtr thisptr(this, true);
	task->actionFinished(thisptr, rdata);

	return true;
}


AosActionObjPtr
AosActIILBatchOprMerge::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActIILBatchOprMerge(false);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}

