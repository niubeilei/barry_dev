////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CliModuleTorturer.h
// Description:
//   
//
// Modification History:
// 10/29/2006: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Torturer_CliModuleTorturer_h
#define Omn_Torturer_CliModuleTorturer_h

#include "aosUtil/Types.h"
#include "CliTorturer/Util.h"
#include "CliTorturer/Ptrs.h"
#include "Random/RandomInteger.h"
#include "Tester/TestMgr.h"
#include "Util/ValList.h"
#include "Util/Ptrs.h"
#include "Util/DynArray.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/ValList.h"
#include "XmlParser/Ptrs.h"

class AosCliModuleTorturer : public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum
	{
		eMaxCommands = 10000,
		eDefaultGroupSize = 10, 
		eDefaultModuleWeight = 50
	};

	enum CheckType
	{
		eNoCheck,
		eCheckAfterEveryCommand,
		eCheckAfterEveryBlock
	};

private:
	OmnString				mModuleId;
	AosCliCmdTorturerPtr	mRetrieveConfigCmd;
    OmnDynArray<AosCliCmdTorturerPtr>   mCmds;
	OmnDynArray<u16>					mWeights;
	AosRandomIntegerPtr		mCmdSelector;
	AosCliTorturerPtr		mCliTorturer;
	CheckType				mCheckType;
	u32						mCmdRun;
	u32 					mCheckGroupSize;
	u32						mMinNumCmds;
	u32						mMaxNumCmds;
	u32						mTotalCmdsRun;
	u32						mTotalFailedCmds;
	u32						mTotalFailedChecks;
	u32						mWeight;
	bool					mStatus;

public:
	AosCliModuleTorturer(
			const OmnString &moduleName, 
			const u32 minNumCmds, 
			const u32 maxNumCmds, 
			const u32 weight, 
			const AosCliTorturerPtr &cliTorturer);
	virtual ~AosCliModuleTorturer();

	static AosCliModuleTorturerPtr createModule(const OmnXmlItemPtr &def, 
					const AosCliTorturerLogType logType, 
					OmnVList<AosGenTablePtr> &tables, 
					AosVarListPtr &variables,
					const AosCliTorturerPtr &cliTorturer);
	virtual bool 	addCommand(const AosCliCmdTorturerPtr &cmd);
	virtual bool	runCli(u32 &cmdsRun, const OmnTestMgrPtr &tm);
	virtual bool 	check();

	void			setCheckGroupSize(u32 s) {mCheckGroupSize = s;}
	OmnString		getModuleId() const {return mModuleId;}
	u32				getWeight() const {return mWeight;}
	void			setWeight(const u32 w) {mWeight = w;}
	
	u32				getTotalCmdsRun() const {return mTotalCmdsRun;}
	u32				getTotalFailedCmds() const {return mTotalFailedCmds;}
	u32				getTotalFailedChecks() const {return mTotalFailedChecks;}
	bool			getStatus() const {return mStatus;}
	void			setStatus(const bool s) {mStatus = s;}
	u32				getNumCmds() const;
	static bool		parseCmd(const OmnXmlItemPtr &item,
							 const AosCliTorturerLogType logType, 
							 OmnVList<AosGenTablePtr> &tables, 
							 AosVarListPtr &variables,
							 const AosCliModuleTorturerPtr &module,
							 const AosCliTorturerPtr &cliTorturer);
	
private:
	bool			createSelector();
};

#endif

