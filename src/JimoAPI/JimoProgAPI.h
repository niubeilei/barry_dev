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
// 2015/04/01 Created by Xia Fan
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoAPI_JimoProgAPI_h
#define Aos_JimoAPI_JimoProgAPI_h

#include "SEInterfaces/JimoProgObj.h"

namespace Jimo
{
	extern AosJimoProgObjPtr jimoGetJimoProg(AosRundata *rdata, const OmnString &name);
	extern AosJimoProgObjPtr jimoRetrieveJimoProg(AosRundata *rdata, const OmnString &name);

	extern AosJimoProgObjPtr jimoCreateJimoProg(
								AosRundata *rdata, 
								const OmnString &classname, 
								const int version);

	extern AosJimoProgObjPtr jimoCreateLoadDataProg(
								AosRundata *rdata,
								const AosXmlTagPtr &inputds, 
								const AosXmlTagPtr &tabledoc,
								std::list<string> &fields,
								JQLTypes::OpType &op);
};
#endif


