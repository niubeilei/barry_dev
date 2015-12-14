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
// 2014/11/19 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataAnalyzer_DataAnalyzer_h
#define Aos_DataAnalyzer_DataAnalyzer_h

#include "SEInterfaces/DataAnalyzerObj.h"


class AosDataAnalyzer : public AosDataAnalyzerObj
{
public:
	AosDataAnalyzer(const int version);
};
#endif

