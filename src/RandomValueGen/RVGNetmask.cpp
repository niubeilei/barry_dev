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
// 02/18/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "Parms/RVGNetmask.h"

#include "Alarm/Alarm.h"
#include "aos/aosReturnCode.h"
#include "alarm/Alarm.h"
#include "Random/RandomInteger.h"
#include "Random/RandomStr.h"
#include "Random/RandomUtil.h"
#include "Util/OmnNew.h"
#include "Util/GenRecord.h"
#include "Util/GenTable.h"

#include <netinet/in.h>



AosRVGNetmask::AosRVGNetmask(const AosCliCmdTorturerPtr &cmd)
:
AosRVG(cmd),
mSelectFromTablePct(eDefaultSelectFromTablePct),
mNotInTableWeight(eDefaultNotInTableWeight),
mInTableWeight(eDefaultInTableWeight),
mInvalidNetmaskWeight(eDefaultInvalidWeight)
{
}


AosRVGNetmask::~AosRVGNetmask()
{
}


AosRVGNetmaskPtr
AosRVGNetmask::createInstance(
			const AosCliCmdTorturerPtr &cmd,
			const OmnXmlItemPtr &def,
			const OmnString &cmdTag, 
            OmnVList<AosGenTablePtr> &tables)
{
	AosRVGNetmaskPtr inst = OmnNew AosRVGNetmask(cmd);
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
AosRVGNetmask::config(const OmnXmlItemPtr &def, 
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
	// 		<Min>
	// 		<Max>
	// 		<CorrectOnly>
	// 		<BadValue>
	// 		<TableName>
	// 		<DependedTables>
	// 	</Parm>
	//
	if (!AosRVG::config(def, cmdTag, tables))
	{
		OmnAlarm << "Failed to parse parm: " << def->toString() << enderr;
		return false;
	}

	mBadValue = def->getStr("BadValue", "1.1.1.1");
	mMin = def->getInt("Min", 0);
	mMax = def->getInt("Max", 32);
	mIsGood = createSelectors();
	return mIsGood;
}


bool
AosRVGNetmask::createSelectors()
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

	int64_t index[3];
	u16 weights[3];
	int idx = 0;
	index[idx] = eGenerateNotInTableNetmask;	weights[idx++] = mNotInTableWeight;
	index[idx] = eGenerateInTableNetmask;		weights[idx++] = mInTableWeight;
	index[idx] = eGenerateInvalidNetmask;		weights[idx++] = mInvalidNetmaskWeight;

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
AosRVGNetmask::nextStr(OmnString &value, 
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
		 rcode = eAosRVGRc_ProgramError;
		 errmsg = "Invalid Key";
		 mLastValue = mBadValue;
		 return false;
	}

	if(ret == true)
	{
		value = mLastValue;
	}
	return ret;
}


bool
AosRVGNetmask::nextExist(OmnString &value, 
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
	isGood = false;
	if (selectFromRecord)
	{
		aos_assert_r(mFieldName != "", false);
		aos_assert_r(record, false);
		aos_assert_r(record->getStr(mFieldName, value), false);
		isGood = true;
		rcode = eAosRVGRc_SelectFromRecord;
		return true;
	}

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

			rcode = eAosRVGRc_SelectFromTable;
			isGood = true;
			return true;
		}
	}

	// 
	// Otherwise, we will generate an incorrect string. 
	// Refer to the comments at the beginning of this file for
	// how to generate an incorrect value.
	//
	isGood = false;
	return generateBadNetmask(value, data, record, rcode, errmsg);	
}	

bool
AosRVGNetmask::nextNew(OmnString &value, 
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
	if (selectFromRecord)
	{
		aos_assert_r(mFieldName != "", false);
		aos_assert_r(record, false);
		aos_assert_r(record->getStr(mFieldName, value), false);
		isGood = true;
		rcode = eAosRVGRc_SelectFromRecord;
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
			value = genValidNetmask();
			AosGenRecordPtr rr;			
			if (!data || !data->valueExist(mFieldName, value, rr))
			{
				rcode = eAosRVGRc_Success;
				return true;
			}
		}

		// 
		// We tried 'tries' number of times, but each time the generated
		// value is in the table. This should happen VERY rarely. 
		//
		rcode = eAosRVGRc_ExistInTable;
		errmsg = mArgName;
		errmsg << " exist in the table. ";
		isGood = false;
		return true;
	}

	// 
	// Otherwise, we will generate an incorrect string. 
	//
	isGood = false;
	return generateBadNetmask(value, data, record, rcode, errmsg);
	
}

bool
AosRVGNetmask::nextSubkeyNew(OmnString &value, 
					const AosGenTablePtr &data, 
					const AosGenRecordPtr &record,
					bool &isGood, 
					const bool correctOnly,
					const bool selectFromRecord,
					AosRVGReturnCode &rcode,
					OmnString &errmsg)
{
OmnAlarm << "Not implemented yet" << enderr;
isGood = false;
rcode = eAosRVGRc_ProgramError;
return mBadValue;
}

bool
AosRVGNetmask::nextNotKey(OmnString &value, 
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
		isGood = true;
		rcode = eAosRVGRc_SelectFromRecord;
		return true;
	}

	isGood = false;
	rcode = eAosRVGRc_ProgramError;
	if (mCorrectOnly || correctOnly || aos_next_pct(mCorrectPct))
	{
		// 
		// Determine whether to select from the table or randomly
		// generate one.
		//
		if (data && data->entries() &&
		    mFieldName != "" && aos_next_pct(mSelectFromTablePct))
		{
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
		rcode = eAosRVGRc_Success;
		value = genValidNetmask();
		return true;
	}

	isGood = false;
	return generateBadNetmask(value, data, record, rcode, errmsg);
}


bool
AosRVGNetmask::generateBadNetmask(OmnString &value, 
							 	   const AosGenTablePtr &data, 
							 	   const AosGenRecordPtr &record, 
								   AosRVGReturnCode &rcode,
							 	   OmnString &errmsg)
{	
	int tries = 10;
	OmnString str;

	rcode = eAosRVGRc_ProgramError;
	aos_assert_r(mBadValueSelector, mBadValue);

	int method;
	switch ((method = mBadValueSelector->nextInt()))
	{
	case eGenerateInTableNetmask:
		 if (data && data->entries() > 0)
		 {
		 	rcode = eAosRVGRc_ExistInTable;
		 	errmsg = mArgName;
			errmsg << " is in the table. ";
		 	AosGenRecordPtr rr = data->getRandomRecord();
			aos_assert_r(rr, false);
			aos_assert_r(rr->getStr(mFieldName, value), false);
			return true;
		 }
		 goto syn_err;

	case eGenerateNotInTableNetmask:
		 while (tries--)
		 {
		 	value = genValidNetmask();
			AosGenRecordPtr rr;
			if (!data || !data->valueExist(mFieldName, str, rr))
			{
				rcode = eAosRVGRc_NotFoundInTable;
				errmsg = mArgName;
				errmsg << " not in the table. ";
				return true;
			}
		 }
		 
syn_err:		 
	case eGenerateInvalidNetmask:
		 rcode = eAosRVGRc_Incorrect;
		 errmsg = mArgName;
		 errmsg << " is syntactically incorrect";
		 value = genInvalidNetmask();
		 return true;

	default:
		 rcode = eAosRVGRc_Incorrect;
		 errmsg = mArgName;
		 errmsg << " unrecognized method, syntactically incorrect ip is used. ";
		 OmnAlarm << "Unrecognized method: " << method << enderr;
		 value = genInvalidNetmask();
		 return false;
	}

	rcode = eAosRVGRc_ProgramError;
	aos_should_never_come_here;

	return false; 
}


bool
AosRVGNetmask::nextInt(
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
AosRVGNetmask::nextInt(
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
AosRVGNetmask::newRecordAdded(const AosGenRecordPtr &newRecord) const
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
	if (!newRecord->set(mFieldName, mLastValue.toString()))
	{
		OmnAlarm << "Failed to set value" << enderr;
		return false;
	}

	newRecord->markField(mFieldName);
	return true;
}


bool 	
AosRVGNetmask::recordDeleted(const AosGenRecordPtr &record) const
{
	return true;
}


OmnString
AosRVGNetmask::toString(const u32 indent) const
{
	OmnString str;
	OmnString ind(indent, ' ', true);

	str << ind << "Argument: " << mArgName << "\n"
		<< ind << "    KeyType:            " << AosGetRVGKeyTypeStr(mKeyType) << "\n"
		<< ind << "    FieldName:          " << mFieldName << "\n"
		<< ind << "    Bad Value:          " << mBadValue << "\n"
		<< ind << "    Last Value:         " << mLastValue.toString() << "\n"
		<< ind << "    CorrectPct:         " << mCorrectPct << "\n";
	return str;
}


bool 	
AosRVGNetmask::setRecord(const AosGenRecordPtr &record) const
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
		 
	if (!record->set(mFieldName, mLastValue.toString()))
	{
		OmnAlarm << "Failed to set value" << enderr;
		return false;
	}

	return record->markField(mFieldName, mark);
}

OmnString
AosRVGNetmask::genValidNetmask()
{
	//
	// generate a valid netmask
	//
	
	int num = aos_next_int(mMin, mMax);
	OmnIpAddr netmask = htonl(0xffffffff << num);
	
	return netmask.toString();	
}


OmnString
AosRVGNetmask::genInvalidNetmask()
{
	// 
	// It randomly generates a bad mask. A correct net mask
	// is a four byte string in the form of "1111...00000". 
	// An incorrect net mask is an integer that is not in 
	// the form of "1111...0000".
	//
	int tries = 5;
	while (tries--)
	{
		OmnIpAddr addr(aos_next_u32(1, UINT_MAX));
		if (!OmnIpAddr::isValidMask(addr))
		{
			return addr.toString();
		}
	}

	//
	// This should RARELY, RARELY happen
	//
	return OmnIpAddr(12345).toString();
}


bool 	
AosRVGNetmask::getCrtValue(int &value) const
{
	OmnAlarm << "Cannot get int from AosRVGNetmask" << enderr;
	return false;
}


bool 	
AosRVGNetmask::getCrtValue(u16 &value) const
{
	OmnAlarm << "Cannot get u16 from AosRVGNetmask" << enderr;
	return false;
}


bool
AosRVGNetmask::getCrtValue(u32 &value) const
{
	OmnIpAddr addr(mLastValue);
	value = addr.getIPv4();
	return true;
}


bool 	
AosRVGNetmask::getCrtValue(char *&value) const
{
	value = (char *)mLastValue.data();
	return true;
}


bool 	
AosRVGNetmask::getCrtValue(OmnString &value) const
{
	value = mLastValue;
	return true;
}


bool 	
AosRVGNetmask::getCrtValue(OmnIpAddr &value) const
{
	value.set(mLastValue);
	return true;
}


bool 	
AosRVGNetmask::getCrtValueAsArg(OmnString &value, 
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
		 if (isStrType(mDataType))
		 {
		 	value << "\"" << mLastValue << "\"";
		 }
		 else if (isVerbose())
		 {
			 value << "OmnIpAddr(\"" << mLastValue << "\")";
		 }
		 else
		 {
		 	value << OmnIpAddr(mLastValue).getIPv4();
		 }
		 break;

	case eAosRVGUsageType_InputOutputThrRef:
		 if (mVarDecl != "")
		 {
		 	decl << mVarDecl << ";\n";
		 }
		 else
		 {
		 	decl << "OmnIpAddr arg" << argIndex << "(\"" << mLastValue << "\"); ";
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
		 	decl << "OmnIpAddr arg" << argIndex << "; ";
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
		 	decl << "OmnIpAddr arg" << argIndex << "; ";
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
		 	decl << "OmnIpAddr arg" << argIndex << "(\"" << mLastValue << "\"); ";
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
AosRVGNetmask::getNextValueFuncCall() const
{
	return "nextStr";
}

