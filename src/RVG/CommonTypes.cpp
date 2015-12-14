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
//   
//
// Modification History:
// 11/21/2007: Created by Allen Xu
//
////////////////////////////////////////////////////////////////////////////
#include "RVG/CommonTypes.h"

#include "alarm/Alarm.h"
#include "RVG/Table.h"

#define SEPERATOR_BINARY ' '


bool byteStringToStream(const OmnString & byteString, char *byteStream, unsigned int &len)
{
	OmnNotImplementedYet;
	/*
	 * Need to convert the std::string portion to OmnString, Chen Ding, 12/16/2008
	unsigned int byteStringLen = byteString.length();
	if (len <  byteStringLen / 3)
	{
		len = byteStringLen/3+1;
		return false;
	}

	OmnString byteStr = byteString;

	OmnString tmp;
	unsigned int tmpChar;
	unsigned int pos = byteStr.find(SEPERATOR_BINARY);
	len = 0;
	while (pos != OmnString std::string::npos)
	{
		tmp = byteStr.substr(0, pos);
		sscanf(tmp.c_str(), "%x", &tmpChar);	
		byteStream[len++] = tmpChar;
	
		byteStr = byteStr.substr(pos+1);
		pos = byteStr.find(SEPERATOR_BINARY);
	}
	if (byteStr.length() == 2)
	{
		sscanf(byteStr.c_str(), "%x", &tmpChar);	
		byteStream[len++] = tmpChar;
	}
	*/

	return true;
}


bool byteStreamToString(const char* byteStream, unsigned int len, OmnString &byteString)
{
	unsigned int i, j;
	char tmp[3]={0};

	for (i = 0, j = 0; i < len; i++)
	{
		sprintf(tmp, "%x", byteStream[i]);
		byteString += tmp;
		byteString += SEPERATOR_BINARY;
	}
	return true;
}


AosValue::AosValue()
{
	mLen = 0;
	mValue = NULL;
	mType = AosValueType::eString;
}


AosValue::AosValue(AosValueType::E type):
mType(type)
{
	mLen = 0;
	mValue = NULL;
}


AosValue::AosValue(int length, AosValueType::E type):
mLen(length),
mType(type)
{
	mValue = new unsigned char[length+1];
}


AosValue::AosValue(const AosValue& value)
{
	mLen = 0;
	mValue = NULL;
	if (value.mLen == 0)
	{
		return;
	}
	if (this != &value)
	{
		if (mLen != value.mLen) 
		{
			if (mValue != NULL)
			{
				delete [] mValue;
				mValue = NULL;
			}
			mLen = value.mLen;
			mValue = new unsigned char[mLen+1];
			mValue[mLen] = '\0';
		}
		memcpy(mValue, value.mValue, mLen);
		mType = value.mType;
	}	
}


AosValue::~AosValue()
{
	if (mValue != NULL)
	{
		delete [] mValue;
	}
}


bool 
AosValue::serialize(TiXmlNode& node)
{
	//
	// <Value>
	// 		<ValueType>string/byteStream/bool/int8_t/int16_t/u8/u16...</ValueType>
	// 		<ByteString>xx xx xx xx xx...</ByteString>
	// </Value>
	//
	node.SetValue("Value");
	//
	// add ValueType node
	//
	node.addElement("ValueType", AosValueType::enum2Str(mType));
	//
	// add ByteString node
	//
	OmnString destStr;
	byteStreamToString((const char*)mValue, mLen, destStr);
	node.addElement("ByteString", destStr);

	return true;
}


bool 
AosValue::deserialize(TiXmlNode* node)
{
	//
	// <Value>
	// 		<ValueType>string/byteStream/bool/int8_t/int16_t/u8/u16...</ValueType>
	// 		<ByteString>xx xx xx xx xx...</ByteString>
	// </Value>
	//
	if (node == NULL)
	{
		return false;
	}
	if (node->ValueStr() != "Value")
	{
		return false;
	}
	//
	// parse value type, mandatory field
	//
	TiXmlHandle docHandle(node);
	TiXmlElement* element = docHandle.FirstChild("ValueType").ToElement();
	if (element)
	{
		const char* typeStr = element->GetText();
		if (!typeStr)
		{
			return false;
		}
		mType = AosValueType::str2Enum(typeStr);
	}

	//
	// parse value type, mandatory field
	//
	OmnString byteStr;
	if (!node->getElementTextValue("ByteString", byteStr))
	{
		OmnAlarm << "XML parse: ByteString field must exist in AosValue" << enderr;
		return false;
	}

	unsigned int len = MAX_AosValue_BUFFER_SIZE;
	char* buffer = new char[len];
	if (!byteStringToStream(byteStr, buffer, len))
	{
		delete [] buffer;
		OmnAlarm << "XML parse: The ByteString is too long, the length must less than " << MAX_AosValue_BUFFER_SIZE << enderr;
		return false;
	}
	setValue(buffer, len);
	delete [] buffer;

	return true;
}


AosValue& 
AosValue::operator=(const AosValue& value)
{
	if (value.mLen == 0)
	{
		return *this;
	}
	if (mLen != value.mLen)
	{
	   	if ( mValue != NULL)
		{
			delete [] mValue;
			mValue = NULL;
		}
		mLen = value.mLen;
		mValue = new unsigned char[mLen+1];
		mValue[mLen] = '\0';
	}
	memcpy(mValue, value.mValue, mLen);
	mType = value.mType;
	return *this;
}


bool 
operator==(const AosValue& valueLeft, const AosValue& valueRight)
{
	if (valueLeft.mType != valueRight.mType)
	{
		OmnAlarm << "The value type is different, can not compare them" << enderr;
		return false;
	}
	if (valueLeft.mLen == valueRight.mLen && !memcmp(valueLeft.mValue, valueRight.mValue, valueLeft.mLen))
	{
		return true;
	}
	return false;
}


bool 
operator>(const AosValue& valueLeft, const AosValue& valueRight)
{
	if (valueLeft.mType != valueRight.mType)
	{
		OmnAlarm << "The value type is different, can not compare them" << enderr;
		return false;
	}
	if (valueLeft.mLen == 0)
	{
		return false;
	}
	if (valueRight.mLen == 0)
	{
		return true;
	}
	unsigned int minLen = MIN_VALUE(valueLeft.mLen, valueRight.mLen);
	if (memcmp(valueLeft.mValue, valueRight.mValue, minLen) > 0)
	{
		return true;
	}
	return false;
}


bool 
operator<(const AosValue& valueLeft, const AosValue& valueRight)
{
	if (valueLeft.mType != valueRight.mType)
	{
		OmnAlarm << "The value type is different, can not compare them" << enderr;
		return false;
	}
	if (valueRight.mLen == 0)
	{
		return false;
	}
	if (valueLeft.mLen == 0)
	{
		return true;
	}
	unsigned int minLen = MIN_VALUE(valueLeft.mLen, valueRight.mLen);
	if (memcmp(valueLeft.mValue, valueRight.mValue, minLen) < 0)
	{
		return true;
	}
	return false;
}


void 
AosValue::setValue(void* buffer, unsigned int len)
{
	if (len <= 0)
	{
		return;
	}
	if (mLen != len) 
	{
		if (mValue != NULL)
			delete [] mValue;
		mLen = len;
		mValue = new unsigned char[len+1];
		mValue[len] = '\0';
	}
	memcpy(mValue, buffer, mLen);
}
	

void 
AosValue::getValue(void* &buffer, unsigned int &len) const
{
	buffer = mValue;
	len = mLen;
}
	

void 
AosValue::getValue(void* buffer) const
{
	if (buffer == NULL)
	{
		return;
	}
	memcpy(buffer, mValue, mLen);
}


void 
AosValue::setType(AosValueType::E type)
{
	mType = type;
}


int 
AosValue::getType() const
{
	return mType;
}


bool 
AosValue::toBool() const
{
	if (mType != AosValueType::eBool)
	{
		OmnAlarm << "type error! The expected type is bool" << enderr;
		return false;
	}
	return convertAosValue<bool>(*this);
}


int8_t 
AosValue::toInt8() const
{
	if (mType != AosValueType::eInt8)
	{
		OmnAlarm << "type error! The expected type is int8" << enderr;
		return 0;
	}
	return convertAosValue<int8_t>(*this);
}


int16_t 
AosValue::toInt16() const
{
	if (mType != AosValueType::eInt16)
	{
		OmnAlarm << "type error! The expected type is int16" << enderr;
		return 0;
	}
	return convertAosValue<int16_t>(*this);
}


int32_t 
AosValue::toInt32() const
{
	if (mType != AosValueType::eInt32)
	{
		OmnAlarm << "type error! The expected type is u32" << enderr;
		return 0;
	}
	return convertAosValue<int32_t>(*this);
}


int64_t 
AosValue::toInt64() const
{
	if (mType != AosValueType::eInt64)
	{
		OmnAlarm << "type error! The expected type is int64" << enderr;
		return 0;
	}
	return convertAosValue<int64_t>(*this);
}


uint8_t 
AosValue::toUint8() const
{
	if (mType != AosValueType::eU8)
	{
		OmnAlarm << "type error! The expected type is u8" << enderr;
		return 0;
	}
	return convertAosValue<uint8_t>(*this);
}


uint16_t 
AosValue::toUint16() const
{
	if (mType != AosValueType::eU16)
	{
		OmnAlarm << "type error! The expected type is u16" << enderr;
		return 0;
	}
	return convertAosValue<uint16_t>(*this);
}


uint32_t 
AosValue::toUint32() const
{
	if (mType != AosValueType::eU32)
	{
		OmnAlarm << "type error! The expected type is u32" << enderr;
		return 0;
	}
	return convertAosValue<uint32_t>(*this);
}


uint64_t 
AosValue::toUint64() const
{
	if (mType != AosValueType::eU64)
	{
		OmnAlarm << "type error! The expected type is u64" << enderr;
		return 0;
	}
	return convertAosValue<uint64_t>(*this);
}


float 
AosValue::toFloat() const
{
	if (mType != AosValueType::eFloat)
	{
		OmnAlarm << "type error! The expected type is float" << enderr;
		return 0;
	}
	return convertAosValue<float>(*this);
}
	

double 
AosValue::toDouble() const
{
	if (mType != AosValueType::eDouble)
	{
		OmnAlarm << "type error! The expected type is double" << enderr;
		return 0;
	}
	return convertAosValue<double>(*this);
}


OmnString 
AosValue::toString() const
{
	if (mValue == NULL || mLen <= 0)
	{
		return "";
	}
	OmnString destStr;
	char destBuf[10]={0};
	switch (mType)
	{
		case AosValueType::eBool:
			{
				bool value = toBool();
				destStr = value ? "true" : "false";
			}
			break;
		case AosValueType::eU8:
			{
				u8 value = toUint8();
				sprintf(destBuf, "%u", value);
				destStr = destBuf;
			}
			break;
		case AosValueType::eU16:
			{
				u16 value = toUint16();
				sprintf(destBuf, "%u", value);
				destStr = destBuf;
			}
			break;
		case AosValueType::eU32:
			{
				u32 value = toUint32();
				sprintf(destBuf, "%u", value);
				destStr = destBuf;
			}
			break;
		case AosValueType::eU64:
			{
				u64 value = toUint64();
				sprintf(destBuf, "%llu", value);
				destStr = destBuf;
			}
			break;
		case AosValueType::eInt8:
			{
				int8_t value = toInt8();
				sprintf(destBuf, "%d", value);
				destStr = destBuf;
			}
			break;
		case AosValueType::eInt16:
			{
				int16_t value = toInt16();
				sprintf(destBuf, "%d", value);
				destStr = destBuf;
			}
			break;
		case AosValueType::eInt32:
			{
				int32_t value = toInt32();
				sprintf(destBuf, "%d", value);
				destStr = destBuf;
			}
			break;
		case AosValueType::eInt64:
			{
				int64_t value = toInt64();
				sprintf(destBuf, "%lld", value);
				destStr = destBuf;
			}
			break;
		case AosValueType::eFloat:
			{
				float value = toFloat();
				sprintf(destBuf, "%f", value);
				destStr = destBuf;
			}
			break;
		case AosValueType::eDouble:
			{
				double value = toDouble();
				sprintf(destBuf, "%f", value);
				destStr = destBuf;
			}
			break;
		case AosValueType::eString:
			{
				char* buffer = (char*)mValue;	
				buffer[mLen] = '\0';
				destStr = buffer;
			}
			break;	
		case AosValueType::eByteStream:
		case AosValueType::eUnknown:
		default:
			byteStreamToString((const char*)mValue, mLen, destStr);
			break;
	}
	return destStr;
}


bool 
AosValue::fromString(OmnString &str)
{
	return true;
}


const char* 
AosValue::itoa(int32_t integer)
{
	char dest[20]={0};
	sprintf(dest, "%d", integer);
	return OmnString(dest).data();
}


void
AosValue::setTable(const AosTablePtr &table)
{
	mTable = table;
	mType = AosValueType::eTable;
}


AosTablePtr
AosValue::toTable() const
{
	aos_assert_r(mType == AosValueType::eTable, 0);
	return mTable;
}

