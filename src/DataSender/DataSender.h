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
// 06/04/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataSender_DataSender_h
#define Aos_DataSender_DataSender_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/DataSenderObj.h"
#include "SEInterfaces/DataSenderType.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"


class AosDataSender : virtual public AosDataSenderObj
{
private:

public:
	AosDataSender(
				const OmnString &name, 
				const AosDataSenderType::E type,
				const bool regflag);
	~AosDataSender();

	static bool init();
};

#endif

