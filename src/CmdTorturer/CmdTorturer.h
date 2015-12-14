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
// 12/28/2007: Modified by Allen Xu 
//
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_CmdTorturer_CmdTorturer_h
#define Aos_CmdTorturer_CmdTorturer_h

#include "Action/ActionUtil.h"
#include "Action/Ptrs.h"
#include "aosUtil/Types.h"
#include "CmdTorturer/Ptrs.h"
#include "Tester/TestPkg.h"
#include "Tester/TestMgr.h"
#include "RVG/Util.h"
#include "RVG/Parm.h"
#include "RVG/Ptrs.h"
#include "SemanticRules/SemanticRule.h"
#include "SemanticRules/Ptrs.h"
#include "SemanticData/Ptrs.h"
#include "Torturer/Ptrs.h"
#include "Torturer/TorUtil.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Conditions/Ptrs.h"


class AosCmdTorturer : virtual public OmnRCObject, 
					   public OmnTestPkg
{
public:
	enum
	{
		eMaxArgs = 100,
		eDefaultCorrectPct = 80,
		eDefaultCmdWeight = 50
	};

	typedef std::vector<AosParmPtr> AosParmPtrArray; 

protected:
	static OmnFilePtr		mLogFile;

	bool					mStatus;
	u32						mId;
	AosTorturerLogType		mLogType;
	AosParmPtrArray			mArgs;
	std::string 			mCmdName;
	u32						mCorrectPct;
	u32						mWeight;
	bool					mCorrectPctSet;
	AosTorturerPtr			mTorturer;
	std::string 			mExecutionErrmsg;
	AosSdContainerPtr		mSemanticData;
	AosSdContainerPtr		mActionData;

	//
	// The following features are brought in by the relationship of Parameters
	// 
	AosRVGMultiFromTablePtr	mCorrectMultiFromTable;
	AosRVGMultiFromTablePtr	mIncorrectMultiFromTable;

	//
	// These conditions are used to check whether the parameter level generated values
	// satisfy all the conditions in the meantime.
	//
	AosConditionList		mCorrectConditions;
	AosConditionList		mIncorrectConditions;

	// 
	// Statistics
	//
	u32							mNumCorrect;
	u32							mNumIncorrect;
	u32							mNumFailed;
	u32							mNumExecuted;

	AosRuleList					mPreSemanticRules;
	AosRuleList					mPostSemanticRules;
	AosActionList				mPreCorrectActions;
	AosActionList				mPreIncorrectActions;
	AosActionList				mPostCorrectActions;
	AosActionList				mPostIncorrectActions;

private:
	bool operator ==(const AosCmdTorturer& another) const;

public:
	AosCmdTorturer(const AosTorturerPtr &torturer);
	virtual ~AosCmdTorturer();

	virtual bool serialize(TiXmlNode& node);
	virtual bool deserialize(TiXmlNode* node);

	virtual std::string getCmdPrefix() const = 0;
	virtual bool 		constructCmd(
							const u32 argIndex,
							std::string &decl,
							std::string &deleteStmt,
							std::string &cmd) = 0;
	virtual void 		finishUpCmd(const u32 cmdId, 
							const std::string &decl,
							const std::string &deleteStmt,
							std::string &cmd) = 0;
	virtual int			doCorrectPostProc(std::string &errmsg);
	virtual int			doIncorrectPostProc(std::string &errmsg);
	virtual int			doPreSemantics(std::string &errmsg);
	virtual int			doPostSemantics(std::string &errmsg);
	virtual int			doCorrectPreActions(std::string &errmsg);
	virtual int			doIncorrectPreActions(std::string &errmsg);
	virtual int			doCorrectPostActions(std::string &errmsg);
	virtual int			doIncorrectPostActions(std::string &errmsg);
	virtual int 		doPreExecuteProc(const bool cmdCorrect, 
							std::string &errmsg);

	virtual bool		runCommand(AosCorCtrl::E, 
							const AosTorturerPtr &,
							const OmnTestMgrPtr &,
							bool &cmdCorrect,
							std::string &cmdErrmsg, 
							bool &cmdExecuted) = 0;


	virtual bool	run(bool &cmdGood, 
						const AosCorCtrl::E correctFlag,
						const OmnTestMgrPtr &tm); 
	virtual bool 	check() = 0;
	virtual bool	preCheck(bool &cmdCorrect);
	virtual bool	postCheck(bool &cmdCorrect);
	virtual void    logCmd(const std::string &cmd, 
						   const bool cmdCorrect,
						   const std::string &cmdErrmsg, 
						   const u32 cmdId);
	virtual bool	addArg(const AosParmPtr &arg);

	static u32		calculateParmCrtPct(const u32 cmdGoodPct, const u32);
	static bool		setLogfile(const OmnFilePtr &file);
	static bool 	cmdFailedStat(const u32 cmdId);
	static bool 	cmdExecutedStat(const u32 cmdId);

	std::string getName() const {return mCmdName;}	
	bool 		setParmCorrectPct();
	bool		getStatus() const {return mStatus;}
	void		setStatus(const bool s) {mStatus= s;}
	u32			getWeight() const {return mWeight;}
	std::string getVar(const std::string &name);
	AosParmPtr	getParm(const std::string &name);
	bool		setCrtValue(const std::string &name, 
						const std::string &value, 
						std::string &errmsg);

	bool 		getCrtValue(const std::string &parmName, 
					std::string &value);

	static bool 		init();

private:
	bool		isObjectTableEmpty() const;
	AosCorCtrl::E 	calculateCorrectCtrl(const AosCorCtrl::E c);
	bool		selectObject(const bool correctFlag, 
							 AosRVGMultiFromTablePtr &multiFromTable,
							 bool &selectFromObj, 
							 bool &cmdCorrect); 
	bool		selectFromTable(const AosRVGMultiFromTablePtr &table, 
								bool &selectFromObj);

};

#endif

