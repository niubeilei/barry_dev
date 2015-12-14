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
// 2014/08/01 Created by Ice 
////////////////////////////////////////////////////////////////////////////
#include "DataSplitter/Jimos/IILSplit.h"

#include "API/AosApi.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"
#include "SEUtil/IILName.h"
#include "VirtualFile/VirtualFile.h"
#include "XmlUtil/Ptrs.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosIILSplit_0(
		const AosRundataPtr &rdata,
		const u32 version) 
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosIILSplit(version);
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


AosIILSplit::AosIILSplit(const u32 version)
:
AosDataSplitter(AOSDATASPLITTER_IIL, version)
{
}


AosIILSplit::AosIILSplit(
		const OmnString &type, 
		const u32 version)
:
AosDataSplitter(type, version)
{
}

bool
AosIILSplit::config(
			const AosRundataPtr &rdata, 
			const AosXmlTagPtr &worker_doc)
{
	// mConfig is dataconnector's conf
	
	OmnString iilname = worker_doc->getAttrStr(AOSTAG_IILNAME, "");
	const char *pos = strstr((const char*)iilname.data(), "__$group"); 
	if (pos)
	{
		mIILName = OmnString(iilname.data(), pos-iilname.data()); 
		mConfig = worker_doc->clone(AosMemoryCheckerArgsBegin);
	}
	return true;
}


bool
AosIILSplit::config(
			const AosRundataPtr &rdata, 
			const AosXmlTagPtr &worker_doc,
			const AosXmlTagPtr &jimo_doc)
//AosDataSplitter(rdata, worker_doc, jimo_doc)
{
	return true;
}


AosIILSplit::~AosIILSplit()
{
}


bool 
AosIILSplit::split(AosRundata *rdata, vector<AosXmlTagPtr> &data_units)
{
	// each data_units conf format
	// <dataconnector type="iilsplit" ... cube_id="xxx" iilname="xxx_0">
	// 		...
	// </dataconnector>
	
	if (mIILName == "")
		return true;

	mConfig->removeNode("split", false, false);
	for (int i=0; i<AosGetNumCubes(); i++)
	{
		AosXmlTagPtr config = mConfig->clone(AosMemoryCheckerArgsBegin);
		OmnString iilname = AosIILName::composeCubeIILName(i, mIILName);
		config->setAttr(AOSTAG_IILNAME, iilname); 
		config->setAttr(AOSTAG_CUBE_ID, i); 
		data_units.push_back(config);
	}
	return true;
}


AosJimoPtr 
AosIILSplit::cloneJimo() const
{
	try
	{
		return OmnNew AosIILSplit(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating jimo" << enderr;
		return 0;
	}
}
