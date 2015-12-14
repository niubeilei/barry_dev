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
// 2013/06/18 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DynJimos/Commands/CommandsImpl.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"

bool AosJimoCommands_CreateDoc(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker)
{
	// This command creates a new doc. This should be handled by the
	// Create Doc Action.
	AosXmlTagPtr doc = AosCreateDocByWorker(rdata, worker);
	if (doc) return true;
	return false;
}


bool AosJimoCommands_ModifyDocAttr(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker)
{
	// This command creates a new doc. This should be handled by the
	// Create Doc Action.
	return AosModifyDocAttrByWorker(rdata, worker);
}

