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
#include "GenericVerbs/GenericVerb.h"

#include "Rundata/Rundata.h"
#include "SEInterfaces/GenericObjMgrObj.h"
#include "SEInterfaces/GenericMethodObj.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosGenericVerb_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosGenericVerb(version);
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


AosGenericVerb::AosGenericVerb(const int version)
:
AosGenericVerbObj(version),
mLock(OmnNew OmnMutex())
{
}


AosGenericVerb::AosGenericVerb(const AosGenericVerb &rhs)
:
AosGenericVerbObj(rhs),
mLock(OmnNew OmnMutex()),
mVerb(rhs.mVerb)
{
}


AosGenericVerb::~AosGenericVerb()
{
}


bool
AosGenericVerb::config(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &jimo_doc)
{
	// 	<generic_verb name="xxx">
	// 		<objects>
	// 			<object name="xxx" version="xxx" .../>
	// 			<object name="xxx" version="xxx" .../>
	// 			...
	// 		</objects>
	// 	</generic_verb>
	mObjects.clear();

	AosRundata *rdata_raw = rdata.getPtr();
	if (!jimo_doc)
	{
		AosSetErrorUser(rdata, "missing_jimo_doc") << enderr;
		return false;
	}

	mVerb = jimo_doc->getAttrStr("name");
	if (mVerb == "")
	{
		AosSetErrorUser(rdata, "missing_verb_name") << jimo_doc << enderr;
		return false;
	}

	AosXmlTagPtr objects = jimo_doc->getFirstChild(AOSTAG_OBJECTS);
	if (!objects)
	{
		AosSetErrorUser(rdata, "missing_objects") << jimo_doc << enderr;
		return false;
	}

	AosXmlTagPtr object = objects->getFirstChild();
	if (!object)
	{
		AosSetErrorUser(rdata, "missing_objects") << jimo_doc << enderr;
		return false;
	}

	while (object)
	{
		OmnString obj_type = object->getAttrStr("name");
		int version = object->getAttrInt("version", -1);
		AosGenericMethodObjPtr gen_obj = AosGenericObjMgrObj::getSelf(rdata_raw)->getGenericObj(
				rdata_raw, mVerb, obj_type, version);
		if (!gen_obj)
		{
			AosSetErrorUser(rdata, "failed_create_object") << object << enderr;
			continue;
		}

		itr_t itr = mObjects.find(obj_type);
		if (itr != mObjects.end())
		{
			AosSetErrorUser(rdata, "object_already_defined") << jimo_doc << enderr;
			return false;
		}

		mObjects[obj_type] = gen_obj;
		object = objects->getNextChild();
	}

	return true;
}


AosJimoPtr
AosGenericVerb::cloneJimo() const
{
	return OmnNew AosGenericVerb(*this);
}


bool 
AosGenericVerb::proc(
		AosRundata *rdata, 
		const OmnString &obj_type,
		const OmnString &obj_name, 
		const OmnString &jsonstr)
{
	mLock->lock();
	itr_t itr = mObjects.find(obj_type);
	if (itr == mObjects.end())
	{
		mLock->unlock();
		AosSetErrorUser(rdata, "obj_type_not_found") 
			<< "Verb=" << mVerb 
			<< ", Object Type = " << obj_type << enderr;
		return false;
	}
	mLock->unlock();
	return itr->second->proc(rdata, obj_name, jsonstr);
}

