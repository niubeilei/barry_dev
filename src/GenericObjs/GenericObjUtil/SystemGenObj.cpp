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
//
// Modification History:
// 2014/10/26 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "GenericObjs/SystemGenObj.h"

#include "GenericObjs/Ptrs.h"
#include "GenericObjs/GenericMethod.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/GenericMethodObj.h"


extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosSystemGenObj_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosSystemGenObj(version);
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


AosSystemGenObj::AosSystemGenObj(const int version)
:
AosGenericObj(version)
{
}


AosSystemGenObj::~AosSystemGenObj()
{
}


bool
AosSystemGenObj::config(
		AosRundata *rdata, 
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)
{
	// This function creates the generic object
	// 	<gen_obj 
	// 		objtype="xxx"/>
	// 		<verbs>
	// 			<verb name="xxx" classname="xxx" version="xxx"/oo>
	// 			<verb name="xxx" classname="xxx" version="xxx"/oo>
	// 			...
	// 		</methods>
	// 	</gen_obj>
	if (!jimo_doc)
	{
		AosSetErrorUser(rdata, "missing_jimo_doc") << enderr;
		return false;
	}

	mObjType = jimo_doc->getAttrStr(AOSTAG_OBJECT_TYPE);
	if (mObjType == "")
	{
		AosSetErrorUser(rdata, "missing_objname") << jimo_doc << enderr;
		return false;
	}

	AosXmlTagPtr verbs = jimo_doc->getFirstChild(AOSTAG_VERBS);
	if (!verbs)
	{
		AosSetErrorUser(rdata, "missing_verbs") << jimo_doc << enderr;
		return false;
	}

	AosXmlTagPtr verb = verbs->getFirstChild();
	if (!verb)
	{
		AosSetErrorUser(rdata, "missing_verbs") << jimo_doc << enderr;
		return false;
	}

	while (verb)
	{
		// Create the jimo
		AosJimoPtr jimo = AosCreateJimo(rdata, worker_doc, verb);
		if (!jimo)
		{
			AosSetErrorUser(rdata, "failed_create_method") << verb<< enderr;
			return false;
		}

		if (jimo->getJimoType() != AosJimoType::eGenericMethod)
		{
			AosSetErrorUser(rdata, "internal_error") << verb << enderr;
			return false;
		}

		AosGenericMethodPtr method = dynamic_cast<AosGenericMethod*>(jimo.getPtr());
		if (!method) 
		{
			AosSetErrorUser(rdata, "internal_error") << verb << enderr;
			return false;
		}

		OmnString method_name = method->getMethodName();
		if (method_name == "")
		{
			AosSetErrorUser(rdata, "method_name_empty") << jimo_doc << enderr;
			return false;
		}

		itr_t itr = mMethods.find(method_name);
		if (itr != mMethods.end())
		{
			AosSetErrorUser(rdata, "method_already_defined") << jimo_doc << enderr;
			return false;
		}

		mMethods[method_name] = method;
		verb = verbs->getNextChild();
	}

	return true;
}


bool 
AosSystemGenObj::proc(
		AosRundata *rdata, 
		const OmnString &verb,
		const OmnString &obj_name, 
		const vector<AosGenericValueObjPtr> &parms)
{
	itr_t itr = mMethods.find(verb);
	if (itr == mMethods.end())
	{
		AosSetErrorUser(rdata, "method_not_defined") 
			<< "Object Type=" << mObjType
			<< "Verb = " << verb << enderr;
		return false;
	}

	return itr->second->proc(rdata, obj_name, parms);
}


AosJimoPtr
AosSystemGenObj::cloneJimo() const
{
	OmnShouldNeverComeHere;
	return 0;
}

