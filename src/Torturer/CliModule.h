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
// Friday, January 04, 2008: Created by Allen Xu 
//
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_Torturer_CliModule_h
#define Aos_Torturer_ClicModule_h

#include "Torturer/Module.h"
#include "Torturer/CliCmd.h"

class AosCliModule : public AosModule 
{
	OmnDefineRCObject;
public:
	enum
	{
		eMaxNumClis = 100	
	};	

protected:
	AosCliCmdPtrArray	mClis;

public:
	AosCliModule();
	virtual ~AosCliModule();
	virtual bool serialize(TiXmlNode& node);
	virtual bool deserialize(TiXmlNode* node);

	bool addCli(const AosCliCmdPtr& cli);
	bool delCli(const AosCliCmdPtr& cli);

	virtual bool runCommands(u32 &cmdRuns, const OmnTestMgrPtr &tm);

};


#endif

