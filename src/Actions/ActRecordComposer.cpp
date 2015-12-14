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
// This action modifies a record based on the configuration. 
//
// Modification History:
// 06/20/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActRecordComposer.h"

#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"


AosActRecordComposer::AosActRecordComposer(const bool flag)
:
AosSdocAction(AOSACTTYPE_RECORDCOMPOSER, AosActionType::eRecordComposer, flag)
{
}


AosActRecordComposer::AosActRecordComposer(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
:
AosSdocAction(AOSACTTYPE_RECORDCOMPOSER, AosActionType::eRecordComposer, false)
{
	if (!config(def, rdata))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosActRecordComposer::~AosActRecordComposer()
{
}


bool	
AosActRecordComposer::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


AosActionObjPtr
AosActRecordComposer::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActRecordComposer(def, rdata);
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
AosActRecordComposer::config(const AosXmlTagPtr &conf, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return true;
}


bool 
AosActRecordComposer::run(const char *data, const int len, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


