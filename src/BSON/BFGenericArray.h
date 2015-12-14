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
// 2014/12/30 Created by Rain  
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_BSON_BFGenericArray_h
#define Aos_BSON_BFGenericArray_h

#include "BSON/BsonField.h"



class AosBFGenericArray : public AosBsonField
{
	OmnDefineRCObject;

private:
    //vector<int>         arrayPos;  //use to store idx->pos   
    //typedef hash_map<const OmnString, MapEntry, Omn_Str_hash, compare_str> map_t;
	//typedef hash_map<const OmnString, MapEntry, Omn_Str_hash, compare_str>::iterator itr_t;
	//typedef hash_map<const u32, MapEntry, u32_hash, u32_cmp> u32map_t;
	//typedef hash_map<const u32, MapEntry, u32_hash, u32_cmp>::iterator u32itr_t;

public:
	AosBFGenericArray(const int version);
	~AosBFGenericArray();

	virtual AosJimoPtr cloneJimo() const;

	virtual bool getFieldValue(
						AosRundata *rdata, 
						AosBuff *buff, 
						i64 &pos, 
						const bool copy_flag,
						AosValueRslt &value);

	/*virtual bool getFieldValue(
						AosRundata *rdata, 
						AosBuff *buff, 
						//const int idx = 0,
						i64 &pos, 
						const bool copy_flag,
						AosValueRslt &value);*/

	virtual bool appendFieldValue(
						AosRundata *rdata, 
						const AosValueRslt &value,
						AosBuff *buff);

	virtual bool appendFieldValue(
						AosRundata *rdata, 
						//const char data_type, 
						const AosBsonField::Type data_type,
						const AosValueRslt &value,
						AosBuff *buff);

	virtual bool skipValue(
						AosRundata *rdata, 
						AosBuff *buff, 
						i64 &pos);

	virtual bool setType(
						AosRundata *rdata,
						AosBuff *buff);

  /*  virtual bool appendArrayLength(                                                                              
				        AosRundata *rdata,
				        AosBsonField::Type dataType,
				        const u32  dataNumber,       
				        AosBuff *buff );*/
	
 /*	virtual bool setField(
						const OmnString &name,
						const OmnString &value,
						AosBuff *buff){return false;} */
   
    virtual bool setField(
			            const OmnString &name,
						vector<AosValueRslt> &values,
						AosBuff *buff);

	virtual bool setField(
						const u32 name,
						const OmnString &value,
						AosBuff *buff){return false;}
private:
    AosBsonField::Type getValueType(AosValueRslt value);   
};

#endif

