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
// 08/04/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Torturer_QoSTorturer_QoSTorturer_h
#define Omn_Torturer_QoSTorturer_QoSTorturer_h

#include "QoSTorturer/Ptrs.h"
#include "Torturer/Torturer.h"

class AosQoSTorturer : public AosTorturer 
{
public:
	enum Config
	{
		eNoConfig,
		eConfigOnce,
		eDynamicConfig
	};

private:
	Config				mConfigMode;
	AosQoSConfigPtr		mConfig;
	AosQoSVerifierPtr	mConfig;

public:
	AosQoSTorturer();
	virtual ~AosQoSTorturer();
};

#endif

