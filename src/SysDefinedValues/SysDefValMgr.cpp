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
// 2013/11/01 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SysDefinedValues/SysDefValMgr.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SysDefinedValues/Ptrs.h"
#include "XmlUtil/XmlTag.h"

AosSysDefValMgr::map_t AosSysDefValMgr::smJimoMap;
static OmnMutex sgLock;
static bool sgInited = false;

AosSysDefValMgr::AosSysDefValMgr()
:
AosSysDefVal("sys_def_val_mgr", 0)
{
}


AosSysDefValMgr::~AosSysDefValMgr()
{
}


bool 
AosSysDefValMgr::init(const AosRundataPtr &rdata)
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
AosSysDefValMgr::resolve(
		const AosRundataPtr &rdata, 
		const OmnString &val_name, 
		const OmnString &dft, 
		OmnString &value)
{
	if (!sgInited) init(rdata);

	aos_assert_rr(val_name != "", rdata, false);

	// SysDefVal can be in the following format:
	// 	1. _dddd::dddd
	// 	2. _dddd
	vector<OmnString> parts;
	int nn = AosSplitStr(val_name, "::", parts, 3);

	if (nn > 1)
	{
		// It is the form "_dddd::dddd". 
		if (nn != 2)
		{
			// This is not supported
			AosSetEntityError(rdata, "sysdefvalmgr_invalid_sysdefval", "SysDefVal", "")
				<< val_name << enderr;
			return false;
		}

		AosJimoPtr jimo = getJimo(rdata, parts[0]);
		if (!jimo)
		{
			AosSetEntityError(rdata, "sysdefvalmgr_jimo_not_found", "SysDefVal", "")
				<< val_name << enderr;
			return false;
		}

		AosSysDefValPtr sysval = dynamic_cast<AosSysDefVal*>(jimo.getPtr());
		aos_assert_rr(sysval, rdata, false);
		return sysval->resolveWithMember(rdata, parts[0], parts[1], dft, value);
	}

	AosJimoPtr jimo = getJimo(rdata, val_name);
	if (!jimo)
	{
		AosSetEntityError(rdata, "sysdefvalmgr_jimo_not_found", "SysDefVal", "")
			<< val_name << enderr;
		return false;
	}

	AosSysDefValPtr sysval = dynamic_cast<AosSysDefVal*>(jimo.getPtr());
	aos_assert_rr(sysval, rdata, false);
	return sysval->resolve(rdata, val_name, dft, value);
}


AosSysDefValObjPtr 
AosSysDefValMgr::getJimo(
		const AosRundataPtr &rdata, 
		const OmnString &val_name)
{
	sgLock.lock();
	itr_t itr = smJimoMap.find(val_name);
	if (itr != smJimoMap.end())
	{
		AosSysDefValObjPtr val = itr->second;
		sgLock.unlock();
		return val;
	}
	sgLock.unlock();

	OmnString objid = AosObjid::composeSysDefValJimoObjid(val_name);

	AosXmlTagPtr doc = AosGetDocByObjid(objid, rdata);
	if (!doc)
	{
		AosSetError(rdata, "sysdefval_jimo_not_defined") << enderr;
		return 0;
	}

	AosJimoPtr jimo = AosCreateJimo(rdata, doc);
	if (!jimo)
	{
		AosSetError(rdata, "sysdefval_failed_creating_jimo")
			<< doc << enderr;
		return 0;
	}

	if (jimo->getJimoType() != AosJimoType::eSysDefinedValue)
	{
		AosSetError(rdata, "sysdefval_internal_error") 
			<< jimo->getJimoType() << enderr;
		return 0;
	}

	AosSysDefValObjPtr val = dynamic_cast<AosSysDefValObj*>(jimo.getPtr());
	aos_assert_rr(val, rdata, 0);

	sgLock.lock();
	smJimoMap[val_name] = val;
	sgLock.unlock();
	return val;
}


bool
AosSysDefValMgr::createDftJimoDocs(const AosRundataPtr &rdata)
{
	OmnString appname = rdata->setAppname(AOSAPPNAME_SYSTEM);
	u64 userid = rdata->setUserid(AOSDOCID_SYSROOT);
	OmnString cid = rdata->setCid(AOSCLOUDID_SYSROOT);

	createDftJimoDoc(rdata, "AosSysDefValLoginInfo", "_logininfo::");

	rdata->setAppname(appname);
	rdata->setUserid(userid);
	rdata->setCid(cid);

	return true;
}


bool
AosSysDefValMgr::createDftJimoDoc(
		const AosRundataPtr &rdata, 
		const OmnString &classname, 
		const OmnString &val_name)
{
	OmnString objid = AosObjid::composeSysDefValJimoObjid(val_name);
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

