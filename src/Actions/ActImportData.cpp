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
#if 0
#include "Actions/ActImportData.h"


#include "Alarm/Alarm.h"
#include "API/AosApiG.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "ImportData/ImportDataCtlr.h"
#include "ImportData/ImportListener.h"
#include "ImportData/Ptrs.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/Ptrs.h"
#include "SmartDoc/SmartDoc.h"
#include "Util/AppendFile.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/TaskDataObj.h"


AosActImportData::AosActImportData(const bool flag)
:
AosSdocAction(AOSACTTYPE_IMPORTDATA, AosActionType::eImportData, flag)
{
}


AosActImportData::~AosActImportData()
{
	//OmnScreen << "action import deleted" << endl;
}


bool
AosActImportData::run(
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

	if(!task || !task->getTaskData())
	{
		AosSetError(rdata, "eMissingSmartDoc");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	mTask = task;

	AosXmlTagPtr impcfg = sdoc->getFirstChild(AOSTAG_IMPORTDATA);
	aos_assert_rr(impcfg, rdata, false);
	
	// Chen Ding, 2013/12/14
	// AosImportListenerPtr thisptr(this, true);
	AosImportListenerPtr thisptr(this, false);
	AosImportDataCtlrPtr impCtlr = AosImportDataCtlr::createImportDataCtlrStatic(
			impcfg, thisptr, task, rdata);
	if(!impCtlr)
	{
		importFinished(false, 0, rdata);
		return false;
	}

	bool rslt = impCtlr->setFileInfo(task->getTaskData(), rdata);
	if(!rslt)
	{
		importFinished(false, 0, rdata);
		return false;
	}

	rslt = impCtlr->start(rdata);
	if(!rslt)
	{
		importFinished(false, 0, rdata);
		return false;
	}
	
	return true;
}


bool
AosActImportData::checkConfig(
		const AosXmlTagPtr &def,
		const AosTaskObjPtr &task,
		const AosRundataPtr &rdata)
{
	aos_assert_r(def, false);

	AosXmlTagPtr impcfg = def->getFirstChild(AOSTAG_IMPORTDATA);
	aos_assert_r(impcfg, false);
	
	bool rslt = AosImportDataCtlr::checkConfigStatic(impcfg, task, rdata);
	aos_assert_r(rslt, false);
	
	return true;
}


AosActionObjPtr
AosActImportData::clone(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata) const
{
	aos_assert_r(def, 0);

	try
	{
		return OmnNew AosActImportData(false);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


void 
AosActImportData::importFinished(
		const bool rslt, 
		const AosXmlTagPtr &status_doc,
		const AosRundataPtr &rdata)
{
	OmnScreen << "import finished: " << rslt << endl;
	if (!mTask)
	{
		OmnScreen << "Import data finished: " << rslt << endl;
		return;
	}

	AosActionObjPtr thisptr(this, true);
	if (!rslt)
	{
OmnScreen << "================importdata failed: " << endl;
		actionFailed(rdata);
	}
	else
	{
		actionFinished(rdata);
	}
	mTask = 0;
}
#endif
