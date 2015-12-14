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
// 2015/11/18 Created by Barry
////////////////////////////////////////////////////////////////////////////
#include "DataReceiver/DataReceiver.h"

#include "API/AosApiG.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"

extern "C"
{
	AosJimoPtr AosCreateJimoFunc_AosDataReceiver_0(
			const AosRundataPtr &rdata, 
			const u32 version) 
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			return OmnNew AosDataReceiver(version);
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



AosDataReceiver::AosDataReceiver()
:
AosDataReceiverObj(),
mLock(OmnNew OmnMutex())
{
}


AosDataReceiver::AosDataReceiver(const u32 version)
:
AosDataReceiverObj(version),
mLock(OmnNew OmnMutex())
{
}


AosDataReceiver::~AosDataReceiver()
{
}


AosJimoPtr 
AosDataReceiver::cloneJimo()  const
{
	try
	{
		return OmnNew AosDataReceiver(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating jimo" << enderr;
	}
	return 0;
}


AosDataReceiverObjPtr
AosDataReceiver::createDataReceiver(
		const AosXmlTagPtr &conf,
		const AosRundataPtr &rdata)
{
	aos_assert_r(conf, NULL);
	OmnString type = conf->getAttrStr(AOSTAG_ZKY_TYPE, "");
	int version = 0;
	OmnString objid = type;
	objid << "_jimodoc_v" << version;
	AosXmlTagPtr jimo_doc = AosGetDocByObjid(objid, rdata);
	aos_assert_r(jimo_doc, NULL);

	AosJimoPtr jimo = AosCreateJimo(rdata, 0, jimo_doc, 0);
	aos_assert_r(jimo, NULL);
	AosDataReceiverObjPtr dataRec = dynamic_cast<AosDataReceiver*>(jimo.getPtr());
	aos_assert_r(dataRec, NULL);
	return dataRec;
}


bool
AosDataReceiver::dataReceiver(
		const AosBuffPtr &buff)
{
	mLock->lock();
	mBuffs.push(buff);
	mLock->unlock();
	return true;
}


bool
AosDataReceiver::finishDataReceiver(
		const AosRundataPtr &rdata)
{
	return true;
}
