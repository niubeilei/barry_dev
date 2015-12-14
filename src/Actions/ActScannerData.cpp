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
#include "Actions/ActScannerData.h"

/*
#include "Alarm/Alarm.h"
#include "API/AosApiG.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/DataScannerObj.h"
#include "SEInterfaces/IILMgrObj.h"
#include "SmartDoc/SmartDoc.h"
#include "Actions/ActSumUniq.h"
#include "Util/AppendFile.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"


AosActScannerData::AosActScannerData(const bool flag)
:
AosSdocAction(AOSACTTYPE_SCANNERDATA, AosActionType::eScannerData, flag)
{
}


AosActScannerData::~AosActScannerData()
{
}


bool	
AosActScannerData::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	// This action add an attribute to a doc. 
	// 	<action 
	// 		<docid .../>
	// 		<objid .../>
	// 		<sdoc .../>
	// 	</action>
	return true;
}
	
bool
AosActScannerData::run(
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

	if(!task)
	{
		AosSetError(rdata, AosErrmsgId::eMissingSmartDoc);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	mTask = task;

	AosXmlTagPtr tag = sdoc->getFirstChild("scanner");
	aos_assert_r(tag, false);
	int len = tag->getAttrInt("length", 0);
	aos_assert_r(len>0, false);
	OmnString map_objid = tag->getAttrStr("zky_map_objid", "");
	aos_assert_r(map_objid != "", false);

	OmnString file_name = tag->getAttrStr("file_name", "");
	aos_assert_r(file_name != "", false);

	int physicalid = tag->getAttrInt("zky_physicalid", 0);
	aos_assert_r(physicalid >= 0, false);

	AosXmlTagPtr datatag = sdoc->getFirstChild("datascanner");
	aos_assert_rr(datatag, rdata, false);
	AosDataScannerObjPtr scanner = 
		AosDataScannerObj::createDataScanner(datatag, rdata);
	aos_assert_r(scanner, false);
	
	//scanner->initFileScanner(physicalid, file_name, len, rdata);
	vector<AosDataScannerObjPtr> scanners;
	scanner->split(scanners, len, rdata);

	AosXmlTagPtr sumuniq = tag->getFirstChild("sumuniq");
	aos_assert_r(sumuniq, false);

	//AosActionObjPtr action = OmnNew AosActSumUniq(sumuniq, rdata);
	AosActSumUniq action(sumuniq, rdata);
	//aos_assert_r(action, false);

	AosBuffPtr buff;
	for(u32 i = 0; i < scanners.size(); i++)
	{
		bool rslt = scanners[i]->getNextBlock(buff, rdata);
		aos_assert_r(rslt ,false);
		while(buff && buff->dataLen()>0) 
		{
			char *entries = buff->data();
			int crtidx = 0;
			while (crtidx < buff->dataLen())
			{
				rslt = action.run(&entries[crtidx], len, rdata);
				aos_assert_r(rslt ,false);
				crtidx += len;
			}

			buff->reset();
			buff->setDataLen(0);
			rslt = scanners[i]->getNextBlock(buff, rdata);
			aos_assert_r(rslt ,false);
		}
	}
	
	action.finish(map_objid, rdata);
	return true;
}


AosActionObjPtr
AosActScannerData::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActScannerData(false);
	}

	catch (const OmnExcept &e)
	{
		rdata->setError() << "Failed to create action: " << e.getErrmsg();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


void 
AosActScannerData::importFinished(
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
		mTask->actionFailed(thisptr, rdata);
		return;
	}

	mTask->actionFinished(thisptr, rdata);
	return;
}
*/
