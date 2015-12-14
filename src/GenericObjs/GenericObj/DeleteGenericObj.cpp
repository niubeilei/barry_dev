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
// 2014/10/26 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "GenericObjs/GenericObj/CreateGenericObj.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Jimo/Ptrs.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"
#include "JQLParser/JQLParser.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosMethodCreateGenericObj_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosMethodCreateGenericObj(version);
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



AosMethodCreateGenericObj::AosMethodCreateGenericObj(const int version)
:
AosGenericMethod(version)
{
}


AosMethodCreateGenericObj::~AosMethodCreateGenericObj()
{
}


bool
AosMethodCreateGenericObj::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)
{
	return true;
}


AosJimoPtr 
AosMethodCreateGenericObj::cloneJimo() const
{
	return OmnNew AosMethodCreateGenericObj(*this);
}


bool 
AosMethodCreateGenericObj::proc(
		AosRundata *rdata,
		const OmnString &obj_type, 
		const vector<AosExprObjPtr> &parms)
{
	// This function creates a generic object. 
	// The JQL format:
	// 	create "generic_object" "obj_type" 
	// 	(
	// 	    versoin:"xxx"
	// 	    container:"xxx"
	// 	)
	// It creates the datalet. If the datalet is already there, it is an error.
	// If the 'obj_type' already exists, it is an error. 

	if (obj_type == "")
	{
		AosSetErrorUser(rdata, "object_type_empty") << enderr;
		return false;
	}

	AosDataRecordObjPtr record = AosDataRecordObj::createRecord(rdata, parms);
	if (!record)
	{
		AosSetErrorUser(rdata, "internal_error") << enderr;
		return false;
	}

	// Retrieve the version
	AosValueRslt vv;
	int version = AosJimo::eDefaultVersion;
	if (record->getFieldValue(AOSPARM_VERSION, vv, false, rdata))
	{
		version = vv.getIntValue(rdata);
	}

	// Retrieve the container. If container is not specified, it defaults to 
	// the system default container.
	OmnString container;
	if (record->getFieldValue(AOSPARM_CONTAINER, vv, false, rdata))
	{
		container = vv.getValueStr1("");
	}
	if (container == "")
	{
		container = AOS_SYSTEM_TABLE_GENERIC_OBJECTS;
	}

	AosGenericObjMgrObjPtr mgr = AosGenericObjMgrObj::getSelf(rdata);
	if (!mgr)
	{
		AosTransLog(trans, AOSLOGENTRYID_INTERNAL_ERROR)
			<< rdata->getErrmsg() << enderr;
		trans->cancel(rdata);
		return false;
	}

	AosGenericObjPtr gen_obj = mgr->createGenericObj(rdata, obj_type, version);
	if (!gen_obj)
	{
		AosTransLog(trans, AOSLOGENTRYID_FAILED_CREATE_GENERIC_OBJID)
			<< rdata->getErrmsg() << enderr;
		trans->cancel(rdata);
		return false;
	}

	// Create the datalet
	AosXmlTagPtr doc = createDatalet(rdata, container, obj_type);
	if (!doc)
	{
		AosTransLog(trans, AOSLOGENTRYID_FAILED_CREATE_DATALET)
			<< rdata->getErrmsg() << enderr;
		mgr->removeGenericObj(rdata, obj_type, version);
		trans->cancel();
		return false;
	}

	trans->commit(rdata);
	return true;
}


bool
AosCreateGenericObj::createDatalet(
		AosRundata *rdata, 
		const OmnString &container, 
		const OmnString &obj_type)
{
	// Generic objects are defined by datalets. This function creates the datalet. 
	// All generic object datalets are stored in a specific container. 
	bool rslt = AosWriteLockContainer(rdata, container);
	if (!rslt) return false;

	// Check whether the obj_type is already defined
	AosXmlTagPtr doc = AosGetDocByKey(rdata, container, AOSPARM_OBJECT_TYPE, obj_type);
	if (doc)
	{
		// It is already defined. 
		AosSetErrorUser(rdata, "object_type_already_defined") 
			<< "Object Type=" << obj_type
			<< "Container=" << container << enderr;
		return false;
	}

	OmnString doc_str;
	doc_str = "<gen_obj ";
	doc_str << AOSPARM_OBJTYPE << "=\"" << obj_type << "\" "
		<< AOSTAG_HPCONTAINER << "=\"" << container << "\" "
		<< AOSTAG_OTYPE << "=\"" << AOSOTYPE_GENERIC_OBJECT << "\"/>";
	doc = AosCreateDoc(rdata, doc_str, true);
	if (!doc)
	{
		AosSetErrorUser(rdata, "failed_creating_doc")
			<< ErrorEntry("Object Type") << obj_type
			<< ErrorEntry("Container") << container << enderr;
		return false;
	}

	return doc;
}

