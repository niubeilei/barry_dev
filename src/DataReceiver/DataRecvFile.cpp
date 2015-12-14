////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2013/08/28 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "DataReceiver/DataRecvFile.h"

#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "Util/Buff.h"


extern "C"
{
AosJimoPtr AosCreateJimoFunc_AosDataRecvFile_ver_0(
		const AosRundataPtr &rdata, 
		const u32 version) 
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		return OmnNew AosDataRecvFile(version);
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



AosDataRecvFile::AosDataRecvFile()
:
AosDataReceiverObj()
{
}


AosDataRecvFile::AosDataRecvFile(const u32 version)
:
AosDataReceiverObj(version)
{
}


AosDataRecvFile::~AosDataRecvFile()
{
}


bool 
AosDataRecvFile::run(const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


AosJimoPtr 
AosDataRecvFile::cloneJimo()  const
{
	try
	{
		return OmnNew AosDataRecvFile(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating jimo" << enderr;
	}
	return 0;
}


bool 
AosDataRecvFile::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)
{
	OmnNotImplementedYet;
	return false;
}


void * 
AosDataRecvFile::getMethod(
		const AosRundataPtr &rdata, 
		const OmnString &name, 
		AosMethodId::E &method_id)
{
	OmnNotImplementedYet;
	return false;
}

#endif
