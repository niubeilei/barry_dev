////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: VersionInfo.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "NMS/VersionInfo.h"

#include "Porting/GetTime.h"

OmnString	OmnVersionInfo::mVersion = "2.0.8";
OmnString	OmnVersionInfo::mPatch = "1";
int			OmnVersionInfo::mBuildNo = 75;
OmnString	OmnVersionInfo::mBuildTime = "09/15/2003";
OmnString	OmnVersionInfo::mDefaultHostName = "sr.packetlink.com";

