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
// 01/01/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_ObjMgrObj_h
#define Aos_SEInterfaces_ObjMgrObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEUtil/Docid.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"

class AosObjMgrObj : virtual public OmnRCObject
{
private:
	static AosObjMgrObjPtr 	smObjMgr;

public:
	virtual AosXmlTagPtr	createPubNamespace(const AosRundataPtr &rdata) = 0;
	virtual AosXmlTagPtr createIdGenDoc(
						const OmnString &ctnr_objid,
						const bool is_public,
						const AosRundataPtr &rdata) = 0;
	virtual AosAccessRcdPtr createAccessRecord1(
						const AosRundataPtr &rdata,
						const OmnString &cloudid, 
						const u64 &docid) = 0;
	virtual AosXmlTagPtr createAccessRecord(
						const AosRundataPtr &rdata, 
						const AosXmlTagPtr &ref_doc) = 0;
	virtual AosXmlTagPtr createDftContainer(
						const AosRundataPtr &rdata,
						const AosXmlTagPtr &doc,
						const bool is_public,
						const OmnString &cid) = 0;

	// Chen Ding, 2013/11/14
	virtual AosXmlTagPtr createUserContainer(
						const AosRundataPtr &rdata,
						const OmnString &ctnr_objid,
						const OmnString &ctnr_name,
						const OmnString &cid) = 0;

	virtual bool createObjsAtStartup() = 0;
	virtual AosXmlTagPtr createLostFoundCtnr(const AosRundataPtr &rdata) = 0;
	inline static u64 getSuperUserDocid()
	{
		return AOSDOCID_ROOT;
	}

	inline static OmnString getSuperUserCid()
	{
		return AOSCLOUDID_SYSROOT;
	}

	static AosObjMgrObjPtr getObjMgr() {return smObjMgr;}
	static void setObjMgr(const AosObjMgrObjPtr &o) {smObjMgr = o;}
	static bool createObjsAtStartupStatic()
	{
		aos_assert_r(smObjMgr, false);
		return smObjMgr->createObjsAtStartup();
	}

	// Chen Ding, 2014/01/31
	virtual AosXmlTagPtr createSysUserCtnr(const AosRundataPtr &rdata) = 0;
};
#endif
