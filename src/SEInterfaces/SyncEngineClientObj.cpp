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
//
// Modification History:
// 2015/10/15 Created by Young
////////////////////////////////////////////////////////////////////////////

#include "SEInterfaces/SyncEngineClientObj.h"


AosSyncEngineClientObjPtr AosSyncEngineClientObj::smClient;

void 
AosSyncEngineClientObj::setSyncEngineClient(const AosSyncEngineClientObjPtr &d)
{
	smClient = d;
}
