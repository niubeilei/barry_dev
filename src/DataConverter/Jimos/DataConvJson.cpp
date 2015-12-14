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
// 2013/08/14 Created by Young Pan
////////////////////////////////////////////////////////////////////////////
#if 0
#include "DataConverter/DataConverterJson.h"

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
AosJimoPtr AosCreateJimoFunc_AosDataConverterJson_0(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &jimo_doc, 
		const OmnString &version) 
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		return OmnNew AosDataConverterJson(rdata, jimo_doc, version);
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


AosDataConverterJson::AosDataConverterJson()
:
AosDataConverter()
{
}


AosDataConverterJson::AosDataConverterJson(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &jimo_doc, 
		const OmnString &version)
:
AosDataConverter(rdata, jimo_doc, version, "data_converter_json")
{
	if (!init(rdata, jimo_doc))
	{
		OmnThrowException(rdata->getErrmsg());
	}
}


AosDataConverterJson::~AosDataConverterJson()
{
}


bool
AosDataConverterJson::init(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &jimo_doc)
{
	if (!sgInited) registerMethods();
	return true;
}


bool                    
AosDataConverterJson::registerMethods()
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
AosDataConverterJson::run(const AosRundataPtr &rdata)   
{   
	OmnNotImplementedYet;
	return false;       
}                       


bool
AosDataConverterJson::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker)
{
	aos_assert_r(worker, false);
	bool rslt = AosDataConverter::config(rdata, worker);
	aos_assert_r(rslt, false);
	return true;
}


void *
AosDataConverterJson::getMethod(                                
		const AosRundataPtr &rdata, 
		const OmnString &name, 
		AosMethodId::E &method_id)
{
	return AosJimo::getMethod(rdata, name, sgFuncMap, method_id);
}                       
                        

AosJimoPtr
AosDataConverterJson::cloneJimo() const
{
	try
	{
		return OmnNew AosDataConverterJson(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return 0;
	}
}
#endif
