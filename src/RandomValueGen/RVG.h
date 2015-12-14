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
// 02/06/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_RandomValueGen_RVG_h
#define Omn_RandomValueGen_RVG_h

#include "Action/Ptrs.h"
#include "Alarm/Alarm.h"
#include "CmdTorturer/Ptrs.h"
#include "LogicExpr/Ptrs.h"
#include "LogicExpr/LogicObj.h"
#include "Parms/Util.h"
#include "Parms/ReturnCode.h"
#include "Parms/Ptrs.h"
#include "SemanticData/Ptrs.h"
// #include "TorturerAction/ActionObj.h"
// #include "TorturerAction/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/Ptrs.h"
#include "Util/ValList.h"
#include "Util/DynArray.h"
#include "XmlParser/Ptrs.h"
#include <string>

class OmnIpAddr;
class OmnMacAddr;

class AosRVG : virtual public OmnRCObject
{

public:
	enum
	{
		eDefaultCorrectPct = 75
	};

protected:
	AosCmdKeyType::E			mKeyType;
	OmnString					mArgName;
	OmnString					mFieldName;
	OmnString					mDataType;
	u32							mCorrectPct;
	bool						mCorrectOnly;
	bool						mIsGood;
	AosGenTablePtr				mTable;
	OmnDynArray<AosGenTablePtr>	mDependedTables;
	OmnDynArray<OmnString>		mDependedFieldNames;
	bool						mIsSmartPtr;
	void *						mInstance;
	AosRVGUsageType::E			mUsageType;
	OmnString					mInstGenName;
	OmnString					mLastValue;
	AosGenTablePtr				mSourceTable;
	OmnString					mSourceFieldName;
	OmnString					mVarDecl;
	OmnString					mStructType;
	bool						mIsVerbose;
	bool						mOverrideFlag;
	OmnString					mInitMemData;
	AosLogicTermPtr				mCondition;
	AosCmdTorturerPtr			mCommand;
	OmnDynArray<AosActionPtr>	mActions;
	bool						mValueGenerated;

public:
	AosRVG(const AosCmdTorturerPtr &cmd);
	virtual ~AosRVG();

	// 
	// LogicObj interface
	//
	virtual bool 	getValue(const AosOperandType type,
			                 const OmnString &name,
			  				 OmnString &value,
			   				 OmnString &errmsg);

	// 
	// ActionObj Interface
	//
	virtual bool    setParm(const OmnString &name, 
							const OmnString &value, 
							OmnString &errmsg);
	virtual bool    setVar(const OmnString &name, 
							const OmnString &value, 
							OmnString &errmsg);
	virtual bool    setCrtValue(const std::string &name, 
							const std::string &value, 
							std::string &errmsg);

	// 
	// This function will randomly generate a value for this 
	// parameter. The generated value can be good or bad, depending
	// on the probability. If the generated is good, 'isGood'
	// is set to true. 
	// If the generated is bad, 'isGood' is set to false and
	// 'errmsg' contains the reasons of why the value is bad.
	//
	// If the generated value is selected from a record in 'data', 
	// 'record' holds that record.
	//
	// Note that this function may make decisions based on 'data'.
	//
	// 'data' can be null. 
	//
	//
	virtual bool nextStr(
				std::string &value,
				const AosVarContainerPtr &data, 
				const AosGenRecordPtr &record, 
				bool &isCorrect, 
				const bool correctOnly,
				const bool selectFromRecord,
				AosRVGReturnCode &rcode,
				std::string &errmsg);

	/*
	 * Chen Ding, 10/16/2007
	 *
	virtual bool nextInt(
				int &value,
				const AosGenTablePtr &data, 
				const AosGenRecordPtr &record, 
				bool &isCorrect, 
				const bool correctOnly,
				const bool selectFromRecord,
				AosRVGReturnCode &rcode, 
				OmnString &errmsg);

	virtual bool nextInt(
				u32 &value,
				const AosGenTablePtr &data, 
				const AosGenRecordPtr &record, 
				bool &isCorrect, 
				const bool correctOnly,
				const bool selectFromRecord,
				AosRVGReturnCode &rcode, 
				OmnString &errmsg);

	virtual bool nextExist(OmnString &value, 
					const AosGenTablePtr &data, 
				    const AosGenRecordPtr &record,
				    bool &isGood, 
					const bool correctOnly,
			 	    const bool selectFromRecord,
					AosRVGReturnCode &rcode,
				    OmnString &errmsg);
	virtual bool nextNew(OmnString &value, 
	                const AosGenTablePtr &data, 
				    const AosGenRecordPtr &record,
			 	    const bool selectFromRecord,
				    bool &isGood, 
					const bool correctOnly,
					AosRVGReturnCode &rcode,
				    OmnString &errmsg);
	virtual bool nextNotKey(OmnString &value, 
	                const AosGenTablePtr &data, 
				    const AosGenRecordPtr &record,
				    bool &isGood, 
					const bool correctOnly,
					const bool selectFromRecord,
					AosRVGReturnCode &rcode,
				    OmnString &errmsg);
	*/

	virtual bool getCrtValue(int &value) const;
	virtual bool getCrtValue(uint8_t &value) const;
	virtual bool getCrtValue(u16 &value) const;
	virtual bool getCrtValue(short &value) const;
	virtual bool getCrtValue(u32 &value) const;
	virtual bool getCrtValue(char *&value) const;
	virtual bool getCrtValue(std::string &value) const;
	virtual bool getCrtValue(OmnIpAddr &value) const;
	virtual bool getCrtValue(OmnMacAddr &value) const;
	virtual bool getCrtValue(AosRVGInstGenPtr &value) const;
	virtual bool getDeleteStmt(std::string &stmt, const u32 argIndex) const;
	virtual bool getCrtValueAsArg(std::string &value, 
								  std::string &decl, 
								  const u32 argIndex) const;
	virtual AosRVGType::E getRVGType() const = 0;

	virtual bool newRecordAdded(const AosGenRecordPtr &newRecord) const;
	virtual bool recordDeleted(const AosGenRecordPtr &record) const;
	virtual bool setRecord(const AosGenRecordPtr &record) const;
	virtual OmnString toString(const u32 indent) const;
	virtual OmnString getNextValueFuncCall() const = 0;
	virtual bool config(const OmnXmlItemPtr &def, 
					const OmnString &cmdTag,
					OmnVList<AosGenTablePtr> &tables);

	virtual void *    	getData() const {return mInstance;}
	void		setSmartPtrFlag(const bool f) {mIsSmartPtr = f;}
	bool		isSmartPtr() const {return mIsSmartPtr;}
	AosCmdKeyType::E	getKeyType() const {return mKeyType;}
	OmnString	getKeyTypeStr() const;
	OmnString	getDataType() const {return mDataType;}
	void		setDataType(const OmnString &d) {mDataType = d;}
	std::string getName() const;
	OmnString	getFieldName() const {return mFieldName;}
	OmnString	getInstGenName() const {return mInstGenName;}
	OmnString	getVarDecl() const {return mVarDecl;}
	OmnString	getStructType() const {return mStructType;}
	OmnString	getInitMemData() const {return mInitMemData;}
	void		setCorrectPct(u32 pct) {mCorrectPct = pct;}
	bool		isGood() const {return mIsGood;}
	void		setKeyType(const AosCmdKeyType::E k) {mKeyType = k;}
	bool		isVerbose() const {return mIsVerbose;}
	bool		checkCond(bool &rslt, OmnString &errmsg);
	bool		doAction(bool &rslt, OmnString &errmsg);
	bool		setValue(const std::string &value);
	bool		setCrtValue(const std::string &value);

	AosRVGUsageType::E		getUsageMethod() const {return mUsageType;}
	static AosGenTablePtr	getTable(OmnVList<AosGenTablePtr> &tables, 
							 	const OmnString &name);

	static AosRVGPtr		createRVG(
								const AosCmdTorturerPtr &cmd,
								const OmnXmlItemPtr &def, 
							   	const OmnString &cmdTag,
	 						   	OmnVList<AosGenTablePtr> &tables, 
							   	const bool codegen);
	bool 					isUsedByOtherTables(const OmnString &value, 
						  		bool &isGood,
						  		AosRVGReturnCode &rcode, 
						  		OmnString &errmsg);
	bool 					isUsedByOtherTables(const int value, 
						  		bool &isGood,
						  		AosRVGReturnCode &rcode, 
						  		OmnString &errmsg);
	static bool parseDependedTables(
					const OmnXmlItemPtr &def,
					OmnDynArray<AosGenTablePtr> &depTables,
					OmnDynArray<OmnString> &fieldNames,
					OmnVList<AosGenTablePtr> &tables);
	bool 		selectValueFromSourceTable(
					OmnString &value, 
				    bool &isGood, 
					AosRVGReturnCode &rcode,
				    OmnString &errmsg);
	bool		isStrType(const OmnString &type) const;
	AosRVGUsageType::E getUsageType() const {return mUsageType;}
	bool		isInputType() const
			    { 
					return (mUsageType == AosRVGUsageType::eInputOnly ||
					        mUsageType == AosRVGUsageType::eInputOutputThrRef ||
						    mUsageType == AosRVGUsageType::eInputOutputThrAddr);
				}
};

#endif

