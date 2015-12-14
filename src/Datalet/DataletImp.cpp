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
// Modifications
// -------------
// 1. If a field was not modified, its value will be retrieved from
//    the raw data.
// 2. When a field is modified, it is marked as being modified (status
//    == eFieldModified) and its new value is push_backed in mFieldValues
//    and its index is stored in the field info.
// 3. When a field is deleted, it is marked as being deleted (status
//    == eFieldDeleted).
//
// Modification History:
// 2013/03/17 Created by Chen Ding
// 2014/01/01 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Datalet/DataletImp.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "DataField/DataField.h"
#include "Debug/Except.h"
#include "DataRecord/RecordUtil.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DataRecordType.h"
#include "Util/ValueRslt.h"
#include "Util/VInt.h"
#include "XmlUtil/XmlTag.h"

extern int gAosEndian;

extern "C"
{
AosJimoPtr AosCreateJimoFunc_AosDatasetByDataScanner_0(const AosRundataPtr &rdata, const u32 version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosDataletImp(version);
		aos_assert_r(jimo, 0);
		return jimo;
	}

	catch (...)
	{
		AosSetErrorU(rdata, "Failed creating jimo") << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}



AosDataletImp::AosDataletImp(const u32 version)
:
AosDatalet(AosDataletType::eBinary, version)
{
}


AosDataletImp::~AosDataletImp()
{
}


bool
AosDataletImp::config(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker_doc)
{
	return true;
}


void
AosDataletImp::clear()
{
	mInternalData = "";
	mData = (char *)mInternalData.data();
	mDataLen = 0;
	mNameMap.clear();
	mFieldValues.clear();
}


bool
AosDataletImp::getFieldValue(
		AosRundata *rdata, 
		const char * const name, 
		AosValueRslt &value) 
{
	if (!mIsDataParsed)
	{
		bool rslt = parseData(rdata);
		aos_assert_r(rslt, false);
	}

	mapitr_t itr = mNameMap.find(name);
	if (itr == mNameMap.end()) 
	{
		// The field does not exist.
		value.reset();
		return true;
	}

	switch (itr->second.status)
	{
	case eFieldModified:
		 value = mFieldValues[itr->second.index];
		 return true;

	case eFieldDeleted:
		 AosSetErrorUser(rdata, "dataletimp_field_deleted") << name << enderr;
		 return false;

	case eCleanEmbeddedDoc:
	case eDirtyEmbeddedDoc:
		 AosSetErrorUser(rdata, "dataletimp_embedded_doc") << name << enderr;
		 return false;

	case eFieldParsed:
		 switch (itr->second.type)
		 {
		 case AosDataType::eU8:
		 	  value.setU32(mData[itr->second.start_pos]);
		 	  return true;

		 case AosDataType::eU16:
		 	  if (mEndian == gAosEndian)
		 	  {
			 	  value.setU32(*(u16*)&mData[itr->second.start_pos]);
				  return true;
			  }
			  value.setU32(AosConvertU16(&mData[itr->second.start_pos]));
		 	  return true;

		 /*
		 case AosDataType::eU32:
		 	  mBuffRaw->setCrtIdx(entry.pos);
		 	  value.setU32(mBuffRaw->getU32(0));
		 	  return true;

		 case AosDataType::eU64:
		 	  mBuffRaw->setCrtIdx(entry.pos);
		 	  value.setU64(mBuffRaw->getU64(0));
		 	  return true;

		 case AosDataType::eChar:
		 	  mBuffRaw->setCrtIdx(entry.pos);
		 	  value.setChar(mBuffRaw->getCHar(0));
		 	  return true;

		 case AosDataType::eInt16:
		 	  mBuffRaw->setCrtIdx(entry.pos);
		 	  value.setInt16(mBuffRaw->getI16(0));
		 	  return true;

		 case AosDataType::eInt32:
		 	  mBuffRaw->setCrtIdx(entry.pos);
		 	  value.setInt32(mBuffRaw->getInt32(0));
		 	  return true;

		 case AosDataType::eInt64:
		 	  mBuffRaw->setCrtIdx(entry.pos);
		 	  value.setInt64(mBuffRaw->getI64(0));
		 	  return true;

		 case AosDataType::eIntFloat:
		 	  mBuffRaw->setCrtIdx(entry.pos);
		 	  value.setFloat(mBuffRaw->getFloat(0));
		 	  return true;

		 case AosDataType::eIntDouble:
		 	  mBuffRaw->setCrtIdx(entry.pos);
		 	  value.setDouble(mBuffRaw->getDouble(0));
		 	  return true;

		 case AosDataType::eIntDouble:
		 	  mBuffRaw->setCrtIdx(entry.pos);
		 	  data = mBuffRaw->getCharStr(len);
		 	  value.setCharStr(data, len);
		 	  return true;
		 */

		 default:
			  AosSetError(rdata, "record_invalid_datatype") << itr->second.type << enderr;
			  return false;
		 }

	default:
		 AosSetError(rdata, "record_invalid_status") << itr->second.status << enderr;
		 return false;
	}

	OmnShouldNeverComeHere;
	return false;
}


bool 		
AosDataletImp::setData(
		char *data,
		const int len,
		const AosBuffDataPtr &metaData,
		const int64_t offset,
		const bool need_copy)
{
	mIsDataParsed = false;
	mMetaData = metaData;
	mOffset = offset;

	if (need_copy)
	{
		if (data)
		{
			mInternalData.assign(data, len);
		}
		else
		{
			mInternalData = "";
			aos_assert_r(len == 0, false);
		}

		mData = (char *)mInternalData.data();
		mDataLen = len;
		mBuff->setData(mData, mDataLen, false);
		return true;
	}
	 	
	mData = data;
	mDataLen = len;
	mBuff->setData(mData, mDataLen, false);
	return true;
}


bool 		
AosDataletImp::setFieldValue( 
		AosRundata *rdata,
		const OmnString &field_name, 
		const AosValueRslt &value) 
{
	// This function checks whether the field exists. If not, 
	// it will create it.
	// Note that this is not thread save. The caller should lock
	// it in case it is accessed by multiple threads.
	mapitr_t itr = mNameMap.find(field_name.data());
	if (itr == mNameMap.end())
	{
		// Field does not exist. Create it.
		mFieldNames.push_back(field_name);
		mFieldValues.push_back(value);
		mIsDirty = true;
		mNameMap[mFieldNames[mFieldNames.size()-1].data()] = FieldInfo(value.getType(),
				-1, 0, mFieldValues.size()-1, eInValueVector);
		return true;
	}
	
	// The field exists. 
	switch (itr->second.status)
	{
	case eFieldParsed:
		 // The field contents are in the input data. Need to change it.
		 mFieldValues.push_back(value);
		 itr->second.index = mFieldValues.size()-1;
		 itr->second.status = eFieldModified;
		 return true;

	case eFieldModified:
		 mFieldValues[itr->second.index] = value;
		 return true;

	case eFieldDeleted:
		 AosSetErrorUser(rdata, "record_jm_modify_deleted_field") << field_name << enderr;
		 return false;

	case eCleanEmbeddedDoc:
	case eDirtyEmbeddedDoc:
		 AosSetErrorUser(rdata, "record_jm_modify_doc_not_allowed") << field_name << enderr;
		 return false;

	default:
		 AosSetError(rdata, "record_jm_unrecog_fieldstatus") << itr->second.status << enderr;
		 return false;
	}

	return true;
}


bool 		
AosDataletImp::setFieldValue( 
		AosRundata *rdata,
		const int field_idx, 
		const AosValueRslt &value) 
{
	OmnShouldNeverComeHere;
	return false;
}


bool 		
AosDataletImp::setFieldValueWPath( 
		AosRundata *rdata,
		const char * const field_name, 
		const AosValueRslt &value) 
{
	int idx;
	FieldInfo *field = getField(rdata, field_name, idx);
	if (!field && field_name[idx] != '.')
	{
		// This is a normal call
		return setFieldValue(rdata, field_name, value);
	}

	if (field_name[idx] == '[')
	{
		// It is to access an array, which is not allowed through
		// this function.
		AosSetErrorUser(rdata, "recordbjimo_access_array_not_allowed") << field_name << enderr;
		return false;
	}	

	AosDataletPtr doc;
	if (!field)
	{
		// The embedded doc does not exist. Need to create it. 
		doc = OmnNew AosDataletImp(mJimoVersion);
		mEmbeddedDocs.push_back(doc);
		OmnString doc_name(field_name);
		mFieldNames.push_back(doc_name);
		mNameMap[doc_name.data()] = FieldInfo(AosDataType::eEmbeddedDoc, 
				-1, 0, mEmbeddedDocs.size()-1, eDirtyEmbeddedDoc);
	}
	else
	{
		int idx = field->index;
		doc = mEmbeddedDocs[idx];
		field->status = eDirtyEmbeddedDoc;
	}

	return doc->setFieldValueWPath(rdata, field_name, value);
}


bool
AosDataletImp::getFieldValueWPath(
		AosRundata *rdata, 
		const char * const name, 
		AosValueRslt &value) 
{
	int idx;
	FieldInfo *field = getField(rdata, name, idx);
	if (!field)
	{
		// This is a normal call
		return getFieldValue(rdata, name, value);
	}

	if (name[idx] == '[')
	{
		// It is to access an array, which is not allowed through
		// this function.
		AosSetErrorUser(rdata, "recordbjimo_access_array_not_allowed") << name << enderr;
		return false;
	}	

	// Check whether it has been created.
	if (field->index < 0)
	{
		// It has not been created yet. 
		AosDataletPtr doc = OmnNew AosDataletImp(mJimoVersion);
		doc->setData(&mData[field->start_pos], field->len, mMetadata, mOffset, false);
		mEmbeddedDocs.push_back(doc);
		field->index = mEmbeddedDocs.size()-1;
	}
	return mEmbeddedDocs[field->index]->getFieldValueWPath(rdata, &name[idx+1], value);
}


bool
AosDataletImp::parseData(AosRundata *rdata)
{
	// Data are saved as:
	// 		DocType			(1 byte)
	// 		Endian			(one byte, 1:little endian, 2:big endian)
	// 		Length			(variable)
	// 		FieldType		1 byte
	// 		FieldName		char *	
	// 		FieldContents
	// 		...
	// 		FieldType		1 byte
	// 		FieldName		char *	
	// 		FieldContents
	// 		0x00
	aos_assert_r(mData, false);
	
		 
	const char *field_name;
	i32 crt_pos = 0;
	const char *data = mData;
	int bytes_used, field_len;
	AosDataType::E field_type;
	bool duplicated_field;
	mIsDirty = false;
	int field_start;
	mapitr_t itr;

	AosDataletType::E doc_type = (AosDataletType::E)mData[0];
	aos_assert_r(doc_type == AosDataletType::eBinary, false);
	mEndian = mData[1];
	mDataletLen = AosParseVInt(&mData[2], bytes_used);

	crt_pos = 2 + bytes_used;

	// Retrieve all the fields for this level
	while (crt_pos < mDataLen)
	{
		aos_assert_r(crt_pos + 3 < mDataLen, false);

		// Get type
		duplicated_field = false;
		field_type = (AosDataType::E)data[crt_pos++];

		// Get the name 
		field_name = &data[crt_pos];
		while (crt_pos < mDataLen && data[crt_pos++]);

		itr = mNameMap.find(field_name);
		if (itr != mNameMap.end())
		{
			duplicated_field = true;
			if (!mIgnoreDuplicatedFields)
			{
				AosSetErrorUser(rdata, "recordbson_duplicate_field_found") << field_name << enderr;
				return false;
			}
		}

		// crt_pos points to the start of the value (if any)
		field_start = crt_pos;
		field_len = -1;
		switch (field_type)
		{
		case AosDataType::eUTF8Str:
			 // vint bytes* 0x00
			 // 'vint' is the length of this field (does not include itself)
			 // but include 0x00. 

		case AosDataType::eEmbeddedDoc:
			 // vint e_list 0x00
			 // The length does not include itself but include 0x00

		case AosDataType::eArray:
			 // vint			Array Size
			 // element_type 	(1 byte)
			 // int32			(num elements)
			 // elements*

		case AosDataType::eBinary:
			 // int32 subtype(one byte) (bytes*)
			 field_len = AosParseVInt(&data[crt_pos], bytes_used);
			 field_start = crt_pos + bytes_used;
			 crt_pos += bytes_used + field_len;
			 break;

		case AosDataType::eNull:
			 break;

		case AosDataType::eDouble:
			 crt_pos += 8;
			 break;

		case AosDataType::eBool:
			 crt_pos++;
			 break;

		case AosDataType::eInt32:
			 crt_pos += 4;
			 break;

		case AosDataType::eInt64:
			 crt_pos += 8;
			 break;

		default:
			 OmnAlarm << "Unrecognized type: " << field_type << enderr;
			 return false;
		}

		if (!duplicated_field)
		{
			mNameMap[field_name] = FieldInfo(field_type, field_start, field_len, 0, eFieldParsed);
		}
	}
	return true;
}


bool
AosDataletImp::serializeToBuff(
		AosRundata *rdata, 
		AosBuff *buff)
{
	// It serializes the contents and returns the binary.
	if (!mIsDirty) return mData;

	mapitr_t itr = mNameMap.begin();
	while (itr != mNameMap.end())
	{
		switch (itr->second.status)
		{
		case eFieldModified:
			 mFieldValues[itr->second.index].serializeToJM(buff);
			 break;

		case eCleanEmbeddedDoc:
			 buff->setBuff(&mData[itr->second.start_pos], itr->second.len);	
			 break;

		case eDirtyEmbeddedDoc:
			 mEmbeddedDocs[itr->second.index]->serializeToBuff(rdata, buff);
			 break;

		case eFieldParsed:
			 switch (itr->second.type)
			 {
			 case AosDataType::eU8:
				  encodeFieldName(rdata, buff, itr->first);
				  buff->setU8((u8)mData[itr->second.start_pos]);
			 	  break;
			 
			 case AosDataType::eU16:
				  encodeFieldName(rdata, buff, itr->first);
				  AosEncodeU16(buff, mEndian, *(u16*)&mData[itr->second.start_pos]);
			 	  break;
			 
			 case AosDataType::eU32:
				  encodeFieldName(rdata, buff, itr->first);
				  AosEncodeU32(buff, mEndian, *(u32*)&mData[itr->second.start_pos]);
			 	  break;
			 
			 case AosDataType::eU64:
				  encodeFieldName(rdata, buff, itr->first);
				  AosEncodeU64(buff, mEndian, *(u64*)&mData[itr->second.start_pos]);
			      break;
			 
			 case AosDataType::eChar:
				  encodeFieldName(rdata, buff, itr->first);
				  buff->setChar((char)mData[itr->second.start_pos]);
			 	  break;
			 
			 case AosDataType::eInt16:
			  	  break;
			 
			 case AosDataType::eInt32:
			 	  break;
			 
			 case AosDataType::eInt64:
			 	  break;
			 
			 case AosDataType::eFloat:
			 	  break;
			 
			 case AosDataType::eDouble:
			  	  break;
			 
			 case AosDataType::eCharStr:
			 	  break;
			 
			 default:
			 	  OmnAlarm << "Unrecognized data type: " << itr->second.type << enderr;
				  return false;
			 }

		default:
			 OmnAlarm << "Invalid field status: " << itr->second.status << enderr;
			 return false;
		}
		itr++;
	}

	return true;
}


AosDataletPtr 
AosDataletImp::clone() const
{
	try
	{
		return OmnNew AosDataletImp(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return 0;
	}
}


AosJimoPtr
AosDataletImp::cloneJimo() const
{
	try
	{
		return OmnNew AosDataletImp(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return 0;
	}
}


/*
AosDataRecordObjPtr 
AosDataletImp::getChild(AosRundata *rdata, const char *name)
{
	aos_assert_r(rdata, false);
	aos_assert_r(name, false);
	int idx = 0;
	char c;
	while ((c = name[idx]) && (c != '[' && c != '.')) idx++;
	if (c == '[')
	{
		// This is not allowed.
		return 0;
	}

	if (c == 0)
	{
		mapitr_t itr = mNameMap.find(name);
		if (itr == mNameMap.end()) return 0;
		if (itr->second.index < 0)
		{
			AosDataletImpPtr doc = OmnNew AosDataletImp(
				&mData[itr->second.start_pos], itr->second.len);
			mEmbeddedDocs.push_back(doc);
			itr->second.index = mEmbeddedDocs.size()-1;
		}
		return mEmbeddedDocs[itr->second.index];
	}

	char c = ((char *)name)[idx];
	((char *)name)[idx] = 0;
	mapitr_t itr = mNameMap.find(name);
	((char *)name)[idx] = c;
	if (itr == mNameMap.end())
	{
		AosDataletImpPtr doc = OmnNew AosDataletImp(
			&mData[itr->second.start_pos], itr->second.len);
		mEmbeddedDocs.push_back(doc);
		itr->second.index = mEmbeddedDocs.size()-1;
	}
	return mEmbeddedDocs[itr->second.index]->getChild(rdata, &name[idx]);
}


AosGenArrayPtr
AosDataletImp::addArray(
		AosRundata *rdata, 
		const char *name, 
		const AosDataType::E type, 
		const int size)
{
	mapitr_t itr = mNameMap.find(name);
	if (itr != mNameMap.end())
	{
		AosSetErrorUser(rdata, "recordbjimo_name_already_exist") << name << enderr;
		return 0;
	}

	AosGenArrayPtr array = OmnNew AosGenArray(type, size);
	mArrays.push_back(array);
	mNameMap[name] = FieldInfo(AosDataType::eArray, -1, 0, mArrays.size()-1, eFieldParsed);
	return array;
}


bool
AosDataletImp::removeAttr(AosRundata *rdata, const char *name)
{
	mapitr_t itr = mNameMap.find(name);
	if (itr == mNameMap.end())
	{
		// Field does not expst. 
		return true;
	}

	itr->second.status = eFieldDeleted;
	return true;
}


bool
AosDataletImp::removeAttrWPath(AosRundata *rdata, const char *name)
{
	int idx;
	FieldInfo *field = getEmbeddedDoc(rdata, name, idx);
	if (!field)
	{
		// It does not identify an embedded doc.
		return true;
	}

	field->status = eFieldDeleted;
	return true;
}


AosDataletImp::FieldInfo *
AosDataletImp::getField(AosRundata *rdata, const char *name, int &idx)
{
	aos_assert_r(rdata, false);
	aos_assert_r(name, false);
	idx = 0;
	char c;
	while ((c = name[idx]) && (c != '[' && c != '.')) idx++;
	if (!c) return 0;

	char c = ((char *)name)[idx];
	((char *)name)[idx] = 0;
	mapitr_t itr = mNameMap.find(name);
	((char *)name)[idx] = c;
	mapitr_t itr = mNameMap.find(name);
	if (itr == mNameMap.end()) return 0;
	return &itr->second;
}


AosGenArrayPtr
AosDataletImp::getArray(
		AosRundata *rdata, 
		const char *name, 
		AosValueRslt &value)
{
	int idx;
	FieldInfo *field = getField(rdata, name, idx);
	if (!field) return 0;

	if (name[idx] == '.')
	{
		if (field->index < 0)
		{
			AosDataletImpPtr doc = OmnNew AosDataletImp(
				&mData[field->start_pos], field->len);
			mEmbeddedDocs.push_back(doc);
			field->index = mEmbeddedDocs.size()-1;
		}
		return mEmbeddedDocs[field->index]->getArray(rdata, &name[idx]);
	}

	if (field->type != AosDataType::eArray)
	{
		AosSetErrorUser(rdata, "recordbjimo_not_array") << name << enderr;
		return 0;
	}

	if (field->index < 0)
	{
		AosGenArrayPtr array = OmnNew AosGenArray(field->type, 
				&mData[field->start_pos], field->len);
		mArrays.push_back(array);
		field->index = mArrays.size()-1;
	}

	return mArrays[field->index];
}
*/
#endif
