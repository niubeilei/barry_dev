////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: MsgCat.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Message_MsgCat_h
#define Omn_Message_MsgCat_h



class OmnMsgCat
{
public:
	enum E
	{
		//
		// Message Category
		//
		eNetworkManagement,
		eCommMsg,
		eCallProc,
		eSO,
		eUserDefined,

		//
		// Call Processing Sub-cateogry
		//
		eMediaMsg,				// Messages related to Media
		eDeviceMsg				// Messages to and from devices
	};

	static OmnString	getName(E code);
};
#endif