////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: NetEtyType.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Network_NetEtyType_h
#define Omn_Network_NetEtyType_h

#include "Util/String.h"



class OmnNetEtyType 
{
public:
	enum E 
	{
		eInvalidNetEntity,
		eNoEntityType,
		eCaEntity,
		eSpnr,
		eIpBubble,
		eAlg,
		eAlgServer,
		eConnServer,
		eServingSpnr,
		eCentralDb,
		eLAN,
		eEReg,				// For Endpoint Registration
		eAReg,				// For ALG Registration
		eInward,			// This is used by Cntler
		eOutward			// Used by Cntler
	};

	static 	E	nameToEnum(const OmnString &name);
};
#endif

