////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: NetEtyType.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Network/NetEtyType.h"


OmnNetEtyType::E	
OmnNetEtyType::nameToEnum(const OmnString &name)
{
	OmnString n = name;
	n.toLower();

	if (n == OmnString("ca"))
	{
		return eCaEntity;
	}

	if (n == OmnString("spnr") || n == OmnString("streamrouter"))
	{
		return eSpnr;
	}

	if (n == OmnString("ipbubble"))
	{
		return eIpBubble;
	}

	if (n == OmnString("lan"))
	{
		return eLAN;
	}

	if (n == OmnString("ereg"))
	{
		return eEReg;
	}

	if (n == OmnString("areg"))
	{
		return eAReg;
	}

	if (n == OmnString("alg"))
	{
		return eAlg;
	}

	if (n == OmnString("connserver"))
	{
		return eConnServer;
	}

	if (n == OmnString("algserver"))
	{
		return eAlgServer;
	}

	if (n == OmnString("centraldb"))
	{
		return eCentralDb;
	}

	if (n == OmnString("inward"))
	{
		return eInward;
	}

	if (n == OmnString("outward"))
	{
		return eOutward;
	}
	return eInvalidNetEntity;
}

