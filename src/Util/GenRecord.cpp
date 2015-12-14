////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: GenericData.h
// Description:
//
// Modification History:
//	11/15/2006: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "Util/GenRecord.h"

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "Debug/Except.h"
#include "Util/OmnNew.h"
#include "Util/GenTable.h"


AosGenRecord::AosGenRecord(
		const AosGenTablePtr &table, 
		const OmnString &name, 
		const u32 numIntegers, 
		const u32 numU32s, 
		const u32 numStrings, 
		const u32 numSubtables)
:
mName(name),
mIntegers(0),
mU32s(0),
mStrs(0),
mSubtables(0),
mNumInt(numIntegers), 
mNumU32(numU32s), 
mNumStr(numStrings),
mNumSubtables(numSubtables), 
mFlag(0),
mMarks(0),
mTable(table)
{
	if (numIntegers > eMaxIntegers)
	{
		OmnAlarm << "Too many integer fields: " 
			<< (int) numIntegers << enderr;
		OmnExcept e(__FILE__, __LINE__, 
				OmnErrId::eAlarmOutOfBound, "Too many integers");
		throw e;
	};

	if (numU32s > eMaxU32s)
	{
		OmnAlarm << "Too many u32 fields: " 
			<< numU32s << enderr;
		OmnExcept e(__FILE__, __LINE__, 
				OmnErrId::eAlarmOutOfBound, "Too many integers");
		throw e;
	};

	if (numStrings > eMaxStrings)
	{
		OmnAlarm << "Too many string fields: " 
			<< (int)numStrings << enderr;
		OmnExcept e(__FILE__, __LINE__, 
				OmnErrId::eAlarmOutOfBound, "Too many strings");
		throw e;
	}

	if (numSubtables > eMaxSubtables)
	{
		OmnAlarm << "Too many subtable fields: " 
			<< numSubtables << enderr;
		OmnExcept e(__FILE__, __LINE__, 
				OmnErrId::eAlarmOutOfBound, "Too many subtables");
		throw e;
	}

	if (!createFields())
	{
		OmnAlarm << "Run out of memory" << enderr;
		OmnExcept e(__FILE__, __LINE__, 
				OmnErrId::eAlarmOutOfMemory, "Run out of memory");
		throw e;
	}
}


bool
AosGenRecord::createFields()
{
	if (mNumInt > 0)
	{
		mIntegers = OmnNew int[mNumInt];
		if (!mIntegers)
		{
			return false;
		}

		memset(mIntegers, 0, sizeof(int) * mNumInt);
	}

	if (mNumU32 > 0)
	{
		mU32s = OmnNew u32[mNumInt];
		if (!mU32s)
		{
			return false;
		}

		memset(mU32s, 0, sizeof(u32) * mNumU32);
	}

	if (mNumStr > 0)
	{
		mStrs = OmnNew OmnString[mNumStr];
		if (!mStrs)
		{
			return false;
		}
	}

	if (mNumSubtables > 0)
	{
		mSubtables = OmnNew AosGenTablePtr[mNumSubtables];
		if (!mSubtables)
		{
			return false;
		}
	}

	return true;
}


// 
// Copy constructor
//
AosGenRecord::AosGenRecord(const AosGenRecord &rhs)
:
mIntegers(0),
mU32s(0),
mStrs(0),
mSubtables(0),
mNumInt(rhs.mNumInt), 
mNumU32(rhs.mNumU32), 
mNumStr(rhs.mNumStr),
mNumSubtables(rhs.mNumSubtables), 
mMarks(0), 
mTable(rhs.mTable)
{
	if (!createFields())
	{
		OmnAlarm << "Run out of memory" << enderr;
		OmnExcept e(__FILE__, __LINE__, 
				OmnErrId::eAlarmOutOfMemory, "Run out of memory");
		throw e;
	}
}


AosGenRecord::~AosGenRecord()
{
	OmnDelete [] mIntegers; 
	OmnDelete [] mU32s; 
	OmnDelete [] mStrs;
	OmnDelete [] mSubtables;
	OmnDelete [] mMarks;

	mIntegers = 0;
	mStrs = 0;
	mSubtables = 0;
	mMarks = 0;
}


// 
// It sets the field 'index'. If 'index' does not identify a value field, error is
// returned. 
//
bool		
AosGenRecord::set(const u32 index, const OmnString &value)
{
	if (index > mNumStr)
	{
		OmnAlarm << "Index out of range: " << (int)index << enderr;
		return false;
	}

	mStrs[index] = value;
	return true;
}


bool		
AosGenRecord::set(const OmnString &fieldName, const OmnString &value)
{
	AosGenFieldType type;
	u32 index;
	if (!getFieldIndex(index, fieldName, type)) return false;
	switch (type)
	{
	case eAosGenFieldType_Int:
		 return set(index, atoi(value.data())); 

	case eAosGenFieldType_U32:
		 return set(index, (u32)atoi(value.data()));

	case eAosGenFieldType_Str:
		 return set(index, value);

	case eAosGenFieldType_Table:
		 OmnAlarm << "Trying to set a subtable: " << fieldName << enderr;
		 return false;

	default:
		 OmnAlarm << "Unrecognized field type: " << type << enderr;
		 return false;
	}

	return set(index, value);
}


bool		
AosGenRecord::set(const OmnString &fname, const int &value)
{
	AosGenFieldType type;
	u32 index;
	OmnString str;
	if (!getFieldIndex(index, fname, type)) return false;
	switch (type)
	{
	case eAosGenFieldType_Int:
		 return set(index, value); 

	case eAosGenFieldType_U32:
		 return set(index, (u32)value);

	case eAosGenFieldType_Str:
		 str << value;
		 return set(index, str);

	case eAosGenFieldType_Table:
		 OmnAlarm << "Trying to set a subtable: " << fname << enderr;
		 return false;

	default:
		 OmnAlarm << "Unrecognized field type: " << type << enderr;
		 return false;
	}

	OmnShouldNeverComeToThisPoint;
	return false;
}


bool		
AosGenRecord::set(const u32 index, const int &value)
{
	if (index > mNumInt)
	{
		OmnAlarm << "Index out of range: " << (int)index << enderr;
		return false;
	}

	mIntegers[index] = value;
	return true;
}


bool		
AosGenRecord::set(const OmnString &fname, const u32 &value)
{
	AosGenFieldType type;
	OmnString str;
	u32 index;
	if (!getFieldIndex(index, fname, type)) return false;
	switch (type)
	{
	case eAosGenFieldType_Int:
		 return set(index, (int)value); 

	case eAosGenFieldType_U32:
		 return set(index, value);

	case eAosGenFieldType_Str:
		 str << value;
		 return set(index, str);

	case eAosGenFieldType_Table:
		 OmnAlarm << "Trying to set a subtable: " << fname << enderr;
		 return false;

	default:
		 OmnAlarm << "Unrecognized field type: " << type << enderr;
		 return false;
	}

	OmnShouldNeverComeToThisPoint;
	return false;
}


bool		
AosGenRecord::set(const u32 index, const u32 &value)
{
	if (index > mNumU32)
	{
		OmnAlarm << "Index out of range: " << index << enderr;
		return false;
	}

	mU32s[index] = value;
	return true;
}


bool		
AosGenRecord::set(const OmnString &fname, const AosGenTablePtr &table)
{
	AosGenFieldType type;
	u32 index;
	if (!getFieldIndex(index, fname, type)) return false;
	if (type != eAosGenFieldType_Table)
	{
		OmnAlarm << "Type mismatch: " << fname << enderr;
		return false;
	}
	return set(index, table);
}


bool		
AosGenRecord::set(const u32 index, const AosGenTablePtr &table)
{
	if (index > mNumSubtables)
	{
		OmnAlarm << "Index out of range: " 
			<< index << ". Size: " << mNumSubtables << enderr;
		return false;
	}

	mSubtables[index] = table;
	return true;
}


bool
AosGenRecord::getValue(const OmnString &fname, OmnString &str) const
{
	AosGenFieldType type;
	u32 index;
	if (!getFieldIndex(index, fname, type)) return false;

	switch (type)
	{
	case eAosGenFieldType_Str:
		 return getStr(index, str);

	case eAosGenFieldType_Int:
		 {
		 	int value;
			if (!getInt(index, value))
			{
				OmnAlarm << "Failed to retrieve value: " << fname << ":" << index << enderr;
				return false;
			}

			str = "";
			str << value;
			return true;
		 }

	case eAosGenFieldType_U32:
		 {
		 	u32 value;
			if (!getU32(index, value))
			{
				OmnAlarm << "Failed to retrieve value: " << fname << ":" << index << enderr;
				return false;
			}

			str = "";
			str << value;
			return true;
		 }

	default:
		 OmnAlarm << "Type mismatch: " << fname << enderr;
		 return false;
	}

	return false;
}


bool
AosGenRecord::getStr(const OmnString &fname, OmnString &str) const
{
	AosGenFieldType type;
	u32 index;
	if (!getFieldIndex(index, fname, type)) 
	{
		OmnAlarm << "Field name unrecognized: " << fname << enderr;
		return false;
	}
	if (type != eAosGenFieldType_Str)
	{
		OmnAlarm << "Type mismatch: Expecting string type but" 
			<< " actual type is: " << AosGenFieldType_toStr(type)
			<< ". Field name: " << fname << enderr;
		return false;
	}
	return getStr(index, str);
}


bool
AosGenRecord::getStr(const u32 index, OmnString &str) const
{
	if (index > mNumStr)
	{
		OmnAlarm << "Index out of range: " << (int)index << enderr;
		return false; 
	}

	aos_assert_r(index < mNumStr, false);
	str = mStrs[index];
	return true;
}


bool
AosGenRecord::getInt(const OmnString &fname, int &value) const
{
	AosGenFieldType type;
	u32 index;
	if (!getFieldIndex(index, fname, type)) return false;
	if (type != eAosGenFieldType_Int)
	{
		// 
		// Try to see whether it is a string
		//
		if (type == eAosGenFieldType_Str)
		{
			OmnString str;
			if (!getStr(index, str))
			{
				OmnAlarm << "Failed to retrieve the value for: "
					<< fname << enderr;
				return false;
			}

			if (str.isDigitStr())
			{
				value = atoi(str.data());
				return true;
			}

			OmnAlarm << "Not a digit string: " << str << enderr;
		}

		OmnAlarm << "Type mismatch: " << fname 
			<< ". Expecting integer but actual: " 
			<< type << enderr;
		return false;
	}
	return getInt(index, value);
}


bool
AosGenRecord::getInt(const u32 index, int &value) const
{
	if (index > mNumInt)
	{
		OmnAlarm << "Index out of range: " << (int)index << enderr;
		return false; 
	}

	value = mIntegers[index];
	return true;
}


bool
AosGenRecord::getU32(const OmnString &fname, u32 &value) const
{
	AosGenFieldType type;
	u32 index;
	if (!getFieldIndex(index, fname, type)) return false;
	if (type != eAosGenFieldType_U32)
	{
		OmnAlarm << "Type mismatch: " << fname << enderr;
		return false;
	}
	return getU32(index, value);
}


bool
AosGenRecord::getU32(const u32 index, u32 &value) const
{
	if (index > mNumU32)
	{
		OmnAlarm << "Index out of range: " << index << enderr;
		return false; 
	}

	value = mU32s[index];
	return true;
}


AosGenTablePtr
AosGenRecord::getTable(const OmnString &fname)
{
	AosGenFieldType type;
	u32 index;
	if (!getFieldIndex(index, fname, type)) return 0;
	if (type != eAosGenFieldType_Table)
	{
		OmnAlarm << "Type mismatch: " << fname << enderr;
		return NULL;
	}
	return getTable(index);
}


AosGenTablePtr
AosGenRecord::getTable(const u32 index)
{
	if (index > mNumSubtables)
	{
		OmnAlarm << "Index out of range: " 
			<< index << ". Size: " << mNumSubtables << enderr;
		return NULL; 
	}

	return mSubtables[index];
}


bool
AosGenRecord::isSame(const OmnString &fname, const OmnString &value)
{
	AosGenFieldType type;
	u32 index;
	if (!getFieldIndex(index, fname, type)) return false;
	if (type != eAosGenFieldType_Str)
	{
		OmnAlarm << "Type mismatch: " << fname << enderr;
		return false;
	}
	return isSame(index, value);
}


bool
AosGenRecord::isSame(const u32 index, const OmnString &value)
{
	if (index > mNumStr)
	{
		OmnAlarm << "Index out of range: " 
			<< index << ". Size: " << mNumStr << enderr;
		return false; 
	}

	return (mStrs[index] == value);
}


bool
AosGenRecord::isSame(const OmnString &fname, const int value)
{
	AosGenFieldType type;
	u32 index;
	if (!getFieldIndex(index, fname, type)) return false;
	if (type != eAosGenFieldType_Int)
	{
		OmnAlarm << "Type mismatch: " << fname << enderr;
		return false;
	}
	return isSame(index, value);
}


bool
AosGenRecord::isSame(const u32 index, const int value)
{
	if (index > mNumInt)
	{
		OmnAlarm << "Index out of range: " 
			<< index << ". Size: " << mNumInt << enderr;
		return false; 
	}

	return (mIntegers[index] == value);
}


bool
AosGenRecord::isSame(const OmnString &fname, const u32 value)
{
	AosGenFieldType type;
	u32 index;
	if (!getFieldIndex(index, fname, type)) return false;
	if (type != eAosGenFieldType_U32)
	{
		OmnAlarm << "Type mismatch: " << fname << enderr;
		return false;
	}
	return isSame(index, value);
}


bool
AosGenRecord::isSame(const u32 index, const u32 value)
{
	if (index > mNumU32)
	{
		OmnAlarm << "Index out of range: " 
			<< index << ". Size: " << mNumU32<< enderr;
		return false; 
	}

	return (mU32s[index] == value);
}


bool
AosGenRecord::isSame(const OmnString &fname, const AosGenTablePtr &table)
{
	AosGenFieldType type;
	u32 index;
	if (!getFieldIndex(index, fname, type)) return false;
	if (type != eAosGenFieldType_Table)
	{
		OmnAlarm << "Type mismatch: " << fname << enderr;
		return false;
	}
	return isSame(index, table);
}


bool
AosGenRecord::isSame(const u32 index, const AosGenTablePtr &table)
{
	// 
	// It returns true if 'table' is the same pointer as 'mSubtables[index]'.
	// Note that we are comparing the pointers, not the contents.
	//
	if (index > mNumSubtables)
	{
		OmnAlarm << "Index out of range: " 
			<< index << ". Size: " << mNumSubtables << enderr;
		return false; 
	}

	return (mSubtables[index] == table);
}


bool
AosGenRecord::isSame(
		const int *intv, const u16 *intIndex, const u16 numInt, 
		const OmnString *strv, const u16 *strIndex, const u16 numStr) const
{
	u16 i;
	u16 idx;

	for (i=0; i<numInt; i++)
	{
		idx = intIndex[i];
		if (idx >= mNumInt)
		{
			OmnAlarm << "Index out of integer bound: " 
				<< idx << ". Size: " << mNumInt << enderr;
			return false;
		}

		if (intv[i] != mIntegers[idx])
		{
			return false;
		}
	}

	for (i=0; i<numStr; i++)
	{
		idx = strIndex[i];
		if (idx >= mNumStr)
		{
			OmnAlarm << "Index out of string bound: " 
				<< idx << ". Size: " << mNumStr << enderr;
			return false;
		}

		if (strv[i] != mStrs[idx])
		{
			return false;
		}
	}

	/*
	for (i=0; i<numtb; i++)
	{
		idx = tableIndex[i];
		if (idx >= mNumSubtables)
		{
			OmnAlarm << "Index out of subtable bound: " 
				<< idx << ". Size: " << mNumSubtables << enderr;
			return false;
		}

		if (tablev[i] != mSubtables[idx])
		{
			return false;
		}
	}
	*/

	return true;
}

	int		   * 	mIntegers;
	u32 	   * 	mU32s;
	OmnString  * 	mStrs;
	AosGenTablePtr*	mSubtables;
	u32 		 	mNumInt; 
	u32 		 	mNumU32; 
	u32 		 	mNumStr;
	u32				mNumSubtables;
	u32				mFlag;


OmnString
AosGenRecord::toString() const
{
	OmnString str = "AosGenericRecord: ";
	str << mName << ": "
		<< mNumInt << ":"
		<< mNumStr << ":"
		<< mNumU32 << ":"
		<< mNumSubtables << ":";
	
	OmnDynArray<OmnString, eArrayInitSize, eArrayIncSize, eArrayMaxSize> names;
	mTable->getFieldNames(names);
	for (int i=0; i<names.entries(); i++)
	{
		OmnString fname = names[i];
		OmnString value;
		if (!getValue(fname, value))
		{
			OmnAlarm << "Failed to retrieve value for: " << fname << enderr;
			return str;
		}

		str << "\n    " << fname << ": " << value;
	}

	return str;
}


OmnString
AosGenRecord::getContents() const
{
	OmnString str;
	if (mNumInt > 0)
	{
		str << "Integers: ";
		for (u32 i=0; i<mNumInt-1; i++)
		{
			str << mIntegers[i] << ", ";
		}
		str << mIntegers[mNumInt-1];
	}

	if (mNumStr > 0)
	{
		str << "  Strings: ";
		for (u32 i=0; i<mNumStr-1; i++)
		{
			str << mStrs[i] << ", ";
		}

		str << mStrs[mNumStr-1];
	}

	if (mNumU32 > 0)
	{
		str << "  U32: ";
		for (u32 i=0; i<mNumU32-1; i++)
		{
			str << mU32s[i] << ", ";
		}
		str << mU32s[mNumU32-1];
	}

	return str;
}


void
AosGenRecord::resetMarks()
{
	u32 size = mNumInt + mNumU32 + mNumStr + mNumSubtables;
	if (size == 0) return;
	if (!mMarks)
	{
		mMarks = OmnNew FieldMark[size];
		if (!mMarks)
		{
			OmnAlarm << "Failed to allocate memory!" << enderr;
			return;
		}
	}

	memset(mMarks, 0, sizeof(FieldMark) * size);
	return;
}


AosGenRecord::FieldMark
AosGenRecord::getFieldMark(const AosGenFieldType type, const OmnString &fieldName)
{
	AosGenFieldType tt;
	u32 index;
	if (!getFieldIndex(index, fieldName, tt)) return eUndefined;
	if (tt != type)
	{
		OmnAlarm << "Type mismatch: " << fieldName << enderr;
		return eUndefined;
	}
	return getFieldMark(type, index);
}


AosGenRecord::FieldMark
AosGenRecord::getFieldMark(const AosGenFieldType type, const u32 fieldIndex)
{
	if (!mMarks) resetMarks();
	aos_assert_r(mMarks, eUndefined);

	switch (type)
	{
	case eAosGenFieldType_Int:
		 aos_assert_r(fieldIndex < mNumInt, eUndefined);
		 return mMarks[fieldIndex];

	case eAosGenFieldType_U32:
		 aos_assert_r(fieldIndex < mNumU32, eUndefined);
		 return mMarks[mNumInt + fieldIndex];

	case eAosGenFieldType_Str:
		 aos_assert_r(fieldIndex < mNumStr, eUndefined);
		 return mMarks[mNumInt + mNumU32 + fieldIndex];

	case eAosGenFieldType_Table:
		 aos_assert_r(fieldIndex < mNumSubtables, eUndefined);
		 return mMarks[mNumInt + mNumU32 + mNumStr + fieldIndex];

	default:
		 OmnAlarm << "Unrecognized field type: " << type << enderr;
		 return eUndefined;
	}

	aos_should_never_come_here;
	return eUndefined;
}


bool
AosGenRecord::isFieldMarked(const OmnString &fieldName)
{
	AosGenFieldType tt;
	u32 index;
	if (!getFieldIndex(index, fieldName, tt)) return false;
	return isFieldMarked(tt, index);
}


bool
AosGenRecord::isFieldMarked(const AosGenFieldType type, const u32 fieldIndex)
{
	if (!mMarks) return false;
	aos_assert_r(mMarks, false);

	switch (type)
	{
	case eAosGenFieldType_Int:
		 aos_assert_r(fieldIndex < mNumInt, false);
		 return mMarks[fieldIndex] != eNotMarked &&
		 	 	mMarks[fieldIndex] != eUndefined;

	case eAosGenFieldType_U32:
		 aos_assert_r(fieldIndex < mNumU32, false);
		 return mMarks[mNumInt + fieldIndex] != eNotMarked &&
		 	 	mMarks[mNumInt + fieldIndex] != eUndefined;

	case eAosGenFieldType_Str:
		 aos_assert_r(fieldIndex < mNumStr, false);
		 return mMarks[mNumInt + mNumU32 + fieldIndex] != eNotMarked &&
		 	 	mMarks[mNumInt + mNumU32 + fieldIndex] != eUndefined;

	case eAosGenFieldType_Table:
		 aos_assert_r(fieldIndex < mNumSubtables, false);
		 return mMarks[mNumInt + mNumU32 + mNumStr + fieldIndex] != eNotMarked &&
		 	 	mMarks[mNumInt + mNumU32 + mNumStr + fieldIndex] != eUndefined;

	default:
		 OmnAlarm << "Unrecognized field type: " << type << enderr;
		 return false;
	}

	aos_should_never_come_here;
	return false;
}


bool
AosGenRecord::markField(const OmnString &fieldName, 
						const bool force,
						const FieldMark mark) 
{
	AosGenFieldType type;
	u32 index;
	if (!getFieldIndex(index, fieldName, type)) return false;
	return markField(type, index, force, mark);
}

bool
AosGenRecord::markField(const AosGenFieldType type, 
						const u32 fieldIndex, 
						const bool force,
						const FieldMark mark) 
{
	if (!mMarks) resetMarks();
	aos_assert_r(mMarks, false);

	switch (type)
	{
	case eAosGenFieldType_Int:
		 aos_assert_r(fieldIndex < mNumInt, false);
		 if (!force)
		 {
		 	aos_assert_r(mMarks[fieldIndex] == eNotMarked, false);
		 }
		 mMarks[fieldIndex] = mark;
		 return true;

	case eAosGenFieldType_U32:
		 aos_assert_r(fieldIndex < mNumU32, false);
		 if (!force)
		 {
		 	aos_assert_r(mMarks[mNumInt + fieldIndex] == eNotMarked, false);
		 }
		 mMarks[mNumInt + fieldIndex] = mark;
		 return true;

	case eAosGenFieldType_Str:
		 aos_assert_r(fieldIndex < mNumStr, false);
		 if (!force)
		 {
		 	aos_assert_r(mMarks[mNumInt + mNumU32 + fieldIndex] == eNotMarked, false);
		 }
		 mMarks[mNumInt + mNumU32 + fieldIndex] = mark;
		 return true;

	case eAosGenFieldType_Table:
		 aos_assert_r(fieldIndex < mNumSubtables, false);
		 if (!force)
		 {
		 	aos_assert_r(mMarks[mNumInt + mNumU32 + mNumStr + fieldIndex] == eNotMarked, false);
		 }
		 mMarks[mNumInt + mNumU32 + mNumStr + fieldIndex] = mark;
		 return true;

	default:
		 OmnAlarm << "Unrecognized field type: " << type << enderr;
		 return false;
	}

	aos_should_never_come_here;
	return false;
}


void
AosGenRecord::removeMarks()
{
	OmnDelete [] mMarks;
	mMarks = 0;
}


bool
AosGenRecord::recordMatch(const AosGenRecordPtr &record) const
{
	// 
	// This function assumes the record fields have been marked 
	// to indicate which fields are key fields. This record 
	// matches 'record' if and only if all the key fields are 
	// the same.
	//
	aos_assert_r(record, false);
	aos_assert_r(record->mNumInt == mNumInt, false);
	aos_assert_r(record->mNumU32 == mNumU32, false);
	aos_assert_r(record->mNumStr == mNumStr, false);

	for (u32 i=0; i<mNumInt; i++)
	{
		switch (record->getFieldMark(eAosGenFieldType_Int, i))
		{
		case ePrimaryKey:
			 return (record->mIntegers[i] == mIntegers[i]);

		case eSubkey:
			 if (record->mIntegers[i] != mIntegers[i]) return false;
			 break;

		case eNotMarked:
		case eUndefined:
		case eMarked:
			 break;
		}
	}

	for (u32 i=0; i<mNumU32; i++)
	{
		switch (record->getFieldMark(eAosGenFieldType_U32, i))
		{
		case ePrimaryKey:
			 return (record->mU32s[i] == mU32s[i]);

		case eSubkey:
			 if (record->mU32s[i] != mU32s[i]) return false;
			 break;

		case eNotMarked:
		case eUndefined:
		case eMarked:
			 break;
		}
	}

	for (u32 i=0; i<mNumStr; i++)
	{
		switch (record->getFieldMark(eAosGenFieldType_Str, i))
		{
		case ePrimaryKey:
			 return (record->mStrs[i] == mStrs[i]);

		case eSubkey:
			 if (record->mStrs[i] != mStrs[i]) return false;
			 break;

		case eNotMarked:
		case eUndefined:
		case eMarked:
			 break;
		}
	}

	return true;
}


bool 
AosGenRecord::getFieldIndex(u32 &index, 
							const OmnString &name, 
							AosGenFieldType &type) const
{
	if (!mTable)
	{
		OmnAlarm << "Table is not set" << enderr;
		return false;
	}

	int ii = mTable->getFieldIndex(name, type);
	if (ii < 0)
	{
		OmnAlarm << "Field name not recognized" << enderr;
		return false;
	}															

	index = (u32)ii;
	return true;
}


bool
AosGenRecord::fieldMatch(const OmnString &fieldName, 
						 AosGenFieldType fieldType, 
						 const AosGenRecordPtr &record)
{
	AosGenFieldType tt;	
	u32 index;
	if (!getFieldIndex(index, fieldName, tt)) return false;
	if (tt != fieldType)
	{
		OmnAlarm << "Field type mismatch: " << tt << ":" << fieldType << enderr;
		return false;
	}

	switch (fieldType)
	{
	case eAosGenFieldType_Int:
	 	 {
		     int value1, value2;
			 if (!getInt(index, value1)) return false;
			 if (!record->getInt(index, value2)) return false;
			 return value1 == value2;
		 }

	case eAosGenFieldType_Str:
	 	 {
		     OmnString value1, value2;
			 if (!getStr(index, value1)) return false;
			 if (!record->getStr(index, value2)) return false;
			 return value1 == value2;
		 }

	case eAosGenFieldType_U32:
	 	 {
		     u32 value1, value2;
			 if (!getU32(index, value1)) return false;
			 if (!record->getU32(index, value2)) return false;
			 return value1 == value2;
		 }

	case eAosGenFieldType_Table:
	     OmnAlarm << "Table cannot be used as a primary key: " << fieldName << enderr;
		 return false;

	default:
		 OmnAlarm << "Unrecognized field type: " << fieldType << enderr;
		 return false;
	}

	return false;
}


bool		
AosGenRecord::isSame(const OmnDynArray<OmnString, eArrayInitSize, eArrayIncSize, eArrayMaxSize> &fields, 
					 const OmnDynArray<OmnString, eArrayInitSize, eArrayIncSize, eArrayMaxSize> &values)
{
	if (fields.entries() != values.entries())
	{
		OmnAlarm << "Entries not match: " << fields.entries() << ":" << values.entries() << enderr;
		return false;
	}

	OmnString value;
	for (int i=0; i<fields.entries(); i++)
	{
		if (!getValue(fields[i], value) || value != values[i]) return false;

	}

	return true;
}



