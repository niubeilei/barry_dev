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
// 2015/03/06 Created by Jackie 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_HadoopUtil_HadoopSingletonDummy_h
#define Aos_HadoopUtil_HadoopSingletonDummy_h

#include "HadoopUtil/HadoopSingleton.h"


class AosHadoopSingletonDummy : public HadoopSingleton
{

public:
	AosHadoopSingletonDummy();
	~AosHadoopSingletonDummy();
};
#endif



