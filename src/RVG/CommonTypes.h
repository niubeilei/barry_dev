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
#ifndef Aos_RVG_COMMON_TYPES_H
#define Aos_RVG_COMMON_TYPES_H

#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <sys/time.h>
#include "aosUtil/Types.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Alarm/Alarm.h"
#include "RVG/Util.h"
#include "RVG/Ptrs.h"
#include "XmlLib/tinyxml.h"

#define MIN_VALUE(x,y) ((x)>(y)?(y):(x))
#define MAX_Value(x,y) ((x)>(y)?(x):(y))

class AosValue;
typedef std::vector<AosValuePtr> AosValuePtrArray;

//
// All kinds of data are stored as BYTE Stream in
// memory. the AosValue will allocate one more BYTE
// memory size than the data size because of the string.
//
// The value type will be changed in these cases.
//		1. constructor function, default is string type. 
//		2. setType() function 
//		3. deserialize() function
//		4. operator= function
//
// These functions will check the value type.
//		1.operator==,>,<
//		2.toBool, toInt8, ..., toString...
//
class AosValue : virtual public OmnRCObject
{
	OmnDefineRCObject;
public:
	enum
	{
		MAX_AosValue_BUFFER_SIZE = 10240
	};

private:
	unsigned char * mValue;
	unsigned int 	mLen;
	AosTablePtr		mTable;		// Added by Chen Ding, 01/28/2008

	AosValueType::E	mType;	

public:
	AosValue();
	AosValue(AosValueType::E type);
	AosValue(int length, AosValueType::E type);
	AosValue(const AosValue& value);
	~AosValue();
	bool serialize(TiXmlNode& node);
	bool deserialize(TiXmlNode* node);
	
	AosValue& operator=(const AosValue& value);
	friend bool operator==(const AosValue& valueLeft, const AosValue& valueRight);
	friend bool operator>(const AosValue& valueLeft, const AosValue& valueRight);
	friend bool operator<(const AosValue& valueLeft, const AosValue& valueRight);
	void 		setValue(void* buffer, unsigned int len);
	void 		getValue(void* &buffer, unsigned int &len)const;
	void 		getValue(void* buffer)const;
	void 		setType(AosValueType::E type);
	int  		getType()const;

	void		setTable(const AosTablePtr &table);
	AosTablePtr	toTable() const;
	
	bool 		toBool()const;
	int8_t 		toInt8()const;
	int16_t 	toInt16()const;
	int32_t 	toInt32()const;
	int64_t 	toInt64()const;
	
	uint8_t 	toUint8()const;
	uint16_t 	toUint16()const;
	uint32_t 	toUint32()const;
	uint64_t 	toUint64()const;
	
	float 		toFloat()const;
	double 		toDouble()const;
	
	OmnString toString()const;
	bool 		fromString(OmnString &str);

	static const char* itoa(int32_t integer);

};


//
// byte string to byte stream buffer
// string ("xx xx xx xx ...") -----> memory byte stream (xxxxxxxx...)
//
// input:
//		byteString
//		byteStream : stream buffer
//		len		   : stream buffer length
// output:
//		byteStream : output stream buffer
//		len		   : output stream buffer length
// return:
//		bool, if it returns false, that means the len of the byteStream is not 
//		enough, and it will give the least length needed. if it return true, 
//		the len will return the length of the byte stream.
//
extern bool byteStringToStream(const OmnString &byteString, char*byteStream, unsigned int &len);


//
// byte stream to byte string
// memory byte stream (xxxxxxxx...)  -----> string ("xx xx xx xx ...")
//
// input:
//		byteStream
//		len
// output:
//		byteString	
// return:
//		bool, always it is true except the system memory is not enough.
//
extern bool byteStreamToString(const char* byteStream, unsigned int len, OmnString &byteString);


//
// Randomly get an integer in the [min, max] range
// T can be u8,u16,u32,u64,int8_t,int16_t,int32_t,int64_t
//
template<class T>
inline T aos_next_integer(T min, T max)
{
	if (min > max)
	{
		OmnAlarm << "min > max: " << min << ":" << max << enderr;
		return min;
	}

	unsigned int space = abs(max - min) +1;

	struct timeval timeValue;
	gettimeofday(&timeValue, NULL);
	srand(timeValue.tv_usec);
	return rand()%space + min;
}


//
// Randomly get a integer quene between [min, max]
//
template<class T>
void aos_get_queue(T min, T max, std::vector<T> &queue)
{
	if (min > max)
	{
		OmnAlarm << "min > max: " << min << ":" << max << enderr;
		return;
	}
}


//
// used by AosValue to convert kinds of values
//
template<class T>
inline T convertAosValue(const AosValue& aosValue)
{
	void* aosBuffer;
	unsigned int aosLen;
	aosValue.getValue(aosBuffer, aosLen);
	
	unsigned int len = sizeof(T);
	T value;
	if (aosLen < len)
	{
		OmnAlarm << "Should not come here, can not convert to your type" << enderr;
		return 0;
	}
	memcpy(&value, aosBuffer, len);
	return value;
}

#endif
