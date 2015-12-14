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
// This class generates a string value. It can be correct or incorrect. 
// Each RVG may be mapped to a field in a Table. If so, mFieldName
// indicates which field (it must be a string field) this parameter
// relates to. Otherwise, mFieldName is set to "". 
//
// For more information about RSG, please refer to Document 
// 2007-000127. 
//
// Modification History:
// 02/06/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "Parms/RVGStr.h"

#include "Alarm/Alarm.h"
#include "aos/aosReturnCode.h"
#include "alarm/Alarm.h"
#include "Random/RandomInteger.h"
#include "Random/RandomStr.h"
#include "Random/RandomUtil.h"
#include "Util/OmnNew.h"
#include "Util/GenRecord.h"
#include "Util/GenTable.h"


const int sgTooLongSelectorSize = 5;
static int64_t sgTooLongSelectorLows[] = {2, 3, 4, 10, 100};
static u16 sgTooLongSelectorWeights[] = {80, 10, 7, 2, 1};
static OmnString sgDefaultStrBadValue = "***BAD***";

AosRVGStr::AosRVGStr(const AosCliCmdTorturerPtr &cmd)
:
AosRVG(cmd),
mStrType(eAosStrType_VarName),
mMin(1),
mMax(eDefaultStrMax),
mBadValue(sgDefaultStrBadValue),
mSelectFromTablePct(eDefaultSelectFromTablePct),
mTooShortWeight(eDefaultTooShortWeight),
mTooLongWeight(eDefaultTooLongWeight),
mInTableWeight(eDefaultInTableWeight),
mNotInTableWeight(eDefaultNotInTableWeight),
mNullStrWeight(eDefaultNullStrWeight),
mInvalidStrGenWeight(eDefaultInvalidStrGenWeight),
mFromBadValueGenWeight(eDefaultFromBadValueGenWeight),
mGenerateIncorrectWeight(0),
mNoNullStr(false)
{
}


AosRVGStr::~AosRVGStr()
{
}


AosRVGStrPtr
AosRVGStr::createInstance(
					const AosCliCmdTorturerPtr &cmd,
					const OmnXmlItemPtr &def, 
					const OmnString &cmdTag,
					OmnVList<AosGenTablePtr> &tables)
{
	AosRVGStrPtr inst = OmnNew AosRVGStr(cmd);
	if (!inst)
	{
		OmnAlarm << "Out of memory" << enderr;
		return 0;
	}

	if (!inst->config(def, cmdTag, tables))
	{
		OmnAlarm << "Failed to parse the parameter: "
			<< def->toString() << enderr;
		return 0;
	}

	return inst;
}


bool
AosRVGStr::config(const OmnXmlItemPtr &def, 
				   const OmnString &cmdTag, 
				   OmnVList<AosGenTablePtr> &tables)
{
	// 
	// 	<Parm>
	// 		<type>
	// 		<Name>
	// 		<DataType>
	// 		<MinLen>
	// 		<MaxLen>
	// 		<FieldName>
	// 		<KeyType>
	// 		<BadValue>
	// 		<CorrectOnly>
	// 		<StrType>
	// 		<BadStrGen>
	// 		<TableName>
	// 		<NoNullStr>
	// 		<DependedTables>
	// 		...
	// 	</Parm>
	//
	if (!AosRVG::config(def, cmdTag, tables))
	{
		OmnAlarm << "Failed to parse the parameter: " 
			<< def->toString() << enderr;
		return false;
	}

	mMin 			= def->getInt("MinLen", 1);
	mMax 			= def->getInt("MaxLen", eDefaultMaxLen);
	mBadValue 		= def->getStr("BadValue", "***BAD***");
	mStrType 		= AosStrType_strToEnum(def->getStr("StrType", "NoSpaceStr"));
	mNoNullStr		= def->getBool("NoNullStr", false);

	OmnXmlItemPtr item = def->tryItem("BadStrGen");
	if (item)
	{
		// 
		// 	<BadStrGen>
		// 		<StrType>
		// 		<MinLen>
		// 		<MaxLen>
		// 	</BadStrGen>
		//
		AosStrType strType = AosStrType_strToEnum(item->getStr("StrType", ""));
		if (strType == eAosStrType_Invalid)
		{
			OmnAlarm << "Invalid BadValueGen string type: " 
				<< item->toString() << enderr;
			return 0;
		}
		int min = item->getInt("MinLen", 1);
		int max = item->getInt("MaxLen", eDefaultMaxLen);
		mBadValueGen = OmnNew AosRandomStr(mArgName, min, max, strType);
	}

	mStrGen = OmnNew AosRandomStr(mArgName, mMin, mMax, mStrType);
	aos_assert_r(mStrGen, false);
	mIsGood = true;

	return true;
}


bool
AosRVGStr::createSelectors()
{
	// 
	// We have the following selectors:
	// 1. mTooLongStrSelector: 
	//    This selector determines how to generate too long strings. 
	//    It will return an integer. If it returns integer t, 
	//    the string to be generated will be in the range of
	//    [mMax + 1, t * mMax + 1]. By default, the selector 
	//    is:
	//    		[2, 80%]
	//    		[3, 10%]
	//    		[4, 7%]
	//    		[10, 2%]
	//    		[100, 1%]
	//
	mTooLongStrSelector = OmnNew AosRandomInteger(
			sgTooLongSelectorLows, 
			0, 
			sgTooLongSelectorWeights, 
			sgTooLongSelectorSize, 
			100);
	aos_assert_r(mTooLongStrSelector, false);

	// 
	// 2. The second selector is mBadValueSelector. For invalid
	//    values, we may generate it in one of the following 
	//    ways:
	//    a. Too Long
	//    b. Too Short
	//    c. Null
	//    d. From Incorrect Set
	//    e. In table / not in table (depends on the key type)
	//
	//    If min <= 1
	//    a. eExist:
	//    	 mTooLongPct
	//    	 mNullPct
	//    	 mNotInTablePct
	//    	 mFromIncorrectSet
	//
	//    b. eNew:
	//       mTooLongPct
	//       mNullPct
	//       mInTablePct
	//       mFromIncorrectSet
	//
	// 2. If min > 0
	//    a. eExist:
	//       mTooShort
	//    	 mTooLongPct
	//    	 mNullPct
	//    	 mNotInTablePct
	//    	 mFromIncorrectSet
	//
	//    b. eNew:
	//       mTooShort
	//       mTooLongPct
	//       mNullPct
	//       mNotInTablePct
	if (mMin <= 1) mTooShortWeight = 0;
	switch (mKeyType)
	{
	case eAosRVGKeyType_SingleKeyExist:
		 aos_assert_r(mFieldName != "", false);
	     mInTableWeight= 0;
		 break;

	case eAosRVGKeyType_SingleKeyNew:
		 aos_assert_r(mFieldName != "", false);
	     if (!mOverrideFlag) mNotInTableWeight = 0;
		 break;

	case eAosRVGKeyType_MultiKeyNew:
	case eAosRVGKeyType_MultiKeyExist:
		 if (mFieldName == "")
		 {
			 OmnAlarm << "Field name is null: " << mArgName << enderr;
			 return false;
		 };
	     mNotInTableWeight = 0;
	     mInTableWeight= 0;
		 break;

	case eAosRVGKeyType_NoKey:
	     mNotInTableWeight = 0;
		 mInTableWeight = 0;
		 break;

	default:
	     OmnAlarm << "Unrecognized key type: " << mKeyType << enderr;
	}

	if (mNoNullStr) mNullStrWeight = 0;
	if (!mBadValueGen)
	{
		mFromBadValueGenWeight = 0;
	}
	else
	{
		mTooShortWeight = 0;
		mTooLongWeight = 0;
	}

	const int size = 7;
	int64_t index[size];
	u16 weights[size];
	memset(index, 0, sizeof(int)*size);
	memset(weights, 0, sizeof(u16)*size);
	int idx = 0;
	index[idx] = eGenerateTooShortStr; 		weights[idx++] = mTooShortWeight;
	index[idx] = eGenerateTooLongStr;		weights[idx++] = mTooLongWeight;
	index[idx] = eGenerateStrNotInTable;	weights[idx++] = mNotInTableWeight;
	index[idx] = eGenerateStrInTable;		weights[idx++] = mInTableWeight;
	index[idx] = eGenerateNullStr;			weights[idx++] = mNullStrWeight;
	index[idx] = eGenerateFromBadValueGen;	weights[idx++] = mFromBadValueGenWeight;
	index[idx] = eGenerateIncorrectValue;	weights[idx++] = mGenerateIncorrectWeight;

	if (mInvalidStrGen.entries() > 0)
	{
		index[idx] = eGenerateFromIncorrectSet;		
		weights[idx++] = mInvalidStrGenWeight;
	}

	mBadValueSelector = OmnNew AosRandomInteger(
			index, 
			0, 
			weights, 
			idx, 
			0);
	aos_assert_r(mBadValueSelector, false);

	return true;
}


bool
AosRVGStr::nextInt(
				int &value,
				const AosGenTablePtr &data, 
				const AosGenRecordPtr &record, 
				bool &isCorrect, 
				const bool correctOnly,
				const bool selectFromRecord,
				AosRVGReturnCode &rcode, 
				OmnString &errmsg)
{
	OmnAlarm << "Cannot call nextInt(...) in this class!" << enderr;
	return 0;
}


bool
AosRVGStr::nextInt(
				u32 &value,
				const AosGenTablePtr &table, 
				const AosGenRecordPtr &record, 
				bool &isCorrect, 
				const bool correctOnly,
				const bool selectFromRecord,
				AosRVGReturnCode &rcode, 
				OmnString &errmsg)
{
	OmnAlarm << "Cannot call nextInt(...) in this class!" << enderr;
	return 0;
}


// 
// This is the main function to generate a new value.
//
bool
AosRVGStr::nextStr(OmnString &value, 
					const AosGenTablePtr &table, 
					const AosGenRecordPtr &record,
					bool &isGood,
					const bool correctOnly,
					const bool selectFromRecord,
					AosRVGReturnCode &rcode, 
					OmnString &errmsg)
{
	// 
	// This function will randomly generate a value for this 
	// parameter. The generated can be correct or incorrect, depending
	// on the probability. If the generated is correct, 'isGood'
	// is set to true. 
	//
	// If the generated is incorrect, 'isGood' is set to false and
	// 'errmsg' contains the reasons why the generated value
	// is incorrect. 
	//
	// If the value is selected from a record, 'record' will 
	// hold the record. 
	//
	// 'table' is the RSG's torturing table. It can be null. 
	
	// 
	// If it is to select from the record, select the value
	// and return. 
	//
	if (selectFromRecord)
	{
		aos_assert_r(record, false);
		aos_assert_r(mFieldName != "", false);
		aos_assert_r(record->getStr(mFieldName, value), false);
		isGood = true;
		rcode = eAosRVGRc_SelectFromRecord;
		return true;
	}

	//
	// If selectors are not created yet, create them.
	//
	if (!mTooLongStrSelector) createSelectors();

	bool ret;

	// 
	// Determine the table. If the caller provides a table, use 
	// that table. Otherwise, this class maintains its own table.
	//
	AosGenTablePtr data = (mTable)?mTable:table;
	switch (mKeyType)
	{
	case eAosRVGKeyType_SingleKeyExist:
		 // 
		 // This parameter is used as a key to 'table' and 
		 // if the value identifies a record in 'table', 
		 // that value is considered correct and 'record' 
		 // will hold the record.
		 //
		 ret = nextExist(mLastValue, data, record, isGood, correctOnly,
		 			selectFromRecord, rcode, errmsg);
		 break;

	case eAosRVGKeyType_SingleKeyNew:
		 // 
		 // This parameter is used as a key to 'table' and
		 // if the generated value does not identify a record
		 // in 'table', it is correct. If the generated value is
		 // incorrect, it can be syntactically incorrect, or
		 // the generated value identifies a record. In this 
		 // case, 'record' holds that record.
		 //
		 ret = nextNew(mLastValue, data, record, isGood, correctOnly,
		 			selectFromRecord, rcode, errmsg);
		 break;

	case eAosRVGKeyType_MultiKeyNew:
	case eAosRVGKeyType_MultiKeyExist:
	case eAosRVGKeyType_NoKey:
		 // 
		 // This parameter is not used as the key. If the 
		 // generated value is selected from the table, 
		 // 'record' holds that record.
		 //
		 ret = nextNotKey(mLastValue, data, record, isGood, correctOnly, 
				 selectFromRecord, rcode, errmsg);
		 break;

	default:
		 // 
		 // This should never happen.
		 //
		 OmnAlarm << "Unrecognized argument type: " 
			 << mKeyType << enderr;
		 errmsg = "Invalid Key";
		 mLastValue = mBadValue;
		 rcode = eAosRVGRc_ProgramError;
		 return false;
	}

	value = mLastValue;
	return true;
}


bool
AosRVGStr::nextExist(OmnString &value, 
					  const AosGenTablePtr &data, 
					  const AosGenRecordPtr &record,
					  bool &isGood, 
					  const bool correctOnly,
					  const bool selectFromRecord,
					  AosRVGReturnCode &rcode,
					  OmnString &errmsg)
{
	// 
	// This parameter is used as a key to 'data' and 
	// if the value identifies a record in 'data', 
	// that value is considered good. If the generated
	// value is good, 'record' will hold the record.
	//
	rcode = eAosRVGRc_ProgramError;
	isGood = false;
	if (mCorrectOnly || correctOnly || aos_next_pct(mCorrectPct))
	{
		//
		// Will generate a correct string. We will select one from the table
		//
		if (data && data->entries() > 0)
		{
			AosGenRecordPtr theRecord = data->getRandomRecord();
			aos_assert_r(theRecord, false);
			aos_assert_r(theRecord->getStr(mFieldName, value), false);

			// 
			// Check whethr the this parameter is used by other tables 
			// (by calling 'isUsedByOtherTables(...)'). If yes, it 
			// means that this value is incorrect. 
			//
			if (mDependedTables.entries() > 0 && 
				isUsedByOtherTables(value, isGood, rcode, errmsg))
			{
				return true;
			}
				
			isGood = true;
			rcode = eAosRVGRc_SelectFromTable;
			return true;
		}
	}

	// 
	// Otherwise, we will generate an incorrect string. 
	// Refer to the comments at the beginning of this file for
	// how to generate an incorrect value.
	//
	isGood = false;
	return generateBadValue(value, data, record, rcode, errmsg);
}


bool
AosRVGStr::nextNew(OmnString &value, 
					const AosGenTablePtr &data, 
					const AosGenRecordPtr &record,
					bool &isGood, 
					const bool correctOnly,
					const bool selectFromRecord,
					AosRVGReturnCode &rcode,
					OmnString &errmsg)
{
	// 
	// 'isGood' is true if the generated string is not in 
	// 'data' and its length is [mMin, mMax]. Otherwise, 
	// 'isGood' is set to false.
	//
	// Error Conditions:
	// For error conditions, please refer to generateBadValue().
	//
	isGood = true;
	if (mCorrectOnly || correctOnly || aos_next_pct(mCorrectPct))
	{
		// 
		// Check whether it should choose a value from a source table.
		// What is "source table"? Chen Ding, 10/17/2007
		//
		if (mSourceTable && mSourceFieldName != "")
		{
			if (selectValueFromSourceTable(value, isGood, rcode, errmsg))
			{
				return true;
			}

			isGood = false;
			return generateBadValue(value, data, record, rcode, errmsg);
		}

		//
		// Will generate a correct string 
		//
		if (getCorrectValue(value, data, rcode))
		{
			isGood = true;
			return true;
		}

		// 
		// We tried 'tries' number of times, but each time the generated
		// value is in the table. This should happen VERY rarely. 
		//
		errmsg = mArgName;
		errmsg << " exist in the table. ";
		isGood = false;
		rcode = eAosRVGRc_ExistInTable;
		return true;
	}

	// 
	// Otherwise, we will generate an incorrect string. 
	//
	isGood = false;
	return generateBadValue(value, data, record, rcode, errmsg);
}


bool
AosRVGStr::generateBadValue(OmnString &value, 
							 const AosGenTablePtr &data, 
							 const AosGenRecordPtr &record, 
							 AosRVGReturnCode &rcode,
							 OmnString &errmsg)
{
	OmnString str;
	int index;

	aos_assert_r(mBadValueSelector, mBadValue);

	int method;
	rcode = eAosRVGRc_ProgramError;
	errmsg = "Program Error";
	switch ((method = mBadValueSelector->nextInt()))
	{
	case eGenerateTooShortStr:
		 aos_assert_r(mMin > 1, mBadValue);
		 rcode = eAosRVGRc_TooShort;
		 errmsg = mArgName;
		 errmsg << " is too short. Minimum: " << mMin << ". ";
		 value = mStrGen->nextStr(1, mMin-1);	 

		 return true;

	case eGenerateStrInTable:
		 if (data && data->entries() > 0)
		 {
		 	errmsg = mArgName;
			errmsg << " is in the table. ";
		 	AosGenRecordPtr rr = data->getRandomRecord();
			aos_assert_r(rr, mBadValue);
			aos_assert_r(rr->getStr(mFieldName, value), mBadValue);
			rcode = eAosRVGRc_ExistInTable;
			return true;
		 }

		 // 
		 // We can't select anything from the table since the table 
		 // is either null or empty. We will skip to the next
		 // (i.e., generate a too long string).
		 //

	case eGenerateTooLongStr:
		 errmsg = mArgName;
		 errmsg << " too long. Max: " << mMax;
		 index = mTooLongStrSelector->nextInt();
		 if (mStrType == eAosStrType_HHMMSS || 
		 	 mStrType == eAosStrType_InvalidHHMMSS)
		 {
		 	value = aos_next_str(eAosStrType_VarName, mMax+1, index * mMax + 1);
		 }
		 else
		 {
		 	value = mStrGen->nextStr(mMax+1, index * mMax + 1);
		 }
		 rcode = eAosRVGRc_TooLong;
		 return true;

	case eGenerateStrNotInTable:
		 if (genValueNotInTable(value, data, rcode, errmsg)) return true;

		 // 
		 // This means that we tried quite a few times but all we tried
		 // are in the table (this happens VERY VERY rarely. We will 
		 // skip to the next error message.
		 //

	case eGenerateNullStr:
		 errmsg = "Missing: ";
		 errmsg << mArgName << ". ";
		 rcode = eAosRVGRc_NullValue;
		 value = "";
		 return true;

	case eGenerateFromIncorrectSet:
		 // 
		 // This there MUST be some incorrect value set. 
		 //
		 if (mInvalidStrGen.entries() == 0)
		 {
		 	OmnAlarm << "No Invalid Str Generator" << enderr;
			errmsg = "Missing: ";
			errmsg << mArgName << ". ";
			value = "";
			rcode = eAosRVGRc_NullValue;
			return true;
		 }

		 index = aos_next_int(0, mInvalidStrGen.entries()-1);
		 errmsg = mArgName;
		 errmsg << " from invalid charset. ";
		 aos_assert_r(mInvalidStrGen[index], "");
		 value = mInvalidStrGen[index]->nextStr();
		 rcode = eAosRVGRc_FromInvalidSet;
		 return true;

	case eGenerateFromBadValueGen:
		 aos_assert_r(mBadValueGen, false);
		 value = mBadValueGen->nextStr();
		 errmsg = mArgName;
		 errmsg << " generated from the bad string generator. ";
		 rcode = eAosRVGRc_FromInvalidSet;
		 return true;
		 
	case eGenerateIncorrectValue:
	     value = genIncorrectValue(rcode, errmsg);
		 return true;
		 
	default:
		 errmsg = mArgName;
		 errmsg << " unrecognized method, null is used. ";
		 rcode = eAosRVGRc_ProgramError;
		 OmnAlarm << "Unrecognized method: " << method << enderr;
		 value = "";
		 return false;
	}

	aos_should_never_come_here;

	return false; 
}


bool
AosRVGStr::nextNotKey(
				OmnString &value, 
				const AosGenTablePtr &data, 
				const AosGenRecordPtr &record, 
				bool &isGood, 
				const bool correctOnly,
				const bool selectFromRecord,
				AosRVGReturnCode &rcode,
				OmnString &errmsg)
{
	// 
	// This parameter is not used as the key. It may generate a good
	// one or bad one. 
	//
	// If it is a good, it can be:
	// 1. Selected from the table
	// 2. Randomly generated a good one.
	//
	// If the generated value is selected from the table, 
	// 'record' holds that record.
	//
	if (mCorrectOnly || correctOnly || aos_next_pct(mCorrectPct))
	{
		// 
		// Check whether it should choose a value from a source table.
		//
		if (mSourceTable && mSourceFieldName != "")
		{
			if (selectValueFromSourceTable(value, isGood, rcode, errmsg))
			{
				return true;
			}

			isGood = false;
			return generateBadValue(value, data, record, rcode, errmsg);
		}

		// 
		// Determine whether to select from the table or randomly
		// generate one.
		//
		if (data && data->entries() &&
		    mFieldName != "" && aos_next_pct(mSelectFromTablePct))
		{
			isGood = false;
			AosGenRecordPtr rr = data->getRandomRecord();
			aos_assert_r(rr, mBadValue);
			aos_assert_r(rr->getStr(mFieldName, value), mBadValue); 
			isGood = true;
			rcode = eAosRVGRc_SelectFromTable;
			return true;
		}

		// 
		// Will randomly generated one.
		//
		isGood = true;
		value = genValue();
		rcode = eAosRVGRc_Success;
		return true;
	}

	isGood = false;
	return generateBadValue(value, data, record, rcode, errmsg);
}


OmnString
AosRVGStr::genValue()
{
	return mStrGen->nextStr();
}


OmnString 
AosRVGStr::genIncorrectValue(AosRVGReturnCode &rcode, OmnString &errmsg)
{
	errmsg = mArgName;
	if (mBadValueGen)
	{
		errmsg << "Generated from BadValueGen. ";
		rcode = eAosRVGRc_FromInvalidSet;
		return mBadValueGen->nextStr();
	}

	rcode = eAosRVGRc_Incorrect;
	errmsg << "From BadValue. ";
	return mBadValue;
}


bool 	
AosRVGStr::newRecordAdded(const AosGenRecordPtr &newRecord) const
{
	aos_assert_r(newRecord, false);

	if (mFieldName == "")
	{
		// 
		// This parm cannot set the record.
		//
		return true;
	}

	if (newRecord->isFieldMarked(mFieldName))
	{
		OmnAlarm << "Field has already been set: " 
			<< mFieldName << enderr;
		return false;
	}

	// 
	// Set the value
	//
	if (!newRecord->set(mFieldName, mLastValue))
	{
		OmnAlarm << "Failed to set value" << enderr;
		return false;
	}

	newRecord->markField(mFieldName);
	return true;
}


bool 	
AosRVGStr::recordDeleted(const AosGenRecordPtr &record) const
{
	return true;
}


OmnString
AosRVGStr::toString(const u32 indent) const
{
	OmnString str;
	OmnString ind(indent, ' ', true);

	str << ind << "Argument: " << mArgName << "\n"
		<< ind << "    KeyType:            " << AosGetRVGKeyTypeStr(mKeyType) << "\n"
		<< ind << "    FieldName:          " << mFieldName << "\n"
		<< ind << "    String Type:        " << AosStrType_enumToStr(mStrType) << "\n"
		<< ind << "    Minimum Len:        " << mMin << "\n"
		<< ind << "    Maximum Len:        " << mMax << "\n"
		<< ind << "    Bad Value:          " << mBadValue << "\n"
		<< ind << "    Last Value:         " << mLastValue << "\n"
		<< ind << "    CorrectPct:         " << mCorrectPct << "\n";
	return str;
}


bool 	
AosRVGStr::setRecord(const AosGenRecordPtr &record) const
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

	case eAosRVGKeyType_SingleKeyExist:
	case eAosRVGKeyType_SingleKeyNew:
	case eAosRVGKeyType_MultiKeyExist:
	case eAosRVGKeyType_MultiKeyNew:
		 mark = AosGenRecord::ePrimaryKey;
		 break;

	default:
		 OmnAlarm << "Unrecognized key type: " << mKeyType << enderr;
		 mark = AosGenRecord::eMarked;
	}
		 
	if (!record->set(mFieldName, mLastValue))
	{
		OmnAlarm << "Failed to set value" << enderr;
		return false;
	}

	return record->markField(mFieldName, mark);
}


bool
AosRVGStr::getCrtValue(int &value) const
{
	OmnAlarm << "Cannot call getInt in RVGStr" << enderr;
	return false;
}


bool
AosRVGStr::getCrtValue(u16 &value) const
{
	OmnAlarm << "Cannot call getCrtU16 in RVGStr" << enderr;
	return false;
}


bool
AosRVGStr::getCrtValue(u32 &value) const
{
	OmnAlarm << "Cannot call getCrtU16 in RVGStr" << enderr;
	return false;
}


bool
AosRVGStr::getCrtValue(char *&value) const
{
	value  = (char *)mLastValue.data();
	return true;
}


bool
AosRVGStr::getCrtValue(OmnString &value) const
{
	value  = mLastValue;
	return true;
}


bool
AosRVGStr::getCrtValueAsArg(OmnString &value, 
							 OmnString &decl, 
							 const u32 argIndex) const
{
	value = "";
	switch (mUsageType)
	{
	case eAosRVGUsageType_InputOnly:
		 if (mVarDecl != "")
		 {
		 	decl << mVarDecl << ";\n";
		 }
		 value << "\"" << mLastValue << "\"";
		 break;

	case eAosRVGUsageType_InputOutputThrRef:
		 if (mVarDecl != "")
		 {
		 	decl << mVarDecl << ";\n";
		 }
		 else
		 {
		 	decl << "OmnString _mArg_" << mArgName << "(\"" << mLastValue << "\"); ";
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
		 	decl << "OmnString _mArg_" << mArgName << "; ";
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
			 if (mDataType != "OmnString")
			 {
		 		decl << "char *_mArg_" << mArgName << "; ";
			 }
			 else
			 {
		 		decl << "OmnString _mArg_" << mArgName << "; ";
			 }
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
			 if (mDataType == "OmnString")
			 {
		 		decl << "char *_mArg_" << mArgName << "(\"" << mLastValue << "\"); ";
			 }
			 else
			 {
		 		decl << "OmnString _mArg_" << mArgName << "(\"" << mLastValue << "\"); ";
			 }
		 }
		 value << "&_mArg_" << mArgName;
		 break;

	default:
		 OmnAlarm << "Unrecognized UsageType: " << mUsageType << enderr;
		 return false;
	}

	return true;
}


// 
// The function will generate a good value. It will return false
// only when we tried enough time but the generated value is still
// in the table.
//
bool
AosRVGStr::getCorrectValue(
					OmnString &value, 
					const AosGenTablePtr &data, 
					AosRVGReturnCode &rcode)
{
	int tries = 5;
	while (tries--)
	{
		value = genValue();
		AosGenRecordPtr rr;
		if (!data || !data->valueExist(mFieldName, value, rr))
		{
			rcode = eAosRVGRc_Success;
			return true;
		}
	}

	return false;
}


bool
AosRVGStr::genValueNotInTable(
				OmnString &value, 
				const AosGenTablePtr &data, 
				AosRVGReturnCode &rcode, 
				OmnString &errmsg)
{
	u32 tries = 5;
	while (tries--)
	{
		value = genValue();
		AosGenRecordPtr rr;
		if (!data || !data->valueExist(mFieldName, value, rr))
		{
			errmsg = mArgName;
			errmsg << " not in the table. ";
			rcode = eAosRVGRc_NotFoundInTable;
			return true;
		}
	}

	return false;
}


void        
AosRVGStr::setBadValueGen(const AosRandomStrPtr &gen)
{
	mBadValueGen = gen;
}


OmnString 
AosRVGStr::getNextValueFuncCall() const
{
	return "nextStr";
}

