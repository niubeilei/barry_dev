////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Statistics.h
// Description:
//   
//
// Modification History:
// 2014/10/01 Created by Phil
// 
////////////////////////////////////////////////////////////////////////////

#ifndef StatUtil_StatRecord_h
#define StatUtil_StatRecord_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "StatUtil/Ptrs.h"
#include "StatUtil/StatTimeUnit.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "StatUtil/MeasureInfo.h"
#include "StatUtil/QueryMeasureInfo.h"
#include "StatUtil/StatTimeArea.h"
#include "Vector2DUtil/MeasureValueMapper.h"
#include "Util/Buff.h"
#include "DataRecord/DataRecord.h"

#include <vector>
using namespace std;

//aggr string is used to mark an aggregated field name or key value
#define AGGR_TOKEN "+!AGGR!+"

//define this type to avoid std-0xx and boost-thread
//compiling conflict on ">>"
typedef vector<OmnString> OmnStringList;
typedef vector<AosValueRslt> OmnValueList;
typedef hash_map<OmnString, int, Omn_Str_hash, compare_str> OmnStringHashMap;

class AosStatRecord : public AosDataRecord
{
	//OmnDefineRCObject;

private:

	//if keyValue = 0, it is time field
	//OmnStringList								*mKeyValueList;
	OmnValueList								*mKeyValueList;
	OmnStringList								mStatKeyValueList;	// Ketty 2014/12/02
	vector<MeasureInfo>							*mMeasureInfoList;
	vector<u32>									*mDistInMeasureList;
	vector<AosMeasureValueMapper>  				*mMeasureMapperList;	

	vector<bool>								mMeasureValidList;
	OmnStringHashMap 							*mFieldNameMap;  

	//order as ASC or DESC list
	vector<bool>								*mOrderIsAscList;
	vector<int>									*mMeasurePosList;
	//
	//save the calller statQryEngine. Some common
	//data is saved in the engine
	//
	bool 						mHasValidFlag;

	//stat values
	char*						mMeasureValues;

	//distinct values. distinct field name -> distinct value list
	vector<OmnStringHashMap>	mDistValueMapList;

	//if -1, no time groupby
	int							mTimeKeyPos;
	AosStatTimeUnit::E			mTimeUnit;
	int							mGrpbyKeyNum;
	//arvin
	map<OmnString,int>			mStatKeyIdxMap;
	map<int,AosDataType::E>     mKeyIdxTypeMap;
	map<int,AosDataType::E>     mGroupByKeyType;
public:
	AosStatRecord(AosMemoryCheckDeclBegin);
	AosStatRecord(OmnValueList *keyValueList,
			vector<MeasureInfo> *infoList,
			vector<u32> *distInMeasureList AosMemoryCheckDecl);

	AosStatRecord(OmnValueList *keyValueList,
				vector<MeasureInfo> *infoList AosMemoryCheckDecl);
	//AosStatRecord(const AosStatRecord &statRcd);

	AosStatRecord(const AosStatRecord &statRcd,
				AosRundata *rdata AosMemoryCheckDecl);

	~AosStatRecord();

	bool	addValue(AosRundata* rdata,
					 AosStatRecord* rcd,
					 bool accumulateOnly = false);

	bool 	addValue(AosRundata* rdata, 
					u32 vt2dIndex,
					char *value, 
					u32 vLen);
	
	bool 	addValue(AosRundata* rdata, 
				     char *value,
				   	 vector<bool> &measureValidList,
					 bool accumulateOnly = false);

	bool 	addDistValue(AosRundata* rdata,
						 OmnString &vlaue);
	
	bool 	addDistValue(
		        AosRundata* rdata,
		        char* value);

	bool 	addDistValue(AosRundata* rdata,
			             vector<OmnStringHashMap> &distValueMapList);

	bool 	serializeTo(
				const AosRundataPtr &rdata,
				AosBuff *buff);

	bool 	serializeFrom(
				const AosRundataPtr &rdata,
				AosBuff * buff,
				vector<int> &measurePosList,
				vector<int> &distMeasurePosList);

	int cmpStatRecordKeys(OmnValueList *keyValues, 
						  vector<bool> &orderIsAscList);

	int cmpStatRecord(AosStatRecord *rcd,
					  vector<int> &orderedMeasurePosList,
					  vector<bool> &orderIsAscList);

	void setHasValidFlag(bool hasValidFlag) { mHasValidFlag = hasValidFlag; }
	void setMeasureInfo(vector<MeasureInfo> *info) { mMeasureInfoList = info; }
	void setDistInMeasure(vector<u32> *distInMeasureList) { mDistInMeasureList = distInMeasureList; }
	void setOrderIsAscList(vector<bool> *orderIsAscList)	{	mOrderIsAscList = orderIsAscList;}
	void setMeasurePosList(vector<int> *measureposlist)	{	mMeasurePosList = measureposlist;	}
	void setMeasureMappers(vector<AosMeasureValueMapper> *mappers) { mMeasureMapperList = mappers; }
 	void setFieldNameMap(OmnStringHashMap *fieldNameMap) 
	{ mFieldNameMap = fieldNameMap; }
	
	void setTimeUnit(AosStatTimeUnit::E timeUnit) { mTimeUnit = timeUnit;}
	AosStatTimeUnit::E  getTimeUnit() { return mTimeUnit;}

	void setKeyValueList(OmnValueList *kvl)	{ mKeyValueList = kvl;	}
	void setTimeKeyPos(int pos) { mTimeKeyPos = pos; }
	void setGrpbyKeyNum(int num) { mGrpbyKeyNum = num; }
	void setGroupByKeyType(const map<int,AosDataType::E> &groupByKeyType){mGroupByKeyType = groupByKeyType;};
	bool reduceDistCount();

	OmnStringList &getDistInMapList();
	OmnValueList *getKeyValueList() { return mKeyValueList; }
	char *getMeasureValues() { return mMeasureValues; }
	vector<OmnStringHashMap> &getDistValueMapList() { return mDistValueMapList; }

	void getKeyValue(u32 idx, AosValueRslt &val);
	void getMeasureValue(u32 idx, AosValueRslt &val);
	vector<bool> &getMeasureValidList() { return mMeasureValidList; }
	vector<bool> &getOrderIsAsc()	{ return *mOrderIsAscList;	}
	vector<int> &getMeasurePosList()	{	return *mMeasurePosList;	}

	int cmpData(bool isAsc, AosValueRslt &data1, AosValueRslt &data2);
	
	// Ketty 2014/12/02
	void setStatKeyValues(OmnStringList &key_values){ mStatKeyValueList = key_values; }
	OmnStringList &getStatKeyValues() { return mStatKeyValueList; }
	bool getStatKeyValue(u32 idx, AosValueRslt &val);

	void setKeyFieldIdxs(const map<OmnString,int> &field_idx_map){mStatKeyIdxMap = field_idx_map;}

	void setKeyIdxTypeMap(const map<int,AosDataType::E> &key_idx_type){mKeyIdxTypeMap = key_idx_type;};
	//the following methods are needed by DataRecord interface
	//since we don't need them now, just use default function
	AosJimoPtr cloneJimo()const { return NULL; } 
	bool isFixed()const { return false; }
	int getRecordLen() { return 0; }
	int getEstimateRecordLen() { return 0; }
	AosDataRecordObjPtr clone(AosRundata* rdata AosMemoryCheckDecl) const { return OmnNew AosStatRecord(*this, rdata AosMemoryCheckerFileLine); }
	bool setData(char*, int, AosMetaData*, int64_t) { return false; }
	AosDataRecordObjPtr create( const AosXmlTagPtr&, const u64, AosRundata* AosMemoryCheckDecl) const { return NULL; }
	
	//accumulate count interface
	bool setTimeFieldValue(const i64 value);
	
	i64	getTimeFieldValue();
	
	bool resetAccumulate();
	
	virtual int   getFieldIdx( 
	     	const OmnString &name,
			AosRundata *rdata);

	virtual bool getFieldValue(
			const int idx,
			AosValueRslt &value, 
			const bool copy_flag,
			AosRundata* rdata);

	 virtual bool  getFieldValue(
			 const OmnString &field_name,
			 AosValueRslt &value,
			 const bool copy_flag,
			 AosRundata* rdata);

	 bool getKeyStr(OmnString &str) const;

private:
	bool serializeKeyToBuff(
			AosBuff 	*buff,
			u32			idx);

	bool serializeKeyFromBuff(
			AosBuff		*buff,
			u32			idx);
};


template<typename T>
class AosOrderedCmp
{  
	public:  
		static int cmpData(
				bool isAsc,
				T data1,
				T data2);
};  

#endif
