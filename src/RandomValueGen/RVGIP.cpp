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
#include "Parms/RVGIP.h"

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

AosRVGIP::AosRVGIP(const AosCliCmdTorturerPtr &cmd)
:
AosRVG(cmd),
mSelectFromTablePct(eDefaultSelectFromTablePct),
mUsedAsInteger(false),
mNotInTableWeight(eDefaultNotInTableWeight),
mInTableWeight(eDefaultInTableWeight),
mNotInRangeWeight(eDefaultNotInRangeWeight),
mInvalidIpWeight(eDefaultInvalidWeight)
{
}


AosRVGIP::~AosRVGIP()
{
}


AosRVGIPPtr
AosRVGIP::createInstance(
				const AosCliCmdTorturerPtr &cmd,
				const OmnXmlItemPtr &def,
				const OmnString &cmdTag,
            	OmnVList<AosGenTablePtr> &tables)
{
	AosRVGIPPtr inst = OmnNew AosRVGIP(cmd);
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
AosRVGIP::config(const OmnXmlItemPtr &def, 
				  const OmnString &cmdTag, 
				  OmnVList<AosGenTablePtr> &tables)
{
	// 
	// 	<Parm>
	// 		<type>
	// 		<Name>
	// 		<DataType>
	// 		<SmartPtr>
	// 		<FieldName>
	// 		<NetAddr>
	// 		<NetMask>
	// 		<KeyType>
	// 		<BadValue>
	// 		<CorrectOnly>
	// 		<TableName>
	// 		<DependedTables>
	// 		<UsedAsInteger>
	// 		...
	// 	</Parm>
	//
	if (!AosRVG::config(def, cmdTag, tables))
	{
		OmnAlarm << "Failed to parse parm: " << def->toString() << enderr;
		return false;
	}

	mAddr = OmnIpAddr(def->getStr("NetAddr", "0.0.0.0"));
	mNetmask = OmnIpAddr(def->getStr("NetMask", "0.0.0.0"));
	mBadValue = def->getStr("BadValue", "***BAD***");
	mUsedAsInteger = def->getBool("UsedAsInteger", false);

	if (!mAddr.isValid())
	{
		OmnAlarm << "Invalid NetAddr: " << def->toString() << enderr;
		return 0;
	}

	if (!OmnIpAddr::isValidMask(mNetmask))
	{
		OmnAlarm << "Invalid NetMask: " << def->toString() << enderr;
		return 0;
	}

	return (mIsGood = createSelectors());
}


bool
AosRVGIP::createSelectors()
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

	int64_t index[4];
	u16 weights[4];
	int idx = 0;
	index[idx] = eGenerateNotInTableIp;		weights[idx++] = mNotInTableWeight;
	index[idx] = eGenerateInTableIp;		weights[idx++] = mInTableWeight;
	index[idx] = eGenerateNotInRangeIp;		weights[idx++] = mNotInRangeWeight;
	index[idx] = eGenerateInvalidIp;		weights[idx++] = mInvalidIpWeight;

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
AosRVGIP::nextStr(OmnString &value, 
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
	     // 
		 // This parameter is used as a key to 'data' and
		 // if the value does not identify a record in 'data', 
		 // that value is considered good. 
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
		 ret = nextNotKey(mLastValue, data, record, isGood, 
				 correctOnly, selectFromRecord, rcode, errmsg);
		 break;

	default:
		 // 
		 // This should never happen.
		 //
		 OmnAlarm << "Unrecognized argument type: " 
			 << mKeyType << enderr;
		 (errmsg = "Invalid Key: ") << mKeyType;
		 rcode = eAosRVGRc_ProgramError;
		 mLastValue = mBadValue;
		 return false;
	}

	if (ret == true)
	{
		value = mLastValue;

	}

	return ret;
}


bool
AosRVGIP::nextExist(OmnString &value, 
					  const AosGenTablePtr &table, 
					  const AosGenRecordPtr &theRecord,
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
	// value is good, 'theRecord' will hold the record.
	//
	rcode = eAosRVGRc_ProgramError;
	isGood = false;
	value = mBadValue;
	AosGenTablePtr data = (mTable)?mTable:table;
	if (selectFromRecord)
	{
		// 
		// The caller forces this function to select a value from the 
		// record 'theRecord'. 'theRecord' SHALL not be null.
		//
		aos_assert_r(mFieldName != "", false);
		aos_assert_r(theRecord, false);

		if (mUsedAsInteger)
		{
			int v;
			if (!theRecord->getInt(mFieldName, v))
			{
				errmsg << "Failed to retrieve the field value from record: "
					<< theRecord->toString()
					<< ". FieldName: " << mFieldName;
				OmnAlarm << errmsg << enderr;
				return false;
			}
			value = OmnIpAddr((u32)v).toString();
		}
		else
		{
			if (!theRecord->getStr(mFieldName, value))
			{
				errmsg << "Failed to retrieve the field value from record: "
					<< theRecord->toString()
					<< ". FieldName: " << mFieldName;
				OmnAlarm << errmsg << enderr;
				return false;
			}
		}
		rcode = eAosRVGRc_SelectFromRecord;
		isGood = true;
		return true;
	}

	if (mCorrectOnly || correctOnly || aos_next_pct(mCorrectPct))
	{
		if (data && data->entries() > 0)
		{
			AosGenRecordPtr record = data->getRandomRecord();
			aos_assert_r(mFieldName != "", false);
			aos_assert_r(record, false);

			if (mUsedAsInteger)
			{
				int v;
				if (!record->getInt(mFieldName, v))
				{
					errmsg << "Failed to retrieve the field value from record: "
						<< record->toString()
						<< ". FieldName: " << mFieldName;
					OmnAlarm << errmsg << enderr;
					return false;
				}
				value = OmnIpAddr((u32)v).toString();
			}
			else
			{
				if (!record->getStr(mFieldName, value))
				{
					errmsg << "Failed to retrieve the field value from record: " 
						<< record->toString()
						<< ". FieldName: " << mFieldName;
					OmnAlarm << errmsg << enderr;
					return false;
				}
			}

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
	//
	isGood = false;
	return generateBadIp(value, data, theRecord, rcode, errmsg);	
	
} 

bool
AosRVGIP::nextNew(OmnString &value, 
					const AosGenTablePtr &data, 
					const AosGenRecordPtr &theRecord,
					bool &isGood, 
					const bool correctOnly,
					const bool selectFromRecord,
					AosRVGReturnCode &rcode, 
					OmnString &errmsg)
{
	// 
	// This function will generate a new IP address. A good IP address
	// must meet the following:
	// 1. The address is syntactically correct
	// 2. THe address is not in the table.
	//
	// Error Conditions:
	// For error conditions, please refer to generateBadValue().
	//
	if (selectFromRecord)
	{
		// 
		// The caller forces this function to select a value from the 
		// record 'record'. 'record' SHALL not be null.
		//
		aos_assert_r(mFieldName != "", mBadValue);
		aos_assert_r(theRecord, false);

		if (mUsedAsInteger)
		{
			int v;
			if (!theRecord->getInt(mFieldName, v))
			{
				errmsg << "Failed to retrieve the field value from record: " 
					<< theRecord->toString()
					<< ". FieldName: " << mFieldName;
				OmnAlarm << errmsg << enderr;
				return false;
			}
			value = OmnIpAddr((u32)v).toString();
		}
		else
		{
			if (!theRecord->getStr(mFieldName, value))
			{
				errmsg << "Failed to retrieve the field value from record: " 
					<< theRecord->toString()
					<< ". FieldName: " << mFieldName;
				OmnAlarm << errmsg << enderr;
				return false;
			}
		}
		rcode = eAosRVGRc_SelectFromRecord;
		isGood = true;
		return true;
	}

	isGood = true;
	if (mCorrectOnly || correctOnly || aos_next_pct(mCorrectPct))
	{
		OmnIpAddr addr;
		int tries = (mCorrectOnly || correctOnly)?50:5;
		while (tries--)
		{
			addr = aos_next_ip(mAddr, mNetmask);
			
			while(!OmnIpAddr::isAddrIn(addr, mAddr, mNetmask))
			{
				addr = aos_next_ip(mAddr, mNetmask);
			}

			// 
			// Check whether the address is in the table.
			//
			AosGenRecordPtr record;
			if (mUsedAsInteger)
			{
				int v = addr.getIPv4();	
				if (!data || !data->valueExist(mFieldName, v, record))
				{
					// 
					// The address is not in the table. It is a good one.
					//
					rcode = eAosRVGRc_Success;
					return true;
				}
			}
			else
			{
				value = addr.toString();	
				if (!data || !data->valueExist(mFieldName, value, record))
				{
					// 
					// The address is not in the table. It is a good one.
					//
					rcode = eAosRVGRc_Success;
					return true;
				}
			}
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
	return generateBadIp(value, data, theRecord, rcode, errmsg);
	
}


bool
AosRVGIP::nextSubkeyNew(OmnString &value, 
	                  const AosGenTablePtr &data, 
				      const AosGenRecordPtr &theRecord,
				      bool &isGood, 
					  const bool correctOnly,
					  const bool selectFromRecord, 
					  AosRVGReturnCode &rcode, 
				      OmnString &errmsg)
{
OmnAlarm << "Called the subkey" << enderr;
return generateBadIp(value, data, theRecord, rcode, errmsg);
}

bool
AosRVGIP::nextNotKey(OmnString &value, 
	                  const AosGenTablePtr &data, 
				      const AosGenRecordPtr &theRecord,
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
		// 
		// The caller forces this function to select a value from the 
		// record 'record'. 'record' SHALL not be null.
		//
		aos_assert_r(theRecord, false);
		if (mUsedAsInteger)
		{
			int v;
			if (!theRecord->getInt(mFieldName, v))
			{
				errmsg << "Failed to retrieve value from record: " 
					<< mFieldName << ". Record: " << theRecord->toString();
				OmnAlarm << errmsg << enderr;
				return false;
			}
			value = OmnIpAddr((u32)v).toString();
		}
		else
		{
			if (!theRecord->getStr(mFieldName, value))
			{
				errmsg << "Failed to retrieve value from record: " 
					<< mFieldName << ". Record: " << theRecord->toString();
				OmnAlarm << errmsg << enderr;
				return false;
			}
		}

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
			AosGenRecordPtr record = data->getRandomRecord();
			aos_assert_r(record, mBadValue);

			if (mUsedAsInteger)
			{
				int v;
				if (!record->getInt(mFieldName, v))
				{
					OmnAlarm << "Failed to retrieve value: " 
						<< mFieldName << ". Record: " 
						<< record->toString() << enderr;
					return mBadValue; 
				}
				value = OmnIpAddr((u32)v).toString();
			}
			else
			{
				if (!record->getStr(mFieldName, value))
				{
					OmnAlarm << "Failed to retrieve value: " 
						<< mFieldName << ". Record: " 
						<< record->toString() << enderr;
					return mBadValue; 
				}
			}
			isGood = true;
			rcode = eAosRVGRc_Success;
			return true;
		}

		// 
		// Will randomly generated one.
		//
		isGood = true;
		rcode = eAosRVGRc_Success;
		value = aos_next_ip(mAddr, mNetmask).toString();
		return true;
	}

	isGood = false;
	return generateBadIp(value, data, theRecord, rcode, errmsg);
}


bool
AosRVGIP::generateBadIp(OmnString &value, 
						 const AosGenTablePtr &data, 
						 const AosGenRecordPtr &theRecord, 
						 AosRVGReturnCode &rcode, 
						 OmnString &errmsg)
{	
	int tries = 10;
	OmnString str;

	rcode = eAosRVGRc_ProgramError;
	aos_assert_r(mBadValueSelector, mBadValue);
	int method = eGenerateInvalidIp;

	AosGenRecordPtr record;
	switch ((method = mBadValueSelector->nextInt()))
	{
	case eGenerateInTableIp:
		 if (data && data->entries() > 0)
		 {
		 	rcode = eAosRVGRc_ExistInTable;
		 	errmsg = mArgName;
			errmsg << " is in the table. ";
		 	record = data->getRandomRecord();
			aos_assert_r(record, mBadValue);

			if (mUsedAsInteger)
			{
				int v;
				if (!record->getInt(mFieldName, v))
				{
					OmnAlarm << "Failed to retrieve integer from record: " 
						<< mFieldName << enderr;
					return false;
				}
				value = OmnIpAddr((u32)v).toString();
			}
			else
			{
				if (!record->getStr(mFieldName, value))
				{
					OmnAlarm << "Failed to retrieve integer from record: " 
						<< mFieldName << enderr;
					return false;
				}
			}
			return true;
		 }
		 goto syn_err;

	case eGenerateNotInTableIp:
		 while (tries--)
		 {
			 if (mUsedAsInteger)
			 {
		 		int v = aos_next_ip(mAddr, mNetmask).getIPv4();
				if (!data || !data->valueExist(mFieldName, v, record))
				{
					rcode = eAosRVGRc_NotFoundInTable;
					errmsg = mArgName;
					errmsg << " not in the table. ";
					return true;
				}
			 }
			 else
			 {
		 		value = aos_next_ip(mAddr, mNetmask).toString();
				if (!data || !data->valueExist(mFieldName, value, record))
				{
					rcode = eAosRVGRc_NotFoundInTable;
					errmsg = mArgName;
					errmsg << " not in the table. ";
					return true;
				}
			 }
		 }
		 goto syn_err;
		 
	case eGenerateNotInRangeIp:
		 while (tries--)
		 {
		 	value = OmnRandom::nextStrIP();
			if (!OmnIpAddr::isAddrIn(OmnIpAddr(value), mAddr, mNetmask))
			{
				rcode = eAosRVGRc_NotInRange;
				errmsg = mArgName;
				errmsg << " not in the sub net.";
				return true;
			}
		 }

syn_err:		 
	case eGenerateInvalidIp:
		 errmsg = mArgName;
		 errmsg << "is syntactically incorrect";
		 value = aos_next_incorrect_ip();
		 value = "111.aaa.bbb";
		 rcode = eAosRVGRc_Incorrect;
		 return true;

	default:
		 rcode = eAosRVGRc_ProgramError;
		 errmsg = mArgName;
		 errmsg << " unrecognized method, syntactically incorrect ip is used. "
		 	<< "Method: " << method;
		 OmnAlarm << errmsg << enderr;
		 value = "111.aaa.bbb";
		 value = aos_next_incorrect_ip();
		 return false;
	}

	aos_should_never_come_here;
	errmsg << "Program Error: " << __FILE__ << ":" << __LINE__;
	return false; 
}


bool
AosRVGIP::nextInt(
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
AosRVGIP::nextInt(
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
AosRVGIP::newRecordAdded(const AosGenRecordPtr &newRecord) const
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
AosRVGIP::recordDeleted(const AosGenRecordPtr &record) const
{
	return true;
}


OmnString
AosRVGIP::toString(const u32 indent) const
{
	OmnString str;
	OmnString ind(indent, ' ', true);

	str << ind << "Argument: " << mArgName << "\n"
		<< ind << "    KeyType:            " << AosGetRVGKeyTypeStr(mKeyType) << "\n"
		<< ind << "    FieldName:          " << mFieldName << "\n"
		<< ind << "    IP addr:            " << mAddr.toString() << "\n"
		<< ind << "    Netmask    :        " << mNetmask.toString() << "\n"
		<< ind << "    Bad Value:          " << mBadValue << "\n"
		<< ind << "    Last Value:         " << mLastValue.toString() << "\n"
		<< ind << "    CorrectPct:         " << mCorrectPct << "\n";
	return str;
}


bool 	
AosRVGIP::setRecord(const AosGenRecordPtr &record) const
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


bool 	
AosRVGIP::getCrtValue(int &value) const
{
	OmnIpAddr addr(mLastValue);
	value = (int)addr.getIPv4();
	return true;
}


bool 	
AosRVGIP::getCrtValue(u16 &value) const
{
	OmnAlarm << "Cannot get u16 from RVGIP" << enderr;
	return false;
}


bool 	
AosRVGIP::getCrtValue(u32 &value) const
{
	OmnIpAddr addr(mLastValue);
	value = (int)addr.getIPv4();
	return true;
}


bool 	
AosRVGIP::getCrtValue(OmnString &value) const
{
	value = mLastValue;
	return true;
}


bool 	
AosRVGIP::getCrtValue(char *&value) const
{
	value = (char *)mLastValue.data();
	return true;
}


bool 	
AosRVGIP::getCrtValue(OmnIpAddr &value) const
{
	value.set(mLastValue);
	return true;
}


bool 	
AosRVGIP::getCrtValueAsArg(OmnString &value, 
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
		 	decl << "OmnIpAddr _mArg_" << mArgName << "(\"" << mLastValue << "\"); ";
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
		 	decl << "OmnIpAddr _mArg_" << mArgName << "; ";
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
			if (mDataType == "OmnIpAddr")
			{
		 		decl << "OmnIpAddr _mArg_" << mArgName << "; ";
		 		value << "&_mArg_" << mArgName;
			}
			else if (mUsedAsInteger)
			{
				decl << "u32 _mArg_" << mArgName << "; ";
		 		value << "&_mArg_" << mArgName;
			}
			else
			{
				decl << "char _mArg_" << mArgName << "[50]; ";
		 		value << "_mArg_" << mArgName;
			}
		 }

		 break;

	case eAosRVGUsageType_InputOutputThrAddr:
		 if (mVarDecl != "")
		 {
		 	decl << mVarDecl << ";\n";
		 }
		 else
		 {
			if (mDataType == "OmnIpAddr")
			{
		 		decl << "OmnIpAddr _mArg_" << mArgName 
					<< "(\"" << mLastValue << "\"); ";
		 		value << "&_mArg_" << mArgName;
			}
			else if (mUsedAsInteger)
			{
				decl << "u32 _mArg_" << mArgName 
					<< " = OmnIpAddr(" << mLastValue
					<< ").getIPv4(); ";
		 		value << "&_mArg_" << mArgName;
			}
			else
			{
				decl << "char _mArg_" << mArgName
					<< "[50]; strcpy(_mArg_" << mArgName
					<< ", " << mLastValue << "); ";
		 		value << "_mArg_" << mArgName;
			}
		 }
		 break;

	default:
		 OmnAlarm << "Unrecognized UsageType: " << mUsageType << enderr;
		 return false;
	}

	return true;
}


OmnString 
AosRVGIP::getNextValueFuncCall() const
{
	return "nextStr";
}

