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
#include "GenericObjs/GenericObj/RemoveMethod.h"

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

AosJimoPtr AosCreateJimoFunc_AosMethodRemoveMethod_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosMethodRemoveMethod(version);
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



AosMethodRemoveMethod::AosMethodRemoveMethod(const int version)
:
AosGenericMethod(version)
{
}


AosMethodRemoveMethod::~AosMethodRemoveMethod()
{
}


bool
AosMethodRemoveMethod::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)
{
	return true;
}


AosJimoPtr 
AosMethodRemoveMethod::cloneJimo() const
{
	return OmnNew AosMethodRemoveMethod(*this);
}


bool 
AosMethodRemoveMethod::proc(
		AosRundata *rdata,
		const OmnString &obj_type, 
		const vector<AosExprObjPtr> &parms)
{
	// This function removes a method from a generic object. 
	// The JQL format:
	// 	remove-member "generic_object" "obj_type" 
	// 	(
	// 	    versoin:"xxx",
	// 	    method:"xxx"
	// 	)

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

	AosDbTransObjPtr trans = AosCreateDbTrans(rdata);
	if (!trans)
	{
		AosSetErrorUser(rdata, "failed_creating_trans") << enderr;
		return false;
	}

	// Retrieve the generic object
	AosGenericObjPtr gen_obj = AosGetGenericObj(rdata, obj_type, version);
	if (!gen_obj)
	{
		AosTransLog(trans, AOSLOGENTRYID_FAILED_GET_GENERIC_OBJECT)
			<< rdata->getErrmsg() << enderr;
		trans->cancel(rdata);
		return false;
	}

	// Retrieve the method
	OmnString method_name = record->getFieldStr(AOSPARM_METHOD_NAME);
	if (method_name == "")
	{
		AosSetErrorUser(rdata, "missing_method_name")
			<< ErrorEntry("Object Type") << obj_type
			<< ErrorEntry("JQL Statement") << rdata->getArg1(AOSARG_JQL_STATEMENT) << ender;
		AosTransLog(trans, rdata->getErrmsg());
		trans->cancel();
		return false;
	}

	AosGenericMethodObjPtr method = AosGetGenericMethod(rdata, method_name);
	if (!method)
	{
		AosSetErrorUser(rdata, "method_not_defined")
			<< ErrorEntry("Object Type") << obj_type
			<< ErrorEntry("Method Name") << method_name 
			<< ErrorEntry("JQL Statement") << rdata->getArg1(AOSARG_JQL_STATEMENT) << ender;
		AosTransLog(trans, rdata->getErrmsg());
		trans->cancel();
		return false;
	}

	bool rslt = gen_obj->addMethod(rdata, method);
	if (!rslt)
	{
		AosSetErrorUser(rdata, "failed_add_method")
			<< ErrorEntry("Object Type") << obj_type
			<< ErrorEntry("Method Name") << method_name 
			<< ErrorEntry("JQL Statement") << rdata->getArg1(AOSARG_JQL_STATEMENT) << ender;
		AosTransLog(trans, rdata->getErrmsg());
		trans->cancel();
	}

	trans->commit(rdata);
	return true;
}

