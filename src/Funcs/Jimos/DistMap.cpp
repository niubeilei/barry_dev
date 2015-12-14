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
// 2015/04/30 Created by Barry 
////////////////////////////////////////////////////////////////////////////
#include "Funcs/Jimos/DistMap.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"


extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosDistMap_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosDistMap(version);
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


AosDistMap::AosDistMap(const int version)
:
AosGenFunc("DistMap", version)
{
}

AosDistMap::AosDistMap()
:
AosGenFunc("DistMap", 1)
{
}



AosDistMap::AosDistMap(const AosDistMap &rhs)
:
AosGenFunc(rhs)
{
}


AosDistMap::~AosDistMap()
{
}


bool
AosDistMap::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosDistMap::getDataType(
		AosRundata *rdata, 
		AosDataRecordObj *record)
{
	OmnNotImplementedYet;
	return AosDataType::eInvalid;
}


bool
AosDistMap::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	if (!mDistMap)
	{
		OmnString mapName, iilName;
		if (!AosGenFunc::getValue(rdata, 0,  record)) return false;
		mapName = mValue.getStr();
		
		//get map doc
		AosXmlTagPtr doc = AosJqlStatement::getDoc(rdata, JQLTypes::eMap, mapName);
		aos_assert_r(doc, false);

		//create IILEntryMap
		iilName = doc->getAttrStr(AOSTAG_IILNAME);
		if (iilName == "")
		{
			AosSetErrorU(rdata, "missing iil name:") << doc->toString();
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		OmnString type = doc->getAttrStr("datatype", "string");
		mDataType = AosDataType::toEnum(type);
		aos_assert_r(mDataType != AosDataType::eInvalid, false);

		bool need_split = doc->getAttrBool(AOSTAG_NEEDSPLIT, false);
		OmnString sep = doc->getAttrStr(AOSTAG_SEPARATOR);
		bool need_swap = doc->getAttrBool(AOSTAG_NEEDSWAP, false);
		bool use_key_as_value = doc->getAttrBool(AOSTAG_USE_KEY_AS_VALUE, false);

		
		mDistMap = AosIILEntryMapMgr::retrieveIILEntryMap(   
					iilName, need_split, sep, need_swap, use_key_as_value, rdata);
		aos_assert_r(mDistMap, false);

		mDistMap->readlock();
		u32 size = mDistMap->size();
		mDistMap->unlock();

		if (size == 0) 
		{
			OmnScreen << "map size is 0, iilname:" << iilName << endl;
		}
	}

	bool rslt = false;
	OmnString key = "";
	for (size_t i = 1; i < mSize; ++i)
	{
		rslt = AosGenFunc::getValue(rdata, i, record);
		aos_assert_r(rslt, false);

		if (mValue.isNull() ||  mValue.getStr() == "")
		{
			value.setU64(0);
			return true;
		}

		if (i > 1) key << char(0x01);
		key << mValue.getStr();	//key01\0x01key02
	}

	OmnString cubeId = getCubeId(key);

	value.setStr(cubeId);
	return true;
}


OmnString
AosDistMap::getCubeId(
		const OmnString &key)
{
	OmnString cubeId = "";
	map<OmnString, OmnString>::iterator itr_up;
	mDistMap->readlock();
	itr_up = mDistMap->upper_bound(key);

	if (itr_up != mDistMap->end())
	{
		cubeId = itr_up->second;
	}
	else
	{
		int id = ((--itr_up)->second).toInt() + 1;
		cubeId << id;
	}	
	mDistMap->unlock();

	return cubeId;
}


bool 
AosDistMap::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	//check if the number of arguments == 2
	if (mSize != 2)
	{
		errmsg << "DistMap needs 2 parameter at least";
		return false;
	}

	return true;
}


AosJimoPtr
AosDistMap::cloneJimo()  const
{
	try
	{
		return OmnNew AosDistMap(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed clone Jimo" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}
