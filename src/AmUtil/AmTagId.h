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
// 3/30/2007: Created by cding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_MsgUtil_AmTagId_h
#define Aos_MsgUtil_AmTagId_h

class AosAmTagId
{

public:
	enum E
	{
		// the data length must be defined well, otherwise may make mistake
		eApp, 			// str
		eRsltCode, 		// u16
		eUser, 			// str
		eUid, 			// u32
		eOpr, 			// u16
		eRsc,			// str
		ePasswd,		// str
		eErrmsg, 		// str
		eDenyReason,	// str
		eIp, 			// u32
		eIpv6, 			// u128
		ePort,			// u16

		eSIp, 			// u32
		eSIpv6, 		// u128
		eSPort,			// u16

		eDIp, 			// u32
		eDIpv6, 		// u128
		eDPort,			// u16
		eProto,			// u8

		eCacheRcd,		// str
		eCacheFlag,		// u16 	0 means: end; 1 means: not finished;

		eMaxTagId			// don't support bigger than 256
	};
};
//	// Define the Tag Type Ids
//	enum 
//	{
//		eAmTagUsrId = 1,
//		eAmTagPsswd,
//		eAmTagClientLocat,
//		eAmTagClientPort,
//		eAmTagServerLocat,
//		eAmTagServerPort,
//		eAmTagMediaTypeId,
//		eAmTagRoleId,
//		eAmTagGroupId,
//	};

#endif

