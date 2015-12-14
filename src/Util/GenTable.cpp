////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: GenericDataContainer.h
// Description:
// mPrimaryKeyIndex: if it is -1, the table has no primary key. Otherwise, 
// it is the field index of the field that serves as the primary key.
//
// Modification History:
//	11/15/2006: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "Util/GenTable.h"

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "Random/RandomUtil.h"
#include "Util/GenRecord.h"
#include "Util/StrParser.h"
#include "XmlParser/XmlItem.h"


AosGenTable::AosGenTable(const OmnXmlItemPtr &def)
{
	if (!config(def))
	{
		OmnExcept e(__FILE__, __LINE__, OmnErrId::eAlarmProgramError, 
			"Program Error");
		return;
	}
}


AosGenTable::AosGenTable(
		const u32 numInt, 
		const u32 numU32, 
		const u32 numStr, 
		const u32 numTables,
		const u32 max) 
:
mMax(max)
{
	AosGenTablePtr thisPtr(this, false);
	mDefaultRecord = OmnNew AosGenRecord(thisPtr, "Default", numInt, numU32, numStr, numTables);
	if (!mDefaultRecord)
	{
		OmnExcept e(__FILE__, __LINE__,
			OmnErrId::eAlarmOutOfMemory, "Run out of memory");
		return;
	}
}


AosGenTable::~AosGenTable()
{
}


bool
AosGenTable::config(const OmnXmlItemPtr &def)
{
	// 
	// 	<TableDef>
	// 		<Name>
	// 		<MaxEntries>
	// 		<KeyFields>name1, name2, ...</KeyFields>
	// 		<Fields>
	// 			<Field>
	// 				<Type>
	// 				<Name>
	// 			</Field>
	// 			...
	// 		</Fields>
	// 	</Table>
	//
	mNumInt = 0;
	mNumStr = 0;
	mNumU32 = 0;
	mNumTables = 0;
	mFieldDef.clear();
	mName = def->getStr("Name", "NoName");
	mMax = def->getInt("MaxEntries", 0);
	OmnXmlItemPtr fields = def->getItem("Fields");
	OmnString keynames = def->getStr("KeyFields", "");
	if (!fields)
	{
		OmnAlarm << "Missing fields: " << def->toString() << enderr;
		return false;
	}

	fields->reset();
	while (fields->hasMore())
	{
		OmnXmlItemPtr item = fields->next();
		OmnString type = item->getStr("Type", "");
		if (type == "")
		{
			OmnAlarm << "Missing type def: " << item->toString() << enderr;
			return false;
		}

		OmnString name = item->getStr("Name", "");
		if (name == "")
		{
			OmnAlarm << "Missing name: " << item->toString() << enderr;
			return false;
		}
	
		Field field;
		field.mName = name;
		type.toLower();
		if (type == "int")
		{
			field.mType = eAosGenFieldType_Int;
			field.mIndex = mNumInt++;
		}
		else if (type == "string")
		{
			field.mType = eAosGenFieldType_Str;
			field.mIndex = mNumStr++;
		}
		else if (type == "u32")
		{
			field.mType = eAosGenFieldType_U32;
			field.mIndex = mNumU32++;
		}
		else if (type == "table")
		{
			field.mType = eAosGenFieldType_Table;
			field.mIndex = mNumTables++;
		}
		else
		{
			OmnAlarm << "Unrecognized field type: " << type
				<< ", " << item->toString() << enderr;
			return false;
		}
		mFieldDef.append(field);
	}

	AosGenTablePtr thisPtr(this, false);
	mDefaultRecord = OmnNew AosGenRecord(thisPtr, "Default", mNumInt, mNumU32, mNumStr, mNumTables);
	if (!mDefaultRecord)
	{
		OmnAlarm << "Memory error" << enderr;
		return false;
	}

	// 
	// Create key fields
	//
	if (keynames != "")
	{
		// 
		// Add the keys.
		//
		OmnStrParser1 parser(keynames);
		while (parser.hasMore())
		{
			OmnString name = parser.nextEngWord();
			if (!parser.expectNext(',', true))
			{
				if (parser.hasMore())
				{
					OmnAlarm << "Expecting comma: " << keynames << enderr;
					parser.dump();
					return false;
				}
			}

			AosGenFieldType fieldType = getFieldType(name);
			if (fieldType == eAosGenFieldType_Invalid)
			{
				OmnAlarm << "Unrecognized field: '" << name 
					<< "' in '" << keynames 
					<< "'. The table definition: " 
					<< def->toString() << enderr;
				return false;
			}

			mKeyFields.append(name);
			mKeyType.append(fieldType);
		}
	}
			
	return true;
}


bool
AosGenTable::appendRecord(
		const int *intv, const u16 *intIndex, const u16 numInt, 
		const OmnString *strv, const u16 *strIndex, const u16 numStr) 
{
	if (mMax > 0 && (u32)mRecords.entries() >= mMax)
	{
		// 
		// There are too many records. 
		//
		return false;
	}

	if (!mDefaultRecord)
	{
		OmnAlarm << "DefaultRecord is null" << enderr;
		return false;
	}

	AosGenRecordPtr record = OmnNew AosGenRecord(*mDefaultRecord);
	if (!record)
	{
		OmnAlarm << "Run out of memory" << enderr;
		return false;
	}

	int i;
	for (i=0; i<numInt; i++)
	{
		if (!record->set(intIndex[i], intv[i]))
		{
			OmnAlarm << "Failed to set integer value: " 
				<< i << ". Index: " << intIndex[i] << enderr;
			return false;
		}
	}

	for (i=0; i<numStr; i++)
	{
		if (!record->set(strIndex[i], strv[i]))
		{
			OmnAlarm << "Failed to set string value: " 
				<< i << ". Index: " << strIndex[i] << enderr;
			return false;
		}
	}

	return true;
}


AosGenRecordPtr
AosGenTable::createRecord()
{
	// 
	// It will create a new record. The new record is created
	// based on mDefaultRecord. If mDefaultREcord is not set, 
	// it is an error. 
	// If successful, it will return the new record. 
	//
	// Errors:
	// 1. mDefaultRecord is not set
	// 2. Run out of memory
	//
	if (!mDefaultRecord)
	{
		OmnAlarm << "DefaultRecord is null" << enderr;
		return 0;
	}

	AosGenRecordPtr record = OmnNew AosGenRecord(*mDefaultRecord);
	if (!record)
	{
		OmnAlarm << "Run out of memory" << enderr;
		return 0;
	}

	return record;
}


AosGenRecordPtr
AosGenTable::appendRecord()
{
	// 
	// It will create a new record and append it to the table. 
	// If successful, it will return the new record. 
	//
	// Errors:
	// 1. Exceeds the maximum
	// 2. mDefaultRecord is not set
	// 3. Run out of memory
	//
	if (mMax > 0 && (u32)mRecords.entries() >= mMax)
	{
		// 
		// There are too many records. 
		//
		return 0;
	}

	AosGenRecordPtr record = createRecord();
	aos_assert_r(record, 0);
	mRecords.append(record);
	return record;
}


bool	
AosGenTable::appendRecord(const AosGenRecordPtr &record)
{
	if (mMax > 0 && (u32)mRecords.entries() >= mMax)
	{
		// 
		// Too many records. 
		//
		return false;
	}

	if (mKeyFields.entries() > 0)
	{
		//
		// The table has key.  Check whether the record is already 
		// in the table 
		//
		AosGenRecordPtr d = getRecordByKey(record);
		if (d)
		{
			// 
			// It is already in the list
			//
			OmnAlarm << "Record is already in the table: " 
				<< record->getName() << enderr;

			OmnTrace << toString() << endl;
			OmnTrace << record->toString() << endl;
			return false;
		}
	}

	mRecords.append(record);
	return true;
}


bool	
AosGenTable::removeRecord(const OmnString &name)
{
	for (int i=0; i<mRecords.entries(); i++)
	{
		AosGenRecordPtr d = mRecords[i];
		if (d->getName() == name)
		{
			mRecords.remove(i);
			return true;
		}
	}

	// 
	// Didn't find it. 
	//
	return false;
}


AosGenTablePtr 
AosGenTable::getSubtable(const u32 fieldIndex, const OmnString &key, const u32 subtableIndex)
{
	AosGenRecordPtr record = getRecord(fieldIndex, key);
	if (!record)
	{
		// 
		// The key does not identify a record. 
		//
		return 0;
	}

	return record->getTable(subtableIndex);
}


AosGenTablePtr 
AosGenTable::getSubtable(const OmnString &name, const OmnString &key, const u32 subtableIndex)
{
	for (int i=0; i<mFieldDef.entries(); i++)
	{
		if (mFieldDef[i].mName == name)
		{
			return getSubtable(mFieldDef[i].mIndex, key, subtableIndex);
		}
	}

	return 0;
}


AosGenTablePtr
AosGenTable::getRandomSubtable(const u32 subtableIndex)
{
	AosGenRecordPtr record = getRandomRecord();
	if (!record)
	{
		if (mRecords.entries() > 0)
		{
			// 
			// This should never happen.
			//
			OmnAlarm << "Failed to retrieve record" << enderr;
		}

		return 0;
	}

	return record->getTable(subtableIndex);
}


int
AosGenTable::getFieldIndex(const OmnString &name, AosGenFieldType &type) const
{
	// 
	// This function retrieves the index of the field whose name is 'name'.
	// If not found, -1 is returned.
	//
	for (int i=0; i<mFieldDef.entries(); i++)
	{
		if (mFieldDef[i].mName == name)
		{
			type = mFieldDef[i].mType;
			return mFieldDef[i].mIndex;
		}
	}

	OmnAlarm << "Field not found: " << name << enderr;
	return -1;
}


AosGenRecordPtr	
AosGenTable::getRecord(const u32 fieldIndex, const OmnString &key)
{
	// 
	// This function retrieves the record whose String 'fieldIndex' field matches
	// the value 'key'. 
	//
	for (int i=0; i<mRecords.entries(); i++)
	{
		if (mRecords[i]->isSame(fieldIndex, key))
		{
			return mRecords[i];
		}
	}

	return 0;
}


AosGenRecordPtr	
AosGenTable::getRecord(const u32 fieldIndex, const int key)
{
	// 
	// This function retrieves the record whose String 'fieldIndex' field matches
	// the value 'key'. 
	//
	for (int i=0; i<mRecords.entries(); i++)
	{
		if (mRecords[i]->isSame(fieldIndex, key))
		{
			return mRecords[i];
		}
	}

	return 0;
}


AosGenRecordPtr	
AosGenTable::getRecord(const u32 fieldIndex, const u32 key)
{
	// 
	// This function retrieves the record whose String 'fieldIndex' field matches
	// the value 'key'. 
	//
	for (int i=0; i<mRecords.entries(); i++)
	{
		if (mRecords[i]->isSame(fieldIndex, key))
		{
			return mRecords[i];
		}
	}

	return 0;
}


AosGenRecordPtr	
AosGenTable::getRecord(const OmnString &name, const OmnString &key)
{
	AosGenFieldType type;
	int index = getFieldIndex(name, type);
	if (index < 0) return 0;

	switch (type)
	{
	case eAosGenFieldType_Int:
		 {
		 	int value = atoi(key.data());
			return getRecord((u32)index, value);
		 }

	case eAosGenFieldType_U32:
		 {
		 	u32 value = (u32)atoi(key.data());
			return getRecord((u32)index, value);
		 }

	case eAosGenFieldType_Str:
		 return getRecord((u32)index, key);

	default:
		 OmnAlarm << "Failed to get record: " << name << ". " << key << enderr;
		 return 0;
	}
}


AosGenRecordPtr	
AosGenTable::getRecord(const u32 index)
{
	if (index >= (u32)mRecords.entries())
	{
		OmnAlarm << "Accessing DynArray out of bound: " 
			<< index << ". Size: " << mRecords.entries() << enderr;
		return 0;
	}

	return mRecords[index];
}


int
AosGenTable::entries() const
{
	return mRecords.entries();
}


OmnString
AosGenTable::getRandomStr(const u32 fieldIndex, 
		const OmnString &dflt, 
		u32 &index)
{
	if (mRecords.entries() <= 0)
	{
		return dflt;
	}

	index = (u32)OmnRandom::nextInt1(0, mRecords.entries()-1);
	AosGenRecordPtr d = mRecords[index];
	OmnString str;
	if (d->getStr(fieldIndex, str))
	{
		return str;
	}

	OmnAlarm << "Failed to retrieve record: " << index 
		<< ". MemberIndex: " << fieldIndex
		<< ". Size: " << mRecords.entries() << enderr;
	return  dflt;
}


OmnString
AosGenTable::getRandomStr(const OmnString &name, 
		const OmnString &dflt, 
		u32 &index)
{
	AosGenFieldType type;
	int ii = getFieldIndex(name, type);
	if (ii < 0) 
	{
		OmnAlarm << "Field not found: " << name << enderr;
		return dflt;
	}

	if (type != eAosGenFieldType_Str)
	{
		OmnAlarm << "Type mismatch: " << type << enderr;
		return dflt;
	}

	return getRandomStr((u32)ii, dflt, index);
}


int
AosGenTable::getRandomInt(const u32 fieldIndex, const int dflt, u32 &index)
{
	AosGenRecordPtr record = getRandomRecord(index);
	if (!record)
	{
		return dflt;
	}

	int value;
	if (!record->getInt(fieldIndex, value))
	{
		OmnAlarm << "Failed to retrieve field: " << fieldIndex << enderr;
		return dflt;
	}

	return value;
}


int
AosGenTable::getRandomInt(const OmnString &name, const int dflt, u32 &index)
{
	AosGenFieldType type;
	int ii = getFieldIndex(name, type);
	if (ii < 0) 
	{
		OmnAlarm << "Field not found: " << name << enderr;
		return dflt;
	}

	if (type != eAosGenFieldType_Int)
	{
		OmnAlarm << "Type mismatch: " << type << enderr;
		return dflt;
	}

	return getRandomInt((u32)ii, dflt, index);
}


u32
AosGenTable::getRandomU32(const u32 fieldIndex, bool &found, u32 &index)
{
	AosGenRecordPtr record = getRandomRecord(index);
	if (!record)
	{
		found = false;
		return 0;
	}

	u32 value;
	if (!record->getU32(fieldIndex, value))
	{
		OmnAlarm << "Failed to retrieve field: " << fieldIndex << enderr;
		found = false;
		return 0;
	}

	found = true;
	return value;
}


u32
AosGenTable::getRandomU32(const OmnString &name, bool &found, u32 &index)
{
	AosGenFieldType type;
	int ii = getFieldIndex(name, type);
	if (ii < 0) 
	{
		OmnAlarm << "Failed to find the field: " << name << enderr;
		found = false;
		return 0;
	}

	if (type != eAosGenFieldType_U32)
	{
		OmnAlarm << "Type mismatch: " << name << enderr;
		found = false;
		return 0;
	}

	return getRandomU32((u32)ii, found, index);
}


AosGenRecordPtr
AosGenTable::getRandomRecord()
{
	if (mRecords.entries() <= 0)
	{
		return 0;
	}

	u32 index = (u32)OmnRandom::nextInt1(0, mRecords.entries()-1);
	return mRecords[index];
}


AosGenRecordPtr
AosGenTable::getRandomRecord(u32 &index)
{
	if (mRecords.entries() <= 0)
	{
		return 0;
	}

	index = (u32)OmnRandom::nextInt1(0, mRecords.entries()-1);
	return mRecords[index];
}


OmnString
AosGenTable::getStr(const u32 recordIndex, 
		const u32 fieldIndex, 
		const OmnString &dflt)
{
	// 
	// Retrieve the 'fieldIndex'-th string field of the 'recordIndex'-th
	// record. If not found, return 'dflt'.
	//
	if (recordIndex >= (u32)mRecords.entries())
	{
		// 
		// This is an error. 
		//
		OmnAlarm << "Index out of bound: " << recordIndex 
			<< ". Size: " << mRecords.entries() << enderr;
		return dflt;
	}

	AosGenRecordPtr d = mRecords[recordIndex];
	OmnString str;
	if (d->getStr(fieldIndex, str))
	{
		return str;
	}

	OmnAlarm << "Failed to retrieve record: " << recordIndex 
		<< ". MemberIndex: " << fieldIndex
		<< ". Size: " << mRecords.entries() << enderr;
	return dflt;
}


OmnString
AosGenTable::getStr(const u32 recordIndex, 
		const OmnString &name, 
		const OmnString &dflt)
{
	AosGenFieldType type;
	int index = getFieldIndex(name, type);
	if (index < 0) 
	{
		OmnAlarm << "Failed to find the name: " << name << enderr;
		return dflt;
	}

	if (type != eAosGenFieldType_Str)
	{
		OmnAlarm << "Type mismatch: " << name << enderr;
		return dflt;
	}

	return getStr(recordIndex, (u32)index, dflt);
}


int
AosGenTable::getInt(const u32 recordIndex, 
		const u32 fieldIndex, 
		const int dflt)
{
	// 
	// Retrieve the 'fieldIndex'-th integer field of the 'recordIndex'-th
	// record. If not found, return 'dflt'.
	//
	if (recordIndex >= (u32)mRecords.entries())
	{
		// 
		// This is an error. 
		//
		OmnAlarm << "Index out of bound: " << recordIndex 
			<< ". Size: " << mRecords.entries() << enderr;
		return dflt;
	}

	AosGenRecordPtr d = mRecords[recordIndex];
	int value;
	if (d->getInt(fieldIndex, value))
	{
		return value;
	}

	OmnAlarm << "Failed to retrieve record: " << recordIndex 
		<< ". MemberIndex: " << fieldIndex
		<< ". Size: " << mRecords.entries() << enderr;
	return dflt;
}


int
AosGenTable::getInt(const u32 recordIndex, 
		const OmnString &name,
		const int dflt)
{
	AosGenFieldType type;
	int index = getFieldIndex(name, type);
	if (index < 0) 
	{
		OmnAlarm << "Failed to find the name: " << name << enderr;
		return dflt;
	}

	if (type != eAosGenFieldType_Int)
	{
		OmnAlarm << "Type mismatch: " << name << enderr;
		return dflt;
	}
	return getInt(recordIndex, (u32)index, dflt);
}


u32
AosGenTable::getU32(const u32 recordIndex, 
		const u32 fieldIndex, 
		const u32 dflt)
{
	// 
	// Retrieve the 'fieldIndex'-th u32 field of the 'recordIndex'-th
	// record. If not found, return 'dflt'.
	//
	if (recordIndex >= (u32)mRecords.entries())
	{
		// 
		// This is an error. 
		//
		OmnAlarm << "Index out of bound: " << recordIndex 
			<< ". Size: " << mRecords.entries() << enderr;
		return dflt;
	}

	AosGenRecordPtr d = mRecords[recordIndex];
	u32 value;
	if (d->getU32(fieldIndex, value))
	{
		return value;
	}

	OmnAlarm << "Failed to retrieve record: " << recordIndex 
		<< ". MemberIndex: " << fieldIndex
		<< ". Size: " << mRecords.entries() << enderr;
	return dflt;
}


u32
AosGenTable::getU32(const u32 recordIndex, 
		const OmnString &name, 
		const u32 dflt)
{
	AosGenFieldType type;
	int index = getFieldIndex(name, type);
	if (index < 0) 
	{
		OmnAlarm << "Failed to find the name: " << name << enderr;
		return dflt;
	}

	if (type != eAosGenFieldType_U32)
	{
		OmnAlarm << "Type mismatch: " << name << enderr;
		return dflt;
	}
	return getU32(recordIndex, (u32)index, dflt);
}


bool
AosGenTable::valueExist(const u32 fieldIndex, 
						const OmnString &value, 
						AosGenRecordPtr &record)
{
	// 
	// Check whether the value 'value' exists in the table. If not, 
	// return false. Otherwise, return true and 'record' holds 
	// the record.
	//
	for (int i=0; i<mRecords.entries(); i++)
    {
		record = mRecords[i];
		if (record->isSame(fieldIndex, value))
		{
			return true;
		}
	}

	record = 0;
	return false;
}


bool
AosGenTable::valueExist(const OmnString &name, 
						const OmnString &value, 
						AosGenRecordPtr &record)
{
	AosGenFieldType type;
	int index = getFieldIndex(name, type);
	if (index < 0) 
	{
		OmnAlarm << "Failed to find the name: " << name << enderr;
		return false;
	}

	if (type != eAosGenFieldType_Str)
	{
		OmnAlarm << "Type mismatch: " << name 
			<< ". Expecting String type but actual type is: " 
			<< AosGenFieldType_toStr(type) << enderr;
		return false;
	}
	return valueExist((u32)index, value, record);
}


bool
AosGenTable::valueExist(const u32 fieldIndex, 
						const int &value, 
						AosGenRecordPtr &record)
{
	// 
	// Check whether the value 'value' exists in the table. If not, 
	// return false. Otherwise, return true and 'record' holds 
	// the record.
	//
	for (int i=0; i<mRecords.entries(); i++)
    {
		record = mRecords[i];
		if (record->isSame(fieldIndex, value))
		{
			return true;
		}
	}

	record = 0;
	return false;
}


bool
AosGenTable::valueExist(const OmnString &name, 
						const int &value, 
						AosGenRecordPtr &record)
{
	AosGenFieldType type;
	int index = getFieldIndex(name, type);
	if (index < 0) 
	{
		OmnAlarm << "Failed to find the name: " << name << enderr;
		return false;
	}

	if (type != eAosGenFieldType_Int)
	{
		OmnAlarm << "Type mismatch: " << name << enderr;
		return false;
	}
	return valueExist((u32)index, value, record);
}


bool
AosGenTable::valueExist(const u32 fieldIndex, 
						const u32 &value, 
						AosGenRecordPtr &record)
{
	// 
	// Check whether the value 'value' exists in the table. If not, 
	// return false. Otherwise, return true and 'record' holds 
	// the record.
	//
	for (int i=0; i<mRecords.entries(); i++)
    {
		record = mRecords[i];
		if (record->isSame(fieldIndex, value))
		{
			return true;
		}
	}

	record = 0;
	return false;
}


bool
AosGenTable::valueExist(const OmnString &name, 
						const u32 &value, 
						AosGenRecordPtr &record)
{
	AosGenFieldType type;
	int index = getFieldIndex(name, type);
	if (index < 0) 
	{
		OmnAlarm << "Failed to find the name: " << name << enderr;
		return false;
	}

	if (type != eAosGenFieldType_U32)
	{
		OmnAlarm << "Type mismatch: " << name << enderr;
		return false;
	}
	return valueExist((u32)index, value, record);
}


bool
AosGenTable::setValue(const u32 recordIndex, 
					  const OmnString &name, 
					  const OmnString &value)
{
	AosGenFieldType type;
	int index = getFieldIndex(name, type);
	if (index < 0) return false;
	return setValue(recordIndex, (u32)index, value);
}


bool
AosGenTable::setValue(const u32 recordIndex, 
					  const u32 fieldIndex, 
					  const OmnString &value)
{
	if (recordIndex >= (u32)mRecords.entries())
	{
		OmnAlarm << "Out of bound: " << recordIndex 
			<< ". Size: " << mRecords.entries() << enderr;
		return false;
	}

	return mRecords[recordIndex]->set(fieldIndex, value);
}


bool
AosGenTable::setValue(const u32 recordIndex, 
					  const OmnString &name,
					  const int value)
{
	AosGenFieldType type;
	int index = getFieldIndex(name, type);
	if (index < 0) 
	{
		OmnAlarm << "Failed to find the name: " << name << enderr;
		return false;
	}

	if (type != eAosGenFieldType_Str)
	{
		OmnAlarm << "Type mismatch: " << name << enderr;
		return false;
	}
	return setValue(recordIndex, name, value);
}


bool
AosGenTable::setValue(const u32 recordIndex, 
					  const u32 fieldIndex, 
					  const int value)
{
	if (recordIndex >= (u32)mRecords.entries())
	{
		OmnAlarm << "Out of bound: " << recordIndex 
			<< ". Size: " << mRecords.entries() << enderr;
		return false;
	}

	return mRecords[recordIndex]->set(fieldIndex, value);
}


bool
AosGenTable::setValue(const u32 recordIndex, 
					  const OmnString &name,
					  const u32 value)
{
	AosGenFieldType type;
	int index = getFieldIndex(name, type);
	if (index < 0) 
	{
		OmnAlarm << "Failed to find the name: " << name << enderr;
		return false;
	}

	if (type != eAosGenFieldType_U32)
	{
		OmnAlarm << "Type mismatch: " << name << enderr;
		return false;
	}
	return setValue(recordIndex, name, value);
}


bool
AosGenTable::setValue(const u32 recordIndex, 
					  const u32 fieldIndex, 
					  const u32 value)
{
	if (recordIndex >= (u32)mRecords.entries())
	{
		OmnAlarm << "Out of bound: " << recordIndex 
			<< ". Size: " << mRecords.entries() << enderr;
		return false;
	}

	return mRecords[recordIndex]->set(fieldIndex, value);
}


int
AosGenTable::entryExist(
		const int *intv, const u16 *intIndex, const u16 numInt, 
		const OmnString *strv, const u16 *strIndex, const u16 numStr) const
{
	// 
	// If a record matches the input, the corresponding index is 
	// returned. Otherwise, -1 is returned. 
	//
	for (int k=0; k<mRecords.entries(); k++)
	{
		if (mRecords[k]->isSame(intv, intIndex, numInt, strv, strIndex, numStr)) 
		{
			return k;
		}
	}

	return -1;
}


AosGenTablePtr	
AosGenTable::getTable(const u32 keyFieldIndex, 
		const OmnString &key)
		// const u32 tableFieldIndex)
{
	/*
	// 
	// It assumes 'keyFieldIndex' identifies the key field,
	// 'key' is the value of the key. If it finds a record
	// whose 'keyFieldIndex''s value matches 'key', it will
	// retrieve the field 'tableFieldIndex', which should
	// be a table field.
	//
	OmnString value;
	for (int i=0; i<mRecords.entries(); i++)
	{
		if (!mRecords[i]->getStr(keyFieldIndex, value))
		{
			OmnAlarm << "Failed to retrieve the value: " 
				<< keyFieldIndex << ". Value: " << key << enderr;
			return 0;
		}

		if (value == key)
		{
			// 
			// Found it
			//
			return mRecords[i]->getTable(tableFieldIndex);
		}
	}
*/
	// 
	// Didn't find it.
	//
	return 0;
}


OmnString
AosGenTable::toString() const
{
	OmnString str;
	str << "Number of Records: " << mRecords.entries() << "\n"
	 	<< "Maximum Entries:   " << mMax << "\n"
		<< "Name: " << mName << "\n";
	if (mKeyFields.entries() >= 0)
	{
		str << "Primary Key Fields: ";
		for (int i=0; i<mKeyFields.entries(); i++)
		{
			str << mKeyFields[i] << " ";
		}

		str << "\n";
	}
	else
	{
		str << "Primary Key Index: no" << "\n";
	}

	for (int i=0; i<mRecords.entries(); i++)
	{
		str << "    " << mRecords[i]->getContents() << "\n";
	}

	return str;
}


bool
AosGenTable::removeRecord(const AosGenRecordPtr &record)
{
	bool ret = recordExist(record, true);
	if (!ret)
	{
		OmnAlarm << "Failed to remove record: " 
			<< record->toString() << enderr;
		return false;
	}
	return true;
}


AosGenRecordPtr
AosGenTable::recordExist(const AosGenRecordPtr &record, const bool removeFlag)
{
	// 
	// This function returns the record if and only if 'record'
	// key fields match the key fields of one of the record in 
	// this table. If 'flag' is true, it will also remove the record.
	//
	OmnString fieldName;
	int intv;
	u32 intu;
	OmnString ints;
	for (int i=0; i<mRecords.entries(); i++)
	{
		bool found = false;
		for (int k=0; k<mKeyFields.entries(); k++)
		{
			fieldName = mKeyFields[k];
			found = true;
			switch (mKeyType[k])
			{
			case eAosGenFieldType_Int:
				 if (!record->getInt(fieldName, intv))
				 {
				 	OmnAlarm << "Failed to get value: " << fieldName << enderr;
					found = false;
					break;
				 }
				 if (!mRecords[i]->isSame(fieldName, intv)) found = false;
				 break;

			case eAosGenFieldType_U32:
				 if (!record->getU32(fieldName, intu))
				 {
				 	OmnAlarm << "Failed to get value: " << fieldName << enderr;
					found = false;
					break;
				 }
				 if (!mRecords[i]->isSame(fieldName, intu)) found = false;
				 break;

			case eAosGenFieldType_Str:
				 if (!record->getStr(fieldName, ints))
				 {
				 	OmnAlarm << "Failed to get value: " << fieldName << enderr;
					found = false;
					break;
				 }
				 if (!mRecords[i]->isSame(fieldName, ints)) found = false;
				 break;

			default:
				 OmnAlarm << "Error: " << mKeyType[k] << enderr;
				 return 0;
			}

			if (!found)
			{
				break;
			}
		}

		if (found)
		{
			if (removeFlag)
			{
				AosGenRecordPtr rec = mRecords[i];
				mRecords.remove(i);
				return rec;
			}

			return mRecords[i];
		}
	}

	return 0;
}


AosGenRecordPtr 
AosGenTable::getRecordByKey(const AosGenRecordPtr &record)
{
	// 
	// Note: we are using the linear search algorithm now. 
	// IT IS VERY SLOW IF THE TABLE IS BIG!!!!!!!!!!!!!
	//
	for (int i=0; i<mRecords.entries(); i++)
	{
		bool match = true;
		for (int k=0; k<mKeyFields.entries(); k++)
		{
			if (!mRecords[i]->fieldMatch(mKeyFields[k], mKeyType[k], record))
			{
				match = false;
				break;
			}
		}

		if (match)
		{
			return mRecords[i];
		}
	}

	return 0;
}


AosGenFieldType
AosGenTable::getFieldType(const OmnString &name) const
{
	for (int i=0; i<mFieldDef.entries(); i++)
	{
		if (mFieldDef[i].mName == name)
		{
			return mFieldDef[i].mType;
		}
	}

	OmnAlarm << "Unrecognized name: " << name << enderr;
	return eAosGenFieldType_Invalid;
}


bool
AosGenTable::removeRecordRandomly()
{
	if (mRecords.entries() <= 0)
	{
		return false;
	}

	int index = OmnRandom::nextInt1(0, mRecords.entries()-1);
	mRecords.remove(index);
	return true;
}


bool	
AosGenTable::removeRecords(
		const OmnDynArray<OmnString, eArrayInitSize, 
			eArrayIncSize, eArrayMaxSize> &fields, 
		const OmnDynArray<OmnString, 
			eArrayInitSize, eArrayIncSize, eArrayMaxSize> &values)
{
	// 
	// This function removes all the records whose values match (fields, values), 
	// where 'fields' is an array of field names and 'values' are an array
	// of values for these fields. 
	//
	for (int i=0; i<mRecords.entries(); i++)
	{
		if (mRecords[i]->isSame(fields, values))
		{
			mRecords.remove(i);
			i--;
		}
	}

	return true;
}


bool
AosGenTable::clearTable()
{
	mRecords.clear();
	return true;
}


void
AosGenTable::resetFlags(const u32 flag)
{
	for (int i=0; i<mRecords.entries(); i++)
	{
		mRecords[i]->setFlag(flag);
	}
}


OmnString	
AosGenTable::dumpRecordsWithNotFlagged(const u32 flag)
{
	OmnString str;
	for (int i=0; i<mRecords.entries(); i++)
	{
		if (mRecords[i]->getFlag() != flag)
		{
			str << mRecords[i]->toString() << "\n";
		}
	}

	return str;
}


bool
AosGenTable::anyRecordNotMarked(const u32 flag)
{
	OmnString str;
	for (int i=0; i<mRecords.entries(); i++)
	{
		if (mRecords[i]->getFlag() != flag)
		{
			return true;
		}
	}

	return false;
}


OmnString
AosGenTable::getFieldName(const u32 fieldIndex) const
{
	for (int i=0; i<mFieldDef.entries(); i++)
	{
		if (mFieldDef[i].mIndex == fieldIndex)
		{
			return mFieldDef[i].mName;
		}
	}

	return "";
}


int
AosGenTable::getFieldNames(
		OmnDynArray<OmnString, eArrayInitSize, eArrayIncSize, eArrayMaxSize> &names) const
{
	for (int i=0; i<mFieldDef.entries(); i++)
	{
		names.append(mFieldDef[i].mName);
	}

	return mFieldDef.entries();
}

