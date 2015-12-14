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
// 2014/01/30 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_QueryFuncs_QueryFunc_h
#define Aos_QueryFuncs_QueryFunc_h

#include "SEInterfaces/QueryFuncObj.h"
#include "Util/String.h"


class AosQueryFunc : public AosQueryFuncObj
{
protected:
	OmnString		mType;

public:
	AosQueryFunc(const OmnString &type, const int version);
	~AosQueryFunc();

	virtual bool pickJimo(const AosRundataPtr &rdata, 
						const AosXmlTagPtr &def, 
						AosValueRslt &value);
};
#endif



