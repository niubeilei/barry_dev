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
// Data Fields are the most foundamental data structure in AOS. It provides
// field level syntax and semantics. 
//
// Value Conversion
// A data field has the ability to convert values from one form to another  
// form. As an example, U64 field can convert data between string and u64.
//
// Retrieving Value from Record
// Record is a higher level data structure on top of data fields. Each data
// field is designed so that it knows how to retrieve values from its 
// compatible data record.
//
// Setting Value to Record
// This is similar to retrieving values from Record.
//
// Data Processing
// In addition to providing field level syntax, data fields may do some data
// processing (semantics).
//
// Modification History:
// 03/01/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataField/DataField.h"

#include "SEInterfaces/ConditionObj.h"
#include "DataField/DataFieldIILEntry.h"
#include "DataField/DataFieldIILMap.h"
#include "DataField/DataFieldMap.h"
#include "DataField/DataFieldStr.h"
#include "DataField/DataFieldChar.h"
#include "DataField/DataFieldUChar.h"
#include "DataField/DataFieldU64.h"
#include "DataField/DataFieldU32.h"
#include "DataField/DataFieldInt64.h"
#include "DataField/DataFieldInt32.h"
#include "DataField/DataFieldDouble.h"
#include "DataField/DataFieldExpr.h"
#include "DataField/DataFieldBool.h"
#include "DataField/DataFieldMetaData.h"
#include "DataField/DataFieldDocid.h"
#include "DataField/DataFieldBuff.h"
#include "DataField/FieldBinU64.h"
#include "DataField/FieldBinU32.h"
#include "DataField/FieldBinInt64.h"
#include "DataField/FieldBinInt.h"
#include "DataField/FieldBinChar.h"
#include "DataField/FieldBinUChar.h"
#include "DataField/FieldBinBool.h"
#include "DataField/FieldBinDouble.h"
#include "DataField/DataFieldXpath.h"
#include "DataField/DataFieldNumber.h"
#include "DataField/DataFieldDateTime.h"
#include "DataField/FieldBinDateTime.h"
#include "DataField/DataFieldVirtual.h"


static AosDataField*	sgFields[AosDataFieldType::eMax];
static OmnMutex			sgLock;
static bool				sgInited = false;


AosDataField::AosDataField(
		const AosDataFieldType::E type,
		const OmnString &name,
		const bool reg)
:
mType(type),
mTypeName(name),
mIgnoreSerialize(false),
mIsFixed(true),
mIsConst(false),
mValueFromFieldIdx(-1)
{
	mDftValue.setNull();
	if (reg)
	{
		registerDataField(type, name, this);
	}
}


AosDataField::AosDataField(const AosDataField &rhs)
:
mType(rhs.mType),
mTypeName(rhs.mTypeName),
mName(rhs.mName),
mIgnoreSerialize(rhs.mIgnoreSerialize),
mIsFixed(rhs.mIsFixed),
mIsConst(rhs.mIsConst),
mConstValueStr(rhs.mConstValueStr),
mValueFromFieldIdx(rhs.mValueFromFieldIdx),
mDftValue(rhs.mDftValue)
{
	mFieldInfo = rhs.mFieldInfo;
}


AosDataField::~AosDataField()
{
}


bool
AosDataField::config(
		const AosXmlTagPtr &def,
		AosDataRecordObj *record,
		AosRundata *rdata)
{
	//  <dataelem
	//      AOSTAG_TYPE="xxx"
	//      AOSTAG_NAME="xxx"					The field's name
	//      AOSTAG_OFFSET="xxx"					The field's position
	//      AOSTAG_ISCONST="xxx"				Whether this is a constant value.
	//      AOSTAG_FIXED_POSITION="true|false"	Whether the position is fixed or not.
	//      AOSTAG_RAW_FORMAT="u64|str|..."		How original data is expressed
	//      AOSTAG_VALUE="xxx" 					Needed only when AOSTAG_ISCONST is true
	//      AOSTAG_FIELD_OPR="xxx" 				
	//      AOSTAG_LENGTH="xxx"> 				Data Length
	//      <data_filter .../>					An action used to filter data.
	//  </dataelem>
	aos_assert_r(def, false);

	mName = def->getAttrStr(AOSTAG_NAME);
	if (mName == "")
	{
		AosSetErrorU(rdata, "missing_data_field_name:") << def->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	mIgnoreSerialize = def->getAttrBool(AOSTAG_IGNORE_SERIALIZE, getDefaultIgnoreSerialize()),
	mIsFixed = def->getAttrBool(AOSTAG_FIXED_POSITION, record->isFixed());

	bool rslt;
	mIsConst = def->getAttrBool(AOSTAG_ISCONST, false);
	if (mIsConst)
	{
		AosXmlTagPtr const_tag = def->getFirstChild(AOSTAG_CONSTANT);
		if (const_tag)
		{
			OmnString type = const_tag->getAttrStr(AOSTAG_ZKY_TYPE, "const");
			if (type == "empty")
			{
				mConstValueStr = "";
			}
			else if (type == "env")
			{
				OmnString str = const_tag->getNodeText();
				aos_assert_r(record, false);
				rslt = record->getRecordENV(str, mConstValueStr, rdata);
				aos_assert_r(rslt, false);
				aos_assert_r(mConstValueStr != "", false);
			}
			else
			{
				mConstValueStr = const_tag->getNodeText();
				aos_assert_r(mConstValueStr != "", false);
			}
		}
		mConstValueStr.normalizeWhiteSpace(true, true);  // trim
	}

	OmnString value_from_field = def->getAttrStr(AOSTAG_VALUEFROMFIELD);
	if (value_from_field != "")
	{
		mValueFromFieldIdx = record->getFieldIdx(value_from_field, rdata);
		//aos_assert_r(mValueFromFieldIdx >= 0, false);
		if (mValueFromFieldIdx < 0)
		{
			AosSetEntityError(rdata, "DataField_missing_value_fieldname", 
				"DataField", "DataField") 
				<< "Field Name: " << value_from_field 
				<< ", Record Objid: " << record->getRecordObjid()
				<< ", Record Name: " << record->getRecordName() << enderr;
			return false;
		}
	}

	// Retrieve mFieldInfo
	rslt = mFieldInfo.config(def, rdata);
	aos_assert_r(rslt, false);

	if (needValueFromField() && !mIsConst)
	{
		//aos_assert_r(mValueFromFieldIdx >= 0, false);
		// Ketty 2014/01/11 DataFieldU64 for iil_field not need valueFromFieldIdx.
		//if (mValueFromFieldIdx < 0)
		//{
		//	AosSetEntityError(rdata, "DataField_virtual_field_not_fount_real_field", 
		//		"DataField", "DataField") << def->toString() << enderr;
		//	return false;
		//}
	}

	// Ketty 2014/01/03
	//if (!needValueFromField() && !mIsConst && mIsFixed)
	if (!needValueFromField() && !mIsConst && mIsFixed)
	{
		if (isVirtualField()) return true;

		//if (mFieldInfo.field_offset < 0)
		//{
		//	AosSetEntityError(rdata, "DataField_invalid_offset", 
		//		"DataField", "DataField") << def->toString() << enderr;
		//	return false;
		//}

		//if (mFieldInfo.field_data_len <= 0)
		//{
		//	AosSetErrorU(rdata, "field_len_invalid") << ": " << def->toString() << enderr;
		//	OmnAlarm << rdata->getErrmsg() << enderr;
		//	return false;
		//}
	}

	bool exist;
	OmnString dft_value_str;
	dft_value_str = def->getAttrStr1(AOSTAG_VALUE_DFT_VALUE, exist, "");
	if(exist)
	{
		dft_value_str.normalizeWhiteSpace(true, true);  // trim
		mDftValue.setStr(dft_value_str);
	}
	return true;
}


bool
AosDataField::setValueToRecord(
		char * const data,
		const int data_len,
		const AosValueRslt &value, 
		bool &outofmem,
		AosRundata* rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosDataField::setValueToRecord(
		AosDataRecordObj* record,
		const AosValueRslt &value, 
		bool &outofmem,
		AosRundata* rdata)
{
	OmnNotImplementedYet;
	return false;
}
	

bool
AosDataField::getValueFromRecord(
		AosDataRecordObj* record,
		const char * data,
		const int len,
		int &idx,
		AosValueRslt &value, 
		const bool copy_flag,
		AosRundata* rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosDataField::createRandomValue(
		const AosBuffPtr &buff,
		AosRundata *rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosDataField::serializeToBuff(
		const AosXmlTagPtr &doc, 
		const AosBuffPtr &buff,
		AosRundata *rdata)
{
	if (mIgnoreSerialize) return true;
	OmnNotImplementedYet;
	return false;
}
	

bool
AosDataField::serializeToXmlDoc(
		OmnString &docstr,
		int &idx, 
		const char *data, 
		const int datalen,
		AosRundata* rdata)
{
	// It converts the data from 'data' to 'docstr'. The value in 'data'
	// starts at 'idx', and its length is 'mFieldInfo.field_data_len'. 
	// If needed, it will do the data conversion (not supported yet)
	// from the original form to the target form.
	if (mIgnoreSerialize) return true;

	// Chen Ding, 2014/08/16
	if (mIsNull) return true;
	
	aos_assert_r(mName != "", false);

	OmnString vv = "";
	if (mIsConst)
	{
		vv = mConstValueStr;
	}
	else
	{
		aos_assert_r(data, false);
		aos_assert_r(mFieldInfo.field_data_len >= 0, false);
		aos_assert_r(idx >= 0 && datalen > 0 && 
			(u32)idx + (u32)mFieldInfo.field_data_len <= (u32)datalen, false);
		
		int start_pos = mIsFixed ? mFieldInfo.field_offset : idx;
		if (start_pos < 0) return false;
		
		OmnString str(&data[start_pos], mFieldInfo.field_data_len);
		idx = start_pos + mFieldInfo.field_data_len;
		str.normalizeWhiteSpace(true, true);  // trim
		vv = str;
	}
	
	docstr << "<" << mName << "><![CDATA[" << vv << "]]></" << mName << ">";
	return true;
}


AosDataFieldObjPtr 
AosDataField::createStrField(AosRundata *rdata)
{
	AosDataFieldObjPtr field = OmnNew AosDataFieldStr(false);
	aos_assert_r(field, 0);
	return field;
}


AosDataFieldObjPtr 
AosDataField::createDataField(
		const AosXmlTagPtr &def,
		AosDataRecordObj *record,
		AosRundata *rdata)
{
	if(!sgInited) init();
	
	aos_assert_r(def, 0);
	OmnString ss = def->getAttrStr(AOSTAG_TYPE);
	AosDataFieldType::E type = AosDataFieldType::toEnum(ss);

	// Chen Ding, 2014/08/16
	if (type == AosDataFieldType::eJimoField)
	{
		// <field classname="xxx" version="xxx" ../>
		AosJimoPtr jimo = AosCreateJimo(rdata, def);
		if (!jimo)
		{
			AosLogError(rdata, true, "field_not_found") << "Config: " << def << enderr;
			return 0;
		}

		if (jimo->getJimoType() != AosJimoType::eDataField)
		{
			AosLogError(rdata, true, "not_field_jimo") << "Config: " << def << enderr;
			return 0;
		}

		AosDataFieldObjPtr field_jimo = dynamic_cast<AosDataField *>(jimo.getPtr());
		aos_assert_rr(field_jimo, rdata, 0);
		return field_jimo;
	}

	if (!AosDataFieldType::isValid(type))
	{
		AosSetEntityError(rdata, "DataField_invalid_type", 
				"DataField", "DataField") << def->toString() << enderr;
		return 0;
	}

	AosDataField * field = sgFields[type];
	aos_assert_r(field, 0);

	return field->create(def, record, rdata);
}


bool
AosDataField::init()
{
	if(sgInited) return true;
	
	for (int i=0; i<AosDataFieldType::eMax; i++)
	{
		sgFields[i] = 0;
	}

	static AosDataFieldMap			lsFieldMap(true);
	static AosDataFieldIILEntry		lsFieldIILEntry(true);
	static AosDataFieldIILMap		lsFieldIILMap(true);
	static AosDataFieldStr			lsFieldStr(true);
	static AosDataFieldChar			lsFieldChar(true);
	static AosDataFieldUChar		lsFieldUChar(true);
	static AosDataFieldU64			lsFieldU64(true);
	static AosDataFieldU32			lsFieldU32(true);
	static AosDataFieldInt64		lsFieldInt64(true);
	static AosDataFieldInt32		lsFieldInt32(true);
	static AosDataFieldDouble		lsFieldDouble(true);
	static AosDataFieldBool			lsFieldBool(true);
	static AosDataFieldMetaData		lsFieldMetaData(true);
	static AosFieldBinU64			lsFieldBinU64(true);
	static AosFieldBinInt64			lsFieldBinInt64(true);
	static AosFieldBinU32			lsFieldBinU32(true);
	static AosFieldBinInt			lsFieldBinInt(true);
	static AosFieldBinChar			lsFieldBinChar(true);
	static AosFieldBinUChar			lsFieldBinUChar(true);
	static AosFieldBinBool			lsFieldBinBool(true);
	static AosFieldBinDouble		lsFieldBinDouble(true);
	static AosDataFieldDocid		lsFieldDocid(true);		// Ketty 2013/12/25
	static AosDataFieldBuff			lsFieldBuff(true);
	static AosDataFieldXpath		lsFieldXpath(true);		
	static AosDataFieldExpr			lsFieldExpr(true);		
	static AosDataFieldNumber 		lsFieldNumber(true);		
	static AosDataFieldDateTime 	lsFieldDateTime(true);		
	static AosFieldBinDateTime 		lsFieldBinDateTime(true);		
	static AosDataFieldVirtual		lsFieldVirtual(true);		

	sgInited = true;
	return true;
}


bool
AosDataField::registerDataField(
		const AosDataFieldType::E type,
		const OmnString &name, 
		AosDataField * field)
{
	sgLock.lock();

	if(name == "bitmap")
	{
		sgLock.unlock();
		return true;
	}

	if (!AosDataFieldType::isValid(type))
	{
		sgLock.unlock();
		OmnString errmsg = "Incorrect id: ";
		errmsg << type;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	if (sgFields[type])
	{
		sgLock.unlock();
		OmnString errmsg = "Already registered: ";
		errmsg << type;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	sgFields[type] = field;
	AosDataFieldType::addName(name, type);
	sgLock.unlock();
	return true;
}


/*
void
AosDataField::setFixedFlag(const bool b)
{
	mIsFixed = b;
}


void
AosDataField::setFieldPos(const int start_pos, const int len)
{
	mIsNull = false;
	mFieldInfo.setPosition(start_pos, len);
}


void 
AosDataField::setFieldValue(const AosValueRslt &value)
{
	if (value.isNull())
	{
		mIsNull = true;
		return;
	}
	mIsNull = false;
	return;
}
*/


bool
AosDataField::isVirtualField()
{
	switch (mType)
	{
	case AosDataFieldType::eIILEntry:
	case AosDataFieldType::eIILMap:
	case AosDataFieldType::eMetaData:
	case AosDataFieldType::eDocid:
	case AosDataFieldType::eVirtual:
	case AosDataFieldType::eExpr:
		return true;
	default:
		return false;
	}

	return false;
}


// Chen Ding, 2015/01/24
AosDataFieldObjPtr 
AosDataField::createDataField(
		AosRundata *rdata, 
		const AosDataType::E data_type)
{
	switch (data_type)
	{
	case AosDataType::eString:
		 return OmnNew AosDataFieldStr(false);

	case AosDataType::eU64:
		 return OmnNew AosDataFieldU64(false);

	default:
		 AosLogUserError(rdata, "not_supported") << enderr;
		 return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}

