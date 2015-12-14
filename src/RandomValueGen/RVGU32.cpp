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
// This class generates an IP address argument. An IP address argument
// has the following attributes:
// a. Field Index
//    This identifies the field in a generic data (AosGenTable/AosGenRecord).
// b. KeyType
//    This tells whether this argument serves as a key. If yes, whether
//    the key exists in a table is considered good (eExistGood) or the 
//    key does not exist in a table is considered good (eNewGood). 
// c. addr/netmask
//    These two defines an address space. An address in the address space
//    is considered good. 
// d. badAddr
//    This is an IP address that is considered as a bad address. This is used
//    when this class generates a new address but encountered something 
//    wrong. 
// e. insertflag
//    It indicates when it generates a good argument, whether it needs to 
//    insert a new record into the table.
//
// Modification History:
// 02/18/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "Parms/RVGU32.h"

#include "Alarm/Alarm.h"
#include "Random/RandomUtil.h"
#include "Util/OmnNew.h"
#include "Util/GenRecord.h"
#include "Util/GenTable.h"
#include "XmlParser/XmlParser.h"
#include "XmlParser/XmlItem.h"

#if 0
AosRVGU32::AosRVGU32(const OmnXmlItemPtr &def)
{
	if (!config(def))
	{
		OmnAlarm << "Failed to create RVGU32: " 
			<< def->toString() << enderr;
		return;
	}
}


AosRVGU32::AosRVGU32(const OmnString &name, 
					 const u32 fieldIndex, 
					 const AosRVG::KeyType keyType, 
					 const u32 min, 
					 const u32 max,
					 const bool insertFlag, 
					 const u32 badValue)
:
AosRVG(name, fieldIndex, keyType), 
mMin(min),
mMax(max),
mInsertFlag(insertFlag),
mBadValue(badValue)
{
}


AosRVGU32::~AosRVGU32()
{
}


bool
AosRVGU32::config(const OmnXmlItemPtr &item)
{
	// 
	//	<Parm>
	//		<Type>				Should be "u32"
	//		<Name>
	//		<DataType>
	//		<DataType>
	//		<FieldIndex>		Default: -1
	//		<KeyType>			Default: NoKey
	//		<Min>				Default: 0
	//		<Max>				Default: eAosMaxU32
	//		<InsertFlag>		Default: false
	//		<BadValue>			Default: 0
	//	</Parm>
	//
	mName = item->getStr("Name", "");
	mDataType = item->getStr("DataType", "");
	if (mName == "")
	{
		OmnAlarm << "Missing Parameter Name: " << item->toString() << enderr;
		return false;
	}

	mFieldIndex = item->getInt("FieldIndex", -1);
	OmnString keyType = item->getStr("KeyType", "NotKey");
	mKeyType = convertKeyType(keyType);
	if (mKeyType == eInvalidKeyType)
	{
		OmnAlarm << "Invalid key type: " << item->toString() << enderr;
		return false;
	}

	mMin = (u32)item->getInt("Min", 0);
	mMax = (u32)item->getInt("Max", eMaxU32);
	mInsertFlag = item->getBool("InsertFlag", false);
	mBadValue = (u32)item->getInt("BadValue", 0);

	return true;
}


OmnString
AosRVGU32::nextValue(const AosGenTablePtr &data, 
					  bool &isGood,
					  OmnString &errmsg)
{
	OmnString str;
	switch (mKeyType)
	{
	case eIntExist:
	case eStrExist:
		 str = nextExist(data, isGood, errmsg);
		 break;

	case eIntNew:
	case eStrNew:
		 str = nextNew(data, isGood, mInsertFlag, errmsg);
		 break;

	case eNotKey:
		 str = nextNotKey(data, isGood, errmsg);
		 break;

	default:
		 OmnAlarm << "Unrecognized argument type: " 
			 << mKeyType << enderr;
		 str = mBadValue;
		 break;
	}

	return str;
}


OmnString
AosRVGU32::nextValue(const AosGenRecordPtr &record, 
					  bool &isGood,
					  OmnString &errmsg)
{
	OmnString str;
	switch (mKeyType)
	{
	case eIntExist:
	case eStrExist:
	case eIntNew:
	case eStrNew:
		 // 
		 // This is not allowed since this argument serves as a key. 
		 // The input must be a table, not a record.
		 // 
		 OmnAlarm << "Trying to generate an IP address, where the address"
			 << " serves as the key but passed is not a table" << enderr;
		 return mBadValue;

	case eNotKey:
		 str = nextNoKey(record, isGood, errmsg);
		 break;

	default:
		 OmnAlarm << "Unrecognized argument type: " 
			 << mArgType << enderr;
		 str = mBadValue;
		 break;
	}

	return str;
}


OmnString
AosRVGU32::nextNotKey(const AosGenRecordPtr &record, 
					 bool &isGood, 
					 OmnString &errmsg)
{
	// 
	// It generates a u32 value. It will first determine whether to generate
	// a syntactically correct or incorrect u32. If it is a syntactically
	// incorrect u32, 'isGood' is set to false. 
	// 
	// If it is a syntactically correct u32, it determines whether to use
	// the u32 contained in the record, whose percentage is determined 
	// by 'mUseExistValuePct'. 
	// 

	// 
	// Determine whether to use the existing value
	//
	if (aos_next_pct(mUseExistValuePct) && record)
	{
		int value;
		if (record->getInt(mFieldIndex, value))
		{
			isGood = true;
			OmnString str;
			str << value;
			return str;
		}
	}

	return generateU32(isGood, errmsg);
}


OmnString
AosRVGU32::nextExist(const AosGenTablePtr &data, 
					  bool &isGood, 
					  OmnString &errmsg)
{
	// 
	// The argument serves as the key and a value is good if the value
	// is in 'data'. 
	// 
	OmnString str;
	if ((u32)aos_next_pct(mCorrectPct) && data->entries())
	{
		//
		// Will generate a correct u32 from the list. 
		//
		u32 index;
		bool found;
		u32 value = data->getRandomU32(mFieldIndex, found, index);
		if (!found)
		{
			isGood = false;
			errmsg = "Failed to retrieve string";
			return mBadValue;
		}

		isGood = true;
		OmnString str;
		str << value;
		return str;
	}

	// 
	// Otherwise, we will generate an incorrect u32. 
	// The u32 can be incorrect in one of the following
	// ways:
	// 1. Value not in 'data'
	// 2. Value is not in [mMin, mMax]
	// 3. Negative if allowed
	//
	u32 value = generateU32(isGood, errmsg);

	// 
	// Check whether the value is in the list
	//
	if (isGood)
	{
		if (data->valueExist(mFieldIndex, value))
		{
			isGood = true;
		}
		else
		{
			isGood = false;
			errmsg = "Value not in the list";
		}
	}

	return value;
}


OmnString
AosRVGU32::nextNew(const AosGenTablePtr &data, 
				   bool &isGood, 
				   const bool insert, 
				   OmnString &errmsg)
{
	// 
	// 'isGood' is true if the generated value is not in 
	// 'data', it is syntactically correct, and it is in [mMin, mMax]. 
	// Otherwise, 'isGood' is set to false. 
	//
	// If the genearted value is good, 
	// and if 'insert' is true, it will insert a new record
	// into 'data'.
	//
	// Error Conditions:
	// 1. Value is already in 'data',
	// 2. Value is not in [mMin, mMax]
	// 3. Negative if allowed
	// 4. There are too many records in 'data'
	//
	OmnString str;
	if (aos_next_pct(mCorrectPct) && data)
	{
		//
		// Will generate a correct value 
		//
		u32 value = OmnRandom::nextU32(mMin, mMax);
		isGood = !data->valueExist(mFieldIndex, str);

		if (!isGood)
		{
			errmsg = "Value already exists";
		}

		if (isGood && insert)
		{
			if (data->exceedMax())
			{
				// 
				// There are already too many entries in the table. 
				//
				errmsg = "Exceed the maximum";
				isGood = false;
			}
			else
			{
				AosGenRecordPtr record = data->appendRecord();
				record->setValue(mFieldIndex, value);	
			}
		}

		OmnString str;
		str << value;
		return str;
	}

	// 
	// Otherwise, we will generate an incorrect address.
	// The address can be wrong in one of the following
	// ways:
	// 1. Value is already in 'data',
	// 2. Value is not in [mMin, mMax]
	// 3. Negative if allowed
	// 4. There are too many records in 'data'
	//

	u32 value = generateU32(isGood, errmsg);
	if (!isGood)
	{
		OmnString str;
		str << value;
		return str;
	}

	// 
	// We will select one from the list. 
	//
	if (!data || data->entries() <= 0)
	{
		OmnString str;
		str << value;
		return str;
	}

	isGood = false;
	errmsg = "Value already exists";
	u32 index;
	bool found;
	value = data->getRandomU32(mFieldIndex, found, index);
	if (!found)
	{
		// 
		// This should never happen
		//
		OmnAlarm << "Program error" << enderr;
		errmsg = "Program error";
		if (mMin > 0) return mMin - 1;
		if (mMax < 0xffffffff) return mMax+1;
		return 0;
	}

	OmnString str;
	str << value;
	return str;
}


OmnString
AosRVGU32::generateU32(bool &isGood, OmnString &errmsg)
{
	// 
	// Determine whether to generate a syntactically good one or bad one. 
	// 'mBadAddrPercent' controls the possibility of generating a syntactically
	// bad address. 
	// 
	// If it generates a syntactically good value, it will check whether
	// the value is allowed. If not, 'isGood' is set to false. 
	// 
	// Error conditions are:
	// 1. Value is already in 'data',
	// 2. Value is not in [mMin, mMax]
	// 3. Negative if allowed
	// 4. There are too many records in 'data'

	int index;
	isGood = false;
	switch (AosRandomInteger::next(
		eValueInData, eValueInDataPct, 
		eValueNotInRange, eValueNotInRangePct, 
		eNegativeValue, negativeValuePct)
	{
	case eValueInData:
		 // 
		 // Value is already in 'data'
		 // 
		 if (data)
		 {
			 errmsg = "Value already in data";
			 return data->getRandomStr(mFieldIndex, "-1", index);
		 }
		 break;

	case eValueNotInRange:
		 // 
		 // Value is not in [mMin, mMax]
		 //
		 switch (AosRandomInteger::next(
			eTooSmall, eTooSmallPct, 
			eTooLarge, eTooLargePct)
		 {
		 case eTooSmall:
			  if (mMin > 0)
			  {
				  errmsg = "Value too small";
				  return OmnString(OmnRandom::nextU32(0, mMin-1));
			  }
			  break;

		 case eTooBig:
			  if (mMax < (u32)eMaxU32)
			  {
				  errmsg = "Value too big";
				  if (mMax <= (u32)eMaxU32 / 2)
				  {
					  return OmnString(OmnRandom::nextU32(mMax+1, mMax + mMax));
				  }
				  else
				  {
					  return OmnString(OmnRandom::nextU32(mMax+1, eMaxU32);
				  }
			  }
			  break;
		 }
		 break;

	case eNegativeValue:
		 // 
		 // Negative, if allowed
		 //
		 if (mNegativeAllowed)
		 {
			 errmsg = "Value is negative";
			 return OmnString(OmnRandom::nextInt(mMinnegative, -1);
		 }
		 break;

	default:
		 OmnAlarm << "Program error" << enderr;
		 return mBadValue;
	}

	isGood = true;
	return OmnString(OmnRandom::nextU32(mMin, mMax));
}


OmnString
AosRVGU32::selectValue(const AosGenTablePtr &data)
{
	if (!data || data->entries() <= 0)
	{
		OmnAlarm << "Program error: table is empty or null" << enderr;
		return mBadAddr;
	}

	int index;
	OmnString str = data->getRandomStr(mFieldIndex, "", index);
	if (str != "")
	{
		return str;
	}

	OmnAlarm << "Failed to retrieve data: " << mFieldIndex << enderr;
	return mBadAddr;
}


OmnString 
AosRVGU32::getNextValueFuncCall() const
{
	return "nextInt";
}

#endif

