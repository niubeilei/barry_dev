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
// 02/13/2007: Created by Jeff
//
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_RandomValueGen_RVGInt_h
#define Omn_RandomValueGen_RVGInt_h

#include "Random/Ptrs.h"
#include "Random/RandomUtil.h"
#include "Parms/RVG.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "Util/DynArray.h"
#include "Util/ValList.h"
#include "XmlParser/Ptrs.h"
#include "aosUtil/Types.h"


class OmnStrParser;


class AosRVGInt : public AosRVG
{
	OmnDefineRCObject;

	enum
	{
		eGenerateIntNotInTable,
		eGenerateIntInTable, 
		eGenerateInvalidInt, 
		
		eDefaultSelectFromTablePct = 50,
		
		eDefaultNotInTableWeight = 20,
		eDefaultInTableWeight = 20,
		eDefaultInvalidIntGenWeight = 25,

		eDefaultWeight = 100,
		eMaxSegments = 100
	};

private:
	AosRandomIntegerPtr	mValidIntGen;
	AosRandomIntegerPtr	mInvalidIntGen;
	AosRandomIntegerPtr	mBadValueSelector;
	
	int			mSelectFromTablePct;
	
	int			mInvalidIntGenWeight;
	int			mInTableWeight;
	int			mNotInTableWeight;

	int			mLastValueInt;
	int			mBadValue;
	OmnString	mLastValueStr;
	bool		mUsedAsStr;

public:
	AosRVGInt(const AosCliCmdTorturerPtr &cmd);
	virtual ~AosRVGInt();
	
//	bool config(const OmnXmlItemPtr &def);
	bool setCorrectInt(int64_t* lows, int64_t* highes, int num, u16* weights = NULL);
	bool setIncorrectInt(int64_t* lows, int64_t* highes, int num, u16* weights = NULL);
	static AosRVGIntPtr createInstance(
				const AosCliCmdTorturerPtr &cmd,
				const OmnXmlItemPtr &def, 
				const OmnString &cmdTag,
				OmnVList<AosGenTablePtr> &tables);

	// 
	// AosRVG interface
	//
	virtual bool nextStr(OmnString& value,
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
	virtual bool config(const OmnXmlItemPtr &def, 
					const OmnString &cmdTag, 
					OmnVList<AosGenTablePtr> &tables);
	virtual bool getCrtValue(uint8_t &value) const;
	virtual bool getCrtValue(short &value) const;
	virtual bool getCrtValue(int &value) const;
	virtual bool getCrtValue(u16 &value) const;
	virtual bool getCrtValue(u32 &value) const;
	virtual bool getCrtValue(OmnString &value) const;
	virtual bool getCrtValue(OmnIpAddr &value) const;
	virtual bool getCrtValueAsArg(OmnString &value, 
								  OmnString &decl, 
								  const u32 argIndex) const;
	virtual OmnString getNextValueFuncCall() const;
	virtual bool 	newRecordAdded(const AosGenRecordPtr &newRecord) const;
	virtual bool 	recordDeleted(const AosGenRecordPtr &record) const;
	virtual bool 	setRecord(const AosGenRecordPtr &record) const;
	virtual OmnString toString(const u32 indent) const;
	virtual AosRVGType::E getRVGType() const {return AosRVGType::eInt;}


	bool nextExist(int& value,
				const AosGenTablePtr &data, 
				const AosGenRecordPtr &record,
				bool &isCorrect,
				const bool correctOnly,
				const bool selectFromRecord,
				AosRVGReturnCode &rcode, 
				OmnString &errmsg);
	bool nextNew(int& value,
				const AosGenTablePtr &data, 
				const AosGenRecordPtr &record,
				bool &isCorrect, 
				const bool correctOnly,
				const bool selectFromRecord,
				AosRVGReturnCode &rcode, 
				OmnString &errmsg);
	bool nextNotKey(int& value,
				const AosGenTablePtr &data, 
				const AosGenRecordPtr &record,
				bool &isCorrect, 
				const bool correctOnly,
				const bool selectFromRecord,
				AosRVGReturnCode &rcode, 
				OmnString &errmsg);
	bool nextNotKey(int& value,
				const AosGenRecordPtr &record,
				bool &isCorrect, 
				const bool correctOnly,
				AosRVGReturnCode &rcode, 
				OmnString &errmsg);
	bool nextSubkeyNew(int& value,
				const AosGenTablePtr &data, 
				const AosGenRecordPtr &record,
				bool &isGood, 
				const bool correctOnly,
				const bool selectFromRecord,
				AosRVGReturnCode &rcode, 
				OmnString &errmsg);
					
	bool addGoodEntry(const OmnString &e);
	bool generateBadValue(int& value,
				const AosGenTablePtr &data, 
				const AosGenRecordPtr &record,
				AosRVGReturnCode &rcode, 
				OmnString &errmsg);

private:
	bool    setValidValues(const OmnString &values);
	bool 	setInvalidValues(const OmnString &values);
	bool 	parseValues(const OmnString &values,
	                int64_t *low, int64_t *high, u16 *weight, int &numSegs);
	bool 	getInt(int64_t &value, 
					OmnStrParser &parser, 
					const OmnString &errmsg);
	bool 	skip(const char c, OmnStrParser &parser);

	bool	createSelectors();
};

#endif

