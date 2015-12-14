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
#include "Funcs/Jimos/IILEntry.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"



extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosIILEntry_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosIILEntry(version);
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


AosIILEntry::AosIILEntry(const int version)
:
AosGenFunc("IILEntry", version)
{
}

AosIILEntry::AosIILEntry()
:
AosGenFunc("IILEntry", 1)
{
}



AosIILEntry::AosIILEntry(const AosIILEntry &rhs)
:
AosGenFunc(rhs)
{
	mIILEntry = rhs.mIILEntry;
	mDataType = rhs.mDataType;
}


AosIILEntry::~AosIILEntry()
{
}


bool
AosIILEntry::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosIILEntry::getDataType(
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
AosIILEntry::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
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
	OmnString key = "";
	rslt = false;
	for (size_t i = 1; i < mSize; i++)
	{
		rslt = AosGenFunc::getValue(rdata, i, record);
		aos_assert_r(rslt, false);

		if (mValue.isNull() || mValue.getStr() == "")
		{
			value.setNull();
			return true;
		}

		if (i > 2) key << char(0x01);                                  
		key << mValue.getStr();   //key1\0x01key2\0x01key3
	}

	AosQueryRsltObjPtr query_rslt = AosQueryRsltObj::getQueryRsltStatic();                  
	query_rslt->setWithValues(true);                                                        

	AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();   
	aos_assert_r(query_context, false);                                                     

	if (need_split)                                                                         
	{                                                                                       
		//key << sep;                                                                        
		query_context->setOpr(eAosOpr_prefix);                                              
	}                                                                                       
	else                                                                                    
	{                                                                                       
		query_context->setOpr(eAosOpr_eq);                                                  
	}                                                                                       

	query_context->setStrValue(key);                                                        
	query_context->setBlockSize(10);                                                        

	rslt = false;
	rslt = AosQueryColumn(iilName, query_rslt, 0, query_context, rdata);           
	aos_assert_r(rslt, false);

	if (query_rslt->getNumDocs() <= 0)
	{
		OmnScreen << "Failed to find the same key from table: " << key << " ; iilname : " << iilName << endl;
			//value.reset();
		value.setNull();
		return true;    
	}
	else
	{
		bool finished;
		u64 v;
		rslt = query_rslt->nextDocidValue(v, key, finished, rdata);
		aos_assert_r(rslt, false);
		
		AosConvertAsciiBinary(sep);
		OmnString vv;
		if (need_split)
		{
			vector<OmnString> keys;
			AosSplitStr(key, sep.data(), keys, 10);
			aos_assert_r(keys.size() == 2, false);
			aos_assert_r(keys[0] != "" && keys[1] != "", false);
			key = keys[0];
			vv = keys[1];
		}
		else
		{
			vv << (int64_t)v;   
		}
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
				value.setDouble(*(double*)(&vv));
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

	return true;
}


bool 
AosIILEntry::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	//check if the number of arguments >= 2
	if (mSize < 2)
	{
		errmsg << "IILEntry needs 2 parameter at least";
		return false;
	}

	return true;
}


AosJimoPtr
AosIILEntry::cloneJimo()  const
{
	try
	{
		return OmnNew AosIILEntry(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed clone Jimo" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}
