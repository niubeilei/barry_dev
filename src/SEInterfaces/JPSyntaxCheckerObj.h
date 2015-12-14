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
// 2015/08/20 Created by Arvin Jiang
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_JPSyntaxCheckerObj_h
#define Aos_SEInterfaces_JPSyntaxCheckerObj_h

#include "Jimo/Jimo.h"
#include "SEInterfaces/JimoProgObj.h"


class AosJPSyntaxCheckerObj : virtual public AosJimo
{
public:
	

	AosJPSyntaxCheckerObj(const int  version);

	virtual bool checkSyntax(AosRundata *rdata, AosJimoProgObj *prog) = 0;
};

#endif

