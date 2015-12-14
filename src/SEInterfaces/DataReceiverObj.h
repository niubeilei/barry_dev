////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2013/08/28 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_DataReceiverObj_h
#define Aos_SEInterfaces_DataReceiverObj_h

#include "Jimo/Jimo.h"

class AosDataReceiverObj : public AosJimo
{
	OmnDefineRCObject;
public:
	AosDataReceiverObj();
	AosDataReceiverObj(const int version);
	virtual ~AosDataReceiverObj();

	virtual bool dataReceiver(const AosBuffPtr &buff) = 0;
	virtual bool finishDataReceiver(const AosRundataPtr &rdata) = 0;
};

#endif

