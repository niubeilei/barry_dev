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
// 2013/06/02 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Interfaces_IfcExample_IfcExample_h
#define Aos_Interfaces_IfcExample_IfcExample_h

#include "Jimo/Interface.h"
#include "Jimo/MethodDef.h"
#include "Rundata/Ptrs.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"


class AosIfcExample : public AosInterface
{
public:
	AosJimoBasicFunc001		mMethod1;
	AosJimoBasicFunc002		mMethod2;

public:
	AosIfcExample();
	~AosIfcExample();
	
	virtual bool retrieveMethods(
							const AosRundataPtr &rdata, 
							const AosJimoPtr &jimo);

	virtual AosInterfacePtr clone(const AosRundataPtr &rdata) const;

private:
	void resetMethods();
};
#endif
