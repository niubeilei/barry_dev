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
#ifndef Aos_DynJimos_Commands_CommandsImpl_h
#define Aos_DynJimos_Commands_CommandsImpl_h

#include "Rundata/Ptrs.h"
#include "XmlUtil/Ptrs.h"

extern bool AosJimoCommands_CreateDoc(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker);

extern bool AosJimoCommands_ModifyDocAttr(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker);

#endif
