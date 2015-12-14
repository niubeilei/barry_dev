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
// 2013/06/10 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DynJimos/ActionsOne/JimoSetDataletAttr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include <string>

static AosJimo::funcmap_t sgFuncMap;
static bool sgInited = false;
static OmnMutex sgLock;

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosJimoSetDataletAttr(
 		const AosRundataPtr &rdata, 
 		const AosXmlTagPtr &worker_doc, 
 		const AosXmlTagPtr &jimo_doc) 
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		return OmnNew AosJimoSetDataletAttr(rdata, worker_doc, jimo_doc);
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


AosJimoSetDataletAttr::AosJimoSetDataletAttr(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)
:
AosJimo(rdata, worker_doc, jimo_doc)
{
	if (!init(rdata, worker_doc, jimo_doc))
	{
		OmnThrowException(rdata->getErrmsg());
	}
}


AosJimoSetDataletAttr::AosJimoSetDataletAttr(const AosJimoSetDataletAttr &rhs)
:
AosJimo(rhs)
{

}


AosJimoSetDataletAttr::~AosJimoSetDataletAttr()
{
}


bool 
AosJimoSetDataletAttr::run(		
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker)
{
	// This action add an attribute to a doc. 
	// 	<worker 
	// 		AOSTAG_USE_JIMO="xxx"
	// 		AOSTAG_ATTRNAME="xxx"
	// 		AOSTAG_VALUE="xxx"
	// 		AOSTAG_VALUE_UNIQUE="true|false"
	// 		AOSTAG_DOCID_UNIQUE="true|false"
	// 		AOSTAG_DATATYPE="xxx">
	// 		<AOSTAG_DOCSELECTOR .../>		// Doc selector
	// 		<AOSTAG_VALUE .../>				// Value selector
	// 		<AOSTAG_ATTRNAME .../>			// Value selector
	// 		<AOSTAG_VALUE_UNIQUE .../>		// Value selector
	// 		<AOSTAG_DOCID_UNIQUE .../>		// Value selector
	// 		<AOSTAG_DATATYPE .../>			// Value selector
	// 	</worker>
	if (!worker)
	{
		AosSetErrorUser(rdata, "missing_worker_doc");
		return false;
	}

	// Retrieve the source doc
	AosXmlTagPtr source_doc = AosRunDocSelector(rdata, worker, AOSTAG_DOCSELECTOR);
	if (!source_doc)
	{
		AosSetErrorUser(rdata, "failed_retrieve_doc") << worker->toString() << enderr;
	 	return false;
	}

	// Retrieve the value
	AosValueRslt value = AosGetValue(rdata, worker, AOSTAG_VALUE, AOSTAG_VALUE);
	if (!value.isValid())
	{
		AosSetErrorUser(rdata, "failed_retrieving_value") << worker->toString() << enderr;
		return false;
	}

	// Retrieve xpath, value_unique, and docid_unique
	OmnString aname = AosGetValueStr(rdata, worker, AOSTAG_ATTRNAME, AOSTAG_ATTRNAME, "");
	if (aname == "")
	{
		AosSetErrorUser(rdata, "attrname_empty") << worker->toString() << enderr;
		return false;
	}

	bool value_unique = AosGetValueBool(rdata, worker, AOSTAG_VALUE_UNIQUE, 
							AOSTAG_VALUE_UNIQUE, false);
	bool docid_unique = AosGetValueBool(rdata, worker, AOSTAG_DOCID_UNIQUE, 
							AOSTAG_DOCID_UNIQUE, false);
	OmnString datatype = AosGetValueStr(rdata, worker, AOSTAG_DATATYPE, 
							AOSTAG_DATATYPE, AOSDATATYPE_STRING);

	OmnString value_str = value.getValueStr1();
	if (datatype == AOSDATATYPE_STRING)
	{
		return AosModifyDocAttrStr(rdata, source_doc, aname, value_str, 
				value_unique, docid_unique);
	}

	if (datatype == AOSDATATYPE_U64)
	{
		u64 vv;
		bool rslt = value.getU64Value(vv, rdata);
		if (!rslt)
		{
			AosSetErrorUser(rdata, "failed_retrieve_value") 
				<< worker->toString() << enderr;
			return false;
		}

		return AosModifyDocAttrU64(rdata, source_doc, aname, vv, 
				value_unique, docid_unique);
	}
	
	AosSetErrorUser(rdata, "invalid_data_type") << worker->toString() << enderr;
	return false;
	return true;
}


bool
AosJimoSetDataletAttr::init(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)
{
	if (!sgInited) registerMethods();
	return config(rdata, worker_doc, jimo_doc);
}


bool
AosJimoSetDataletAttr::registerMethods()
{
	sgLock.lock();
	if (sgInited)
	{
		sgLock.unlock();
		return true;
	}

	sgInited = true;
	sgLock.unlock();
	return true;
}


bool
AosJimoSetDataletAttr::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)
{
	return true;
}


OmnString
AosJimoSetDataletAttr::toString() const
{
	return "";
}


bool 
AosJimoSetDataletAttr::serializeTo(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff) 
{
	bool rslt = AosJimo::serializeTo(rdata, buff);
	aos_assert_rr(rslt, rdata, false);
	return true;
}


bool 
AosJimoSetDataletAttr::serializeFrom(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff)
{
	bool rslt = AosJimo::serializeFrom(rdata, buff);
	aos_assert_rr(rslt, rdata, false);
	return true;
}


AosJimoPtr 
AosJimoSetDataletAttr::clone(const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosJimoSetDataletAttr(*this);
	}

	catch (...)
	{
		AosSetErrorU(rdata, "failed_cloning_jimo") << toString() << enderr;
		return 0;
	}
}


void * 
AosJimoSetDataletAttr::getMethod(
		const AosRundataPtr &rdata, 
		const OmnString &name, 
		AosMethodId::E &method_id)
{
	return AosJimo::getMethod(rdata, name, sgFuncMap, method_id);
}

