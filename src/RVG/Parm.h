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
// This class is used by the CmdTorturer as the parameter object.
// correct value generation from:
//		1.correct RVG (mandatory)
//		2.user-added correct RVGs (optional)
//		3.the value generated should satisfy all the correct conditions (optional)
// 
// Incorrect value generation from:
//		1.incorrect RVGs created from correct RVG (above first). (mandatory, created default)
//		2.user-added incorrect RVGs (optional)
//		3.the value generated should satisfy all the incorrect conditions (optional)
//   
//
// Modification History:
// Saturday, December 15, 2007: Created by Allen Xu 
//
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_RVG_Parm_H
#define Aos_RVG_Parm_H

#include "RVG/RIG.h"
#include "Conditions/Ptrs.h"
#include "Conditions/CondUtil.h"

//
// This class is used by the Command as the parameter object
//
class AosParm : virtual public OmnRCObject
{
	OmnDefineRCObject;
protected:
	OmnString			mName;
	OmnString			mProductName;
	//
	// This is the command name of the parameter
	//
	OmnString			mCmdName;
	//
	// This is the argument name in the command
	//
	OmnString			mArgName;
	AosRVGUsageType::E	mUsageType;
	AosValueType::E		mDataType;
	OmnString			mDataTypeStr;
	//
	// This attribute determines whether an Parm creates correct only, 
	// incorrect only, or random values(default).
	//
	AosCorCtrl::E		mCorrectnessControl;
	//
	// determines a percentage at which an Parm should generate correct values. 
	// It is dynamically calculated by the caller. 
	// Effective when mCorrectnessControl is eRandomValue.
	//
	u32					mCorrectPct;			
	//
	// correctRVG is used to create syntax correct RVG, 
	// it will create default syntax incorrect RVGs for incorrect value generation.
	//

	AosRVGPtr			mCorrectRVG;
	//
	// The incorrect RVGs are created by the Correct RVGs automatically
	//
	AosRVGPtrArray		mIncorrectRVGs;
	//
	// The user-added incorrect RVGs are used for creating incorrect RVGs,
	// these RVGs are added/deleted by GUI users, which is different with the 
	// syntax incorrect RVGs automatically created by the syntax correct RVG.
	// when generating incorrect value, we will use the syntax incorrect RVGs 
	// and the user defined incorrect RVGs to generate incorrect values.
	//
	AosRVGPtrArray		mUserIncorrectRVGs;
	AosRVGPtrArray		mUserCorrectRVGs;

	//
	// incorrect and correct semantics condition list
	//
	AosConditionList	mCorrectCondList;
	AosConditionList	mInCorrectCondList;

	//
	// These fileds are used to select RVG from all the RVGs,
	// Created and used in runtime
	//
	AosRIG<u32>			mCorrectSelector;
	AosRIG<u32>			mIncorrectSelector;
	AosRVGPtrArray		mAllCorrectRVGs;
	AosRVGPtrArray		mAllIncorrectRVGs;

	AosValuePtr			mLastValue;
	OmnString			mVarDecl;
	OmnString			mInitMemData;

public:
	AosParm();
	~AosParm();
	virtual bool nextValue(AosValue &value, bool &isCorrect, OmnString &errmsg);
	virtual bool serialize(TiXmlNode& node);
	virtual bool deserialize(TiXmlNode* node);

	void setCorrectnessFlag(AosCorCtrl::E correctFlag);
	bool getCorrectnessFlag();

	void setCorrectPct(u32 correctPct);
	u32 getCorrectPct();

	bool getCrtValue(AosValuePtr& valuePtr);
	bool getCrtValue(AosValue& value);
	bool setCrtValue(AosValuePtr& valuePtr);
	bool setCrtValue(const OmnString &valueStr);

	bool getDeleteStmt(OmnString &stmt, const u32 argIndex) const {return true;};
	bool getCrtValueAsArg(OmnString	&value, 
						  OmnString	&decl, 
						  const u32 argIndex) const;

	OmnString getName();
	OmnString getInitMemData();
	OmnString getVarDecl();
	AosRVGUsageType::E getUsageType();
	AosValueType::E getDataType();
	OmnString	getDataTypeStr();

	bool		isInputType() const
	{ 
		return (mUsageType == AosRVGUsageType::eInputOnly ||
				mUsageType == AosRVGUsageType::eInputOutputThrRef ||
				mUsageType == AosRVGUsageType::eInputOutputThrAddr);
	}

private:
	bool initSelector();
	bool calculateCorrectFlag(bool& isCorrect);
	bool createIncorrectValue(AosValue &value, OmnString &errmsg);
	bool createCorrectValue(AosValue &value, OmnString &errmsg);
	void createCorrectSelector();
	void createIncorrectSelector();
};

#endif
