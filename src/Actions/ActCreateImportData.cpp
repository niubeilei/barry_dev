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
#include "Actions/ActCreateImportData.h"


#include "Alarm/Alarm.h"
#include "API/AosApiG.h"
#include "DataFormat/DataFormatter.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "SmartDoc/SmartDoc.h"
#include "Util/AppendFile.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"


AosActCreateImportData::AosActCreateImportData(const bool flag)
:
AosSdocAction(AOSACTTYPE_CREATEIMPORTDATA, AosActionType::eCreateImportData, flag),
mThread(0),
mConfig(0),
mRundata(OmnNew AosRundata(AosMemoryCheckerArgsBegin))
{
}


AosActCreateImportData::~AosActCreateImportData()
{
}


bool	
AosActCreateImportData::run(
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	if (!sdoc)
	{
		AosSetError(rdata, "eMissingSmartDoc");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	if (mThread)
	{
		AosSetErrorU(rdata, "internal_error");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	mConfig = sdoc;

	OmnThreadedObjPtr thisptr(this, false);
	mThread = OmnNew OmnThread(thisptr, "createimportdata", 0, false, true, __FILE__, __LINE__);
	mThread->start();
	return true;
}


bool
AosActCreateImportData::run(
		const AosTaskObjPtr &task, 
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	mTask = task;
	mTaskData = task->getTaskData();
	return run(sdoc, rdata);
}

bool	
AosActCreateImportData::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	createData(mConfig, mRundata);	
	state = OmnThrdStatus::eExit; 
	return true;
}

	
bool	
AosActCreateImportData::signal(const int threadLogicId)
{
	return true;
}


bool    
AosActCreateImportData::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}

	
bool	
AosActCreateImportData::createData(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	// This action add an attribute to a doc. 
	// 	<action 
	// 		<docid .../>
	// 		<objid .../>
	// 		<sdoc .../>
	// 	</action>
	
	if (!sdoc)
	{
		AosSetError(rdata, "eMissingSmartDoc");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	AosXmlTagPtr impcfg = sdoc->getFirstChild(AOSTAG_IMPORTDATA);
	aos_assert_rr(impcfg, rdata, false);

	OmnString filename;
	int num_files = 0;
	if(mTaskData)
	{
		if(mTaskData->getTaskDataType() != AosTaskDataType::eAuto) 
		{
			AosSetErrorU(rdata, "missing record cfg");
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
		filename = mTaskData->getFileName();
		num_files = 1;
	}
	else
	{
		OmnString dir_base = impcfg->getAttrStr("dir_base", "");
		OmnString fname_base = impcfg->getAttrStr("fname_base");
		aos_assert_rr(fname_base != "", rdata, false);
		filename << dir_base << fname_base;
	
		num_files = impcfg->getAttrInt("num_files", -1);
	}
	aos_assert_r(filename != "", false);
	aos_assert_rr(num_files > 0, rdata, false);

	u64 batch_size = impcfg->getAttrU64("batch_size", 10000);
	u64 num_docs_per_file = impcfg->getAttrU64("num_docs_per_file", 10000000);
	if(batch_size > num_docs_per_file) batch_size = num_docs_per_file;

	// This function creates a number of files. 
	// Retrieve the record
	AosXmlTagPtr record = impcfg->getFirstChild(AOSTAG_RECORD);
	if (!record)
	{
		AosSetErrorU(rdata, "missing record cfg");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	AosDataRecordObjPtr dataRecord = AosDataRecordObj::createDataRecordStatic(record, mTask, rdata.getPtr() AosMemoryCheckerArgs);
	if (!dataRecord)
	{
		AosSetErrorU(rdata, "missing record");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	u64 num_docs = 0;
	u32 start_sec = OmnGetSecond();
	u32 dur;
	OmnString fname;
	AosAppendFilePtr file;
	AosBuffPtr buff;
	for (int i=0; i<num_files; i++)
	{
		fname = filename;
		if(num_files != 1)
		{
			fname << "_" << i;
		}
		
		file = OmnNew AosAppendFile(fname, true);
		aos_assert_r(file && file->isGood(), false);
		
		for (u64 mm=0; mm<num_docs_per_file; mm+=batch_size)
		{
			//buff = OmnNew AosBuff(10000000, 10000000 AosMemoryCheckerArgs);
			//felicia, 2012/09/26
			buff = OmnNew AosBuff(10000000 AosMemoryCheckerArgs);
			if (num_docs % 100000 == 0)
			{
				dur = OmnGetSecond() - start_sec;
				if (dur > 0)
				{
					OmnScreen << "Docs Created: " << num_docs
						<< ", Time: " << dur 
						<< ", Rate: " << (num_docs / dur) << endl;
				}
			}
			for (u64 k=0; k<batch_size; k++)
			{
				dataRecord->createRandomDoc(buff, rdata.getPtr());
			}
			num_docs += batch_size;
			file->append(buff->data(), buff->dataLen(), true);
			
			int Progress = ((u64)i * 100 + mm * 100 / num_docs_per_file) / num_files;
			OmnScreen << "progress:" << Progress << endl;
		}
	}

	dur = OmnGetSecond() - start_sec;
	if (dur > 0)
	{
		OmnScreen << "Finished" << endl
			  << "     Total docs created: " << num_docs << endl
			  << "     Total Second:       " << dur << endl
			  << "     Rate:               " << (1.0 * num_docs / dur) << endl;
	}
	if(mTask)
	{
		AosActionObjPtr thisptr(this, true);
		mTask->actionFinished(thisptr, rdata);
	}

	return true;
}


AosActionObjPtr
AosActCreateImportData::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		AosActCreateImportData* act = OmnNew AosActCreateImportData(false);
		act->mRundata = rdata->clone(AosMemoryCheckerArgsBegin);
		act->mConfig = def->clone(AosMemoryCheckerArgsBegin);
		return act;
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


bool
AosActCreateImportData::checkConfig(
		const AosXmlTagPtr &sdoc,
		const AosTaskObjPtr &task,
		const AosRundataPtr &rdata)
{
	aos_assert_r(sdoc, false);
	aos_assert_r(task, false);
	
	
	AosXmlTagPtr impcfg = sdoc->getFirstChild(AOSTAG_IMPORTDATA);
	aos_assert_rr(impcfg, rdata, false);

	// This function creates a number of files. 
	// Retrieve the record
	AosXmlTagPtr record = impcfg->getFirstChild(AOSTAG_RECORD);
	if (!record)
	{
		AosSetErrorU(rdata, "missing record cfg");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	AosDataRecordObjPtr dataRecord = AosDataRecordObj::createDataRecordStatic(record, task, rdata.getPtr() AosMemoryCheckerArgs);
	if (!dataRecord)
	{
		AosSetErrorU(rdata, "missing record");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	return true;
}

