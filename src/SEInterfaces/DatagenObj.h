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
// 2014/11/05 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_DatagenObj_h
#define Aos_SEInterfaces_DatagenObj_h

#include "Jimo/Jimo.h"
#include "SEInterfaces/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Util/Ptrs.h"
#include "XmlUtil/XmlTag.h"


class AosDatagenObj : public AosJimo
{
private:

public:
	AosDatagenObj(const int version);
	~AosDatagenObj();

	virtual bool generateData(AosRundata *rdata, 
						const OmnString &data_name, 
						const vector<AosExprObjPtr> &parms) = 0;
};
#endif

