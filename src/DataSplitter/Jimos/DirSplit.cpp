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
#include "DataSplitter/Jimos/DirSplit.h"

#include "API/AosApi.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/Ptrs.h"
#include "DataCube/DataCube.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosDirSplit_0(
		const AosRundataPtr &rdata,
		const u32 version) 
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosDirSplit(version);
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


AosDirSplit::AosDirSplit(const u32 version)
:
AosDataSplitter(AOSDATASPLITTER_DIR, version)
{
}


AosDirSplit::AosDirSplit(const OmnString &type, const u32 version)
:
AosDataSplitter(type, version)
{
}


AosDirSplit::~AosDirSplit()
{
}


bool
AosDirSplit::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)
{
	aos_assert_r(worker_doc, false);

	mReadBlockSize = worker_doc->getAttrInt64("read_block_size", AosDataCube::eDftReadBlockSize);
	if (mReadBlockSize <= 0) mReadBlockSize = AosDataCube::eDftReadBlockSize;
	mCharset = worker_doc->getAttrStr(AOSTAG_CHARACTER, "UTF8");
	mIsSkipFirstLine = worker_doc->getAttrBool("skip_first_line", false);

	return true;
}


bool
AosDirSplit::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc)
{
	OmnShouldNeverComeHere;
	return true;
}

bool 
AosDirSplit::split(
		AosRundata *rdata, 
		vector<AosXmlTagPtr> &data_units)
{
	OmnShouldNeverComeHere;
	return true;
}

AosJimoPtr 
AosDirSplit::cloneJimo() const
{
	try
	{
		return OmnNew AosDirSplit(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating jimo" << enderr;
		return 0;
	}
}
