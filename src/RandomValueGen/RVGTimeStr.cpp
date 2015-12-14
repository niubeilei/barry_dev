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
// This class generates arguments based on a list of known values. 
// This argument has the following attributes:
// a. Field Index
//    This identifies the field in a generic data (AosGenTable/AosGenRecord).
// b. KeyType
//    This tells whether this argument serves as a key. If yes, whether
//    the key exists in a table is considered good (eExistGood) or the 
//    key does not exist in a table is considered good (eNewGood). 
// c. badValue
//    This is a string that is considered as a bad string. This is used
//    when this class generates a new string but encountered something 
//    wrong. 
// d. insertflag
//    It indicates when it generates a good argument, whether it needs to 
//    insert a new record into the table.
// e. values
//    This is a list of words, each serves as an allowed string. It is 
//    expressed in the form of: 
//       "word word word ... word"
// f. minLen
//    The minimum length of the valid strings.
// g. maxLen
//    The maximum length of the valid strings.
//
// Modification History:
// 07/03/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "Parms/RVGTimeStr.h"

#include "Alarm/Alarm.h"
#include "aos/aosReturnCode.h"
#include "alarm/Alarm.h"
#include "Random/RandomInteger.h"
#include "Random/RandomStr.h"
#include "Random/RandomUtil.h"
#include "Util/OmnNew.h"
#include "Util/GenRecord.h"
#include "Util/GenTable.h"

AosRVGTimeStr::AosRVGTimeStr(const AosCliCmdTorturerPtr &cmd)
:
AosRVG(cmd),
mSelectFromTablePct(eDefaultSelectFromTablePct),
mNotInTableWeight(eDefaultNotInTableWeight),
mInTableWeight(eDefaultInTableWeight),
mInvalidValueWeight(eDefaultInvalidWeight)
{
}


AosRVGTimeStr::~AosRVGTimeStr()
{
}


AosRVGTimeStrPtr
AosRVGTimeStr::createInstance(
				const AosCliCmdTorturerPtr &cmd,
				const OmnXmlItemPtr &def,
				const OmnString &cmdTag,
            	OmnVList<AosGenTablePtr> &tables)
{
	AosRVGTimeStrPtr inst = OmnNew AosRVGTimeStr(cmd);
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
AosRVGTimeStr::config(const OmnXmlItemPtr &def, 
					const OmnString &cmdTag,
					OmnVList<AosGenTablePtr> &tables)
{
	// 
	// 	<Parm>
	// 		<type>
	// 		<Name>
	// 		<DataType>
	// 		<FieldName>
	// 		<DataType>
	// 		<KeyType>
	// 		<BadValue>
	// 		<CorrectOnly>
	// 		<TableName>
	// 		<DependedTables>
	// 		<Format>
	// 		...
	// 	</Parm>
	//
	if (!AosRVG::config(def, cmdTag, tables))
	{
		OmnAlarm << "Failed to parse parm: " << def->toString() << enderr;
		return false;
	}

	mBadValue = def->getStr("BadValue", "***BAD***");
	mIsGood = createSelectors();
	mFormat = AosGetTimeStrEnum(def->getStr("Format", "HH:MM:SS"));
	if (mFormat == eAosTimeStrFormat_Invalid)
	{
		OmnAlarm << "Incorrect Time String format: " << def->toString() << enderr;
		return false;
	}

	return mIsGood;
}


bool
AosRVGTimeStr::createSelectors()
{
	switch (mKeyType)
	{
	case eAosRVGKeyType_SingleKeyExist:
		 aos_assert_r(mFieldName != "", false);
	     mInTableWeight = 0;
		 break;

	case eAosRVGKeyType_SingleKeyNew:
		 aos_assert_r(mFieldName != "", false);
	     if (!mOverrideFlag) mNotInTableWeight = 0;
		 break;

	case eAosRVGKeyType_MultiKeyNew:
	case eAosRVGKeyType_MultiKeyExist:
		 aos_assert_r(mFieldName != "", false);
	     mNotInTableWeight = 0;
	     mInTableWeight = 0;
		 break;

	case eAosRVGKeyType_NoKey:
		 mNotInTableWeight = 0;
		 mInTableWeight = 0;
		 break;

	default:
	     OmnAlarm << "Unrecognized key type: " << mKeyType << enderr;
	}

	int64_t index[10];
	u16 weights[10];
	int idx = 0;
	index[idx] = eGenerateNotInTable;	weights[idx++] = mNotInTableWeight;
	index[idx] = eGenerateInTable;		weights[idx++] = mInTableWeight;
	index[idx] = eGenerateInvalid;		weights[idx++] = mInvalidValueWeight;

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
AosRVGTimeStr::nextStr(OmnString &value, 
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
	// parameter. The generated can be good or bad, depending
	// on the probability. If the generated is good, 'isGood'
	// is set to true. If the value is selected from a record,
	// 'record' holds the pointer to that record. 
	// If the generated is bad, 'isGood' is set to false and
	// 'errmsg' contains the information about how the bad 
	// value is generated. If the value is bad because this
	// parameter serves as the key and the key type is either
	// 'eIntNew' or 'eStrNew', the value is bad because it
	// identifies a record. In this case, 'record' will hold 
	// that record.
	//
	bool ret;
	AosGenTablePtr data = (mTable)?mTable:table;
	switch (mKeyType)
	{
	case eAosRVGKeyType_SingleKeyExist:
		 // 
		 // This parameter is used as a key to 'data' and 
		 // if the value identifies a record in 'data', 
		 // that value is considered good. If the generated
		 // value is good, 'record' will hold the record.
		 //
		 ret = nextExist(mLastValue, data, record, isGood, correctOnly,
		 			selectFromRecord, rcode, errmsg);
		 break;

	case eAosRVGKeyType_SingleKeyNew:
		 ret = nextNew(mLastValue, data, record, selectFromRecord, 
		 			isGood, correctOnly, rcode, errmsg);
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
		 OmnAlarm << "Unrecognized argument type: " << mKeyType << enderr;
		 errmsg = "Invalid Key: ";
		 errmsg << mKeyType;
		 mLastValue = mBadValue;
		 return false;
	}

	if(ret)
	{
		value = mLastValue;
	}
	return ret;
}


bool
AosRVGTimeStr::nextExist(OmnString &value, 
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
	if (selectFromRecord)
	{
		aos_assert_r(mFieldName != "", false);
		aos_assert_r(record, false);
		aos_assert_r(record->getStr(mFieldName, value), false);
		isGood = true;
		rcode = eAosRVGRc_SelectFromRecord;
		return true;
	}

	isGood = false;
	rcode = eAosRVGRc_ProgramError;
	if (mCorrectOnly || correctOnly || aos_next_pct(mCorrectPct))
	{
		//
		// Will generate a correct string. We will select one from the table
		//
		if (data && data->entries() > 0)
		{
			AosGenRecordPtr rr = data->getRandomRecord();
			aos_assert_r(rr, false);
			aos_assert_r(rr->getStr(mFieldName, value), false);

			if (mDependedTables.entries() > 0 &&
				isUsedByOtherTables(value, isGood, rcode, errmsg))
			{
				isGood = false;
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
AosRVGTimeStr::nextNew(OmnString &value, 
					const AosGenTablePtr &data, 
					const AosGenRecordPtr &record,
					const bool selectFromRecord,
					bool &isGood, 
					const bool correctOnly,
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
	if (selectFromRecord)
	{
		aos_assert_r(mFieldName != "", false);
		aos_assert_r(record, false);
		aos_assert_r(record->getStr(mFieldName, value), false);
		rcode = eAosRVGRc_SelectFromRecord;
		isGood = true;
		return true;
	}

	isGood = true;
	if (mCorrectOnly || correctOnly || aos_next_pct(mCorrectPct))
	{
		//
		// Will generate a correct string 
		//
		int tries = (mCorrectOnly || correctOnly)?50:5;
		while (tries--)
		{
			value = genValidValue();
			AosGenRecordPtr rr;
			if (!data || !data->valueExist(mFieldName, value, rr))
			{
				return true;
			}
		}

		// 
		// We tried 'tries' number of times, but each time the generated
		// value is in the table. This should happen VERY rarely. 
		//
		errmsg = mArgName;
		errmsg << " exist in the table. ";
		rcode = eAosRVGRc_ExistInTable;
		isGood = false;
		return true;
	}

	// 
	// Otherwise, we will generate an incorrect string. 
	//
	isGood = false;
	return generateBadValue(value, data, record, rcode, errmsg);
	
}


bool
AosRVGTimeStr::nextNotKey(OmnString &value, 
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
	if (selectFromRecord && mFieldName != "")
	{
		aos_assert_r(record, false);
		aos_assert_r(record->getStr(mFieldName, value), false);
		rcode = eAosRVGRc_SelectFromRecord;
		isGood = true;
		return true;
	}

	if (mCorrectOnly || correctOnly || aos_next_pct(mCorrectPct))
	{
		// 
		// Determine whether to select from the table or randomly
		// generate one.
		//
		if (data && data->entries() &&
		    mFieldName != "" && aos_next_pct(mSelectFromTablePct))
		{
			isGood = false;
			AosGenRecordPtr rr = data->getRandomRecord();
			aos_assert_r(rr, false);
			aos_assert_r(rr->getStr(mFieldName, value), false); 
			isGood = true;
			rcode = eAosRVGRc_SelectFromTable;
			return true;
		}

		// 
		// Will randomly generated one.
		//
		isGood = true;
		value = genValidValue();
		rcode = eAosRVGRc_Success;
		return true;
	}

	isGood = false;
	return generateBadValue(value, data, record, rcode, errmsg);
}


bool
AosRVGTimeStr::generateBadValue(OmnString &value, 
						   const AosGenTablePtr &data, 
						   const AosGenRecordPtr &record, 
						   AosRVGReturnCode &rcode,
						   OmnString &errmsg)
{	
	int tries = 10;
	OmnString str;

	aos_assert_r(mBadValueSelector, mBadValue);

	int method;
	switch ((method = mBadValueSelector->nextInt()))
	{
	case eGenerateInTable:
		 if (data && data->entries() > 0)
		 {
		 	errmsg << mArgName << " is in the table. ";
		 	AosGenRecordPtr rr = data->getRandomRecord();
			aos_assert_r(rr, false);
			aos_assert_r(rr->getStr(mFieldName, value), false);
			rcode = eAosRVGRc_ExistInTable;
			return true;
		 }
		 goto syn_err;

	case eGenerateNotInTable:
		 while (tries--)
		 {
		 	value = genValidValue();
			AosGenRecordPtr rr;
			if (!data || !data->valueExist(mFieldName, str, rr))
			{
				(errmsg = mArgName) << " not in the table. ";
				rcode = eAosRVGRc_NotFoundInTable;
				return true;
			}
		 }
		 
syn_err:		 
	case eGenerateInvalid:
		 errmsg = mArgName;
		 errmsg << "is syntactically incorrect";
		 value = genInvalidValue();
		 rcode = eAosRVGRc_Incorrect;
		 return true;

	default:
		 errmsg = mArgName;
		 errmsg << " unrecognized method, syntactically incorrect ip is used. ";
		 OmnAlarm << "Unrecognized method: " << method << enderr;
		 value = genInvalidValue();
		 rcode = eAosRVGRc_ProgramError;
		 return false;
	}

	aos_should_never_come_here;

	return false; 
}


bool
AosRVGTimeStr::nextInt(
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
AosRVGTimeStr::nextInt(
				u32 &value,
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
AosRVGTimeStr::newRecordAdded(const AosGenRecordPtr &newRecord) const
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
AosRVGTimeStr::recordDeleted(const AosGenRecordPtr &record) const
{
	return true;
}


OmnString
AosRVGTimeStr::toString(const u32 indent) const
{
	OmnString str;
	OmnString ind(indent, ' ', true);

	str << ind << "Argument: " << mArgName << "\n"
		<< ind << "    KeyType:            " << AosGetRVGKeyTypeStr(mKeyType) << "\n"
		<< ind << "    FieldName:          " << mFieldName << "\n"
		<< ind << "    Bad Value:          " << mBadValue << "\n"
		<< ind << "    Last Value:         " << mLastValue << "\n"
		<< ind << "    CorrectPct:         " << mCorrectPct << "\n";
	return str;
}


bool 	
AosRVGTimeStr::setRecord(const AosGenRecordPtr &record) const
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


OmnString
AosRVGTimeStr::genValidValue()
{
	return aos_next_time_str(mFormat);
}


OmnString
AosRVGTimeStr::genInvalidValue()
{
	// 
	// It randomly generate a bad time string. 
	// It is a random non-space string 
	// Its lenghth is between 1 and 32
	//
	return aos_next_invalid_time_str(mMinInvalidStrLen, mMaxInvalidStrLen);
}


bool 	
AosRVGTimeStr::getCrtValue(int &value) const
{
	OmnAlarm << "Cannot get int from AosRVGTimeStr" << enderr;
	return false;
}


bool 	
AosRVGTimeStr::getCrtValue(u16 &value) const
{
	OmnAlarm << "Cannot get u16 from AosRVGTimeStr" << enderr;
	return false;
}


bool 	
AosRVGTimeStr::getCrtValue(u32 &value) const
{
	OmnAlarm << "Cannot get u32 from AosRVGTimeStr" << enderr;
	return false;
}


bool 	
AosRVGTimeStr::getCrtValue(char *&value) const
{
	value = (char *)mLastValue.data();
	return true;
}


bool 	
AosRVGTimeStr::getCrtValue(OmnString &value) const
{
	value = mLastValue;
	return true;
}


bool 	
AosRVGTimeStr::getCrtValueAsArg(OmnString &value, 
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
		 	decl << "OmnString arg" << argIndex << " = " 
		 		<< "\"" << mLastValue << "\"; ";
		 }
		 value << "arg" << argIndex;
		 break;

    case eAosRVGUsageType_OutputThrRef:
		 if (mVarDecl != "")
		 {
		 	decl << mVarDecl << ";\n";
		 }
		 else
		 {
		 	decl << "OmnString arg" << argIndex << "; ";
		 }
		 value << "arg" << argIndex;
		 break;

	case eAosRVGUsageType_OutputThrAddr:
		 if (mVarDecl != "")
		 {
		 	decl << mVarDecl << ";\n";
		 }
		 else
		 {
		 	decl << "OmnString arg" << argIndex << "; ";
		 }
		 value << "&arg" << argIndex;
		 break;

	case eAosRVGUsageType_InputOutputThrAddr:
		 if (mVarDecl != "")
		 {
		 	decl << mVarDecl << ";\n";
		 }
		 else
		 {
		 	decl << "OmnString arg" << argIndex << " = " 
		 		<< "\"" << mLastValue << "\"; ";
		 }
		 value << "&arg" << argIndex;
		 break;

	default:
		 OmnAlarm << "Unrecognized UsageType: " << mUsageType << enderr;
		 return false;
	}

	return true;
}


OmnString 
AosRVGTimeStr::getNextValueFuncCall() const
{
	return "nextStr";
}

