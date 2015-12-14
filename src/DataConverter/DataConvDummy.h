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
// 2013/11/30 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataConverter_DataConvDummy_h
#define Aos_DataConverter_DataConvDummy_h

#include "DataConverter/DataConverter.h"


class AosDataConvDummy : public AosDataConverter
{
	OmnDefineRCObject;

private:

public:
	AosDataConvDummy(const OmnString &version);
	~AosDataConvDummy();
};

#endif
