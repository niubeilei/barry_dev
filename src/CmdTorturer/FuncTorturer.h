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
// 11/24/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_CmdTorturer_FuncTorturer_h
#define Aos_CmdTorturer_FuncTorturer_h

#include "CmdTorturer/CmdTorturer.h"
#include "Tester/TestMgr.h"
#include "RVG/Ptrs.h"
#include "Util/Ptrs.h"


class AosFuncTorturer : public AosCmdTorturer
{
	OmnDefineRCObject;

public:

protected:
	std::string 	mFuncName;
	std::string 	mFuncRet;			// Function return value
	bool			mExecutionFailed;	// 
	std::string 	mExecutionErrmsg;	

public:
	AosFuncTorturer(const AosTorturerPtr &torturer);
	virtual ~AosFuncTorturer();

	virtual bool	run(bool &cmdGood, 
						const bool correctOnly,
						const OmnTestMgrPtr &tm, 
						bool &ignore);
	virtual std::string getCmdPrefix() const;
	virtual bool 	check();
	virtual bool	runFunc(const bool correctOnly, 
							const AosTorturerPtr &torturer,
							const OmnTestMgrPtr &testMgr, 
							bool &cmdCorrect, 
							std::string &errmsg, 
							bool &cmdExecuted) = 0;
	bool 			constructCmd(
						  const u32 argIndex, 
						  std::string &decl, 
						  std::string &deleteStmt,
						  std::string &cmd);
	virtual void 	finishUpCmd(const u32 cmdId, 
							const std::string &decl, 
							const std::string &deleteStmt,
							std::string &cmd);
					
	std::string 	toString();

public:
	virtual bool serialize(TiXmlNode& node);
	virtual bool deserialize(TiXmlNode* node);
};

#endif

