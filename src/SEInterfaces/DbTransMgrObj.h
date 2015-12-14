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
// 2014/10/27 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_DbTransMgrObj_h
#define Aos_SEInterfaces_DbTransMgrObj_h

#include "Jimo/Jimo.h"
#include "SEInterfaces/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Util/Ptrs.h"
#include "XmlUtil/XmlTag.h"


class AosDbTransMgrObj : public AosJimo
{
private:

public:
	AosDbTransMgrObj(const int version);
	~AosDbTransMgrObj();

	virtual AosDbTransObjPtr createDbTrans(AosRundata *rdata) = 0;
};
#endif

