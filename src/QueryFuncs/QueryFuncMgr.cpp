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
// 2014/01/30 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "QueryFuncs/QueryFuncMgr.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "QueryFuncs/Ptrs.h"
#include "XmlUtil/XmlTag.h"

AosQueryFuncMgr::map_t AosQueryFuncMgr::smJimoMap;
static OmnMutex sgLock;
static bool sgInited = false;

AosQueryFuncMgr::AosQueryFuncMgr()
:
AosQueryFunc("query_func_mgr", 1)
{
}


AosQueryFuncMgr::~AosQueryFuncMgr()
{
}


bool 
AosQueryFuncMgr::init(const AosRundataPtr &rdata)
{
	sgLock.lock();
	if (sgInited)
	{
		sgLock.unlock();
		return true;
	}

	bool rslt = createDftJimoDocs(rdata);
	sgInited = true;
	sgLock.unlock();
	return rslt;
}


bool
AosQueryFuncMgr::pickJimo(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &def,
		AosValueRslt &value)
{
	if (!sgInited) init(rdata);

	OmnString func_name = def->getAttrStr("func_name");
	if (func_name == "")
	{
		AosSetError(rdata, "queryfuncobj_missing_funcname") << def << enderr;
		return false;
	}

	AosJimoPtr jimo = getJimo(rdata, func_name);
	if (!jimo)
	{
		AosSetEntityError(rdata, "queryfuncmgr_jimo_not_found", "QueryFunc", "")
			<< func_name << enderr;
		return false;
	}

	AosQueryFuncPtr func = dynamic_cast<AosQueryFunc*>(jimo.getPtr());
	aos_assert_rr(func, rdata, false);
	return func->eval(rdata, def, value);
}


AosQueryFuncObjPtr 
AosQueryFuncMgr::getJimo(
		const AosRundataPtr &rdata, 
		const OmnString &func_name)
{
	sgLock.lock();
	itr_t itr = smJimoMap.find(func_name);
	if (itr != smJimoMap.end())
	{
		AosQueryFuncPtr val = itr->second;
		sgLock.unlock();
		return val;
	}
	sgLock.unlock();

	OmnString objid = AosObjid::composeQueryFuncJimoObjid(func_name);

	AosXmlTagPtr doc = AosGetDocByObjid(objid, rdata);
	if (!doc)
	{
		AosSetError(rdata, "queryfuncmgr_jimo_not_defined") << enderr;
		return 0;
	}

	AosJimoPtr jimo = AosCreateJimo(rdata, doc);
	if (!jimo)
	{
		AosSetError(rdata, "queryfuncmgr_failed_creating_jimo")
			<< doc << enderr;
		return 0;
	}

	if (jimo->getJimoType() != AosJimoType::eQueryFunc)
	{
		AosSetError(rdata, "queryfuncmgr_internal_error") 
			<< jimo->getJimoType() << enderr;
		return 0;
	}

	AosQueryFuncPtr val = dynamic_cast<AosQueryFunc*>(jimo.getPtr());
	aos_assert_rr(val, rdata, 0);

	sgLock.lock();
	smJimoMap[func_name] = val;
	sgLock.unlock();
	return val;
}


bool
AosQueryFuncMgr::createDftJimoDocs(const AosRundataPtr &rdata)
{
	OmnString appname = rdata->setAppname(AOSAPPNAME_SYSTEM);
	u64 userid = rdata->setUserid(AOSDOCID_SYSROOT);
	OmnString cid = rdata->setCid(AOSCLOUDID_SYSROOT);

	createDftJimoDoc(rdata, "AosEpochTime", "EpochTime");

	rdata->setAppname(appname);
	rdata->setUserid(userid);
	rdata->setCid(cid);

	return true;
}


bool
AosQueryFuncMgr::createDftJimoDoc(
		const AosRundataPtr &rdata, 
		const OmnString &classname, 
		const OmnString &func_name)
{
	OmnString objid = AosObjid::composeQueryFuncJimoObjid(func_name);
	OmnString jimostr = "<jimo ";
	jimostr << "current_version=\"1\" "
		<< "zky_classname=\"" << classname << "\" "
		<< "zky_objid=\"" << objid << "\">"
		<< "<versions>"
		<< 		"<version_1>libIndexMgrJimos.so</version_1>"
		<< "</versions>"
		<< "</jimo>";

	AosXmlTagPtr doc = AosStr2Xml(rdata.getPtrNoLock(), jimostr AosMemoryCheckerArgs);
	aos_assert_rr(doc, rdata, false);
	aos_assert_rr(doc->getAttrStr(AOSTAG_OBJID) == objid, rdata, false);
	return true;
}

