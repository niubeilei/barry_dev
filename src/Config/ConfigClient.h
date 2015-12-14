////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ConfigClient.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Config_ConfigClient_h
#define Omn_Config_ConfigClient_h

#include "Util/RCObject.h"


class OmnConfigClient : virtual public OmnRCObject
{
public:
	OmnConfigClient();
	~OmnConfigClient();
};

#endif

