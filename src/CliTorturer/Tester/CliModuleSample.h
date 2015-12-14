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
// 02/13/2007 Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_CliTorturer_Tester_IacCliModuleSample_h
#define Omn_CliTorturer_Tester_IacCliModuleSample_h

#include "CliTorturer/CliModuleTorturer.h"


class AosCliModuleSample : public AosCliModuleTorturer 
{
	OmnDefineRCObject;

private:

public:
	AosCliModuleSample();
	~AosCliModuleSample() {}

	virtual bool		start();

private:
	bool	basicTest();
};

#endif

