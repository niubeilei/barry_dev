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
#include "QueryVars/QueryVarMgr.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "QueryVars/Ptrs.h"
#include "XmlUtil/XmlTag.h"

AosQueryVarMgr::map_t AosQueryVarMgr::smJimoMap;
static OmnMutex sgLock;
static bool sgInited = false;

AosQueryVarMgr::AosQueryVarMgr()
:
AosQueryVar("query_var_mgr", 1)
{
}


AosQueryVarMgr::~AosQueryVarMgr()
{
}


bool 
AosQueryVarMgr::init(const AosRundataPtr &rdata)
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
AosQueryVarMgr::pickJimo(  
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &def, 
		AosValueRslt &value)
{
	// 'def' is defined as:
	// 	<var var_name="ddd"
	// 		var_type="literal|var|func"
	// 		default_policy="xxx">
	// 		<defaul>xxx</default>
	// 	</var>
	if (!sgInited) init(rdata);

	OmnString var_name = def->getAttrStr("var_name");
	if (var_name == "")
	{
		AosSetError(rdata, "queryvarmgr_missing_var_name") << def << enderr;
		return false;
	}

	AosJimoPtr jimo = getJimo(rdata, var_name);
	if (!jimo)
	{
		AosSetEntityError(rdata, "queryvarmgr_jimo_not_found", "QueryVar", "")
			<< var_name << enderr;
		return false;
	}

	AosQueryVarPtr var  = dynamic_cast<AosQueryVar*>(jimo.getPtr());
	aos_assert_rr(var, rdata, false);
	return var->eval(rdata, def, value);
}


AosQueryVarObjPtr 
AosQueryVarMgr::getJimo(
		const AosRundataPtr &rdata, 
		const OmnString &var_name)
{
	sgLock.lock();
	itr_t itr = smJimoMap.find(var_name);
	if (itr != smJimoMap.end())
	{
		AosQueryVarPtr val = itr->second;
		sgLock.unlock();
		return val;
	}
	sgLock.unlock();

	OmnString objid = AosObjid::composeQueryVarJimoObjid(var_name);

	AosXmlTagPtr doc = AosGetDocByObjid(objid, rdata);
	if (!doc)
	{
		AosSetError(rdata, "queryvarmgr_jimo_not_defined") << enderr;
		return 0;
	}

	AosJimoPtr jimo = AosCreateJimo(rdata, doc);
	if (!jimo)
	{
		AosSetError(rdata, "queryvarmgr_failed_creating_jimo")
			<< doc << enderr;
		return 0;
	}

	if (jimo->getJimoType() != AosJimoType::eQueryVar)
	{
		AosSetError(rdata, "queryvarmgr_internal_error") 
			<< jimo->getJimoType() << enderr;
		return 0;
	}

	AosQueryVarPtr val = dynamic_cast<AosQueryVar*>(jimo.getPtr());
	aos_assert_rr(val, rdata, 0);

	sgLock.lock();
	smJimoMap[var_name] = val;
	sgLock.unlock();
	return val;
}


bool
AosQueryVarMgr::createDftJimoDocs(const AosRundataPtr &rdata)
{
	OmnString appname = rdata->setAppname(AOSAPPNAME_SYSTEM);
	u64 userid = rdata->setUserid(AOSDOCID_SYSROOT);
	OmnString cid = rdata->setCid(AOSCLOUDID_SYSROOT);

	createDftJimoDoc(rdata, "AosQueryVarSysDate", "_SysDate");
	createDftJimoDoc(rdata, "AosQueryVarClipvar", "_Clipvar");
	createDftJimoDoc(rdata, "AosQueryVarCloudid", "_Cloudid");
	createDftJimoDoc(rdata, "AosQueryLoginInfo", "_LoginInfo");

	rdata->setAppname(appname);
	rdata->setUserid(userid);
	rdata->setCid(cid);

	return true;
}


bool
AosQueryVarMgr::createDftJimoDoc(
		const AosRundataPtr &rdata, 
		const OmnString &classname, 
		const OmnString &var_name)
{
	OmnString objid = AosObjid::composeQueryVarJimoObjid(var_name);
	OmnString jimostr = "<jimo ";
	jimostr << "current_version=\"1\" "
		<< "zky_classname=\"" << classname << "\" "
		<< "zky_objid=\"" << objid << "\">"
		<< "<versions>"
		<< 		"<version_1>libIndexMgrJimos.so</version_1>"
		<< "</versions>"
		<< "</jimo>";

	AosXmlTagPtr doc = AosStr2Xml(rdata, jimostr AosMemoryCheckerArgs);
	aos_assert_rr(doc, rdata, false);
	aos_assert_rr(doc->getAttrStr(AOSTAG_OBJID) == objid, rdata, false);
	return true;
}


AosJimoPtr
AosQueryVarMgr::cloneJimo() const
{
	return OmnNew AosQueryVarMgr(*this);
}


bool 
AosQueryVarMgr::eval(  
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &def, 
		AosValueRslt &value)
{
	OmnShouldNeverComeHere;
	return false;
}

