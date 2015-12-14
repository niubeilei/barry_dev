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
#include "Actions/ActRebuildBitmapOpr.h"

#include "Alarm/Alarm.h"
//#include "API/AosApiG.h"
#include "API/AosApi.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/TaskDataObj.h"
#include "SEInterfaces/BitmapEngineObj.h"
#include "SEInterfaces/DataScannerObj.h"
#include "SEInterfaces/IILExecutorObj.h"
#include "SEInterfaces/IILMgrObj.h"
#include "SEInterfaces/VfsMgrObj.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/BitmapObj.h"
#include "SmartDoc/SmartDoc.h"
#include "UtilData/ModuleId.h"
#include "Util/AppendFile.h"
#include "Util/Buff.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"


AosActRebuildBitmapOpr::AosActRebuildBitmapOpr(const bool flag)
:
AosSdocAction(AOSACTTYPE_REBUILDBITMAPOPR, AosActionType::eRebuildBitmapOpr, flag)
{
}


AosActRebuildBitmapOpr::AosActRebuildBitmapOpr(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
:
AosSdocAction(AOSACTTYPE_REBUILDBITMAPOPR, AosActionType::eRebuildBitmapOpr, false)
{
}


AosActRebuildBitmapOpr::~AosActRebuildBitmapOpr()
{
	//OmnScreen << "act iil batch opr deleted" << endl;
}

bool
AosActRebuildBitmapOpr::run(
		const AosTaskObjPtr &task, 
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	if (!sdoc)
	{
		AosSetError(rdata, AosErrmsgId::eMissingSmartDoc);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	/*if (!task)
	{
		AosSetError(rdata, AosErrmsgId::eMissingSmartDoc);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}*/
	
	mRundata = rdata->clone(AosMemoryCheckerArgsBegin);
	//mTask = task;
	
	AosXmlTagPtr iil_name = sdoc->getFirstChild("iil");
	aos_assert_r(iil_name, false);
	
	mIILName = iil_name->getAttrStr(AOSTAG_IILNAME);

	bool rslt = AosIILClientObj::getIILClient()->rebuildBitmap(mIILName,mRundata);
	return rslt;
}


AosActionObjPtr
AosActRebuildBitmapOpr::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActRebuildBitmapOpr(def, rdata);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
