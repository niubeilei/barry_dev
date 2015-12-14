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
#ifndef Aos_BSON_BSON_h
#define Aos_BSON_BSON_h

#include "BSON/BsonField.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Buff.h"
#include "Util/Ptrs.h"
#include "UtilComm/Ptrs.h"
#include "BSON/Ptrs.h"
#include "BSON/BsonValue.h"
#include "BSON/BsonValueStrMap.h"


class AosBSON : virtual public OmnRCObject
{
	OmnDefineRCObject;

	enum 
	{
		eDefaultBuffSize = 5000,
		eMaxFieldLevel = 100
	};


private:
	typedef hash_map<const OmnString, AosBsonValuePtr, Omn_Str_hash, compare_str> map_t;
	typedef hash_map<const OmnString, AosBsonValuePtr, Omn_Str_hash, compare_str>::iterator itr_t;
	typedef hash_map<const u32, AosBsonValuePtr, u32_hash, u32_cmp> u32map_t;
	typedef hash_map<const u32, AosBsonValuePtr, u32_hash, u32_cmp>::iterator u32itr_t;

	map_t			mMap;    
	u32map_t		mU32Map; 

	AosBuffPtr		mBuff;
	AosBuff*		mBuffRaw;
	u32				mDocLength;
	u32 			mDefaultBuffSize;
	bool			mParsed;
	bool			mIsGood;
	i64 			mDealt;
	OmnConnBuffPtr	mConnBuff;
    bool            mChanged; //indicator for getBuff 
//	bool            goFlag; // judge if we have to find name for not! 2015/1/11 21:55 by rain
//    static __thread AosBsonField **tsFields; 
    static AosBsonField **tsFields; 

public:
	AosBSON();
	AosBSON(const AosBuffPtr &buff);
	~AosBSON();

	////////////////// set
	bool setValue(const u32 name, const u8 value);
	bool setValue(const u32 name, const u16 value);
	bool setValue(const u32 name, const u32 value);
	bool setValue(const u32 name, const u64 &value);
	bool setValue(const u32 name, const i8 value);
	bool setValue(const u32 name, const i16 value);
	bool setValue(const u32 name, const i32 value);
	bool setValue(const u32 name, const i64 &value);
	bool setValue(const u32 name, const OmnString &value);
	bool setValue(const u32 name, const bool value);
	bool setValue(const u32 name, const char value);
	bool setValue(const u32 name, const float value);
	bool setValue(const u32 name, const double value);
	bool setValue(const u32 name);  //value is null
	bool setValue(const u32 name, const char *data, const int len);
	bool setValue(const u32 name, const AosBuff* value);  //value is Binary
	bool setValue(const u32 name, const AosValueRslt &value);

	bool setValue(const OmnString &name, const u8 value);
	bool setValue(const OmnString &name, const u16 value);
	bool setValue(const OmnString &name, const u32 value);
	bool setValue(const OmnString &name, const u64 &value);
	bool setValue(const OmnString &name, const i8 value);
	bool setValue(const OmnString &name, const i16 value);
	bool setValue(const OmnString &name, const i32 value);
	bool setValue(const OmnString &name, const i64 &value);
	bool setValue(const OmnString &name, const OmnString &value);
	bool setValue(const OmnString &name, const bool value);
	bool setValue(const OmnString &name, const char value);
	bool setValue(const OmnString &name, const float value);
	bool setValue(const OmnString &name, const double value);
	bool setValue(const OmnString &name); //value is null 
	bool setValue(const OmnString &name, const char *data, const int len);
	bool setValue(const OmnString &name, const AosBuff* value); //value is Binary
	bool setValue(const OmnString &name, const AosValueRslt &value);
    
	//////////////////get
	bool getValue(const u32 name, AosValueRslt &value);
	bool getValue(const OmnString &name, AosValueRslt &value);

	OmnString getOmnStr(const u32 name, const OmnString &def = "");
	OmnString getOmnStr(const OmnString &name, const OmnString &def = "");

	u32 getU32(const u32 name, const u32 &def = 0);
	u32 getU32(const OmnString &name, const u32 &def = 0);

	u64 getU64(const u32 name, const u64 &def = 0);
	u64 getU64(const OmnString &name, const u64 &def = 0);

	int getInt(const u32 name, const int &def = 0);
	int getInt(const OmnString &name, const int &def = 0);

	i64 getI64(const u32 name, const i64 &def = 0);
	i64 getI64(const OmnString &name, const i64 &def = 0);

	bool getBool(const u32 name, const bool &def = false);
	bool getBool(const OmnString &name, const bool &def = false);

	AosBuffPtr getBuff(const u32 name, const AosBuffPtr &def = 0);
	AosBuffPtr getBuff(const OmnString &name, const AosBuffPtr &def = 0);


	AosBsonValue & operator [] (const OmnString &name);

  	//create map
	bool createMap(OmnString &name, AosBsonField::Type keyType, AosBsonField::Type valueType);

	bool isMember(const OmnString &name);
	bool isMember(const u32 name);

    bool init();
    bool clear();    
    bool swap(AosBSON& rhs);

	void setConnBuff(const OmnConnBuffPtr &buff);
 	void setBuff(const AosBuffPtr &buff);
    AosBuffPtr	getBuff();
    AosBuff *	getCrtBuff(){return mBuffRaw;}
    AosBuff *	getBuffRaw() { getBuff(); return mBuffRaw;}
    
    AosBsonField::Type getValueType(AosValueRslt value);
    AosBsonField* getField(const i8 id);

private:	
    u32		getCrtPos();
    bool	resetBuffLength();
	bool	build();
    
	friend class AosBsonValue;
	//flow function called by AosBsonValue
	bool	setValue(const u32 name, const u64 &value, i64 &pos);
	bool	setValue(const u32 name, const OmnString &value, i64 &pos);

	bool	setValue(const OmnString &name, const u64 &value, i64 &pos);
	bool	setValue(const OmnString &name, const OmnString &value, i64 &pos);
};


#endif

