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
// This class generates a string value. It can be good or incorrect. 
// Each parameter may relate to a field in GenTable. If so, mFieldName
// indicates which field (it must be a string field) this parameter
// relates to. Otherwise, mFieldName is set to "". 
//
// When generating an integer, it can generate in one of the following
// ways:
// 1. Generate a correct integer
//    a. Generate the correct integer outside the table if it is a key
//    b. Generate the correct integer by random generate in valid range
// 2. Generate an incorrect integer:
//    a. In the table (key, in table incorrect), or not in the table
//       (key, in table good)
//    b. In the invalid set. 
//
// The member data mCorrectPct controls the percentage of correct
// strings. 
// 
// Modification History:
// 02/06/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "Parms/RVGInt.h"

#include "Alarm/Alarm.h"
#include "aos/aosReturnCode.h"
#include "alarm/Alarm.h"
#include "Random/RandomInteger.h"
#include "Random/RandomStr.h"
#include "Random/RandomUtil.h"
#include "Util/OmnNew.h"
#include "Util/GenRecord.h"
#include "Util/GenTable.h"
#include "Util/StrParser.h"


AosRVGInt::AosRVGInt(const AosCliCmdTorturerPtr &cmd)
:
AosRVG(cmd),
mSelectFromTablePct(eDefaultSelectFromTablePct),
mInvalidIntGenWeight(eDefaultInvalidIntGenWeight),
mInTableWeight(eDefaultInTableWeight),
mNotInTableWeight(eDefaultNotInTableWeight)
{
}


AosRVGIntPtr
AosRVGInt::createInstance(
				const AosCliCmdTorturerPtr &cmd, 
				const OmnXmlItemPtr &def, 
				const OmnString &cmdTag,
				OmnVList<AosGenTablePtr> &tables)
{
	AosRVGIntPtr inst = OmnNew AosRVGInt(cmd);
	if (!inst)
	{
		OmnAlarm << "Run out of memory" << enderr;
		return 0;
	}

	if (!inst->config(def, cmdTag, tables))
	{
		OmnAlarm << "Failed to create the parm: " << def->toString() << enderr;
		return 0;
	}

	return inst;
}


bool
AosRVGInt::config(const OmnXmlItemPtr &def, 
				const OmnString &cmdTag,
				OmnVList<AosGenTablePtr> &tables)
{
	// 
	// 	<Parm>
	// 		<type>
	// 		<Name>
	// 		<DataType>
	// 		<FieldName>
	// 		<KeyType>
	// 		<BadValue>
	// 		<ValidValues>[low, high, weight], [low, high, weight]...</ValidValues>
	// 		<BadValues>[low, high, weight], [low, high, weight]...</BadValues>
	// 		<CorrectOnly>
	// 		<TableName>
	// 		<DependedTables>
	// 		<UsedAsString>
	// 		...
	// 	</Parm>
	//
	if (!AosRVG::config(def, cmdTag, tables))
	{
		OmnAlarm << "Failed to parse parm: " << def->toString() << enderr;
		return false;
	}
	mBadValue = def->getInt("BadValue", INT_MIN);
	mUsedAsStr = def->getBool("UsedAsString", false);

	OmnString validValues = def->getStr("ValidValues", "[INT_MIN, INT_MAX, 50");
	OmnString badValues = def->getStr("BadValues", "");
	if (validValues == "")
	{
		OmnAlarm << "Missing ValidValues tag: " << def->toString() << enderr;
		return 0;
	}

	mIsGood = setInvalidValues(badValues) &&
			  createSelectors() &&
			  setValidValues(validValues);

	return mIsGood;
}


AosRVGInt::~AosRVGInt()
{
}


bool
AosRVGInt::createSelectors()
{
	switch (mKeyType)
	{
	case eAosRVGKeyType_SingleKeyExist:
		 aos_assert_r(mFieldName != "", false);
		 mInTableWeight = 0;
		 break;

	case eAosRVGKeyType_MultiKeyNew:
	case eAosRVGKeyType_MultiKeyExist:
		 aos_assert_r(mFieldName != "", false);
	     mInTableWeight= 0;
	     mNotInTableWeight = 0;
		 break;

	case eAosRVGKeyType_SingleKeyNew:
		 aos_assert_r(mFieldName != "", false);
	     if (!mOverrideFlag) mNotInTableWeight = 0;
		 break;

	case eAosRVGKeyType_NoKey:
		 mNotInTableWeight = 0;
		 mInTableWeight = 0;
		 break;

	default:
	     OmnAlarm << "Unrecognized key type: " << mKeyType << enderr;
	}

	if (!mInvalidIntGen) 
	{
		mInvalidIntGenWeight = 0;
	}

	int64_t index[3];
	u16 weights[3];
	int idx = 0;
	int sum = 0;
	index[idx] = eGenerateIntNotInTable;	
	weights[idx] = mNotInTableWeight;
	sum += weights[idx++];

	index[idx] = eGenerateIntInTable;		
	weights[idx] = mInTableWeight;
	sum += weights[idx++];

	index[idx] = eGenerateInvalidInt;		
	weights[idx] = mInvalidIntGenWeight;
	sum += weights[idx++];

	if (sum > 0)
	{
		mBadValueSelector = OmnNew AosRandomInteger(
			index, 
			0, 
			weights, 
			idx, 
			0);
		aos_assert_r(mBadValueSelector, false);
	}

	return true;
}


bool
AosRVGInt::nextInt(
				int &value,
				const AosGenTablePtr &table, 
				const AosGenRecordPtr &record, 
				bool &isCorrect, 
				const bool correctOnly,
				const bool selectFromRecord,
				AosRVGReturnCode &rcode, 
				OmnString &errmsg)
{
	bool ret = true;
	AosGenTablePtr data = (mTable)?mTable:table;
	switch (mKeyType)
	{
	case eAosRVGKeyType_SingleKeyExist:
		 ret = nextExist(mLastValueInt, data, record, isCorrect, correctOnly, 
				 selectFromRecord, rcode, errmsg);
		 break;

	case eAosRVGKeyType_SingleKeyNew:
		 ret = nextNew(mLastValueInt, data, record, isCorrect, correctOnly, 
				 selectFromRecord, rcode, errmsg);
		 break;

	case eAosRVGKeyType_MultiKeyExist:
	case eAosRVGKeyType_MultiKeyNew:
	case eAosRVGKeyType_NoKey:
		 ret = nextNotKey(mLastValueInt, data, record, isCorrect, correctOnly, 
				 selectFromRecord, rcode, errmsg);
		 break;

	default:
		 OmnAlarm << "Unrecognized argument type: " 
			 << mKeyType << enderr;
		 errmsg = "Invalid Key";
		 mLastValueInt = mBadValue;
		 rcode = eAosRVGRc_ProgramError;
		 ret = false;
		 break;
	}
	
	if (ret) value = mLastValueInt;
	return ret;
}


bool
AosRVGInt::nextInt(
				u32 &value,
				const AosGenTablePtr &data, 
				const AosGenRecordPtr &record, 
				bool &isCorrect, 
				const bool correctOnly,
				const bool selectFromRecord,
				AosRVGReturnCode &rcode, 
				OmnString &errmsg)
{
	bool ret;
	int intValue;
	
	ret = nextInt(intValue, data, record, isCorrect, correctOnly, 
			selectFromRecord, rcode, errmsg);
	if (ret)
		value = (u32)intValue;

	return ret;
}


bool
AosRVGInt::nextStr(OmnString &value, 
					const AosGenTablePtr &data, 
					const AosGenRecordPtr &record,
					bool &isGood,
					const bool correctOnly,
					const bool selectFromRecord,
					AosRVGReturnCode &rcode, 
					OmnString &errmsg)
{
	value = "";
	bool ret = nextInt(mLastValueInt, data, record, isGood, correctOnly,
					selectFromRecord, rcode, errmsg);
	if (!ret) return false;

	if (mUsedAsStr) 
	{
		value = mLastValue;
	}
	else
	{
		value << mLastValueInt;
	}

	return ret;
}


bool
AosRVGInt::nextExist(int& value, 
					  const AosGenTablePtr &data, 
					  const AosGenRecordPtr &record,
					  bool &isGood,
					  const bool correctOnly,
					  const bool selectFromRecord,
					  AosRVGReturnCode &rcode,
					  OmnString &errmsg)
{
	rcode = eAosRVGRc_ProgramError;
	isGood = false;
	if (selectFromRecord)
	{
		aos_assert_r(mFieldName != "", false);
		aos_assert_r(record, false);
		if (mUsedAsStr)
		{
			aos_assert_r(record->getStr(mFieldName, mLastValue), false);
		}
		else
		{
			aos_assert_r(record->getInt(mFieldName, value), false);
		}
		rcode = eAosRVGRc_SelectFromRecord;
		isGood = true;
		return true;
	}
	
	isGood = false;
	if (mCorrectOnly || correctOnly || aos_next_pct(mCorrectPct))
	{
		if (data && data->entries() > 0)
		{
			AosGenRecordPtr rr = data->getRandomRecord();
			aos_assert_r(rr, false);

			if (mUsedAsStr)
			{
				aos_assert_r(rr->getStr(mFieldName, mLastValue), false);
				if (mDependedTables.entries() > 0 &&
					isUsedByOtherTables(mLastValue, isGood, rcode, errmsg))
				{
					isGood = false;
					return true;
				}
			}
			else
			{
				aos_assert_r(rr->getInt(mFieldName, value), false);
				if (mDependedTables.entries() > 0 &&
					isUsedByOtherTables(value, isGood, rcode, errmsg))
				{
					isGood = false;
					return true;
				}

				mLastValue = "";
				mLastValue << value;
			}

			isGood = true;
			rcode = eAosRVGRc_SelectFromTable;
			return true;
		}
	}

	isGood = false;
	return generateBadValue(value, data, record, rcode, errmsg);
}


bool
AosRVGInt::nextNew(int& value,
					const AosGenTablePtr &data, 
					const AosGenRecordPtr &record,
					bool &isGood, 
					const bool correctOnly,
					const bool selectFromRecord,
					AosRVGReturnCode &rcode,
					OmnString &errmsg)
{
	if (selectFromRecord)
	{
		aos_assert_r(mFieldName != "", false);
		aos_assert_r(record, false);

		if (mUsedAsStr)
		{
			aos_assert_r(record->getStr(mFieldName, mLastValue), false);
		}
		else
		{
			aos_assert_r(record->getInt(mFieldName, value), false);
		}
		rcode = eAosRVGRc_SelectFromRecord;
		isGood = true;
		return true;
	}
	
	isGood = false;
	rcode = eAosRVGRc_ProgramError;
	if (mCorrectOnly || correctOnly || aos_next_pct(mCorrectPct))
	{
		int tries = (mCorrectOnly || correctOnly)?50:5;;
		while (tries--)
		{
			value = mValidIntGen->nextInt();
			AosGenRecordPtr rr;
			if (!data || !data->valueExist(mFieldName, value, rr))
			{
				rcode = eAosRVGRc_Success;
				isGood = true;
				mLastValue = "";
				mLastValue << value;
				return true;
			}
		}
		
		// 
		// We tried 'tries' number of times, but each time the generated
		// value is in the table. This should happen VERY rarely. 
		//
		isGood = false;
		errmsg = mArgName;
		errmsg << " exist in the table. ";
		rcode = eAosRVGRc_ExistInTable;
		mLastValue = "";
		mLastValue << value;
		return true;
	}

	isGood = false;
	return generateBadValue(value, data, record, rcode, errmsg);
}


bool
AosRVGInt::generateBadValue(int& value,
							 const AosGenTablePtr &data, 
							 const AosGenRecordPtr &record, 
							 AosRVGReturnCode &rcode,
							 OmnString &errmsg)
{
	int tries = 10;

	if (!mBadValueSelector)
	{
		rcode = eAosRVGRc_Incorrect;
		value = mBadValue;
		mLastValue = "";
		mLastValue << value;
		errmsg = __FILE__;
		errmsg << ":" << __LINE__ << " bad value. "
			<< mArgName << ": bad value: " << value;
		return true;
	}

	rcode = eAosRVGRc_ProgramError;

	int method;
	switch ((method = mBadValueSelector->nextInt()))
	{
	case eGenerateIntInTable:
		 if (data && data->entries() > 0)
		 {
		 	errmsg = mArgName;
			errmsg << " is in the table. ";
			rcode = eAosRVGRc_ExistInTable;
		 	AosGenRecordPtr rr = data->getRandomRecord();
			aos_assert_r(rr, false);
			
			if (mUsedAsStr)
			{
				aos_assert_r(rr->getStr(mFieldName, mLastValue), false);
			}
			else
			{
				aos_assert_r(rr->getInt(mFieldName, value), false);
			}
			return true;
		 }
		 goto bad_value;
		 
	case eGenerateIntNotInTable:
		 while (tries--)
		 {
		 	value = mValidIntGen->nextInt();
			mLastValue = "";
			mLastValue << value;
			AosGenRecordPtr rr;
			if (!data || 
				mUsedAsStr && !data->valueExist(mFieldName, mLastValue, rr) ||
				!mUsedAsStr && !data->valueExist(mFieldName, value, rr))
			{
				errmsg = mArgName;
				errmsg << " not in the table. ";
				rcode = eAosRVGRc_NotFoundInTable;
				return true;
			}
		 }

bad_value:
	//
	// skip to next error generator
	// 
	case eGenerateInvalidInt: 
		 errmsg = mArgName;
		 errmsg << " invalid value.";
		 if (mInvalidIntGen)
		 {
		 	value = mInvalidIntGen->nextInt();
		 	rcode = eAosRVGRc_FromInvalidSet;
		 }
		 else
		 {
		 	value = mBadValue;
			rcode = eAosRVGRc_Incorrect;
		 }
		 mLastValue = "";
		 mLastValue << value;
		 return true;

	default:
		 errmsg = mArgName;
		 errmsg << " unrecognized method, null is used. ";
		 rcode = eAosRVGRc_ProgramError;
		 OmnAlarm << "Unrecognized method: " << method << enderr;
		 return false;
	}

	aos_should_never_come_here;
	return false;
}

bool
AosRVGInt::nextNotKey(int& value,
				const AosGenTablePtr &data, 
				const AosGenRecordPtr &record, 
				bool &isGood, 
				const bool correctOnly,
				const bool selectFromRecord,
				AosRVGReturnCode &rcode,
				OmnString &errmsg)
{
	if (selectFromRecord && mFieldName != "")
	{
		aos_assert_r(record, false);
		if (mUsedAsStr)
		{
			aos_assert_r(record->getStr(mFieldName, mLastValue), false);
		}
		else
		{
			aos_assert_r(record->getInt(mFieldName, value), false);
		}
		rcode = eAosRVGRc_SelectFromRecord;
		isGood = true;
		return true;
	}
	
	rcode = eAosRVGRc_ProgramError;
	if (mCorrectOnly || correctOnly || aos_next_pct(mCorrectPct))
	{
		if (data && data->entries() &&
		    mFieldName != "" && aos_next_pct(mSelectFromTablePct))
		{
			isGood = false;
			AosGenRecordPtr rr = data->getRandomRecord();
			aos_assert_r(rr, false);

			if (mUsedAsStr)
			{
				aos_assert_r(rr->getStr(mFieldName, mLastValue), false); 
			}
			else
			{
				aos_assert_r(rr->getInt(mFieldName, value), false); 
			}
			rcode = eAosRVGRc_SelectFromTable;
			isGood = true;
			return true;
		}

		isGood = true;
		rcode = eAosRVGRc_Success;
		value = mValidIntGen->nextInt();
		mLastValue = "";
		mLastValue << value;
		return true;
	}

	isGood = false;
	return generateBadValue(value, data, record, rcode, errmsg);
}


// 
// This function sets the corresponding field of the record
// using 'mLastValue'. This function is normally called when
// the torturer needs to add a record into the table.
//
bool 
AosRVGInt::newRecordAdded(const AosGenRecordPtr &newRecord) const
{
	aos_assert_r(newRecord, false);

	if (mFieldName == "")
	{
		return true;
	}

	if (newRecord->isFieldMarked(mFieldName))
	{
		OmnAlarm << "Field has already been set: " 
			<< mFieldName << enderr;
		return false;
	}

	if (!newRecord->set(mFieldName, mLastValueInt))
	{
		OmnAlarm << "Failed to set value" << enderr;
		return false;
	}

	newRecord->markField(mFieldName);
	return true;
}


bool 	
AosRVGInt::recordDeleted(const AosGenRecordPtr &record) const
{
	return true;
}


bool
AosRVGInt::skip(const char c, OmnStrParser &parser)
{
	if (parser.finished()) return true;
	if (parser.expectNext(c, true)) 
	{
		parser.skipWhiteSpace();
		return true;
	}

	OmnAlarm << "Expecting: " << c << " but failed: " << parser.getStr() << enderr;
	return false;
}


bool
AosRVGInt::getInt(int64_t &value, 
				   OmnStrParser &parser, 
				   const OmnString &errmsg)
{
	if (parser.nextInt(value)) return true;

	OmnAlarm << errmsg << ". " << parser.getStr() << enderr;
	return false;
}


bool
AosRVGInt::setValidValues(const OmnString &values)
{
	// 
	// 'values' is the inform of:
	// "[low, high, weight], [low, high, weight], ..., [low, high, weight]"
	//
	OmnStrParser parser(values);
	int64_t lows[eMaxSegments];
	int64_t highs[eMaxSegments];
	u16 weights[eMaxSegments];
	int numSegs = eMaxSegments;

	if (!parseValues(values, lows, highs, weights, numSegs))
	{
		OmnAlarm << "Failed to set valid values" << enderr;
		return false;
	}

	return setCorrectInt(lows, highs, numSegs, weights);
}


bool
AosRVGInt::setCorrectInt(int64_t * lows, int64_t * highes, int num, u16* weights)
{
	bool isNew = false;

	if (weights == NULL)
	{
		weights = OmnNew u16[num];
		aos_assert_r(weights, false);

		for (int i = 0; i < num; i++)
			weights[i] = eDefaultWeight;

		isNew = true;
	}

	mValidIntGen = OmnNew AosRandomInteger(
			lows, 
			highes, 
			weights, 
			num, 
			0);
	
	if (isNew)
		OmnDelete(weights);

	return true;
}


bool
AosRVGInt::parseValues(const OmnString &values, 
						int64_t *lows, 
						int64_t *highs, 
						u16 *weights, 
						int &numSegs)
{
	// 
	// 'values' is the inform of:
	// "[low, high, weight], [low, high, weight], ..., [low, high, weight]"
	//
	OmnStrParser parser(values);
	int index = 0;
	int64_t ww;

	if (values == "")
	{
		numSegs = 0;
		return true;
	}

	while (index < numSegs && !parser.finished())
	{
		if (!skip('[', parser)) return false;
		if (!getInt(lows[index], parser, "Expecting low")) return false;
		if (!skip(',', parser)) return false;
		if (!getInt(highs[index], parser, "Expecting high")) return false;
		if (!skip(',', parser)) return false;
		if (!getInt(ww, parser, "Expecting weight")) return false;
		if (!skip(']', parser)) return false;
		if (!skip(',', parser)) return false;

		if (lows[index] > highs[index])
		{
			OmnAlarm << "Low is greater than high: " << values << enderr;
			return false;
		}

		if (ww < 0)
		{
			OmnAlarm << "Weight is negative: " << values << enderr;
			return false;
		}
		weights[index] = (u16) ww;
		index++;
	}

	numSegs = index;
	return true;
}


bool
AosRVGInt::setInvalidValues(const OmnString &values)
{
	// 
	// 'values' is the inform of:
	// "[low, high, weight], [low, high, weight], ..., [low, high, weight]"
	//
	OmnStrParser parser(values);
	int64_t lows[eMaxSegments];
	int64_t highs[eMaxSegments];
	u16 weights[eMaxSegments];
	int numSegs = eMaxSegments;

	if (!parseValues(values, lows, highs, weights, numSegs))
	{
		OmnAlarm << "Failed to set valid values" << enderr;
		return false;
	}

	return setIncorrectInt(lows, highs, numSegs, weights);
}


bool
AosRVGInt::setIncorrectInt(int64_t* lows, int64_t* highes, int num, u16* weights)
{
	if (num <= 0)
	{
		return true;
	}

	bool isNew = false;

	if (weights == NULL)
	{
		weights = OmnNew u16[num];
		aos_assert_r(weights, false);

		for (int i = 0; i < num; i++)
			weights[i] = eDefaultWeight;

		isNew = true;
	}

	mInvalidIntGen = OmnNew AosRandomInteger(
			lows, 
			highes, 
			weights, 
			num, 
			0);

	if (isNew)
		OmnDelete(weights);

	return true;
}


OmnString
AosRVGInt::toString(const u32 indent) const
{
	OmnString str;
	OmnString ind(indent, ' ', true);

	str << ind << "Argument: " << mArgName << "\n"
		<< ind << "    KeyType:            " << AosGetRVGKeyTypeStr(mKeyType) << "\n"
		<< ind << "    FieldName:          " << mFieldName << "\n"
		<< ind << "    Bad Value:          " << mBadValue << "\n"
		<< ind << "    Last Value:         " << mLastValueInt << "\n"
		<< ind << "    CorrectPct:         " << mCorrectPct << "\n";
	return str;
}


bool 	
AosRVGInt::setRecord(const AosGenRecordPtr &record) const
{
	aos_assert_r(record, false);

	if (mFieldName == "")
	{
		// 
		// This parm cannot set the record.
		//
		return true;
	}

	if (record->isFieldMarked(mFieldName))
	{
		OmnAlarm << "Field has already been set: " 
			<< mFieldName << enderr;
		return false;
	}

	// 
	// Set the value
	//
	AosGenRecord::FieldMark mark;
	switch (mKeyType)
	{
	case eAosRVGKeyType_NoKey:
		 mark = AosGenRecord::eMarked;
		 break;

	case eAosRVGKeyType_MultiKeyExist:
	case eAosRVGKeyType_MultiKeyNew:
	case eAosRVGKeyType_SingleKeyExist:
	case eAosRVGKeyType_SingleKeyNew:
		 mark = AosGenRecord::ePrimaryKey;
		 break;

	default:
		 OmnAlarm << "Unrecognized key type: " << mKeyType << enderr;
		 mark = AosGenRecord::eMarked;
	}
		 
	if (!record->set(mFieldName, mLastValueInt))
	{
		OmnAlarm << "Failed to set value" << enderr;
		return false;
	}

	return record->markField(mFieldName, mark);
}


bool
AosRVGInt::getCrtValue(short &value) const
{
	value = mLastValueInt;
	return true;
}


bool
AosRVGInt::getCrtValue(OmnString &value) const
{
	value = "";
	value << mLastValueInt;
	return true;
}


bool
AosRVGInt::getCrtValue(OmnIpAddr &value) const
{
	value.set((u32)mLastValueInt);
	return true;
}


bool
AosRVGInt::getCrtValue(int &value) const
{
	value = (int)mLastValueInt;
	return true;
}


bool
AosRVGInt::getCrtValue(uint8_t &value) const
{
	value = (uint8_t)mLastValueInt;
	return true;
}


bool
AosRVGInt::getCrtValue(u16 &value) const
{
	value = (u16)mLastValueInt;
	return true;
}


bool
AosRVGInt::getCrtValue(u32 &value) const
{
	value = (u32)mLastValueInt;
	return true;
}


bool
AosRVGInt::getCrtValueAsArg(OmnString &value, 
							 OmnString &decl, 
							 const u32 argIndex) const
{
	value = "";
	switch (mUsageType)
	{
	case eAosRVGUsageType_InputOnly:
		 if (isStrType(mDataType))
		 {
			value << "\"" << mLastValueInt << "\"";
		 }
		 else
		 {
		 	value << mLastValueInt;
		 }
		 break;

	case eAosRVGUsageType_InputOutputThrRef:
		 if (mVarDecl != "")
		 {
		 	decl << mVarDecl << ";\n";
		 }
		 else
		 {
		 	decl << "int _mArg_" << mArgName << " = " << mLastValueInt << "; ";
		 }
		 value << "_mArg_" << mArgName;
		 break;

    case eAosRVGUsageType_OutputThrRef:
		 if (mVarDecl != "")
		 {
		 	decl << mVarDecl << ";\n";
		 }
		 else
		 {
		 	decl << "int _mArg_" << mArgName << "; ";
		 }
		 value << "_mArg_" << mArgName;
		 break;

	case eAosRVGUsageType_OutputThrAddr:
		 if (mVarDecl != "")
		 {
		 	decl << mVarDecl << ";\n";
		 }
		 else
		 {
		 	decl << "int _mArg_" << mArgName << "; ";
		 }
		 value << "&_mArg_" << mArgName;
		 break;

	case eAosRVGUsageType_InputOutputThrAddr:
		 if (mVarDecl != "")
		 {
		 	decl << mVarDecl << ";\n";
		 }
		 else
		 {
		 	decl << "int _mArg_" << mArgName << " = " << mLastValueInt << "; ";
		 }
		 value << "&_mArg_" << mArgName;
		 break;

	default:
		 OmnAlarm << "Unrecognized UsageType: " << mUsageType << enderr;
		 return false;
	}

	return true;
}


OmnString 
AosRVGInt::getNextValueFuncCall() const
{
	if (mUsedAsStr) return "nextStr";
	return "nextInt";
}
