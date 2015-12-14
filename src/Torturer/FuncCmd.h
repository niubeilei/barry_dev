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
#ifndef Aos_Torturer_FuncCmd_h
#define Aos_Torturer_FuncCmd_h

#include "Torturer/Command.h"
#include "Tester/TestMgr.h"
#include "RVG/Ptrs.h"
#include "Util/Ptrs.h"

typedef std::vector<AosFuncCmdPtr> AosFuncCmdPtrArray;

class AosFuncCmd : public AosCommand
{
	OmnDefineRCObject;

public:

protected:
	std::string 	mFuncName;	
	std::string 	mFuncRet;			// Function return value
	std::string 	mReturnType;
	bool			mExecutionFailed;	 
	std::string 	mExecutionErrmsg;	

	//
	// The following attributes are used for code generation
	//
	std::string		mCppIncludes;
	std::string		mHeaderIncludes;
	std::string		mPreCallProcess;
	std::string		mPostCallProcess;
	std::string		mPreCheck;
	std::string		mPostCheck;
	std::string		mMemberDataDecl;
	std::string		mPreActionFunc;
	std::string		mPostActionFunc;
	std::string		mClassName;

public:
	AosFuncCmd();
	AosFuncCmd(const std::string &cmdName, 
				const std::string &productName);
	virtual ~AosFuncCmd();

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
							bool &cmdExecuted);
					
	virtual std::string getCmdPrefix() const;
	std::string 	toString();

public:
	virtual bool serialize(TiXmlNode& node);
	virtual bool deserialize(TiXmlNode* node);

	bool generateCode(const std::string& srcDir, const bool force);
	std::string getClassName();
	std::string getIncludeStmt();

private:
	bool	createCppFile(const std::string& srcDir, const bool force);
	bool	createHeaderFile(const std::string& srcDir, const bool force)const;
};

#endif

