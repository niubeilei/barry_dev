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
//
// Modification History:
// 2013/11/24 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataSplitter/Jimos/FileSplit.h"

#include "API/AosApi.h"
#include "NetFile/NetFile.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/Ptrs.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosFileSplit_0(
		const AosRundataPtr &rdata,
		const u32 version) 
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosFileSplit(version);
		aos_assert_rr(jimo, rdata, 0);
		rdata->setJimo(jimo);
		return jimo;
	}

	catch (...)
	{
		AosSetErrorU(rdata, "Failed creating jimo") << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}


AosFileSplit::AosFileSplit(const u32 version)
:
AosDataSplitter(AOSDATASPLITTER_FILE, version)
{
}


AosFileSplit::AosFileSplit(
		const OmnString &type, 
		const u32 version)
:
AosDataSplitter(type, version)
{
}


bool
AosFileSplit::config(
			const AosRundataPtr &rdata, 
			const AosXmlTagPtr &worker_doc)
{
	aos_assert_r(worker_doc, false);

	AosXmlTagPtr split_config = worker_doc->getFirstChild("split");
	aos_assert_r(split_config, false);

	mGroupSize = split_config->getAttrInt64("group_size", 10000000);

	/*mFileInfo.mStartOffset = worker_doc->getAttrInt64(AOSTAG_START_POS, 0);

	mFileInfo.mPhysicalId = worker_doc->getAttrInt(AOSTAG_PHYSICALID, -1);
	if (!AosIsValidPhysicalIdNorm(mFileInfo.mPhysicalId))
	{
		AosSetErrorUser(rdata, "invalid_physical_id") << worker_doc->toString() << enderr;
		return false;
	}

	mFileInfo.mFileName = worker_doc->getAttrStr("file_name", "");
	if (mFileInfo.mFileName == "")
	{
		AosSetErrorUser(rdata, "missing_filename") << worker_doc->toString() << enderr;
	}
	mFileInfo.mCharset = worker_doc->getAttrStr(AOSTAG_CHARACTER, "UTF8");
	mFileInfo.mFileLen = worker_doc->getAttrInt64("file_length", -1);
	if(mFileInfo.mFileLen<=0) 
	{
		AosNameNetFile mFile(mFileInfo);
		mFileInfo.mFileLen = mFile.length(rdata.getPtr());
	}*/
	
	AosXmlTagPtr file_doc = worker_doc->getFirstChild("file");
	aos_assert_r(file_doc, false);

	mFileInfo.serializeFrom(file_doc);

	if (mFileInfo.mFileName == "")
	{
		if (mFileInfo.mFileId == 0)
		{
			AosSetErrorUser(rdata, "missing_fileidname_and_fileid") << worker_doc->toString() << enderr;
			return false;
		}
	}

	if (!AosIsValidPhysicalIdNorm(mFileInfo.mPhysicalId))
	{
		AosSetErrorUser(rdata, "invalid_physical_id") << worker_doc->toString() << enderr;
		return false;
	}

	AosNetFileObjPtr file;
	if (mFileInfo.mFileName != "")
	{
		file = OmnNew AosNameNetFile(mFileInfo);
	}
	else
	{
		file = OmnNew AosIdNetFile(mFileInfo);
	}

	if (!file)
	{
		AosSetErrorUser(rdata, "faild_create_virtual_file,") << mFileInfo.mFileName << "," << mFileInfo.mFileId << enderr;
		return false;
	}

	if (mFileInfo.mFileLen <= 0) 
	{
		mFileInfo.mFileLen = file->length(rdata.getPtr());
	}

	return true;
}


bool
AosFileSplit::config(
			const AosRundataPtr &rdata, 
			const AosXmlTagPtr &worker_doc,
			const AosXmlTagPtr &jimo_doc)
//AosDataSplitter(rdata, worker_doc, jimo_doc)
{
	return true;
}


AosFileSplit::~AosFileSplit()
{
}


bool 
AosFileSplit::split(AosRundata *rdata, vector<AosXmlTagPtr> &data_units)
{
	return true;
}


AosJimoPtr 
AosFileSplit::cloneJimo() const
{
	try
	{
		return OmnNew AosFileSplit(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating jimo" << enderr;
		return 0;
	}
}
