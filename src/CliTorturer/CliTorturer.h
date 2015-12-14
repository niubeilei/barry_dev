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
// 02/11/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_CliTorturer_CliTorturer_h
#define Omn_CliTorturer_CliTorturer_h

#include "CliTorturer/Util.h"
#include "CliTorturer/Ptrs.h"
#include "Random/Ptrs.h"
#include "Tester/TestPkg.h"
#include "RVG/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/DynArray.h"
#include "Util/ValList.h"
#include "XmlParser/Ptrs.h"


extern OmnString AosCliTorturer_getVar(const OmnString &name);
extern bool      AosCliTorturer_setVar(const OmnString &name, const OmnString &value);

class AosCliTorturer : virtual public OmnRCObject,
					   public OmnTestPkg
{
	OmnDefineRCObject;

public:
	enum
	{
		eMaxModules = 1000,
		eDefaultTorturingTime = 3600
	};

	struct ModuleInfo
	{
		OmnString name;
		bool	  status;
		u32		  weight;
	};

protected:
	OmnDynArray<AosCliModuleTorturerPtr>	mModules;
	AosRandomIntegerPtr			mModuleSelector;
	OmnFilePtr					mLogFile;
	AosCliTorturerLogType		mLogType;
	OmnVList<AosGenTablePtr>	mTables;
	OmnString					mName;
	u32							mNumCmdsToRun;
	u32							mTorturingTime;
	bool						mIsGood;
	static bool					mTestOnly;
	OmnString					mInitCommands;
	OmnString					mInitVars;
	AosVarListPtr				mVariables;
	OmnDynArray<OmnXmlItemPtr>	mCmdDefs;

	OmnString					mDataDir;
	OmnString					mSrcDir;
//	OmnString					mLibName;
//	OmnString					mFuncTorturerDir;
//	OmnString					mTorturerDir;
	OmnString					mTorturerName;
	AosFuncModulePtr			mFuncModule;
	u32							mTotalCmdsCollected;
	u32							mNumModulesRun;
	u32							mTotalCmdsRun;
	OmnString					mInitIncludes;
	OmnString					mLogIncludes;
	OmnString					mInitAPIs;
	bool						mIsCodeGen;
	bool						mFlushFlag;
	AosApiCodeGenPtr			mCodeGens;
	OmnString					mLogFilename;
	static bool					mCorrectOnly;

	OmnDynArray<OmnString>	mAdditionalLibs;
	OmnDynArray<OmnString>	mCommands;
	OmnDynArray<bool>		mCommandStatus;
	OmnDynArray<ModuleInfo>	mModuleInfo;

public:
	// AosCliTorturer();
	AosCliTorturer(const u32 cmdsToRun, 
				   const u32 torturingTime, 
				   const OmnString &confFilename, 
				   const OmnString &logFilename, 
				   const AosCliTorturerLogType logType, 
				   const bool codegen, 
				   const OmnString &datadir);
	AosCliTorturer(const u32 cmdsToRun, 
				   const u32 torturingTime, 
				   const OmnString &confFilename, 
				   const OmnString &logFilename, 
				   const AosCliTorturerLogType logType, 
				   const OmnString &srcdir, 
				   const OmnString &objname,
				   const bool codegen, 
				   const OmnString &datadir);
	virtual ~AosCliTorturer();

	bool			config(const OmnString &filename, const bool codegen);
	bool			addModule(const AosCliModuleTorturerPtr &module);
	bool			start();
	bool			isGood() const {return mIsGood;}
	OmnTestMgrPtr	getTestMgr() const;
	bool			setVar(const OmnString &name, const OmnString &value);
	bool			setVar(const OmnString &name, const int value);
	bool			getVar(const OmnString &name, OmnString &value) const;
	OmnXmlItemPtr	getCmdDef(const OmnString &name) const;
	AosVarListPtr	getVariables(); 
	OmnFilePtr 		getLogFile() const;
	bool			genCode(const bool force) const;
	OmnString		getInitAPI() const {return mInitAPIs;}
	OmnString		getInitIncludes() const {return mInitIncludes;}
	bool			printTables();
	OmnString		getFuncTorturerDir() const {return mSrcDir;}
	OmnString		getLogIncludes() const;
	bool			isCodeGen() const {return mIsCodeGen;}
	bool			addCommand(const OmnString &, const bool);
	bool			addCommand(const AosCliCmdTorturerPtr &cmd);
	bool			addCommands(const OmnXmlItemPtr &);
	bool			saveCommandStatus(const OmnString &filename);
	bool 			isCommandOn(const OmnXmlItemPtr &def);
	bool 			isCommandOn(const OmnString &name);
	bool 			addModule(const OmnString &name, const bool, const u32);
	bool 			addModules(const OmnXmlItemPtr &item);
	bool 			isModuleOn(const OmnXmlItemPtr &def);
	bool 			saveModuleStatus(const OmnString &filename);
	u32				getModuleWeight(const OmnString &name);
	void			setFlushFlag(const bool b) {mFlushFlag = b;}
	bool			isFlushOn() const {return mFlushFlag;}
	OmnString		dumpVariables() const;
	OmnFilePtr		createNextLogFile();

	AosCliModuleTorturerPtr 	getModule(const OmnString &moduleId) const;
	OmnVList<AosGenTablePtr>& 	getTables();
	AosCliTorturerLogType 		getLogType() const;
	static AosCliTorturerPtr	getSelf();
	static void					setAosHome(const OmnString &home);
	static bool		getTestOnlyFlag() {return mTestOnly;}
	static void		setTestOnlyFlag(const bool b) {mTestOnly = b;}
	static int		getNextFuncIndex();
	static void		setCorrectOnly(const bool f) {mCorrectOnly = f;}
	static bool		isCorrectOnly() {return mCorrectOnly;}

private:
	bool	init(const OmnString &confFn);
	bool	createModuleSelector();
	bool	createTable(const OmnXmlItemPtr &def);
	bool 	parseTorturerInfo(const OmnXmlItemPtr &torturerInfo);
	bool 	parseModule(const OmnXmlItemPtr &moduleDef);
	bool 	parseInitAPI(const OmnXmlItemPtr &def);
	bool 	parseCmd(const OmnXmlItemPtr &def);
	bool 	parseFunc(const OmnXmlItemPtr &def);
	bool 	runClis();
	bool 	parseInstanceGen(const OmnXmlItemPtr &def);
	bool	processIncludes(const OmnString &filenames);
	bool	processOneFile(const OmnString &filename);
	bool	setDataDir();
	bool 	processSourceCode(const OmnXmlItemPtr &def);
};

#endif

