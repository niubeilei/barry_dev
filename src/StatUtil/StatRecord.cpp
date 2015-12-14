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
// 2014/01/22 Created by Ketty
////////////////////////////////////////////////////////////////////////////

#include "StatUtil/StatRecord.h"

#include "CounterUtil/CounterUtil.h"
#include "Util/Buff.h"
#include "Debug/Debug.h"
#include <time.h>

static u64 sgTotalCmpNum = 0;

AosStatRecord::AosStatRecord(AosMemoryCheckDeclBegin)
:
AosDataRecord(AosDataRecordType::eStatRecord, 
		AOSRECORDTYPE_STAT_RECORD, 
		false AosMemoryCheckerFileLine),
mKeyValueList(0),
mMeasureInfoList(0),
mMeasureMapperList(0),
mHasValidFlag(false),
mMeasureValues(0),
mTimeKeyPos(-1),
mGrpbyKeyNum(-1)
{
	//do nothing for now
}

AosStatRecord::AosStatRecord(const AosStatRecord &rhs,
		AosRundata *rdata AosMemoryCheckDecl) 
:
AosDataRecord(rhs, rdata AosMemoryCheckerFileLine),
mKeyValueList(rhs.mKeyValueList),
mStatKeyValueList(rhs.mStatKeyValueList),
mMeasureInfoList(rhs.mMeasureInfoList),
mDistInMeasureList(rhs.mDistInMeasureList),
mMeasureMapperList(rhs.mMeasureMapperList),
mMeasureValidList(rhs.mMeasureValidList),
mFieldNameMap(rhs.mFieldNameMap),
mHasValidFlag(rhs.mHasValidFlag),
mMeasureValues(rhs.mMeasureValues),
mDistValueMapList(rhs.mDistValueMapList),
mTimeKeyPos(rhs.mTimeKeyPos),
mTimeUnit(rhs.mTimeUnit),
mGrpbyKeyNum(rhs.mGrpbyKeyNum),
mStatKeyIdxMap(rhs.mStatKeyIdxMap),
mKeyIdxTypeMap(rhs.mKeyIdxTypeMap)
{
}


AosStatRecord::AosStatRecord(OmnValueList *keyValueList,
			vector<MeasureInfo> *infoList,
			vector<u32> *distInMeasureList AosMemoryCheckDecl)
:
AosDataRecord(AosDataRecordType::eStatRecord, 
		AOSRECORDTYPE_STAT_RECORD, 
		false AosMemoryCheckerFileLine),
mKeyValueList(keyValueList),
mMeasureInfoList(infoList),
mDistInMeasureList(distInMeasureList),
mMeasureMapperList(0),
mHasValidFlag(false),
mMeasureValues(0),
mTimeKeyPos(-1),
mGrpbyKeyNum(-1)
{
	int dataLen = 8;
	OmnStringHashMap valueMap;

	//init the measure value list 
	u32	len = mMeasureInfoList->size() * dataLen;
	mMeasureValues = (char *)calloc(len+1, sizeof(char));

	for (u32 i = 0; i < mMeasureInfoList->size(); i++)
		mMeasureValidList.push_back(false);

	//init distinct count value map
	mDistValueMapList.clear();
	valueMap.clear();
	for (u32 i = 0; i < mDistInMeasureList->size(); i++)
		mDistValueMapList.push_back(valueMap);

}


AosStatRecord::AosStatRecord(OmnValueList *keyValueList,
					vector<MeasureInfo> *infoList AosMemoryCheckDecl)
:
AosDataRecord(AosDataRecordType::eStatRecord, 
		AOSRECORDTYPE_STAT_RECORD, 
		false AosMemoryCheckerFileLine),
mKeyValueList(keyValueList),
mMeasureInfoList(infoList),
mDistInMeasureList(0),
mMeasureMapperList(0),
mHasValidFlag(false),
mMeasureValues(0),
mTimeKeyPos(-1),
mGrpbyKeyNum(-1)
{
	int dataLen = 8;
	OmnStringHashMap valueMap;

	//init the measure value list 
	u32	len = mMeasureInfoList->size() * dataLen;
	mMeasureValues = (char *)calloc(len+1, sizeof(char));

}


AosStatRecord::~AosStatRecord()
{
	if (mMeasureValues)
		free(mMeasureValues);

	if (mKeyValueList)
		delete mKeyValueList;
}

//
// Add both normal values and dist count values
// from another stat record
//
//
// When merging value from another record, which measure to merge is based on
// the local record. If the record to be merged has other measures not in the
// local measure list, they will be ignored
//
bool
AosStatRecord::addValue(
		AosRundata* rdata,
		AosStatRecord *rcd,
		bool accumulateFlag)
{
	bool rslt, rslt1;

	aos_assert_r(rcd, false);

	rslt = addValue(rdata, rcd->getMeasureValues(), rcd->getMeasureValidList(), 
			accumulateFlag);
	rslt1 = addDistValue(rdata, rcd->getDistValueMapList());

	aos_assert_r(rslt && rslt1, false);
	return rslt && rslt1;
}

//
//Add normal measure values (excluding dist count)
//this value is mostly added from vt2d record
//
//There might be a hasValidFlag if data is from
//vt2d Record
//
//This method is mostly called in cube.exe
//
bool
AosStatRecord::addValue(AosRundata* rdata,
						u32 vt2dIndex,
						char *value,
						u32 vLen)
{
	AosMeasureValueMapper *mapper;
	u32 iPos, oPos;
    char* dataPos;

	AosExprObjPtr cond;
	char *statMeasureValue = 0;
	AosDataType::E dataType;
	AosAggrFuncObj *aggr;
	int defaultLen = 8;
	int dataLen = defaultLen;  //all the stat value use 8 bytes for now

	aos_assert_r(vt2dIndex >= 0 && 
			vt2dIndex < mMeasureMapperList->size(), false);

	if (mHasValidFlag)
		dataLen++;

	// select count(abc=3), count(abc=4)
	mapper = &((*mMeasureMapperList)[vt2dIndex]);
	for (u32 i = 0; i < mapper->getMeasureValueNum(); i++)
	{
		iPos = mapper->getInputIndex(i);
		oPos = mapper->getOutputIndex(i);
		aggr = mapper->getAggrFunc(i);
		aos_assert_r(aggr, false);

		//get stat value and len
		statMeasureValue = &mMeasureValues[oPos * defaultLen];
		dataType = (*mMeasureInfoList)[oPos].mDataType;

		aos_assert_r(vLen >= (iPos + 1) * dataLen, false);
		dataPos = value + iPos * dataLen;
		if (mHasValidFlag)
		{
			if (dataPos[0] == 0)
				continue;

			dataPos++;
		}
		AosValueRslt vv;
		char tmpValue[8];
		memcpy(tmpValue,dataPos,defaultLen);
		cond =(*mMeasureInfoList)[i].mCond;	
		if(cond)
		{
			cond->getValue(rdata,this,vv);
			if (!vv.getBool())
			{
				memset(tmpValue,0,defaultLen);	
			}
		}
		if (mMeasureValidList[oPos])
		{
			aggr->updateOutputData(rdata, tmpValue,
					dataType, statMeasureValue, dataType);
		}
		else
		{
			memcpy(statMeasureValue, tmpValue, sizeof(i64)); 
			mMeasureValidList[oPos] = true;
		}
	}
	return true;
}

//
//add normal measure value (excluding dist count) from another stat record
//
bool
AosStatRecord::addValue(AosRundata* rdata,
						   char *value,
						   vector<bool> &measureValidList,
						   bool accumulateFlag)
{
	AosDataType::E dataType;
	AosAggrFuncObj *aggr;
	int dataLen = 8;  //all the stat value use 8 bytes for now

	aos_assert_r(value, false);
	aos_assert_r(measureValidList.size() == mMeasureValidList.size(), false);
	for (u32 i = 0; i < mMeasureInfoList->size(); i++)
	{
		if (!measureValidList[i])
		{
			//the new record doesn't have this measure value
			continue;
		}

		if (!mMeasureValidList[i])
		{
			//just copy the new record's measure value
			memcpy(mMeasureValues + i * dataLen, value + i * dataLen, sizeof(i64));
			mMeasureValidList[i] = true;
			continue;
		}

		if (accumulateFlag && !(*mMeasureInfoList)[i].mIsAccumulate)
		{
			//not an accumulate measure, skip
			continue;
		}

		dataType = (*mMeasureInfoList)[i].mDataType;
		aggr = (*mMeasureInfoList)[i].mAggrFuncRaw;
		aos_assert_r(aggr, false);

		aggr->updateOutputData(rdata, value + i * dataLen, 
				dataType, mMeasureValues + i * dataLen, dataType);

		//aos_assert_r(mMeasureValues[2] == 0, false);
	}

	return true;
}

//
//add dist value from a vector2d record
//
bool
AosStatRecord::addDistValue(AosRundata* rdata,
					 OmnString &value)
{
	OmnStringHashMap *valueMap;
	OmnStringHashMap::iterator valItr;

	//if value from vt2drecord, there should be only 
	//one dist field
	aos_assert_r(mDistValueMapList.size() == 1, false);

	valueMap = &mDistValueMapList[0];
	//add a new variation if not existing
	(*valueMap)[value] = 1;

	return true;
}


//
//merge dist value from another stat record
//
bool
AosStatRecord::addDistValue(AosRundata* rdata,
							vector<OmnStringHashMap> &distValueMapList)
{
	OmnStringHashMap *valueMap, *localValueMap;
	OmnStringHashMap::iterator valItr, localValItr;
	OmnString str;

	aos_assert_r(distValueMapList.size() == mDistValueMapList.size(), false);
	for (u32 i = 0; i < distValueMapList.size(); i++)
	{
		localValueMap = &mDistValueMapList[i];
		valueMap = &distValueMapList[i];

		//merge valueList and localValueList into localValueList
		valItr = valueMap->begin();
		while (valItr != valueMap->end())
		{
			str = valItr->first;
			(*localValueMap)[str] = 1;
			valItr++;
		}
	}

	return true;
}

bool
AosStatRecord::addDistValue(
		        AosRundata* rdata,
		        char* value)
{
	OmnStringHashMap *valueMap;
	OmnStringHashMap::iterator valItr;

	//	if value from vt2drecord, there should be only
	//	one dist field
		char* ptr = value;
	for(int i=0;i<mDistValueMapList.size();i++)
	{
		valueMap = &mDistValueMapList[i];
		//add a new variation if not existing
		ptr += 1;
		u64* valptr = (u64*)(*(u64*)ptr);
		u64 valcnt = *valptr;
		u64* vals = valptr+1;
		for(u64 j=0;j<valcnt;j++)
		{
			OmnString val;
			val << vals[j];
			(*valueMap)[val] = 1;
		}
		ptr += sizeof(u64);
	}
	char *buff = (char*)(*(u64*)(value+1));
	delete buff;
	buff = NULL;
	return true;
}
			   


bool
AosStatRecord::serializeTo(
		const AosRundataPtr &rdata,
		AosBuff *buff)
{
	int dataLen = 8;
	u32 num, valNum;
	bool rslt;
	OmnStringHashMap *valueMap;
	OmnStringHashMap::iterator itr;

	//set key values
	num = mKeyValueList->size();
	buff->setU32(num);
	for (u32 i = 0; i < num; i++)
	{
		//buff->setOmnStr((*mKeyValueList)[i]);
		rslt= serializeKeyToBuff(buff,i);
		aos_assert_r(rslt,false);
	}

	// Ketty 2014/12/02
	// set stat key values.
	num = mStatKeyValueList.size();
	buff->setU32(num);
	for (u32 i = 0; i < num; i++)
	{
		buff->setOmnStr(mStatKeyValueList[i]);
	}

	//set measure value
	//buff->setCharStr(mMeasureValues, len, false);
	num = mMeasureInfoList->size() * dataLen;
	buff->setU32(num);
	buff->setBuff(mMeasureValues, num);

	//set measure valid info
	num = mMeasureValidList.size();
	buff->setU32(num);
	for (u32 i = 0; i < num; i++)
	{
		if (mMeasureValidList[i])
			buff->setBool(true);
		else
			buff->setBool(false);
	}

	//serialize the dist count values
	//save number of dist_count measures firstly
	num = mDistValueMapList.size();
	buff->setU32(num);
	for (u32 i = 0; i < num; i++)
	{
		//save dist count values one by one
		//number of distinct values saved firstly
		valueMap = &mDistValueMapList[i];
		valNum = valueMap->size();
		buff->setU32(valNum);
		itr = valueMap->begin();
		while (itr != valueMap->end())
		{
			buff->setOmnStr(itr->first);
			itr++;
		}
	}

	return true;
}

bool
AosStatRecord::serializeFrom(
		const AosRundataPtr &rdata,
		AosBuff *buff,
		vector<int> &measurePosList,
		vector<int> &distMeasurePosList)
{
	u32 num;
	u32 valNum;
	OmnStringHashMap *valueMap;
	OmnStringHashMap::iterator itr;
	OmnString field;
	bool rslt;
	vector<bool> measureValidList;
	//static char measureValues[1024 * 8];  //give enough space

	//since there could be multiple stat engine
	//running at the same time, not use static
	//static char measureValues[1024 * 8];  //give enough space
	char measureValues[1024 * 8];  //give enough space
	int pos;
	u32 dataLen = 8;
	
	mKeyValueList = new OmnValueList();
	//get key values
	num = buff->getU32(0);
	for (u32 i = 0; i < num; i++)
	{
		//mKeyValueList->push_back(buff->getOmnStr(""));
		rslt = serializeKeyFromBuff(buff,i);
		aos_assert_r(rslt, false);
	}
	
	num = buff->getU32(0);
	for (u32 i = 0; i < num; i++)
	{
		mStatKeyValueList.push_back(buff->getOmnStr(""));
	}

	num = buff->getU32(0);
	buff->getBuff(measureValues, num);

	//get valid flag list
	num = buff->getU32(0);
	for (u32 i = 0; i < num; i++)
	{
		measureValidList.push_back(buff->getBool(true));
	}

	//serialize the value to the new record
	aos_assert_r(num == measurePosList.size(), false);
	for (u32 i = 0; i < num; i++)
	{
		if (!measureValidList[i]) continue;

		//get the position in mMeasureValues
		pos = measurePosList[i];
		if (pos < 0) continue;

		memcpy(mMeasureValues + pos * dataLen, measureValues + i * dataLen, dataLen);
		mMeasureValidList[pos] = true;
	}

	//get the dist count values if any
	//remove old values since this method assume new values
	//coming from network only
	num = buff->getU32(0);
	aos_assert_r(num == distMeasurePosList.size(), false);
	OmnString str;
	for (u32 i = 0; i < num; i++)
	{
		pos = distMeasurePosList[i];
		aos_assert_r(pos >= 0, false);

		valueMap = &mDistValueMapList[pos];
		valNum = buff->getU32(0);
		for (u32 j = 0; j < valNum; j++)
		{
			str = buff->getOmnStr("");
			(*valueMap)[str] = 1;
		}
	}

	return true;
}

//
//1: parameter > current statRcd, current is before param
//0: parameter = current statRcd

//-1: parameter < current statRcd, current is after param
//
int
AosStatRecord::cmpStatRecordKeys(OmnValueList *keyValueList,
									vector<bool> &orderIsAscList)
{
	AosValueRslt kv;
	AosValueRslt localKv;
	i64	kvNum;
	i64 localKvNum;
	bool isAsc;
	int rslt;
	OmnString kv1;
	OmnString kv2;

	sgTotalCmpNum++;
	aos_assert_r(keyValueList->size() == mKeyValueList->size(), false);
	for (u32 i = 0; i < keyValueList->size(); i++)
	{
		kv = (*keyValueList)[i];
		localKv = (*mKeyValueList)[i];
		isAsc = orderIsAscList[i];

		if (mTimeKeyPos ==(int) i)
		{
			//compare timeValue using integer
			kv1 = kv.getStr();
			kvNum = kv1.toInt64(0);
			kv2 = localKv.getStr();
			localKvNum = kv2.toInt64(0);
			rslt = AosOrderedCmp<i64>::cmpData(isAsc, localKvNum, kvNum); 
			//rslt = cmpData(isAsc, localKv, kv);
		}
		else
		{
			rslt = cmpData(isAsc, localKv, kv); 
		}

		if (rslt != 0) return rslt;
	}

	return 0;
}

void 
AosStatRecord::getKeyValue(u32 idx, AosValueRslt &val)
{
	OmnString str;
	AosValueRslt vv;

	if (mKeyValueList->size() > idx)
	{
		//normal key value
		//str = (*mKeyValueList)[idx];
		val = (*mKeyValueList)[idx];
		if (val.getStr() == AGGR_TOKEN)
			//str = "";
			val.setStr("");
	}
	else
	{
		//most likely, this record is a rollup record which
		//doesn't have the asked field
		if (idx == 0)
			str = "Total";
		else
			str = "";
		val.setStr(str);
	}
	return ;
/*	//arvin 2015-10-8
	AosDataType::E type = mKeyIdxTypeMap[idx];
	switch(type)
	{
		case AosDataType::eInt64:	
		case AosDataType::eDateTime:
			{
				i64 vv = str.toInt64();
				val.setI64(vv);
				break;
			}
		case AosDataType::eU64:			
			{
				u64 vv = str.toU64();
				val.setU64(vv);
				break;
			}
		case AosDataType::eDouble:			
			{
				double vv = str.toDouble();
				val.setDouble(vv);
				break;
			}
		default:
			val.setStr(str);
			break;
	}
*/
}

bool 
AosStatRecord::reduceDistCount()
{
	u32 pos;
	u32 dataLen = 8;

	for (u32 i = 0; i < mDistValueMapList.size(); i++)
	{
		pos = (*mDistInMeasureList)[i];
		*((u64*)(&mMeasureValues[pos * dataLen])) = mDistValueMapList[i].size();
	}

	return true;
}


double char_x_to_double(char* input_char)
{
    int flag = 0;
    double sum = 0.0;
    double divisor = 10.0;
    char *p = input_char;
    if (*input_char)
    {
    //所有输入的数值都必须在(0~9,+,-,.)之中
        while(*p)
        {
        if (!((*p=='+') || (*p=='-')||(*p=='.')|| ((*p>='0') && (*p<='9'))))
        {
                cout<<"输入非法！请输入数值型参数！例如：12.3,125,-12.6等"<<endl;
                return 0.0;
            }
        p++;
        }

        //int j=0;
        if (*input_char == '+')
            {
                flag = 1;
               input_char++;
               //j++;
               //cout<<"(flag + )j="<<j<<endl;
            }
        if (*input_char == '-')
            {
                flag = -1;
                input_char++;
                   //j++;
               //cout<<"(flag - )j="<<j<<endl;
            }

        //计算小数点前面的和
        while(*input_char != '.' && *input_char)
            {
                sum = sum*10 + (double(*input_char) - 48); //0的ascii码是48

              // cout<<"j"<<*input_char<<endl;
               input_char++;
                   //j++;
            }

        if (*input_char=='.')
            {
                input_char++;
                //计算小数点后面的和
                while(*input_char)
                {

                sum = sum + ( double(*input_char)-48)/divisor;
                divisor *= 10;
                input_char++;
                //j++;
                //cout<<"j"<<j<<endl;
            }
        }
        if (flag == 1)
            return sum;
        if (flag == -1)
            return -sum;
        return sum;

    }
    else
    {
    cout<<"输入为空!"<<endl;;
    return 0.0;
    }

}


void 
AosStatRecord::getMeasureValue(u32 idx, AosValueRslt &val)
{
	aos_assert(idx < mMeasureInfoList->size());
	MeasureInfo &measure = (*mMeasureInfoList)[idx];
	AosDataType::E dataType;
	char *statValue;
	AosNumber n;
	OmnString value;//yang

	statValue = mMeasureValues + 8 * idx;
	dataType = measure.mDataType;
	switch (dataType)
	{
		case AosDataType::eInt64:
		case AosDataType::eDateTime:
			val.setI64(*(i64*)statValue);
			break;

		case AosDataType::eU64:
			val.setU64(*(u64*)statValue);
			break;
		
		case AosDataType::eDouble:
			val.setDouble(*(double *)statValue);
			break;

		case AosDataType::eNumber:
			n = measure.mNumber;
			//yang,fix bug of data type conversion problem
			//value.assign(statValue,8);
			//value = statValue;
			//vallen = value.length();
			//rsltval = strtod(statValue, &endptr);
			//rsltval = char_x_to_double(statValue);

			//rsltval = *(u64*)statValue;

			//n.setValue(rsltval);

			n.setValue(*(double*)statValue);
			//val.setNumber(n);
			val.setDouble(n.getValue());
			break;

		default:
			OmnNotImplementedYet;
			return;
	}

}

//
//Compare 2 statrecords based on orderby list
//1: parameter > current statRcd
//0: parameter = current statRcd
//-1: parameter < current statRcd
//
int
AosStatRecord::cmpStatRecord(AosStatRecord *rcd,
								vector<int> &orderedMeasurePosList,
								vector<bool> &orderIsAscList)
{
	//OmnString kv;
	//OmnString localKv;
	AosValueRslt kv;
	AosValueRslt localKv;
	i64	kvNum;
	i64 localKvNum;
	OmnString kv1;
	OmnString kv2;

	bool isAsc;
	u32 keyIndex = 0; //index of keys (excluding measure values) for next comparision
	OmnValueList *keyValueList;
	AosValueRslt val;
	AosValueRslt localVal;
	int pos;
	int rslt;

	aos_assert_r(rcd, 0);
	keyValueList = rcd->getKeyValueList();
	aos_assert_r(keyValueList->size() == mKeyValueList->size(), false);

	//compare orderby fields firstly
	for (u32 i = 0; i < orderedMeasurePosList.size(); i++)
	{
		pos = orderedMeasurePosList[i];	
		isAsc = orderIsAscList[i];
		if (pos < 0)
		{
			//the order by field is key
			kv = (*keyValueList)[keyIndex];
			localKv = (*mKeyValueList)[keyIndex];

			if (mTimeKeyPos ==(int) keyIndex)
			{
				//compare timeValue using integer
				kv1 = kv.getStr();
				kvNum = kv1.toInt64(0);
				kv2 = localKv.getStr();
				localKvNum = kv2.toInt64(0);
				rslt = AosOrderedCmp<i64>::cmpData(isAsc, localKvNum, kvNum); 
				//rslt = cmpData(isAsc, localKv, kv);
			}
			else
			{
				rslt = cmpData(isAsc, localKv, kv); 
			}

			keyIndex++;
		}
		else
		{
			//the orderby field is a measure value
			//get the field value and datatype firstly
			getMeasureValue(pos, localVal);
			rcd->getMeasureValue(pos, val);
			switch ((*mMeasureInfoList)[pos].mDataType)
			{
				case AosDataType::eInt64:
					rslt = AosOrderedCmp<i64>::cmpData(isAsc, 
												localVal.getI64(),
												val.getI64());
					break;

				case AosDataType::eU64:
					rslt = AosOrderedCmp<u64>::cmpData(isAsc, 
												localVal.getU64(),
												val.getU64());
					break;

				case AosDataType::eDouble:
				case AosDataType::eNumber:
					// Pay 		JIMODB-1254	
					rslt = AosOrderedCmp<double>::cmpData(isAsc, 
												localVal.getDouble(),
												val.getDouble());
					break;

				default:
					OmnNotImplementedYet;
			}
		}

		if (rslt != 0) return rslt;
	}

	//compare the rest key fields if any
	for (u32 i = keyIndex; i <  mKeyValueList->size(); i++)
	{
		kv = (*keyValueList)[i];
		localKv = (*mKeyValueList)[i];
		isAsc = true;

		if (mTimeKeyPos == (int)i)
		{
			//compare timeValue using integer
			kv1 = kv.getStr();
			kvNum = kv1.toInt64(0);
			kv2 = localKv.getStr();
			localKvNum = kv2.toInt64(0);
			rslt = AosOrderedCmp<i64>::cmpData(isAsc, localKvNum, kvNum); 
		}
		else
		{
			rslt = cmpData(isAsc, localKv, kv); 
		}

		if (rslt != 0) return rslt;
	}

	return 0;
}

//
//Compare 2 OmnString
//Comparisons over basic typename use template
//1: data1 should be before data2
//0: data1 = data2
//-1: data1 should be after data2
//
int 
AosStatRecord::cmpData(bool isAsc,
						  AosValueRslt &data1,
						  AosValueRslt &data2)
{
	AosDataType::E dataType1 = data1.getType();
	//AosDataType::E dataType2 = data2.getType();
	switch(dataType1)
	{
	case AosDataType::eU64:
		{
			u64 u1 = data1.getU64();
			u64 u2 = data2.getU64();
			return AosOrderedCmp<u64>::cmpData(isAsc, u1, u2); 
		}
	//	break;
	case AosDataType::eInt64:
		{
			i64 u1 = data1.getI64();
			i64 u2 = data2.getI64();
			return AosOrderedCmp<i64>::cmpData(isAsc, u1, u2);
		}
		//	break;
	case AosDataType::eDouble:
	case AosDataType::eNumber:
		{
			double u1 = data1.getDouble();
			double u2 = data2.getDouble();
			return AosOrderedCmp<double>::cmpData(isAsc, u1, u2);
		}
	//	break;
	case AosDataType::eString:
		{
			OmnString str1 = data1.getStr();
			OmnString str2 = data2.getStr();
			int rslt = str1.compare1(str2);

			if (rslt >= 0)
			{
				if (rslt == 0)
					return 0;

				if (isAsc)
					return -1;
				else
					return 1;
			}

			if (isAsc)
				return 1;
			else
				return -1;
		}
		//break;
	default:
		OmnNotImplementedYet;
	}
	return 0;
	/*if ( (isAsc && data1 > data2) ||
			(!isAsc && data1 < data2) )
		return -1;

	if ( (isAsc && data1 < data2) ||
			(!isAsc && data1 > data2) )
		return 1;

	return 0;*/
}

//
//Compare 2 statrecords based on orderby list
//1: data1 should be before data2
//0: data1 = data2
//-1: data1 should be after data2
//

template<typename T>
int AosOrderedCmp<T>::cmpData(
				bool isAsc,
				T data1,
				T data2)
{
	if ( (isAsc && data1 > data2) ||
			(!isAsc && data1 < data2) )
		return -1;

	if ( (isAsc && data1 < data2) ||
			(!isAsc && data1 > data2) )
		return 1;

	return 0;
}


// Ketty 2014/12/02
bool
AosStatRecord::getStatKeyValue(u32 idx, AosValueRslt &val)
{
	aos_assert_r(idx < mStatKeyValueList.size(), false);
	if(mKeyIdxTypeMap.size())
	{
		AosDataType::E type = mKeyIdxTypeMap[idx];
		OmnString value = mStatKeyValueList[idx];
		//arvin 2015.10.16
		//JIMODB-970
		if(type==AosDataType::eInvalid)
		{
			idx += mGrpbyKeyNum + mMeasureInfoList->size();
			type = mKeyIdxTypeMap[idx];
		}
		switch(type)
		{
			case AosDataType::eInt64:	
				{
					i64 vv = value.toInt64();
					val.setI64(vv);
					break;
				}
			case AosDataType::eU64:			
				{
					u64 vv = value.toU64();
					val.setU64(vv);
					break;
				}
			case AosDataType::eDouble:			
				{
					double vv = value.toDouble();
					val.setDouble(vv);
					break;
				}
			case AosDataType::eDateTime:
				{
					i64 vv = value.toInt64();
					AosDateTime dt(vv,"");
					if (dt.isNotADateTime()) 
					{   
						OmnAlarm << "Current DateTime Object is invalid" << enderr;
						return false;
					}   
					val.setDateTime(dt);
					break;
				}
			default:
				val.setStr(value);
				break;
		}
	}
	else
	{
		val.setStr(mStatKeyValueList[idx]);
	}
	return true;
}

	
int
AosStatRecord::getFieldIdx(
		const OmnString &name,
		AosRundata *rdata)
{
	map<OmnString, int>::iterator itr = mFieldIdxs.find(name);
	if(itr != mFieldIdxs.end())
	{
		return itr->second;
	}
	else
	{
		itr = mStatKeyIdxMap.find(name);
		if(itr != mStatKeyIdxMap.end())
		{
			return itr->second;
		}
	}

	return -1;
}

	

bool
AosStatRecord::getFieldValue(
		const int idx,
		AosValueRslt &value_rslt, 
		const bool copy_flag,
		AosRundata* rdata)
{
	if (idx < mGrpbyKeyNum)
	{
		//get value from key value list
		getKeyValue(idx, value_rslt);
		if(idx == (u32)mTimeKeyPos)
		{
			OmnString time_value_str = value_rslt.getStr();
			i64 time_value = time_value_str.toInt64(-1);
			aos_assert_r(time_value != -1, false);
			//arvin 2015.10.22
			//JIMODB-989
			i64 epochUnixTime = AosStatTimeUnit::convertToUnixTime(time_value,mTimeUnit);
			value_rslt.setI64(epochUnixTime);
/*
			OmnString calendar_time = AosStatTimeUnit::convertToCalendarTime(
					time_value, mTimeUnit);
			value_rslt.setStr(calendar_time);
*/
		}
		return true;
	}

	//get measure fields
	u32 measure_num = mMeasureInfoList->size();
	if (idx < mGrpbyKeyNum + (int)measure_num)
	{
		getMeasureValue(idx - mGrpbyKeyNum, value_rslt);
		return true;
	}

	// get value from stat key value list.
	getStatKeyValue(idx - mGrpbyKeyNum - measure_num, value_rslt);
	return true;
}

//Gavin 2015/08/11 JIMODB-341
bool  
AosStatRecord::getFieldValue(  
		const OmnString &field_name,
		AosValueRslt &value,
		const bool copy_flag,       
		AosRundata* rdata)
{
	OmnString measure_name;
	for(size_t i = 0; i < mMeasureInfoList->size(); i++)
	{
		//arvin 2015.11.05
		////Jimodb-1120
		if((*mMeasureInfoList)[i].mCond)
			measure_name = (*mMeasureInfoList)[i].mCondMeasureName;
		else
			measure_name = (*mMeasureInfoList)[i].mName;
		
		if(measure_name == field_name)
		{
			switch((*mMeasureInfoList)[i].mDataType)
			{
				case AosDataType::eU64:
					{
						u64 vv = *(u64*)&mMeasureValues[i*8];
						value.setU64(vv);
						return true;
					}
				case AosDataType::eInt64:
					{
						i64 vv = *(i64*)&mMeasureValues[i*8];
						value.setI64(vv);
						return true;
					}
				case AosDataType::eDouble:
					{
						double vv = *(double*)&mMeasureValues[i*8];
						value.setDouble(vv);
						return true;
					}
				default:
						OmnShouldNeverComeHere;
			}
		}
	}
	OmnShouldNeverComeHere;
	return false;
}


bool 
AosStatRecord::setTimeFieldValue(const i64 value)
{
	if(mTimeKeyPos < (int)mKeyValueList->size())
	{
		OmnString val;
		val << value;
		(*mKeyValueList)[mTimeKeyPos] = AosValueRslt(val);
		return true;
	}

	//need to set statKeyField as well
	//mStatKeyValueList[mTimeKeyPos] = val;

	return false;
}

i64
AosStatRecord::getTimeFieldValue()
{
	i64 time_value =-1;
	if(mTimeKeyPos < (int)mKeyValueList->size())
	{
		AosValueRslt vv = (*mKeyValueList)[mTimeKeyPos];
		OmnString value = vv.getStr();
		time_value = value.toInt64(-1);
	}
	return time_value;;
}

bool
AosStatRecord::resetAccumulate()
{
	bool isAccumulate;	
	u32 dataLen = sizeof(u64);
	for(size_t i = 0; i < mMeasureInfoList->size();i++)
	{
		isAccumulate = (*mMeasureInfoList)[i].mIsAccumulate;
		if(isAccumulate)
		{
			char *data = mMeasureValues+i*dataLen;
			memset(data,0,dataLen);
		}
	}
	return true;
}


bool 
AosStatRecord::serializeKeyToBuff(
		AosBuff 	*buff,
		u32			idx)
{
	if(idx ==(u32) mTimeKeyPos)
	{
		buff->setI64((*mKeyValueList)[idx].getI64());
		return true;
	}

	AosDataType::E type = mGroupByKeyType[idx];
	switch(type)
	{
	case AosDataType::eU64:
		buff->setU64((*mKeyValueList)[idx].getU64());
		return true;

	case AosDataType::eInt64:
		buff->setI64((*mKeyValueList)[idx].getI64());
		return true;

	case AosDataType::eDouble:
		buff->setDouble((*mKeyValueList)[idx].getDouble());
		return true;

	case AosDataType::eString:
		buff->setOmnStr((*mKeyValueList)[idx].getStr());
		return true;

	case AosDataType::eDateTime:
		buff->setI64((*mKeyValueList)[idx].getI64());
		return true;

	default:
		OmnAlarm << "Invalid datatype : " << type << enderr;
		break;
	}
	return false;
}

bool
AosStatRecord::serializeKeyFromBuff(
		AosBuff		*buff,
		u32			idx)
{
	AosValueRslt vv;
	AosDataType::E type = mGroupByKeyType[idx];
	if(idx ==(u32) mTimeKeyPos)
	{
		vv.setI64(buff->getI64(0));
		mKeyValueList->push_back(vv);
		return true;
	}

	switch(type)
	{
	case AosDataType::eU64:
		vv.setU64(buff->getU64(0));
		mKeyValueList->push_back(vv);
		return true;
	
	case AosDataType::eDouble:
		vv.setDouble(buff->getDouble(0));
		mKeyValueList->push_back(vv);
		return true;
	
	case AosDataType::eInt64:
		vv.setI64(buff->getI64(0));
		mKeyValueList->push_back(vv);
		return true;
	
	case AosDataType::eDateTime:
		vv.setI64(buff->getI64(0));
		mKeyValueList->push_back(vv);
		return true;
	
	case AosDataType::eString:
		vv.setStr(buff->getOmnStr(""));
		mKeyValueList->push_back(vv);
		return true;

	default:
		OmnAlarm << "Invalid datatype : " << type << enderr;
		break;
	}
	return false;

}

bool
AosStatRecord::getKeyStr(OmnString &str) const
{
	str = "";
	for (u32 i = 0; i < mKeyValueList->size(); i++)
	{
		str << ((*mKeyValueList)[i]).getStr();
	}

	return true;
}
