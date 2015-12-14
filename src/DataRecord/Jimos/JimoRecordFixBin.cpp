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
// This is binary fixed length record:
// 	String		fixed length
// 	char *		fixed length
// 	bool		one byte
// 	char		one byte
// 	int			sizeof(int)
// 	int64_t		sizeof(int64_t)
// 	u32			sizeof(u32)
// 	u64			sizeof(u64)
// 	double		sizeof(double)
//
// Modification History:
// 07/23/2013 Created by Young Pan
////////////////////////////////////////////////////////////////////////////
#if 0
#include "DataRecord/Jimos/JimoRecordFixBin.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Rundata/Rundata.h"
#include "Util/ValueRslt.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"

static AosJimoUtil::funcmap_t sgFuncMap;
static bool sgInited = false;
static OmnMutex sgLock;

extern "C"
{
AosJimoPtr AosCreateJimoFunc_AosJimoRecordFixBin_0(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &jimo_doc, 
		const OmnString &version) 
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		return OmnNew AosJimoRecordFixBin(rdata, jimo_doc, version);
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


AosJimoRecordFixBin::AosJimoRecordFixBin(const bool flag AosMemoryCheckDecl)
:
mDataLen(0),
mRecordLen(0)
{
}


AosJimoRecordFixBin::AosJimoRecordFixBin(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &jimo_doc, 
		const OmnString &version)
:
AosJimoDataRecord(rdata, jimo_doc, version, "datarecord_fix_bin"),
mDataLen(0),
mRecordLen(0)
{
	if (!init(rdata, jimo_doc))
	{
		OmnThrowException(rdata->getErrmsg());
	}
}


AosJimoRecordFixBin::AosJimoRecordFixBin(
		const AosJimoRecordFixBin &rhs,
		const AosRundataPtr &rdata AosMemoryCheckDecl)
:
AosJimoDataRecord(rhs),
mInternalData(rhs.mInternalData),
mData((char *)mInternalData.data()),
mDataLen(rhs.mDataLen),
mRecordLen(rhs.mRecordLen)
{
}


AosJimoRecordFixBin::~AosJimoRecordFixBin()
{
}


bool
AosJimoRecordFixBin::init(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &jimo_doc)
{
	if (!sgInited) registerMethods();
	return true;
}


bool                    
AosJimoRecordFixBin::registerMethods()
{   
    sgLock.lock();      
    if (sgInited)       
    {
        sgLock.unlock();
        return true;    
    }                   

    // registerMethod("next_value", sgFuncMap,                 
    //      AosMethodId::eAosRundata_Jimo_WorkerDoc,
    //      (void *)AosJimoDataProcIILIndexing_nextValue);
                        
    sgInited = true;
    sgLock.unlock();
    return true;
}   


bool 
AosJimoRecordFixBin::run(const AosRundataPtr &rdata)   
{   
	OmnNotImplementedYet;
	return false;       
}                       


AosJimoPtr
AosJimoRecordFixBin::cloneJimo(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)  const            
{
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	rdata->setSiteid(100);
	try
	{
		return OmnNew AosJimoRecordFixBin(*this, rdata AosMemoryCheckerArgs);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating jimo" << enderr;
		return 0;
	}
	return 0;
}


bool
AosJimoRecordFixBin::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def)
{
	aos_assert_r(def, false);

	bool rslt = AosDataRecord::config(def, rdata);
	aos_assert_r(rslt, false);

	mRecordLen = def->getAttrInt("record_len", 0);
	aos_assert_r(mRecordLen > 0, false);

	mInternalData.setLength(mRecordLen);
	mData = (char *)mInternalData.data();
	memset(mData, 0, mRecordLen);
	mDataLen = mRecordLen;

	return true;
}


void *
AosJimoRecordFixBin::getMethod(                                
		const AosRundataPtr &rdata, 
		const OmnString &name, 
		AosMethodId::E &method_id)
{
	return AosJimo::getMethod(rdata, name, sgFuncMap, method_id);
}                       
                        

AosDataRecordObjPtr 
AosJimoRecordFixBin::clone(const AosRundataPtr &rdata AosMemoryCheckDecl) const
{
	try
	{
		return OmnNew AosJimoRecordFixBin(*this, rdata AosMemoryCheckerFileLine);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return 0;
	}
}


AosDataRecordObjPtr 
AosJimoRecordFixBin::create(
		const AosXmlTagPtr &def,
		const AosTaskObjPtr &task,
		const AosRundataPtr &rdata AosMemoryCheckDecl) const
{
	AosJimoRecordFixBin * record = OmnNew AosJimoRecordFixBin(false AosMemoryCheckerFileLine);
	record->setTask(task);
	bool rslt = record->config(def, rdata);
	aos_assert_r(rslt, 0);
	return record;
}


bool		
AosJimoRecordFixBin::getFieldValue(
		const int idx,
		AosValueRslt &value,
		const AosRundataPtr &rdata) 
{
	aos_assert_r(idx >= 0 && (u32)idx < mFields.size(), false);

	if (mFieldValues.find(idx) != mFieldValues.end())
	{
		value = mFieldValues[idx];
		return true;
	}

	int index = 0;
	AosDataRecordObjPtr thisptr(this, false);
	bool rslt = mFields[idx]->getValueFromRecord(
		thisptr, mData, mDataLen, index, value, true, rdata);
	aos_assert_r(rslt, false);

	mFieldValues[idx] = value;
	return true;
}


bool		
AosJimoRecordFixBin::setFieldValue(
		const int idx, 
		const AosValueRslt &value, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(idx >= 0 && (u32)idx < mFields.size(), false);

	AosDataRecordObjPtr thisptr(this, false);
	bool rslt = mFields[idx]->setValueToRecord(thisptr, value, rdata);
	aos_assert_r(rslt, false);

	mFieldValues[idx] = value;
	return true;
}


bool
AosJimoRecordFixBin::getFieldValue(
		const char *data,
		const int data_len,
		const OmnString &field_name,
		AosValueRslt &value,
		const AosRundataPtr &rdata)
{
	int idx = getFieldIdx(field_name, rdata);
	return getFieldValue(data, data_len, idx, value, rdata);
}


bool
AosJimoRecordFixBin::getFieldValue(
		const char *data,
		const int data_len,
		const int field_idx,
		AosValueRslt &value,
		const AosRundataPtr &rdata)
{
	aos_assert_r(field_idx >=0 && (u32)field_idx < mFields.size(), false);

	mLock->lock();
	bool rslt = setData((char *)data, data_len, 0, false);
	aos_assert_rl(rslt, mLock, false);

	rslt = getFieldValue(field_idx, value, rdata);
	aos_assert_rl(rslt, mLock, false);

	clear();
	mLock->unlock();	

	return true;
}



void
AosJimoRecordFixBin::clear()
{
	mInternalData.setLength(mRecordLen);
	mData = (char *)mInternalData.data();
	memset(mData, 0, mRecordLen);
	
	AosDataRecord::clear();
}


bool 		
AosJimoRecordFixBin::setData(
		char *data,
		const int len,
		const AosBuffDataPtr &metaData,
		const bool need_copy)
{
	if (!data || len <= 0) return false;
	if (len < mRecordLen) return false;
	mMetaData = metaData;

	if (need_copy)
	{
		mInternalData.assign(data, len);
		mData = (char *)mInternalData.data();
		mDataLen = len;
		return true;
	}

	mData = data;
	mDataLen = len;
	return true;
}


AosXmlTagPtr
AosJimoRecordFixBin::serializeToXmlDoc(
		const char *data, 
		const int data_len,
		const AosRundataPtr &rdata)
{
	OmnString docstr;
	docstr << "<" << mName << ">";
	
	bool rslt = true;
	int idx = 0;
	for (u32 i=0; i<mFields.size(); i++)
	{
		rslt = mFields[i]->serializeToXmlDoc(docstr, idx, data, data_len, rdata);
		aos_assert_r(rslt, 0);
	}
	
	docstr << "</" << mName << ">";
	AosXmlTagPtr doc = AosXmlParser::parse(docstr AosMemoryCheckerArgs);
	aos_assert_r(doc, 0);

	return doc;
}


AosBuffPtr
AosJimoRecordFixBin::serializeToBuff(
		const AosXmlTagPtr &doc,
		const AosRundataPtr &rdata)
{
	aos_assert_r(doc, 0);
		
	bool rslt = true;
	AosBuffPtr buff = OmnNew AosBuff(eBuffInitSize AosMemoryCheckerArgs);
	for (u32 i=0; i<mFields.size(); i++)
	{
		rslt = mFields[i]->serializeToBuff(doc, buff, rdata);
		aos_assert_r(rslt, 0);
	}
	return buff;	
}


bool
AosJimoRecordFixBin::createRandomDoc(
		const AosBuffPtr &buff,
		const AosRundataPtr &rdata)
{
	int len = mFields.size();
	aos_assert_r(len > 0, false);

	bool rslt = true;
	for(int i=0; i<len; i++)
	{
		rslt = mFields[i]->createRandomValue(buff, rdata);
		aos_assert_r(rslt, false);
	}
	return true;
}
#endif
