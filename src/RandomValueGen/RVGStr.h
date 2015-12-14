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
#ifndef Omn_RandomValueGen_RVGStr_h
#define Omn_RandomValueGen_RVGStr_h

#include "Random/Ptrs.h"
#include "Random/RandomUtil.h"
#include "Parms/RVG.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "Util/ValList.h"
#include "Util/DynArray.h"


class AosRVGStr : public AosRVG
{
	OmnDefineRCObject;

public:
	enum
	{
		eGenerateTooShortStr, 
		eGenerateTooLongStr,
		eGenerateStrNotInTable,
		eGenerateStrInTable, 
		eGenerateFromIncorrectSet, 
		eGenerateFromBadValueGen,
		eGenerateNullStr,
		eGenerateIncorrectValue,

		eDefaultMaxLen = 32,
		eDefaultSelectFromTablePct = 50,
		eDefaultSelectFromRecordPct = 50,

		eDefaultTooShortWeight = 10,
		eDefaultTooLongWeight = 20,
		eDefaultInTableWeight = 20,
		eDefaultNotInTableWeight = 20,
		eDefaultNullStrWeight = 5,
		eDefaultInvalidStrGenWeight = 25,
		eDefaultFromBadValueGenWeight = 40,
		eDefaultGenerateIncorrectWeight = 40,
		eDefaultStrMax = 32
	};

protected:
	AosStrType			mStrType;
	u32					mMin;
	u32					mMax;
	OmnString			mBadValue;
	AosRandomStrPtr		mStrGen;
	AosRandomStrPtr		mBadValueGen;
	AosRandomIntegerPtr	mTooLongStrSelector;
	AosRandomIntegerPtr	mBadValueSelector;
	OmnDynArray<AosRandomStrPtr>	mInvalidStrGen;

	int			mSelectFromTablePct;
	int			mTooShortWeight;
	int			mTooLongWeight;
	int			mInTableWeight;
	int			mNotInTableWeight;
	int			mNullStrWeight;
	int			mInvalidStrGenWeight;
	int			mFromBadValueGenWeight;
	int			mGenerateIncorrectWeight;
	bool		mNoNullStr;

public:
	AosRVGStr(const AosCmdTorturerPtr &cmd);
	virtual ~AosRVGStr();

	// 
	// AosRVG interface
	//
	virtual bool nextStr(
				OmnString &value,
				const AosGenTablePtr &data,
				const AosGenRecordPtr &record,
				bool &isCorrect,
				const bool correctOnly,
				const bool selectFromRecord,
				AosRVGReturnCode &rcode, 
				OmnString &errmsg);

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

	virtual bool 	config(const OmnXmlItemPtr &def, 
							const OmnString &cmdTag,
							OmnVList<AosGenTablePtr> &tables);
	virtual bool 	newRecordAdded(const AosGenRecordPtr &newRecord) const;
	virtual bool 	recordDeleted(const AosGenRecordPtr &record) const;
	virtual bool 	setRecord(const AosGenRecordPtr &record) const;
	virtual OmnString 		toString(const u32 indent) const;
	virtual AosRVGType::E 	getRVGType() const {return AosRVGType::eStr;}
	static 	AosRVGStrPtr	createInstance(
								const AosCmdTorturerPtr &cmd,
								const OmnXmlItemPtr &def, 
								const OmnString &cmdTag,
								OmnVList<AosGenTablePtr> &tables);


	bool	nextExist(OmnString &value, const AosGenTablePtr &data, 
				const AosGenRecordPtr &record,
				bool &isCorrect, 
				const bool correctOnly,
				const bool selectFromRecord,
				AosRVGReturnCode &rcode, 
				OmnString &errmsg);
	bool	nextNew(OmnString &value, const AosGenTablePtr &data, 
				const AosGenRecordPtr &record,
				bool &isCorrect, 
				const bool correctOnly,
				const bool selectFromRecord,
				AosRVGReturnCode &rcode, 
				OmnString &errmsg);
	bool	nextNotKey(OmnString &value, const AosGenTablePtr &data, 
				const AosGenRecordPtr &record,
				bool &isCorrect, 
				const bool correctOnly,
				const bool selectFromRecord,
				AosRVGReturnCode &rcode, 
				OmnString &errmsg);
	bool	nextNotKey(OmnString &value, 
				const AosGenRecordPtr &record,
				bool &isCorrect, 
				const bool correctOnly,
				AosRVGReturnCode &rcode, 
				OmnString &errmsg);
	bool 	nextSubkeyNew(OmnString &value, 
				const AosGenTablePtr &data, 
				const AosGenRecordPtr &record,
				bool &isGood, 
				const bool correctOnly,
				const bool selectFromRecord,
				AosRVGReturnCode &rcode, 
				OmnString &errmsg);

	bool	addGoodEntry(const OmnString &e);
	bool	generateBadValue(
				OmnString &value,
				const AosGenTablePtr &data, 
				const AosGenRecordPtr &record,
				AosRVGReturnCode &rcode, 
				OmnString &errmsg);


	virtual bool getCrtValue(int &value) const;
	virtual bool getCrtValue(u16 &value) const;
	virtual bool getCrtValue(u32 &value) const;
	virtual bool getCrtValue(char *&value) const;
	virtual bool getCrtValue(OmnString &value) const;
	virtual bool getCrtValueAsArg(OmnString &value, 
								  OmnString &decl,
								  const u32 argIndex) const;
	virtual OmnString getNextValueFuncCall() const;

	bool 		getCorrectValue(
					OmnString &value, 
					const AosGenTablePtr &data, 
					AosRVGReturnCode &rcode);
	bool 		genValueNotInTable(
					OmnString &value, 
					const AosGenTablePtr &data, 
					AosRVGReturnCode &rcode, 
					OmnString &errmsg);
	virtual OmnString genValue();
	virtual OmnString genIncorrectValue(
				AosRVGReturnCode &rcode,
            	OmnString &errmsg);
	void		setBadValueGen(const AosRandomStrPtr &gen);

protected:
	bool	createSelectors();
};

#endif

