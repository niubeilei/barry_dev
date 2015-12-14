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
// 07/17/2010 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DocUtil_SengUtil_h
#define Aos_DocUtil_SengUtil_h

#include "SEUtil/IILName.h"
#include "Util/String.h"
#include "XmlUtil/XmlTag.h"

extern OmnString AosGetPrivContainerName(
		const OmnString &siteid, 
		const OmnString &ctnr_objid,
		const OmnString &username, 
		const OmnString &ctnrname);
#endif

