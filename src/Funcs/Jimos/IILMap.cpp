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
// Modification History:
// 2014/04/08 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Funcs/Jimos/IILMap.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"



extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosIILMap_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosIILMap(version);
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


AosIILMap::AosIILMap(const int version)
:
AosGenFunc("IILMap", version)
{
}

AosIILMap::AosIILMap()
:
AosGenFunc("IILMap", 1)
{
}



AosIILMap::AosIILMap(const AosIILMap &rhs)
:
AosGenFunc(rhs)
{
	mIILMap = rhs.mIILMap;
	mDataType = rhs.mDataType;
}


AosIILMap::~AosIILMap()
{
}


bool
AosIILMap::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosIILMap::getDataType(
		AosRundata *rdata,
		AosDataRecordObj *record)
{
	bool rslt = AosGenFunc::getValue(rdata, 0, record);
	aos_assert_r(rslt, AosDataType::eInvalid);

	OmnString mapName = mValue.getStr();
	//get map doc
	AosXmlTagPtr doc = AosJqlStatement::getDoc(rdata, JQLTypes::eMap, mapName);
	aos_assert_r(doc, AosDataType::eInvalid);

	OmnString type = doc->getAttrStr("datatype", "string");
	mDataType = AosDataType::toEnum(type);
	aos_assert_r(mDataType != AosDataType::eInvalid, AosDataType::eInvalid);

	return mDataType;
}


bool
AosIILMap::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	if (!mIILMap)
	{
		bool rslt = AosGenFunc::getValue(rdata, 0, record);
		aos_assert_r(rslt, false);
		OmnString mapName = mValue.getStr();
		
		//get map doc
		AosXmlTagPtr doc = AosJqlStatement::getDoc(rdata, JQLTypes::eMap, mapName);
		aos_assert_r(doc, false);

		OmnString type = doc->getAttrStr("datatype", "string");
		mDataType = AosDataType::toEnum(type);
		aos_assert_r(mDataType != AosDataType::eInvalid, AosDataType::eInvalid);
		mDataFieldFormat = doc->getAttrStr("zky_dataField_format","");
		//create IILEntryMap
		OmnString iilName = doc->getAttrStr(AOSTAG_IILNAME);
		if (iilName == "")
		{
			AosSetErrorU(rdata, "missing iil name:") << doc->toString();
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		bool need_split = doc->getAttrBool(AOSTAG_NEEDSPLIT, false);
		OmnString sep = doc->getAttrStr(AOSTAG_SEPARATOR);
		bool need_swap = doc->getAttrBool(AOSTAG_NEEDSWAP, false);
		bool use_key_as_value = doc->getAttrBool(AOSTAG_USE_KEY_AS_VALUE, false);

		
		mIILMap = AosIILEntryMapMgr::retrieveIILEntryMap(   
					iilName, need_split, sep, need_swap, use_key_as_value, rdata);
		aos_assert_r(mIILMap, false);

		mIILMap->readlock();
		OmnScreen << "iil map read lock:" << mIILMap->mIILName << ":" << (u64)mIILMap.getPtr() << endl;
		u32 size = mIILMap->size();
		OmnScreen << "iil map read unlock:" << mIILMap->mIILName << ":" << (u64)mIILMap.getPtr() << endl;
		mIILMap->unlock();

		if (size == 0) 
		{
			OmnScreen << "map size is 0, iilname:" << iilName << endl;
		}

		OmnScreen << "iil map size:" << mIILMap->mIILName << ":" << size << endl; 
	}

	OmnString key = "";
	bool rslt = false;
	for (size_t i = 1; i < mSize; i++)
	{
		rslt = AosGenFunc::getValue(rdata, i, record);
		aos_assert_r(rslt, false);

		if (mValue.isNull() || mValue.getStr() == "")
		{
			value.setNull();
			return true;
		}

		if (i > 1) key << char(0x01);                                  
		key << mValue.getStr();   //key1\0x01key2\0x01key3
	}

	mIILMap->readlock();
	map<OmnString, OmnString>::iterator itr = mIILMap->find(key);
	if (itr != mIILMap->end())
	{
		OmnString vv = itr->second;
		mIILMap->unlock();

		u64 v_u64 = vv.toU64();
		switch(mDataType)
		{
			case AosDataType::eString:
				value.setStr(vv);
				break;
			case AosDataType::eNumber:
				value.setDouble(atof(vv.data()));
				break;
			case AosDataType::eU64:
				value.setU64(vv.toU64());
				break;
			case AosDataType::eInt64:
				value.setI64(vv.toInt64());
				break;
			case AosDataType::eDouble:
				value.setDouble(*(double*)(&v_u64));
				break;
			case AosDataType::eDateTime:
				{
					AosDateTime tt(vv, mDataFieldFormat);
					if (tt.isNotADateTime()) 
					{   
						OmnAlarm << "Current DateTime Object is invalid" << enderr;
						return false;
					}   
					value.setDateTime(tt);
					break;
				}
			default:
				OmnAlarm << "not handle this type:" << AosDataType::getTypeStr(mDataType) << enderr;
				break;
		}
	}
	else
	{
		//OmnScreen << "Failed to find the primary key from joining table: " << key << endl;
		//value.setStr("__NULL__");
		mIILMap->unlock();
		value.setNull();
	}

	return true;
}


bool 
AosIILMap::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	//check if the number of arguments >= 2
	if (mSize < 2)
	{
		errmsg << "IILMap needs 2 parameter at least";
		return false;
	}

	return true;
}


AosJimoPtr
AosIILMap::cloneJimo()  const
{
	try
	{
		return OmnNew AosIILMap(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed clone Jimo" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}
