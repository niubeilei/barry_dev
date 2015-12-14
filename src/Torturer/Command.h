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
// 1/4/2008: Created by Allen Xu 
//
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_Torturer_Command_h
#define Aos_Torturer_Command_h

#include "Action/ActionUtil.h"
#include "Action/Ptrs.h"
#include "aosUtil/Types.h"
#include "Torturer/Ptrs.h"
#include "Tester/TestPkg.h"
#include "Tester/TestMgr.h"
#include "RVG/Util.h"
#include "RVG/Parm.h"
#include "RVG/Ptrs.h"
#include "SemanticRules/Ptrs.h"
#include "SemanticData/Ptrs.h"
#include "Torturer/Ptrs.h"
#include "Torturer/TorUtil.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Conditions/Ptrs.h"

class AosRecord;
typedef std::vector<AosCommandPtr> AosCommandPtrArray;

class AosCommand : virtual public OmnRCObject, 
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
	AosTorturerLogType::E	mLogType;
	AosParmPtrArray			mArgs;
	std::string 			mCmdName;
	std::string 			mProductName;
	u32						mCorrectPct;
	bool					mCorrectPctSet;
	u32						mWeight;
	AosTorturerPtr			mTorturer;
	std::string 			mExecutionErrmsg;
	AosVarContainerPtr		mSemanticData;
	AosVarContainerPtr		mActionData;
	AosModulePtr			mParentModule;

	//
	// The following features are brought in by the relationship 
	// of Parameters
	//
	// This RVG is used to generate incorrect or correct values 
	// in the command level, select a record from the table, some 
	// of the parameters get value from the record.
	//
	AosTablePtr		mCorrectTable;
	AosTablePtr		mIncorrectTable;

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

	std::list<AosSemanticRulePtr>	mPreSemanticRules;
	std::list<AosSemanticRulePtr>	mPostSemanticRules;
	AosActionList				mPreCorrectActions;
	AosActionList				mPreIncorrectActions;
	AosActionList				mPostCorrectActions;
	AosActionList				mPostIncorrectActions;

private:
	bool operator ==(const AosCommand& another) const;

public:
	AosCommand();
	AosCommand(const std::string &cmdName, const std::string &productName);
	virtual ~AosCommand();

	virtual bool serialize(TiXmlNode& node);
	virtual bool deserialize(TiXmlNode* node);
	virtual bool readFromFile();
	virtual bool writeToFile();
	virtual bool readFromFile(const std::string& configFile);
	virtual bool writeToFile(const std::string& configFile);

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
	virtual bool 	doPreSemantics(
						bool &cmdCorrect, 
						std::string &theErrmsg);
	virtual int		doPostSemantics(
						bool &cmdCorrect, 
						std::string &errmsg);
	virtual bool	doCorrectPreActions();
	virtual bool	doIncorrectPreActions();
	virtual bool	doCorrectPostActions();
	virtual bool	doIncorrectPostActions();
	virtual bool	runCommand(AosCorCtrl::E, 
							const AosTorturerPtr &,
							const OmnTestMgrPtr &,
							bool &cmdCorrect,
							std::string &cmdErrmsg, 
							bool &cmdExecuted) = 0;


	virtual bool 	run(bool &isGeneratedCmdCorrect, 
							const AosCorCtrl::E expectedCorrectness,
							bool &cmdGeneratedFlag);
	virtual bool 	check() = 0;
	virtual bool	preCheck(bool &cmdCorrect){return true;};
	virtual bool	postCheck(bool &cmdCorrect){return true;};
	virtual void    logCmd(const std::string &cmd, 
						   const bool cmdCorrect,
						   const std::string &cmdErrmsg, 
						   const u32 cmdId);
	virtual bool	addArg(const AosParmPtr &arg);

	static int		calculateParmCrtPct(const int cmdGoodPct, const int);
	static bool		setLogfile(const OmnFilePtr &file);
	static bool 	cmdFailedStat(const u32 cmdId){return true;};
	static bool 	cmdExecutedStat(const u32 cmdId){return true;};

	std::string getName() const;
	void setName(const std::string& name);
	std::string getProductName() const;
	void setProductName(const std::string& name);
	bool 		setParmCorrectPct();
	bool		getStatus() const {return mStatus;}
	void		setStatus(const bool s) {mStatus= s;}
	u32			getWeight() const {return mWeight;}
	AosVarPtr	getVar(const std::string &name);
	AosValuePtr	getValue(const std::string &name);
	AosTablePtr	getTable(const std::string &name);
	AosParmPtr	getParm(const std::string &name);
	bool		setCrtValue(const std::string &name, 
						const std::string &value, 
						std::string &errmsg);

	bool 		getCrtValue(const std::string &parmName, 
					std::string &value);

	static bool commandFactory(std::string configFile, AosCommandPtr& cmd);

private:
	AosCorCtrl::E 	calculateCorrectCtrl(const AosCorCtrl::E c);
	bool	isObjectTableEmpty() const;
	bool 	isPossible(const AosCorCtrl::E crtCtrl, 
					const bool isCorrect) const;

	bool 	selectObject(
					const AosCorCtrl::E expectedCorrectness,
					AosRecord &record, 
					AosTablePtr &tableSelected,
					bool &cmdCorrect, 
					bool &isPossible);

	bool 	selectFromTable(
					const AosTablePtr &table, 
					AosRecord &record, 
					const bool isRandom);

	bool 	createCmdByParms(
					AosRecord &record, 
					const AosTablePtr &tableSelected,
					const AosCorCtrl::E expectedCorrectness, 
					bool &isGeneratedCmdCorrect,
					bool &cmdGeneratedFlag,
					std::string &cmdErrmsg,
					std::string &decl,
					std::string &deleteStmt, 
					std::string &cmd);
};

#endif

