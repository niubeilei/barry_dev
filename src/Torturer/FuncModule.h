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

#ifndef Aos_Torturer_FuncModule_h
#define Aos_Torturer_FuncModule_h

#include "Torturer/Module.h"
#include "Torturer/FuncCmd.h"
#include "RVG/RIG.h"

class AosFuncModule : public AosModule 
{
	OmnDefineRCObject;
public:
	enum
	{
		eMaxNumFuncs = 100	
	};	

protected:
	//
	// Used in torturer running time, the real type pointed to is 
	// the subclass of the AosFuncCmd. All the subclasses are created
   	// in code generation. it will be created in the torturer running 	
	// time after the code is generated and compiled over.
	//
	AosFuncCmdPtrArray	mRunTimeFuncs;

	AosU32RIGPtr		mFuncSelector;

public:
	AosFuncModule();
	AosFuncModule(const std::string &moduleName, const std::string &productName);
	virtual ~AosFuncModule();
	virtual bool serialize(TiXmlNode& node);
	virtual bool deserialize(TiXmlNode* node);

	bool 	addRunTimeFunc(const AosFuncCmdPtr& func);
	bool 	delRunTimeFunc(const AosFuncCmdPtr& func);

	virtual bool runCommands(u32 &cmdRuns, const OmnTestMgrPtr &tm);

protected:
	virtual bool createFuncSelector();
};


#endif

