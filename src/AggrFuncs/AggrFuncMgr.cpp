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
#include "AggrFuncs/AggrFuncMgr.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "QueryFuncs/Ptrs.h"
#include "XmlUtil/XmlTag.h"

#include "Jimo/Jimo.h"
#include "Jimo/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/DataTypes.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosAggrFuncMgr_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosAggrFuncMgr(version);
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



AosAggrFuncMgr::map_t AosAggrFuncMgr::smJimoMap;
static OmnMutex sgLock;
static bool sgInited = false;

AosAggrFuncMgr::AosAggrFuncMgr(const int version)
:
AosAggrFuncMgrObj(version)
{
}


AosAggrFuncMgr::~AosAggrFuncMgr()
{
}


bool 
AosAggrFuncMgr::init(const AosRundataPtr &rdata)
{
	sgLock.lock();
	if (sgInited)
	{
		sgLock.unlock();
		return true;
	}

	bool rslt = createDftJimos(rdata);
	sgInited = true;
	sgLock.unlock();
	return rslt;
}


AosJimoPtr 
AosAggrFuncMgr::cloneJimo() const
{
	return OmnNew AosAggrFuncMgr(*this);
}


bool
AosAggrFuncMgr::createDftJimos(const AosRundataPtr &rdata)
{
	OmnString appname = rdata->setAppname(AOSAPPNAME_SYSTEM);
	u64 userid = rdata->setUserid(AOSDOCID_SYSROOT);
	OmnString cid = rdata->setCid(AOSCLOUDID_SYSROOT);

	createDftJimo(rdata, "AosAggrFuncSum", "sum");
	createDftJimo(rdata, "AosAggrFuncCount", "count");
	createDftJimo(rdata, "AosAggrFuncMax", "max");
	createDftJimo(rdata, "AosAggrFuncMin", "min");
	createDftJimo(rdata, "AosAggrFuncDistCount", "dist_count");
	//createDftJimo(rdata, "AosAggrFuncObjDistCountQuery", "dist_count_query");
	//createDftJimo(rdata, "AosAggrFuncObjDistCountCreate", "dist_count_create");

	rdata->setAppname(appname);
	rdata->setUserid(userid);
	rdata->setCid(cid);

	return true;
}

	
bool
AosAggrFuncMgr::createDftJimo(
		const AosRundataPtr &rdata, 
		const OmnString &classname, 
		const OmnString &func_name)
{
	AosXmlTagPtr jimo_doc = createDftJimoDoc(rdata, classname, func_name);
	aos_assert_r(jimo_doc, false);
	
	AosJimoPtr jimo = AosCreateJimo(rdata, jimo_doc);
	if (!jimo)
	{
		AosSetError(rdata, "AggrFuncMgr_failed_creating_jimo")
			<< jimo_doc << enderr;
		return 0;
	}
	
	AosAggrFuncObjPtr val = dynamic_cast<AosAggrFuncObj*>(jimo.getPtr());
	aos_assert_rr(val, rdata, 0);

	smJimoMap[func_name] = val;
	return true;
}


AosXmlTagPtr
AosAggrFuncMgr::createDftJimoDoc(
		const AosRundataPtr &rdata, 
		const OmnString &classname, 
		const OmnString &func_name)
{
	OmnString objid = AosObjid::composeAggrFuncJimoObjid(func_name);
	OmnString jimostr = "<jimo ";
	jimostr << "current_version=\"1\" "
		<< AOSTAG_OTYPE << "=\"" << AOSOTYPE_JIMO << "\" " 
		<< "jimo_type=\"jimo_AggrFuncObj\" " 
		<< AOSTAG_OBJID << "=\"" << objid << "\" " 
		<< "zky_classname=\"" << classname << "\" "
		<< AOSTAG_CTNR_PUBLIC << "=\"true\" "
		<< AOSTAG_PUBLIC_DOC << "=\"true\" "
		<< ">"
		<< "<versions>"
		<< 		"<version_1>libAggrFuncJimos.so</version_1>"
		<< "</versions>"
		<< "</jimo>";

	AosXmlTagPtr doc = AosStr2Xml(rdata.getPtrNoLock(), jimostr AosMemoryCheckerArgs);
	aos_assert_rr(doc, rdata, 0);
	aos_assert_rr(doc->getAttrStr(AOSTAG_OBJID) == objid, rdata, 0);
	
	return doc;
}


AosAggrFuncObjPtr
AosAggrFuncMgr::pickAggrFunc(
		const AosRundataPtr &rdata,
		const OmnString &func_name)
{
	if (!sgInited) init(rdata);
	
	if (func_name == "")
	{
		AosSetError(rdata, "AggrFuncObj_missing_funcname") << func_name << enderr;
		return 0;
	}

	OmnString lower_func_name = func_name;
	lower_func_name.toLower();
	AosAggrFuncObjPtr func = getJimo(rdata, lower_func_name);
	if (!func)
	{
		AosSetEntityError(rdata, "AggrFuncMgr_jimo_not_found", "AggrFuncObj", "")
			<< func_name << enderr;
		return 0;
	}
	return func;	
}


bool
AosAggrFuncMgr::isFuncValid(const AosRundataPtr &rdata, const OmnString &func_name)
{
	if (!sgInited) init(rdata);

	if (func_name == "")
	{
		AosSetError(rdata, "AggrFuncObj_missing_funcname") << func_name << enderr;
		return false;
	}

	AosAggrFuncObjPtr func = getJimo(rdata, func_name);
	if (!func)	return false;
	
	return true;
}




AosAggrFuncObjPtr 
AosAggrFuncMgr::getJimo(
		const AosRundataPtr &rdata, 
		const OmnString &func_name)
{
	sgLock.lock();
	itr_t itr = smJimoMap.find(func_name);
	if (itr != smJimoMap.end())
	{
		AosAggrFuncObjPtr val = itr->second;
		sgLock.unlock();
		return val;
	}
	sgLock.unlock();
	
	return 0;
}


