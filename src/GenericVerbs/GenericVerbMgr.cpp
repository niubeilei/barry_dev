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
#include "GenericVerbs/GenericVerbMgr.h"

#include "GenericVerbs/GenericVerb.h"
#include "SEUtil/GenericVerbNames.h"
#include "SEUtil/GenericObjTypes.h"

#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "API/AosApi.h"

static OmnMutex sgLock;
static bool 	sgInited = false;

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosGenericVerbMgr_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosGenericVerbMgr(version);
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



AosGenericVerbMgr::AosGenericVerbMgr(const int version)
:
AosGenericVerbMgrObj(version),
mLock(OmnNew OmnMutex())
{
	if (!sgInited) init();
}


AosGenericVerbMgr::AosGenericVerbMgr(const AosGenericVerbMgr &rhs)
:
AosGenericVerbMgrObj(rhs.mJimoVersion),
mLock(OmnNew OmnMutex()),
mVerbs(rhs.mVerbs),
mJimoDocs(rhs.mJimoDocs)
{
}

AosGenericVerbMgr::~AosGenericVerbMgr()
{
}


bool
AosGenericVerbMgr::init()
{
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	AosRundata *rdata_raw = rdata.getPtr();
	sgLock.lock();
	if (sgInited)
	{
		sgLock.unlock();
		return true;
	}

	bool rslt = createVerbs(rdata_raw);
	if (!rslt)
	{
		AosSetErrorUser(rdata, "internal_error") << enderr;
		sgInited = true;
		sgLock.unlock();
		return false;
	}

	sgInited = true;
	sgLock.unlock();
	return true;
}


bool
AosGenericVerbMgr::createVerbs(AosRundata *rdata)
{
	// 	<generic_verb name="xxx">
	// 		<objects>
	// 			<object name="xxx" .../>
	// 			<object name="xxx" .../>
	// 			...
	// 		</objects>
	// 	</generic_verb>
	OmnString doc_str = "<verb name=\"";
	doc_str << AOSGENERICVERB_CREATE << "\" version=\"1\" >"
			<< "<zky_objects>"
			<< "<object name=\"" << AOSGENOBJTYPE_SCHEMA << "\" version=\"1\"/>"
			<< "<object name=\"" << AOSGENOBJTYPE_RAND_DATA << "\" version=\"1\"/>"
			<< "<object name=\"" << AOSGENOBJTYPE_CLUSTER << "\" version=\"1\"/>"
			<< "</zky_objects>"
			<< "</verb>";

	bool rslt = createGenericVerb(rdata, doc_str);
	aos_assert_rr(rslt, rdata, false);
	return true;
}


bool
AosGenericVerbMgr::createGenericVerb(
		AosRundata *rdata, 
		const OmnString &doc_str)
{
	AosXmlTagPtr doc = AosStr2Xml(rdata, doc_str AosMemoryCheckerArgs);
	aos_assert_rr(doc, rdata, false);

	AosGenericVerbObjPtr verb = createVerb(rdata, doc);
	aos_assert_rr(verb, rdata, false);

	OmnString verb_name = verb->getVerbName();
	itr_t itr = mVerbs.find(verb_name);
	if (itr != mVerbs.end())
	{
		OmnAlarm << "Verbs re-defined: " << verb_name << enderr;
	}
	else
	{
		mVerbs[verb_name] = verb;
	}

	return true;
}


AosGenericVerbObjPtr
AosGenericVerbMgr::createVerb(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &jimo_doc)
{
	AosGenericVerbObjPtr verb = OmnNew AosGenericVerb(jimo_doc->getAttrInt("jimo_doc", 0));
	bool rslt = verb->config(rdata, jimo_doc);
	if (!rslt)
	{
		AosSetErrorUser(rdata, __func__ ) 
			<< jimo_doc->toString() << enderr;
		return NULL;
	}
	return verb;
}


bool 
AosGenericVerbMgr::addGenericVerb(
		AosRundata *rdata, 
		const AosGenericVerbObjPtr &verb)
{
	OmnString verbname = verb->getVerbName();
	aos_assert_rr(verbname != "", rdata, false);

	mLock->lock();
	itr_t itr = mVerbs.find(verbname);
	if (itr != mVerbs.end())
	{
		// The verb has already been registered. This is an error.
		mLock->unlock();
		AosSetErrorUser(rdata, "internal_error") << verbname << enderr;
		return false;
	}

	mVerbs[verbname] = verb;
	mLock->unlock();
	return true;
}


bool 
AosGenericVerbMgr::proc( 
		AosRundata *rdata, 
		const OmnString &verb,
		const OmnString &obj_type,
		const OmnString &obj_name, 
		const OmnString &jsonstr)
{
	mLock->lock();
	itr_t itr = mVerbs.find(verb);
	if (itr == mVerbs.end())
	{
		mLock->unlock();
		AosSetErrorUser(rdata, "verb_not_found") << verb << enderr;
		return false;
	}
	mLock->unlock();
	return itr->second->proc(rdata, obj_type, obj_name, jsonstr);
}


AosJimoPtr
AosGenericVerbMgr::cloneJimo() const
{
	return OmnNew AosGenericVerbMgr(*this);
}
