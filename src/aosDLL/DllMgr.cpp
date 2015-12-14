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
// 10/27/2011	Created by Linda Lin 
////////////////////////////////////////////////////////////////////////////
#include "aosDLL/DllMgr.h"

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "API/AosApi.h"
#include "aosDLL/JimoDocMgr.h"
#include "Jimo/Jimo.h"
#include "Porting/Sleep.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"


AosDllMgr::AosDllMgr()
:
mLock(OmnNew OmnMutex())
{
}


AosDllMgr::~AosDllMgr()
{
	lmapitr_t itr = mLib.begin();
	for(; itr!=mLib.end(); ++itr)
	{
		dlclose(itr->second);
		itr->second = NULL;
	}
}

bool
AosDllMgr::start()
{
	return true;
}

bool
AosDllMgr::stop()
{
	return true;
}


bool
AosDllMgr::config(const AosXmlTagPtr &config)
{
	return true;
}


void *
AosDllMgr::findLibPriv(
		AosRundata *rdata,
		const OmnString &libname)
{
	lmapitr_t itermap = mLib.find(libname);

	if (itermap != mLib.end())
	{
		return itermap->second;	
	}

	// Has not been loaded yet. Check whether it is an invalid library.
	if (isInvalidLibraryLocked(rdata, libname))
	{
		OmnAlarm << "Library not found: " << libname << enderr;
		return 0;
	}

	// Try to open it.
	void *dl_handle = dlopen(libname.data(), RTLD_LAZY);	
	if (!dl_handle)
	{
		unlink(libname.data());
		dl_handle = dlopen(libname.data(), RTLD_LAZY);	
	}
	
	if (!dl_handle)
	{
		// It is an invalid library
		mInvalidLibs[libname] = true;
		char *error = dlerror();
		OmnAlarm << "Library not found: " << libname << ". " << error << enderr;
		return 0;
	}

	mLib.insert(make_pair(libname, dl_handle));
	return dl_handle;
}


bool
AosDllMgr::isInvalidLibraryLocked(
		AosRundata *rdata, 
		const OmnString &libname)
{
	bmapitr_t itr = mInvalidLibs.find(libname);
	if (itr != mInvalidLibs.end()) return true;
	return false;
}


AosDllMgr::norm_func
AosDllMgr::findNormFunc(
		AosRundata *rdata,
		const OmnString &libname,
		const OmnString &method, 
		const int version)
{
	aos_assert_r(libname != "", 0);
	aos_assert_r(method != "", 0);

	mLock->lock();
	void *dl_handle = findLibPriv(rdata, libname);
	aos_assert_rl(dl_handle, mLock, 0);

	norm_func func = 0;
	char *error = 0;
	if (version == 0)
	{
		func = (norm_func)(dlsym(dl_handle, method.data()));
		error = dlerror();
	}
	else
	{
		int vv = version;
		OmnString key = getKey(method, vv);
		func = (norm_func)(dlsym(dl_handle, key.data()));
		error = dlerror();
	}

	if (error != NULL)
	{
		mLock->unlock();
		AosSetErrorU(rdata, "failed_retrieving_dll_func: ") << method 
			<< ":" << version << ". Error: " << error;
		return 0;
	}
	mLock->unlock();
	aos_assert_rr(func, rdata, 0);
	return func;
}


AosDllMgr::create_dllobj_func
AosDllMgr::findCreateDLLObjFunc(
		AosRundata *rdata,
		const OmnString &libname,
		const OmnString &method,
		const int version)
{
	aos_assert_r(libname != "", 0);
	aos_assert_r(method != "", 0);

	mLock->lock();
	void *dl_handle = findLibPriv(rdata, libname);
	aos_assert_rl(dl_handle, mLock, 0);

	create_dllobj_func func = 0;
	char *error = 0;
	OmnString key = getKey(method, version);
	func = (create_dllobj_func)(dlsym(dl_handle, key.data()));
	error = dlerror();

	if (error != NULL)
	{
		mLock->unlock();
		AosSetErrorU(rdata, "failed_retrieving_dll_func: ") << key 
			<< ". Error: " << error;
		return 0;
	}
	mLock->unlock();
	aos_assert_rr(func, rdata, 0);
	return func;
}


AosDllMgr::create_jimo_func
AosDllMgr::findCreateJimoFuncLocked(
		AosRundata *rdata,
		const OmnString &libname,
		const OmnString &method,
		const int version)
{
	aos_assert_rr(libname != "", rdata, 0);
	aos_assert_rr(method != "", rdata, 0);

	void *dl_handle = findLibPriv(rdata, libname);
	aos_assert_rr(dl_handle, rdata, 0);

	create_jimo_func func = 0;
	char *error = 0;
	OmnString key = getKey(method, version);
	func = (create_jimo_func)(dlsym(dl_handle, key.data()));
	error = dlerror();

	if (error != NULL)
	{
		AosSetErrorUser(rdata, "failed_retrieving_dll_func: ") << key 
			<< ". Error: " << error;
		return 0;
	}
	aos_assert_rr(func, rdata, 0);
	return func;
}


bool 	
AosDllMgr::callFunc(
		AosRundata *rdata,
		const OmnString &lib,
		const OmnString &method, 
		const int version)
{
	norm_func func = findNormFunc(rdata, lib, method, version);
	aos_assert_rr(func, rdata, false);
	return func(rdata);
}	


AosDLLObjPtr
AosDllMgr::createDLLObj(
		AosRundata *rdata,
		const AosXmlTagPtr &def, 
		const OmnString &libname,
		const OmnString &method,
		const int version)
{
	int vv = version;
	create_dllobj_func func = findCreateDLLObjFunc(rdata, libname, method, vv);
	aos_assert_rr(func, rdata, 0);
	AosRundataPtr rr(rdata, false);
	return (*func)(rr, def, libname, version);
}	


AosJimoPtr 
AosDllMgr::createJimo(
		AosRundata *rdata,
		const AosXmlTagPtr &worker, 
		const int version) 
{
	// This function create a jimo. The jimo is defined in 'worker'
	aos_assert_rr(worker, rdata, 0);
	if (worker->getAttrStr(AOSTAG_OTYPE, "") == AOSOTYPE_JIMO)
	{
		// The worker is itself a jimo. 
		return createJimo(rdata, worker, worker, version);
	}

	OmnString jimo_objid = worker->getAttrStr(AOSTAG_JIMO_OBJID);
	if (jimo_objid == "")
	{
		OmnString classname = worker->getAttrStr("classname");
		int ver = version;
		if (ver == -1)
		{
			ver = worker->getAttrInt("version", -1);
		}

		if (classname != "")
		{
			AosXmlTagPtr jimo_doc = AosJimoDocMgr::getJimoDocStatic(rdata, classname, ver);
			if (jimo_doc)
			{
				return createJimo(rdata, worker, jimo_doc, version);
			}
		}
		return 0;
	}

	AosXmlTagPtr jimo_doc = AosGetDocByObjid(jimo_objid, rdata);
	if (!jimo_doc)
	{
		AosSetErrorUser(rdata, "jimo_not_found")
			<< jimo_objid << ". " << AOSDICTERM("worker_is", rdata)
			<< worker->toString() << enderr;
		return 0;
	}

	return createJimo(rdata, worker, jimo_doc, version);
}


AosJimoPtr 
AosDllMgr::createJimo(
		AosRundata *rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc, 
		const int ver) 
{
	// This function creates a new jimo based on 'jimo_doc', which 
	// must have the following attributes:
	// 	<jimo AOSTAG_OBJID="xxx"
	// 		AOSTAG_LIBNAME="libJimoExample.so"
	// 		AOSTAG_CLASSNAME="AosJimoExample"
	// 		AOSTAG_OBJID="jimo_objid"
	// 		namespace="xxx"
	// 		name="xxx"
	// 		AOSTAG_VERSION="0.1,0.2" .../>
	//
	// Errors:
	// 1. If AOSTAG_CLASSNAME is empty, it is an error.
	// 2. If 'ver' is not negative, it uses this version. 
	//    Otherwise, it checks whether 'work_doc' is not null and if not null,
	//    whether it specified AOSTAG_JIMO_VERSION. If yes, it uses its version.
	//    (If its version is negative, it is ignored).
	//    Otherwise, it retrieves the version from 'jimo_doc'. 
	//    If no valid version is retrieved through the above, it is an error.
	// 3. 'jimo_doc' must have a subtag named "<tags...>". Otherwise, it 
	//    is an error.
	//    There must be a subtag under <tags ...> named:
	//    	AOSTAG_VERSIONS_PREFIX + "_" + version
	//    	"version_" + version
	//    If the tag is not defined, it is an error. 
	//    The subtag must be in the form:
	//    	<tagname ...>libname</tagname>
	// 4. AOSTAG_OBJID is not empty. In this case, the Jimo is identified
	//    by jimo objid. 
	//
	//    In addition, if namespace and name are not empty. The jimo is identified
	//    by namespace + "." + name.
	//
	aos_assert_rr(jimo_doc, rdata, 0);

	OmnString classname = jimo_doc->getAttrStr(AOSTAG_CLASSNAME);
	if (classname == "")
	{
		AosSetErrorUser(rdata, "missing_classname") 
			<< ". Jimo: " << jimo_doc->toString() << enderr;
		return 0;
	}

	OmnString method = AOSTAG_CREATEJIMO_FUNC_PREFIX;
	method << "_" << classname;

	// Check whethr 'worker_doc' wants to use a specific version of the jimo. 
	// 	<worker AOSTAG_JIMO_VERSION="xxx" .../>
	// If the version is specified, 
	// 	<jimo AOSTAG_CURRENT_VERSION="xxx">
	// 		<versions>
	// 			<version_number version="xxx">libname</version>
	// 			<version_number version="xxx">libname</version>
	// 			...
	// 		</versions>
	// 	</jimo>
	int version = ver;
	if (version < 1)
	{
		if (worker_doc)
		{
			version = worker_doc->getAttrInt(AOSTAG_JIMO_VERSION, version);
		}
	}

	if ((int)version < 0)
	{
		// Version is not specified, use the current version.
		version = jimo_doc->getAttrInt(AOSTAG_CURRENT_VERSION, -1);
		if ((int)version < 0)
		{
			AosSetErrorUser(rdata, "version_current_version")
				<< ": " << AOSDICTERM("jimo", rdata) << ": " << jimo_doc->toString() << enderr;
			return 0;
		}
	}

	AosXmlTagPtr tags = jimo_doc->getFirstChild(AOSTAG_VERSIONS);
	if (!tags)
	{
		AosSetErrorUser(rdata, "missing_versions_tag")
			<< ". Jimo: " << jimo_doc->toString() << enderr;
		return 0;
	}
	
	//medfied By Andy zhang 2013/07/26
	OmnString tagname;
	tagname << AOSTAG_VERSIONS_PREFIX << "_" << version;

	AosXmlTagPtr tag = tags->getFirstChild(tagname);
	if (!tag)
	{
		// Chen Ding, 2014/01/12
		// We will consider both the following are valid:
		// 	<ver_0>...</ver_0>
		// 	<version_0>...</version_0>
		tagname = "version_";
		tagname << version;
		tag = tags->getFirstChild(tagname);
		if (!tag)
		{
			AosSetErrorUser(rdata, "version_not_defined")
				<< AOSDICTERM("version", rdata) << ": " << version
				<< ". " << AOSDICTERM("jimo", rdata) << ": " << jimo_doc->toString() << enderr;
			return 0;
		}
	}

	OmnString libname = tag->getNodeText();
	OmnString objid = jimo_doc->getAttrStr(AOSTAG_OBJID);
	OmnString jimo_namespace = jimo_doc->getAttrStr("namespace");
	OmnString jimo_name = jimo_doc->getAttrStr("name");
	
	// Chen Ding, 2015/09/11, JIMODB-741
	if (libname == "")
	{
		AosSetErrorUser(rdata, "missing_lib_name") 
			<< ". Jimo: " << jimo_doc->toString() << enderr;
		return 0;
	}

	mLock->lock();
	AosJimoPtr jimo;
	// Chen Ding, 2015/09/11, JIMODB-741
	if (objid != "")
	{
		jimo = findJimoLocked(rdata, objid, version);
	}
	else if (jimo_namespace != "" && jimo_name != "")
	{
		jimo = findJimoLocked(rdata, jimo_namespace, jimo_name, version);
	}

	if (jimo)
	{
		mLock->unlock();
		jimo = jimo->cloneJimo();

		if (!jimo->config(rdata, worker_doc, jimo_doc)) 
		{
			AosSetError(rdata, "dllmgr_failed_config_jimo") 
				<< rdata->getErrmsg() << enderr;
			return 0;
		}
		return jimo;
	}

	create_jimo_func func = findCreateJimoFuncLocked(rdata, libname, method, version);
	if (!func)
	{
		AosSetErrorUser(rdata, "jimo_creation_func_not_defined") 
			<< libname << ":" << method << ":" << version
			<< ". Jimo: " << jimo_doc->toString() << enderr;
		mLock->unlock();
		return 0;
	}

	mLock->unlock();
	AosRundataPtr rr(rdata, false);
	jimo = (*func)(rr, version);
	if (!jimo)
	{
		AosSetErrorUser(rdata, "internal_error")
			<< AOSDICTERM("version", rdata) << ": " << version
			<< ". " << AOSDICTERM("jimo", rdata) << ": " << jimo_doc->toString() << enderr;
		return 0;
	}

	mLock->lock();
	addJimoLocked(rdata, jimo, objid, version);
	if (jimo_namespace != "" && jimo_name != "") 
	{
		addJimoLocked(rdata, jimo, jimo_namespace, jimo_name, version);
	}
	mLock->unlock();

	try
	{
		jimo = jimo->cloneJimo();
	}

	catch (...) 
	{
		OmnAlarm << "Failed To Create Jimo. Jimo Doc is " << jimo_doc->toString() << enderr;
		return NULL;
	}

	aos_assert_r(jimo, 0);
	if (!jimo->config(rdata, worker_doc, jimo_doc)) return 0;
	return jimo;
}


AosJimoPtr 
AosDllMgr::createJimoByClassname(
		AosRundata *rdata,
		const OmnString &classname, 
		const int version)
{
	AosXmlTagPtr jimo_doc = AosJimoDocMgr::getJimoDocStatic(rdata, classname, version);
	if (jimo_doc) 
	{
		return createJimo(rdata, 0, jimo_doc, version);
		//return createJimo(rdata, 0, jimo_doc);
	}

	jimo_doc = AosGetDocByKey(rdata, AOSCTNR_JIMO_CONTAINER, 
			AOSTAG_CLASSNAME, classname);
	if (!jimo_doc)
	{
		AosSetErrorUser(rdata, "dllmgr_jimodoc_not_found") << classname << enderr;
		return 0;
	}

	if (jimo_doc->getAttrStr(AOSTAG_OTYPE) != AOSOTYPE_JIMO)
	{
		AosSetErrorUser(rdata, "dllmgr_jimodoc_not_jimo") << jimo_doc << enderr;
		return 0;
	}

	return createJimo(rdata, 0, jimo_doc, version);
}


bool
AosDllMgr::addJimoLocked(
		AosRundata *rdata, 
		const AosJimoPtr &jimo, 
		const OmnString &objid, 
		const int version)
{
	aos_assert_rr(objid != "", rdata, false);
	aos_assert_rr(jimo, rdata, false);
	OmnString key = objid;
	key << "_" << version;
	mJimoMap[key] = jimo;
	return true;
}


AosJimoPtr 
AosDllMgr::findJimoLocked(
		AosRundata *rdata, 
		const OmnString &objid, 
		const int version)
{
	aos_assert_rr(objid != "", rdata, 0);
	OmnString key = objid;
	key << "_" << version;
	jmapitr_t itr = mJimoMap.find(key);
	if (itr == mJimoMap.end()) return 0;
	return itr->second;
}


AosJimoPtr 
AosDllMgr::createJimo(
		AosRundata *rdata,
		const AosXmlTagPtr &worker,
		const OmnString &jimo_objid)
{
	AosXmlTagPtr jimo_doc = AosGetDocByObjid(jimo_objid, rdata);
	if (!jimo_doc)
	{
		AosSetErrorUser(rdata, "jimo_not_found")
			<< jimo_objid << ". " << AOSDICTERM("worker_is", rdata)
			<< worker->toString() << enderr;
		return 0;
	}

	return createJimo(rdata, worker, jimo_doc);
}


AosJimoPtr
AosDllMgr::createJimoByName(
		AosRundata *rdata, 
		const OmnString &container, 
		const OmnString &key,
		const OmnString &name, 
		const int version)
{
	AosXmlTagPtr jimo_doc = AosGetDocByKey(rdata, container, key, name);
	if (jimo_doc) 
	{
		return createJimo(rdata, 0, jimo_doc, version);
	}
		
	jimo_doc = AosJimoDocMgr::getJimoDocStatic(rdata, name, version);
	if (jimo_doc)
	{
		return createJimo(rdata, 0, jimo_doc, version);
	}

	return 0;
}


// Chen Ding, 2015/09/11, JIMODB-741
AosJimoPtr 
AosDllMgr::findJimoLocked(
		AosRundata *rdata, 
		const OmnString &jimo_namespace,
		const OmnString &jimo_name, 
		const int version)
{
	OmnString nn = jimo_namespace;
	nn << "." << jimo_name << "_" << version;
	nn.toLower();
	jmapitr_t itr = mJimoMap.find(nn);
	return itr->second;
}


// Chen Ding, 2015/09/11, JIMODB-741
bool
AosDllMgr::addJimoLocked(
		AosRundata *rdata, 
		const AosJimoPtr &jimo, 
		const OmnString &jimo_namespace, 
		const OmnString &jimo_name, 
		const int version)
{
	OmnString key = jimo_namespace;
	key << "." << jimo_name << "_" << version;
	key.toLower();
	mJimoMap[key] = jimo;
	return true;
}


AosJimoPtr 
AosDllMgr::createJimoByName(
		AosRundata *rdata, 
		const OmnString &jimo_namespace,
		const OmnString &jimo_name, 
		const int version)
{
	OmnString nn = jimo_namespace;
	nn << "." << jimo_name << "_" << version;
	nn.toLower();

	mLock->lock();
	jmapitr_t itr = mJimoMap.find(nn);
	if (itr != mJimoMap.end())
	{
		AosJimoPtr jimo = itr->second;
		if (jimo) 
		{
			mLock->unlock();
			return jimo->cloneJimo();
		}
	}
	mLock->unlock();

	nn = jimo_namespace;
	nn << "." << jimo_name;
	nn.toLower();
	AosXmlTagPtr jimo_doc = AosJimoDocMgr::getJimoDocStatic(rdata, nn, version);
	if (jimo_doc) 
	{
		return createJimo(rdata, 0, jimo_doc, version);
	}

	AosLogError(rdata, false, "jimo_not_found") << enderr;
	return 0;
}

