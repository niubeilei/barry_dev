////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AppType.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_NMS_Apptype_h
#define Omn_NMS_AppType_h


class OmnAppType 
{
public:
	enum E
	{
		eUnknown,
		eMediaRouter,		// Chen Ding, 09/08/2003
		eAlg,
		eCentralDb,
		eAlarmServer,
		eMonitor,
		eCntler
	};
};
#endif

