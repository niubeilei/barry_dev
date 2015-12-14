////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: IpBubbleMgr.h
// Description:
//	The class that manages all IP bubbles for an SPNR.    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Network_IpBubbleMgr_h
#define Omn_Network_IpBubbleMgr_h

#include "Util/Obj.h"



class OmnIpBubbleMgr OmnDeriveFromObj
{
private:
	OmnValList<OmnIpBubblePtr>		mBubbles;

public:
	OmnIpBubbleMgr(const OmnString &config);
	~OmnIpBubbleMgr();

	bool		addIpBubble(const OmnIpBubblePtr &bubble);
	bool		removeIpBubble(const OmnIpBubblePtr &bubble);
};
#endif
