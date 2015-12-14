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
// 2014/10/26	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "GenericObjs/GenericObjMgr.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/GenericObj.h"
#include "GenericObjs/GenericMethod.h"
#include "SEUtil/GenericObjTypes.h"
#include "Thread/Mutex.h"

static bool 	sgInited = false;
static OmnMutex	sgLock;

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosGenericObjMgr_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosGenericObjMgr(version);
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


AosGenericObjMgr::AosGenericObjMgr(const int version)
:
AosGenericObjMgrObj(version),
mLock(OmnNew OmnMutex())
{
	if (!sgInited) init();
}


AosGenericObjMgr::AosGenericObjMgr(const AosGenericObjMgr &rhs)
:
AosGenericObjMgrObj(rhs.mJimoVersion),
mLock(OmnNew OmnMutex()),
mObjects(rhs.mObjects),
mObjectsJimoObjid(rhs.mObjectsJimoObjid)
{
}

AosGenericObjMgr::~AosGenericObjMgr()
{
}

bool
AosGenericObjMgr::init()
{
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);

	sgLock.lock();
	if (sgInited)
	{
		sgLock.unlock();
		return true;
	}


	// We will temporarily create a few generic objects. Generic objects can
	// be created through JQL statements. Once created, it should be saved
	// in Admin. These generic objects can be populated to new JimoOne installations.
	
	// Generic Object: "testing"
	// <gen_objs>
	//  <gen_obj objtype="schema">
	//      <verbs>
	//          <verb name="create" version="" jimodoc="xxxx" />
	//          <verb name="xxx" version="" jimodoc="xxxx""/>
	//          ...
	//      </methods>
	//  </gen_obj>
	//  ...
	// </gen_objs>
	
#if 0
	OmnString verb, obj_type, key, jimoobjid;
	int version = -1;

	AosXmlTagPtr verbs_conf, verb_conf;
	AosXmlTagPtr conf = getConf();
	AosXmlTagPtr gen_obj_conf = conf->getFirstChild(true);
	if (!gen_obj_conf)
	{
		AosSetErrorUser(rdata, __func__)
			<< "internal_error." << enderr;

		sgLock.unlock();
		return false;
	}

	while (gen_obj_conf)
	{
		obj_type = gen_obj_conf->getAttrStr("objtype");
		if (obj_type == "")
		{
			AosSetErrorUser(rdata, __func__)
				<< "internal_error." << enderr;

			sgLock.unlock();
			return false;
		}

		verbs_conf = gen_obj_conf->getFirstChild(true);
		if (verbs_conf == "")
		{
			AosSetErrorUser(rdata, __func__)
				<< "internal_error." << enderr;

			sgLock.unlock();
			return false;
		}

		verb_conf = verbs_conf->getFirstChild(true);
		while(verb_conf)
		{
			verb = verb_conf->getAttrStr("name");
			version = verb_conf->getAttrInt("version", -1);
			jimoobjid = verb_conf->getAttrStr("jimodoc"); 
			if (verb == "" || version < 0 || jimoobjid == "")
			{
				AosSetErrorUser(rdata, __func__)
					<< "internal_error. verb is " << verb
					<< ", version is " << version << ", "
					<< "jimoobjid is " << jimoobjid << enderr;

				sgLock.unlock();
				return false;
			}
			key = verb;
			key << "_" << obj_type << "_" << version;

			if (mObjectsJimoObjid.find(key) != mObjectsJimoObjid.end())
			{
				AosSetErrorUser(rdata, __func__)
					<< "internal_error" << enderr;

				sgLock.unlock();
				return false;
			}

			mObjectsJimoObjid[key] = jimoobjid;
			verb_conf = verbs_conf->getNextChild();
		}
		gen_obj_conf = conf->getNextChild();
	}

#endif
	sgLock.unlock();
	return true;
}


AosGenericMethodObjPtr
AosGenericObjMgr::createGenericObj(
		AosRundata *rdata, 
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)
{
	AosJimoPtr jimo = AosCreateJimo(rdata, worker_doc, jimo_doc, 1);
	if (!jimo)
	{
		AosSetError(rdata, "failed_create_jimo") << enderr;
		return NULL;
	}

	if (jimo->getJimoType() != AosJimoType::eGenericMethod)
	{
		AosSetErrorUser(rdata, "invalid_jimo") 
			<< "Expected Jimo Type=" << AosJimoType::eGenericObj
			<< "Actual Jimo Type=" << jimo->getJimoType() << enderr;
		return NULL;
	}

	AosGenericMethodObjPtr gen_obj = dynamic_cast<AosGenericMethodObj*>(jimo.getPtr());

	if (!gen_obj)
	{
		AosSetError(rdata, "internal_error") << enderr;
		return NULL;
	}

	return gen_obj;
}


AosGenericMethodObjPtr
AosGenericObjMgr::getGenericObj(
		AosRundata *rdata, 
		const OmnString &verb, 
		const OmnString &obj_type, 
		const int version)
{
	int ver = (version >= 0)?version:AosJimo::eDefaultVersion;

	OmnString key = verb;
	key << "_" << obj_type << "_" << ver;
	mLock->lock();
	itr_t itr = mObjects.find(key);
	if (itr == mObjects.end())
	{
		AosXmlTagPtr doc = getJimoDoc(rdata, verb, obj_type, version);
		aos_assert_rl(doc, mLock, NULL);
		AosGenericMethodObjPtr gen_obj = createGenericObj(rdata, 0, doc);
		aos_assert_rl(gen_obj, mLock, NULL);
		mObjects[key] = gen_obj;
		mLock->unlock();
		return gen_obj;
	}

	AosGenericMethodObjPtr obj = itr->second;
	mLock->unlock();
	return obj;
}

AosJimoPtr
AosGenericObjMgr::cloneJimo() const
{
	return OmnNew AosGenericObjMgr(*this);
}


AosXmlTagPtr
AosGenericObjMgr::getJimoDoc(
		const AosRundataPtr &rdata,
		const OmnString &verb,
		const OmnString &obj_type,
		const u32 ver)
{
#if 0
	OmnString objid = "";
	OmnString key = "";
	AosXmlTagPtr jimodoc = NULL;

	key << verb << "_" << obj_type << "_" << ver;

	if (mObjectsJimoObjid.find(key) == mObjectsJimoObjid.end())
	{
		AosSetErrorUser(rdata, __func__)
			<< "internal_error."
			<< key << " is undefined."
			<< enderr;

		return NULL;
	}

	objid = mObjectsJimoObjid[key];
	jimodoc = getDocByObjid(objid); 
	if (!jimodoc)
	{
		AosSetErrorUser(rdata, __func__)
			<< "internal_error." << enderr;
		return NULL;
	}
	return jimodoc;
#endif

	OmnString jimostr = "<jimo ";
	jimostr << AOSTAG_CURRENT_VERSION << "=\"" << "1" << "\" "  
		<< "zky_classname=\"" << "AosMethodCreateRandomData" << "\" "                 
		<< "zky_otype=\"" << AOSOTYPE_JIMO << "\" "             
		<< "zky_objid=\"_jimo_" << "AosMethodCreateRandomData" << "\">"               
		<< "<versions>"                                         
		<<      "<version_1>" << "libGenRandomData.so" << "</version_1>"     
		<< "</versions>"                                        
		<< "</jimo>";                                           

	AosXmlTagPtr doc = AosStr2Xml(rdata.getPtr(), jimostr);
	aos_assert_rr(doc, rdata, NULL);
	return doc;
}


/*
bool
AosGenericObjMgr::addObject(
		AosRundata *rdata, 
		const AosGenericObjPtr &object)
{
	OmnString objtype = object->getObjType();
	aos_assert_rr(objtype != "", rdata, false);

	int version = object->getVersion();
	OmnString key = objtype;
	key << "_" << version;
	mLock->lock();
	itr_t itr = mObjects.find(key);
	if (itr != mObjects.end())
	{
		mLock->unlock();
		AosSetErrorUser(rdata, "object_already_defined")
			<< "Object Type=" << objtype
			<< "Version=" << version << enderr;
		return false;
	}

	mObjects[key] = object;
	mLock->unlock();
	return true;
}
*/

