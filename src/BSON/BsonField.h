////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2014/08/17 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_BSON_BsonField_h
#define Aos_BSON_BsonField_h

#include "Jimo/Jimo.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/ValueRslt.h"


class AosBsonField : public AosJimo
{
public:/*
	enum Type
	{
	   //use for prove the value type edited by rain in 2014/12/17
		eInvalidFieldType, 

		eFieldTypeDouble			= 0x01,
		eFieldTypeString			= 0x02,
		
		eFieldTypeBoolFalse         = 0x04,
		eFieldTypeBoolTrue          = 0x05,
		
		eFieldTypeNull				= 0x07,
		eFieldTypeInt64				= 0x08,
        eFieldTypeInt40             = 0x09,
		eFieldTypeInt32				= 0x0a,
        eFieldTypeInt24             = 0x0b,
		eFieldTypeInt16				= 0x0c,
		eFieldTypeInt8				= 0x0d,
		eFieldTypeU64				= 0x0e,
        eFieldTypeU40               = 0x0f,
		eFieldTypeU32				= 0x10,
        eFieldTypeU24 				= 0x11,
		eFieldTypeU16				= 0x12,
		eFieldTypeU8				= 0x13,

		eFieldTypeDocument			= 0x20,
		eFieldTypeArray				= 0x21,
        
		eFieldTypeGenericArray      = 0x99,  // defined by rain
		eFieldTypeFloat				= 0x98,  // lost in doc from ding
		eMaxBsonFieldType
	};*/
	enum Type
    {
	     eInvalidFieldType, 
		 
		 eFieldTypeDouble            = 0x01,
		 eFieldTypeString            = 0x02,
		 eFieldTypeDocument          = 0x03,
		 eFieldTypeArray             = 0x04,
		 eFieldTypeChar              = 0x05,
		 eFieldTypeBool              = 0x06,
		 eFieldTypeBinary            = 0x07,          
         eFieldTypeMap               = 0x08,  //rain 2015-01-30
		 eFieldTypeNull              = 0x09,
		 eFieldTypeInt32             = 0x0a,
         eFieldTypeGenericArray      = 0x0b,  
		 eFieldTypeInt64             = 0x0c,
		 eFieldTypeInt8              = 0x0d,
		 eFieldTypeInt16             = 0x0e,
		 eFieldTypeU8                = 0x0f,
		 eFieldTypeU16               = 0x10,
		 eFieldTypeU32               = 0x11,
		 eFieldTypeU64               = 0x12,
		 eFieldTypeVarInt            = 0x13,
		 eFieldTypeVarUint           = 0x14, 
		 eFieldTypeFloat             = 0x15,
         
		 eMaxBsonFieldType
	 };
	enum E
	{
		eFieldStart = 0x01, 
		eNumFields = 70,

		eStringFieldName = 0x01,     //use for judge the Name Type and prove it's String
		eDictionaryFieldName = 0x02,    // Not use now
		eNumericalFieldName = 0x03   // use for judge the Name Type and prove it's U32
	};

protected:
	Type		mType;

	static OmnString smClassnames[256];

public:
	AosBsonField(const int version, const Type type);
	~AosBsonField();

	Type getFieldType() {return mType;}

	virtual bool setField(const OmnString &name, const u64 &value, AosBuff *buff_raw);
	virtual bool setField(const OmnString &name, const u32 value, AosBuff *buff_raw);
	virtual bool setField(const OmnString &name, const u16 value, AosBuff *buff_raw);
	virtual bool setField(const OmnString &name, const u8  value, AosBuff *buff_raw);
	virtual bool setField(const OmnString &name, const i64 &value, AosBuff *buff_raw);
	virtual bool setField(const OmnString &name, const i32 value, AosBuff *buff_raw);
	virtual bool setField(const OmnString &name, const i16 value, AosBuff *buff_raw);
	virtual bool setField(const OmnString &name, const i8  value, AosBuff *buff_raw);
	virtual bool setField(const OmnString &name, const bool  value, AosBuff *buff_raw);
	virtual bool setField(const OmnString &name, const char  value, AosBuff *buff_raw);
	virtual bool setField(const OmnString &name, const float value, AosBuff *buff_raw);
	virtual bool setField(const OmnString &name, const double value, AosBuff *buff_raw);
	virtual bool setField(const OmnString &name, const OmnString &value, AosBuff *buff_raw);
	virtual bool setField(const OmnString &name, AosBuff *buff_raw);    //value is null
	//binary: value is buff type
	virtual bool setField(const OmnString &name, const AosBuff* value, AosBuff *buff_raw);
    //map
	virtual bool setField(map<OmnString, OmnString> &values, AosBuff *buff_raw);
    //Array
	virtual bool setField(const OmnString &name, vector<AosValueRslt>& values, AosBuff *buff_raw);  

	
	virtual bool setField(const u32 name, const u64 &value, AosBuff *buff_raw);
	virtual bool setField(const u32 name, const u32 value, AosBuff *buff_raw);
	virtual bool setField(const u32 name, const u16 value, AosBuff *buff_raw);
	virtual bool setField(const u32 name, const u8  value, AosBuff *buff_raw);
	virtual bool setField(const u32 name, const i64 &value, AosBuff *buff_raw);
	virtual bool setField(const u32 name, const i32 value, AosBuff *buff_raw);
	virtual bool setField(const u32 name, const i16 value, AosBuff *buff_raw);
	virtual bool setField(const u32 name, const i8  value, AosBuff *buff_raw);
	virtual bool setField(const u32 name, const bool  value, AosBuff *buff_raw);
	virtual bool setField(const u32 name, const char  value, AosBuff *buff_raw);
	virtual bool setField(const u32 name, const float value, AosBuff *buff_raw);
	virtual bool setField(const u32 name, const double value, AosBuff *buff_raw);
	virtual bool setField(const u32 name, const OmnString &value, AosBuff *buff_raw);
	virtual bool setField(const u32 name, AosBuff *buff_raw);
    //binary: value is buff type
	virtual bool setField(const u32 name, const AosBuff* value, AosBuff *buff_raw);

	/*virtual bool getFieldValue(
						AosRundata *rdata, 
						AosBuff *buff, 
						i64 &pos, 
						const bool copy_flag,
						AosValueRslt &value) = 0;*/

	virtual bool appendFieldValue(
						AosRundata *rdata, 
						const AosValueRslt &value,
						AosBuff *buff) = 0;

	virtual bool skipValue(AosRundata *rdata, 
						AosBuff *buff, 
						i64 &pos) = 0;

	virtual bool setType(AosRundata *rdata,
						AosBuff *buff) = 0;

	virtual bool xpathSetAttr(
						AosRundata *rdata, 
						AosBuffPtr buff,
						const vector<OmnString> &fnames, 
						int crt_idx,
						const AosValueRslt &value, 
						const bool create);

	virtual bool appendFieldValue(
						AosRundata *rdata,
						//const char data_type,
						const AosBsonField::Type data_type,
						const AosValueRslt &value,
						AosBuff *buff);
    
	virtual bool getFieldValue(
						AosRundata *rdata, 
						AosBuff *buff, 
						const int idx,
						i64 &pos, 
						const bool copy_flag,
						AosValueRslt &value);

	//Generic Array
	virtual bool getFieldValue(
						AosRundata *rdata, 
						AosBuff *buff, 
					//	const int idx,
						i64 &pos, 
						const bool copy_flag,
						AosValueRslt &value);
	
	static bool skipEname(AosRundata *rdata, 
						AosBuff *buff, 
						i64 &pos);

	static bool getName(AosRundata *rdata, 
						AosBuff *buff, 
						OmnString &name,
						i64 &pos); 
	static bool getName(AosRundata *rdata, 
						AosBuff *buff, 
						u32 &name,
						i64 &pos); 

	virtual bool isIgnoreSerialize(){return false;}

	static AosBsonField **initFields();
	static inline bool isEname(const u8 id) {return id >= eFieldStart && id < eFieldStart + eNumFields;}
	static inline bool isDname(const u8 id) {return id >= eFieldStart + eNumFields && id < eFieldStart + 2*eNumFields;}
	static inline bool isNname(const u8 id) {return id >= eFieldStart + 2*eNumFields && id < eFieldStart + 3*eNumFields;}

	static bool setFieldNameStr(const OmnString &name, AosBuff *buff_raw);
	static bool setFieldNameU32(const u32 name, AosBuff *buff_raw);
    static bool isValid(const i8 id);	
private:
	bool init();
};

#endif

