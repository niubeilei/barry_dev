////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SCATorturer.h
// Description:
//   
//
// Modification History:
// 	11/16/2006	Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_SystemCli_SCATester_SCATorturer_h
#define Omn_SystemCli_SCATester_SCATorturer_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"
#include "Util/String.h"
#include "UtilComm/Ptrs.h"


class AosSCATorturer : public OmnTestPkg
{
private:

	enum CliId
	{
		eSetStatus = 1,
		eShowStatus = 2, 
		eAddCommand = 3, 
		eDelCommand = 4,
		eResetCommand = 5,
		eShowcommand = 6,
		eSetPolicy = 7
	};
	
	enum
	{
		macCmdNUM = 100,
	
		eRunTimes = 1000,
		eMaxCmd = 512
	};
	
	OmnString cmd_correct[128];
	OmnString cmd_incorrect[128];
	OmnString cmd_exist[eMaxCmd];
	int mNumCmd;
	

public:
	AosSCATorturer();
	~AosSCATorturer(){}
	virtual bool		start();
	
private:
	bool do_init();
	bool basicTest();
	bool runSetStatus();
	bool runShowStatus();
	bool runAddCommand();  
	bool runDelCommand();	
	bool runResetCommand();
	bool runSetPolicy(); 
	bool runShowCommand();
	
    bool genPolicy(OmnString &policy);
    bool genCommand(OmnString &command);    
    bool genCmdIncorrect(OmnString &command);
    bool addCmd(const OmnString &command);
                
};

#endif

