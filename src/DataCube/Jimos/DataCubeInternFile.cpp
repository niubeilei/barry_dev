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
// An Internal File is a file stored in the storage engine. 
//
// Modification History:
// 2013/12/25 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "DataCube/Jimos/DataCubeInternFile.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/CodeConvertion.h"
extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosDataCubeInternFile_0(
		const AosRundataPtr &rdata,
		const OmnString &version) 
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosDataCubeInternFile(version);
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


AosDataCubeInternFile::AosDataCubeInternFile(const OmnString &version)
:
AosDataCube(AOS_DATACUBETYPE_INTERN_FILE, version)
{
}


AosDataCubeInternFile::~AosDataCubeInternFile()
{
}


bool
AosDataCubeInternFile::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc)
{
	return true;
}


int 
AosDataCubeInternFile::getPhysicalId() const
{
	return mPhysicalId;
}


int64_t
AosDataCubeInternFile::getFileLength() const
{
	return mFileCubeRaw->getFileLength();
}


OmnString
AosDataCubeInternFile::getFileName() const
{
	return mFileName;
}


AosDataCubeObjPtr 
AosDataCubeInternFile::cloneDataCube()
{
	return OmnNew AosDataCubeInternFile(*this);
}


bool 
AosDataCubeInternFile::readBlock(
		const AosRundataPtr &rdata, 
		AosBuffDataPtr &buff_data)
{
	return mFileCubeRaw->readBlock(rdata, buff_data);
}


bool 
AosDataCubeInternFile::appendBlock(
		const AosRundataPtr &rdata, 
		AosBuffDataPtr &buff_data)
{
	OmnShouldNeverComeHere;
	return false;
}


bool 
AosDataCubeInternFile::writeBlock(
		const AosRundataPtr &rdata, 
		const int64_t pos,
		AosBuffDataPtr &buff_data)
{
	OmnShouldNeverComeHere;
	return false;
}


bool 
AosDataCubeInternFile::copyData(
		const AosRundataPtr &rdata, 
		const OmnString &from_name,
		const OmnString &to_name)
{
	OmnShouldNeverComeHere;
	return false;
}


bool 
AosDataCubeInternFile::removeData(
		const AosRundataPtr &rdata, 
		const OmnString &name)
{
	OmnShouldNeverComeHere;
	return false;
}


bool
AosDataCubeInternFile::serializeTo(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosDataCubeInternFile::serializeFrom(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff)
{
	OmnNotImplementedYet;
	return false;
}


AosJimoPtr 
AosDataCubeInternFile::cloneJimo() const
{
	try
	{
		return OmnNew AosDataCubeInternFile(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating jimo" << enderr;
		return 0;
	}
}
#endif
