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

#ifndef Aos_Torturer_Module_h
#define Aos_Torturer_Module_h

#include "Torturer/Ptrs.h"
#include "Torturer/Command.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"

typedef std::vector<AosModulePtr> AosModulePtrArray;

class AosModule : virtual public OmnRCObject
{
public:
	enum
	{
		eMaxNumCmds = 100	
	};	

	std::list<std::string> mCmdNames;

protected:
	std::string			mName;
	std::string			mProductName;
	AosTorturerPtr		mTorturer;
	AosModulePtr		mParentModule;

	u32					mWeight;
	u32					mMinNumCmds;
	u32					mMaxNumCmds;
	u32					mTotalCmdsRun;
	u32					mTotalCorrectCmds;
	u32					mTotalIncorrectCmds;
	u32					mTotalIgnoreCmds;
	u32					mTotalFailedChecks;

	bool				mStatus;

public:
	AosModule();
	AosModule(const std::string &moduleName, const std::string &productName);
	virtual ~AosModule();
	virtual bool serialize(TiXmlNode& node);
	virtual bool deserialize(TiXmlNode* node);
	virtual bool writeToFile();
	virtual bool writeToFile(const std::string& configFile);
	virtual bool readFromFile();
	virtual bool readFromFile(const std::string& configFile);

	virtual bool addCommand(const std::string& cmdName);
	virtual bool delCommand(const std::string& cmdName);
	virtual void getCommands(std::list<std::string>& cmds);
	virtual bool isCmdNameExist(const std::string& cmdName);

	virtual std::string getName();
	virtual void setName(const std::string& name);
	virtual u32 getWeight();
	virtual void setWeight(const u32 weight);
	virtual u32 getMinNumCmds();
	virtual void setMinNumCmds(const u32 num);
	virtual u32 getMaxNumCmds();
	virtual void setMaxNumCmds(const u32 num);
	virtual bool getStatus();
	virtual void setStatus(const bool status);
	virtual int getNumCmds();
	virtual u32 getTotalCorrectCmds();
	virtual u32 getTotalIncorrectCmds();
	virtual u32 getTotalFailedChecks();
	virtual u32 getTotalCmdsRun();
	virtual u32 getTotalIgnoreCmds();

	//
	// select commands to run
	//
	virtual bool runCommands(u32 &cmdRuns, const OmnTestMgrPtr &tm) = 0;
	virtual bool check(){return true;};

	static bool ModuleFactory(const std::string& configFile, AosModulePtr &module);

};


#endif

