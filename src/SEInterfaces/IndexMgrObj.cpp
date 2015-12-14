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
// 2014/01/29	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/IndexMgrObj.h"

#include "API/AosApi.h"


AosIndexMgrObjPtr AosIndexMgrObj::smIndexMgrObj;

static AosXmlTagPtr			sgJimoDoc;
static OmnMutex				sgLock;


AosIndexMgrObj::AosIndexMgrObj(const int version)
:
AosJimo(AosJimoType::eIndexMgr, version)
{
	OmnScreen << "JimoType: " << mJimoType << endl;
}


static bool AosCreateIndexMgrJimoDoc(AosRundata *rdata)
{
	if (sgJimoDoc) return true;
	OmnString jimostr = "<jimo ";
	jimostr << AOSTAG_CURRENT_VERSION << "=\"1\" "
		<< "zky_classname=\"AosIndexMgr\" "
		<< "zky_otype=\"" << AOSOTYPE_JIMO << "\" "
		<< "zky_objid=\"index_mgr_jimo\">"
		<< "<versions>"
		<< 		"<version_1>libIndexMgrJimos.so</version_1>"
		<< "</versions>"
		<< "</jimo>";

	sgJimoDoc = AosStr2Xml(rdata, jimostr AosMemoryCheckerArgs);
	aos_assert_r(sgJimoDoc, 0);
	return sgJimoDoc;
}


AosIndexMgrObjPtr 
AosIndexMgrObj::getIndexMgr(AosRundata *rdata)
{
	if (smIndexMgrObj) return smIndexMgrObj;

	sgLock.lock();
	if (smIndexMgrObj)
	{
		sgLock.unlock();
		return smIndexMgrObj;
	}

	if (!sgJimoDoc)
	{
		if (!AosCreateIndexMgrJimoDoc(rdata))
		{
			sgLock.unlock();
			AosSetError(rdata, "indexmgrobj_internal_error") << enderr;
			return 0;
		}

		if (!sgJimoDoc)
		{
			AosSetError(rdata, "indexmgrobj_internal_error") << enderr;
			sgLock.unlock();
			return 0;
		}
	}

	AosJimoPtr jimo = AosCreateJimo(rdata, sgJimoDoc);
	if (!jimo)
	{
		AosSetError(rdata, "indexmgrobj_internal_error") << enderr;
		sgLock.unlock();
		return 0;
	}

	if (jimo->getJimoType() != AosJimoType::eIndexMgr)
	{
		AosSetErrorUser(rdata, "indexmgrobj_invalid_jimo") << enderr;
		sgLock.unlock();
		return 0;
	}

	smIndexMgrObj = dynamic_cast<AosIndexMgrObj*>(jimo.getPtr());
	sgLock.unlock();
	if (!smIndexMgrObj)
	{
		AosSetError(rdata, "internal_error") << enderr;
		return 0;
	}

	return smIndexMgrObj;
}


bool 
AosIndexMgrObj::getIILNamesStatic(
		AosRundata *rdata, 
		const AosQueryReqObjPtr &query_req, 
		const OmnString &iilname, 
		vector<OmnString> &iilnames)
{
	if (!smIndexMgrObj) getIndexMgr(rdata);
	aos_assert_rr(smIndexMgrObj, rdata, false);
	return smIndexMgrObj->getIILNames(rdata, query_req, iilname, iilnames);
}


bool 
AosIndexMgrObj::isParalIILStatic(
		AosRundata *rdata, 
		const OmnString &iilname)
{
	if (!smIndexMgrObj) getIndexMgr(rdata);
	aos_assert_rr(smIndexMgrObj, rdata, false);

	return smIndexMgrObj->isParalIIL(rdata, iilname);
}


bool 
AosIndexMgrObj::createIndexEntryStatic(
		AosRundata *rdata, 
		const AosXmlTagPtr &def)
{
	if (!smIndexMgrObj) getIndexMgr(rdata);
	aos_assert_rr(smIndexMgrObj, rdata, false);

	return smIndexMgrObj->createEntry(rdata, def);
}


AosXmlTagPtr 
AosIndexMgrObj::getIndexDefDocStatic(
		AosRundata *rdata, 
		const OmnString &iilname)
{
	if (!smIndexMgrObj) getIndexMgr(rdata);
	aos_assert_rr(smIndexMgrObj, rdata, 0);
	return smIndexMgrObj->getIndexDefDoc(rdata, iilname);
}


AosXmlTagPtr 
AosIndexMgrObj::getIndexDefDocStatic(
		AosRundata *rdata, 
		const OmnString &table_name, 
		const OmnString &field_name)
{
	if (!smIndexMgrObj) getIndexMgr(rdata);
	aos_assert_rr(smIndexMgrObj, rdata, 0);
	return smIndexMgrObj->getIndexDefDoc(rdata, table_name, field_name);
}


bool
AosIndexMgrObj::resolveIndexStatic(
		AosRundata *rdata, 
		const OmnString &table_name, 
		const OmnString &field_name, 
		OmnString &iilname)
{
	if (!smIndexMgrObj) getIndexMgr(rdata);
	aos_assert_rr(smIndexMgrObj, rdata, 0);
	return smIndexMgrObj->resolveIndex(rdata, table_name, field_name, iilname);
}


bool 
AosIndexMgrObj::isNormalIILStatic(
		AosRundata *rdata, 
		const OmnString &iilname)
{
	if (!smIndexMgrObj) getIndexMgr(rdata);
	aos_assert_rr(smIndexMgrObj, rdata, 0);
	return smIndexMgrObj->isNormalIIL(rdata, iilname);
}

bool 
AosIndexMgrObj::convertIILNameStatic(
		AosRundata *rdata, 
		const int epoch_day, 
		const OmnString &iilname,
		bool &converted,
		OmnString &new_iilname)
{
	if (!smIndexMgrObj) getIndexMgr(rdata);
	aos_assert_rr(smIndexMgrObj, rdata, 0);
	return smIndexMgrObj->convertIILName(rdata, epoch_day, iilname, converted, new_iilname);
}


bool 
AosIndexMgrObj::createEntryStatic(
		AosRundata *rdata, 
		const OmnString iilname, 
		const OmnString &table_name, 
		const OmnString &field_name, 
		const OmnString &index_type)
{
	if (!smIndexMgrObj) getIndexMgr(rdata);
	aos_assert_rr(smIndexMgrObj, rdata, 0);
	return smIndexMgrObj->createEntry(rdata, iilname, table_name, 
			field_name, index_type);
}

bool 
AosIndexMgrObj::createEntryStatic(
		AosRundata *rdata, 
		const AosXmlTagPtr &def)
{
	if (!smIndexMgrObj) getIndexMgr(rdata);
	aos_assert_rr(smIndexMgrObj, rdata, 0);
	return smIndexMgrObj->createEntry(rdata, def);
}

bool 
AosIndexMgrObj::modifyEntryStatic(
		AosRundata *rdata, 
		const AosXmlTagPtr &def)
{
	if (!smIndexMgrObj) getIndexMgr(rdata);
	aos_assert_rr(smIndexMgrObj, rdata, 0);
	return smIndexMgrObj->modifyEntry(rdata, def);
}

bool 
AosIndexMgrObj::removeEntryStatic(
		AosRundata *rdata, 
		const OmnString &table_name,
		const OmnString &field_name)
{
	if (!smIndexMgrObj) getIndexMgr(rdata);
	aos_assert_rr(smIndexMgrObj, rdata, 0);
	return smIndexMgrObj->removeEntry(rdata, table_name, field_name);
}


