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
// 01/03/2008: created by Allen Xu
//
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Torturer_Torturer_h
#define Aos_Torturer_Torturer_h

#include "RVG/Ptrs.h"
#include "RVG/RIG.h"
#include "RVG/Table.h"
#include "SemanticData/Ptrs.h"
#include "Torturer/Module.h"
#include "Torturer/TorUtil.h"
#include "Tester/TestPkg.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/ValList.h"
#include <vector>

class AosTorturer;
typedef std::vector<AosTorturerPtr> AosTorturerPtrArray;

class AosTorturer : virtual public OmnRCObject, public OmnTestPkg
{
public:
	enum 
	{
		eMaxModules = 1000,
		eDefaultTorturingTime = 3600
	};

protected:
	std::string			mTorturerName;
	std::string			mProductName;

	bool				mIsGood;
	AosModulePtrArray	mModules;
	u32					mNumCmdsToRun;
	u32					mTorturingTime;
	AosRIG<u32>			mModuleSelector;

	u32					mTotalCmdsCollected;
	u32					mNumModulesRun;
	u32					mTotalCmdsRun;

	std::string			mLogFilename;
	OmnFilePtr			mLogFile;

	std::list<std::string> mAdditionalLibs;
	std::list<std::string> mInitIncludes;

	//
	// Runtime torturer data, they are global data for each torturer executable
	//
	static  AosTorturerLogType::E	mLogType;
	static  std::string			mInitFuncs;
	static  std::list<std::string>	mLogIncludes;
	static	AosVarContainerPtr	mVariables;
	static	AosTablePtrArray	mTables;
	static  bool				mTestOnly;
	static  AosCorCtrl::E		mCorrectCtrl;

protected:
	virtual bool init();

public:
	AosTorturer();
	AosTorturer(const std::string& torturerName, const std::string& productName);
	virtual ~AosTorturer();
	virtual bool writeToFile();
	virtual bool readFromFile();
	virtual bool writeToFile(const std::string &configFile);
	virtual bool readFromFile(const std::string &configFile);
	//
	// start running the torturer
	// initialize data, select module and commands to run
	//
	virtual bool start();
	bool createModuleSelector();
	//
	// Select a module and run it
	//
	virtual bool run();

	virtual bool addModule(const AosModulePtr& module);
	virtual bool delModule(const AosModulePtr& module);
	void	getCommandNames(std::list<std::string>& cmdNames);

	bool	isGood() const {return mIsGood;}
	OmnFilePtr			getLogFile() const;
	OmnFilePtr 			createNextLogFile();
	OmnTestMgrPtr 		getTestMgr() const;
	AosVarContainerPtr	getVariables();
	AosValuePtr			getVar(const std::string &name) const;
	std::string			getLogFileName();
	void				setLogFileName(const std::string& filename);
	std::string			getName();
	void				setName(const std::string& name);


	static void getTables(std::vector<AosTablePtr> &tables);
	static bool getTable(const std::string &tableName, AosTablePtr& table);
	static bool addTable(const AosTablePtr& table);
	static bool delTable(const AosTablePtr& table);
	static bool printTables();
	static bool	getTestOnlyFlag(){return mTestOnly;}
	static void setCorrectCtrlFlag(AosCorCtrl::E flag);
	static AosCorCtrl::E getCorrectCtrlFlag();
	static int 	getNextFuncIndex();
	static void getLogIncludes(std::list<std::string>& includes);
	static std::string getInitFuncs();
	static AosTorturerLogType::E	getLogType();

	static bool createInstance(const std::string &configFile, AosTorturerPtr &torturer);

private:
	bool 	parseTorturerInfo(TiXmlNode* node);
	bool	parseTables(TiXmlNode* node);
	bool	parseModules(TiXmlNode* node);
	bool	parseAdditionalLibs(TiXmlNode* node);
	bool	parseInitIncludes(TiXmlNode* node);
	bool	parseLogIncludes(TiXmlNode* node);
	bool 	parseInitFuncs(TiXmlNode* node);

	bool 	addTorturerInfo(TiXmlNode& node);
	bool 	addTablesConfig(TiXmlNode& node);
	bool 	addModulesConfig(TiXmlNode& node);
	bool 	addLibsConfig(TiXmlNode& node);

};

#endif

