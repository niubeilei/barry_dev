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
// 01/04/2008: Created by Allen Xu
//
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Torturer_CliCmd_h
#define Aos_Torturer_CliCmd_h

#include "Torturer/Command.h"
#include "Tester/TestMgr.h"
#include "RVG/Ptrs.h"
#include "Util/Ptrs.h"

typedef std::vector<AosCliCmdPtr> AosCliCmdPtrArray;

class AosCliCmd : public AosCommand
{
	OmnDefineRCObject;

protected:
	std::string 	mCliName;
	bool			mExecutionFailed;	// 
	std::string 	mExecutionErrmsg;	

public:
	AosCliCmd();
	virtual ~AosCliCmd();

	virtual bool	run(bool &cmdGood, 
						const bool correctOnly,
						const OmnTestMgrPtr &tm, 
						bool &ignore){return false;};
	virtual bool 	check();
	bool 			constructCmd(
						  const u32 argIndex, 
						  std::string &decl, 
						  std::string &deleteStmt,
						  std::string &cmd);
	virtual void 	finishUpCmd(const u32 cmdId, 
							const std::string &decl, 
							const std::string &deleteStmt,
							std::string &cmd);
	virtual bool	runCommand(AosCorCtrl::E, 
							const AosTorturerPtr &,
							const OmnTestMgrPtr &,
							bool &cmdCorrect,
							std::string &cmdErrmsg, 
							bool &cmdExecuted){return false;};
					
	virtual std::string getCmdPrefix() const;
	std::string 	toString();

public:
	virtual bool serialize(TiXmlNode& node);
	virtual bool deserialize(TiXmlNode* node);
	static bool createInstance(std::string configFile, AosCliCmdPtr& cli);
};

#endif

